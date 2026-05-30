# Project examples report

Автогенерированный обзор top cross-file duplicated block > 5 строк для app-only режима.

Режим отчёта:

- test-like файлы исключаются бинарём по умолчанию
- report-level excludes: `examples`, `_deps`, `CMakeFiles`, `CompilerId*`
- для шумных репозиториев добавлены scope-specific excludes для amalgam/autogen/vendor деревьев

## Сводка

| Проект | Ratio | Cross-file blocks | Время | RSS | Пример |
|--------|-------|-------------------|-------|-----|--------|
| `abseil-cpp` | 8.57% | 278 | 0.10 s | 20.7 MB | [пример](examples/abseil-cpp.md) |
| `AetherSDR` | 14.39% | 609 | 0.14 s | 27.6 MB | [пример](examples/AetherSDR.md) |
| `BambuStudio` | 12.92% | 1835 | 0.97 s | 142.3 MB | [пример](examples/BambuStudio.md) |
| `Catch2` | 4.40% | 23 | 0.02 s | 7.1 MB | [пример](examples/Catch2.md) |
| `cpparch` | 5.22% | 17 | 0.61 s | 4.4 MB | [пример](examples/cpparch.md) |
| `fmt` | 2.07% | 1 | 0.01 s | 5.4 MB | — |
| `fmt/include` | 2.07% | 0 | 0.00 s | 5.4 MB | — |
| `folly` | 10.48% | 514 | 0.21 s | 40.8 MB | [пример](examples/folly.md) |
| `gm` | 5.09% | 1312 | 2.68 s | 304.4 MB | [пример](examples/gm.md) |
| `grpc` | 20.15% | 2494 | 2.16 s | 101.8 MB | [пример](examples/grpc.md) |
| `GWToolboxpp` | 6.80% | 232 | 0.12 s | 28.2 MB | [пример](examples/GWToolboxpp.md) |
| `IrredenEngine` | 10.54% | 213 | 0.09 s | 13.3 MB | [пример](examples/IrredenEngine.md) |
| `Kartend` | 15.53% | 358 | 0.09 s | 18.8 MB | [пример](examples/Kartend.md) |
| `LibreSprite` | 8.14% | 335 | 0.11 s | 22.2 MB | [пример](examples/LibreSprite.md) |
| `moqx` | 12.11% | 32 | 0.01 s | 5.6 MB | [пример](examples/moqx.md) |
| `nlohmann_json` | 16.24% | 18 | 0.04 s | 6.5 MB | [пример](examples/nlohmann_json.md) |
| `OreStudio` | 74.23% | 5874 | 1.12 s | 86.8 MB | [пример](examples/OreStudio.md) |
| `pico-sdk` | 21.60% | 224 | 0.06 s | 10.8 MB | [пример](examples/pico-sdk.md) |
| `samastra_itain` | 15.31% | 516 | 0.71 s | 26.1 MB | [пример](examples/samastra_itain.md) |
| `skyrim-community-shaders` | 6.98% | 95 | 0.06 s | 14.4 MB | [пример](examples/skyrim-community-shaders.md) |
| `spdlog` | 8.46% | 32 | 0.02 s | 5.1 MB | [пример](examples/spdlog.md) |
| `spdlog/include` | 8.26% | 29 | 0.00 s | 5.0 MB | [пример](examples/spdlog_include.md) |
| `spectre` | 26.96% | 4522 | 0.53 s | 74.4 MB | [пример](examples/spectre.md) |
| `test-motion_platform` | 3.29% | 0 | 0.00 s | 3.7 MB | — |
| `vmecpp` | 11.74% | 61 | 0.03 s | 7.4 MB | [пример](examples/vmecpp.md) |

## Карточки

### abseil-cpp

- `discovered files`: 616
- `eligible files`: 614
- `significant LOC`: 99845
- `duplicated LOC`: 8558
- `duplication ratio`: 8.57%
- `cross-file blocks`: 278
- `elapsed`: 0.10 s
- `peak RSS`: 20.7 MB
- `файл A`: `absl/container/linked_hash_map.h:274`
- `файл B`: `absl/container/linked_hash_set.h:263`
- `длина`: `30` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: Параллельные реализации linked_hash_map/linked_hash_set. Реальный текстовый дубль, но выглядит как осознанный twin-контейнер.
- `полный пример`: [examples/abseil-cpp.md](examples/abseil-cpp.md)

### AetherSDR

- `discovered files`: 551
- `eligible files`: 550
- `significant LOC`: 136670
- `duplicated LOC`: 19668
- `duplication ratio`: 14.39%
- `cross-file blocks`: 609
- `elapsed`: 0.14 s
- `peak RSS`: 27.6 MB
- `файл A`: `src/gui/ClientCompEditor.cpp:68`
- `файл B`: `src/gui/StripCompPanel.cpp:68`
- `длина`: `141` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: ClientCompEditor и StripCompPanel содержат почти дословный UI/Qt twin. Это не генерация, а реальный ручной copy-paste-кандидат.
- `полный пример`: [examples/AetherSDR.md](examples/AetherSDR.md)

### BambuStudio

- `discovered files`: 1842
- `eligible files`: 1827
- `significant LOC`: 788138
- `duplicated LOC`: 101825
- `duplication ratio`: 12.92%
- `cross-file blocks`: 1835
- `elapsed`: 0.97 s
- `peak RSS`: 142.3 MB
- `файл A`: `src/slic3r/GUI/Gizmos/GLGizmoAdvancedCut.cpp:2327`
- `файл B`: `src/slic3r/GUI/Gizmos/GLGizmoColorCut.cpp:2745`
- `длина`: `197` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: GLGizmoAdvancedCut и GLGizmoColorCut делят большой ImGui-блок. Находка валидная и архитектурно интересная.
- `полный пример`: [examples/BambuStudio.md](examples/BambuStudio.md)

### Catch2

- `discovered files`: 298
- `eligible files`: 295
- `significant LOC`: 20766
- `duplicated LOC`: 913
- `duplication ratio`: 4.40%
- `cross-file blocks`: 23
- `elapsed`: 0.02 s
- `peak RSS`: 7.1 MB
- `файл A`: `src/catch2/reporters/catch_reporter_junit.cpp:261`
- `файл B`: `src/catch2/reporters/catch_reporter_sonarqube.cpp:107`
- `длина`: `21` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: После исключения extras/amalgam остался twin-код двух репортеров: junit и sonarqube. Это не packaging-артефакт, а намеренно параллельная реализация.
- `полный пример`: [examples/Catch2.md](examples/Catch2.md)

### cpparch

- `discovered files`: 61
- `eligible files`: 61
- `significant LOC`: 5422
- `duplicated LOC`: 283
- `duplication ratio`: 5.22%
- `cross-file blocks`: 17
- `elapsed`: 0.61 s
- `peak RSS`: 4.4 MB
- `файл A`: `src/report/json_reporter.cpp:6`
- `файл B`: `src/report/violation_baseline.cpp:12`
- `длина`: `21` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: Небольшой реальный дубль helper-функции jsonEscape между reporter и baseline-кодом. Сигнал валиден, но локальный по масштабу.
- `полный пример`: [examples/cpparch.md](examples/cpparch.md)

### fmt

- `discovered files`: 19
- `eligible files`: 18
- `significant LOC`: 13428
- `duplicated LOC`: 278
- `duplication ratio`: 2.07%
- `cross-file blocks`: 1
- `elapsed`: 0.01 s
- `peak RSS`: 5.4 MB
- `пример`: нет cross-file блока длиннее 5 значимых строк

### fmt/include

- `discovered files`: 15
- `eligible files`: 14
- `significant LOC`: 12926
- `duplicated LOC`: 268
- `duplication ratio`: 2.07%
- `cross-file blocks`: 0
- `elapsed`: 0.00 s
- `peak RSS`: 5.4 MB
- `пример`: нет cross-file блока длиннее 5 значимых строк

### folly

- `discovered files`: 1385
- `eligible files`: 1358
- `significant LOC`: 220269
- `duplicated LOC`: 23087
- `duplication ratio`: 10.48%
- `cross-file blocks`: 514
- `elapsed`: 0.21 s
- `peak RSS`: 40.8 MB
- `файл A`: `folly/hash/SpookyHashV1.cpp:235`
- `файл B`: `folly/hash/SpookyHashV2.cpp:236`
- `длина`: `56` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: SpookyHashV1 и SpookyHashV2 делят одну алгоритмическую основу. Это intentional version-fork, а не подозрительный копипаст.
- `полный пример`: [examples/folly.md](examples/folly.md)

### gm

- `discovered files`: 1486
- `eligible files`: 1484
- `significant LOC`: 1436936
- `duplicated LOC`: 73167
- `duplication ratio`: 5.09%
- `cross-file blocks`: 1312
- `elapsed`: 2.68 s
- `peak RSS`: 304.4 MB
- `файл A`: `rmi_src/devices/RASP/Encoder/encoder.cpp:75`
- `файл B`: `rmi_src/devices/RASP/Encoder/encoder2.cpp:73`
- `длина`: `134` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: encoder.cpp и encoder2.cpp делят большой ручной дубль таблиц и setup-логики для variant-specific устройства.
- `полный пример`: [examples/gm.md](examples/gm.md)

### grpc

- `discovered files`: 2903
- `eligible files`: 2753
- `significant LOC`: 510798
- `duplicated LOC`: 102904
- `duplication ratio`: 20.15%
- `cross-file blocks`: 2494
- `elapsed`: 2.16 s
- `peak RSS`: 101.8 MB
- `файл A`: `src/core/server/xds_server_config_fetcher.cc:19`
- `файл B`: `src/core/server/xds_server_config_fetcher_legacy.cc:19`
- `длина`: `845` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: xds_server_config_fetcher и xds_server_config_fetcher_legacy почти совпадают. Это intentional legacy-fork, не автогенерация.
- `полный пример`: [examples/grpc.md](examples/grpc.md)

### GWToolboxpp

- `discovered files`: 560
- `eligible files`: 550
- `significant LOC`: 138649
- `duplicated LOC`: 9428
- `duplication ratio`: 6.80%
- `cross-file blocks`: 232
- `elapsed`: 0.12 s
- `peak RSS`: 28.2 MB
- `файл A`: `GWToolboxdll/Utils/TextUtils.cpp:11`
- `файл B`: `plugins/Base/PluginUtils.cpp:35`
- `длина`: `52` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: В TextUtils и PluginUtils продублирован большой справочник диакритики. Это реальный copy-paste shared data/helper-кода.
- `полный пример`: [examples/GWToolboxpp.md](examples/GWToolboxpp.md)

### IrredenEngine

- `discovered files`: 617
- `eligible files`: 595
- `significant LOC`: 58728
- `duplicated LOC`: 6191
- `duplication ratio`: 10.54%
- `cross-file blocks`: 213
- `elapsed`: 0.09 s
- `peak RSS`: 13.3 MB
- `файл A`: `creations/demos/z_yaw_rotation/main_mouse.cpp:157`
- `файл B`: `creations/demos/z_yaw_rotation/main_static.cpp:100`
- `длина`: `36` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: main_mouse и main_static делят bootstrap одной demo-сцены. Это intentional demo twin с небольшими отличиями по режиму запуска.
- `полный пример`: [examples/IrredenEngine.md](examples/IrredenEngine.md)

### Kartend

- `discovered files`: 693
- `eligible files`: 689
- `significant LOC`: 87036
- `duplicated LOC`: 13513
- `duplication ratio`: 15.53%
- `cross-file blocks`: 358
- `elapsed`: 0.09 s
- `peak RSS`: 18.8 MB
- `файл A`: `src/core/mainwindow_dialogs.cpp:93`
- `файл B`: `src/core/mainwindow_setup.cpp:624`
- `длина`: `581` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: mainwindow_dialogs.cpp и mainwindow_setup.cpp содержат большой копипаст методов MainWindow. Это реальный ручной дубль, не generated.
- `полный пример`: [examples/Kartend.md](examples/Kartend.md)

### LibreSprite

- `discovered files`: 1204
- `eligible files`: 1197
- `significant LOC`: 116002
- `duplicated LOC`: 9447
- `duplication ratio`: 8.14%
- `cross-file blocks`: 335
- `elapsed`: 0.11 s
- `peak RSS`: 22.2 MB
- `файл A`: `src/app/commands/cmd_move_mask.cpp:203`
- `файл B`: `src/app/commands/cmd_scroll.cpp:139`
- `длина`: `36` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: cmd_move_mask и cmd_scroll делят одинаковую логику форматирования текста команды. Похоже на intentional command twin, который можно было бы вынести в helper.
- `полный пример`: [examples/LibreSprite.md](examples/LibreSprite.md)

### moqx

- `discovered files`: 75
- `eligible files`: 75
- `significant LOC`: 11490
- `duplicated LOC`: 1391
- `duplication ratio`: 12.11%
- `cross-file blocks`: 32
- `elapsed`: 0.01 s
- `peak RSS`: 5.6 MB
- `файл A`: `src/MoqxPicoRelayServer.h:36`
- `файл B`: `src/MoqxRelayServer.h:29`
- `длина`: `18` значимых строк
- `вес сигнала`: `средний`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: MoqxPicoRelayServer и MoqxRelayServer имеют почти одинаковый интерфейс под два транспорта. Это intentional transport twin, не шум генерации.
- `полный пример`: [examples/moqx.md](examples/moqx.md)

### nlohmann_json

- `discovered files`: 63
- `eligible files`: 63
- `significant LOC`: 19702
- `duplicated LOC`: 3199
- `duplication ratio`: 16.24%
- `cross-file blocks`: 18
- `elapsed`: 0.04 s
- `peak RSS`: 6.5 MB
- `файл A`: `include/nlohmann/detail/iterators/iter_impl.hpp:115`
- `файл B`: `include/nlohmann/json.hpp:737`
- `длина`: `11` значимых строк
- `вес сигнала`: `средний`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: Дубль между detail-header и umbrella json.hpp. Это скорее packaging/public-header aggregation artifact, чем ручной архитектурный долг.
- `полный пример`: [examples/nlohmann_json.md](examples/nlohmann_json.md)

### OreStudio

- `discovered files`: 6876
- `eligible files`: 6753
- `significant LOC`: 504979
- `duplicated LOC`: 374849
- `duplication ratio`: 74.23%
- `cross-file blocks`: 5874
- `elapsed`: 1.12 s
- `peak RSS`: 86.8 MB
- `файл A`: `projects/ores.http.core/src/routes/iam_routes.cpp:45`
- `файл B`: `projects/ores.http.server/src/routes/iam_routes.cpp:45`
- `длина`: `1066` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: iam_routes.cpp в http.core и http.server выглядят как почти полные file-copy mirror-слои. Это реальный дубль, не generated.
- `полный пример`: [examples/OreStudio.md](examples/OreStudio.md)

### pico-sdk

- `discovered files`: 438
- `eligible files`: 407
- `significant LOC`: 45950
- `duplicated LOC`: 9925
- `duplication ratio`: 21.60%
- `cross-file blocks`: 224
- `elapsed`: 0.06 s
- `peak RSS`: 10.8 MB
- `файл A`: `src/boards/include/boards/gen4_rp2350_24.h:21`
- `файл B`: `src/boards/include/boards/gen4_rp2350_24ct.h:21`
- `длина`: `68` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: gen4_rp2350_24.h и gen4_rp2350_24ct.h почти совпадают как board-variant headers. Это intentional hardware-family twin.
- `полный пример`: [examples/pico-sdk.md](examples/pico-sdk.md)

### samastra_itain

- `discovered files`: 905
- `eligible files`: 903
- `significant LOC`: 133445
- `duplicated LOC`: 20436
- `duplication ratio`: 15.31%
- `cross-file blocks`: 516
- `elapsed`: 0.71 s
- `peak RSS`: 26.1 MB
- `файл A`: `rmi/diag/bat_diagwindow.cpp:26`
- `файл B`: `rmi/diag/imr_diagwindow.cpp:26`
- `длина`: `124` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: bat_diagwindow и imr_diagwindow делят большой Qt/UI+UDP twin-блок. Похоже на ручной copy-paste между вариантами изделия.
- `полный пример`: [examples/samastra_itain.md](examples/samastra_itain.md)

### skyrim-community-shaders

- `discovered files`: 249
- `eligible files`: 248
- `significant LOC`: 62872
- `duplicated LOC`: 4388
- `duplication ratio`: 6.98%
- `cross-file blocks`: 95
- `elapsed`: 0.06 s
- `peak RSS`: 14.4 MB
- `файл A`: `src/Features/WeatherEditor.cpp:933`
- `файл B`: `src/Utils/Game.cpp:234`
- `длина`: `25` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: Логика разворачивания weather flags продублирована между WeatherEditor и Utils/Game. Это валидный helper-duplication, а не include-list шум.
- `полный пример`: [examples/skyrim-community-shaders.md](examples/skyrim-community-shaders.md)

### spdlog

- `discovered files`: 104
- `eligible files`: 102
- `significant LOC`: 8797
- `duplicated LOC`: 744
- `duplication ratio`: 8.46%
- `cross-file blocks`: 32
- `elapsed`: 0.02 s
- `peak RSS`: 5.1 MB
- `файл A`: `include/spdlog/sinks/daily_file_sink.h:114`
- `файл B`: `include/spdlog/sinks/hourly_file_sink.h:88`
- `длина`: `15` значимых строк
- `вес сигнала`: `средний`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: Топовый блок daily/hourly вручную проверен. Это валидный архитектурный дубль между двумя sink-реализациями.
- `полный пример`: [examples/spdlog.md](examples/spdlog.md)

### spdlog/include

- `discovered files`: 91
- `eligible files`: 90
- `significant LOC`: 7810
- `duplicated LOC`: 645
- `duplication ratio`: 8.26%
- `cross-file blocks`: 29
- `elapsed`: 0.00 s
- `peak RSS`: 5.0 MB
- `файл A`: `spdlog/sinks/daily_file_sink.h:114`
- `файл B`: `spdlog/sinks/hourly_file_sink.h:88`
- `длина`: `15` значимых строк
- `вес сигнала`: `средний`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: Ручная проверка: валидные дубли daily/hourly и syslog/systemd.
- `полный пример`: [examples/spdlog_include.md](examples/spdlog_include.md)

### spectre

- `discovered files`: 3584
- `eligible files`: 3453
- `significant LOC`: 373391
- `duplicated LOC`: 100676
- `duplication ratio`: 26.96%
- `cross-file blocks`: 4522
- `elapsed`: 0.53 s
- `peak RSS`: 74.4 MB
- `файл A`: `src/Evolution/Systems/GrMhd/GhValenciaDivClean/FiniteDifference/ReconstructWork.tpp:88`
- `файл B`: `src/Evolution/Systems/GrMhd/ValenciaDivClean/FiniteDifference/ReconstructWork.tpp:184`
- `длина`: `91` значимых строк
- `вес сигнала`: `очень высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: GhValenciaDivClean и ValenciaDivClean делят один и тот же reconstruction path. Это intentional scientific/system twin.
- `полный пример`: [examples/spectre.md](examples/spectre.md)

### test-motion_platform

- `discovered files`: 5
- `eligible files`: 5
- `significant LOC`: 639
- `duplicated LOC`: 21
- `duplication ratio`: 3.29%
- `cross-file blocks`: 0
- `elapsed`: 0.00 s
- `peak RSS`: 3.7 MB
- `пример`: нет cross-file блока длиннее 5 значимых строк

### vmecpp

- `discovered files`: 97
- `eligible files`: 97
- `significant LOC`: 22172
- `duplicated LOC`: 2603
- `duplication ratio`: 11.74%
- `cross-file blocks`: 61
- `elapsed`: 0.03 s
- `peak RSS`: 7.4 MB
- `файл A`: `src/vmecpp/cpp/vmecpp/common/fourier_basis_fast_poloidal/fourier_basis_fast_poloidal.cc:357`
- `файл B`: `src/vmecpp/cpp/vmecpp/common/fourier_basis_fast_toroidal/fourier_basis_fast_toroidal.cc:356`
- `длина`: `21` значимых строк
- `вес сигнала`: `высокий`
- `похожесть`: `100.0%`
- `проверка`: вручную проверено
- `комментарий`: fourier_basis_fast_poloidal и fourier_basis_fast_toroidal делят одинаковую математику conversion indices. Это intentional basis twin.
- `полный пример`: [examples/vmecpp.md](examples/vmecpp.md)

