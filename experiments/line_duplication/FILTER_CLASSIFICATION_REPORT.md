# Filter and Classification Report

Дата: `2026-05-30`

Цель: понять, какие path-mask исключения можно безопасно добавить в
standalone spike, и где после этого остаётся осмысленный copy-paste.

## Главное

Сканер уже даёт полезный сигнал, но на сырых репозиториях у него два очень
разных класса находок:

1. **Технический шум**:
   amalgam/single-header, generated/autogen, build outputs, vendored deps,
   version snapshots, ABI snapshots.
2. **Реальный ручной дубликат**:
   twin-модули, legacy fork рядом с новым кодом, device/UI variants, test twins,
   иногда явный missing reuse edge.

Важно: по **snapshot**-прогону нельзя честно сказать, что дубль сделал именно
AI. Здесь можно разделить только на:

- `generated/package duplication`
- `manual-looking duplication`

Для ответа "это пришло в конкретном commit" нужен `--git-commit/--git-diff`
режим, который уже есть в spike.

## Рекомендуемые маски

### Жёсткие маски: почти всегда выключать

Эти паттерны стабильно давали low-value шум:

- `single_include`
- `extras`
- `amalgamate`
- `amalgamated`
- `dist`
- `generated`
- `*_generated*`
- `*_autogen`
- `qrc_*`
- `moc_*`
- `moc_predefs.h`
- `build-*`
- `CMakeFiles`
- `CompilerId*`
- `third_party`
- `3rdparty`
- `thirdparty`
- `bundled`
- `vendor`
- `vendors`

### Мягкие маски: включать только для app-only отчёта

Эти паттерны отсекают не шум среды, а отдельные классы кода:

- `test`
- `tests`
- `examples`
- `docs/examples`
- `utils/upgrade`
- `legacy`
- `hardware_regs`
- `hardware_structs`
- `CMSIS`
- `glad`
- `glcorearb.h`
- `deps`
- `external`

## Классификация по репозиториям

| Repo | Доминирующий класс | Что видно | Вердикт | Маска |
|------|--------------------|-----------|---------|-------|
| `Catch2` | packaging/amalgam | `extras/catch_amalgamated.* <-> src/catch2/*` | generated/package duplication | `extras` |
| `nlohmann_json` | single-header + ABI snapshots | `single_include/*`, `tests/abi/*` | generated/package duplication | `single_include`, `tests/abi/*`, `thirdparty` |
| `spdlog/include` | library copy-paste | `daily/hourly`, `syslog/systemd` | реальный ручной дубль | маски не нужны |
| `spdlog` root | build dependency noise | `build-cc/_deps/catch2-src/*` | generated/build duplication | `build-*`, `_deps` |
| `gm_github` | product duplication | `PacketLogger`, device files `p_light.cpp`, `p_brake.cpp` | реальный ручной дубль | маски не нужны |
| `gm` raw | upgrade snapshots + vendor copies | `utils/upgrade/*`, `codegenTemplates`, `3rdparty` | generated/snapshot duplication | `utils/upgrade`, `3rdparty`, `thirdparty`, `build-*` |
| `samastra_itain` raw | autogen/build | `*_autogen`, `qrc_*`, `moc_*`, `CompilerId*` | generated/build duplication | `build-*`, `*_autogen`, `qrc_*`, `moc_*`, `CMakeFiles` |
| `OreStudio` | mixed | огромный `domain.cpp` generated, потом большие app duplicates между `*.core`, `*.api`, `*.qt.*` | смешанный случай | сначала убрать generated domain, потом смотреть руками |
| `pico-sdk` | generated/register families | `hardware_regs/*`, `hardware_structs/*`, `CMSIS/*` | generated/spec duplication | `hardware_regs`, `hardware_structs`, `CMSIS` |
| `BambuStudio` raw | vendored libs | `minilzo`, `qhull`, `GMP`, `agg` | mostly vendor duplication | `minilzo`, `qhull`, `deps`, `agg` |
| `BambuStudio` filtered | app GUI duplication | `GLGizmoAdvancedCut <-> GLGizmoColorCut`, `Notebook <-> Tabbook` | реальный ручной дубль | после vendor masks сигнал полезный |
| `IrredenEngine` | generated GL headers | `glad.h <-> glcorearb.h` | generated/vendor duplication | `glad`, `glcorearb.h` |
| `AetherSDR` raw | tests first, then GUI twins | `tests/*`, потом `Client* <-> Strip*`, `Waveform*` | mixed, после tests остаётся реальный дубль | `tests` для app-only |
| `spectre` raw | tests first | `tests/*` наверху | mixed | `tests` для app-only |
| `spectre` filtered | variant duplication in src | `GhValenciaDivClean <-> ValenciaDivClean`, `GaugePlaneWave <-> GaugeWave` | реальный, но часто intentional variant duplication | решать policy отдельно |
| `folly` raw | tests first | `*Test.cpp` | mixed | `test` для app-only |
| `folly` filtered | twin implementations | `SpookyHashV1 <-> V2`, `F14Map <-> F14Set` | реальный, но intentional twin code | policy отдельно |
| `Kartend` | app/module duplication | `mainwindow_*`, `scanservice*`, `scraperesultdialogunified*` | реальный ручной дубль | маски не нужны |
| `LibreSprite` | command/UI duplication | `cmd_move_mask <-> cmd_scroll`, `copy_cel <-> move_cel` | реальный ручной дубль | маски не нужны |
| `GWToolboxpp` | app/plugin duplication + one vendor hit | `TextUtils <-> PluginUtils`, `GuiUtils <-> PluginUtils` | в основном реальный ручной дубль | optional `Dependencies` |
| `vmecpp` | tests first, then parallel implementations | `fast_poloidal_test <-> fast_toroidal_test`, `dft_toroidal <-> fft_toroidal` | mixed | `tests` для app-only |
| `moqx` | wrapper/server variants | `MoqxPicoRelayServer <-> MoqxRelayServer` | реальный ручной дубль | маски не нужны |

## До / после масок

Ниже — несколько показательных прогонов.

### `Catch2`

- raw: `75.68%`, `304` cross-file
- `--exclude extras`: `6.27%`, `39` cross-file

Вывод: почти весь шум — это amalgam.

### `nlohmann_json`

- raw: `63.14%`, `471` cross-file
- `--exclude single_include`: `51.04%`, `915` cross-file
- `--exclude single_include --exclude 'tests/abi/*' --exclude thirdparty`: `37.53%`, `374` cross-file

Вывод: single-header снимается, но остаётся очень много test duplication.
Если нужен app/library-only сигнал, придётся исключать и часть `tests/*`.

### `samastra_itain`

- raw: `5.68%`, `743` cross-file
- `--exclude build-* --exclude *_autogen --exclude qrc_* --exclude moc_* --exclude CMakeFiles`:
  `14.20%`, `624` cross-file

Вывод: build-noise исчезает, но под ним остаётся уже реальный продуктовый
дубль (`bat_diagwindow`, `imr_diagwindow`, `gear_*`).

### `pico-sdk`

- raw: `42.05%`, `1021` cross-file
- `--exclude hardware_regs --exclude CMSIS`: `21.85%`, `285` cross-file

Вывод: половина сигнала — generated register/CMSIS code. Остаток всё равно
сильно завязан на hardware family variants.

### `BambuStudio`

- raw: `15.41%`, `3673` cross-file
- `--exclude minilzo --exclude qhull --exclude deps --exclude agg`:
  `13.49%`, `2582` cross-file

После vendor masks наверху уже видны реальные GUI duplicates:

- `GLGizmoAdvancedCut.cpp <-> GLGizmoColorCut.cpp`
- `Notebook.hpp <-> Tabbook.hpp`
- `3mf.cpp <-> bbs_3mf.cpp`

### `AetherSDR`

- raw top block: `tests/CAT_Flex_test.cpp <-> tests/CAT_TS-2000_test.cpp`
- `--exclude tests`: наверху остаются:
  - `ClientCompEditor.cpp <-> StripCompPanel.cpp`
  - `ClientEqEditor.cpp <-> StripEqPanel.cpp`
  - `StripWaveform.cpp <-> WaveformWidget.cpp`

Вывод: tests шумят, но без них сигнал очень предметный.

### `spectre`

- raw top block: test-vs-test
- `--exclude tests`: наверху остаются:
  - `GhValenciaDivClean/.../ReconstructWork.tpp`
    `<-> ValenciaDivClean/.../ReconstructWork.tpp`
  - `GaugePlaneWave.hpp <-> GaugeWave.hpp`

Вывод: это уже не generated шум, а реальная variant duplication в library code.

## Где точно generated/package duplication

Эти находки руками выглядят не как "человек наспамил дублей", а как нормальная
структура поставки/генерации:

- `Catch2`: `extras/catch_amalgamated.cpp/.hpp`
- `nlohmann_json`: `single_include/*`, `tests/abi/*`
- `spdlog` root: `build-cc/_deps/catch2-src/*`
- `gm` raw: `common/thirdparty/*`, `3rdparty/*`, `utils/upgrade/*`,
  `codegenTemplates/*`
- `samastra_itain` raw: `*_autogen/*`, `qrc_*`, `moc_*`, `CompilerId*`
- `pico-sdk`: `hardware_regs/*`, `hardware_structs/*`, `CMSIS/*`
- `IrredenEngine`: `glad/glad.h`, `glcorearb.h`
- `BambuStudio` raw: `minilzo`, `qhull`, `deps/GMP`, `agg`
- `cpparch` raw: `sandbox/drift_repos/*`

## Где остаётся реальный ручной дубль

Эти находки выглядят как обычный copy-paste в исходниках, не как автоген:

- `spdlog/include`: `daily_file_sink.h <-> hourly_file_sink.h`
- `spdlog/include`: `syslog_sink.h <-> systemd_sink.h`
- `gm_github`: `packetlogger.cpp <-> packetlogger.cpp`
- `gm_github`: device twins `p_light.cpp`, `p_brake.cpp`, `l_pnev.cpp`
- `AetherSDR` после `tests`-mask: `Client* <-> Strip*`, `Waveform*`
- `Kartend`: `mainwindow_*`, `scanservice*`, `scraperesultdialogunified*`
- `LibreSprite`: `cmd_move_mask.cpp <-> cmd_scroll.cpp`
- `GWToolboxpp`: `TextUtils.cpp <-> PluginUtils.cpp`
- `moqx`: `MoqxPicoRelayServer.h <-> MoqxRelayServer.h`
- `BambuStudio` после vendor masks: `GLGizmoAdvancedCut.cpp <-> GLGizmoColorCut.cpp`
- `spectre` после `tests`-mask: `GhValenciaDivClean <-> ValenciaDivClean`
- `OreStudio` после domain masks:
  `ores.http.core <-> ores.http.server`,
  `ores.qt <-> ores.qt.refdata/api/admin`

## Практический вывод

Для test-app режима я бы делал так:

1. Всегда включать жёсткие маски из раздела выше.
2. Держать отдельный app-only профиль, который дополнительно выключает
   `tests`, `examples`, `upgrade`, `hardware_regs`, `CMSIS` и подобные
   репо-специфические каталоги.
3. После этого руками смотреть только оставшиеся top blocks.

Именно в таком режиме spike начинает показывать не packaging artifact, а
реальные reuse holes.
