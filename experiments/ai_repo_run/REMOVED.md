# Убранные из корпуса репозитории (#054)

Клоны, на которых archcheck (дубли + граф) **ничего не нашёл** или нашёл
**только ложное** (вендоринг / fork-in-repo / data-blob / мета-агрегатор /
codegen / header-only-фантом). Удалены с диска для экономии места; данные
(сводки `runs_history/*.tsv`) сохранены — список воспроизводим скриптами
`classify_remove.py` → `do_remove.py`.

**Теги причин:** `false` — ложное срабатывание/артефакт; `no-signal` —
дубли плоско, циклов нет, связность не растёт (нет срабатывания).

Итого убрано: **116** реп, освобождено **~6167 МБ**.

## High-star (`_aidev_run`) — 29 реп

| репа | тег | МБ | причина |
|---|---|---:|---|
| CnC_Generals_Zero_Hour | false | 118 | fork-in-repo (Generals/ vs GeneralsMD/), 90% dup стабилен — дублирование-by-design, не дрейф (§7.5) |
| GSL | false | 3 | 12.5→0 — header-only реструктуризация, фантом-«починка», не реальный спад (§7.5) |
| GacUI | false | 918 | codegen, 70% dup стабилен (Δ-0.16) — генерёнка, не дрейф (§7.5) |
| ReP_AL-3D-Lawn-Mower | false | 180 | data-blob camera_index.h (546 строк hex) + ESP32 boilerplate — шум-класс #056, не код (§3,§7.5) |
| SDRPlusPlusBrown | false | 101 | 19.8→0 при nodes→1 — смена структуры/субпуть, фантом, не починка (§7.5) |
| mqtt_client | false | 1 | весь dup = header↔impl одной компоненты (2 файла) — cross-file proxy, не архитектурный дубль (README §6) |
| rii | false | 2 | dup был раздут вендоренным extern/sse2neon.h; после excludes — плоско 3.6→4.0, сигнала нет |
| FTXUI | no-signal | 9 | dup плоско 9% (Δ=+3.4), циклов нет, связность не растёт |
| Todour | no-signal | 2 | dup плоско 3% (Δ=+0.9), циклов нет, связность не растёт |
| WLED | no-signal | 50 | dup плоско 0% (Δ=+0.0), циклов нет, связность не растёт |
| gffcompare | no-signal | 56 | dup плоско 6% (Δ=+3.8), циклов нет, связность не растёт |
| guetzli | no-signal | 2 | dup плоско 5% (Δ=+0.0), циклов нет, связность не растёт |
| log4cplus | no-signal | 19 | dup плоско 9% (Δ=+4.1), циклов нет, связность не растёт |
| mazegenerator | no-signal | 4 | dup плоско 0% (Δ=+0.0), циклов нет, связность не растёт |
| onnxruntime-genai | no-signal | 118 | dup плоско 10% (Δ=+4.3), циклов нет, связность не растёт |
| pict | no-signal | 8 | dup плоско 6% (Δ=+0.1), циклов нет, связность не растёт |
| pkt2flow | no-signal | 1 | dup плоско 6% (Δ=+1.1), циклов нет, связность не растёт |
| qscripts | no-signal | 6 | dup плоско 1% (Δ=+0.8), циклов нет, связность не растёт |
| react-native-fast-io | no-signal | 5 | dup плоско 0% (Δ=+0.0), циклов нет, связность не растёт |
| rocky | no-signal | 15 | dup плоско 10% (Δ=-1.0), циклов нет, связность не растёт |
| skyrim-community-shaders | no-signal | 519 | dup плоско 6% (Δ=-4.1), циклов нет, связность не растёт |
| slang | no-signal | 42 | dup плоско 8% (Δ=+3.8), циклов нет, связность не растёт |
| spdlog | no-signal | 9 | dup плоско 0% (Δ=+0.0), циклов нет, связность не растёт |
| tesseract | no-signal | 69 | dup плоско 6% (Δ=-2.3), циклов нет, связность не растёт |
| wasp | no-signal | 73 | dup плоско 4% (Δ=+1.9), циклов нет, связность не растёт |
| workerd | no-signal | 84 | dup плоско 9% (Δ=+1.6), циклов нет, связность не растёт |
| x64dbg | no-signal | 68 | dup плоско 11% (Δ=+0.3), циклов нет, связность не растёт |
| z3 | no-signal | 122 | dup плоско 8% (Δ=-0.6), циклов нет, связность не растёт |
| zed-open-capture | no-signal | 3 | dup плоско 10% (Δ=-3.3), циклов нет, связность не растёт |

## Low-star (`_aidev_lowstar`) — 87 реп

| репа | тег | МБ | причина |
|---|---|---:|---|
| ggml-org-central | false | 159 | мета-агрегатор (llama.cpp+whisper.cpp+ggml в одной папке), вендоринг-агрегация, не одна репа (§3) |
| 3d_printer | no-signal | 116 | dup 9% (Δ=+6.8), циклов нет (nodes=17) — нет сигнала |
| 3dsLL1 | no-signal | 2 | dup 0% (Δ=+0.0), циклов нет (nodes=7) — нет сигнала |
| BMI-Calculator | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=2) — нет сигнала |
| ConfigurableMetasoundMixer | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=4) — нет сигнала |
| Core | no-signal | 39 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| EDF-Tempo-E-Ink-Display | no-signal | 2 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| Engine2D | no-signal | 2 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| FluidNC | no-signal | 486 | dup 6% (Δ=+4.3), циклов нет (nodes=427) — нет сигнала |
| GTA-GPS-Redux | no-signal | 7 | dup 6% (Δ=+1.5), циклов нет (nodes=9) — нет сигнала |
| GZSelaco | no-signal | 228 | dup 10% (Δ=+1.6), циклов нет (nodes=1134) — нет сигнала |
| Galileo | no-signal | 353 | dup 8% (Δ=+4.9), циклов нет (nodes=66) — нет сигнала |
| HNanoSolver | no-signal | 5 | dup 12% (Δ=+3.7), циклов нет (nodes=18) — нет сигнала |
| InterObfu | no-signal | 2 | dup 4% (Δ=+3.7), циклов нет (nodes=18) — нет сигнала |
| JuncTek_Batterymonitor | no-signal | 6 | dup 0% (Δ=+0.0), циклов нет (nodes=2) — нет сигнала |
| KinovaArmController | no-signal | 4 | dup 6% (Δ=+5.9), циклов нет (nodes=6) — нет сигнала |
| LLMCardputer | no-signal | 1 | dup 2% (Δ=+2.4), циклов нет (nodes=4) — нет сигнала |
| MIDIPlusPlus | no-signal | 3 | dup 14% (Δ=-0.4), циклов нет (nodes=32) — нет сигнала |
| MagicPixelEngine | no-signal | 2 | dup 5% (Δ=-0.6), циклов нет (nodes=85) — нет сигнала |
| MicroChess | no-signal | 52 | dup 3% (Δ=+3.4), циклов нет (nodes=16) — нет сигнала |
| NepTrain | no-signal | 6 | dup 12% (Δ=+4.1), циклов нет (nodes=9) — нет сигнала |
| NepTrainKit | no-signal | 98 | dup 11% (Δ=+3.0), циклов нет (nodes=11) — нет сигнала |
| Operating-Systems-Ex4 | no-signal | 3 | dup 6% (Δ=+6.1), циклов нет (nodes=6) — нет сигнала |
| PGN-Spy | no-signal | 2 | dup 3% (Δ=+1.9), циклов нет (nodes=24) — нет сигнала |
| RED-Safe | no-signal | 70 | dup 11% (Δ=+2.6), циклов нет (nodes=9) — нет сигнала |
| RESTinCurl | no-signal | 2 | dup 3% (Δ=+3.0), циклов нет (nodes=16) — нет сигнала |
| RaylibLiveWallpaper | no-signal | 21 | dup 0% (Δ=+0.0), циклов нет (nodes=3) — нет сигнала |
| SandBox-2D | no-signal | 10 | dup 9% (Δ=+3.2), циклов нет (nodes=10) — нет сигнала |
| Slither | no-signal | 4 | dup 12% (Δ=+0.0), циклов нет (nodes=17) — нет сигнала |
| SnapKey | no-signal | 27 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| SpeedTestGamer | no-signal | 1 | dup 3% (Δ=-3.0), циклов нет (nodes=2) — нет сигнала |
| TTGO-T-HIGrow | no-signal | 31 | dup 3% (Δ=-1.3), циклов нет (nodes=13) — нет сигнала |
| TagManager2 | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| Talk-to-AI | no-signal | 2 | dup 0% (Δ=+0.0), циклов нет (nodes=16) — нет сигнала |
| TaterClient-ddnet | no-signal | 297 | dup 8% (Δ=+4.3), циклов нет (nodes=823) — нет сигнала |
| TempleWare-External | no-signal | 47 | dup 4% (Δ=+3.6), циклов нет (nodes=40) — нет сигнала |
| TermGraph | no-signal | 12 | dup 6% (Δ=+5.0), циклов нет (nodes=114) — нет сигнала |
| UltimateMangaReaderCodexFix | no-signal | 6 | dup 11% (Δ=-0.1), циклов нет (nodes=127) — нет сигнала |
| Webserv-karen | no-signal | 129 | dup 2% (Δ=+1.6), циклов нет (nodes=24) — нет сигнала |
| adminweb | no-signal | 2 | dup 0% (Δ=+0.0), циклов нет (nodes=16) — нет сигнала |
| airmouse | no-signal | 1 | dup 7% (Δ=-0.2), циклов нет (nodes=30) — нет сигнала |
| almo | no-signal | 10 | dup 11% (Δ=-4.7), циклов нет (nodes=38) — нет сигнала |
| alpaca_zorro_plugin | no-signal | 16 | dup 8% (Δ=+6.2), циклов нет (nodes=94) — нет сигнала |
| anklang | no-signal | 49 | dup 6% (Δ=+2.0), циклов нет (nodes=1930) — нет сигнала |
| arduino-ten | no-signal | 9 | dup 0% (Δ=+0.0), циклов нет (nodes=2) — нет сигнала |
| arduino-uno-aws-irrigation-system | no-signal | 12 | dup 4% (Δ=+3.6), циклов нет (nodes=2) — нет сигнала |
| argparse | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=8) — нет сигнала |
| awesome-low-level-design | no-signal | 18 | dup 7% (Δ=-0.9), циклов нет (nodes=473) — нет сигнала |
| bash-clipboard | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| bitcoin | no-signal | 186 | dup 7% (Δ=+1.3), циклов нет (nodes=1333) — нет сигнала |
| bpf_conformance | no-signal | 4 | dup 9% (Δ=+0.0), циклов нет (nodes=14) — нет сигнала |
| c--coroutine-load-balancer | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=4) — нет сигнала |
| casual | no-signal | 45 | dup 13% (Δ=+5.4), циклов нет (nodes=869) — нет сигнала |
| circlery | no-signal | 3 | dup 0% (Δ=+0.0), циклов нет (nodes=0) — нет сигнала |
| cyrsoxs | no-signal | 12 | dup 0% (Δ=+0.0), циклов нет (nodes=3) — нет сигнала |
| desesc | no-signal | 8 | dup 10% (Δ=-3.5), циклов нет (nodes=156) — нет сигнала |
| donner | no-signal | 164 | dup 12% (Δ=+5.3), циклов нет (nodes=1652) — нет сигнала |
| electron | no-signal | 83 | dup 6% (Δ=+1.5), циклов нет (nodes=729) — нет сигнала |
| glu | no-signal | 10 | dup 7% (Δ=+6.9), циклов нет (nodes=90) — нет сигнала |
| hf-utils-canopen | no-signal | 1 | dup 9% (Δ=-2.4), циклов нет (nodes=2) — нет сигнала |
| ifc | no-signal | 3 | dup 1% (Δ=-0.0), циклов нет (nodes=23) — нет сигнала |
| inversify-cpp | no-signal | 2 | dup 7% (Δ=+7.4), циклов нет (nodes=36) — нет сигнала |
| iohomecontrol | no-signal | 4 | dup 13% (Δ=+5.2), циклов нет (nodes=25) — нет сигнала |
| keypad | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=0) — нет сигнала |
| kmtest | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=8) — нет сигнала |
| marine_anchormate_esp | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=1) — нет сигнала |
| nnstreamer | no-signal | 200 | dup 12% (Δ=-7.5), циклов нет (nodes=287) — нет сигнала |
| note-c | no-signal | 6 | dup 8% (Δ=+0.2), циклов нет (nodes=201) — нет сигнала |
| obs-airplay | no-signal | 1 | dup 3% (Δ=+3.1), циклов нет (nodes=8) — нет сигнала |
| openspaceprogram | no-signal | 4 | dup 5% (Δ=+4.1), циклов нет (nodes=25) — нет сигнала |
| pd4web | no-signal | 216 | dup 14% (Δ=+7.3), циклов нет (nodes=22) — нет сигнала |
| personal-scheduler | no-signal | 9 | dup 13% (Δ=+7.1), циклов нет (nodes=110) — нет сигнала |
| photobroom | no-signal | 17 | dup 2% (Δ=+2.3), циклов нет (nodes=472) — нет сигнала |
| radio_tool | no-signal | 3 | dup 9% (Δ=-3.8), циклов нет (nodes=21) — нет сигнала |
| seviye1 | no-signal | 2 | dup 0% (Δ=+0.0), циклов нет (nodes=16) — нет сигнала |
| sfs-client | no-signal | 3 | dup 8% (Δ=+3.6), циклов нет (nodes=102) — нет сигнала |
| skrypt | no-signal | 1 | dup 0% (Δ=+0.0), циклов нет (nodes=10) — нет сигнала |
| soap_dispanser_ha | no-signal | 1 | dup 1% (Δ=-0.1), циклов нет (nodes=34) — нет сигнала |
| teal-parser | no-signal | 2 | dup 9% (Δ=+2.7), циклов нет (nodes=7) — нет сигнала |
| ton | no-signal | 78 | dup 14% (Δ=+2.1), циклов нет (nodes=1332) — нет сигнала |
| uc2-esp32 | no-signal | 14 | dup 6% (Δ=+6.0), циклов нет (nodes=18) — нет сигнала |
| vad | no-signal | 102 | dup 7% (Δ=+0.0), циклов нет (nodes=34) — нет сигнала |
| video-viewer | no-signal | 1 | dup 9% (Δ=+2.8), циклов нет (nodes=5) — нет сигнала |
| video_fix | no-signal | 6 | dup 5% (Δ=+0.5), циклов нет (nodes=80) — нет сигнала |
| virtualgimbal_ros | no-signal | 2 | dup 4% (Δ=+3.9), циклов нет (nodes=14) — нет сигнала |
| yahat-cpp | no-signal | 2 | dup 1% (Δ=-0.5), циклов нет (nodes=4) — нет сигнала |
| yush | no-signal | 2 | dup 2% (Δ=+2.2), циклов нет (nodes=14) — нет сигнала |

