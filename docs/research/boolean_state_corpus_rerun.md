# Boolean-State Rule — re-run (improved rule)

**What the rule flags now:** 1 struct (was 59 with the "bare" 5+ bool counter).

**Filters applied:** depth-0 (fields only, not local variables), vendor exclusion, config-name exclusion (*Settings/*Config/*Cache/...), stop-fields has_*/use_*/can_*/*_enabled, state-ratio ≥ 60%.

| Struct | Bools | State | Ratio | File |
|---|---|---|---|---|
| SimulationData | 6 | 4 | 66% | [simulation_data.hpp](file://~/oss/EVerest_EVerest/modules/EV/EvManager/main/simulation_data.hpp) |

## Fields per item

- **SimulationData** (6 bool, 4 state-like, 66%) — [simulation_data.hpp](file://~/oss/EVerest_EVerest/modules/EV/EvManager/main/simulation_data.hpp)
  `v2g_finished, iso_stopped, iso_charger_paused, iso_pwr_ready, bcb_toggle_C, dc_power_on`
  → Real TP: phases of an EV charging-session simulation (`finished`/`stopped`/`paused`/`ready`) + 2 orthogonal toggles. The state subset is explicit.

---

## Before / After

| | Bare counter (5+ bool) | Improved rule |
|---|---|---|
| Candidates | 59 | **1** |
| Of those "keep bool" (noise) | ~46 (78%) | 0 |
| Extractor FP (local variables) | 2 (+8 ETL timers with junk) | 0 (depth-0) |
| Vendored libs | ≥7 | 0 (excluded) |
| Precision over flags | ~19% | 100% (1/1 — real TP) |

## What worked

1. **Depth-0** — all ETL timers (`bool result`/`success` inside method bodies) and `CodeGenerator`/`bit_stream` were filtered out: only direct fields are counted.
2. **Vendor exclusion** — `cgltf`, `Catch2`, `cpp-peglib`, `tclap`, `mosquitto`, `bitstream_state` (it lives in `third_party/`) dropped out of the sample.
3. **Config names** — `SettingsCache` (76!), `*Options`, `*ConfigData`, `*Arguments` were filtered out by the struct-name suffix.
4. **Stop-fields** — `has_*`/`use_*`/`can_*`/`*_enabled`/`*UpToDate*` no longer count as state, so capability bags (`cgltf_material`, `VMStructs`) don't reach the ratio.

## The price: recall

The rule became almost "mute" — 1 flag across 50 repos. Strong 🟡 cases from the [analysis](boolean_state_enum_analysis.md) **did not fire** for objective reasons:

- **bitstream_state** — in `third_party/` → correctly skipped (vendor, can't be fixed).
- **ImageBackingStore** (`m_israw/m_isrom/m_isfolder`), **VM** (`_hotspot/_openj9/_zing`), **db** (`transaction_started`), **BedrockCommand** (`_inDBRead/WriteOperation`) — their mutually-exclusive subset **is not named with lifecycle words** and/or makes up <60% of the fields. Naming doesn't catch them.

This is exactly the conclusion baked into the [proposal](boolean_state_drift_proposal.md): **the naming heuristic gives high precision but low recall; to catch state machines with "non-lifecycle" names (raw/rom/folder, hotspot/zing), you need semantic analysis of mutual exclusion (if/else-if + group assignment) — the semantic backend (#042).**

For CI this is an acceptable compromise for v0.2: the rule doesn't make noise (we avoid "5000 violations on first run"), it flags only obvious state machines. We recover recall in v0.3 via #042.
