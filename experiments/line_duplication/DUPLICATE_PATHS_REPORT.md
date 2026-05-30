# Duplicate Paths Report

> Исторический ручной индекс из raw/app-mixed анализа.
> Для свежего app-only прогона см. `PROJECT_EXAMPLES_REPORT.md` и `examples/`.

Дата: `2026-05-30`

Этот файл — плоский индекс вида `из какого файла -> в какой файл`.

Связанные отчёты:

- [PROJECT_EXAMPLES_REPORT.md](~/projects/cpparch/experiments/line_duplication/PROJECT_EXAMPLES_REPORT.md)
- [FILTER_CLASSIFICATION_REPORT.md](~/projects/cpparch/experiments/line_duplication/FILTER_CLASSIFICATION_REPORT.md)
- [examples/](~/projects/cpparch/experiments/line_duplication/examples)

## Generated / packaging / vendor

| Repo | Файл A | Файл B | Строк | Класс |
|------|--------|--------|-------|-------|
| `Catch2` | [extras/catch_amalgamated.cpp](~/oss/Catch2/extras/catch_amalgamated.cpp:5851) | [src/catch2/internal/catch_run_context.cpp](~/oss/Catch2/src/catch2/internal/catch_run_context.cpp:31) | 636 | amalgam |
| `Catch2` | [extras/catch_amalgamated.hpp](~/oss/Catch2/extras/catch_amalgamated.hpp:4309) | [src/catch2/internal/catch_clara.hpp](~/oss/Catch2/src/catch2/internal/catch_clara.hpp:37) | 553 | amalgam |
| `nlohmann_json` | [include/nlohmann/json.hpp](~/oss/nlohmann_json/include/nlohmann/json.hpp:75) | [single_include/nlohmann/json.hpp](~/oss/nlohmann_json/single_include/nlohmann/json.hpp:20578) | 3686 | single-header |
| `gm` | [common/thirdparty/jacarta/include/WinCryptEx.h](~/projects/gm/common/thirdparty/jacarta/include/WinCryptEx.h:2) | [rmi_src/GMinstructor/3rdparty/jacarta/include/WinCryptEx.h](~/projects/gm/rmi_src/GMinstructor/3rdparty/jacarta/include/WinCryptEx.h:2) | 1516 | vendored copy |
| `gm` | [utils/upgrade/2.10.0.0/material.h](~/projects/gm/utils/upgrade/2.10.0.0/material.h:14) | [utils/upgrade/2.11.0.0/material.h](~/projects/gm/utils/upgrade/2.11.0.0/material.h:14) | 369 | version snapshot |
| `samastra_itain` | [rmi/build-debug/rmi_autogen/EWIEGA46WW/qrc_resmenu.cpp](~/projects/samastra_itain/rmi/build-debug/rmi_autogen/EWIEGA46WW/qrc_resmenu.cpp:96079) | [rmi/build-debug/rmi_autogen/KH3RVDIA23/qrc_res.cpp](~/projects/samastra_itain/rmi/build-debug/rmi_autogen/KH3RVDIA23/qrc_res.cpp:985524) | 758 | autogen |
| `samastra_itain` | [mdl/build-debug/CMakeFiles/3.18.4/CompilerIdCXX/CMakeCXXCompilerId.cpp](~/projects/samastra_itain/mdl/build-debug/CMakeFiles/3.18.4/CompilerIdCXX/CMakeCXXCompilerId.cpp:2) | [mdl/build_bat/CMakeFiles/3.18.4/CompilerIdCXX/CMakeCXXCompilerId.cpp](~/projects/samastra_itain/mdl/build_bat/CMakeFiles/3.18.4/CompilerIdCXX/CMakeCXXCompilerId.cpp:2) | 512 | build output |
| `pico-sdk` | [src/rp2040/hardware_regs/include/hardware/regs/usb_device_dpram.h](~/oss/pico-sdk/src/rp2040/hardware_regs/include/hardware/regs/usb_device_dpram.h:14) | [src/rp2350/hardware_regs/include/hardware/regs/usb_device_dpram.h](~/oss/pico-sdk/src/rp2350/hardware_regs/include/hardware/regs/usb_device_dpram.h:14) | 3748 | generated register map |
| `BambuStudio` | [src/minilzo/lzodefs.h](~/oss/BambuStudio/src/minilzo/lzodefs.h:29) | [src/minilzo/minilzo.c](~/oss/BambuStudio/src/minilzo/minilzo.c:49) | 3235 | vendored lib |
| `IrredenEngine` | [engine/render/include/glad/glad.h](~/oss/IrredenEngine/engine/render/include/glad/glad.h:1274) | [engine/render/include/irreden/render/gl_wrap/glcorearb.h](~/oss/IrredenEngine/engine/render/include/irreden/render/gl_wrap/glcorearb.h:2778) | 194 | generated GL headers |
| `OreStudio` | [projects/ores.ore/src/domain/domain.cpp](~/oss/OreStudio/projects/ores.ore/src/domain/domain.cpp:20513) | [projects/ores.ore.core/src/domain/domain.cpp](~/oss/OreStudio/projects/ores.ore.core/src/domain/domain.cpp:20558) | 13641 | mirrored/generated domain |
| `cpparch` | [sandbox/drift_repos/BambuStudio/src/minilzo/lzodefs.h](~/projects/cpparch/sandbox/drift_repos/BambuStudio/src/minilzo/lzodefs.h:29) | [sandbox/drift_repos/BambuStudio/src/minilzo/minilzo.c](~/projects/cpparch/sandbox/drift_repos/BambuStudio/src/minilzo/minilzo.c:49) | 3235 | imported drift repo |

## Test twins / expected variants

| Repo | Файл A | Файл B | Строк | Класс |
|------|--------|--------|-------|-------|
| `fmt` | [test/fuzzing/named-arg.cc](~/oss/fmt/test/fuzzing/named-arg.cc:45) | [test/fuzzing/two-args.cc](~/oss/fmt/test/fuzzing/two-args.cc:41) | 41 | fuzz harness twin |
| `abseil-cpp` | [unordered_map_members_test.h](~/oss/abseil-cpp/absl/container/internal/unordered_map_members_test.h:38) | [unordered_set_members_test.h](~/oss/abseil-cpp/absl/container/internal/unordered_set_members_test.h:37) | 44 | twin test |
| `AetherSDR` | [tests/CAT_Flex_test.cpp](~/oss/AetherSDR/tests/CAT_Flex_test.cpp:10) | [tests/CAT_TS-2000_test.cpp](~/oss/AetherSDR/tests/CAT_TS-2000_test.cpp:10) | 211 | test twin |
| `spectre` | [Test_KerrSchild.cpp](~/oss/spectre/tests/Unit/PointwiseFunctions/AnalyticSolutions/GeneralRelativity/Test_KerrSchild.cpp:105) | [Test_SphericalKerrSchild.cpp](~/oss/spectre/tests/Unit/PointwiseFunctions/AnalyticSolutions/GeneralRelativity/Test_SphericalKerrSchild.cpp:104) | 124 | test twin |
| `folly` | [folly/container/test/F14MapTest.cpp](~/oss/folly/folly/container/test/F14MapTest.cpp:1032) | [folly/container/test/UtilTest.cpp](~/oss/folly/folly/container/test/UtilTest.cpp:30) | 63 | test/helper twin |
| `vmecpp` | [fourier_basis_fast_poloidal_test.cc](~/oss/vmecpp/src/vmecpp/cpp/vmecpp/common/fourier_basis_fast_poloidal/fourier_basis_fast_poloidal_test.cc:948) | [fourier_basis_fast_toroidal_test.cc](~/oss/vmecpp/src/vmecpp/cpp/vmecpp/common/fourier_basis_fast_toroidal/fourier_basis_fast_toroidal_test.cc:918) | 46 | test twin |
| `nlohmann_json` | [tests/src/unit-class_parser.cpp](~/oss/nlohmann_json/tests/src/unit-class_parser.cpp:345) | [tests/src/unit-class_parser_diagnostic_positions.cpp](~/oss/nlohmann_json/tests/src/unit-class_parser_diagnostic_positions.cpp:356) | 1112 | test twin |

## Real manual-looking duplicates

| Repo | Файл A | Файл B | Строк | Класс |
|------|--------|--------|-------|-------|
| `spdlog/include` | [daily_file_sink.h](~/oss/spdlog/include/spdlog/sinks/daily_file_sink.h:114) | [hourly_file_sink.h](~/oss/spdlog/include/spdlog/sinks/hourly_file_sink.h:88) | 15 | real copy-paste |
| `spdlog/include` | [syslog_sink.h](~/oss/spdlog/include/spdlog/sinks/syslog_sink.h:47) | [systemd_sink.h](~/oss/spdlog/include/spdlog/sinks/systemd_sink.h:54) | 8 | real copy-paste |
| `gm_github` | [rmi/diagnostic/packetlogger.cpp](~/projects/gm_github/rmi/diagnostic/packetlogger.cpp:50) | [viz/network/packetlogger.cpp](~/projects/gm_github/viz/network/packetlogger.cpp:55) | 78 | real copy-paste |
| `gm_github` | [BAT2_Devices/p_light.cpp](~/projects/gm_github/rmi_src/devices/BAT2_Devices/p_light.cpp:1) | [IMR_Devices/p_light.cpp](~/projects/gm_github/rmi_src/devices/IMR_Devices/p_light.cpp:1) | 90 | device twin |
| `gm_github` | [BAT2_Devices/p_brake.cpp](~/projects/gm_github/rmi_src/devices/BAT2_Devices/p_brake.cpp:1) | [PTS_Devices/p_brake.cpp](~/projects/gm_github/rmi_src/devices/PTS_Devices/p_brake.cpp:1) | 45 | device twin |
| `gm_github` | [BAT2_Devices/l_pnev.cpp](~/projects/gm_github/rmi_src/devices/BAT2_Devices/l_pnev.cpp:3) | [PTS_Devices/l_pnev.cpp](~/projects/gm_github/rmi_src/devices/PTS_Devices/l_pnev.cpp:3) | 44 | device twin |
| `AetherSDR` | [src/gui/ClientCompEditor.cpp](~/oss/AetherSDR/src/gui/ClientCompEditor.cpp:68) | [src/gui/StripCompPanel.cpp](~/oss/AetherSDR/src/gui/StripCompPanel.cpp:68) | 141 | GUI twin |
| `AetherSDR` | [src/gui/ClientEqEditor.cpp](~/oss/AetherSDR/src/gui/ClientEqEditor.cpp:78) | [src/gui/StripEqPanel.cpp](~/oss/AetherSDR/src/gui/StripEqPanel.cpp:79) | 118 | GUI twin |
| `AetherSDR` | [src/gui/StripWaveform.cpp](~/oss/AetherSDR/src/gui/StripWaveform.cpp:178) | [src/gui/WaveformWidget.cpp](~/oss/AetherSDR/src/gui/WaveformWidget.cpp:172) | 92 | GUI twin |
| `Kartend` | [src/core/mainwindow_dialogs.cpp](~/oss/Kartend/src/core/mainwindow_dialogs.cpp:93) | [src/core/mainwindow_setup.cpp](~/oss/Kartend/src/core/mainwindow_setup.cpp:624) | 581 | real copy-paste |
| `Kartend` | [src/modules/data/query/scanservice.cpp](~/oss/Kartend/src/modules/data/query/scanservice.cpp:825) | [src/modules/data/query/scanservice_persist.cpp](~/oss/Kartend/src/modules/data/query/scanservice_persist.cpp:116) | 424 | real copy-paste |
| `LibreSprite` | [src/app/commands/cmd_move_mask.cpp](~/oss/LibreSprite/src/app/commands/cmd_move_mask.cpp:203) | [src/app/commands/cmd_scroll.cpp](~/oss/LibreSprite/src/app/commands/cmd_scroll.cpp:139) | 36 | command twin |
| `GWToolboxpp` | [GWToolboxdll/Utils/TextUtils.cpp](~/oss/GWToolboxpp/GWToolboxdll/Utils/TextUtils.cpp:11) | [plugins/Base/PluginUtils.cpp](~/oss/GWToolboxpp/plugins/Base/PluginUtils.cpp:35) | 52 | real copy-paste |
| `moqx` | [src/MoqxPicoRelayServer.h](~/oss/moqx/src/MoqxPicoRelayServer.h:36) | [src/MoqxRelayServer.h](~/oss/moqx/src/MoqxRelayServer.h:29) | 18 | wrapper twin |
| `BambuStudio` | [GLGizmoAdvancedCut.cpp](~/oss/BambuStudio/src/slic3r/GUI/Gizmos/GLGizmoAdvancedCut.cpp:2327) | [GLGizmoColorCut.cpp](~/oss/BambuStudio/src/slic3r/GUI/Gizmos/GLGizmoColorCut.cpp:2745) | 197 | real copy-paste |
| `BambuStudio` | [Notebook.hpp](~/oss/BambuStudio/src/slic3r/GUI/Notebook.hpp:270) | [Tabbook.hpp](~/oss/BambuStudio/src/slic3r/GUI/Tabbook.hpp:270) | 79 | UI twin |
| `OreStudio` | [projects/ores.http.core/src/routes/iam_routes.cpp](~/oss/OreStudio/projects/ores.http.core/src/routes/iam_routes.cpp:45) | [projects/ores.http.server/src/routes/iam_routes.cpp](~/oss/OreStudio/projects/ores.http.server/src/routes/iam_routes.cpp:45) | 1066 | real copy-paste |
| `OreStudio` | [projects/ores.qt/src/DataLibrarianWindow.cpp](~/oss/OreStudio/projects/ores.qt/src/DataLibrarianWindow.cpp:319) | [projects/ores.qt.refdata/src/DataLibrarianWindow.cpp](~/oss/OreStudio/projects/ores.qt.refdata/src/DataLibrarianWindow.cpp:324) | 911 | UI twin |
| `spectre` | [GhValenciaDivClean/ReconstructWork.tpp](~/oss/spectre/src/Evolution/Systems/GrMhd/GhValenciaDivClean/FiniteDifference/ReconstructWork.tpp:88) | [ValenciaDivClean/ReconstructWork.tpp](~/oss/spectre/src/Evolution/Systems/GrMhd/ValenciaDivClean/FiniteDifference/ReconstructWork.tpp:184) | 91 | variant twin |
| `spectre` | [GaugePlaneWave.hpp](~/oss/spectre/src/PointwiseFunctions/AnalyticSolutions/GeneralRelativity/GaugePlaneWave.hpp:172) | [GaugeWave.hpp](~/oss/spectre/src/PointwiseFunctions/AnalyticSolutions/GeneralRelativity/GaugeWave.hpp:178) | 71 | variant twin |
| `folly` | [folly/hash/SpookyHashV1.cpp](~/oss/folly/folly/hash/SpookyHashV1.cpp:235) | [folly/hash/SpookyHashV2.cpp](~/oss/folly/folly/hash/SpookyHashV2.cpp:236) | 56 | implementation twin |
| `folly` | [folly/container/F14Map.h](~/oss/folly/folly/container/F14Map.h:873) | [folly/container/F14Set.h](~/oss/folly/folly/container/F14Set.h:602) | 49 | implementation twin |
| `samastra_itain` | [rmi/diag/bat_diagwindow.cpp](~/projects/samastra_itain/rmi/diag/bat_diagwindow.cpp:26) | [rmi/diag/imr_diagwindow.cpp](~/projects/samastra_itain/rmi/diag/imr_diagwindow.cpp:26) | 124 | real copy-paste |
| `samastra_itain` | [mdl/gear_bat.cpp](~/projects/samastra_itain/mdl/gear_bat.cpp:27) | [mdl/gear_pts.cpp](~/projects/samastra_itain/mdl/gear_pts.cpp:27) | 57 | device twin |

## Что смотреть сначала

Если нужен короткий ручной review-list, я бы начал с этих пар:

1. `spdlog/include`: `daily_file_sink.h -> hourly_file_sink.h`
2. `gm_github`: `rmi/diagnostic/packetlogger.cpp -> viz/network/packetlogger.cpp`
3. `BambuStudio`: `GLGizmoAdvancedCut.cpp -> GLGizmoColorCut.cpp`
4. `Kartend`: `mainwindow_dialogs.cpp -> mainwindow_setup.cpp`
5. `AetherSDR`: `ClientCompEditor.cpp -> StripCompPanel.cpp`
6. `OreStudio`: `ores.http.core/.../iam_routes.cpp -> ores.http.server/.../iam_routes.cpp`
