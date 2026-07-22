# Boolean-State — detection by GROUP ASSIGNMENT (no names)

**Date:** 2026-06-07 · **Task:** #089 / #090
**Corpus:** all 790 repos, scanned `.cpp/.cc/.h/.hpp`.

## Signal (names are not used)

We look for places where **≥3 bool fields of one object** are assigned `true`/`false` near each other (within 10 lines) with a **true+false mix** — the "one `true`, the rest `false`" pattern, i.e. a **state transition**. We count **transition sites** — how many DIFFERENT places perform such a group reset over the same set of fields. ≥2 sites = the developer manually duplicates "clear the others" = a clear sign of an implicit FSM.

## Funnel

| Method | Candidates | Nature of the noise |
|---|---|---|
| "5+ bool" counter | 5811 | 78% — config bags |
| Group assignment (raw) | 3033 | test fixtures, bulk config init |
| **+ filter: not a test, 3-6 fields, ≥2 sites** | **283** | the remainder — real FSMs + some state-like config |

A small field count is essential here: a real machine = 3-6 mutually exclusive phases. 22-46 fields in a single group assignment is not state, but bulk config setup.

## Confirmed by inspection (🟢 real FSMs that naming would NOT catch)

**ZuluSCSI audio** — [audio_i2s.cpp:824](file://~/oss_corpus/oss/ZuluSCSI_ZuluSCSI-firmware/lib/ZuluSCSI_audio_RP2MCU/audio_i2s.cpp#L824): 3 bools `audio_idle/playing/paused`, transitions in 4 places (824→IDLE, 831→PLAYING, 876→IDLE, 1019→PLAYING). A textbook 3-state automaton. `playing` is not in the name dictionary.

Others confirmed by the look of the transitions:
- **ncnn** `prefer_winograd23/43/63` — mutually exclusive choice of convolution algorithm ([convolution_x86.cpp:418](file://~/oss_corpus/oss/Tencent_ncnn/src/layer/x86/convolution_x86.cpp#L418)), duplicated for x86/mips/loongarch.
- **Lightpad markdown** `inBlockquote/inFencedBlock/inList/inTable…` — parser state ([markdowntools.cpp:1050](file://~/oss_corpus/oss/djeada_Lightpad/App/markdown/markdowntools.cpp#L1050)).
- **OASIS SLAM** — [MonocularInertialSlamNode.cpp:648](file://~/oss_corpus/oss/eigendude_OASIS/oasis_perception_cpp/src/nodes/MonocularInertialSlamNode.cpp#L648) (our ⭐ from the usage analysis).
- **socnetv** `isCycle/isUp/isDown/isTrans…` — triad classification ([graph_triad_census.cpp:216](file://~/oss_corpus/oss/socnetv_app/src/graph/clustering/graph_triad_census.cpp#L216)).
- **v4l2** `g_isCaptureOn/PreviewOn/VideoOn` — camera mode.

## Residual FPs of this detector (🔴, group assignment ≠ always an FSM)

- **Z3** `m_params` (solver config), **irrlicht** `rasterizerDesc` (D3D11 render state), **blender** `pass_info` `use_*` — independent flags set as a group during configuration, but not mutually exclusive.
- Test fixtures/options would leak through without the filter (`options`/`settings` in `*_test.cpp`).

→ The final decision still requires a mutual-exclusion check (does the set read as "exactly one true"). But **the share of real FSMs here is incomparably higher** than with naming.

## Comparison of the three methods

| | Naming (v0.2 rule) | 5+ counter | Group assignment |
|---|---|---|---|
| Candidates (50/790 repos) | 1 | 59 / 5811 | 283 (filtered) |
| Real FSMs in the catch | 0 (the only one — an FP) | ~19% partial | confirmed 🟢 at the top |
| Catches FSMs with "non-standard" names | ❌ | — | ✅ (winograd, audio_playing, inList) |
| Depends on names | yes | no | **no** |
| Needs AST | no | no | no (regex proxy) |

## Conclusion

**Group assignment is the right primitive.** A cheap regex proxy (without AST) already finds real state machines that the name dictionary fundamentally misses, and its single "catch" (`SimulationData`) was a false one. This empirically justifies direction **#042**: a full detector looks at *usage* (mutual exclusion + group transition + the absence of an already-ready enum nearby), not at names.

Full list — `experiments/boolean_state/transition_scan.csv`.

## Top-40 (not a test, 3-6 fields, ≥2 sites; mirror repos collapsed)

| Sites | Fields | Receiver | File | Fields |
|---|---|---|---|---|
| 26 | 3 | `interface_ip` | [core.cc:115](file://~/oss_corpus/oss/OpenXiangShan_GEM5/ext/mcpat/core.cc#L115)<br><sub>OpenXiangShan_GEM5/ext/mcpat/core.cc</sub> | `is_cache, pure_cam, pure_ram` |
| 6 | 5 | `tOpt` | [gtests_filter.cpp:25](file://~/oss_corpus/oss/manticoresoftware_manticoresearch/src/gtests/gtests_filter.cpp#L25)<br><sub>manticoresoftware_manticoresearch/src/gtests/gtests_filter.cpp</sub> | `m_bExclude, m_bHasEqualMax, m_bHasEqualMin, m_bOpenLeft, m_bOpenRight` |
| 6 | 5 | `cfg` | [TFTDisplay.cpp:74](file://~/oss_corpus/oss/meshtastic_firmware/src/graphics/TFTDisplay.cpp#L74)<br><sub>meshtastic_firmware/src/graphics/TFTDisplay.cpp</sub> | `bus_shared, dlen_16bit, invert, readable, rgb_order` |
| 5 | 5 | `extension` | [extension_set.cc:412](file://~/oss_corpus/oss/sailfishos-mirror_protobuf/src/google/protobuf/extension_set.cc#L412)<br><sub>sailfishos-mirror_protobuf/src/google/protobuf/extension_set.cc</sub> | `is_cleared, is_lazy, is_packed, is_pointer, is_repeated` |
| 5 | 4 | `(this)` | [visual_script_property_selector.cpp:724](file://~/oss_corpus/oss/RebelToolbox_RebelEngine/modules/visual_script/visual_script_property_selector.cpp#L724)<br><sub>RebelToolbox_RebelEngine/modules/visual_script/visual_script_property_selector.cpp</sub> | `properties, seq_connect, virtuals_only, visual_script_generic` |
| 5 | 4 | `settings` | [tst_imageoptimizertest.cpp:296](file://~/oss_corpus/oss/_cpp_archcheck/JakubMelka_PDF4QT/UnitTests/tst_imageoptimizertest.cpp#L296)<br><sub>_cpp_archcheck/JakubMelka_PDF4QT/UnitTests/tst_imageoptimizertest.cpp</sub> | `autoMode, enabled, keepOriginalIfLarger, preserveTransparency` |
| 5 | 3 | `(this)` | [convolution_loongarch.cpp:234](file://~/oss_corpus/oss/Tencent_ncnn/src/layer/loongarch/convolution_loongarch.cpp#L234)<br><sub>Tencent_ncnn/src/layer/loongarch/convolution_loongarch.cpp</sub> | `prefer_winograd23, prefer_winograd43, prefer_winograd63` |
| 4 | 6 | `(this)` | [TextureImportDialog.cpp:1916](file://~/oss_corpus/oss/BambuStudio/src/slic3r/GUI/TextureImportDialog.cpp#L1916)<br><sub>BambuStudio/src/slic3r/GUI/TextureImportDialog.cpp</sub> | `m_cancel_flag, m_current_computation_initial, m_fallback_to_geometry_only, m_initial_computation_cancelled, m_initial_computation_failed, m_initial_computation_pending` |
| 4 | 5 | `(this)` | [imagebuf.cpp:432](file://~/oss_corpus/oss/AcademySoftwareFoundation_OpenImageIO/src/libOpenImageIO/imagebuf.cpp#L432)<br><sub>AcademySoftwareFoundation_OpenImageIO/src/libOpenImageIO/imagebuf.cpp</sub> | `m_badfile, m_pixels_read, m_pixels_valid, m_readonly, m_spec_valid` |
| 4 | 5 | `(this)` | [QoreSocket.cpp:12675](file://~/oss_corpus/oss/qoretechnologies_qore/lib/QoreSocket.cpp#L12675)<br><sub>qoretechnologies_qore/lib/QoreSocket.cpp</sub> | `done, initialized, ok, received, set_non_block` |
| 4 | 4 | `(this)` | [network_service.cpp:722](file://~/oss_corpus/oss/LoRaMesher_LoRaMesher/src/protocols/lora_mesh/services/network_service.cpp#L722)<br><sub>LoRaMesher_LoRaMesher/src/protocols/lora_mesh/services/network_service.cpp</sub> | `is_synchronized_, network_creator_, network_found_, surrendered_in_election_` |
| 4 | 4 | `(this)` | [network_service.cpp:722](file://~/oss_corpus/oss/_cpp_archcheck/LoRaMesher_LoRaMesher/src/protocols/lora_mesh/services/network_service.cpp#L722)<br><sub>_cpp_archcheck/LoRaMesher_LoRaMesher/src/protocols/lora_mesh/services/network_service.cpp</sub> | `is_synchronized_, network_creator_, network_found_, surrendered_in_election_` |
| 4 | 4 | `entryV` | [BDPTIntegrator.cpp:2118](file://~/oss_corpus/oss/aravindkrishnaswamy_RISE/src/Library/Shaders/BDPTIntegrator.cpp#L2118)<br><sub>aravindkrishnaswamy_RISE/src/Library/Shaders/BDPTIntegrator.cpp</sub> | `guidingHasSegment, isBSSRDFEntry, isConnectible, isDelta` |
| 4 | 4 | `result` | [BDPTIntegrator.cpp:2925](file://~/oss_corpus/oss/aravindkrishnaswamy_RISE/src/Library/Shaders/BDPTIntegrator.cpp#L2925)<br><sub>aravindkrishnaswamy_RISE/src/Library/Shaders/BDPTIntegrator.cpp</sub> | `guidingUseDirectContribution, guidingValid, needsSplat, valid` |
| 4 | 4 | `(this)` | [blocktriple.hpp:311](file://~/oss_corpus/oss/stillwater-sc_universal/include/sw/universal/internal/blocktriple/blocktriple.hpp#L311)<br><sub>stillwater-sc_universal/include/sw/universal/internal/blocktriple/blocktriple.hpp</sub> | `_inf, _nan, _sign, _zero` |
| 4 | 3 | `parameters` | [MRFileDialog.cpp:520](file://~/oss_corpus/oss/MeshInspector_MeshLib/source/MRViewer/MRFileDialog.cpp#L520)<br><sub>MeshInspector_MeshLib/source/MRViewer/MRFileDialog.cpp</sub> | `folderDialog, multiselect, saveDialog` |
| 4 | 3 | `(this)` | [convolution_x86.cpp:418](file://~/oss_corpus/oss/Tencent_ncnn/src/layer/x86/convolution_x86.cpp#L418)<br><sub>Tencent_ncnn/src/layer/x86/convolution_x86.cpp</sub> | `prefer_winograd23, prefer_winograd43, prefer_winograd63` |
| 4 | 3 | `(this)` | [convolution_mips.cpp:289](file://~/oss_corpus/oss/Tencent_ncnn/src/layer/mips/convolution_mips.cpp#L289)<br><sub>Tencent_ncnn/src/layer/mips/convolution_mips.cpp</sub> | `prefer_winograd23, prefer_winograd43, prefer_winograd63` |
| 4 | 3 | `new_symbol` | [converter_stmt.cpp:550](file://~/oss_corpus/oss/esbmc_esbmc/src/python-frontend/converter/converter_stmt.cpp#L550)<br><sub>esbmc_esbmc/src/python-frontend/converter/converter_stmt.cpp</sub> | `file_local, is_extern, lvalue` |
| 4 | 3 | `(this)` | [FilePicker.cpp:37](file://~/oss_corpus/oss/godot42x_ya/Engine/Source/Editor/FilePicker.cpp#L37)<br><sub>godot42x_ya/Engine/Source/Editor/FilePicker.cpp</sub> | `_bSceneSaveMode, _isOpen, _pendingClose` |
| 4 | 3 | `project_requirements.physical_activity` | [IncomeStratumAdjustment.Test.cpp:67](file://~/oss_corpus/oss/imperialCHEPI_healthgps/src/HealthGPS.Tests/IncomeStratumAdjustment.Test.cpp#L67)<br><sub>imperialCHEPI_healthgps/src/HealthGPS.Tests/IncomeStratumAdjustment.Test.cpp</sub> | `adjust_to_factors_mean, enabled, trended` |
| 4 | 3 | `ci` | [ha_mcs_impl.cpp:3077](file://~/oss_corpus/oss/mariadb-corporation_mariadb-columnstore-engine/dbcon/mysql/ha_mcs_impl.cpp#L3077)<br><sub>mariadb-corporation_mariadb-columnstore-engine/dbcon/mysql/ha_mcs_impl.cpp</sub> | `isCacheInsert, isLoaddataInfile, singleInsert` |
| 4 | 3 | `Candidate` | [SemaOverload.cpp:5800](file://~/oss_corpus/oss/microsoft_DirectXShaderCompiler/tools/clang/lib/Sema/SemaOverload.cpp#L5800)<br><sub>microsoft_DirectXShaderCompiler/tools/clang/lib/Sema/SemaOverload.cpp</sub> | `IgnoreObjectArgument, IsSurrogate, Viable` |
| 3 | 6 | `bufferTestThreadStruct_` | [DTCFrontEndInterfaceImpl.cc:5277](file://~/oss_corpus/oss/Mu2e_otsdaq-mu2e/otsdaq-mu2e/FEInterfaces/DTCFrontEndInterfaceImpl.cc#L5277)<br><sub>Mu2e_otsdaq-mu2e/otsdaq-mu2e/FEInterfaces/DTCFrontEndInterfaceImpl.cc</sub> | `activeMatch_, doNotResetCounters_, exitThread_, inSubeventMode_, resetStartEventTag_, running_` |
| 3 | 6 | `m_params` | [smt_setup.cpp:274](file://~/oss_corpus/oss/Z3Prover_z3/src/smt/smt_setup.cpp#L274)<br><sub>Z3Prover_z3/src/smt/smt_setup.cpp</sub> | `m_arith_eq2ineq, m_arith_propagate_eqs, m_arith_reflect, m_lemma_gc_half, m_nnf_cnf, m_restart_adaptive` |
| 3 | 6 | `(this)` | [audio_i2s.cpp:824](file://~/oss_corpus/oss/ZuluSCSI_ZuluSCSI-firmware/lib/ZuluSCSI_audio_RP2MCU/audio_i2s.cpp#L824)<br><sub>ZuluSCSI_ZuluSCSI-firmware/lib/ZuluSCSI_audio_RP2MCU/audio_i2s.cpp</sub> | `audio_idle, audio_paused, audio_playing, audio_stopping, last_track_reached, within_gap` |
| 3 | 6 | `pass_info` | [pass.cpp:166](file://~/oss_corpus/oss/blender_cycles/src/scene/pass.cpp#L166)<br><sub>blender_cycles/src/scene/pass.cpp</sub> | `is_written, support_denoise, use_compositing, use_denoising_albedo, use_exposure, use_filter` |
| 3 | 6 | `(this)` | [markdowntools.cpp:1050](file://~/oss_corpus/oss/djeada_Lightpad/App/markdown/markdowntools.cpp#L1050)<br><sub>djeada_Lightpad/App/markdown/markdowntools.cpp</sub> | `inBlockquote, inFencedBlock, inList, inOrderedList, inTable, inTableHeader` |
| 3 | 6 | `(this)` | [InductionVariable.cpp:1813](file://~/oss_corpus/oss/eclipse-omr_omr/compiler/optimizer/InductionVariable.cpp#L1813)<br><sub>eclipse-omr_omr/compiler/optimizer/InductionVariable.cpp</sub> | `_newNonAddressTempsCreated, _newTempsCreated, examineChildren, isAdd, reassociatedComputation, seenInductionVariableComputation` |
| 3 | 6 | `(this)` | [MonocularInertialSlamNode.cpp:648](file://~/oss_corpus/oss/eigendude_OASIS/oasis_perception_cpp/src/nodes/MonocularInertialSlamNode.cpp#L648)<br><sub>eigendude_OASIS/oasis_perception_cpp/src/nodes/MonocularInertialSlamNode.cpp</sub> | `armedNow, m_initRetryPending, m_initialTrackingDiagnosticPending, m_postStallRecoveryPending, m_stableInputPaused, m_startupArmed` |
| 3 | 6 | `(this)` | [v4l2_main.cpp:677](file://~/oss_corpus/oss/openharmony_drivers_peripheral/camera/vdi_base/common/adapter/platform/v4l2/src/driver_adapter/main_test/v4l2_main.cpp#L677)<br><sub>openharmony_drivers_peripheral/camera/vdi_base/common/adapter/platform/v4l2/src/driver_adapter/main_test/v4l2_main.cpp</sub> | `g_isCaptureOn, g_isCaptureOnUvc, g_isPreviewOn, g_isPreviewOnUvc, g_isVideoOn, g_isVideoOnUvc` |
| 3 | 6 | `(this)` | [QoreClass.cpp:1575](file://~/oss_corpus/oss/qoretechnologies_qore/lib/QoreClass.cpp#L1575)<br><sub>qoretechnologies_qore/lib/QoreClass.cpp</sub> | `committed, has_final, has_private_internal_variants, is_abstract, parse_init_called, parse_init_partial_called` |
| 3 | 6 | `(this)` | [graph_triad_census.cpp:216](file://~/oss_corpus/oss/socnetv_app/src/graph/clustering/graph_triad_census.cpp#L216)<br><sub>socnetv_app/src/graph/clustering/graph_triad_census.cpp</sub> | `isCycle, isDown, isInLinked, isOutLinked, isTrans, isUp` |
| 3 | 6 | `(this)` | [SampleSlicesView.cpp:57](file://~/oss_corpus/oss/xiphonics_picoTracker/sources/Application/Views/SampleSlicesView.cpp#L57)<br><sub>xiphonics_picoTracker/sources/Application/Views/SampleSlicesView.cpp</sub> | `isDirty_, modalWasOpen_, needsFullRedraw_, playKeyHeld_, previewActive_, previewCursorVisible_` |
| 3 | 5 | `(this)` | [UDPFirewallTester.cpp:148](file://~/oss_corpus/oss/amule-project_amule/src/kademlia/kademlia/UDPFirewallTester.cpp#L148)<br><sub>amule-project_amule/src/kademlia/kademlia/UDPFirewallTester.cpp</sub> | `m_firewalledLastStateUDP, m_firewalledUDP, m_isFWVerifiedUDP, m_nodeSearchStarted, m_timedOut` |
| 3 | 5 | `(this)` | [dapclient.cpp:110](file://~/oss_corpus/oss/djeada_Lightpad/App/dap/dapclient.cpp#L110)<br><sub>djeada_Lightpad/App/dap/dapclient.cpp</sub> | `m_dataBreakpointsConfigured, m_dataBreakpointsSupported, m_functionBreakpointsConfigured, m_hasDeferredFunctionBreakpoints, m_pausePending` |
| 3 | 5 | `(this)` | [symbol.cpp:16](file://~/oss_corpus/oss/esbmc_esbmc/src/util/symbol.cpp#L16)<br><sub>esbmc_esbmc/src/util/symbol.cpp</sub> | `is_set, legacy_type_valid_, legacy_value_valid_, type2_valid_, value2_valid_` |
| 3 | 5 | `preader` | [rtf.cpp:1414](file://~/oss_corpus/oss/grommunio_gromox/lib/mapi/rtf.cpp#L1414)<br><sub>grommunio_gromox/lib/mapi/rtf.cpp</sub> | `b_printed_cell_begin, b_printed_cell_end, b_printed_row_begin, b_printed_row_end, is_within_table` |
| 3 | 5 | `(this)` | [cdrom.cc:518](file://~/oss_corpus/oss/grumpycoders_pcsx-redux/src/core/cdrom.cc#L518)<br><sub>grumpycoders_pcsx-redux/src/core/cdrom.cc</sub> | `m_locationChanged, m_muted, m_play, m_suceeded, m_trackChanged` |
| 3 | 5 | `rasterizerDesc` | [CD3D11Driver.cpp:4008](file://~/oss_corpus/oss/hyyh619_irrlicht-1.8.3/source/Irrlicht/CD3D11Driver.cpp#L4008)<br><sub>hyyh619_irrlicht-1.8.3/source/Irrlicht/CD3D11Driver.cpp</sub> | `AntialiasedLineEnable, DepthClipEnable, FrontCounterClockwise, MultisampleEnable, ScissorEnable` |
