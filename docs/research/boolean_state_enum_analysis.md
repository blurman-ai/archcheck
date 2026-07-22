# Boolean-State → enum: usage analysis across 59 candidates

**Date:** 2026-06-07
**Task:** #089 / #090
**Method:** 8 parallel agents read the real code of each candidate (struct definition + grep of field usage across the whole repository) and rendered a verdict against a single rubric.
**Candidate source:** [boolean_state_corpus_validation.md](boolean_state_corpus_validation.md) — the extractor selected every struct/class with 5+ bool fields across 50 OSS repos (filter — counter only, no naming heuristic).

## Verdict rubric

- 🟢 **to enum** — all/almost all bools are mutually exclusive lifecycle phases, exactly one true at any moment. Collapses entirely into `enum class State`.
- 🟡 **partial** — mostly independent flags, but there is a recognizable state-subset → extract it into an enum, leave the rest.
- 🔴 **keep bool** — independent toggles / `has_*`/`_can_*`/`_use_*` presence flags / config options / protocol bits. Converting to enum would be a mistake (combinations are orthogonal and legal).
- ⚪ **extractor FP** — these aren't struct fields at all (local `bool result`/`first` in methods, counted multiple times).

## Result (key point)

| Verdict | Count | Share |
|---------|-------:|-----:|
| 🟢 pure state machine | **0** | 0% |
| 🟡 partial (has a state-subset) | **11** | 19% |
| 🔴 keep bool | **46** | 78% |
| ⚪ extractor FP | **2** | 3% |
| **Total** | **59** | 100% |

### What this means for the `implicit_state_machine_growth` rule

**The "5+ bool" counter by itself is a weak signal: only 19% of candidates have an extractable state, and not one is a pure state machine.** The overwhelming majority are:

1. **Config bags / CLI options** (Oiiotool 42, SettingsCache 76, Catch2 ConfigData 15, daemon 15, Arguments 11) — orthogonal toggles, any combination is legal.
2. **`has_*`/`_can_*`/`_use_*` presence/capability flags** (cgltf_material 15, VMStructs 8, IvGL `m_use_*`) — presence of an optional feature/format section/JVM capability.
3. **Protocol bits** (EV102, Charger109 — CHAdeMO/IEEE 2030.1.1 status/error bits, several faults at once).
4. **Lazy-cache "up-to-date" flags** (vcs_VolPhase 8 — cache invalidation along independent axes).
5. **Vendored libraries** (cgltf, Catch2, cpp-peglib, tclap, mosquitto) — touching upstream is impossible in principle.

### Concrete rule edits (feedback into #090 / proposal)

1. **Exclude vendored paths** — `third_party/`, `vendor/`, `extern/`, single-header drops (`catch.hpp`, `cgltf.h`, `peglib.h`, `tclap/`). At least 7 vendored structs landed in the sample — pure noise.
2. **Push harder on config naming** — `*Settings`/`*Config`/`*Options`/`*Cache`/`*Arguments`/`*Preferences`/`*ConfigData`. `SettingsCache` (76 bools!) and `MessagePreferences` (11) are obvious configs that slipped past the struct-name filter.
3. **Stop-prefixes for fields** — `has_*`, `_can_*`, `_use_*`, `m_use_*`, `enable_*`, `dev_*`, `*_provided`, `*_ok`, `*UpToDate*` → strong "keep bool" signal. Add to config-pattern exclusions.
4. **The real signal is semantics, not naming:** mutual exclusion in `if … else if …` chains + group assignment "one true, rest false". This is visible only via AST/usage analysis → confirms the direction **v0.3 + semantic backend (#042)**. Regex naming yields at most ~19% usefulness on this sample.
5. **Filter out extractor FPs** — ignore `bool` in method bodies (local variables), count only class fields. `bit_stream`, `CodeGenerator` are pure FPs (local `result`/`first`). ETL timers — 8 structs where out of "5-6 bool" only `enabled` + `repeating` are real.

---

## 🟡 Partial — has an extractable state-subset (11)

> The most valuable findings: here, moving part of the fields into `enum class` really would remove impossible states.

### bitstream_state (5 bools) — `AetherSDR` ⭐ strongest case
- **File:** [bitstream.h](file://~/oss_corpus/oss/AetherSDR/third_party/libmodem_core/bitstream.h)
- **Fields:** `searching`, `in_preamble`, `in_frame`, `complete` (phases) + `aborted`, `enable_diagnostics`
- **How used:** The decoder treats 4 fields strictly as mutually exclusive phases via `if(searching) … else if(in_preamble) … else if(in_frame) …`; on each transition the group is set at once (`searching=false; in_preamble=true`). `reset()` returns to `searching=true`.
- **Proposed:** `enum class DecodeState { Searching, InPreamble, InFrame, Complete }` — the group assignments disappear, and the invariant "exactly one phase is true" becomes type-safe. Keep `aborted`/`enable_diagnostics` as bool. *(NB: repo `third_party` — vendor, the edit is conceptual.)*

### IvGL (12 bools) — `AcademySoftwareFoundation_OpenImageIO`
- **File:** [ivgl.h](file://~/oss_corpus/oss/AcademySoftwareFoundation_OpenImageIO/src/iv/ivgl.h)
- **Fields:** `m_dragging`, `m_selecting` (gesture) + `m_use_*` (GL capabilities), `m_*_created` (latches)
- **How used:** In `mousePressEvent` we enter either drag or select (else branches), on release both reset — de facto mutually exclusive gesture phases. `m_use_*` are orthogonal hardware capability flags.
- **Proposed:** `enum class MouseGesture { None, Dragging, Selecting }`. Keep the rest as bool.

### ReactorNet (5 bools) — `Cantera_cantera`
- **File:** [ReactorNet.h](file://~/oss_corpus/oss/Cantera_cantera/include/cantera/zeroD/ReactorNet.h)
- **Fields:** `m_integratorInitialized` + `m_needIntegratorInit` (pair) + `m_verbose`, `m_atolUserSpecified`, `m_timeIsIndependent`
- **How used:** The pair encodes the integrator lifecycle: "not initialized" → "initialized, clean" → "initialized, needs reinit" (initialize()/reinitialize()).
- **Proposed:** `enum class IntegratorState { Uninitialized, Initialized, NeedsReinit }`. The other four — bool.

### TransportPropertyData (6 bools) — `CoolProp_CoolProp`
- **File:** [CoolPropFluid.h](file://~/oss_corpus/oss/CoolProp_CoolProp/include/CoolProp/CoolPropFluid.h)
- **Fields:** `viscosity_using_ECS`, `viscosity_using_Chung`, `viscosity_using_rhosr` (method) + `*_model_provided`, `conductivity_using_ECS`
- **How used:** The three viscosity_using_* are checked in a chain `if(...) {…; return;}` — selection of one mutually exclusive method, with an existing enum `hardcoded_viscosity` nearby.
- **Proposed:** `enum class ViscosityModel { None, ECS, Chung, RhoSr, Hardcoded }`. `*_model_provided` — bool.

### VM (6 bools) — `DataDog_java-profiler`
- **File:** [vmEntry.h](file://~/oss_corpus/oss/DataDog_java-profiler/ddprof-lib/src/main/cpp/vmEntry.h)
- **Fields:** `_hotspot`, `_openj9`, `_zing` (JVM vendor, mutually exclusive by construction) + `_can_sample_objects`, `_can_intercept_binding`, `_is_adaptive_gc_boundary_flag_set`
- **How used:** `_zing = !_hotspot && …`, `_openj9 = !_hotspot && …` — this is "which JVM", exactly one true.
- **Proposed:** `enum class JvmVendor { Unknown, Hotspot, OpenJ9, Zing }`. Keep `_can_*` as bool.

### ImageBackingStore (5 bools) — `BlueSCSI_BlueSCSI-v2`
- **File:** [ImageBackingStore.h](file://~/oss_corpus/oss/BlueSCSI_BlueSCSI-v2/src/ImageBackingStore.h)
- **Fields:** `m_israw`, `m_isrom`, `m_isfolder` (backend type) + `m_iscontiguous`, `m_isreadonly_attr`
- **How used:** In the constructor, exactly one is set by prefix (`RAW:`/`ROM:`/directory/file) — a mutually exclusive storage kind.
- **Proposed:** `enum class Backend { File, Raw, Rom, Folder }`. `m_iscontiguous` (dynamic optimization toggle) and `m_isreadonly_attr` (permissions) — bool.

### BedrockCommand (5 bools) — `Expensify_Bedrock`
- **File:** [BedrockCommand.h](file://~/oss_corpus/oss/Expensify_Bedrock/BedrockCommand.h)
- **Fields:** `_inDBReadOperation`, `_inDBWriteOperation` (mutually exclusive) + `repeek`, `escalateImmediately`, `_commitEmptyTransactions`
- **How used:** Set/cleared in pairs around different phases (peek vs process), never true at the same time.
- **Proposed:** `enum class DBOp { None, Read, Write }` (there is already an `enum class STAGE` nearby). The rest — bool.

### db (5 bools) — `ElementsProject_lightning`
- **File:** [common.h](file://~/oss_corpus/oss/ElementsProject_lightning/db/common.h)
- **Fields:** `transaction_started`, `dirty` (transaction phases) + `developer`, `readonly`, `in_migration`
- **How used:** `db_begin`→`db_need_transaction`(lazy)→`db_commit` branches on `transaction_started`, then on `dirty` — a progression of transaction phases.
- **Proposed:** `enum class TxState { None, Open, Started }` for `transaction_started`. `dirty` can stay a separate flag; `developer`/`readonly`/`in_migration` — bool.

### MessageParseArgs (8 bools) — `Chatterino_chatterino2`
- **File:** [MessageBuilder.hpp](file://~/oss_corpus/oss/Chatterino_chatterino2/src/messages/MessageBuilder.hpp)
- **Fields:** `isReceivedWhisper`, `isSentWhisper` (mutually exclusive) + 6 independent parsing flags
- **How used:** The pair is checked in a single if/else-if; a message is never both an incoming and outgoing whisper at once.
- **Proposed:** `enum class WhisperKind { None, Received, Sent }`. Low priority (cosmetic on a POD argument struct).

### ObjectSampler (5 bools) — `DataDog_java-profiler`
- **File:** [objectSampler.h](file://~/oss_corpus/oss/DataDog_java-profiler/ddprof-lib/src/main/cpp/objectSampler.h)
- **Fields:** `_active` (start/stop lifecycle) + `_record_allocations`, `_record_liveness`, `_gc_generations`, `_disable_rate_limiting` (config)
- **How used:** `start()` atomically sets `_active=true`, `stop()`=false, the callback gates on `_active`. The rest are config from Arguments.
- **Proposed:** Optionally `enum class SamplerState { Stopped, Active }` for `_active` (but that's only 2 states, bool/atomic is fine too). 4 config flags — bool.

### Interpreter (5 bools) — `Emute-Lab-Instruments_uSEQ`
- **File:** [interpreter.h](file://~/oss_corpus/oss/Emute-Lab-Instruments_uSEQ/uSEQ/src/lisp/interpreter.h)
- **Fields:** `m_manual_evaluation`, `m_update_loop_evaluation` (conceptually mutually exclusive modes) + `m_attempt_expr_eval_first`, `m_eval_expr_if_def_not_found`, `m_builtindefs_init`
- **How used:** The pair toggles around manual input vs loop update, BUT both fields are almost never read (write-only), and `m_builtindefs_init` is a dead guard.
- **Proposed:** Conceptually `enum class EvalMode { Idle, Manual, UpdateLoop }`, but **deal with the dead code first**. `m_attempt_*`/`m_eval_*` — independent eval toggles.

---

## 🔴 Keep bool — config / capability / protocol / vendor (46)

> Converting to enum here would be a mistake: combinations are orthogonal and legal simultaneously.

### Config bags and CLI options
- **Oiiotool** (42) — [oiiotool.h](file://~/oss_corpus/oss/AcademySoftwareFoundation_OpenImageIO/src/oiiotool/oiiotool.h) — CLI toggles `ap.arg(...)`, legally combined (`-v -n --no-clobber`).
- **SettingsCache** (76) — [cache_settings.h](file://~/oss_corpus/oss/Cockatrice_Cockatrice/cockatrice/src/client/settings/cache_settings.h) — QSettings singleton, each bool has its own get/set pair + Qt signal. Config, not FSM.
- **Catch2 ConfigData** (15) — [catch_config.hpp](file://~/oss_corpus/oss/Catch2/src/catch2/catch_config.hpp) — **vendor**; `list*` confirmed not mutually exclusive (additive `if` in catch_list.cpp).
- **uSEQ ConfigData** (12) — [catch.hpp](file://~/oss_corpus/oss/Emute-Lab-Instruments_uSEQ/test/catch.hpp) — **vendor** (same Catch2).
- **daemon** (15) — [connectd.h](file://~/oss_corpus/oss/ElementsProject_lightning/connectd/connectd.h) — a bag of `dev_*`/config toggles; the connection lifecycle is already in a separate enum (`draining_state`).
- **Arguments** (11) — [arguments.h](file://~/oss_corpus/oss/DataDog_java-profiler/ddprof-lib/src/main/cpp/arguments.h) — agent options, checked in combinations `_record_allocations || _record_liveness`.
- **MessagePreferences** (11) — [MessageLayoutContext.hpp](file://~/oss_corpus/oss/Chatterino_chatterino2/src/messages/layouts/MessageLayoutContext.hpp) — a snapshot of `enable*` checkboxes, several highlights at once.
- **print_info_options** (10) — [imageio_pvt.h](file://~/oss_corpus/oss/AcademySoftwareFoundation_OpenImageIO/src/include/imageio_pvt.h) — options bag, additive `if`.
- **GLOBAL** (7) — [Uhr.h](file://~/oss_corpus/oss/ESPWortuhr_Multilayout-ESP-Wordclock/include/Uhr.h) — firmware EEPROM config; changing the format would break storage.
- **GCodeChecker** (5) — [GCodeChecker.h](file://~/oss_corpus/oss/BambuStudio/bbs_test_tools/bbs_gcode_checker/GCodeChecker.h) — G-code feature parser flags, often true together.
- **TextureSystemImpl** (5) — [texture_pvt.h](file://~/oss_corpus/oss/AcademySoftwareFoundation_OpenImageIO/src/libtexture/texture_pvt.h) — runtime attributes via `attribute()`.
- **BedrockTester** (5) — [BedrockTester.h](file://~/oss_corpus/oss/Expensify_Bedrock/test/lib/BedrockTester.h) — test-harness CLI toggles.

### GUI settings (gnuplot)
- **QtGnuplotWidget** (7) — [QtGnuplotWidget.h](file://~/oss_corpus/oss/AlexanderTaeschner_gnuplot/src/qtterminal/QtGnuplotWidget.h) — Q_PROPERTY + QSettings, orthogonal.
- **QtGnuplotWindow** (5) — [QtGnuplotWindow.h](file://~/oss_corpus/oss/AlexanderTaeschner_gnuplot/src/qtterminal/QtGnuplotWindow.h) — mirror of the widget settings.
- **wxtConfigDialog** (5) — [wxt_gui.h](file://~/oss_corpus/oss/AlexanderTaeschner_gnuplot/src/wxterminal/wxt_gui.h) — dialog checkboxes; multi-valued options are already `int`.

### `has_*`/`_can_*`/`_use_*` presence and capability flags
- **cgltf_material** (15) — [cgltf.h](file://~/oss_corpus/oss/BlazingRenderer_BRender/core/fmt/cgltf.h) — **vendor** (cgltf); `has_*` glTF sections/extensions, orthogonal.
- **cgltf_node** (5) — [cgltf.h](file://~/oss_corpus/oss/BlazingRenderer_BRender/core/fmt/cgltf.h) — **vendor**; `has_translation/rotation/scale/matrix` (a partial TRS is allowed).
- **VMStructs** (8) — [vmStructs.h](file://~/oss_corpus/oss/DataDog_java-profiler/ddprof-lib/src/main/cpp/hotspot/vmStructs.h) — `_has_*`/`_can_*` JVM capability, detection cache.
- **AddElementNotificationClientCommand** (6) — [NotificationCommands.hpp](file://~/oss_corpus/oss/ENZYME-APD_tapir-archicad-automation/archicad-addon/Sources/NotificationCommands.hpp) — subscription bitmask (3 axes × 2 levels).
- **IteratorBase** (6) — [imagebuf.h](file://~/oss_corpus/oss/AcademySoftwareFoundation_OpenImageIO/src/include/OpenImageIO/imagebuf.h) — `m_valid`/`m_exists` orthogonal predicates (all 4 combinations meaningful), hot loop.
- **api_connector** (5) — [api_connector.hpp](file://~/oss_corpus/oss/EVerest_EVerest/applications/pionix_chargebridge/include/charge_bridge/everest_api/api_connector.hpp) — three `*_enabled` sub-APIs (feature bitmask).

### Lazy-cache / dirty / validity flags
- **vcs_VolPhase** (8) — [vcs_VolPhase.h](file://~/oss_corpus/oss/Cantera_cantera/include/cantera/equil/vcs_VolPhase.h) — 6 `m_UpToDate*` (invalidation along independent axes) + 2 property flags.
- **InterfaceKinetics** (6) — [InterfaceKinetics.h](file://~/oss_corpus/oss/Cantera_cantera/include/cantera/kinetics/InterfaceKinetics.h) — cache (`m_ROP_ok`) + `m_has_*` + skip options; pairs `if(!m_jac_skip_X && m_has_X)`.
- **value_information** (13) — [rapidjson.h](file://~/oss_corpus/oss/CoolProp_CoolProp/include/CoolProp/detail/rapidjson.h) — mirror of JSON type predicates (overlapping: istrue+isbool); plus dead code.
- **GraphBuilder** (6) — [graph_builder.h](file://~/oss_corpus/oss/Emute-Lab-Instruments_uSEQ/uSEQ/src/signal_engine/graph_builder.h) — sticky-error + 2 static init-guards + a mode flag.

### Config properties / modes (not mutually exclusive)
- **Reaction** (7) — [Reaction.h](file://~/oss_corpus/oss/Cantera_cantera/include/cantera/kinetics/Reaction.h) — reaction properties from YAML + serialization flags.
- **Reactor** (6) — [Reactor.h](file://~/oss_corpus/oss/Cantera_cantera/include/cantera/zeroD/Reactor.h) — 2 enable + 4 Jacobian skip options (64 legal combinations).
- **ResidualHelmholtzGeneralizedExponential** (7) — [Helmholtz.h](file://~/oss_corpus/oss/CoolProp_CoolProp/include/CoolProp/fluids/Helmholtz.h) — `*_in_u` "which terms are present" (subsets), not one state.
- **ProfiledThread** (5) — [thread.h](file://~/oss_corpus/oss/DataDog_java-profiler/ddprof-lib/src/main/cpp/thread.h) — 3 flags from different subsystems; the thread type is ALREADY extracted into `enum ThreadType` (confirms the rubric).
- **SQLite** (10) — [SQLite.h](file://~/oss_corpus/oss/Expensify_Bedrock/sqlitecluster/SQLite.h) — orthogonal latches/modes of a concurrent object, true in arbitrary combinations.
- **image_config_t** (7) — [BlueSCSI_disk.h](file://~/oss_corpus/oss/BlueSCSI_BlueSCSI-v2/src/BlueSCSI_disk.h) — a mix of config options, latches and one-shot guards on a SCSI target.
- **NoteControlBar** (6) — [NoteControlBar.h](file://~/oss_corpus/oss/203-Systems_MatrixOS/Applications/Note/NoteControlBar.h) — the lifecycle mode is already in `enum NoteControlBarMode`; the remainder are per-key latches.
- **Context** (5) — [peglib.h](file://~/oss_corpus/oss/Cockatrice_Cockatrice/libcockatrice_utility/libcockatrice/utility/peglib.h) — **vendor** (cpp-peglib); re-entrancy guard + const config.
- **Definition** (13) — [peglib.h](file://~/oss_corpus/oss/Cockatrice_Cockatrice/libcockatrice_utility/libcockatrice/utility/peglib.h) — **vendor**; orthogonal grammar-rule properties.
- **Arg** (6) — [Arg.h](file://~/oss_corpus/oss/BelledonneCommunications_flexisip/src/tclap/Arg.h) — **vendor** (TCLAP); 4 specification properties + 2 runtime flags.

### Protocol / status bits
- **EV102** (10) — [messages.hpp](file://~/oss_corpus/oss/EVerest_EVerest/lib/everest/ieee2030_1_1/include/ieee2030/common/messages/messages.hpp) — CHAdeMO fault/status bits (`fault & (1<<n)`), several at once. A candidate for `std::bitset`, not enum.
- **Charger109** (6) — [messages.hpp](file://~/oss_corpus/oss/EVerest_EVerest/lib/everest/ieee2030_1_1/include/ieee2030/common/messages/messages.hpp) — symmetrically, station status bits.
- **mosquitto** (16) — [mosquitto_internal.h](file://~/oss_corpus/oss/AetherSDR/third_party/mosquitto/src/mosquitto_internal.h) — **vendor**; the lifecycle is already in `enum mosquitto_client_state`, the remainder are options/working flags.

### ETL timers (only `enabled` + `repeating` are real, the rest are local FPs)
- **icallback_timer_atomic** (6) — [callback_timer_atomic.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/callback_timer_atomic.h)
- **icallback_timer_locked** (6) — [callback_timer_locked.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/callback_timer_locked.h)
- **icallback_timer_interrupt** (5) — [callback_timer_interrupt.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/callback_timer_interrupt.h)
- **icallback_timer** (5) — [callback_timer.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/callback_timer.h)
- **imessage_timer_interrupt** (5) — [message_timer_interrupt.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/message_timer_interrupt.h)
- **imessage_timer_atomic** (5) — [message_timer_atomic.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/message_timer_atomic.h)
- **imessage_timer_locked** (5) — [message_timer_locked.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/message_timer_locked.h)
- **format_spec_t** (5) — [format.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/format.h) — independent format flags (`#`,`0`,`L`); the mutually exclusive align/sign are ALREADY in separate enums.

---

## ⚪ Extractor FP — not struct fields (2)

- **bit_stream** (7) — [bit_stream.h](file://~/oss_corpus/oss/ETLCPP_etl/include/etl/bit_stream.h) — local `bool success`/`bool result` in inline methods were counted. The struct has no bool fields.
- **CodeGenerator** (5) — [code_generate.h](file://~/oss_corpus/oss/BoleynSu-Org_monorepo/legacy/BSL-AlgorithmW/src/code_generate.h) — the name `first` (a local "first-in-list" separator) was counted from 5 different scopes. There are no real bool fields.

> Additionally: 7 of the 8 ETL timers are formally 🔴, but in each of the "5-6 bool" only 2 fields are real (`enabled` + nested `repeating`), the rest are the same local FPs. So the actual extractor FP noise is even higher than the 2 rows above.

---

## Connection to hypothesis #089

This refines the verdict of research #089 (**boolean-state growth = drift signal? YES, with caveats**):

- The **YES** part is confirmed: real extractable states exist (bitstream FSM, ImageBackingStore Backend, ReactorNet integrator, db transaction) — growth of such bools is genuine drift.
- The **caveats grow stronger**: the "bare" 5+ bool counter yields ~78% "keep bool". The rule's usefulness depends entirely on (a) excluding vendor/configs/`has_*`, (b) semantic analysis of mutual exclusion — i.e. on the **semantic backend (#042)**, as predicted in [boolean_state_drift_proposal.md](boolean_state_drift_proposal.md).
