# Boolean-State — verdicts by USAGE (not by names)

**Date:** 2026-06-07
**Task:** #089 / #090
**Sample:** the 24 most promising candidates (non-vendor, 3+ state-like names, ≥50%, ≤12 bools) from a broad run of 790 repos + 5 control giants.
**Method:** agents read the real code and grep'd the usage of each bool field. The verdict is by usage SIGNALS (mutual exclusion `if/else-if`, group transition assignment, combinatorial gap), NOT by names.

## Bottom line

| Verdict | Count out of 29 |
|---|---|
| 🟢 clean state machine (almost all bools are phases) | **0** |
| 🟡 partial (there is a mutually exclusive subset) | **8** |
| 🔴 not a state machine (independent flags/config/bits) | **17** |
| ⚪ dead code / FP | **2** |

## Three conclusions that change the picture

### 1. Even among the BEST candidates by naming — 0 clean FSMs
We deliberately selected 24 structures that *by name* look maximally like state machines. By actual usage: not a single 🟢, only ~28% have an extractable state subset (🟡), most are independent flags.

### 2. The single flag of the v0.2 rule (`SimulationData`) is a FALSE POSITIVE
The rule with the naming heuristic flagged exactly `SimulationData` (EVerest) as a TP. Usage analysis: the real state machine there is **already in `enum SimState`**, and the 6 bools (`iso_pwr_ready`, `dc_power_on`, `v2g_finished`…) are independent event/intent signals that *drive* the transitions but are not themselves state (often consume-once). → The v0.2 precision in practice is **0/1**, not 100%.

### 3. Mature projects ALREADY hold state in an enum — and the bools next to it are orthogonal
A recurring pattern: the real FSM is extracted into a separate `enum`, and that is exactly why the remaining bools are NOT state:
- `TransformOperator` → `enum TransformState`; `vrrp_router` → `fsm.state`; `I2C_helper` → `i2c_internal_state_e`; `SimulationData` → `SimState`; `mosquitto` → `mosquitto_client_state`; `BufferMgrDynamic` → `profile_state_t`/`port_state_t`.

This explains the "0 clean 🟢" mechanically: as soon as a developer recognizes the state — they introduce an enum. Bool-soup-as-state in mature code degenerates into "a real FSM in an enum + orthogonal bools around it."

### What this means for the #089 verdict
The hypothesis "growth of boolean-state = a drift signal" has to be **softened from YES → closer to MAYBE**: the phenomenon exists, but (a) it's rarer and more orthogonal than assumed; (b) naming detection is nearly useless (its only "catch" is an FP); (c) even usage detection mostly yields *subsets* (🟡), not whole machines. The real value of the rule = finding 🟡 cores (group assignment), which requires a **semantic backend (#042)**. Details in the [proposal](boolean_state_drift_proposal.md).

---

## 🟡 Partial — there is an extractable mutually exclusive subset (8)

### MonocularInertialSlamNode (7 bool) — `eigendude_OASIS` ⭐ strongest
- **File:** [MonocularInertialSlamNode.h](file://~/oss_corpus/oss/eigendude_OASIS/oasis_perception_cpp/src/nodes/MonocularInertialSlamNode.h)
- **Signal:** Classic group transition assignment: entering recovery `recoveryPending=true; initRetryPending=false; stableInputPaused=false; startupArmed=false`; in retry/arming — similarly one true, the rest cleared; a full reset clears all 4. Combinatorial gap 16→a handful.
- **Proposed:** `enum class TrackingPhase { StartupUnarmed, Armed, StableInputPaused, PostStallRecovery, InitRetryBackoff }`. Keep `m_imageWorkerStop/Wake` (CV signals) and the diagnostic latch as bool.

### VisualScriptPropertySelector (12 bool) — `RebelToolbox_RebelEngine`
- **File:** [visual_script_property_selector.h](file://~/oss_corpus/oss/RebelToolbox_RebelEngine/modules/visual_script/visual_script_property_selector.h)
- **Signal:** The triple `{properties, seq_connect, visual_script_generic}` is always set as a GROUP in the `select_from_*` setters and read as a selector in `_update_search` (`if(properties||seq_connect)`, `if(seq_connect && !visual_script_generic)`). Of 2³, ~5 occur.
- **Proposed:** `enum class SelectorMode {…}` by the actual combinations; `connecting/virtuals_only` — bool.

### chiaki_stream_connection_t (5 bool) — `streetpea_chiaki-ng`
- **File:** [streamconnection.h](file://~/oss_corpus/oss/streetpea_chiaki-ng/lib/include/chiaki/streamconnection.h)
- **Signal:** `state_finished`/`state_failed` — a mutually exclusive outcome pair under a mutex, group-reset before each step. The linear progress meanwhile is in `int state`.
- **Proposed:** `enum class StepOutcome { Pending, Succeeded, Failed }`; `should_stop/remote_disconnected/feedback_sender_active` — bool.

### BufferMgrDynamic (8 bool) — `sonic-net_sonic-swss`
- **File:** [buffermgrdyn.h](file://~/oss_corpus/oss/sonic-net_sonic-swss/cfgmgr/buffermgrdyn.h)
- **Signal:** 4 init flags — monotonic progress `pending→poolReady→portInitDone→completed` (partial group rollback). The other 4 — independent capability. The profile/port already have their own enums.
- **Proposed:** `enum class InitPhase {…}` for progress; `m_support*` etc. — bool.

### WiFiManager (6 bool) — `FujiNetWIFI_fujinet-firmware`
- **File:** [fnWiFi.h](file://~/oss_corpus/oss/FujiNetWIFI_fujinet-firmware/lib/hardware/fnWiFi.h)
- **Signal:** The "connection phase" axis `_scan_in_progress/_trying_stored/_all_stored_failed` is resolved by `if/else if`. `_started/_connected/_disconnecting` are orthogonal.
- **Proposed:** `enum class ConnectPhase { idle, scanning, connecting_current, trying_stored, all_failed }`; the rest — bool.

### mt76x02_calibration (6 bool) — `openwrt_mt76`
- **File:** [mt76x02.h](file://~/oss_corpus/oss/openwrt_mt76/mt76x02.h)
- **Signal:** Mostly independent `*_done` calibration-progress latches (they accumulate). A light subset — `tssi_cal_done`+`tssi_comp_pending`.
- **Proposed:** mostly bool; optionally an `enum` for the TSSI stage. The benefit is small.

### MM_GCExtensionsBase (104 bool) — `eclipse-omr_omr` (giant)
- **File:** [GCExtensionsBase.hpp](file://~/oss_corpus/oss/eclipse-omr_omr/gc/base/GCExtensionsBase.hpp)
- **Signal:** A bag of -Xgc options, BUT inside there is a mutually exclusive policy selector `_isStandardGC/_isVLHGC/_isMetronomeGC/_isSegregatedHeap` (exactly one true per-config).
- **Proposed:** `enum GcPolicy` for the 4 fields (low priority: the API is intentionally compile-time via `#if`). The other ~100 — bool.

### DxcOpts (73 bool) — `microsoft_DirectXShaderCompiler` (giant)
- **File:** [HLSLOptions.h](file://~/oss_corpus/oss/microsoft_DirectXShaderCompiler/include/dxc/Support/HLSLOptions.h)
- **Signal:** A bag of CLI flags, but there are explicitly exclusive pairs with a runtime check "both set → error": `DefaultColMajor/DefaultRowMajor`, `AvoidFlowControl/PreferFlowControl`.
- **Proposed:** `enum {Default,Col,Row}` / `enum FlowControl` for the pairs; the mass of the rest — bool 1:1 with argv.

---

## 🔴 Not a state machine — independent flags/config/bits (17)

- **ipahal_reg_tx_wrapper** (11) — [ipahal_reg.h](file://~/oss_corpus/oss/LineageOS_android_kernel_asus_sm8350/techpack/dataipa/drivers/platform/msm/ipa/ipa_v3/ipahal/ipahal_reg.h) — a mirror of a hardware register, independent idle/empty bits of subblocks.
- **TestState** (12) — [test_state.h](file://~/oss_corpus/oss/aegis-aead_boringssl/ssl/test/test_state.h) — independent one-shot "callback-ready" test latches, freely combined.
- **coronAlign** (11) — [coronAlign.hpp](file://~/oss_corpus/oss/magao-x_MagAOX/gui/widgets/coronAlign/coronAlign.hpp) — N per-axis pending/refresh latches; the real state is in the string `m_*State` from the controller.
- **Schema_info** (10) — [dump_reader.h](file://~/oss_corpus/oss/mysql_mysql-shell/modules/util/load/dump_reader.h) — monotonic progress latches + `has_*` capability from JSON.
- **wrapper_ostream** (9) — [ostream-wrapper.h](file://~/oss_corpus/oss/dovecot_core/src/lib/ostream-wrapper.h) — a cumulative lifecycle + orthogonal reentrancy-guard (bitfield), coexisting.
- **TransformOperator** (8) — [TransformOperator.h](file://~/oss_corpus/oss/fernandotonon_QtMeshEditor/src/TransformOperator.h) — the FSM is already in `enum TransformState`; the bools are config + independent drag flags.
- **ShtUsermod** (8) — [ShtUsermod.h](file://~/oss_corpus/oss/wled_WLED/usermods/sht/ShtUsermod.h) — config + idempotent `*Done` latches of different subsystems.
- **I2C_helper** (8) — [i2c_helper.hpp](file://~/oss_corpus/oss/gvsoc_gvsoc-core/models/devices/i2c/helper/i2c_helper.hpp) — the FSM is already in `i2c_internal_state_e`; the bools are intent/pin-driver/guard (+2 dead).
- **ChargePointImpl** (6) — [charge_point_impl.hpp](file://~/oss_corpus/oss/EVerest_EVerest/lib/everest/ocpp/include/ocpp/v16/charge_point_impl.hpp) — independent flags + log-once latches (+1 dead).
- **SimulationData** (6) — [simulation_data.hpp](file://~/oss_corpus/oss/EVerest_EVerest/modules/EV/EvManager/main/simulation_data.hpp) — **the v0.2 rule's FP**: the FSM is already in `enum SimState`, the bools are independent intent signals (consume-once).
- **WiFiManager-adjacent / DATALAYER_SYSTEM_STATUS_TYPE** (6) — [datalayer.h](file://~/oss_corpus/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h) — permission bits from independent actors (battery1/2/3, inverter), `&&`-aggregation.
- **CustomAgent** (5) — [AgentRenderer.h](file://~/oss_corpus/oss/gwdevhub_GWToolboxpp/GWToolboxdll/Widgets/Minimap/AgentRenderer.h) — independent render override checkboxes.
- **MCPServer** (5) — [MCPServer.h](file://~/oss_corpus/oss/fernandotonon_QtMeshEditor/src/MCPServer.h) — flags of 4 different subsystems (stdio/MCP/Ogre/HTTP).
- **CameraInputState** (5) — [RCameraController.hpp](file://~/oss_corpus/oss/solvcon_modmesh/cpp/modmesh/pilot/RCameraController.hpp) — input signals that are specifically combined (chord gestures).
- **vrrp_router** (5) — [vrrp.h](file://~/oss_corpus/oss/FRRouting_frr/vrrpd/vrrp.h) — the FSM is already in `fsm.state`; the bools are is_active/is_owner + independent pending packets.
- **rc2014Fuji** (5) — [rc2014Fuji.h](file://~/oss_corpus/oss/FujiNetWIFI_fujinet-firmware/lib/device/rc2014/rc2014Fuji.h) — independent busy/scan/ssid/config markers.
- **TeslaBattery** (215) / **PQCSettings** (199) / **SettingsCache** (76) — [TESLA-BATTERY.h](file://~/oss_corpus/oss/dalathegreat_Battery-Emulator/Software/src/battery/TESLA-BATTERY.h), [pqc_settings.h](file://~/oss_corpus/oss/luspi_photoqt/cplusplus/header/pqc_settings.h), [cache_settings.h](file://~/oss_corpus/oss/Cockatrice_Cockatrice/cockatrice/src/client/settings/cache_settings.h) — CAN telemetry / a Qt property-bag / UI preferences. Pure data/config bags.

## ⚪ Dead code / FP (2)

- **DatabaseManager** (7) — [DatabaseManager.h](file://~/oss_corpus/oss/OpenMagnetics_MKF/src/support/DatabaseManager.h) — 7 `_*Loaded` fields are never read/written anywhere; `is*Loaded()` goes through `!container.empty()`. Remove.
- **macFuji** (6) — [macFuji.h](file://~/oss_corpus/oss/FujiNetWIFI_fujinet-firmware/lib/device/mac/macFuji.h) — an unfinished port, 4 of 6 are unused.

---

## What follows from this for the rule

1. **Naming detection is confirmed to be a dead end** — its only catch (`SimulationData`) turned out to be an FP. Names like `israw/hotspot/recoveryPending` either don't match the dictionary or match for non-states.
2. **The real signal is group transition assignment** (`a=true; b=false; c=false;` in one place) + mutually exclusive `if/else-if`. This is exactly what separated MonocularInertialSlamNode/VisualScriptPropertySelector/chiaki from the noise. It requires AST/dataflow → **#042**.
3. **A useful detector should also check whether an enum state is ALREADY present nearby** — otherwise we'll flag orthogonal bools on objects where the FSM is already correctly extracted (TransformOperator, vrrp, I2C, SimulationData).
4. A possible "cheap" signal without a full AST: look in `.cpp` for **group assignments** of ≥3 bools of the same struct in one block (`x.a=...; x.b=...; x.c=...;`) — this is a proxy for a state transition that doesn't depend on names.
