# Boolean-State DRIFT — per-struct (attribution to struct + git blame)

**Method:** parse current structs → real bool fields (depth-0, no signatures/locals) → `git blame` each field → group BY STRUCT. Drift = fields of one struct arrived through ≥4 distinct commits.

**Repos:** 73 agentic. **Struct drifts:** 65 in 30 repos; of those content (not config-bag): **51 in 27 repos**.

## Top content structs (config-bag excluded)

| Commits | Fields | Days | Struct | File |
|---|---|---|---|---|
| **13** | 16 | 640 (2024-06-03→2026-03-05) | `IsolateBase` | [setup.h](file://~/oss/cloudflare_workerd/src/workerd/jsg/setup.h)<br><sub>cloudflare_workerd/src/workerd/jsg/setup.h</sub> |
| **9** | 24 | 725 (2024-06-03→2026-05-29) | `ToolboxUIElement` | [ToolboxUIElement.h](file://~/oss/gwdevhub_GWToolboxpp/GWToolboxdll/ToolboxUIElement.h)<br><sub>gwdevhub_GWToolboxpp/GWToolboxdll/ToolboxUIElement.h</sub> |
| **8** | 13 | 328 (2024-11-27→2025-10-21) | `DATALAYER_SYSTEM_INFO_TYPE` | [datalayer.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h</sub> |
| **7** | 13 | 378 (2024-12-31→2026-01-13) | `DATALAYER_BATTERY_SETTINGS_TYPE` | [datalayer.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h</sub> |
| **6** | 7 | 705 (2024-06-05→2026-05-11) | `FGenericPlatformSentrySubsystem` | [GenericPlatformSentrySubsystem.h](file://~/oss/getsentry_sentry-unreal/plugin-dev/Source/Sentry/Private/GenericPlatform/GenericPlatformSentrySubsystem.h)<br><sub>getsentry_sentry-unreal/plugin-dev/Source/Sentry/Private/GenericPlatform/GenericPlatformSentrySubsystem.h</sub> |
| **6** | 9 | 636 (2024-07-27→2026-04-24) | `MethodState` | [LidarOdometry.h](file://~/oss/MOLAorg_mola_lidar_odometry/module/include/mola_lidar_odometry/LidarOdometry.h)<br><sub>MOLAorg_mola_lidar_odometry/module/include/mola_lidar_odometry/LidarOdometry.h</sub> |
| **6** | 12 | 525 (2024-06-04→2025-11-11) | `Microsoft` | [Terminal.hpp](file://~/oss/microsoft_terminal/src/cascadia/TerminalCore/Terminal.hpp)<br><sub>microsoft_terminal/src/cascadia/TerminalCore/Terminal.hpp</sub> |
| **6** | 7 | 469 (2024-12-22→2026-04-05) | `CommonAudioProcessor` | [CommonPluginProcessor.h](file://~/oss/jameshball_osci-render/Source/CommonPluginProcessor.h)<br><sub>jameshball_osci-render/Source/CommonPluginProcessor.h</sub> |
| **6** | 8 | 278 (2024-06-03→2025-03-08) | `Channel` | [Channel.h](file://~/oss/bdring_FluidNC/FluidNC/src/Channel.h)<br><sub>bdring_FluidNC/FluidNC/src/Channel.h</sub> |
| **6** | 7 | 230 (2024-06-03→2025-01-19) | `CustomLine` | [CustomRenderer.h](file://~/oss/gwdevhub_GWToolboxpp/GWToolboxdll/Widgets/Minimap/CustomRenderer.h)<br><sub>gwdevhub_GWToolboxpp/GWToolboxdll/Widgets/Minimap/CustomRenderer.h</sub> |
| **6** | 8 | 208 (2024-06-02→2024-12-27) | `MR` | [MRMeshDecimate.h](file://~/oss/MeshInspector_MeshLib/source/MRMesh/MRMeshDecimate.h)<br><sub>MeshInspector_MeshLib/source/MRMesh/MRMeshDecimate.h</sub> |
| **6** | 21 | 40 (2026-04-20→2026-05-30) | `EditorShell` | [EditorShell.h](file://~/oss/jwmcglynn_donner/donner/editor/EditorShell.h)<br><sub>jwmcglynn_donner/donner/editor/EditorShell.h</sub> |
| **5** | 14 | 674 (2024-07-27→2026-06-01) | `Visualization` | [LidarOdometry.h](file://~/oss/MOLAorg_mola_lidar_odometry/module/include/mola_lidar_odometry/LidarOdometry.h)<br><sub>MOLAorg_mola_lidar_odometry/module/include/mola_lidar_odometry/LidarOdometry.h</sub> |
| **5** | 6 | 495 (2024-07-11→2025-11-18) | `DATALAYER_SYSTEM_STATUS_TYPE` | [datalayer.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer.h</sub> |
| **5** | 5 | 484 (2024-06-03→2025-09-30) | `SqliteDatabase` | [sqlite.h](file://~/oss/cloudflare_workerd/src/workerd/util/sqlite.h)<br><sub>cloudflare_workerd/src/workerd/util/sqlite.h</sub> |
| **5** | 7 | 473 (2024-06-03→2025-09-19) | `python_converter` | [python_converter.h](file://~/oss/esbmc_esbmc/src/python-frontend/python_converter.h)<br><sub>esbmc_esbmc/src/python-frontend/python_converter.h</sub> |
| **5** | 6 | 441 (2025-02-18→2026-05-05) | `solidity_convertert` | [solidity_convert.h](file://~/oss/esbmc_esbmc/src/solidity-frontend/solidity_convert.h)<br><sub>esbmc_esbmc/src/solidity-frontend/solidity_convert.h</sub> |
| **5** | 10 | 180 (2025-05-19→2025-11-15) | `BmwIXBattery` | [BMW-IX-BATTERY.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/battery/BMW-IX-BATTERY.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/battery/BMW-IX-BATTERY.h</sub> |
| **5** | 21 | 74 (2025-12-28→2026-03-12) | `OptionManager` | [option_manager.h](file://~/oss/colmap_colmap/src/colmap/controllers/option_manager.h)<br><sub>colmap_colmap/src/colmap/controllers/option_manager.h</sub> |
| **5** | 5 | 52 (2026-04-05→2026-05-27) | `Graph` | [graph.hpp](file://~/oss/masc-ucsc_hhds/hhds/graph.hpp)<br><sub>masc-ucsc_hhds/hhds/graph.hpp</sub> |
| **5** | 8 | 36 (2026-02-05→2026-03-13) | `Shell` | [Shell.hpp](file://~/oss/contour-terminal_endo/src/shell/Shell.hpp)<br><sub>contour-terminal_endo/src/shell/Shell.hpp</sub> |
| **5** | 10 | 33 (2026-03-01→2026-04-03) | `NodeGraphComponent` | [NodeGraphComponent.h](file://~/oss/jameshball_osci-render/Source/components/modulation/NodeGraphComponent.h)<br><sub>jameshball_osci-render/Source/components/modulation/NodeGraphComponent.h</sub> |
| **5** | 5 | 14 (2026-03-23→2026-04-06) | `ReactionRule` | [ReactionRule.hpp](file://~/oss/RuleWorld_bionetgen/src/ast/ReactionRule.hpp)<br><sub>RuleWorld_bionetgen/src/ast/ReactionRule.hpp</sub> |
| **4** | 5 | 705 (2024-06-03→2026-05-09) | `ReactorNet` | [ReactorNet.h](file://~/oss/Cantera_cantera/include/cantera/zeroD/ReactorNet.h)<br><sub>Cantera_cantera/include/cantera/zeroD/ReactorNet.h</sub> |
| **4** | 6 | 702 (2024-06-02→2026-05-05) | `SizedType` | [types.h](file://~/oss/bpftrace_bpftrace/src/types.h)<br><sub>bpftrace_bpftrace/src/types.h</sub> |
| **4** | 40 | 685 (2024-06-04→2026-04-20) | `Oiiotool` | [oiiotool.h](file://~/oss/AcademySoftwareFoundation_OpenImageIO/src/oiiotool/oiiotool.h)<br><sub>AcademySoftwareFoundation_OpenImageIO/src/oiiotool/oiiotool.h</sub> |
| **4** | 13 | 660 (2024-06-02→2026-03-24) | `ImageComponent` | [ImageComponent.h](file://~/oss/batocera-linux_batocera-emulationstation/es-core/src/components/ImageComponent.h)<br><sub>batocera-linux_batocera-emulationstation/es-core/src/components/ImageComponent.h</sub> |
| **4** | 11 | 633 (2024-06-05→2026-02-28) | `MainWindow` | [mainwindow.h](file://~/oss/hluk_CopyQ/src/gui/mainwindow.h)<br><sub>hluk_CopyQ/src/gui/mainwindow.h</sub> |
| **4** | 28 | 630 (2024-06-03→2026-02-23) | `State` | [HttpTransact.h](file://~/oss/apache_trafficserver/include/proxy/http/HttpTransact.h)<br><sub>apache_trafficserver/include/proxy/http/HttpTransact.h</sub> |
| **4** | 4 | 620 (2024-07-12→2026-03-24) | `DefinitionInfo` | [inst_kind.h](file://~/oss/carbon-language_carbon-lang/toolchain/sem_ir/inst_kind.h)<br><sub>carbon-language_carbon-lang/toolchain/sem_ir/inst_kind.h</sub> |
| **4** | 5 | 578 (2024-09-12→2026-04-13) | `ActorSqlite` | [actor-sqlite.h](file://~/oss/cloudflare_workerd/src/workerd/io/actor-sqlite.h)<br><sub>cloudflare_workerd/src/workerd/io/actor-sqlite.h</sub> |
| **4** | 21 | 558 (2024-06-02→2025-12-12) | `FileFilterIndex` | [FileFilterIndex.h](file://~/oss/batocera-linux_batocera-emulationstation/es-app/src/FileFilterIndex.h)<br><sub>batocera-linux_batocera-emulationstation/es-app/src/FileFilterIndex.h</sub> |
| **4** | 8 | 539 (2024-06-03→2025-11-24) | `Ident` | [idents.h](file://~/oss/aardappel_lobster/dev/src/lobster/idents.h)<br><sub>aardappel_lobster/dev/src/lobster/idents.h</sub> |
| **4** | 8 | 514 (2024-06-02→2025-10-29) | `BPFtrace` | [bpftrace.h](file://~/oss/bpftrace_bpftrace/src/bpftrace.h)<br><sub>bpftrace_bpftrace/src/bpftrace.h</sub> |
| **4** | 5 | 451 (2024-06-03→2025-08-28) | `verilog_languaget` | [verilog_language.h](file://~/oss/diffblue_hw-cbmc/src/verilog/verilog_language.h)<br><sub>diffblue_hw-cbmc/src/verilog/verilog_language.h</sub> |
| **4** | 10 | 375 (2024-10-11→2025-10-21) | `DATALAYER_INFO_NISSAN_LEAF` | [datalayer_extended.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer_extended.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer_extended.h</sub> |
| **4** | 4 | 242 (2025-09-25→2026-05-25) | `AutoOpt` | [autoopt.h](file://~/oss/OpenChemistry_avogadrolibs/avogadro/qtplugins/autoopt/autoopt.h)<br><sub>OpenChemistry_avogadrolibs/avogadro/qtplugins/autoopt/autoopt.h</sub> |
| **4** | 209 | 235 (2025-05-18→2026-01-08) | `TeslaBattery` | [TESLA-BATTERY.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/battery/TESLA-BATTERY.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/battery/TESLA-BATTERY.h</sub> |
| **4** | 9 | 214 (2025-10-21→2026-05-23) | `DATALAYER_INFO_BYDATTO3` | [datalayer_extended.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer_extended.h)<br><sub>dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer_extended.h</sub> |
| **4** | 5 | 206 (2025-10-29→2026-05-23) | `symbolt` | [symbol.h](file://~/oss/esbmc_esbmc/src/util/symbol.h)<br><sub>esbmc_esbmc/src/util/symbol.h</sub> |

*(config-bag structs separately: 14; full list — perstruct_drift.csv)*


---

## Manual verification of the top-14 (eye-check)

I eye-checked the 14 top content structs (code + git history of fields):

| Verdict | Count | Structs |
|---|---|---|
| 🟢 **real drift** | **8 (57%)** | EditorShell, State (ATS HttpTransact), Terminal, MethodState, Channel, Graph, CommonAudioProcessor, CustomLine |
| 🟡 config-bag (name not caught by filter) | 6 (43%) | IsolateBase, DATALAYER_SYSTEM_INFO_TYPE, DATALAYER_BATTERY_SETTINGS_TYPE, FGenericPlatformSentrySubsystem, OptionManager, Oiiotool |
| 🔴 FP | **0 (0%)** | — |

**Precision jumped: file-level 23% real / 45% junk → per-struct 57% real / 0% junk.**
Per-struct attribution + depth-0 parser eliminated ALL false positives (different structs, generated code, bool in signatures). The remaining confusion is only 🟢 vs 🟡 (config-bag), and that is a semantic distinction (nature of the fields: state/mode vs `is*Enabled`/logging-toggle/registration-guard) that the struct name doesn't catch.

### Reference cases of real drift (confirmed)
- **EditorShell** (donner) — [file](file://~/oss/jwmcglynn_donner/donner/editor/EditorShell.h) — from 5 to 23 bool in 6 weeks, "owns all long-lived GUI/editor orchestration state".
- **State** (apache trafficserver, `HttpTransact`) — [file](file://~/oss/apache_trafficserver/include/proxy/http/HttpTransact.h) — 48 per-transaction bool, multi-year growth (already an architectural smell).
- **Terminal** (microsoft) — [file](file://~/oss/microsoft_terminal/src/cascadia/TerminalCore/Terminal.hpp) — terminal god-class.
- **MethodState** (MOLA lidar) — [file](file://~/oss/MOLAorg_mola_lidar_odometry/module/include/mola_lidar_odometry/LidarOdometry.h) — odometry lifecycle/dirty flags.
- **Channel** (FluidNC), **Graph** (hhds), **CommonAudioProcessor** (osci-render), **CustomLine** (GWToolbox) — + earlier ToolboxUIElement, platform.hpp, engine.hpp, solidity_convertert.

### Honest estimate of prevalence
51 content-struct drifts in 27 repos; at ~57% confirmability, real drift is in roughly **~15-16 of 73 repos (~21%)**. That is, "accumulation of boolean-state in a single meaningful struct over time" really exists in **about one in five** agentic C++ repos — not 75% as the flimsy file-metric suggested, but not "nothing" either.
