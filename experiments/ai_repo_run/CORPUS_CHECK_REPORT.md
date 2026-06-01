# Проверка корпуса AI-плотных C++ реп — агрегат + детали

**Дата:** 2026-06-01. Прогнан **весь локальный корпус: 343 AI-плотных C++ реп** (чекер: #056 копипаст + archcheck циклы). Когорта «год агентской разработки» (≥300 коммитов после мая 2025) — **112** реп. Окно `--diff` = последние 300 коммитов. generated-skip (#065) включён.

## Главные цифры (для статьи)

| | весь корпус | % | когорта ≥300/год | % |
|---|---|---|---|---|
| проверено | 343 | 100% | 112 | 100% |
| **clean — без дрейфа** | **208** | **61%** | **36** | **32%** |
| **drift — есть дрейф** | **135** | **39%** | **76** | **68%** |
| структурный (циклы>0) | 45 | 13% | 22 | 20% |
| partial (таймаут) | 0 | — | 0 | — |

Дрейф по доверию к сигналу:
- **Структурный: 45 реп (13%)** — include-цикл вне стандартных вендор-папок (#068: third_party/vendor/external/… исключены). ВЕРХНЯЯ ОЦЕНКА: часть реп вендорит под нестандартными именами (напр. `MacKernelSDK`) → ночная верификация (#067) отделит реальные src-циклы.
- **Копипаст-кандидаты (ручная проверка): 116 реп**, всего **3195 хитов** (within-file **1822**, cross-file **1373**). #056 ~16.5% precision → claims, не факт.

> Для статьи (ВЕРХНЯЯ оценка, ждёт верификации #067): **«до 39% AI-плотных C++ репозиториев показали признаки дрейфа за год; до 13% — структурный (include-цикл).»** 61% чистых по сырому чекеру.

## Топ по копипаст-хитам

| репа | коммитов | копипаст | within | cross | циклы |
|---|---|---|---|---|---|
| alexandrosk0/Smatchet | 983 | 303 | 0 | 303 | 0 |
| makr-code/ThemisDB | 12433 | 209 | 195 | 14 | 0 |
| elisir80/Decodium-4.0-Core-Shannon | 717 | 124 | 123 | 1 | 0 |
| hammermaps/halflife-op4-updated-sohl | 222 | 123 | 44 | 79 | 0 |
| alphaonex86/CatchChallenger | 301 | 121 | 27 | 94 | 0 |
| aethersdr/AetherSDR | 2003 | 109 | 103 | 6 | 0 |
| Krilliac/SparkEngine | 2229 | 106 | 33 | 73 | 1 |
| starseeker/obol | 525 | 94 | 11 | 83 | 4 |
| GregorGullwi/FlashCpp | 2930 | 90 | 85 | 5 | 4 |
| fuddlesworth/PlasmaZones | 3757 | 89 | 39 | 50 | 0 |

## Топ по include-циклам (структурный дрейф)

| репа | циклов | крупнейший SCC | копипаст |
|---|---|---|---|
| victorwitkamp/XePCI | 18 | 5 | 0 |
| ericflo/kiln | 11 | 14 | 0 |
| brontoguana/krasis | 10 | 13 | 0 |
| AztecProtocol/aztec-packages | 5 | 6 | 20 |
| GregorGullwi/FlashCpp | 4 | 8 | 90 |
| starseeker/obol | 4 | 6 | 94 |
| andrele/perse_rover-firmware-microros | 4 | 5 | 0 |
| andrewnakas/Boxedwine64 | 3 | 5 | 19 |

## Копипаст-кандидаты по уверенности × scope

| | within-file (в одном файле) | cross-file (между файлами) |
|---|---|---|
| **verbatim** (line≥0.95, почти дословно) | 264 | 563 |
| **near** (line 0.8–0.95) | 552 | 206 |
| **diverged** (line<0.8, сильно правили) | 1006 | 604 |

> **verbatim within-file** = «скопировал функцию в том же файле» — целевой сигнал, высокая точность.
> **diverged** — самый шумный класс (может быть coincidental), главный кандидат на FP.


## Оговорки
- Окно `--diff` 300 коммитов: для огромных реп ранний копипаст вне окна → счётчик занижен.
- Копипаст per-commit атрибуция шумная; сами пары `file:line` точны — смотреть их.
- Полный per-repo вердикт: `corpus_check_summary.tsv`. Детали по коммитам — ниже.

---

# Детальный отчёт проверки корпуса (per-commit, для ручной проверки)

> Окно --diff: последние 300 коммитов на репу. Копипаст = КАНДИДАТЫ (#056, ~16.5% precision — глазами!). Циклы = надёжно (archcheck).
> Открыть коммит: команда `inspect:` под каждым.

## AztecProtocol/aztec-packages — 12813 коммитов | циклы=5(SCC≤6) | копипаст=20
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 5 include-циклов, крупнейший SCC=6. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/AztecProtocol_aztec-packages`

### `8f347013715f` — "fix: include standard-contracts artifacts in release image (#23753)" (Gregorio Juliana, 2026-05-31) [20 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/AztecProtocol_aztec-packages show 8f347013715f`
  - [near within-file w=1.00 line=0.89] `barretenberg/cpp/src/barretenberg/vm2/tracegen/execution_trace.cpp:489-509` ⟵ `barretenberg/cpp/src/barretenberg/vm2/tracegen/execution_trace.cpp:462-482`
  - [near cross-file w=1.00 line=0.83] `barretenberg/cpp/src/barretenberg/benchmark/ultra_bench/mega_honk.bench.cpp:97-109` ⟵ `barretenberg/cpp/src/barretenberg/benchmark/ultra_bench/ultra_honk.bench.cpp:115-129`
  - [near within-file w=1.00 line=0.94] `barretenberg/cpp/src/barretenberg/chonk/chonk.cpp:677-721` ⟵ `barretenberg/cpp/src/barretenberg/chonk/chonk.cpp:607-651`
  - [near cross-file w=1.00 line=0.81] `barretenberg/cpp/src/barretenberg/wsdb/wsdb_ipc_server.cpp:40-66` ⟵ `barretenberg/cpp/src/barretenberg/api/api_msgpack.cpp:112-146`
  - [near within-file w=0.98 line=0.83] `barretenberg/cpp/src/barretenberg/ecc/scalar_multiplication/scalar_multiplication.test.cpp:319-351` ⟵ `barretenberg/cpp/src/barretenberg/ecc/scalar_multiplication/scalar_multiplication.test.cpp:238-270`
  - [diverged within-file w=0.96 line=0.22] `barretenberg/cpp/src/barretenberg/ecc/groups/affine_element.test.cpp:323-327` ⟵ `barretenberg/cpp/src/barretenberg/ecc/groups/affine_element.test.cpp:380-387`
  - [near within-file w=0.92 line=0.94] `barretenberg/cpp/src/barretenberg/ecc/scalar_multiplication/scalar_multiplication.cpp:521-540` ⟵ `barretenberg/cpp/src/barretenberg/ecc/scalar_multiplication/scalar_multiplication.cpp:458-476`
  - [near within-file w=0.90 line=0.86] `barretenberg/cpp/src/barretenberg/hypernova/hypernova_prover.cpp:163-172` ⟵ `barretenberg/cpp/src/barretenberg/hypernova/hypernova_prover.cpp:147-155`
  - [diverged within-file w=0.86 line=0.66] `barretenberg/cpp/src/barretenberg/vm2/simulation/gadgets/tx_execution.cpp:186-224` ⟵ `barretenberg/cpp/src/barretenberg/vm2/simulation/gadgets/tx_execution.cpp:99-131`
  - [diverged cross-file w=0.83 line=0.48] `barretenberg/cpp/src/barretenberg/op_queue/ecc_ops_table.hpp:235-258` ⟵ `barretenberg/cpp/src/barretenberg/op_queue/ecc_op_queue.hpp:319-348`
  - [diverged within-file w=0.79 line=0.64] `barretenberg/cpp/src/barretenberg/ipc/socket_server.cpp:392-460` ⟵ `barretenberg/cpp/src/barretenberg/ipc/socket_server.cpp:187-251`
  - [diverged within-file w=0.79 line=0.64] `barretenberg/cpp/src/barretenberg/ipc/socket_server.cpp:188-255` ⟵ `barretenberg/cpp/src/barretenberg/ipc/socket_server.cpp:388-453`
  - [diverged within-file w=0.78 line=0.67] `barretenberg/cpp/src/barretenberg/chonk/proof_compression.hpp:582-590` ⟵ `barretenberg/cpp/src/barretenberg/chonk/proof_compression.hpp:563-568`
  - [diverged within-file w=0.74 line=0.73] `barretenberg/cpp/src/barretenberg/bb/cli.cpp:1016-1076` ⟵ `barretenberg/cpp/src/barretenberg/bb/cli.cpp:968-1013`
  - [diverged within-file w=0.72 line=0.69] `barretenberg/cpp/src/barretenberg/chonk/chonk.cpp:628-664` ⟵ `barretenberg/cpp/src/barretenberg/chonk/chonk.cpp:555-594`
  - [diverged within-file w=0.69 line=0.53] `barretenberg/cpp/src/barretenberg/op_queue/ecc_ops_table.test.cpp:344-377` ⟵ `barretenberg/cpp/src/barretenberg/op_queue/ecc_ops_table.test.cpp:413-455`
  - [diverged cross-file w=0.67 line=0.37] `barretenberg/cpp/src/barretenberg/goblin/goblin.cpp:71-83` ⟵ `barretenberg/cpp/src/barretenberg/goblin_avm/goblin_avm.cpp:51-68`
  - [diverged within-file w=0.65 line=0.42] `barretenberg/cpp/src/barretenberg/stdlib/primitives/biggroup/biggroup.test.cpp:1362-1406` ⟵ `barretenberg/cpp/src/barretenberg/stdlib/primitives/biggroup/biggroup.test.cpp:1411-1441`
  - [diverged within-file w=0.65 line=0.68] `barretenberg/cpp/src/barretenberg/bb/cli.cpp:1077-1106` ⟵ `barretenberg/cpp/src/barretenberg/bb/cli.cpp:1014-1033`
  - [diverged within-file w=0.63 line=0.70] `barretenberg/cpp/src/barretenberg/honk/execution_trace/execution_trace_block.hpp:318-330` ⟵ `barretenberg/cpp/src/barretenberg/honk/execution_trace/execution_trace_block.hpp:281-290`

## makr-code/ThemisDB — 12433 коммитов | циклы=0(SCC≤1) | копипаст=209

### `8dbcebff06d0` — "Refactor and enhance Utils module documentation and security measures" (makr-code, 2026-05-31) [209 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/makr-code_ThemisDB show 8dbcebff06d0`
  - [diverged within-file w=1.00 line=0.36] `src/auth/mtls_authenticator.cpp:284-307` ⟵ `src/auth/mtls_authenticator.cpp:312-345`
  - [diverged within-file w=1.00 line=0.60] `src/content/archive_processor.cpp:784-795` ⟵ `src/content/archive_processor.cpp:532-545`
  - [diverged within-file w=1.00 line=0.71] `src/config/config_schema_validator.cpp:437-452` ⟵ `src/config/config_schema_validator.cpp:399-414`
  - [diverged within-file w=1.00 line=0.46] `src/config/config_file_watcher.cpp:462-479` ⟵ `src/config/config_file_watcher.cpp:388-408`
  - [verbatim within-file w=1.00 line=1.00] `src/cache/distributed_cache_coordinator.cpp:691-700` ⟵ `src/cache/distributed_cache_coordinator.cpp:611-620`
  - [diverged within-file w=1.00 line=0.67] `src/cache/distributed_cache_coordinator.cpp:540-561` ⟵ `src/cache/distributed_cache_coordinator.cpp:460-483`
  - [diverged within-file w=1.00 line=0.75] `src/cache/distributed_cache_coordinator.cpp:424-455` ⟵ `src/cache/distributed_cache_coordinator.cpp:352-382`
  - [verbatim within-file w=1.00 line=1.00] `src/cache/distributed_cache_coordinator.cpp:343-367` ⟵ `src/cache/distributed_cache_coordinator.cpp:269-293`
  - [near within-file w=1.00 line=0.88] `src/cache/distributed_cache_coordinator.cpp:309-340` ⟵ `src/cache/distributed_cache_coordinator.cpp:233-265`
  - [diverged within-file w=1.00 line=0.54] `src/auth/webauthn_authenticator.cpp:755-766` ⟵ `src/auth/webauthn_authenticator.cpp:734-747`
  - [diverged within-file w=1.00 line=0.57] `src/auth/saml_authenticator.cpp:850-861` ⟵ `src/auth/saml_authenticator.cpp:883-897`
  - [verbatim within-file w=1.00 line=1.00] `src/auth/saml_authenticator.cpp:685-692` ⟵ `src/auth/saml_authenticator.cpp:696-703`
  - [diverged within-file w=1.00 line=0.59] `src/auth/oauth_pkce_flow.cpp:79-100` ⟵ `src/auth/oauth_pkce_flow.cpp:109-131`
  - [diverged within-file w=1.00 line=0.36] `src/auth/mtls_authenticator.cpp:342-348` ⟵ `src/auth/mtls_authenticator.cpp:383-392`
  - [near within-file w=1.00 line=0.89] `src/content/async_ingestion_worker.cpp:693-726` ⟵ `src/content/async_ingestion_worker.cpp:741-775`
  - [diverged within-file w=1.00 line=0.31] `src/auth/mtls_authenticator.cpp:267-275` ⟵ `src/auth/mtls_authenticator.cpp:293-302`
  - [diverged within-file w=1.00 line=0.06] `src/auth/mtls_authenticator.cpp:219-225` ⟵ `src/auth/mtls_authenticator.cpp:236-247`
  - [verbatim within-file w=1.00 line=1.00] `src/auth/kerberos_security.cpp:417-457` ⟵ `src/auth/kerberos_security.cpp:138-178`
  - [diverged within-file w=1.00 line=0.77] `src/auth/kerberos_security.cpp:361-405` ⟵ `src/auth/kerberos_security.cpp:75-122`
  - [verbatim within-file w=1.00 line=1.00] `src/auth/jwt_validator.cpp:817-827` ⟵ `src/auth/jwt_validator.cpp:738-748`
  - [diverged within-file w=1.00 line=0.57] `src/auth/auth_rate_limiter.cpp:638-650` ⟵ `src/auth/auth_rate_limiter.cpp:672-686`
  - [diverged within-file w=1.00 line=0.17] `src/aql/llm_aql_handler.cpp:1617-1630` ⟵ `src/aql/llm_aql_handler.cpp:1476-1494`
  - [diverged within-file w=1.00 line=0.24] `src/aql/llm_aql_handler.cpp:1598-1611` ⟵ `src/aql/llm_aql_handler.cpp:1450-1467`
  - [diverged within-file w=1.00 line=0.79] `src/aql/aql_lora_finetuner.cpp:650-682` ⟵ `src/aql/aql_lora_finetuner.cpp:728-763`
  - [near within-file w=1.00 line=0.86] `src/aql/aql_fewshot_example_library.cpp:106-120` ⟵ `src/aql/aql_fewshot_example_library.cpp:129-143`
  - [diverged within-file w=1.00 line=0.75] `src/aql/aql_conversation_context.cpp:257-273` ⟵ `src/aql/aql_conversation_context.cpp:275-293`
  - [verbatim within-file w=1.00 line=1.00] `src/api/graphql.cpp:1198-1212` ⟵ `src/api/graphql.cpp:1168-1182`
  - [diverged within-file w=1.00 line=0.71] `src/api/graphql.cpp:791-799` ⟵ `src/api/graphql.cpp:806-814`
  - [verbatim cross-file w=1.00 line=1.00] `src/scraper/scraper_plugin.cpp:522-572` ⟵ `plugins/scraper/src/scraper_plugin.cpp:536-586`
  - [near within-file w=1.00 line=0.92] `src/llm/lora_framework/lora_training_service.cpp:819-852` ⟵ `src/llm/lora_framework/lora_training_service.cpp:774-807`
  - [verbatim cross-file w=1.00 line=1.00] `src/scraper/scraper_config.cpp:111-132` ⟵ `plugins/scraper/src/scraper_config.cpp:124-145`
  - [verbatim within-file w=1.00 line=1.00] `src/importers/deterministic_matcher.cpp:332-337` ⟵ `src/importers/deterministic_matcher.cpp:321-326`
  - [diverged within-file w=1.00 line=0.33] `src/graph/gpu_traversal.cpp:488-500` ⟵ `src/graph/gpu_traversal.cpp:842-858`
  - [diverged within-file w=1.00 line=0.33] `src/graph/gpu_traversal.cpp:464-476` ⟵ `src/graph/gpu_traversal.cpp:810-826`
  - [diverged within-file w=1.00 line=0.57] `src/gpu/query_accelerator.cpp:980-996` ⟵ `src/gpu/query_accelerator.cpp:1002-1020`
  - [near within-file w=1.00 line=0.85] `src/governance/policy_review.cpp:903-926` ⟵ `src/governance/policy_review.cpp:866-890`
  - [verbatim cross-file w=1.00 line=1.00] `src/scraper/scraper_metadata_writer.cpp:83-101` ⟵ `plugins/scraper/src/scraper_metadata_writer.cpp:97-115`
  - [verbatim cross-file w=1.00 line=1.00] `src/scraper/scraper_plugin.cpp:369-430` ⟵ `plugins/scraper/src/scraper_plugin.cpp:383-444`
  - [diverged within-file w=1.00 line=0.50] `src/geo/gpu_backend_stub.cpp:235-252` ⟵ `src/geo/gpu_backend_stub.cpp:261-282`
  - [diverged within-file w=1.00 line=0.73] `src/geo/gpu_backend_stub.cpp:176-195` ⟵ `src/geo/gpu_backend_stub.cpp:203-224`
  - [diverged within-file w=1.00 line=0.35] `src/geo/gpu_backend_stub.cpp:102-116` ⟵ `src/geo/gpu_backend_stub.cpp:121-140`
  - [diverged within-file w=1.00 line=0.50] `src/geo/gpu_backend_production.cpp:781-784` ⟵ `src/geo/gpu_backend_production.cpp:767-771`
  - [diverged within-file w=1.00 line=0.27] `plugins/scraper/src/scraper_config.cpp:156-183` ⟵ `plugins/scraper/src/scraper_config.cpp:124-145`
  - [diverged within-file w=1.00 line=0.73] `src/exporters/streaming_exporter.cpp:79-89` ⟵ `src/exporters/streaming_exporter.cpp:98-109`
  - [near within-file w=1.00 line=0.94] `src/exporters/jsonl_llm_exporter.cpp:673-726` ⟵ `src/exporters/jsonl_llm_exporter.cpp:757-810`
  - [diverged within-file w=1.00 line=0.61] `src/exporters/jsonl_llm_exporter.cpp:241-267` ⟵ `src/exporters/jsonl_llm_exporter.cpp:274-303`
  - [verbatim within-file w=1.00 line=1.00] `src/exporters/huggingface_exporter.cpp:102-111` ⟵ `src/exporters/huggingface_exporter.cpp:123-132`
  - [near within-file w=1.00 line=0.92] `src/exporters/export_encryption.cpp:584-615` ⟵ `src/exporters/export_encryption.cpp:671-702`
  - [diverged within-file w=1.00 line=0.75] `src/exporters/data_augmentation.cpp:307-332` ⟵ `src/exporters/data_augmentation.cpp:347-374`
  - [verbatim within-file w=1.00 line=0.96] `src/security/timestamp_authority.cpp:998-1065` ⟵ `src/security/timestamp_authority.cpp:888-955`
  - [diverged within-file w=1.00 line=0.55] `src/exporters/arrow_ipc_exporter.cpp:701-708` ⟵ `src/exporters/arrow_ipc_exporter.cpp:723-733`
  - [diverged within-file w=1.00 line=0.55] `src/content/video_processor.cpp:833-843` ⟵ `src/content/video_processor.cpp:821-832`
  - [near within-file w=1.00 line=0.91] `src/content/stt_processor.cpp:788-814` ⟵ `src/content/stt_processor.cpp:834-860`
  - [near within-file w=1.00 line=0.80] `src/content/stt_processor.cpp:426-452` ⟵ `src/content/stt_processor.cpp:464-490`
  - [verbatim within-file w=1.00 line=1.00] `src/content/stt_processor.cpp:322-333` ⟵ `src/content/stt_processor.cpp:355-366`
  - [verbatim within-file w=1.00 line=1.00] `src/analytics/anomaly_detection.cpp:624-633` ⟵ `src/analytics/anomaly_detection.cpp:574-583`
  - [diverged within-file w=1.00 line=0.52] `src/acceleration/plugin_security.cpp:1003-1031` ⟵ `src/acceleration/plugin_security.cpp:1054-1086`
  - [verbatim within-file w=1.00 line=1.00] `src/analytics/columnar_execution.cpp:757-775` ⟵ `src/analytics/columnar_execution.cpp:656-674`
  - [diverged within-file w=1.00 line=0.71] `src/analytics/columnar_execution.cpp:732-752` ⟵ `src/analytics/columnar_execution.cpp:633-651`
  - [diverged within-file w=1.00 line=0.21] `src/acceleration/tensor_core_matmul.cpp:87-101` ⟵ `src/acceleration/tensor_core_matmul.cpp:108-145`
  - [verbatim within-file w=1.00 line=1.00] `src/analytics/columnar_execution.cpp:717-721` ⟵ `src/analytics/columnar_execution.cpp:618-622`
  - [diverged within-file w=1.00 line=0.43] `src/analytics/columnar_execution.cpp:698-714` ⟵ `src/analytics/columnar_execution.cpp:603-615`
  - [diverged within-file w=1.00 line=0.40] `src/analytics/anomaly_detection.cpp:579-605` ⟵ `src/analytics/anomaly_detection.cpp:535-555`
  - [near within-file w=1.00 line=0.80] `src/analytics/cep_engine.cpp:1433-1455` ⟵ `src/analytics/cep_engine.cpp:1275-1297`
  - [verbatim within-file w=1.00 line=1.00] `src/analytics/forecasting.cpp:310-315` ⟵ `src/analytics/forecasting.cpp:282-287`
  - [diverged within-file w=1.00 line=0.44] `src/analytics/cep_engine.cpp:977-1006` ⟵ `src/analytics/cep_engine.cpp:867-886`
  - [verbatim within-file w=1.00 line=1.00] `src/analytics/anomaly_detection.cpp:1208-1223` ⟵ `src/analytics/anomaly_detection.cpp:1095-1110`
  - [near within-file w=1.00 line=0.81] `src/analytics/anomaly_detection.cpp:1231-1258` ⟵ `src/analytics/anomaly_detection.cpp:1118-1146`
  - [near within-file w=1.00 line=0.91] `src/analytics/cep_engine.cpp:463-495` ⟵ `src/analytics/cep_engine.cpp:396-428`
  - [diverged within-file w=1.00 line=0.52] `src/analytics/arrow_flight.cpp:403-436` ⟵ `src/analytics/arrow_flight.cpp:441-477`
  - [diverged within-file w=1.00 line=0.37] `src/analytics/arrow_flight.cpp:770-804` ⟵ `src/analytics/arrow_flight.cpp:849-885`
  - [diverged within-file w=1.00 line=0.32] `src/analytics/arrow_flight.cpp:865-909` ⟵ `src/analytics/arrow_flight.cpp:949-996`
  - [diverged within-file w=1.00 line=0.25] `src/acceleration/ai_hardware_dispatcher.cpp:287-311` ⟵ `src/acceleration/ai_hardware_dispatcher.cpp:225-241`
  - [diverged within-file w=1.00 line=0.64] `src/analytics/olap.cpp:1103-1112` ⟵ `src/analytics/olap.cpp:984-991`
  - [diverged within-file w=1.00 line=0.64] `src/analytics/olap.cpp:1062-1071` ⟵ `src/analytics/olap.cpp:952-959`
  - [diverged within-file w=1.00 line=0.67] `src/analytics/olap.cpp:1029-1036` ⟵ `src/analytics/olap.cpp:924-930`
  - [near within-file w=1.00 line=0.82] `src/analytics/olap.cpp:753-790` ⟵ `src/analytics/olap.cpp:693-731`
  - [near within-file w=1.00 line=0.91] `src/analytics/jit_aggregation.cpp:398-429` ⟵ `src/analytics/jit_aggregation.cpp:366-397`
  - [verbatim within-file w=1.00 line=1.00] `src/analytics/forecasting.cpp:337-342` ⟵ `src/analytics/forecasting.cpp:310-315`
  - [diverged within-file w=1.00 line=0.75] `src/acceleration/ai_hardware_dispatcher.cpp:645-688` ⟵ `src/acceleration/ai_hardware_dispatcher.cpp:544-589`
  - [diverged within-file w=1.00 line=0.59] `src/analytics/forecasting.cpp:2026-2042` ⟵ `src/analytics/forecasting.cpp:1281-1298`
  - [diverged within-file w=1.00 line=0.62] `src/acceleration/ai_hardware_dispatcher.cpp:810-815` ⟵ `src/acceleration/ai_hardware_dispatcher.cpp:696-702`
  - [diverged within-file w=1.00 line=0.61] `src/acceleration/ai_hardware_dispatcher.cpp:868-882` ⟵ `src/acceleration/ai_hardware_dispatcher.cpp:759-774`
  - [diverged within-file w=1.00 line=0.39] `src/acceleration/cuda_backend.cpp:769-790` ⟵ `src/acceleration/cuda_backend.cpp:944-968`
  - [diverged within-file w=1.00 line=0.53] `src/acceleration/plugin_security.cpp:749-784` ⟵ `src/acceleration/plugin_security.cpp:793-828`
  - [diverged within-file w=1.00 line=0.59] `src/acceleration/plugin_security.cpp:888-904` ⟵ `src/acceleration/plugin_security.cpp:936-953`
  - [near within-file w=1.00 line=0.88] `src/network/wire_protocol_server.cpp:718-738` ⟵ `src/network/wire_protocol_server.cpp:469-489`
  - [diverged within-file w=1.00 line=0.78] `src/rag/http_metrics_client.cpp:223-231` ⟵ `src/rag/http_metrics_client.cpp:243-251`
  - [near within-file w=1.00 line=0.94] `src/auth/ldap_authenticator.cpp:490-529` ⟵ `src/auth/ldap_authenticator.cpp:450-489`
  - [near within-file w=1.00 line=0.90] `src/ingestion/legal_domain.cpp:353-379` ⟵ `src/ingestion/legal_domain.cpp:297-323`
  - [diverged within-file w=1.00 line=0.45] `src/analytics/arrow_flight.cpp:835-862` ⟵ `src/analytics/arrow_flight.cpp:915-945`
  - [near within-file w=1.00 line=0.83] `src/analytics/streaming_join.cpp:575-626` ⟵ `src/analytics/streaming_join.cpp:505-556`
  - [diverged within-file w=0.99 line=0.56] `src/analytics/arrow_flight.cpp:807-832` ⟵ `src/analytics/arrow_flight.cpp:888-912`
  - [diverged within-file w=0.99 line=0.79] `src/analytics/anomaly_detection.cpp:636-652` ⟵ `src/analytics/anomaly_detection.cpp:586-600`
  - [near within-file w=0.99 line=0.90] `src/security/timestamp_authority.cpp:905-930` ⟵ `src/security/timestamp_authority.cpp:795-820`
  - [diverged within-file w=0.99 line=0.47] `src/analytics/automl.cpp:856-885` ⟵ `src/analytics/automl.cpp:728-750`
  - [diverged within-file w=0.99 line=0.69] `src/api/otlp_exporter.cpp:525-556` ⟵ `src/api/otlp_exporter.cpp:478-509`
  - [diverged within-file w=0.99 line=0.74] `src/analytics/cep_engine.cpp:1706-1738` ⟵ `src/analytics/cep_engine.cpp:1533-1564`
  - [near within-file w=0.99 line=0.93] `src/analytics/automl.cpp:1330-1348` ⟵ `src/analytics/automl.cpp:1104-1121`
  - [diverged within-file w=0.99 line=0.49] `src/analytics/anomaly_detection.cpp:922-958` ⟵ `src/analytics/anomaly_detection.cpp:850-876`
  - [diverged within-file w=0.99 line=0.54] `src/analytics/automl.cpp:692-723` ⟵ `src/analytics/automl.cpp:590-618`
  - [diverged within-file w=0.99 line=0.56] `src/core/concerns/redis_cache.cpp:660-679` ⟵ `src/core/concerns/redis_cache.cpp:586-604`
  - [diverged within-file w=0.99 line=0.24] `src/auth/mtls_authenticator.cpp:351-374` ⟵ `src/auth/mtls_authenticator.cpp:395-417`
  - [diverged within-file w=0.99 line=0.47] `src/analytics/automl.cpp:2169-2203` ⟵ `src/analytics/automl.cpp:1707-1743`
  - [near within-file w=0.99 line=0.82] `src/llm/vision_resource_monitor.cpp:658-681` ⟵ `src/llm/vision_resource_monitor.cpp:636-657`
  - [diverged within-file w=0.99 line=0.73] `src/core/concerns/redis_cache.cpp:867-880` ⟵ `src/core/concerns/redis_cache.cpp:749-760`
  - [diverged within-file w=0.98 line=0.35] `src/analytics/arrow_flight.cpp:689-701` ⟵ `src/analytics/arrow_flight.cpp:769-782`
  - [diverged within-file w=0.98 line=0.38] `src/analytics/columnar_execution.cpp:920-960` ⟵ `src/analytics/columnar_execution.cpp:803-841`
  - [diverged within-file w=0.98 line=0.67] `src/aql/aql_confidence_scorer.cpp:199-207` ⟵ `src/aql/aql_confidence_scorer.cpp:228-234`
  - [near within-file w=0.98 line=0.90] `src/server/http_server.cpp:2183-2255` ⟵ `src/server/http_server.cpp:1953-2022`
  - [near within-file w=0.98 line=0.91] `src/server/http_server.cpp:9884-9918` ⟵ `src/server/http_server.cpp:8183-8216`
  - [verbatim within-file w=0.98 line=0.97] `src/themis/wire_protocol_server.cpp:708-758` ⟵ `src/themis/wire_protocol_server.cpp:544-593`
  - [diverged within-file w=0.98 line=0.57] `src/analytics/incremental_view.cpp:484-512` ⟵ `src/analytics/incremental_view.cpp:447-468`
  - [diverged within-file w=0.98 line=0.51] `src/geo/cpu_backend.cpp:973-1040` ⟵ `src/geo/cpu_backend.cpp:857-908`
  - [diverged within-file w=0.98 line=0.58] `src/analytics/automl.cpp:746-779` ⟵ `src/analytics/automl.cpp:641-667`
  - [near within-file w=0.98 line=0.90] `src/server/http_server.cpp:9795-9826` ⟵ `src/server/http_server.cpp:8097-8127`
  - [diverged within-file w=0.98 line=0.35] `src/governance/policy_manager_versioned.cpp:375-399` ⟵ `src/governance/policy_manager_versioned.cpp:438-456`
  - [diverged within-file w=0.98 line=0.39] `src/analytics/automl.cpp:2005-2056` ⟵ `src/analytics/automl.cpp:1556-1595`
  - [diverged within-file w=0.98 line=0.44] `src/analytics/automl.cpp:1398-1437` ⟵ `src/analytics/automl.cpp:1179-1204`
  - [diverged within-file w=0.98 line=0.63] `src/analytics/anomaly_detection.cpp:824-849` ⟵ `src/analytics/anomaly_detection.cpp:763-784`
  - [diverged within-file w=0.98 line=0.23] `src/auth/jwt_validator.cpp:550-584` ⟵ `src/auth/jwt_validator.cpp:491-516`
  - [diverged within-file w=0.97 line=0.47] `src/aql/llm_aql_handler.cpp:1556-1592` ⟵ `src/aql/llm_aql_handler.cpp:1408-1442`
  - [diverged within-file w=0.97 line=0.45] `src/importers/mdm_audit_trail.cpp:113-126` ⟵ `src/importers/mdm_audit_trail.cpp:136-145`
  - [diverged within-file w=0.97 line=0.71] `src/llm/lora_framework/lora_training_service.cpp:1373-1386` ⟵ `src/llm/lora_framework/lora_training_service.cpp:1256-1269`
  - [diverged within-file w=0.97 line=0.67] `src/server/query_api_handler.cpp:2012-2017` ⟵ `src/server/query_api_handler.cpp:1849-1854`
  - [diverged within-file w=0.97 line=0.30] `src/governance/policy_manager_versioned.cpp:340-361` ⟵ `src/governance/policy_manager_versioned.cpp:408-423`
  - [diverged within-file w=0.97 line=0.69] `src/server/postgres_session.cpp:2242-2271` ⟵ `src/server/postgres_session.cpp:2005-2029`
  - [near within-file w=0.96 line=0.94] `src/sharding/cross_shard_transaction.cpp:2391-2410` ⟵ `src/sharding/cross_shard_transaction.cpp:2133-2151`
  - [diverged within-file w=0.96 line=0.45] `src/exporters/export_encryption.cpp:620-676` ⟵ `src/exporters/export_encryption.cpp:710-747`
  - [near within-file w=0.96 line=0.91] `src/server/mcp_server.cpp:394-434` ⟵ `src/server/mcp_server.cpp:186-225`
  - [verbatim within-file w=0.96 line=0.96] `src/themis/wire_protocol_server.cpp:419-448` ⟵ `src/themis/wire_protocol_server.cpp:258-286`
  - [verbatim within-file w=0.96 line=0.95] `src/transaction/distributed_transaction_manager.cpp:271-327` ⟵ `src/transaction/distributed_transaction_manager.cpp:163-216`
  - [near within-file w=0.96 line=0.89] `src/llm/lora_security_validator.cpp:1081-1108` ⟵ `src/llm/lora_security_validator.cpp:1036-1063`
  - [diverged within-file w=0.96 line=0.56] `tests/test_fused_kernels.cpp:256-297` ⟵ `tests/test_fused_kernels.cpp:311-355`
  - [near within-file w=0.95 line=0.90] `src/server/postgres_session.cpp:205-256` ⟵ `src/server/postgres_session.cpp:127-178`
  - [verbatim within-file w=0.94 line=0.95] `src/llm/llama_wrapper.cpp:1138-1193` ⟵ `src/llm/llama_wrapper.cpp:997-1048`
  - [near within-file w=0.94 line=0.90] `src/security/timestamp_authority.cpp:500-570` ⟵ `src/security/timestamp_authority.cpp:394-460`
  - [near within-file w=0.93 line=0.89] `src/server/voice_api_handler.cpp:1955-2043` ⟵ `src/server/voice_api_handler.cpp:1241-1324`
  - [diverged within-file w=0.93 line=0.33] `src/auth/auth_rate_limiter.cpp:558-579` ⟵ `src/auth/auth_rate_limiter.cpp:585-610`
  - [diverged within-file w=0.93 line=0.67] `src/training/incremental_lora_trainer.cpp:910-933` ⟵ `src/training/incremental_lora_trainer.cpp:836-855`
  - [near within-file w=0.93 line=0.80] `src/ingestion/workflow_engine.cpp:512-553` ⟵ `src/ingestion/workflow_engine.cpp:312-353`
  - [near within-file w=0.92 line=0.93] `tests/test_hsm_key_provider_adapter.cpp:518-539` ⟵ `tests/test_hsm_key_provider_adapter.cpp:436-455`
  - [near within-file w=0.92 line=0.91] `src/graph/graph_query_optimizer.cpp:1827-1866` ⟵ `src/graph/graph_query_optimizer.cpp:1785-1817`
  - [diverged within-file w=0.91 line=0.71] `src/server/task_scheduler_api_handler.cpp:1044-1061` ⟵ `src/server/task_scheduler_api_handler.cpp:937-953`
  - [diverged within-file w=0.91 line=0.68] `src/acceleration/ai_hardware_dispatcher.cpp:139-181` ⟵ `src/acceleration/ai_hardware_dispatcher.cpp:93-127`
  - [diverged within-file w=0.91 line=0.46] `src/analytics/forecasting.cpp:596-635` ⟵ `src/analytics/forecasting.cpp:549-581`
  - [diverged cross-file w=0.91 line=0.71] `src/user_storage_encrypted/multi_level_storage.cpp:60-105` ⟵ `plugins/user_storage_encrypted/src/multi_level_storage.cpp:69-112`
  - [near within-file w=0.91 line=0.83] `src/training/incremental_lora_trainer.cpp:597-620` ⟵ `src/training/incremental_lora_trainer.cpp:540-562`
  - [near within-file w=0.90 line=0.83] `src/server/postgres_session.cpp:694-752` ⟵ `src/server/postgres_session.cpp:569-624`
  - [diverged within-file w=0.90 line=0.67] `src/rag/multi_step_rag.cpp:385-448` ⟵ `src/rag/multi_step_rag.cpp:288-333`
  - [near within-file w=0.89 line=0.86] `src/network/quic_server.cpp:664-697` ⟵ `src/network/quic_server.cpp:630-661`
  - [diverged within-file w=0.89 line=0.79] `src/training/incremental_lora_trainer.cpp:623-641` ⟵ `src/training/incremental_lora_trainer.cpp:565-582`
  - [diverged within-file w=0.89 line=0.61] `src/core/concerns/redis_cache.cpp:705-773` ⟵ `src/core/concerns/redis_cache.cpp:628-684`
  - [diverged within-file w=0.88 line=0.61] `src/sharding/cross_shard_transaction.cpp:642-714` ⟵ `src/sharding/cross_shard_transaction.cpp:368-429`
  - [near within-file w=0.88 line=0.88] `src/server/http3_session.cpp:498-531` ⟵ `src/server/http3_session.cpp:463-492`
  - [diverged within-file w=0.87 line=0.41] `src/analytics/arrow_flight.cpp:643-664` ⟵ `src/analytics/arrow_flight.cpp:720-742`
  - [near within-file w=0.86 line=0.90] `src/server/http3_session.cpp:1033-1103` ⟵ `src/server/http3_session.cpp:910-977`
  - [diverged within-file w=0.85 line=0.45] `tests/test_raii_wrappers.cpp:156-176` ⟵ `tests/test_raii_wrappers.cpp:73-93`
  - [diverged within-file w=0.85 line=0.45] `tests/test_raii_wrappers.cpp:62-82` ⟵ `tests/test_raii_wrappers.cpp:167-187`
  - [diverged within-file w=0.84 line=0.78] `src/llm/llama_wrapper.cpp:681-714` ⟵ `src/llm/llama_wrapper.cpp:608-636`
  - [diverged cross-file w=0.83 line=0.67] `src/ethics_ai/discourse_engine.cpp:65-114` ⟵ `plugins/ethics_ai/discourse_engine.cpp:73-126`
  - [near within-file w=0.83 line=0.91] `src/server/llm_api_handler.cpp:2003-2042` ⟵ `src/server/llm_api_handler.cpp:1638-1673`
  - [diverged within-file w=0.82 line=0.78] `src/query/query_engine.cpp:1873-1901` ⟵ `src/query/query_engine.cpp:1707-1735`
  - [near within-file w=0.82 line=0.84] `src/server/rpc/rpc_service_impl.cpp:1266-1312` ⟵ `src/server/rpc/rpc_service_impl.cpp:793-832`
  - [diverged within-file w=0.82 line=0.75] `src/utils/audit_logger.cpp:1569-1593` ⟵ `src/utils/audit_logger.cpp:1540-1560`
  - [diverged within-file w=0.82 line=0.61] `src/server/mcp_server.cpp:2842-2924` ⟵ `src/server/mcp_server.cpp:1967-2058`
  - [diverged within-file w=0.81 line=0.75] `src/security/access_control.cpp:208-231` ⟵ `src/security/access_control.cpp:183-197`
  - [diverged within-file w=0.81 line=0.43] `src/index/gpu_vector_index.cpp:999-1024` ⟵ `src/index/gpu_vector_index.cpp:388-413`
  - [diverged within-file w=0.81 line=0.64] `src/server/mcp_server.cpp:242-293` ⟵ `src/server/mcp_server.cpp:76-112`
  - [near within-file w=0.80 line=0.84] `src/storage/rocksdb_wrapper.cpp:2242-2290` ⟵ `src/storage/rocksdb_wrapper.cpp:2077-2119`
  - [near within-file w=0.80 line=0.81] `src/storage/rocksdb_wrapper.cpp:2188-2239` ⟵ `src/storage/rocksdb_wrapper.cpp:2030-2074`
  - [diverged within-file w=0.80 line=0.72] `src/llm/llama_wrapper.cpp:1456-1540` ⟵ `src/llm/llama_wrapper.cpp:1198-1270`
  - [diverged within-file w=0.79 line=0.72] `src/server/postgres_session.cpp:336-376` ⟵ `src/server/postgres_session.cpp:260-297`
  - [diverged within-file w=0.79 line=0.32] `src/exporters/huggingface_hub_client.cpp:657-719` ⟵ `src/exporters/huggingface_hub_client.cpp:513-576`
  - [near within-file w=0.79 line=0.81] `src/network/wire_protocol_server.cpp:1097-1190` ⟵ `src/network/wire_protocol_server.cpp:786-879`
  - [diverged within-file w=0.79 line=0.78] `src/server/rpc/rpc_service_impl.cpp:612-633` ⟵ `src/server/rpc/rpc_service_impl.cpp:376-392`
  - [diverged within-file w=0.79 line=0.50] `src/aql/llm_aql_handler.cpp:1434-1504` ⟵ `src/aql/llm_aql_handler.cpp:1251-1310`
  - [diverged within-file w=0.78 line=0.59] `src/llm/llama_wrapper.cpp:2316-2337` ⟵ `src/llm/llama_wrapper.cpp:1964-1980`
  - [diverged within-file w=0.76 line=0.77] `src/acceleration/graphics_backends.cpp:3195-3251` ⟵ `src/acceleration/graphics_backends.cpp:2970-3010`
  - [diverged cross-file w=0.76 line=0.78] `src/user_storage_encrypted/key_derivation_service.cpp:264-309` ⟵ `plugins/user_storage_encrypted/src/key_derivation_service.cpp:52-92`
  - [diverged within-file w=0.75 line=0.24] `tests/llm/test_kernel_fusion_cuda.cpp:248-286` ⟵ `tests/llm/test_kernel_fusion_cuda.cpp:644-678`
  - [diverged within-file w=0.75 line=0.24] `tests/llm/test_kernel_fusion_cuda.cpp:633-667` ⟵ `tests/llm/test_kernel_fusion_cuda.cpp:259-297`
  - [diverged within-file w=0.75 line=0.50] `src/llm/embedded_llm.cpp:102-114` ⟵ `src/llm/embedded_llm.cpp:80-92`
  - [diverged within-file w=0.75 line=0.50] `src/llm/embedded_llm.cpp:81-93` ⟵ `src/llm/embedded_llm.cpp:101-113`
  - [diverged within-file w=0.75 line=0.76] `src/llm/gpu_memory_manager.cpp:628-696` ⟵ `src/llm/gpu_memory_manager.cpp:422-481`
  - [diverged cross-file w=0.74 line=0.27] `src/exporters/parquet_exporter.cpp:491-533` ⟵ `src/exporters/arrow_ipc_exporter.cpp:868-913`
  - [diverged cross-file w=0.74 line=0.26] `src/exporters/arrow_ipc_exporter.cpp:841-882` ⟵ `src/exporters/parquet_exporter.cpp:500-549`
  - [diverged within-file w=0.73 line=0.58] `src/llm/llama_wrapper.cpp:3220-3283` ⟵ `src/llm/llama_wrapper.cpp:2772-2818`
  - [diverged within-file w=0.73 line=0.68] `src/server/cache_admin_api_handler.cpp:619-666` ⟵ `src/server/cache_admin_api_handler.cpp:526-567`
  - [diverged within-file w=0.73 line=0.36] `src/storage/disk_space_monitor.cpp:103-165` ⟵ `src/storage/disk_space_monitor.cpp:369-403`
  - [near within-file w=0.72 line=0.88] `src/updates/parallel_downloader.cpp:380-419` ⟵ `src/updates/parallel_downloader.cpp:305-346`
  - [diverged cross-file w=0.71 line=0.35] `src/llm/lora_framework/nccl_backend.cpp:109-195` ⟵ `src/llm/lora_framework/rccl_backend.cpp:123-186`
  - [diverged cross-file w=0.71 line=0.35] `src/llm/lora_framework/rccl_backend.cpp:109-194` ⟵ `src/llm/lora_framework/nccl_backend.cpp:123-186`
  - [diverged cross-file w=0.70 line=0.19] `src/llm/lora_framework/nccl_backend.cpp:324-378` ⟵ `src/llm/lora_framework/rccl_backend.cpp:290-337`
  - [diverged within-file w=0.70 line=0.05] `src/analytics/forecasting.cpp:1625-1639` ⟵ `src/analytics/forecasting.cpp:912-917`
  - [diverged cross-file w=0.70 line=0.19] `src/llm/lora_framework/rccl_backend.cpp:323-377` ⟵ `src/llm/lora_framework/nccl_backend.cpp:290-337`
  - [diverged within-file w=0.69 line=0.74] `src/index/advanced_vector_index.cpp:362-416` ⟵ `src/index/advanced_vector_index.cpp:284-322`
  - [diverged within-file w=0.69 line=0.67] `src/server/voice_api_handler.cpp:271-290` ⟵ `src/server/voice_api_handler.cpp:183-197`
  - [diverged within-file w=0.67 line=0.44] `src/core/concerns/redis_cache.cpp:819-863` ⟵ `src/core/concerns/redis_cache.cpp:710-745`
  - [diverged cross-file w=0.65 line=0.51] `src/ethics_ai/ethics_evaluator.cpp:40-66` ⟵ `plugins/ethics_ai/ethics_evaluator.cpp:32-67`
  - [diverged within-file w=0.65 line=0.25] `tests/exporters/test_export_encryption.cpp:1146-1182` ⟵ `tests/exporters/test_export_encryption.cpp:1283-1321`
  - [diverged within-file w=0.65 line=0.28] `src/cache/warmup.cpp:532-561` ⟵ `src/cache/warmup.cpp:499-527`
  - [diverged within-file w=0.65 line=0.28] `src/cache/warmup.cpp:490-525` ⟵ `src/cache/warmup.cpp:534-556`
  - [diverged within-file w=0.64 line=0.28] `src/acceleration/backend_registry.cpp:195-229` ⟵ `src/acceleration/backend_registry.cpp:153-174`
  - [diverged within-file w=0.63 line=0.37] `src/cache/redis_cache_coordinator.cpp:194-264` ⟵ `src/cache/redis_cache_coordinator.cpp:121-163`
  - [diverged within-file w=0.62 line=0.19] `src/server/mcp_server.cpp:3071-3084` ⟵ `src/server/mcp_server.cpp:2201-2210`
  - [diverged within-file w=0.62 line=0.19] `src/server/mcp_server.cpp:3223-3236` ⟵ `src/server/mcp_server.cpp:2350-2359`
  - [diverged within-file w=0.61 line=0.17] `src/base/module_sandbox.cpp:365-430` ⟵ `src/base/module_sandbox.cpp:454-516`
  - [diverged within-file w=0.61 line=0.69] `src/server/http3_session.cpp:906-932` ⟵ `src/server/http3_session.cpp:857-873`

## deltahdl/deltahdl — 9914 коммитов | циклы=0(SCC≤1) | копипаст=29

### `b24a20b5c087` — "Satisfy §19.5.5" (Jose Drowne, 2026-06-01) [29 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/deltahdl_deltahdl show b24a20b5c087`
  - [near within-file w=1.00 line=0.86] `src/elaborator/elaborator_validate_classes.cpp:3276-3291` ⟵ `src/elaborator/elaborator_validate_classes.cpp:2206-2221`
  - [near within-file w=1.00 line=0.84] `src/parser/parser_port.cpp:427-454` ⟵ `src/parser/parser_port.cpp:370-397`
  - [near within-file w=1.00 line=0.89] `src/simulator/lowerer.cpp:555-575` ⟵ `src/simulator/lowerer.cpp:529-549`
  - [near within-file w=1.00 line=0.80] `src/simulator/eval_format.cpp:281-291` ⟵ `src/simulator/eval_format.cpp:167-177`
  - [diverged within-file w=0.99 line=0.53] `src/elaborator/elaborator_validate_classes.cpp:254-273` ⟵ `src/elaborator/elaborator_validate_classes.cpp:237-253`
  - [near within-file w=0.99 line=0.88] `src/elaborator/elaborator.cpp:1170-1232` ⟵ `src/elaborator/elaborator.cpp:1107-1164`
  - [diverged within-file w=0.98 line=0.73] `src/simulator/eval_array.cpp:901-913` ⟵ `src/simulator/eval_array.cpp:889-900`
  - [near within-file w=0.98 line=0.84] `src/elaborator/elaborator.cpp:3073-3120` ⟵ `src/elaborator/elaborator.cpp:2751-2794`
  - [near within-file w=0.94 line=0.86] `src/elaborator/elaborator.cpp:2562-2599` ⟵ `src/elaborator/elaborator.cpp:2284-2317`
  - [near within-file w=0.93 line=0.93] `src/simulator/constraint_solver.cpp:387-421` ⟵ `src/simulator/constraint_solver.cpp:142-174`
  - [near within-file w=0.91 line=0.90] `src/simulator/coverage.cpp:202-215` ⟵ `src/simulator/coverage.cpp:139-151`
  - [near within-file w=0.91 line=0.92] `src/elaborator/elaborator_validate_operations.cpp:951-963` ⟵ `src/elaborator/elaborator_validate_operations.cpp:609-620`
  - [diverged within-file w=0.87 line=0.78] `src/simulator/vpi.cpp:1368-1395` ⟵ `src/simulator/vpi.cpp:283-305`
  - [near within-file w=0.87 line=0.87] `src/elaborator/elaborator_validate_classes.cpp:2148-2164` ⟵ `src/elaborator/elaborator_validate_classes.cpp:1140-1153`
  - [diverged within-file w=0.84 line=0.73] `src/elaborator/const_eval.cpp:266-280` ⟵ `src/elaborator/const_eval.cpp:211-221`
  - [diverged within-file w=0.82 line=0.43] `src/simulator/constraint_solver.cpp:664-688` ⟵ `src/simulator/constraint_solver.cpp:304-318`
  - [near within-file w=0.81 line=0.88] `src/simulator/statement_assign_core.cpp:1040-1049` ⟵ `src/simulator/statement_assign_core.cpp:875-883`
  - [diverged within-file w=0.78 line=0.56] `src/simulator/eval_expr.cpp:1328-1344` ⟵ `src/simulator/eval_expr.cpp:1222-1233`
  - [diverged within-file w=0.77 line=0.18] `test/src/unit/test_simulator_clause_18_09.cpp:88-124` ⟵ `test/src/unit/test_simulator_clause_18_09.cpp:38-59`
  - [diverged within-file w=0.77 line=0.44] `src/elaborator/type_eval.cpp:225-240` ⟵ `src/elaborator/type_eval.cpp:179-190`
  - [diverged within-file w=0.77 line=0.64] `src/parser/parser_verify.cpp:440-498` ⟵ `src/parser/parser_verify.cpp:323-368`
  - [diverged within-file w=0.75 line=0.72] `src/elaborator/elaborator_validate_classes.cpp:1362-1420` ⟵ `src/elaborator/elaborator_validate_classes.cpp:638-680`
  - [diverged within-file w=0.75 line=0.44] `src/simulator/constraint_solver.cpp:598-625` ⟵ `src/simulator/constraint_solver.cpp:252-267`
  - [diverged within-file w=0.75 line=0.64] `src/simulator/eval_function.cpp:566-596` ⟵ `src/simulator/eval_function.cpp:206-228`
  - [diverged within-file w=0.73 line=0.53] `src/simulator/eval_systask.cpp:954-973` ⟵ `src/simulator/eval_systask.cpp:617-628`
  - [diverged within-file w=0.72 line=0.57] `src/simulator/eval_expr.cpp:486-533` ⟵ `src/simulator/eval_expr.cpp:448-483`
  - [diverged within-file w=0.64 line=0.44] `src/simulator/vpi.cpp:1121-1180` ⟵ `src/simulator/vpi.cpp:183-213`
  - [diverged within-file w=0.64 line=0.67] `src/elaborator/elaborator.cpp:1872-1914` ⟵ `src/elaborator/elaborator.cpp:1638-1669`
  - [diverged within-file w=0.61 line=0.65] `src/simulator/eval_expr.cpp:631-658` ⟵ `src/simulator/eval_expr.cpp:548-564`

## deskull-m/bakabakaband — 4712 коммитов | циклы=2(SCC≤22) | копипаст=40
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=22. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/deskull-m_bakabakaband`

### `960afc05f6e2` — "Merge pull request #8459 from deskull-m/feat/npc-weapon-attack" (Deskull, 2026-06-01) [40 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/deskull-m_bakabakaband show 960afc05f6e2`
  - [diverged within-file w=1.00 line=0.71] `src/view/display-lore.cpp:1275-1302` ⟵ `src/view/display-lore.cpp:1109-1136`
  - [diverged within-file w=1.00 line=0.64] `src/action/activation-execution.cpp:112-128` ⟵ `src/action/activation-execution.cpp:141-157`
  - [diverged within-file w=1.00 line=0.56] `src/mspell/mspell-floor.cpp:434-441` ⟵ `src/mspell/mspell-floor.cpp:445-452`
  - [near within-file w=1.00 line=0.93] `src/view/display-lore.cpp:1306-1350` ⟵ `src/view/display-lore.cpp:1140-1184`
  - [near within-file w=0.99 line=0.80] `src/main-x11.cpp:1272-1282` ⟵ `src/main-x11.cpp:1252-1262`
  - [near within-file w=0.98 line=0.88] `src/view/display-player.cpp:306-326` ⟵ `src/view/display-player.cpp:255-275`
  - [near within-file w=0.96 line=0.80] `src/monster/monster-processor.cpp:1564-1574` ⟵ `src/monster/monster-processor.cpp:874-884`
  - [near within-file w=0.96 line=0.82] `src/spell-kind/earthquake.cpp:207-221` ⟵ `src/spell-kind/earthquake.cpp:222-236`
  - [diverged within-file w=0.94 line=0.71] `src/player/player-status-flags.cpp:1784-1803` ⟵ `src/player/player-status-flags.cpp:1842-1862`
  - [diverged within-file w=0.94 line=0.78] `src/monster/monster-processor.cpp:827-871` ⟵ `src/monster/monster-processor.cpp:150-194`
  - [near within-file w=0.93 line=0.87] `src/blue-magic/learnt-power-getter.cpp:646-681` ⟵ `src/blue-magic/learnt-power-getter.cpp:484-520`
  - [diverged within-file w=0.90 line=0.62] `src/melee/monster-attack-monster.cpp:416-442` ⟵ `src/melee/monster-attack-monster.cpp:354-380`
  - [diverged within-file w=0.88 line=0.73] `src/main-x11.cpp:1287-1298` ⟵ `src/main-x11.cpp:1267-1275`
  - [diverged within-file w=0.88 line=0.50] `src/cmd-action/cmd-spell.cpp:930-942` ⟵ `src/cmd-action/cmd-spell.cpp:943-955`
  - [diverged within-file w=0.86 line=0.67] `src/monster-floor/monster-object.cpp:214-240` ⟵ `src/monster-floor/monster-object.cpp:185-212`
  - [diverged within-file w=0.85 line=0.57] `src/floor/floor-leaver.cpp:464-477` ⟵ `src/floor/floor-leaver.cpp:488-504`
  - [diverged within-file w=0.84 line=0.62] `src/status/bad-status-setter.cpp:552-565` ⟵ `src/status/bad-status-setter.cpp:572-586`
  - [diverged within-file w=0.84 line=0.64] `src/monster-attack/monster-attack-switcher.cpp:550-562` ⟵ `src/monster-attack/monster-attack-switcher.cpp:567-579`
  - [diverged within-file w=0.84 line=0.31] `src/birth/birth-wizard.cpp:590-597` ⟵ `src/birth/birth-wizard.cpp:600-608`
  - [diverged within-file w=0.82 line=0.52] `src/view/display-lore-attacks.cpp:334-369` ⟵ `src/view/display-lore-attacks.cpp:111-143`
  - [diverged within-file w=0.81 line=0.54] `src/main-x11.cpp:2299-2336` ⟵ `src/main-x11.cpp:2193-2218`
  - [diverged within-file w=0.81 line=0.42] `src/monster-attack/monster-attack-player.cpp:613-630` ⟵ `src/monster-attack/monster-attack-player.cpp:539-555`
  - [diverged within-file w=0.80 line=0.69] `src/blue-magic/learnt-power-getter.cpp:584-624` ⟵ `src/blue-magic/learnt-power-getter.cpp:425-462`
  - [diverged cross-file w=0.79 line=0.70] `src/main-unix/unix-cfg-reader.cpp:143-178` ⟵ `src/main-win/main-win-cfg-reader.cpp:88-117`
  - [diverged cross-file w=0.78 line=0.56] `src/monster-attack/monster-attack-status.cpp:163-190` ⟵ `src/effect/effect-player-resist-hurt.cpp:486-511`
  - [diverged cross-file w=0.77 line=0.50] `src/effect/effect-player-resist-hurt.cpp:474-498` ⟵ `src/monster-attack/monster-attack-status.cpp:171-199`
  - [diverged within-file w=0.75 line=0.50] `src/monster/monster-processor.cpp:1445-1481` ⟵ `src/monster/monster-processor.cpp:750-787`
  - [diverged within-file w=0.75 line=0.61] `src/monster-attack/monster-attack-player.cpp:438-469` ⟵ `src/monster-attack/monster-attack-player.cpp:370-400`
  - [diverged within-file w=0.74 line=0.52] `src/monster-attack/monster-attack-player.cpp:284-319` ⟵ `src/monster-attack/monster-attack-player.cpp:249-272`
  - [diverged within-file w=0.74 line=0.53] `src/monster-floor/monster-death.cpp:254-264` ⟵ `src/monster-floor/monster-death.cpp:354-365`
  - [near within-file w=0.70 line=0.92] `src/main-x11.cpp:2709-2729` ⟵ `src/main-x11.cpp:2572-2590`
  - [diverged within-file w=0.68 line=0.74] `src/main-x11.cpp:2012-2067` ⟵ `src/main-x11.cpp:1925-1965`
  - [diverged within-file w=0.67 line=0.73] `src/status/bad-status-setter.cpp:535-548` ⟵ `src/status/bad-status-setter.cpp:554-568`
  - [diverged within-file w=0.66 line=0.30] `src/info-reader/dungeon-reader.cpp:892-916` ⟵ `src/info-reader/dungeon-reader.cpp:362-403`
  - [diverged within-file w=0.66 line=0.59] `src/view/status-first-page.cpp:648-665` ⟵ `src/view/status-first-page.cpp:528-540`
  - [diverged within-file w=0.65 line=0.54] `src/melee/monster-attack-monster.cpp:270-296` ⟵ `src/melee/monster-attack-monster.cpp:244-260`
  - [diverged within-file w=0.64 line=0.48] `src/blue-magic/learnt-power-getter.cpp:441-473` ⟵ `src/blue-magic/learnt-power-getter.cpp:317-340`
  - [diverged within-file w=0.63 line=0.54] `src/status/bad-status-setter.cpp:622-635` ⟵ `src/status/bad-status-setter.cpp:645-660`
  - [diverged cross-file w=0.62 line=0.45] `src/main-win.cpp:2720-2738` ⟵ `src/main.cpp:223-245`
  - [diverged cross-file w=0.62 line=0.45] `src/main.cpp:225-247` ⟵ `src/main-win.cpp:2718-2736`

## ericflo/kiln — 4462 коммитов | циклы=11(SCC≤14) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 11 include-циклов, крупнейший SCC=14. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/ericflo_kiln`

## fuddlesworth/PlasmaZones — 3757 коммитов | циклы=0(SCC≤1) | копипаст=89

### `c72f6622150c` — "Merge pull request #548 from fuddlesworth/fix/wayland-startup-guard-empty-display" (Nathan, 2026-05-29) [89 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/fuddlesworth_PlasmaZones show c72f6622150c`
  - [verbatim cross-file w=1.00 line=1.00] `kwin-effect/plasmazoneseffect/borders.cpp:150-155` ⟵ `kwin-effect/plasmazoneseffect.cpp:2956-2961`
  - [near within-file w=1.00 line=0.81] `src/daemon/unifiedlayoutcontroller.cpp:169-194` ⟵ `src/daemon/unifiedlayoutcontroller.cpp:106-131`
  - [diverged cross-file w=1.00 line=0.67] `libs/phosphor-zones/src/layoutregistry_persistence.cpp:179-225` ⟵ `src/core/layoutmanager/persistence.cpp:169-214`
  - [verbatim cross-file w=1.00 line=1.00] `libs/phosphor-zones/src/layoutregistry_assignments.cpp:806-813` ⟵ `src/core/layoutmanager/assignments.cpp:481-488`
  - [near cross-file w=1.00 line=0.88] `libs/phosphor-zones/src/layoutfactory.cpp:54-63` ⟵ `src/core/layoutfactory.cpp:54-64`
  - [diverged cross-file w=1.00 line=0.48] `libs/phosphor-zones/src/layoutfactory.cpp:20-50` ⟵ `src/core/layoutfactory.cpp:19-50`
  - [verbatim cross-file w=1.00 line=1.00] `libs/phosphor-zones/src/layout.cpp:455-461` ⟵ `src/core/layout.cpp:351-357`
  - [verbatim cross-file w=1.00 line=1.00] `libs/phosphor-rendering/src/shadernoderhipipeline.cpp:486-495` ⟵ `src/daemon/rendering/zoneshadernoderhi/pipeline.cpp:433-442`
  - [near cross-file w=1.00 line=0.85] `libs/phosphor-audio/src/cavaspectrumprovider.cpp:175-220` ⟵ `src/daemon/cavaservice.cpp:171-220`
  - [verbatim cross-file w=1.00 line=1.00] `libs/phosphor-animation/src/easing.cpp:21-28` ⟵ `kwin-effect/windowanimator.cpp:23-30`
  - [verbatim cross-file w=1.00 line=1.00] `kwin-effect/plasmazoneseffect/borders.cpp:162-180` ⟵ `kwin-effect/plasmazoneseffect.cpp:2968-2986`
  - [verbatim cross-file w=1.00 line=1.00] `kwin-effect/plasmazoneseffect/window_lifecycle.cpp:416-444` ⟵ `kwin-effect/plasmazoneseffect.cpp:646-674`
  - [diverged cross-file w=1.00 line=0.79] `libs/phosphor-shaders/src/shaderregistry.cpp:697-725` ⟵ `src/core/shaderregistry/params.cpp:199-226`
  - [near within-file w=1.00 line=0.88] `src/dbus/layoutadaptor.cpp:565-588` ⟵ `src/dbus/layoutadaptor.cpp:466-489`
  - [near cross-file w=1.00 line=0.85] `libs/phosphor-tile-engine/src/AutotileEngine.cpp:1012-1044` ⟵ `src/autotile/AutotileEngine.cpp:526-558`
  - [diverged cross-file w=0.99 line=0.60] `libs/phosphor-zones/src/layoutregistry_persistence.cpp:129-165` ⟵ `src/core/layoutmanager/persistence.cpp:120-155`
  - [near within-file w=0.99 line=0.82] `src/daemon/daemon/autotile.cpp:455-480` ⟵ `src/daemon/daemon/autotile.cpp:373-398`
  - [diverged within-file w=0.99 line=0.78] `src/dbus/windowdragadaptor/drag.cpp:310-374` ⟵ `src/dbus/windowdragadaptor/drag.cpp:178-241`
  - [diverged cross-file w=0.99 line=0.59] `libs/phosphor-zones/src/layoutregistry_persistence.cpp:32-68` ⟵ `src/core/layoutmanager/persistence.cpp:23-59`
  - [diverged cross-file w=0.99 line=0.71] `libs/phosphor-animation/src/easing.cpp:95-113` ⟵ `kwin-effect/windowanimator.cpp:206-228`
  - [diverged within-file w=0.98 line=0.77] `src/dbus/windowdragadaptor/drag.cpp:395-426` ⟵ `src/dbus/windowdragadaptor/drag.cpp:261-292`
  - [diverged within-file w=0.98 line=0.67] `kwin-effect/autotilehandler.cpp:685-711` ⟵ `kwin-effect/autotilehandler.cpp:764-789`
  - [diverged within-file w=0.98 line=0.40] `src/daemon/unifiedlayoutcontroller.cpp:133-140` ⟵ `src/daemon/unifiedlayoutcontroller.cpp:70-77`
  - [diverged cross-file w=0.98 line=0.26] `libs/phosphor-zones/src/layoutregistry.cpp:576-590` ⟵ `src/core/layoutmanager.cpp:460-474`
  - [diverged cross-file w=0.97 line=0.62] `libs/phosphor-shaders/src/shaderincluderesolver.cpp:47-115` ⟵ `src/core/shaderincluderesolver.cpp:47-112`
  - [diverged cross-file w=0.97 line=0.27] `libs/phosphor-identity/include/PhosphorIdentity/ScreenId.h:139-146` ⟵ `src/core/utils.cpp:98-105`
  - [diverged cross-file w=0.97 line=0.55] `libs/phosphor-audio/src/cavaspectrumprovider.cpp:54-83` ⟵ `src/daemon/cavaservice.cpp:36-72`
  - [near cross-file w=0.96 line=0.82] `libs/phosphor-rendering/src/shadernoderhicore.cpp:487-516` ⟵ `src/daemon/rendering/zoneshadernoderhi.cpp:320-348`
  - [diverged cross-file w=0.95 line=0.53] `kwin-effect/plasmazoneseffect/daemon_bringup.cpp:665-682` ⟵ `kwin-effect/plasmazoneseffect.cpp:1530-1542`
  - [diverged cross-file w=0.95 line=0.75] `libs/phosphor-workspaces/src/VirtualDesktopManager.cpp:35-65` ⟵ `src/core/virtualdesktopmanager.cpp:48-84`
  - [near within-file w=0.95 line=0.92] `src/dbus/windowdragadaptor/drag.cpp:656-672` ⟵ `src/dbus/windowdragadaptor/drag.cpp:419-434`
  - [diverged within-file w=0.94 line=0.03] `src/dbus/layoutadaptor/editor.cpp:255-277` ⟵ `src/dbus/layoutadaptor/editor.cpp:231-253`
  - [near within-file w=0.94 line=0.88] `src/editor/controller/shader.cpp:435-447` ⟵ `src/editor/controller/shader.cpp:394-402`
  - [diverged within-file w=0.93 line=0.49] `src/daemon/daemon.cpp:538-576` ⟵ `src/daemon/daemon.cpp:97-129`
  - [near cross-file w=0.93 line=0.92] `libs/phosphor-tile-engine/src/AutotileEngine.cpp:3589-3621` ⟵ `src/autotile/AutotileEngine.cpp:1715-1747`
  - [diverged cross-file w=0.92 line=0.73] `libs/phosphor-zones/src/layoutregistry.cpp:330-404` ⟵ `src/core/layoutmanager.cpp:224-286`
  - [diverged cross-file w=0.92 line=0.53] `libs/phosphor-snap-engine/src/calculate.cpp:145-162` ⟵ `src/core/windowtrackingservice/snap.cpp:115-133`
  - [diverged cross-file w=0.92 line=0.38] `libs/phosphor-rendering/src/shadernoderhicore.cpp:564-577` ⟵ `src/daemon/rendering/zoneshadernoderhi.cpp:388-393`
  - [near within-file w=0.92 line=0.88] `src/daemon/zoneselectorcontroller.cpp:316-327` ⟵ `src/daemon/zoneselectorcontroller.cpp:288-298`
  - [diverged cross-file w=0.90 line=0.66] `libs/phosphor-workspaces/src/VirtualDesktopManager.cpp:131-175` ⟵ `src/core/virtualdesktopmanager.cpp:154-211`
  - [diverged within-file w=0.89 line=0.76] `src/config/settings/perscreen.cpp:544-570` ⟵ `src/config/settings/perscreen.cpp:478-503`
  - [diverged within-file w=0.88 line=0.68] `src/settings/screenprovider.cpp:183-207` ⟵ `src/settings/screenprovider.cpp:122-146`
  - [near cross-file w=0.87 line=0.86] `kwin-effect/plasmazoneseffect/drag_snap.cpp:251-286` ⟵ `kwin-effect/plasmazoneseffect.cpp:2559-2594`
  - [diverged within-file w=0.87 line=0.35] `src/daemon/unifiedlayoutcontroller.cpp:256-302` ⟵ `src/daemon/unifiedlayoutcontroller.cpp:193-254`
  - [diverged cross-file w=0.87 line=0.64] `libs/phosphor-rendering/src/shadereffect.cpp:872-887` ⟵ `src/daemon/rendering/zoneshaderitem/setters.cpp:156-168`
  - [diverged within-file w=0.86 line=0.71] `src/config/settings/perscreen.cpp:595-616` ⟵ `src/config/settings/perscreen.cpp:528-548`
  - [diverged cross-file w=0.86 line=0.73] `libs/phosphor-rendering/src/shadernoderhiuniforms.cpp:296-323` ⟵ `src/daemon/rendering/zoneshadernoderhi/uniforms.cpp:268-298`
  - [diverged cross-file w=0.86 line=0.47] `libs/phosphor-rendering/src/shadereffect.cpp:848-868` ⟵ `src/daemon/rendering/zoneshaderitem/setters.cpp:140-152`
  - [diverged cross-file w=0.85 line=0.49] `libs/phosphor-snap-engine/src/resnap_calc.cpp:68-137` ⟵ `src/core/windowtrackingservice/resnap.cpp:71-139`
  - [diverged within-file w=0.85 line=0.67] `src/daemon/daemon.cpp:578-582` ⟵ `src/daemon/daemon.cpp:131-135`
  - [diverged within-file w=0.85 line=0.71] `src/config/settings/perscreen.cpp:492-513` ⟵ `src/config/settings/perscreen.cpp:427-447`
  - [diverged cross-file w=0.84 line=0.24] `libs/phosphor-shaders/src/shaderregistry.cpp:905-924` ⟵ `src/core/shaderregistry/params.cpp:295-308`
  - [diverged within-file w=0.83 line=0.46] `src/dbus/windowdragadaptor/drop.cpp:214-298` ⟵ `src/dbus/windowdragadaptor/drop.cpp:114-187`
  - [diverged cross-file w=0.83 line=0.71] `libs/phosphor-workspaces/src/VirtualDesktopManager.cpp:251-267` ⟵ `src/core/virtualdesktopmanager.cpp:318-338`
  - [diverged cross-file w=0.83 line=0.55] `libs/phosphor-tile-engine/src/AutotileEngine.cpp:2328-2371` ⟵ `src/autotile/AutotileEngine.cpp:1187-1222`
  - [near within-file w=0.82 line=0.82] `src/editor/controller/layout.cpp:311-370` ⟵ `src/editor/controller/layout.cpp:143-188`
  - [near within-file w=0.81 line=0.86] `src/config/settings/perscreen.cpp:307-332` ⟵ `src/config/settings/perscreen.cpp:243-269`
  - [diverged within-file w=0.81 line=0.25] `kwin-effect/windowanimator.cpp:438-449` ⟵ `kwin-effect/windowanimator.cpp:408-415`
  - [diverged within-file w=0.81 line=0.57] `src/editor/controller/layout.cpp:124-164` ⟵ `src/editor/controller/layout.cpp:38-66`
  - [near within-file w=0.80 line=0.92] `src/editor/controller/shader.cpp:356-375` ⟵ `src/editor/controller/shader.cpp:328-343`
  - [diverged cross-file w=0.80 line=0.61] `libs/phosphor-rendering/src/shadernoderhipipeline.cpp:19-49` ⟵ `src/daemon/rendering/zoneshadernoderhi/pipeline.cpp:24-45`
  - [diverged cross-file w=0.78 line=0.68] `libs/phosphor-tile-engine/src/AutotileEngine.cpp:968-1008` ⟵ `src/autotile/AutotileEngine.cpp:491-522`
  - [diverged within-file w=0.77 line=0.20] `src/daemon/overlayservice/overlay_data.cpp:339-377` ⟵ `src/daemon/overlayservice/overlay_data.cpp:197-233`
  - [diverged cross-file w=0.77 line=0.11] `libs/phosphor-screens/src/manager.cpp:233-288` ⟵ `src/core/screenmanager.cpp:150-206`
  - [diverged cross-file w=0.75 line=0.67] `libs/phosphor-rendering/src/shadereffect.cpp:1494-1504` ⟵ `src/daemon/rendering/zoneshaderitem.cpp:409-427`
  - [diverged within-file w=0.75 line=0.68] `src/dbus/settingsadaptor.cpp:659-696` ⟵ `src/dbus/settingsadaptor.cpp:391-423`
  - [diverged within-file w=0.75 line=0.32] `src/editor/controller/shader.cpp:513-534` ⟵ `src/editor/controller/shader.cpp:469-491`
  - [diverged cross-file w=0.74 line=0.55] `kwin-effect/plasmazoneseffect/daemon_apply.cpp:643-707` ⟵ `kwin-effect/plasmazoneseffect.cpp:2277-2330`
  - [diverged within-file w=0.74 line=0.15] `src/daemon/unifiedlayoutcontroller.cpp:112-121` ⟵ `src/daemon/unifiedlayoutcontroller.cpp:52-58`
  - [diverged within-file w=0.73 line=0.27] `src/dbus/layoutadaptor.cpp:251-284` ⟵ `src/dbus/layoutadaptor.cpp:211-243`
  - [diverged cross-file w=0.70 line=0.61] `libs/phosphor-rendering/src/shadernoderhicore.cpp:537-562` ⟵ `src/daemon/rendering/zoneshadernoderhi.cpp:371-386`
  - [diverged within-file w=0.69 line=0.53] `src/daemon/daemon/osd.cpp:151-172` ⟵ `src/daemon/daemon/osd.cpp:132-149`
  - [diverged cross-file w=0.67 line=0.44] `kwin-effect/plasmazoneseffect/window_lifecycle.cpp:339-396` ⟵ `kwin-effect/plasmazoneseffect.cpp:597-626`
  - [diverged within-file w=0.67 line=0.40] `src/daemon/rendering/zoneshaderitem.cpp:352-422` ⟵ `src/daemon/rendering/zoneshaderitem.cpp:278-344`
  - [diverged within-file w=0.67 line=0.50] `src/daemon/overlayservice/overlay.cpp:652-661` ⟵ `src/daemon/overlayservice/overlay.cpp:390-397`
  - [diverged cross-file w=0.66 line=0.50] `libs/phosphor-workspaces/src/ActivityManager.cpp:45-78` ⟵ `src/core/activitymanager.cpp:64-118`
  - [diverged cross-file w=0.66 line=0.39] `kwin-effect/plasmazoneseffect/window_lifecycle.cpp:457-515` ⟵ `kwin-effect/plasmazoneseffect.cpp:682-733`
  - [diverged within-file w=0.65 line=0.51] `src/daemon/daemon/start.cpp:298-372` ⟵ `src/daemon/daemon/start.cpp:176-219`
  - [diverged cross-file w=0.65 line=0.33] `src/settings/snappingappearancecontroller.cpp:48-65` ⟵ `src/settings/settingscontroller.cpp:1432-1447`
  - [diverged within-file w=0.65 line=0.50] `src/config/settings/perscreen.cpp:364-377` ⟵ `src/config/settings/perscreen.cpp:301-313`
  - [diverged within-file w=0.65 line=0.69] `src/config/settings.cpp:2659-2671` ⟵ `src/config/settings.cpp:428-439`
  - [diverged within-file w=0.64 line=0.42] `tests/unit/dbus/test_compositor_bridge.cpp:107-138` ⟵ `tests/unit/dbus/test_compositor_bridge.cpp:739-762`
  - [diverged within-file w=0.64 line=0.55] `src/daemon/daemon.cpp:810-873` ⟵ `src/daemon/daemon.cpp:213-279`
  - [diverged cross-file w=0.64 line=0.50] `libs/phosphor-rendering/src/shadereffect.cpp:1527-1539` ⟵ `src/daemon/rendering/zoneshaderitem.cpp:431-448`
  - [diverged cross-file w=0.64 line=0.40] `libs/phosphor-rendering/src/shadernoderhipipeline.cpp:292-333` ⟵ `src/daemon/rendering/zoneshadernoderhi/pipeline.cpp:198-246`
  - [diverged cross-file w=0.64 line=0.56] `src/settings/snappingappearancecontroller.cpp:69-77` ⟵ `src/settings/settingscontroller.cpp:1451-1459`
  - [diverged within-file w=0.63 line=0.20] `src/daemon/daemon.cpp:615-653` ⟵ `src/daemon/daemon.cpp:97-129`
  - [diverged within-file w=0.61 line=0.43] `src/daemon/overlayservice/shader.cpp:457-492` ⟵ `src/daemon/overlayservice/shader.cpp:355-388`
  - [diverged cross-file w=0.60 line=0.23] `src/settings/kzonesimporter.cpp:40-46` ⟵ `src/settings/settingscontroller.cpp:1837-1845`

## etkecc/komai — 3419 коммитов | циклы=0(SCC≤1) | копипаст=38

### `830877c71386` — "Release v2026.05.31.1" (Slavi Pantaleev, 2026-05-31) [38 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/etkecc_komai show 830877c71386`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:530-561` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:3357-3388`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:543-597` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2202-2256`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:287-301` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1661-1675`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:91-145` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1462-1516`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:32-87` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1402-1457`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineCompose.cpp:323-379` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1341-1397`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:637-678` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2351-2392`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:698-710` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2413-2425`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineCompose.cpp:173-197` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1199-1223`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineBackend.cpp:188-216` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2944-2972`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:608-651` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:3436-3479`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:589-603` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:3417-3431`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineForward.cpp:122-155` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1946-1979`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:385-398` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2992-3005`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:372-380` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2978-2986`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:276-295` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2673-2692`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:128-155` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2535-2562`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineLifecycle.cpp:43-67` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:267-291`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineLifecycle.cpp:92-123` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:317-348`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineLifecycle.cpp:854-873` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1051-1070`
  - [verbatim cross-file w=1.00 line=1.00] `src/timeline/view/TimelineViewManagerMatrixTimelineThreads.cpp:26-73` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2263-2310`
  - [verbatim cross-file w=0.95 line=0.96] `src/timeline/view/TimelineViewManagerMatrixTimelineForward.cpp:242-300` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2067-2123`
  - [verbatim within-file w=0.94 line=0.95] `src/utils/UtilsReveal.cpp:95-145` ⟵ `src/utils/UtilsReveal.cpp:38-85`
  - [near cross-file w=0.94 line=0.89] `src/timeline/view/TimelineViewManagerMatrixTimelineEvents.cpp:358-417` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1724-1782`
  - [near within-file w=0.94 line=0.93] `tests/SettingsIntegrationTest.cpp:2298-2364` ⟵ `tests/SettingsIntegrationTest.cpp:1819-1881`
  - [near cross-file w=0.93 line=0.90] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:480-523` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:3010-3051`
  - [near cross-file w=0.91 line=0.87] `src/timeline/view/TimelineViewManagerMatrixTimelineCompose.cpp:207-286` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1235-1301`
  - [diverged cross-file w=0.88 line=0.48] `src/timeline/view/TimelineViewManagerMatrixTimelineThreads.cpp:140-161` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:3150-3171`
  - [diverged within-file w=0.87 line=0.78] `src/app/MainApplication.cpp:539-559` ⟵ `src/app/MainApplication.cpp:494-508`
  - [diverged cross-file w=0.84 line=0.67] `src/timeline/view/TimelineViewManagerMatrixTimelineThreads.cpp:77-136` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:3098-3145`
  - [near within-file w=0.82 line=0.81] `etc/tools/logo-assets/LogoAssetGenerator.cpp:219-255` ⟵ `etc/tools/logo-assets/LogoAssetGenerator.cpp:144-172`
  - [near within-file w=0.79 line=0.87] `src/timeline/roomlist/RoomlistModelPreviews.cpp:143-167` ⟵ `src/timeline/roomlist/RoomlistModelPreviews.cpp:70-94`
  - [diverged within-file w=0.78 line=0.67] `src/timeline/rust/MatrixTimelineModel.cpp:1061-1075` ⟵ `src/timeline/rust/MatrixTimelineModel.cpp:927-939`
  - [diverged cross-file w=0.76 line=0.00] `src/ui/ThemeDef.h:66-70` ⟵ `src/timeline/view/TimelineViewManagerColors.cpp:55-59`
  - [diverged within-file w=0.74 line=0.74] `src/timeline/litehtml/LitehtmlContainer.cpp:407-432` ⟵ `src/timeline/litehtml/LitehtmlContainer.cpp:373-399`
  - [diverged within-file w=0.72 line=0.75] `src/timeline/roomlist/RoomlistModelPreviews.cpp:173-186` ⟵ `src/timeline/roomlist/RoomlistModelPreviews.cpp:100-112`
  - [diverged cross-file w=0.64 line=0.44] `src/timeline/view/TimelineViewManagerSlashCommands.cpp:234-318` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:1235-1301`
  - [diverged cross-file w=0.62 line=0.51] `src/timeline/view/TimelineViewManagerMatrixTimelineAttachments.cpp:214-259` ⟵ `src/timeline/view/TimelineViewManagerMatrixTimeline.cpp:2597-2668`

## chrxh/alien — 3234 коммитов | циклы=0(SCC≤1) | копипаст=19

### `caeee1608c92` — "Update program version to 5.0.0-alpha.17" (Christian Heinemann, 2026-05-31) [19 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/chrxh_alien show caeee1608c92`
  - [diverged within-file w=1.00 line=0.78] `source/EngineTestData/DescTestDataFactory.cpp:375-384` ⟵ `source/EngineTestData/DescTestDataFactory.cpp:360-369`
  - [near within-file w=1.00 line=0.86] `source/Gui/CreaturePreviewWidget.cpp:441-457` ⟵ `source/Gui/CreaturePreviewWidget.cpp:401-417`
  - [diverged within-file w=1.00 line=0.57] `source/EngineInterface/GeometryBuffers.cpp:130-135` ⟵ `source/EngineInterface/GeometryBuffers.cpp:125-129`
  - [diverged within-file w=0.99 line=0.71] `source/Gui/PreviewWidget.cpp:288-294` ⟵ `source/Gui/PreviewWidget.cpp:275-281`
  - [diverged within-file w=0.97 line=0.50] `source/PersisterInterface/SerializerService.cpp:225-244` ⟵ `source/PersisterInterface/SerializerService.cpp:122-141`
  - [near within-file w=0.94 line=0.93] `source/Gui/SpecificationGuiService.cpp:517-532` ⟵ `source/Gui/SpecificationGuiService.cpp:488-502`
  - [diverged within-file w=0.94 line=0.61] `source/EngineTests/BalanceTests.cpp:158-193` ⟵ `source/EngineTests/BalanceTests.cpp:96-132`
  - [diverged within-file w=0.92 line=0.43] `source/EngineTests/ConstructorTests.cpp:3260-3310` ⟵ `source/EngineTests/ConstructorTests.cpp:2829-2871`
  - [diverged within-file w=0.90 line=0.52] `source/EngineTests/CreatureTests.cpp:510-540` ⟵ `source/EngineTests/CreatureTests.cpp:378-404`
  - [near within-file w=0.90 line=0.89] `source/Gui/MassOperationsDialog.cpp:266-319` ⟵ `source/Gui/MassOperationsDialog.cpp:147-194`
  - [diverged within-file w=0.84 line=0.60] `source/EngineTestData/DescTestDataFactory.cpp:972-980` ⟵ `source/EngineTestData/DescTestDataFactory.cpp:950-958`
  - [diverged within-file w=0.82 line=0.71] `source/Gui/GenomeTabWidget.cpp:160-179` ⟵ `source/Gui/GenomeTabWidget.cpp:185-207`
  - [diverged within-file w=0.74 line=0.50] `source/Gui/RenderStep.cpp:145-165` ⟵ `source/Gui/RenderStep.cpp:178-199`
  - [diverged within-file w=0.73 line=0.59] `source/Gui/StatisticsWindow.cpp:699-730` ⟵ `source/Gui/StatisticsWindow.cpp:582-610`
  - [diverged within-file w=0.71 line=0.53] `source/EngineTestData/DescTestDataFactory.cpp:845-864` ⟵ `source/EngineTestData/DescTestDataFactory.cpp:824-842`
  - [diverged within-file w=0.71 line=0.36] `source/Gui/CreatorWindow.cpp:347-383` ⟵ `source/Gui/CreatorWindow.cpp:285-310`
  - [diverged within-file w=0.63 line=0.57] `source/Gui/CreatorWindow.cpp:185-198` ⟵ `source/Gui/CreatorWindow.cpp:144-160`
  - [diverged within-file w=0.62 line=0.74] `source/EngineInterface/PreviewDescConverterService.cpp:151-172` ⟵ `source/EngineInterface/PreviewDescConverterService.cpp:121-141`
  - [diverged cross-file w=0.60 line=0.32] `source/EngineInterface/ShapeGenerator.h:67-87` ⟵ `source/EngineInterface/ShapeGenerator.cpp:334-355`

## GregorGullwi/FlashCpp — 2930 коммитов | циклы=4(SCC≤8) | копипаст=90
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 4 include-циклов, крупнейший SCC=8. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/GregorGullwi_FlashCpp`

### `3fab46902847` — "Compact sema and template planning docs" (Gregor, 2026-05-28) [90 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/GregorGullwi_FlashCpp show 3fab46902847`
  - [verbatim within-file w=1.00 line=1.00] `src/Parser_Templates_Inst_Deduction.cpp:4717-4732` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:3016-3031`
  - [near within-file w=1.00 line=0.89] `src/IrGenerator_Expr_Operators.cpp:1582-1625` ⟵ `src/IrGenerator_Expr_Operators.cpp:1142-1183`
  - [verbatim within-file w=1.00 line=1.00] `src/SemanticAnalysis.cpp:3025-3041` ⟵ `src/SemanticAnalysis.cpp:2419-2435`
  - [verbatim within-file w=1.00 line=0.97] `src/Parser_Templates_Class.cpp:1186-1273` ⟵ `src/Parser_Templates_Class.cpp:1019-1106`
  - [diverged within-file w=1.00 line=0.71] `src/Parser_Expr_BinaryPrecedence.cpp:169-184` ⟵ `src/Parser_Expr_BinaryPrecedence.cpp:210-228`
  - [near within-file w=1.00 line=0.89] `src/Parser_Templates_Inst_ClassTemplate.cpp:10754-10797` ⟵ `src/Parser_Templates_Inst_ClassTemplate.cpp:5766-5809`
  - [verbatim within-file w=1.00 line=1.00] `src/Parser_Expr_BinaryPrecedence.cpp:121-141` ⟵ `src/Parser_Expr_BinaryPrecedence.cpp:170-190`
  - [near within-file w=1.00 line=0.85] `src/IrGenerator_Visitors_Decl.cpp:438-449` ⟵ `src/IrGenerator_Visitors_Decl.cpp:316-327`
  - [verbatim within-file w=0.99 line=0.96] `src/TemplateRegistry_Lazy.h:1666-1749` ⟵ `src/TemplateRegistry_Lazy.h:1351-1434`
  - [near within-file w=0.99 line=0.93] `src/Parser_Statements.cpp:1501-1545` ⟵ `src/Parser_Statements.cpp:1155-1199`
  - [near within-file w=0.99 line=0.91] `src/SemanticAnalysis.cpp:4850-4901` ⟵ `src/SemanticAnalysis.cpp:3351-3402`
  - [diverged within-file w=0.99 line=0.72] `src/ConstExprEvaluator_Core.cpp:1045-1066` ⟵ `src/ConstExprEvaluator_Core.cpp:802-822`
  - [diverged within-file w=0.99 line=0.62] `src/Parser_Templates_Substitution.cpp:822-842` ⟵ `src/Parser_Templates_Substitution.cpp:647-661`
  - [verbatim within-file w=0.98 line=0.95] `src/IrGenerator_Stmt_Decl.cpp:2682-2734` ⟵ `src/IrGenerator_Stmt_Decl.cpp:2510-2562`
  - [near within-file w=0.98 line=0.84] `src/Parser_Expr_BinaryPrecedence.cpp:193-215` ⟵ `src/Parser_Expr_BinaryPrecedence.cpp:239-260`
  - [diverged within-file w=0.98 line=0.73] `src/ConstExprEvaluator_Members.cpp:3233-3252` ⟵ `src/ConstExprEvaluator_Members.cpp:1849-1868`
  - [diverged within-file w=0.98 line=0.73] `src/ConstExprEvaluator_Members.cpp:3956-3975` ⟵ `src/ConstExprEvaluator_Members.cpp:1849-1868`
  - [near within-file w=0.98 line=0.91] `src/FileReader_Macros.cpp:348-375` ⟵ `src/FileReader_Macros.cpp:314-341`
  - [diverged within-file w=0.97 line=0.67] `src/Parser_Templates_Inst_ClassTemplate.cpp:2891-2925` ⟵ `src/Parser_Templates_Inst_ClassTemplate.cpp:162-196`
  - [diverged within-file w=0.96 line=0.76] `src/SemanticAnalysis.cpp:539-584` ⟵ `src/SemanticAnalysis.cpp:322-367`
  - [near within-file w=0.96 line=0.87] `src/Parser_Templates_Lazy.cpp:1454-1496` ⟵ `src/Parser_Templates_Lazy.cpp:1321-1361`
  - [near cross-file w=0.95 line=0.83] `src/Parser_Templates_Inst_MemberFunc.cpp:1824-1850` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:2208-2233`
  - [near cross-file w=0.95 line=0.83] `src/Parser_Templates_Inst_Deduction.cpp:2231-2257` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:759-784`
  - [diverged within-file w=0.94 line=0.71] `src/TemplateTypes.h:381-387` ⟵ `src/TemplateTypes.h:277-283`
  - [near within-file w=0.94 line=0.80] `src/Parser_Templates_Substitution.cpp:945-982` ⟵ `src/Parser_Templates_Substitution.cpp:766-805`
  - [near within-file w=0.94 line=0.86] `src/IrGenerator_Visitors_TypeInit.cpp:388-440` ⟵ `src/IrGenerator_Visitors_TypeInit.cpp:284-336`
  - [diverged within-file w=0.94 line=0.74] `src/Parser_Templates_Substitution.cpp:843-868` ⟵ `src/Parser_Templates_Substitution.cpp:662-687`
  - [near within-file w=0.94 line=0.90] `src/Parser_Expr_QualLookup.cpp:2107-2144` ⟵ `src/Parser_Expr_QualLookup.cpp:1450-1491`
  - [near within-file w=0.94 line=0.90] `src/SemanticAnalysis.cpp:5730-5769` ⟵ `src/SemanticAnalysis.cpp:4124-4157`
  - [diverged within-file w=0.93 line=0.69] `src/SemanticAnalysis.cpp:2337-2357` ⟵ `src/SemanticAnalysis.cpp:1810-1826`
  - [diverged within-file w=0.93 line=0.70] `src/SemanticAnalysis.cpp:466-503` ⟵ `src/SemanticAnalysis.cpp:246-284`
  - [near within-file w=0.93 line=0.88] `src/Parser_Templates_Inst_Deduction.cpp:909-964` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:482-534`
  - [near within-file w=0.93 line=0.94] `src/Parser_Templates_Inst_MemberFunc.cpp:2530-2572` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:1351-1393`
  - [diverged within-file w=0.93 line=0.74] `src/SemanticAnalysis.cpp:597-617` ⟵ `src/SemanticAnalysis.cpp:380-403`
  - [diverged within-file w=0.93 line=0.60] `src/SemanticAnalysis.cpp:1428-1452` ⟵ `src/SemanticAnalysis.cpp:1204-1228`
  - [near within-file w=0.93 line=0.93] `src/Parser_Expr_PrimaryUnary.cpp:361-431` ⟵ `src/Parser_Expr_PrimaryUnary.cpp:250-315`
  - [near within-file w=0.93 line=0.94] `src/Parser_Decl_StructEnum.cpp:1963-2002` ⟵ `src/Parser_Decl_StructEnum.cpp:1817-1856`
  - [diverged within-file w=0.92 line=0.67] `src/SemanticAnalysis.cpp:7559-7594` ⟵ `src/SemanticAnalysis.cpp:5693-5727`
  - [near within-file w=0.92 line=0.90] `src/SemanticAnalysis.cpp:6149-6225` ⟵ `src/SemanticAnalysis.cpp:4477-4555`
  - [near within-file w=0.92 line=0.88] `src/IrGenerator_Stmt_Decl.cpp:2387-2411` ⟵ `src/IrGenerator_Stmt_Decl.cpp:2227-2250`
  - [diverged within-file w=0.92 line=0.79] `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:123-178` ⟵ `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:50-107`
  - [diverged within-file w=0.91 line=0.78] `src/ConstExprEvaluator_Core.cpp:5238-5259` ⟵ `src/ConstExprEvaluator_Core.cpp:4473-4492`
  - [diverged within-file w=0.91 line=0.61] `src/Parser_Templates_Inst_Deduction.cpp:436-465` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:3532-3567`
  - [near within-file w=0.91 line=0.83] `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:1685-1731` ⟵ `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:1523-1567`
  - [near within-file w=0.90 line=0.86] `src/Parser.h:510-558` ⟵ `src/Parser.h:401-442`
  - [near within-file w=0.89 line=0.90] `src/Parser_TypeSpecifiers.cpp:2452-2525` ⟵ `src/Parser_TypeSpecifiers.cpp:1782-1851`
  - [diverged within-file w=0.89 line=0.78] `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:788-819` ⟵ `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:642-671`
  - [diverged within-file w=0.89 line=0.66] `src/IrGenerator_Visitors_Decl.cpp:3391-3436` ⟵ `src/IrGenerator_Visitors_Decl.cpp:3104-3149`
  - [diverged within-file w=0.88 line=0.74] `src/IrGenerator_Call_Direct.cpp:1163-1208` ⟵ `src/IrGenerator_Call_Direct.cpp:1242-1284`
  - [near within-file w=0.87 line=0.92] `src/Parser_Expr_BinaryPrecedence.cpp:824-856` ⟵ `src/Parser_Expr_BinaryPrecedence.cpp:735-767`
  - [near within-file w=0.86 line=0.88] `src/IrGenerator_Visitors_TypeInit.cpp:1212-1247` ⟵ `src/IrGenerator_Visitors_TypeInit.cpp:931-962`
  - [diverged within-file w=0.86 line=0.45] `src/CallNodeHelpers.h:148-159` ⟵ `src/CallNodeHelpers.h:108-119`
  - [near within-file w=0.85 line=0.85] `src/Parser_Templates_Inst_Deduction.cpp:4246-4303` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:2621-2679`
  - [near within-file w=0.85 line=0.83] `src/Parser.h:2328-2370` ⟵ `src/Parser.h:1686-1723`
  - [near within-file w=0.85 line=0.91] `src/Parser_Templates_Substitution.cpp:373-419` ⟵ `src/Parser_Templates_Substitution.cpp:330-370`
  - [diverged within-file w=0.85 line=0.78] `src/IrGenerator_Visitors_TypeInit.cpp:1121-1186` ⟵ `src/IrGenerator_Visitors_TypeInit.cpp:840-907`
  - [near within-file w=0.85 line=0.82] `src/ConstExprEvaluator_Core.cpp:5839-5857` ⟵ `src/ConstExprEvaluator_Core.cpp:4827-4846`
  - [diverged within-file w=0.82 line=0.60] `src/FileReader_Macros.cpp:740-771` ⟵ `src/FileReader_Macros.cpp:685-714`
  - [diverged within-file w=0.82 line=0.77] `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:2936-2988` ⟵ `tests/FlashCppTest/FlashCppTest/FlashCppTest/FlashCppTest.cpp:2610-2661`
  - [near within-file w=0.81 line=0.80] `src/InlineVector.h:555-564` ⟵ `src/InlineVector.h:388-397`
  - [near within-file w=0.81 line=0.82] `src/IrGenerator_Visitors_Decl.cpp:1461-1510` ⟵ `src/IrGenerator_Visitors_Decl.cpp:1299-1343`
  - [near within-file w=0.80 line=0.82] `src/IrGenerator_Call_Direct.cpp:1043-1054` ⟵ `src/IrGenerator_Call_Direct.cpp:1020-1031`
  - [diverged within-file w=0.80 line=0.65] `src/Parser_Templates_Lazy.cpp:335-394` ⟵ `src/Parser_Templates_Lazy.cpp:243-293`
  - [diverged within-file w=0.80 line=0.78] `src/ConstExprEvaluator_Members.cpp:2580-2631` ⟵ `src/ConstExprEvaluator_Members.cpp:1435-1468`
  - [near within-file w=0.79 line=0.88] `src/FileReader_Core.cpp:154-227` ⟵ `src/FileReader_Core.cpp:46-111`
  - [diverged within-file w=0.79 line=0.79] `src/Parser_Templates_Substitution.cpp:1316-1363` ⟵ `src/Parser_Templates_Substitution.cpp:1123-1182`
  - [diverged within-file w=0.79 line=0.80] `src/Parser_Templates_Inst_ClassTemplate.cpp:9979-10049` ⟵ `src/Parser_Templates_Inst_ClassTemplate.cpp:5218-5274`
  - [diverged within-file w=0.78 line=0.77] `src/ConstExprEvaluator_Members.cpp:7424-7458` ⟵ `src/ConstExprEvaluator_Members.cpp:5112-5137`
  - [diverged within-file w=0.78 line=0.74] `src/Parser_Templates_Substitution.cpp:1513-1537` ⟵ `src/Parser_Templates_Substitution.cpp:1231-1255`
  - [diverged within-file w=0.77 line=0.16] `src/SemanticAnalysis.cpp:1385-1425` ⟵ `src/SemanticAnalysis.cpp:1169-1201`
  - [diverged within-file w=0.76 line=0.70] `src/ConstExprEvaluator_Core.cpp:2863-2903` ⟵ `src/ConstExprEvaluator_Core.cpp:2468-2494`
  - [near within-file w=0.76 line=0.82] `src/SemanticAnalysis.cpp:5170-5232` ⟵ `src/SemanticAnalysis.cpp:3621-3670`
  - [diverged within-file w=0.76 line=0.67] `src/Parser_Expr_PrimaryExpr.cpp:5741-5785` ⟵ `src/Parser_Expr_PrimaryExpr.cpp:4651-4683`
  - [diverged within-file w=0.75 line=0.73] `src/Parser_Templates_Inst_MemberFunc.cpp:1948-1986` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:888-915`
  - [diverged within-file w=0.75 line=0.75] `src/Parser_Statements.cpp:1276-1345` ⟵ `src/Parser_Statements.cpp:927-1003`
  - [diverged within-file w=0.73 line=0.52] `src/CallNodeHelpers.h:59-77` ⟵ `src/CallNodeHelpers.h:44-56`
  - [diverged within-file w=0.72 line=0.75] `src/SemanticAnalysis.cpp:3769-3803` ⟵ `src/SemanticAnalysis.cpp:2638-2667`
  - [diverged within-file w=0.71 line=0.71] `src/Parser_Templates_Inst_ClassTemplate.cpp:4181-4210` ⟵ `src/Parser_Templates_Inst_ClassTemplate.cpp:1004-1028`
  - [diverged within-file w=0.71 line=0.74] `src/Parser_TypeSpecifiers.cpp:1547-1572` ⟵ `src/Parser_TypeSpecifiers.cpp:1123-1146`
  - [diverged cross-file w=0.70 line=0.21] `tests/for_loops_ret89.cpp:82-95` ⟵ `tests/for_loops_simple_ret85.cpp:77-85`
  - [diverged within-file w=0.69 line=0.17] `src/TemplateRegistry_Types.h:1129-1169` ⟵ `src/TemplateRegistry_Types.h:822-845`
  - [diverged within-file w=0.68 line=0.63] `src/ConstExprEvaluator_Members.cpp:1111-1150` ⟵ `src/ConstExprEvaluator_Members.cpp:450-482`
  - [diverged within-file w=0.67 line=0.61] `src/SemanticAnalysis.cpp:5258-5314` ⟵ `src/SemanticAnalysis.cpp:3696-3744`
  - [diverged within-file w=0.66 line=0.23] `src/Parser_Templates_Inst_MemberFunc.cpp:1349-1361` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:429-446`
  - [diverged within-file w=0.65 line=0.72] `src/SemanticAnalysis.cpp:4762-4813` ⟵ `src/SemanticAnalysis.cpp:3281-3314`
  - [diverged cross-file w=0.64 line=0.16] `src/Parser_Templates_Inst_Deduction.cpp:2363-2388` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:1351-1393`
  - [diverged within-file w=0.63 line=0.40] `src/IrGenerator_Stmt_Decl.cpp:3560-3611` ⟵ `src/IrGenerator_Stmt_Decl.cpp:3460-3519`
  - [diverged cross-file w=0.62 line=0.29] `src/ExpressionSubstitutor.cpp:3394-3437` ⟵ `src/Parser_Templates_Substitution.cpp:662-687`
  - [diverged within-file w=0.61 line=0.68] `src/Parser_Core.cpp:846-876` ⟵ `src/Parser_Core.cpp:655-677`
  - [diverged within-file w=0.61 line=0.39] `src/Parser_Templates_Inst_Deduction.cpp:3691-3753` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:1912-1943`

## boydsoftprez/NereusSDR — 2788 коммитов | циклы=0(SCC≤1) | копипаст=21

### `a513bf726d58` — "Merge pull request #286 from boydsoftprez/fix/audio-ring-overrun-protection" (JJ Boyd ~ KG4VCF, 2026-05-27) [21 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/boydsoftprez_NereusSDR show a513bf726d58`
  - [diverged within-file w=1.00 line=0.59] `src/gui/meters/MeterItem.cpp:1675-1701` ⟵ `src/gui/meters/MeterItem.cpp:732-758`
  - [verbatim within-file w=0.97 line=0.95] `src/gui/meters/MeterItem.cpp:1503-1555` ⟵ `src/gui/meters/MeterItem.cpp:601-649`
  - [verbatim within-file w=0.95 line=0.96] `src/core/ReceiverManager.cpp:336-367` ⟵ `src/core/ReceiverManager.cpp:199-229`
  - [diverged within-file w=0.93 line=0.67] `src/gui/containers/FloatingContainer.cpp:94-122` ⟵ `src/gui/containers/FloatingContainer.cpp:38-57`
  - [near within-file w=0.92 line=0.81] `src/core/RxChannel.cpp:1384-1403` ⟵ `src/core/RxChannel.cpp:135-148`
  - [diverged within-file w=0.91 line=0.66] `src/gui/meters/MeterItem.cpp:1804-1844` ⟵ `src/gui/meters/MeterItem.cpp:851-882`
  - [diverged within-file w=0.87 line=0.70] `src/gui/MainWindow.cpp:4843-4898` ⟵ `src/gui/MainWindow.cpp:401-444`
  - [diverged within-file w=0.85 line=0.68] `src/core/P2RadioConnection.cpp:1549-1647` ⟵ `src/core/P2RadioConnection.cpp:322-396`
  - [near within-file w=0.84 line=0.91] `src/gui/meters/MeterWidget.cpp:849-861` ⟵ `src/gui/meters/MeterWidget.cpp:432-443`
  - [near within-file w=0.82 line=0.81] `src/gui/SpectrumWidget.cpp:5484-5525` ⟵ `src/gui/SpectrumWidget.cpp:866-896`
  - [diverged within-file w=0.81 line=0.67] `src/gui/widgets/VfoWidget.cpp:650-692` ⟵ `src/gui/widgets/VfoWidget.cpp:219-265`
  - [diverged within-file w=0.80 line=0.67] `src/gui/containers/ContainerManager.cpp:561-603` ⟵ `src/gui/containers/ContainerManager.cpp:336-366`
  - [diverged within-file w=0.79 line=0.74] `src/core/FFTEngine.cpp:277-319` ⟵ `src/core/FFTEngine.cpp:88-122`
  - [near within-file w=0.79 line=0.82] `src/core/FFTEngine.cpp:77-91` ⟵ `src/core/FFTEngine.cpp:17-28`
  - [diverged within-file w=0.75 line=0.71] `src/gui/SpectrumWidget.cpp:6925-6958` ⟵ `src/gui/SpectrumWidget.cpp:1697-1718`
  - [diverged within-file w=0.75 line=0.40] `src/models/RadioModel.cpp:3211-3227` ⟵ `src/models/RadioModel.cpp:94-99`
  - [diverged within-file w=0.68 line=0.28] `src/core/FFTEngine.cpp:223-273` ⟵ `src/core/FFTEngine.cpp:59-84`
  - [diverged within-file w=0.67 line=0.37] `src/models/RadioModel.cpp:2488-2520` ⟵ `src/models/RadioModel.cpp:52-64`
  - [diverged within-file w=0.65 line=0.51] `src/core/WdspEngine.cpp:366-439` ⟵ `src/core/WdspEngine.cpp:204-275`
  - [diverged within-file w=0.63 line=0.29] `src/gui/widgets/VfoWidget.cpp:449-507` ⟵ `src/gui/widgets/VfoWidget.cpp:91-136`
  - [diverged within-file w=0.62 line=0.41] `src/core/AppSettings.cpp:394-438` ⟵ `src/core/AppSettings.cpp:30-75`

## AlchemyViewer/Alchemy — 2739 коммитов | циклы=2(SCC≤2) | копипаст=30
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/AlchemyViewer_Alchemy`

### `089faea959a6` — "Update velopack to 1.1.1" (Rye, 2026-05-31) [30 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/AlchemyViewer_Alchemy show 089faea959a6`
  - [diverged within-file w=1.00 line=0.47] `indra/llphysicsextensionsos/llconvexdecompositionvhacd.cpp:239-265` ⟵ `indra/llphysicsextensionsos/llconvexdecompositionvhacd.cpp:175-201`
  - [diverged cross-file w=1.00 line=0.54] `indra/newview/llbugsplat_mac.cpp:80-94` ⟵ `indra/newview/llappviewermacosx.cpp:215-229`
  - [verbatim cross-file w=1.00 line=1.00] `indra/newview/llappviewersdl.cpp:544-563` ⟵ `indra/llwindow/llsdl.cpp:44-63`
  - [near within-file w=1.00 line=0.90] `indra/llrender/llimagegl.cpp:2764-2791` ⟵ `indra/llrender/llimagegl.cpp:2625-2652`
  - [diverged cross-file w=0.99 line=0.54] `indra/llaudio/llwavfile.cpp:84-135` ⟵ `indra/llaudio/llaudioengine_openal.cpp:445-496`
  - [near within-file w=0.99 line=0.90] `indra/llwindow/llwindowwin32.cpp:4230-4258` ⟵ `indra/llwindow/llwindowwin32.cpp:4157-4183`
  - [near within-file w=0.98 line=0.92] `indra/newview/llscripteditorws.cpp:777-830` ⟵ `indra/newview/llscripteditorws.cpp:689-741`
  - [diverged within-file w=0.98 line=0.62] `indra/llwindow/llwindowwin32.cpp:4276-4283` ⟵ `indra/llwindow/llwindowwin32.cpp:4199-4204`
  - [near within-file w=0.97 line=0.86] `indra/llwindow/llwindowsdl.cpp:1379-1414` ⟵ `indra/llwindow/llwindowsdl.cpp:982-1015`
  - [diverged within-file w=0.96 line=0.68] `indra/llaudio/llaudioengine.cpp:2084-2136` ⟵ `indra/llaudio/llaudioengine.cpp:1880-1927`
  - [near within-file w=0.95 line=0.80] `indra/llwindow/llwindowwin32.cpp:4564-4587` ⟵ `indra/llwindow/llwindowwin32.cpp:4481-4500`
  - [near within-file w=0.95 line=0.92] `indra/llrender/llfontregistry.cpp:1469-1560` ⟵ `indra/llrender/llfontregistry.cpp:1212-1297`
  - [diverged within-file w=0.94 line=0.79] `indra/llaudio/llaudioengine.cpp:631-689` ⟵ `indra/llaudio/llaudioengine.cpp:578-628`
  - [diverged within-file w=0.92 line=0.76] `indra/llrender/llfontregistry.cpp:1410-1445` ⟵ `indra/llrender/llfontregistry.cpp:1161-1188`
  - [near within-file w=0.91 line=0.89] `indra/llwindow/llwindowsdl.cpp:2960-3001` ⟵ `indra/llwindow/llwindowsdl.cpp:1745-1785`
  - [diverged within-file w=0.91 line=0.77] `indra/llui/lllineeditor.cpp:1143-1233` ⟵ `indra/llui/lllineeditor.cpp:1030-1110`
  - [diverged within-file w=0.91 line=0.74] `indra/llaudio/llaudioengine_fmodstudio.cpp:575-623` ⟵ `indra/llaudio/llaudioengine_fmodstudio.cpp:300-349`
  - [near within-file w=0.89 line=0.94] `indra/llaudio/llaudioengine_openal.cpp:219-246` ⟵ `indra/llaudio/llaudioengine_openal.cpp:160-185`
  - [diverged within-file w=0.89 line=0.71] `indra/llwindow/llwindowsdl.cpp:558-601` ⟵ `indra/llwindow/llwindowsdl.cpp:431-468`
  - [diverged within-file w=0.88 line=0.48] `indra/llrender/llfontregistry.cpp:900-963` ⟵ `indra/llrender/llfontregistry.cpp:778-832`
  - [near within-file w=0.85 line=0.88] `indra/newview/llviewerwindow.cpp:6531-6558` ⟵ `indra/newview/llviewerwindow.cpp:6504-6525`
  - [near within-file w=0.84 line=0.91] `indra/llwindow/llwindowwin32.cpp:3397-3438` ⟵ `indra/llwindow/llwindowwin32.cpp:3340-3373`
  - [diverged within-file w=0.83 line=0.50] `indra/llwindow/llwindowsdl.cpp:441-464` ⟵ `indra/llwindow/llwindowsdl.cpp:352-372`
  - [diverged within-file w=0.82 line=0.79] `indra/llaudio/llaudioengine_fmodstudio.cpp:628-644` ⟵ `indra/llaudio/llaudioengine_fmodstudio.cpp:354-371`
  - [diverged within-file w=0.82 line=0.67] `indra/llimagej2coj/llimagej2coj.cpp:580-613` ⟵ `indra/llimagej2coj/llimagej2coj.cpp:500-523`
  - [diverged within-file w=0.81 line=0.38] `indra/llui/lltextbase.cpp:2722-2767` ⟵ `indra/llui/lltextbase.cpp:2684-2712`
  - [diverged within-file w=0.76 line=0.34] `indra/llimagej2coj/llimagej2coj.cpp:832-885` ⟵ `indra/llimagej2coj/llimagej2coj.cpp:639-768`
  - [diverged within-file w=0.74 line=0.57] `indra/llaudio/llaudioengine.cpp:1538-1586` ⟵ `indra/llaudio/llaudioengine.cpp:1363-1391`
  - [diverged within-file w=0.73 line=0.57] `indra/llwindow/llwindowsdl.cpp:1063-1071` ⟵ `indra/llwindow/llwindowsdl.cpp:819-825`
  - [diverged within-file w=0.61 line=0.50] `indra/llcommon/hbxxh.cpp:311-319` ⟵ `indra/llcommon/hbxxh.cpp:328-341`

## viperx1/Usagi-dono — 2640 коммитов | циклы=0(SCC≤1) | копипаст=13

### `fc9decc85ebb` — "Merge pull request #1023 from viperx1/copilot/fix-log-truncation-issue-again" (viperx1, 2026-02-26) [13 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/viperx1_Usagi-dono show fc9decc85ebb`
  - [verbatim within-file w=1.00 line=1.00] `usagi/src/window.cpp:2168-2178` ⟵ `usagi/src/window.cpp:632-642`
  - [near within-file w=0.99 line=0.92] `usagi/src/window.cpp:2182-2225` ⟵ `usagi/src/window.cpp:646-689`
  - [diverged within-file w=0.85 line=0.74] `usagi/src/window.cpp:2843-2896` ⟵ `usagi/src/window.cpp:950-1003`
  - [diverged within-file w=0.83 line=0.67] `usagi/src/anidbapi.cpp:4117-4123` ⟵ `usagi/src/anidbapi.cpp:1657-1663`
  - [near within-file w=0.79 line=0.80] `usagi/src/window.cpp:2905-2930` ⟵ `usagi/src/window.cpp:1012-1037`
  - [diverged within-file w=0.77 line=0.78] `usagi/src/window.cpp:2789-2823` ⟵ `usagi/src/window.cpp:896-930`
  - [near within-file w=0.76 line=0.86] `usagi/src/anidbapi.cpp:1510-1558` ⟵ `usagi/src/anidbapi.cpp:713-761`
  - [diverged within-file w=0.72 line=0.75] `usagi/src/anidbapi.cpp:4159-4205` ⟵ `usagi/src/anidbapi.cpp:1699-1745`
  - [diverged cross-file w=0.68 line=0.52] `usagi/src/hashercoordinator.cpp:227-259` ⟵ `usagi/src/window.cpp:357-394`
  - [diverged within-file w=0.65 line=0.64] `usagi/src/anidbapi.cpp:1611-1620` ⟵ `usagi/src/anidbapi.cpp:814-823`
  - [diverged within-file w=0.65 line=0.48] `usagi/src/anidbapi.cpp:467-506` ⟵ `usagi/src/anidbapi.cpp:172-204`
  - [diverged cross-file w=0.65 line=0.35] `usagi/src/hashercoordinator.cpp:209-223` ⟵ `usagi/src/window.cpp:338-353`
  - [diverged within-file w=0.62 line=0.68] `usagi/src/anidbapi.cpp:2129-2156` ⟵ `usagi/src/anidbapi.cpp:1118-1136`

## NamecoinGithub/NexusMiner — 2417 коммитов | циклы=0(SCC≤1) | копипаст=22

### `d95c17e42883` — "Fix stale target_length when nbits changes on same base_hash #706 from NamecoinGithub/copilot/a44e71ef-3448-4cb2-80a9-f38bed797e7c" (NamecoinGithub, 2026-05-27) [22 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/NamecoinGithub_NexusMiner show d95c17e42883`
  - [verbatim within-file w=1.00 line=1.00] `src/protocol/src/protocol/solo.cpp:1061-1067` ⟵ `src/protocol/src/protocol/solo.cpp:1166-1172`
  - [diverged within-file w=1.00 line=0.75] `src/protocol/node_session_context_test.cpp:45-53` ⟵ `src/protocol/node_session_context_test.cpp:34-42`
  - [diverged within-file w=0.96 line=0.60] `src/worker_manager.cpp:1692-1695` ⟵ `src/worker_manager.cpp:967-970`
  - [diverged within-file w=0.93 line=0.76] `src/cpu/src/cpu/prime/chain_sieve.cpp:476-572` ⟵ `src/cpu/src/cpu/prime/chain_sieve.cpp:313-394`
  - [near within-file w=0.89 line=0.92] `src/protocol/genesis_serialization_utils_test.cpp:319-337` ⟵ `src/protocol/genesis_serialization_utils_test.cpp:160-177`
  - [diverged within-file w=0.85 line=0.71] `src/gpu/src/gpu/worker_prime.cpp:185-190` ⟵ `src/gpu/src/gpu/worker_prime.cpp:218-223`
  - [near within-file w=0.85 line=0.89] `src/protocol/node_shutdown_wire_test.cpp:244-266` ⟵ `src/protocol/node_shutdown_wire_test.cpp:192-212`
  - [near within-file w=0.84 line=0.88] `src/protocol/packet_builder_test.cpp:313-343` ⟵ `src/protocol/packet_builder_test.cpp:239-266`
  - [diverged within-file w=0.82 line=0.32] `src/protocol/src/protocol/mining_template_interface.cpp:1570-1604` ⟵ `src/protocol/src/protocol/mining_template_interface.cpp:1176-1190`
  - [diverged within-file w=0.82 line=0.68] `src/gpu/src/gpu/worker_hash.cpp:202-259` ⟵ `src/gpu/src/gpu/worker_hash.cpp:126-175`
  - [diverged within-file w=0.80 line=0.64] `src/gpu/src/gpu/worker_hash.cpp:136-197` ⟵ `src/gpu/src/gpu/worker_hash.cpp:77-121`
  - [near within-file w=0.78 line=0.81] `src/protocol/height_tracker_test.cpp:1917-1985` ⟵ `src/protocol/height_tracker_test.cpp:1383-1439`
  - [diverged within-file w=0.75 line=0.59] `src/protocol/src/protocol/solo.cpp:4418-4450` ⟵ `src/protocol/src/protocol/solo.cpp:2725-2751`
  - [diverged within-file w=0.73 line=0.79] `src/protocol/stateless_block_utility_test.cpp:555-596` ⟵ `src/protocol/stateless_block_utility_test.cpp:397-431`
  - [diverged within-file w=0.69 line=0.73] `src/LLP/packet_framing_test.cpp:1761-1816` ⟵ `src/LLP/packet_framing_test.cpp:1190-1233`
  - [diverged within-file w=0.68 line=0.60] `src/cpu/src/cpu/prime_validation.cpp:89-102` ⟵ `src/cpu/src/cpu/prime_validation.cpp:75-84`
  - [diverged within-file w=0.68 line=0.28] `src/protocol/src/protocol/solo.cpp:5473-5513` ⟵ `src/protocol/src/protocol/solo.cpp:3531-3550`
  - [diverged within-file w=0.67 line=0.58] `src/gpu/src/gpu/worker_hash.cpp:107-131` ⟵ `src/gpu/src/gpu/worker_hash.cpp:53-72`
  - [diverged within-file w=0.66 line=0.75] `src/protocol/session_status_wire_test.cpp:373-402` ⟵ `src/protocol/session_status_wire_test.cpp:206-229`
  - [diverged within-file w=0.65 line=0.62] `src/cpu/src/cpu/worker_hash.cpp:80-148` ⟵ `src/cpu/src/cpu/worker_hash.cpp:149-219`
  - [diverged within-file w=0.65 line=0.62] `src/cpu/src/cpu/worker_hash.cpp:153-224` ⟵ `src/cpu/src/cpu/worker_hash.cpp:77-144`
  - [diverged within-file w=0.61 line=0.69] `src/protocol/e2e_block_submission_test.cpp:730-780` ⟵ `src/protocol/e2e_block_submission_test.cpp:478-516`

## danielraffel/pulp — 2394 коммитов | циклы=0(SCC≤1) | копипаст=18

### `93d0117ccbf7` — "docs: regenerate changelog for v0.303.0 [skip ci]" (pulp-release-bot, 2026-06-01) [18 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/danielraffel_pulp show 93d0117ccbf7`
  - [diverged within-file w=1.00 line=0.09] `core/view/src/claude_bundle.cpp:590-596` ⟵ `core/view/src/claude_bundle.cpp:520-526`
  - [verbatim cross-file w=1.00 line=1.00] `core/view/src/design_ir_json.cpp:1357-1388` ⟵ `core/view/src/design_import.cpp:1267-1298`
  - [verbatim cross-file w=1.00 line=1.00] `core/view/src/design_ir_json.cpp:1491-1504` ⟵ `core/view/src/design_import.cpp:1401-1414`
  - [verbatim cross-file w=1.00 line=1.00] `core/view/src/design_ir_json.cpp:1555-1570` ⟵ `core/view/src/design_import.cpp:1465-1480`
  - [near within-file w=1.00 line=0.93] `test/test_pulp_bridge.cpp:224-260` ⟵ `test/test_pulp_bridge.cpp:173-209`
  - [near within-file w=1.00 line=0.86] `test/test_inspector.cpp:2253-2292` ⟵ `test/test_inspector.cpp:1968-2008`
  - [diverged within-file w=0.90 line=0.79] `core/runtime/src/zip.cpp:336-359` ⟵ `core/runtime/src/zip.cpp:302-323`
  - [near within-file w=0.88 line=0.85] `test/test_mcp_server.cpp:249-290` ⟵ `test/test_mcp_server.cpp:187-224`
  - [near within-file w=0.88 line=0.88] `core/events/src/interprocess_connection.cpp:374-432` ⟵ `core/events/src/interprocess_connection.cpp:317-370`
  - [diverged within-file w=0.87 line=0.32] `test/test_inspector.cpp:224-254` ⟵ `test/test_inspector.cpp:1843-1870`
  - [diverged within-file w=0.87 line=0.72] `tools/cli/cmd_ship.cpp:666-683` ⟵ `tools/cli/cmd_ship.cpp:494-508`
  - [diverged within-file w=0.79 line=0.77] `test/test_inspector_server.cpp:394-419` ⟵ `test/test_inspector_server.cpp:165-186`
  - [diverged within-file w=0.68 line=0.45] `core/format/src/host_quirks.cpp:256-285` ⟵ `core/format/src/host_quirks.cpp:43-60`
  - [diverged within-file w=0.68 line=0.56] `core/view/src/yoga_layout.cpp:193-212` ⟵ `core/view/src/yoga_layout.cpp:218-229`
  - [diverged within-file w=0.68 line=0.48] `test/test_inspector_server.cpp:256-281` ⟵ `test/test_inspector_server.cpp:190-213`
  - [diverged within-file w=0.67 line=0.50] `test/test_events.cpp:756-775` ⟵ `test/test_events.cpp:118-131`
  - [diverged within-file w=0.65 line=0.54] `tools/import-design/pulp_import_design.cpp:1856-1897` ⟵ `tools/import-design/pulp_import_design.cpp:1282-1327`
  - [diverged within-file w=0.60 line=0.29] `test/test_inspector.cpp:1939-1965` ⟵ `test/test_inspector.cpp:1843-1870`

## CrispStrobe/CrispASR — 2292 коммитов | циклы=1(SCC≤2) | копипаст=7
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/CrispStrobe_CrispASR`

### `9d259a9eac26` — "docs: finalize funasr CUDA §136 — v16 confirmed, full investigation writeup" (crispasr integration, 2026-06-01) [7 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/CrispStrobe_CrispASR show 9d259a9eac26`
  - [near within-file w=1.00 line=0.90] `examples/cli/crispasr_backend_kyutai_stt.cpp:101-121` ⟵ `examples/cli/crispasr_backend_kyutai_stt.cpp:56-76`
  - [near within-file w=0.93 line=0.95] `ggml/src/ggml-backend.cpp:1746-1775` ⟵ `ggml/src/ggml-backend.cpp:1685-1713`
  - [near within-file w=0.91 line=0.93] `src/crispasr_c_api.cpp:3060-3089` ⟵ `src/crispasr_c_api.cpp:2688-2715`
  - [near within-file w=0.91 line=0.93] `src/crispasr_c_api.cpp:2836-2865` ⟵ `src/crispasr_c_api.cpp:2470-2497`
  - [near within-file w=0.83 line=0.82] `src/crispasr_c_api.cpp:4470-4502` ⟵ `src/crispasr_c_api.cpp:4062-4088`
  - [diverged within-file w=0.68 line=0.67] `ggml/src/ggml-alloc.c:751-782` ⟵ `ggml/src/ggml-alloc.c:692-711`
  - [diverged cross-file w=0.62 line=0.19] `src/outetts.cpp:1065-1121` ⟵ `src/orpheus.cpp:834-874`

## Krilliac/SparkEngine — 2229 коммитов | циклы=1(SCC≤2) | копипаст=106
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/Krilliac_SparkEngine`

### `1ef9347d8d78` — "chore: update lines of code counter [skip ci]" (github-actions[bot], 2026-04-19) [106 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Krilliac_SparkEngine show 1ef9347d8d78`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineWindows.cpp:289-303` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:374-388`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/MaterialSystemLinux.cpp:97-109` ⟵ `SparkEngine/Source/Graphics/MaterialSystem.cpp:989-1001`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemWindows.cpp:666-692` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:627-653`
  - [near cross-file w=1.00 line=0.93] `SparkEngine/Source/Core/Lifecycle/GameplayLifecycleShared.cpp:998-1037` ⟵ `SparkEngine/Source/Core/GameplaySystemLifecycle.cpp:640-678`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemWindows.cpp:130-146` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:125-141`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemLinux.cpp:102-110` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:904-912`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemInternalWindows.cpp:849-891` ⟵ `SparkEngine/Source/Graphics/LightingSystemInternal.cpp:844-886`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineLinux.cpp:195-209` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1032-1046`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemInternalWindows.cpp:242-309` ⟵ `SparkEngine/Source/Graphics/LightingSystemInternal.cpp:237-304`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsStateAndSettingsWindows.cpp:427-485` ⟵ `SparkEngine/Source/Graphics/GraphicsStateAndSettings.cpp:422-480`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineLinux.cpp:560-609` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1315-1364`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsStateAndSettingsWindows.cpp:339-361` ⟵ `SparkEngine/Source/Graphics/GraphicsStateAndSettings.cpp:334-356`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsStateAndSettingsWindows.cpp:250-273` ⟵ `SparkEngine/Source/Graphics/GraphicsStateAndSettings.cpp:245-268`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesWindows.cpp:607-619` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:659-671`
  - [verbatim cross-file w=1.00 line=0.95] `SparkEngine/Source/Graphics/ParticleSystemLinux.cpp:145-167` ⟵ `SparkEngine/Source/Graphics/ParticleSystem.cpp:199-222`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesWindows.cpp:358-400` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:410-452`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesLinux.cpp:384-420` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:1048-1084`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineWindows.cpp:587-606` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:587-606`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:1286-1315` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:922-951`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:988-1024` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:765-801`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineWindows.cpp:904-919` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:831-846`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:776-829` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:612-665`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsDeviceResourcesWindows.cpp:73-87` ⟵ `SparkEngine/Source/Graphics/GraphicsDeviceResources.cpp:73-87`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsConsoleOpsLinux.cpp:98-104` ⟵ `SparkEngine/Source/Graphics/GraphicsConsoleOps.cpp:616-622`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/FogSystem.cpp:76-84` ⟵ `SparkEngine/Source/Graphics/FogSystem.h:277-285`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/AssetPipelineWindows.cpp:109-164` ⟵ `SparkEngine/Source/Graphics/AssetPipeline.cpp:104-159`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/AssetPipelineLinux.cpp:116-167` ⟵ `SparkEngine/Source/Graphics/AssetPipeline.cpp:534-585`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/RenderTargetWindows.cpp:440-458` ⟵ `SparkEngine/Source/Graphics/RenderTarget.cpp:435-453`
  - [verbatim within-file w=1.00 line=1.00] `SparkEngine/Source/Input/InputManager.cpp:797-817` ⟵ `SparkEngine/Source/Input/InputManager.cpp:244-264`
  - [near within-file w=1.00 line=0.94] `SparkEngine/Source/Input/InputManager.cpp:687-732` ⟵ `SparkEngine/Source/Input/InputManager.cpp:75-122`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationLinux.cpp:93-127` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:588-622`
  - [verbatim within-file w=1.00 line=1.00] `SparkEngine/Source/Input/InputManager.cpp:668-683` ⟵ `SparkEngine/Source/Input/InputManager.cpp:56-71`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationLinux.cpp:131-188` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:626-683`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationLinux.cpp:192-218` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:687-713`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationLinux.cpp:223-246` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:718-741`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/TextureSystemLinux.cpp:288-322` ⟵ `SparkEngine/Source/Graphics/TextureSystem.cpp:914-948`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:36-84` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:151-199`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:153-186` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:268-301`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:190-238` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:305-353`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:242-253` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:357-368`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:258-282` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:373-397`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:311-316` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:426-431`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/ParticleSystemWindows.cpp:664-676` ⟵ `SparkEngine/Source/Graphics/ParticleSystem.cpp:1228-1240`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEditor/Source/Core/EditorNotificationManager.cpp:105-127` ⟵ `SparkEditor/Source/Core/EditorUI.cpp:835-857`
  - [verbatim cross-file w=1.00 line=0.95] `SparkEngine/Source/Graphics/ParticleSystemWindows.cpp:204-227` ⟵ `SparkEngine/Source/Graphics/ParticleSystem.cpp:808-830`
  - [verbatim cross-file w=1.00 line=1.00] `SparkEngine/Source/Graphics/ParticleSystemLinux.cpp:566-578` ⟵ `SparkEngine/Source/Graphics/ParticleSystem.cpp:1228-1240`
  - [near cross-file w=1.00 line=0.91] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:376-410` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:485-519`
  - [verbatim within-file w=0.99 line=1.00] `SparkEngine/Source/Input/InputManager.cpp:745-785` ⟵ `SparkEngine/Source/Input/InputManager.cpp:140-184`
  - [diverged cross-file w=0.98 line=0.09] `Tests/TestNetworkSecurityPhaseHH.cpp:75-80` ⟵ `Tests/TestNetworkSecurity.cpp:108-113`
  - [near within-file w=0.98 line=0.94] `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:1010-1026` ⟵ `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:884-899`
  - [near within-file w=0.96 line=0.84] `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:569-594` ⟵ `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:472-494`
  - [near cross-file w=0.95 line=0.94] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:91-149` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:89-144`
  - [verbatim within-file w=0.95 line=0.96] `SparkEditor/Source/Gizmos/GizmoSystem.cpp:751-782` ⟵ `SparkEditor/Source/Gizmos/GizmoSystem.cpp:703-733`
  - [near within-file w=0.94 line=0.81] `SparkEngine/Source/Engine/Networking/DedicatedServer.cpp:188-217` ⟵ `SparkEngine/Source/Engine/Networking/DedicatedServer.cpp:258-288`
  - [near within-file w=0.93 line=0.90] `SparkEngine/Source/Utils/CrashHandler.cpp:1187-1211` ⟵ `SparkEngine/Source/Utils/CrashHandler.cpp:956-977`
  - [near cross-file w=0.91 line=0.86] `SparkEngine/Source/Core/Lifecycle/GameplayLifecycleShared.cpp:674-737` ⟵ `SparkEngine/Source/Core/GameplaySystemLifecycle.cpp:363-434`
  - [near within-file w=0.91 line=0.94] `SparkEditor/Source/Core/EditorUI.cpp:578-641` ⟵ `SparkEditor/Source/Core/EditorUI.cpp:508-570`
  - [diverged within-file w=0.91 line=0.71] `SparkEngine/Source/Engine/AI/AIIntegration.h:136-163` ⟵ `SparkEngine/Source/Engine/AI/AIIntegration.h:102-127`
  - [diverged within-file w=0.91 line=0.62] `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:1277-1329` ⟵ `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:1143-1174`
  - [near cross-file w=0.91 line=0.88] `SparkEditor/Source/Core/EditorApplicationWindows.cpp:320-371` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:435-480`
  - [near cross-file w=0.90 line=0.87] `SparkEditor/Source/Core/EditorApplicationLinux.cpp:268-317` ⟵ `SparkEditor/Source/Core/EditorApplication.cpp:763-806`
  - [near cross-file w=0.90 line=0.90] `SparkEngine/Source/Core/SparkEngineLinux.cpp:752-787` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1494-1529`
  - [diverged cross-file w=0.90 line=0.62] `SparkEditor/Source/Utils/FolderDialog.cpp:25-76` ⟵ `SparkEditor/Source/Panels/ProjectBrowserPanel.cpp:496-548`
  - [near cross-file w=0.88 line=0.81] `SparkEngine/Source/Core/SparkEngineLinux.cpp:281-308` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1100-1127`
  - [diverged cross-file w=0.88 line=0.50] `GameModules/SparkGameOpenWorld/Source/Core/OWEngineSystems.cpp:83-116` ⟵ `GameModules/SparkGameRPG/Source/Core/RPGEngineSystems.cpp:95-129`
  - [diverged cross-file w=0.88 line=0.75] `SparkEngine/Source/Core/Lifecycle/GameplayLifecycleShared.cpp:753-775` ⟵ `SparkEngine/Source/Core/GameplaySystemLifecycle.cpp:438-456`
  - [diverged within-file w=0.87 line=0.76] `Tests/TestMain.cpp:163-217` ⟵ `Tests/TestMain.cpp:66-106`
  - [diverged within-file w=0.87 line=0.24] `SparkEngine/Source/Core/SparkEngine.cpp:133-143` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:121-130`
  - [diverged within-file w=0.87 line=0.70] `SparkEngine/Source/Graphics/RHI/Vulkan/VulkanDevice.cpp:928-1003` ⟵ `SparkEngine/Source/Graphics/RHI/Vulkan/VulkanDevice.cpp:747-801`
  - [near within-file w=0.87 line=0.82] `SparkEngine/Source/Engine/Scripting/AngelScriptEngine.cpp:386-417` ⟵ `SparkEngine/Source/Engine/Scripting/AngelScriptEngine.cpp:269-295`
  - [near within-file w=0.86 line=0.80] `SparkEngine/Source/Utils/CrashReportUploader.cpp:757-780` ⟵ `SparkEngine/Source/Utils/CrashReportUploader.cpp:347-369`
  - [diverged within-file w=0.86 line=0.73] `SparkEngine/Source/Engine/Networking/DedicatedServer.cpp:143-184` ⟵ `SparkEngine/Source/Engine/Networking/DedicatedServer.cpp:213-254`
  - [diverged cross-file w=0.85 line=0.68] `SparkEngine/Source/Core/Lifecycle/GameplayLifecycleShared.cpp:957-988` ⟵ `SparkEngine/Source/Core/GameplaySystemLifecycle.cpp:602-630`
  - [near within-file w=0.84 line=0.83] `SparkEngine/Source/Graphics/SceneRenderer.cpp:157-211` ⟵ `SparkEngine/Source/Graphics/SceneRenderer.cpp:97-137`
  - [near cross-file w=0.83 line=0.85] `SparkEngine/Source/Graphics/ShaderCompilationLinux.cpp:443-512` ⟵ `SparkEngine/Source/Graphics/ShaderCompilation.cpp:942-1000`
  - [diverged within-file w=0.83 line=0.67] `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:511-540` ⟵ `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:423-443`
  - [diverged cross-file w=0.83 line=0.13] `GameModules/SparkGameOpenWorld/Source/Core/OWEngineSystems.cpp:145-204` ⟵ `GameModules/SparkGameRPG/Source/Core/RPGEngineSystems.cpp:175-222`
  - [near within-file w=0.82 line=0.83] `SparkEngine/Source/Core/EngineDiagnosticsExtended.cpp:496-540` ⟵ `SparkEngine/Source/Core/EngineDiagnosticsExtended.cpp:389-425`
  - [diverged within-file w=0.81 line=0.73] `SparkEngine/Source/Engine/Networking/DedicatedServer.cpp:308-341` ⟵ `SparkEngine/Source/Engine/Networking/DedicatedServer.cpp:129-161`
  - [diverged cross-file w=0.81 line=0.50] `SparkEngine/Source/Core/SparkEngineWindows.cpp:987-1015` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:914-942`
  - [diverged within-file w=0.81 line=0.78] `SparkEngine/Source/Graphics/RHI/Vulkan/VulkanDevice.cpp:1798-1809` ⟵ `SparkEngine/Source/Graphics/RHI/Vulkan/VulkanDevice.cpp:1489-1497`
  - [diverged cross-file w=0.80 line=0.36] `SparkEditor/Source/VersionControl/VersionControlRender.cpp:53-93` ⟵ `SparkEditor/Source/Panels/ProjectBrowserPanel.cpp:496-548`
  - [diverged cross-file w=0.78 line=0.63] `GameModules/SparkGameOpenWorld/Source/Core/OWEngineSystems.cpp:34-50` ⟵ `GameModules/SparkGameRPG/Source/Core/RPGEngineSystems.cpp:35-54`
  - [near within-file w=0.78 line=0.82] `SparkEngine/Source/Utils/CrashHandler.cpp:302-316` ⟵ `SparkEngine/Source/Utils/CrashHandler.cpp:166-176`
  - [diverged cross-file w=0.77 line=0.55] `SparkEngine/Source/Core/SparkEngineLinux.cpp:622-679` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1377-1433`
  - [diverged cross-file w=0.77 line=0.57] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesLinux.cpp:56-83` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:730-747`
  - [diverged within-file w=0.76 line=0.75] `SparkEngine/Source/Graphics/RHI/D3D12/D3D12Device.cpp:1180-1187` ⟵ `SparkEngine/Source/Graphics/RHI/D3D12/D3D12Device.cpp:1099-1104`
  - [diverged cross-file w=0.76 line=0.61] `SparkEngine/Source/Graphics/GraphicsEngineLinux.cpp:447-529` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:1273-1323`
  - [diverged cross-file w=0.75 line=0.61] `SparkEngine/Source/Core/SparkEngineLinux.cpp:1025-1057` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1614-1646`
  - [diverged within-file w=0.73 line=0.53] `Tests/TestMain.cpp:360-408` ⟵ `Tests/TestMain.cpp:218-256`
  - [diverged cross-file w=0.72 line=0.46] `SparkEngine/Source/Core/SparkEngineWindows.cpp:610-644` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:610-643`
  - [diverged cross-file w=0.72 line=0.68] `SparkEngine/Source/Core/Lifecycle/GameplayLifecycleShared.cpp:1324-1345` ⟵ `SparkEngine/Source/Core/GameplaySystemLifecycle.cpp:860-875`
  - [near within-file w=0.71 line=0.80] `SparkEditor/Source/Core/EditorUI.cpp:395-434` ⟵ `SparkEditor/Source/Core/EditorUI.cpp:344-377`
  - [diverged cross-file w=0.70 line=0.08] `Tests/TestLoadingScreenReal.cpp:49-55` ⟵ `Tests/TestLoadingScreen.cpp:70-76`
  - [diverged cross-file w=0.69 line=0.67] `SparkEngine/Source/Utils/ConsoleProcessManager.cpp:349-365` ⟵ `SparkEngine/Source/Utils/ConsoleProcessManagerWin32.cpp:292-310`
  - [diverged cross-file w=0.68 line=0.51] `SparkEngine/Source/Core/SparkEngineLinux.cpp:320-370` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1139-1173`
  - [diverged cross-file w=0.68 line=0.65] `SparkEngine/Source/Graphics/LightingSystemWindows.cpp:247-313` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:228-274`
  - [diverged cross-file w=0.68 line=0.45] `SparkEngine/Source/Core/SparkEngineWindows.cpp:541-583` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:554-583`
  - [diverged cross-file w=0.67 line=0.47] `SparkEngine/Source/Utils/ConsoleProcessManager.cpp:166-239` ⟵ `SparkEngine/Source/Utils/ConsoleProcessManagerLinux.cpp:53-121`
  - [diverged within-file w=0.66 line=0.31] `SparkEngine/Source/Core/SparkEngine.cpp:235-306` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:181-227`
  - [diverged within-file w=0.66 line=0.54] `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:1333-1376` ⟵ `SparkEngine/Source/Graphics/RHI/OpenGL/OpenGLDevice.cpp:1178-1201`
  - [diverged within-file w=0.66 line=0.33] `SparkEngine/Source/Graphics/RHI/Vulkan/VulkanDevice.cpp:1571-1581` ⟵ `SparkEngine/Source/Graphics/RHI/Vulkan/VulkanDevice.cpp:1300-1305`
  - [diverged within-file w=0.65 line=0.78] `SparkEngine/Source/Engine/Editor/PlayModeManager.h:112-144` ⟵ `SparkEngine/Source/Engine/Editor/PlayModeManager.h:76-103`
  - [diverged within-file w=0.63 line=0.47] `SparkEngine/Source/Graphics/RHI/DXRSupport.cpp:346-399` ⟵ `SparkEngine/Source/Graphics/RHI/DXRSupport.cpp:217-259`
  - [diverged cross-file w=0.63 line=0.53] `SparkEngine/Source/Core/SparkEngineWindows.cpp:831-897` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:788-824`
  - [diverged within-file w=0.61 line=0.67] `GameModules/SparkGameRacing/Source/AI/RacingAIDriver.cpp:203-232` ⟵ `GameModules/SparkGameRacing/Source/AI/RacingAIDriver.cpp:153-170`

## NamecoinGithub/LLL-TAO — 2218 коммитов | циклы=1(SCC≤2) | копипаст=6
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/NamecoinGithub_LLL-TAO`

### `b62a2f180d45` — "Convert RISC-V notes into build documentation #613 from NamecoinGithub/copilot/cleanup-documentation-structure" (NamecoinGithub, 2026-05-30) [6 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/NamecoinGithub_LLL-TAO show b62a2f180d45`
  - [near within-file w=1.00 line=0.89] `src/LLP/tritium.cpp:2692-2744` ⟵ `src/LLP/tritium.cpp:2576-2628`
  - [near within-file w=0.78 line=0.83] `src/TAO/Ledger/stateless_block_utility.cpp:811-838` ⟵ `src/TAO/Ledger/stateless_block_utility.cpp:324-347`
  - [diverged within-file w=0.77 line=0.58] `src/LLP/stateless_miner_connection.cpp:3683-3696` ⟵ `src/LLP/stateless_miner_connection.cpp:2713-2722`
  - [diverged within-file w=0.76 line=0.72] `src/LLP/miner.cpp:2469-2538` ⟵ `src/LLP/miner.cpp:1360-1418`
  - [diverged within-file w=0.70 line=0.50] `src/LLP/data.cpp:831-890` ⟵ `src/LLP/data.cpp:570-602`
  - [diverged within-file w=0.68 line=0.57] `src/LLP/data.cpp:627-673` ⟵ `src/LLP/data.cpp:407-432`

## dilithion/dilithion — 2040 коммитов | циклы=0(SCC≤1) | копипаст=37

### `758e652a8d50` — "chore(gitignore): harden against repo-root scratch + local MCP config + sibling clones (#90)" (WillBarton888, 2026-05-29) [37 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/dilithion_dilithion show 758e652a8d50`
  - [verbatim within-file w=1.00 line=1.00] `src/consensus/chain.cpp:2191-2210` ⟵ `src/consensus/chain.cpp:1331-1350`
  - [near cross-file w=1.00 line=0.80] `src/wallet/crypter.h:40-56` ⟵ `src/util/secure_allocator.h:50-63`
  - [near within-file w=1.00 line=0.93] `src/rpc/server.cpp:7581-7623` ⟵ `src/rpc/server.cpp:5602-5644`
  - [verbatim within-file w=1.00 line=1.00] `src/rpc/auth.cpp:247-295` ⟵ `src/rpc/auth.cpp:153-201`
  - [verbatim within-file w=1.00 line=1.00] `src/rpc/auth.cpp:190-241` ⟵ `src/rpc/auth.cpp:96-147`
  - [verbatim within-file w=1.00 line=1.00] `src/rpc/auth.cpp:135-178` ⟵ `src/rpc/auth.cpp:41-84`
  - [near within-file w=1.00 line=0.86] `src/net/headers_manager.cpp:471-502` ⟵ `src/net/headers_manager.cpp:288-319`
  - [verbatim within-file w=1.00 line=1.00] `src/node/utxo_set.h:409-447` ⟵ `src/node/utxo_set.h:257-295`
  - [near within-file w=0.98 line=0.82] `src/net/net.cpp:2481-2524` ⟵ `src/net/net.cpp:2307-2350`
  - [diverged within-file w=0.98 line=0.79] `src/net/net.cpp:2440-2478` ⟵ `src/net/net.cpp:2266-2304`
  - [verbatim within-file w=0.95 line=0.97] `src/test/rpc_auth_tests.cpp:360-407` ⟵ `src/test/rpc_auth_tests.cpp:305-351`
  - [verbatim within-file w=0.95 line=0.96] `src/node/block_processing.cpp:536-567` ⟵ `src/node/block_processing.cpp:287-316`
  - [near within-file w=0.95 line=0.85] `src/net/peers.cpp:1700-1750` ⟵ `src/net/peers.cpp:1529-1578`
  - [verbatim within-file w=0.94 line=0.97] `src/test/timestamp_tests.cpp:394-438` ⟵ `src/test/timestamp_tests.cpp:295-338`
  - [verbatim within-file w=0.93 line=0.96] `src/node/ibd_coordinator.cpp:2110-2144` ⟵ `src/node/ibd_coordinator.cpp:1575-1607`
  - [verbatim within-file w=0.93 line=0.95] `src/net/net.cpp:793-826` ⟵ `src/net/net.cpp:741-772`
  - [near within-file w=0.93 line=0.91] `src/net/net.cpp:1416-1459` ⟵ `src/net/net.cpp:1278-1318`
  - [verbatim within-file w=0.92 line=0.98] `src/net/headers_manager.cpp:3343-3412` ⟵ `src/net/headers_manager.cpp:2877-2942`
  - [near within-file w=0.90 line=0.95] `src/net/headers_manager.cpp:841-866` ⟵ `src/net/headers_manager.cpp:617-639`
  - [near within-file w=0.90 line=0.89] `src/node/block_processing.cpp:1312-1331` ⟵ `src/node/block_processing.cpp:933-949`
  - [verbatim within-file w=0.90 line=0.96] `src/net/net.cpp:1096-1135` ⟵ `src/net/net.cpp:1025-1061`
  - [diverged within-file w=0.88 line=0.79] `src/consensus/chain.cpp:1871-1891` ⟵ `src/consensus/chain.cpp:1044-1067`
  - [near within-file w=0.87 line=0.80] `src/node/ibd_coordinator.cpp:542-547` ⟵ `src/node/ibd_coordinator.cpp:481-485`
  - [near within-file w=0.84 line=0.81] `src/net/peers.cpp:1618-1662` ⟵ `src/net/peers.cpp:1455-1491`
  - [near within-file w=0.84 line=0.80] `src/digital_dna/digital_dna_rpc.cpp:559-602` ⟵ `src/digital_dna/digital_dna_rpc.cpp:501-545`
  - [diverged within-file w=0.83 line=0.36] `src/test/utxo_tests.cpp:547-588` ⟵ `src/test/utxo_tests.cpp:460-497`
  - [diverged within-file w=0.82 line=0.53] `src/node/ibd_coordinator.cpp:2900-2961` ⟵ `src/node/ibd_coordinator.cpp:2272-2312`
  - [near within-file w=0.80 line=0.84] `src/net/connman.cpp:1680-1711` ⟵ `src/net/connman.cpp:1594-1627`
  - [diverged within-file w=0.77 line=0.59] `src/digital_dna/digital_dna_rpc.cpp:606-630` ⟵ `src/digital_dna/digital_dna_rpc.cpp:549-574`
  - [diverged within-file w=0.74 line=0.59] `src/consensus/pow.cpp:602-619` ⟵ `src/consensus/pow.cpp:531-554`
  - [diverged cross-file w=0.74 line=0.47] `src/node/dilv-node.cpp:6866-6914` ⟵ `src/node/dilithion-node.cpp:5278-5313`
  - [diverged cross-file w=0.74 line=0.47] `src/node/dilithion-node.cpp:6968-7016` ⟵ `src/node/dilv-node.cpp:4940-4975`
  - [diverged within-file w=0.69 line=0.55] `src/consensus/pow.cpp:620-646` ⟵ `src/consensus/pow.cpp:555-588`
  - [diverged within-file w=0.69 line=0.55] `src/consensus/pow.cpp:647-673` ⟵ `src/consensus/pow.cpp:589-622`
  - [diverged cross-file w=0.67 line=0.26] `src/net/socket.cpp:338-355` ⟵ `src/net/sock.cpp:30-40`
  - [diverged cross-file w=0.62 line=0.17] `src/test/crypter_tests.cpp:265-296` ⟵ `src/test/test_authenticated_encryption.cpp:203-246`
  - [diverged within-file w=0.62 line=0.43] `src/wallet/wallet.cpp:676-755` ⟵ `src/wallet/wallet.cpp:602-671`

## aethersdr/AetherSDR — 2003 коммитов | циклы=0(SCC≤1) | копипаст=109

### `42be037c42de` — "feat(peripherals): auto-reconnect LAN devices Principle V." (CJ Johnson, 2026-05-31) [109 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/aethersdr_AetherSDR show 42be037c42de`
  - [diverged within-file w=1.00 line=0.50] `src/core/AudioEngine.cpp:3340-3358` ⟵ `src/core/AudioEngine.cpp:2978-2988`
  - [near within-file w=1.00 line=0.89] `src/gui/Ax25HfPacketDecodeDialog.cpp:963-986` ⟵ `src/gui/Ax25HfPacketDecodeDialog.cpp:882-905`
  - [verbatim within-file w=1.00 line=1.00] `src/gui/MainWindow.cpp:6444-6495` ⟵ `src/gui/MainWindow.cpp:5225-5276`
  - [diverged within-file w=1.00 line=0.60] `src/gui/MainWindow.cpp:10307-10310` ⟵ `src/gui/MainWindow.cpp:8319-8322`
  - [diverged within-file w=1.00 line=0.50] `src/gui/MainWindow.cpp:13045-13051` ⟵ `src/gui/MainWindow.cpp:10622-10626`
  - [near within-file w=1.00 line=0.80] `src/gui/SpectrumWidget.cpp:5855-5863` ⟵ `src/gui/SpectrumWidget.cpp:4840-4848`
  - [near within-file w=1.00 line=0.91] `src/gui/NetworkDiagnosticsDialog.cpp:617-637` ⟵ `src/gui/NetworkDiagnosticsDialog.cpp:363-383`
  - [verbatim within-file w=1.00 line=1.00] `src/core/CwSidetonePortAudioSink.cpp:165-172` ⟵ `src/core/CwSidetonePortAudioSink.cpp:30-37`
  - [near within-file w=1.00 line=0.94] `src/core/tnc/AetherAx25LibmodemShim.cpp:120-137` ⟵ `src/core/tnc/AetherAx25LibmodemShim.cpp:63-79`
  - [verbatim within-file w=0.99 line=0.96] `src/gui/SpectrumWidget.cpp:3580-3638` ⟵ `src/gui/SpectrumWidget.cpp:2728-2786`
  - [near within-file w=0.99 line=0.82] `src/gui/SpectrumWidget.cpp:763-772` ⟵ `src/gui/SpectrumWidget.cpp:496-505`
  - [diverged within-file w=0.99 line=0.75] `src/models/XvtrPolicy.cpp:38-45` ⟵ `src/models/XvtrPolicy.cpp:26-33`
  - [near within-file w=0.99 line=0.89] `src/core/AudioEngine.cpp:4629-4651` ⟵ `src/core/AudioEngine.cpp:3963-3981`
  - [verbatim within-file w=0.99 line=0.96] `src/gui/MainWindow.cpp:7666-7691` ⟵ `src/gui/MainWindow.cpp:5795-5819`
  - [near within-file w=0.98 line=0.80] `src/gui/TitleBar.cpp:602-613` ⟵ `src/gui/TitleBar.cpp:557-568`
  - [verbatim within-file w=0.98 line=0.97] `src/gui/MainWindow.cpp:2771-2820` ⟵ `src/gui/MainWindow.cpp:2352-2398`
  - [diverged within-file w=0.97 line=0.71] `src/core/TciProtocol.cpp:532-545` ⟵ `src/core/TciProtocol.cpp:502-515`
  - [diverged within-file w=0.97 line=0.73] `src/core/TciProtocol.cpp:1421-1434` ⟵ `src/core/TciProtocol.cpp:1340-1353`
  - [near within-file w=0.97 line=0.83] `src/gui/MainWindow.cpp:10219-10230` ⟵ `src/gui/MainWindow.cpp:8233-8244`
  - [near within-file w=0.97 line=0.91] `src/gui/SpectrumWidget.cpp:4640-4675` ⟵ `src/gui/SpectrumWidget.cpp:3707-3741`
  - [diverged within-file w=0.96 line=0.71] `src/core/TciProtocol.cpp:1438-1450` ⟵ `src/core/TciProtocol.cpp:1357-1369`
  - [near within-file w=0.96 line=0.86] `src/core/WanConnection.cpp:374-427` ⟵ `src/core/WanConnection.cpp:158-204`
  - [near within-file w=0.96 line=0.93] `src/gui/MainWindow.cpp:2741-2763` ⟵ `src/gui/MainWindow.cpp:2325-2344`
  - [near within-file w=0.96 line=0.80] `src/gui/SpectrumWidget.cpp:7181-7202` ⟵ `src/gui/SpectrumWidget.cpp:6096-6113`
  - [near within-file w=0.95 line=0.85] `src/gui/CwxPanel.cpp:546-597` ⟵ `src/gui/CwxPanel.cpp:458-509`
  - [near within-file w=0.95 line=0.91] `src/gui/MainWindow.cpp:17478-17492` ⟵ `src/gui/MainWindow.cpp:14659-14672`
  - [near within-file w=0.95 line=0.91] `src/gui/MainWindow.cpp:17460-17474` ⟵ `src/gui/MainWindow.cpp:14642-14655`
  - [near within-file w=0.95 line=0.91] `src/gui/SpectrumOverlayMenu.cpp:317-374` ⟵ `src/gui/SpectrumOverlayMenu.cpp:201-258`
  - [verbatim within-file w=0.95 line=0.97] `src/models/RadioModel.cpp:3289-3344` ⟵ `src/models/RadioModel.cpp:2955-3008`
  - [near within-file w=0.95 line=0.95] `src/gui/MainWindow.cpp:11191-11233` ⟵ `src/gui/MainWindow.cpp:9066-9106`
  - [verbatim within-file w=0.95 line=0.96] `tests/async_log_writer_test.cpp:370-397` ⟵ `tests/async_log_writer_test.cpp:315-341`
  - [verbatim within-file w=0.95 line=0.96] `src/gui/MainWindow.cpp:14825-14860` ⟵ `src/gui/MainWindow.cpp:12237-12271`
  - [near within-file w=0.94 line=0.94] `src/models/RadioModel.cpp:953-993` ⟵ `src/models/RadioModel.cpp:849-887`
  - [near within-file w=0.94 line=0.84] `src/gui/SpectrumWidget.cpp:6490-6519` ⟵ `src/gui/SpectrumWidget.cpp:5412-5438`
  - [near within-file w=0.94 line=0.80] `src/gui/RadioSetupDialog.cpp:2961-2997` ⟵ `src/gui/RadioSetupDialog.cpp:2632-2669`
  - [near within-file w=0.94 line=0.89] `src/gui/SpectrumWidget.cpp:7416-7436` ⟵ `src/gui/SpectrumWidget.cpp:6320-6340`
  - [near within-file w=0.94 line=0.81] `src/gui/SpectrumWidget.cpp:4163-4178` ⟵ `src/gui/SpectrumWidget.cpp:3274-3288`
  - [near within-file w=0.94 line=0.88] `src/core/RADEEngine.cpp:148-179` ⟵ `src/core/RADEEngine.cpp:99-127`
  - [verbatim within-file w=0.94 line=0.95] `src/gui/MainWindow.cpp:1160-1205` ⟵ `src/gui/MainWindow.cpp:846-891`
  - [near within-file w=0.93 line=0.93] `src/core/SerialPortController.cpp:692-728` ⟵ `src/core/SerialPortController.cpp:635-669`
  - [near within-file w=0.93 line=0.90] `src/gui/MainWindow.cpp:2823-2837` ⟵ `src/gui/MainWindow.cpp:2401-2412`
  - [diverged within-file w=0.93 line=0.74] `src/core/tnc/AetherAx25LibmodemShim.cpp:620-659` ⟵ `src/core/tnc/AetherAx25LibmodemShim.cpp:403-440`
  - [near within-file w=0.93 line=0.86] `src/gui/SpectrumWidget.cpp:7399-7413` ⟵ `src/gui/SpectrumWidget.cpp:6303-6317`
  - [near within-file w=0.93 line=0.81] `src/gui/MainWindow.cpp:2428-2478` ⟵ `src/gui/MainWindow.cpp:2033-2078`
  - [diverged within-file w=0.92 line=0.60] `src/core/tnc/AetherAx25LibmodemShim.cpp:663-717` ⟵ `src/core/tnc/AetherAx25LibmodemShim.cpp:444-493`
  - [near within-file w=0.92 line=0.90] `src/gui/SpectrumOverlayMenu.cpp:507-526` ⟵ `src/gui/SpectrumOverlayMenu.cpp:324-343`
  - [near within-file w=0.92 line=0.84] `src/gui/VfoWidget.cpp:301-358` ⟵ `src/gui/VfoWidget.cpp:231-283`
  - [near within-file w=0.91 line=0.86] `src/gui/SpectrumWidget.cpp:5574-5587` ⟵ `src/gui/SpectrumWidget.cpp:4541-4554`
  - [near within-file w=0.91 line=0.93] `src/models/RadioModel.cpp:1766-1800` ⟵ `src/models/RadioModel.cpp:1601-1635`
  - [near within-file w=0.91 line=0.87] `src/gui/MainWindow.cpp:11073-11123` ⟵ `src/gui/MainWindow.cpp:8952-9001`
  - [diverged within-file w=0.90 line=0.71] `src/gui/SpectrumWidget.cpp:7601-7638` ⟵ `src/gui/SpectrumWidget.cpp:6504-6542`
  - [near within-file w=0.90 line=0.91] `src/core/SerialPortController.cpp:636-683` ⟵ `src/core/SerialPortController.cpp:583-626`
  - [near within-file w=0.90 line=0.84] `src/gui/MainWindow.cpp:16272-16296` ⟵ `src/gui/MainWindow.cpp:13538-13562`
  - [near within-file w=0.90 line=0.93] `src/core/AudioEngine.cpp:999-1032` ⟵ `src/core/AudioEngine.cpp:790-821`
  - [near within-file w=0.89 line=0.83] `src/gui/RxApplet.cpp:1329-1413` ⟵ `src/gui/RxApplet.cpp:1239-1312`
  - [verbatim within-file w=0.89 line=0.97] `src/core/tnc/AetherAx25LibmodemShim.cpp:505-566` ⟵ `src/core/tnc/AetherAx25LibmodemShim.cpp:290-351`
  - [near within-file w=0.89 line=0.83] `src/gui/SpectrumWidget.cpp:1949-1972` ⟵ `src/gui/SpectrumWidget.cpp:1409-1428`
  - [near within-file w=0.89 line=0.89] `src/gui/SpectrumWidget.cpp:2171-2190` ⟵ `src/gui/SpectrumWidget.cpp:1621-1640`
  - [diverged within-file w=0.88 line=0.67] `src/gui/MainWindow.cpp:10237-10251` ⟵ `src/gui/MainWindow.cpp:8251-8263`
  - [near within-file w=0.88 line=0.87] `src/gui/MainWindow.cpp:11248-11264` ⟵ `src/gui/MainWindow.cpp:9117-9131`
  - [near within-file w=0.88 line=0.86] `src/gui/SpectrumWidget.cpp:4811-4847` ⟵ `src/gui/SpectrumWidget.cpp:3853-3888`
  - [near within-file w=0.87 line=0.89] `src/core/DeviceDiagnostics.cpp:321-329` ⟵ `src/core/DeviceDiagnostics.cpp:219-226`
  - [near within-file w=0.87 line=0.89] `src/gui/SpectrumWidget.cpp:7129-7171` ⟵ `src/gui/SpectrumWidget.cpp:6044-6086`
  - [near within-file w=0.87 line=0.93] `src/gui/StripCompPanel.cpp:402-436` ⟵ `src/gui/StripCompPanel.cpp:350-382`
  - [near within-file w=0.87 line=0.82] `tests/ax25_libmodem_shim_test.cpp:676-692` ⟵ `tests/ax25_libmodem_shim_test.cpp:441-454`
  - [near within-file w=0.85 line=0.82] `src/gui/SliceTroubleshootingDialog.cpp:487-528` ⟵ `src/gui/SliceTroubleshootingDialog.cpp:388-422`
  - [near within-file w=0.85 line=0.86] `src/gui/CwxPanel.cpp:338-366` ⟵ `src/gui/CwxPanel.cpp:287-315`
  - [near within-file w=0.85 line=0.93] `src/gui/MainWindow.cpp:3693-3707` ⟵ `src/gui/MainWindow.cpp:3140-3153`
  - [diverged within-file w=0.85 line=0.76] `src/gui/CwxPanel.cpp:247-264` ⟵ `src/gui/CwxPanel.cpp:208-221`
  - [diverged within-file w=0.84 line=0.63] `src/gui/MainWindow.cpp:8416-8432` ⟵ `src/gui/MainWindow.cpp:6565-6582`
  - [near within-file w=0.84 line=0.85] `src/gui/MainWindow.cpp:7954-7969` ⟵ `src/gui/MainWindow.cpp:6082-6095`
  - [diverged within-file w=0.84 line=0.71] `src/core/AudioEngine.cpp:5003-5026` ⟵ `src/core/AudioEngine.cpp:4300-4316`
  - [near within-file w=0.84 line=0.92] `src/core/TciServer.cpp:1535-1606` ⟵ `src/core/TciServer.cpp:1250-1315`
  - [diverged within-file w=0.84 line=0.68] `src/models/RadioModel.cpp:1812-1854` ⟵ `src/models/RadioModel.cpp:1639-1677`
  - [diverged within-file w=0.83 line=0.78] `src/gui/SpectrumWidget.cpp:7547-7573` ⟵ `src/gui/SpectrumWidget.cpp:6450-6476`
  - [near within-file w=0.83 line=0.83] `src/gui/SpectrumOverlayMenu.cpp:717-748` ⟵ `src/gui/SpectrumOverlayMenu.cpp:469-495`
  - [diverged within-file w=0.83 line=0.77] `src/core/CwSidetonePortAudioSink.cpp:305-319` ⟵ `src/core/CwSidetonePortAudioSink.cpp:172-183`
  - [diverged within-file w=0.81 line=0.56] `src/gui/CwxPanel.cpp:409-441` ⟵ `src/gui/CwxPanel.cpp:359-391`
  - [diverged within-file w=0.81 line=0.57] `src/gui/MqttApplet.cpp:191-217` ⟵ `src/gui/MqttApplet.cpp:262-284`
  - [diverged within-file w=0.80 line=0.62] `src/gui/MainWindow.cpp:13055-13109` ⟵ `src/gui/MainWindow.cpp:10637-10674`
  - [diverged cross-file w=0.80 line=0.71] `src/core/ZipArchive.cpp:51-76` ⟵ `src/core/ProfileTransfer.cpp:65-90`
  - [diverged within-file w=0.80 line=0.48] `src/core/WanConnection.cpp:143-167` ⟵ `src/core/WanConnection.cpp:33-51`
  - [diverged within-file w=0.80 line=0.68] `src/gui/ProfileManagerDialog.cpp:277-312` ⟵ `src/gui/ProfileManagerDialog.cpp:250-274`
  - [near within-file w=0.79 line=0.82] `src/gui/MainWindow.cpp:14103-14172` ⟵ `src/gui/MainWindow.cpp:11549-11605`
  - [diverged cross-file w=0.79 line=0.65] `src/gui/WaveformWidget.cpp:632-664` ⟵ `src/gui/StripWaveform.cpp:635-667`
  - [diverged cross-file w=0.79 line=0.65] `src/gui/StripWaveform.cpp:637-669` ⟵ `src/gui/WaveformWidget.cpp:630-662`
  - [diverged within-file w=0.79 line=0.69] `src/models/RadioModel.cpp:3944-4006` ⟵ `src/models/RadioModel.cpp:3586-3633`
  - [near within-file w=0.78 line=0.90] `src/gui/MainWindow.cpp:2691-2722` ⟵ `src/gui/MainWindow.cpp:2275-2306`
  - [diverged within-file w=0.77 line=0.79] `src/gui/MainWindow.cpp:1101-1123` ⟵ `src/gui/MainWindow.cpp:816-842`
  - [near within-file w=0.77 line=0.83] `src/gui/StripCompPanel.cpp:374-398` ⟵ `src/gui/StripCompPanel.cpp:326-346`
  - [diverged within-file w=0.76 line=0.79] `tests/ax25_libmodem_shim_test.cpp:771-803` ⟵ `tests/ax25_libmodem_shim_test.cpp:530-556`
  - [diverged within-file w=0.76 line=0.67] `src/models/RadioModel.cpp:2545-2598` ⟵ `src/models/RadioModel.cpp:2375-2419`
  - [diverged within-file w=0.75 line=0.11] `src/gui/MainWindow.cpp:17314-17323` ⟵ `src/gui/MainWindow.cpp:11129-11139`
  - [diverged cross-file w=0.75 line=0.55] `src/gui/ClientCompCurveWidget.cpp:116-124` ⟵ `src/gui/ClientGateCurveWidget.cpp:96-105`
  - [diverged cross-file w=0.75 line=0.55] `src/gui/ClientGateCurveWidget.cpp:95-104` ⟵ `src/gui/ClientCompCurveWidget.cpp:117-125`
  - [diverged within-file w=0.75 line=0.64] `src/gui/TunerApplet.cpp:305-335` ⟵ `src/gui/TunerApplet.cpp:264-297`
  - [diverged within-file w=0.75 line=0.58] `src/gui/VfoWidget.cpp:257-295` ⟵ `src/gui/VfoWidget.cpp:204-225`
  - [diverged within-file w=0.75 line=0.58] `src/models/RadioModel.cpp:3408-3470` ⟵ `src/models/RadioModel.cpp:3072-3112`
  - [diverged within-file w=0.75 line=0.71] `src/gui/SpectrumWidget.cpp:7387-7392` ⟵ `src/gui/SpectrumWidget.cpp:6291-6296`
  - [diverged within-file w=0.72 line=0.71] `src/core/MidiControlManager.cpp:425-430` ⟵ `src/core/MidiControlManager.cpp:399-404`
  - [diverged cross-file w=0.68 line=0.37] `src/core/ZipArchive.cpp:185-229` ⟵ `src/core/ProfileTransfer.cpp:190-243`
  - [diverged within-file w=0.67 line=0.52] `src/gui/SpectrumWidget.cpp:7972-7993` ⟵ `src/gui/SpectrumWidget.cpp:6841-6861`
  - [diverged within-file w=0.67 line=0.53] `src/core/tnc/AetherAx25LibmodemShim.cpp:896-934` ⟵ `src/core/tnc/AetherAx25LibmodemShim.cpp:628-654`
  - [diverged within-file w=0.66 line=0.52] `src/models/RadioModel.cpp:2613-2645` ⟵ `src/models/RadioModel.cpp:2434-2449`
  - [diverged within-file w=0.65 line=0.62] `src/core/AudioEngine.cpp:3569-3585` ⟵ `src/core/AudioEngine.cpp:3070-3085`
  - [diverged within-file w=0.64 line=0.71] `src/core/MidiControlManager.cpp:407-412` ⟵ `src/core/MidiControlManager.cpp:399-404`
  - [diverged within-file w=0.64 line=0.33] `src/gui/SpectrumWidget.cpp:5667-5672` ⟵ `src/gui/SpectrumWidget.cpp:4627-4632`
  - [diverged within-file w=0.62 line=0.41] `src/gui/MainWindow.cpp:14082-14099` ⟵ `src/gui/MainWindow.cpp:11538-11545`
  - [diverged within-file w=0.62 line=0.53] `src/gui/AtuPreTuneDialog.cpp:747-820` ⟵ `src/gui/AtuPreTuneDialog.cpp:631-680`

## djeada/Standard-of-Iron — 1960 коммитов | циклы=0(SCC≤1) | копипаст=55

### `7a0588bbe3f4` — "refactor game engine, reduce duplication (#1056)" (Adam Djellouli, 2026-05-30) [55 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/djeada_Standard-of-Iron show 7a0588bbe3f4`
  - [verbatim cross-file w=1.00 line=1.00] `game/audio/miniaudio_backend.cpp:406-413` ⟵ `game/audio/MiniaudioBackend.cpp:106-113`
  - [diverged within-file w=1.00 line=0.65] `game/map/minimap/unit_layer.cpp:130-147` ⟵ `game/map/minimap/unit_layer.cpp:94-111`
  - [diverged within-file w=1.00 line=0.73] `game/map/minimap/unit_layer.cpp:154-177` ⟵ `game/map/minimap/unit_layer.cpp:116-139`
  - [diverged within-file w=1.00 line=0.67] `game/map/minimap/minimap_generator.cpp:543-563` ⟵ `game/map/minimap/minimap_generator.cpp:250-269`
  - [near within-file w=1.00 line=0.80] `game/map/minimap/minimap_generator.cpp:428-438` ⟵ `game/map/minimap/minimap_generator.cpp:166-176`
  - [diverged within-file w=1.00 line=0.45] `game/map/map_transformer.cpp:198-233` ⟵ `game/map/map_transformer.cpp:83-120`
  - [diverged within-file w=1.00 line=0.75] `game/map/map_transformer.cpp:118-127` ⟵ `game/map/map_transformer.cpp:212-221`
  - [diverged within-file w=1.00 line=0.56] `game/map/skirmish_loader.cpp:260-266` ⟵ `game/map/skirmish_loader.cpp:231-237`
  - [verbatim cross-file w=1.00 line=1.00] `render/ground/terrain_renderer_mesh.cpp:349-364` ⟵ `render/ground/terrain_renderer.cpp:207-222`
  - [diverged within-file w=1.00 line=0.41] `game/systems/ai_system.cpp:193-212` ⟵ `game/systems/ai_system.cpp:114-133`
  - [diverged within-file w=1.00 line=0.56] `render/gl/shader.cpp:389-395` ⟵ `render/gl/shader.cpp:258-264`
  - [diverged cross-file w=1.00 line=0.25] `game/audio/miniaudio_backend.cpp:330-340` ⟵ `game/audio/MiniaudioBackend.cpp:33-43`
  - [diverged within-file w=1.00 line=0.73] `game/systems/movement_system.cpp:762-783` ⟵ `game/systems/movement_system.cpp:274-295`
  - [diverged within-file w=1.00 line=0.50] `render/gl/shader.cpp:369-374` ⟵ `render/gl/shader.cpp:237-242`
  - [near within-file w=1.00 line=0.82] `render/gl/camera.cpp:551-573` ⟵ `render/gl/camera.cpp:512-535`
  - [diverged within-file w=1.00 line=0.54] `render/humanoid/mounted_pose_controller.cpp:214-223` ⟵ `render/humanoid/mounted_pose_controller.cpp:177-188`
  - [diverged within-file w=1.00 line=0.58] `render/humanoid/mounted_pose_controller.cpp:331-350` ⟵ `render/humanoid/mounted_pose_controller.cpp:289-308`
  - [diverged within-file w=1.00 line=0.54] `main.cpp:281-303` ⟵ `main.cpp:124-149`
  - [diverged within-file w=1.00 line=0.41] `main.cpp:354-410` ⟵ `main.cpp:184-225`
  - [near within-file w=1.00 line=0.91] `main.cpp:656-684` ⟵ `main.cpp:382-410`
  - [diverged cross-file w=0.99 line=0.25] `render/ground/linear_feature_geometry.cpp:435-465` ⟵ `render/ground/bridge_renderer.cpp:175-205`
  - [diverged cross-file w=0.97 line=0.26] `render/gl/backend/effects_command_executor.cpp:229-251` ⟵ `render/gl/backend.cpp:1600-1617`
  - [diverged within-file w=0.96 line=0.62] `tests/render/pose_controller_compatibility_test.cpp:104-117` ⟵ `tests/render/pose_controller_compatibility_test.cpp:118-133`
  - [diverged within-file w=0.95 line=0.49] `render/geom/projectile_renderer.cpp:242-278` ⟵ `render/geom/projectile_renderer.cpp:160-199`
  - [diverged within-file w=0.95 line=0.60] `tests/render/pose_controller_test.cpp:439-453` ⟵ `tests/render/pose_controller_test.cpp:172-188`
  - [diverged within-file w=0.93 line=0.71] `game/map/minimap/minimap_generator.cpp:805-839` ⟵ `game/map/minimap/minimap_generator.cpp:449-483`
  - [diverged within-file w=0.93 line=0.68] `app/controllers/command_controller.cpp:741-782` ⟵ `app/controllers/command_controller.cpp:671-713`
  - [diverged cross-file w=0.92 line=0.70] `game/audio/miniaudio_backend.cpp:479-512` ⟵ `game/audio/MiniaudioBackend.cpp:159-193`
  - [diverged within-file w=0.92 line=0.69] `app/core/input_command_handler.cpp:498-516` ⟵ `app/core/input_command_handler.cpp:360-378`
  - [diverged within-file w=0.91 line=0.27] `render/gl/backend.cpp:292-313` ⟵ `render/gl/backend.cpp:219-235`
  - [diverged cross-file w=0.91 line=0.06] `app/core/runtime_frame_orchestrator.cpp:72-82` ⟵ `app/core/game_engine.cpp:879-889`
  - [diverged within-file w=0.90 line=0.50] `game/systems/ai_system.cpp:149-189` ⟵ `game/systems/ai_system.cpp:75-110`
  - [diverged within-file w=0.88 line=0.76] `game/systems/troop_profile_service.cpp:49-69` ⟵ `game/systems/troop_profile_service.cpp:25-44`
  - [diverged within-file w=0.88 line=0.33] `app/core/game_engine.cpp:2214-2256` ⟵ `app/core/game_engine.cpp:1131-1170`
  - [diverged within-file w=0.85 line=0.61] `game/map/map_catalog.cpp:232-263` ⟵ `game/map/map_catalog.cpp:131-161`
  - [diverged cross-file w=0.84 line=0.48] `app/models/minimap_image_provider.cpp:11-39` ⟵ `app/models/map_preview_image_provider.cpp:10-37`
  - [diverged cross-file w=0.84 line=0.48] `app/models/map_preview_image_provider.cpp:12-39` ⟵ `app/models/minimap_image_provider.cpp:9-37`
  - [near cross-file w=0.83 line=0.93] `game/audio/miniaudio_backend.cpp:442-459` ⟵ `game/audio/MiniaudioBackend.cpp:130-146`
  - [diverged cross-file w=0.83 line=0.11] `app/core/frame_ui_coordinator.cpp:87-93` ⟵ `app/core/game_engine.cpp:960-966`
  - [diverged within-file w=0.83 line=0.62] `render/geom/projectile_renderer.cpp:217-237` ⟵ `render/geom/projectile_renderer.cpp:135-155`
  - [near within-file w=0.80 line=0.82] `game/systems/pathfinding.cpp:942-959` ⟵ `game/systems/pathfinding.cpp:557-574`
  - [diverged cross-file w=0.79 line=0.06] `render/entity/nations/roman/ballista_renderer.cpp:150-205` ⟵ `render/entity/nations/carthage/ballista_renderer.cpp:128-167`
  - [diverged cross-file w=0.79 line=0.06] `render/entity/nations/carthage/ballista_renderer.cpp:160-215` ⟵ `render/entity/nations/roman/ballista_renderer.cpp:123-162`
  - [diverged cross-file w=0.76 line=0.47] `game/audio/music_player.cpp:225-235` ⟵ `game/audio/MusicPlayer.cpp:141-151`
  - [diverged within-file w=0.76 line=0.67] `game/systems/save_storage.cpp:743-768` ⟵ `game/systems/save_storage.cpp:676-696`
  - [diverged within-file w=0.75 line=0.36] `render/submitter.h:391-407` ⟵ `render/submitter.h:234-246`
  - [diverged within-file w=0.71 line=0.73] `render/entity/registry.cpp:82-127` ⟵ `render/entity/registry.cpp:47-80`
  - [diverged within-file w=0.70 line=0.50] `game/map/map_catalog.cpp:266-295` ⟵ `game/map/map_catalog.cpp:164-191`
  - [diverged within-file w=0.66 line=0.59] `game/systems/game_state_serializer.cpp:202-229` ⟵ `game/systems/game_state_serializer.cpp:130-151`
  - [diverged within-file w=0.64 line=0.13] `render/gl/backend/vegetation_pipeline.cpp:821-874` ⟵ `render/gl/backend/vegetation_pipeline.cpp:568-606`
  - [diverged within-file w=0.64 line=0.56] `game/systems/pathfinding.cpp:671-727` ⟵ `game/systems/pathfinding.cpp:361-407`
  - [diverged within-file w=0.63 line=0.20] `app/core/game_engine.cpp:2262-2292` ⟵ `app/core/game_engine.cpp:1131-1170`
  - [diverged within-file w=0.63 line=0.41] `app/core/game_engine.cpp:2712-2736` ⟵ `app/core/game_engine.cpp:1805-1825`
  - [diverged within-file w=0.61 line=0.67] `app/core/game_engine.cpp:3433-3463` ⟵ `app/core/game_engine.cpp:2299-2324`
  - [diverged within-file w=0.60 line=0.30] `render/ground/riverbank_renderer.cpp:98-135` ⟵ `render/ground/riverbank_renderer.cpp:461-516`

## florinzgz/FULL-FIRMWARE-Coche-Marcos — 1923 коммитов | циклы=0(SCC≤1) | копипаст=18

### `c52beec7663d` — "Merge pull request #304 from florinzgz/copilot/update-inventario-componentes" (florinzgz, 2026-02-06) [18 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/florinzgz_FULL-FIRMWARE-Coche-Marcos show c52beec7663d`
  - [diverged within-file w=1.00 line=0.75] `src/core/eeprom_persistence.cpp:39-49` ⟵ `src/core/eeprom_persistence.cpp:29-38`
  - [diverged within-file w=1.00 line=0.75] `src/core/eeprom_persistence.cpp:54-64` ⟵ `src/core/eeprom_persistence.cpp:43-52`
  - [near within-file w=1.00 line=0.83] `src/core/system.cpp:476-505` ⟵ `src/core/system.cpp:398-427`
  - [diverged within-file w=1.00 line=0.75] `src/safety/regen_ai.cpp:219-239` ⟵ `src/safety/regen_ai.cpp:241-261`
  - [diverged within-file w=1.00 line=0.09] `src/menu/menu_obstacle_config.cpp:343-367` ⟵ `src/menu/menu_obstacle_config.cpp:299-307`
  - [verbatim within-file w=1.00 line=1.00] `src/hud/menu_encoder_calibration.cpp:434-446` ⟵ `src/hud/menu_encoder_calibration.cpp:396-408`
  - [verbatim within-file w=1.00 line=1.00] `src/input/steering.cpp:96-132` ⟵ `src/input/steering.cpp:53-89`
  - [near within-file w=0.98 line=0.86] `src/test/watchdog_tests.cpp:138-155` ⟵ `src/test/watchdog_tests.cpp:161-178`
  - [diverged within-file w=0.93 line=0.68] `src/main.cpp:378-443` ⟵ `src/main.cpp:164-228`
  - [diverged within-file w=0.89 line=0.68] `src/test/watchdog_tests.cpp:35-55` ⟵ `src/test/watchdog_tests.cpp:56-78`
  - [diverged within-file w=0.88 line=0.67] `src/test/hardware_failure_tests.cpp:31-50` ⟵ `src/test/hardware_failure_tests.cpp:52-73`
  - [diverged within-file w=0.84 line=0.53] `src/control/traction.cpp:621-698` ⟵ `src/control/traction.cpp:555-615`
  - [diverged within-file w=0.84 line=0.47] `src/hud/hud_manager.cpp:445-520` ⟵ `src/hud/hud_manager.cpp:148-200`
  - [diverged within-file w=0.81 line=0.41] `src/hud/led_control_menu.cpp:117-131` ⟵ `src/hud/led_control_menu.cpp:133-147`
  - [diverged within-file w=0.80 line=0.71] `include/pins.h:509-557` ⟵ `include/pins.h:407-462`
  - [diverged within-file w=0.77 line=0.22] `src/control/traction.cpp:433-485` ⟵ `src/control/traction.cpp:555-615`
  - [diverged within-file w=0.63 line=0.25] `src/hud/gauges.cpp:214-266` ⟵ `src/hud/gauges.cpp:147-178`
  - [diverged within-file w=0.61 line=0.42] `src/hud/gauges.cpp:284-340` ⟵ `src/hud/gauges.cpp:191-231`

## bylins/mud — 1886 коммитов | циклы=0(SCC≤1) | копипаст=54

### `98269c6ad736` — "Merge pull request #3379 from bylins/feat/create-test-world-py" (bylins, 2026-05-31) [54 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/bylins_mud show 98269c6ad736`
  - [near within-file w=1.00 line=0.93] `src/engine/scripting/dg_scripts.cpp:5824-5862` ⟵ `src/engine/scripting/dg_scripts.cpp:5700-5738`
  - [diverged within-file w=1.00 line=0.40] `src/gameplay/communication/remember.cpp:201-207` ⟵ `src/gameplay/communication/remember.cpp:194-200`
  - [diverged within-file w=1.00 line=0.46] `src/gameplay/ai/mobact.cpp:1241-1270` ⟵ `src/gameplay/ai/mobact.cpp:880-909`
  - [diverged within-file w=1.00 line=0.33] `src/engine/scripting/dg_scripts.cpp:2504-2511` ⟵ `src/engine/scripting/dg_scripts.cpp:2430-2437`
  - [diverged within-file w=1.00 line=0.30] `src/engine/scripting/dg_scripts.cpp:1611-1645` ⟵ `src/engine/scripting/dg_scripts.cpp:1544-1578`
  - [diverged within-file w=1.00 line=0.78] `src/engine/entities/obj_data.cpp:1004-1014` ⟵ `src/engine/entities/obj_data.cpp:970-980`
  - [diverged cross-file w=1.00 line=0.45] `src/engine/scripting/trigger_indenter.cpp:28-35` ⟵ `src/engine/scripting/dg_db_scripts.cpp:62-69`
  - [diverged within-file w=1.00 line=0.73] `src/gameplay/mechanics/sight.cpp:291-306` ⟵ `src/gameplay/mechanics/sight.cpp:231-246`
  - [near within-file w=1.00 line=0.93] `src/engine/ui/cmd/do_where.cpp:303-336` ⟵ `src/engine/ui/cmd/do_where.cpp:250-283`
  - [near within-file w=1.00 line=0.80] `src/gameplay/mechanics/sight.cpp:520-544` ⟵ `src/gameplay/mechanics/sight.cpp:443-467`
  - [diverged within-file w=0.97 line=0.76] `src/gameplay/economics/auction.cpp:834-870` ⟵ `src/gameplay/economics/auction.cpp:790-824`
  - [near within-file w=0.97 line=0.90] `src/gameplay/affects/affect_data.cpp:745-769` ⟵ `src/gameplay/affects/affect_data.cpp:589-613`
  - [near within-file w=0.96 line=0.94] `src/gameplay/magic/magic.cpp:3673-3717` ⟵ `src/gameplay/magic/magic.cpp:3748-3792`
  - [near within-file w=0.96 line=0.94] `src/gameplay/magic/magic.cpp:3628-3670` ⟵ `src/gameplay/magic/magic.cpp:3703-3745`
  - [diverged within-file w=0.95 line=0.60] `src/engine/ui/interpreter.cpp:2560-2600` ⟵ `src/engine/ui/interpreter.cpp:2684-2724`
  - [near within-file w=0.95 line=0.80] `src/gameplay/mechanics/sight.cpp:167-177` ⟵ `src/gameplay/mechanics/sight.cpp:107-117`
  - [near within-file w=0.95 line=0.81] `src/gameplay/core/game_limits.cpp:1064-1109` ⟵ `src/gameplay/core/game_limits.cpp:999-1043`
  - [near within-file w=0.94 line=0.90] `src/engine/scripting/dg_scripts.cpp:3912-3965` ⟵ `src/engine/scripting/dg_scripts.cpp:3832-3885`
  - [diverged within-file w=0.94 line=0.75] `src/engine/core/config.cpp:818-837` ⟵ `src/engine/core/config.cpp:740-759`
  - [diverged within-file w=0.94 line=0.61] `src/engine/db/db.cpp:3340-3360` ⟵ `src/engine/db/db.cpp:3130-3151`
  - [near within-file w=0.93 line=0.94] `src/engine/ui/objects_filter.cpp:771-789` ⟵ `src/engine/ui/objects_filter.cpp:655-672`
  - [near within-file w=0.93 line=0.94] `src/engine/ui/objects_filter.cpp:752-768` ⟵ `src/engine/ui/objects_filter.cpp:637-652`
  - [diverged within-file w=0.93 line=0.62] `src/gameplay/mechanics/sight.cpp:1224-1260` ⟵ `src/gameplay/mechanics/sight.cpp:1098-1134`
  - [near within-file w=0.93 line=0.91] `src/engine/core/comm.cpp:937-969` ⟵ `src/engine/core/comm.cpp:836-868`
  - [near within-file w=0.93 line=0.81] `src/engine/scripting/dg_scripts.cpp:4123-4187` ⟵ `src/engine/scripting/dg_scripts.cpp:4043-4106`
  - [diverged within-file w=0.91 line=0.67] `src/gameplay/core/game_limits.cpp:851-879` ⟵ `src/gameplay/core/game_limits.cpp:784-814`
  - [diverged within-file w=0.89 line=0.43] `src/engine/scripting/dg_scripts.cpp:2427-2431` ⟵ `src/engine/scripting/dg_scripts.cpp:2353-2357`
  - [diverged within-file w=0.88 line=0.33] `src/engine/scripting/dg_scripts.cpp:3123-3128` ⟵ `src/engine/scripting/dg_scripts.cpp:3045-3050`
  - [near within-file w=0.86 line=0.85] `src/engine/core/comm.cpp:982-1067` ⟵ `src/engine/core/comm.cpp:881-968`
  - [near within-file w=0.86 line=0.85] `src/engine/db/db.cpp:1295-1322` ⟵ `src/engine/db/db.cpp:1169-1195`
  - [diverged within-file w=0.86 line=0.67] `src/gameplay/mechanics/damage.cpp:337-352` ⟵ `src/gameplay/mechanics/damage.cpp:506-521`
  - [diverged within-file w=0.86 line=0.39] `src/engine/scripting/dg_scripts.cpp:2540-2592` ⟵ `src/engine/scripting/dg_scripts.cpp:2467-2514`
  - [near within-file w=0.85 line=0.82] `src/gameplay/magic/magic.cpp:3327-3338` ⟵ `src/gameplay/magic/magic.cpp:3402-3413`
  - [diverged within-file w=0.84 line=0.57] `src/gameplay/core/game_limits.cpp:757-781` ⟵ `src/gameplay/core/game_limits.cpp:692-716`
  - [near within-file w=0.83 line=0.80] `src/engine/scripting/dg_scripts.cpp:4803-4830` ⟵ `src/engine/scripting/dg_scripts.cpp:4684-4711`
  - [diverged within-file w=0.82 line=0.60] `src/engine/scripting/dg_scripts.cpp:1656-1686` ⟵ `src/engine/scripting/dg_scripts.cpp:1589-1619`
  - [diverged within-file w=0.81 line=0.50] `src/engine/scripting/dg_scripts.cpp:4101-4116` ⟵ `src/engine/scripting/dg_scripts.cpp:4021-4036`
  - [diverged within-file w=0.81 line=0.71] `src/gameplay/ai/mobact.cpp:1103-1109` ⟵ `src/gameplay/ai/mobact.cpp:1147-1153`
  - [diverged within-file w=0.81 line=0.20] `src/gameplay/ai/mobact.cpp:1112-1119` ⟵ `src/gameplay/ai/mobact.cpp:1147-1153`
  - [diverged within-file w=0.80 line=0.76] `src/engine/ui/interpreter.cpp:1344-1400` ⟵ `src/engine/ui/interpreter.cpp:1487-1543`
  - [diverged within-file w=0.79 line=0.68] `src/engine/ui/interpreter.cpp:3742-3764` ⟵ `src/engine/ui/interpreter.cpp:3866-3892`
  - [diverged within-file w=0.79 line=0.35] `src/gameplay/mechanics/sight.cpp:1119-1164` ⟵ `src/gameplay/mechanics/sight.cpp:993-1038`
  - [diverged within-file w=0.78 line=0.56] `src/engine/db/db.cpp:2381-2389` ⟵ `src/engine/db/db.cpp:2166-2177`
  - [diverged within-file w=0.76 line=0.65] `src/engine/scripting/dg_scripts.cpp:1808-1823` ⟵ `src/engine/scripting/dg_scripts.cpp:1741-1756`
  - [diverged within-file w=0.76 line=0.62] `src/engine/scripting/dg_scripts.cpp:5045-5107` ⟵ `src/engine/scripting/dg_scripts.cpp:4918-4980`
  - [diverged within-file w=0.75 line=0.68] `src/engine/scripting/dg_scripts.cpp:5235-5279` ⟵ `src/engine/scripting/dg_scripts.cpp:5108-5152`
  - [diverged within-file w=0.71 line=0.70] `src/engine/boot/boot_data_files.cpp:1017-1027` ⟵ `src/engine/boot/boot_data_files.cpp:1037-1048`
  - [diverged within-file w=0.69 line=0.71] `src/engine/ui/cmd/do_where.cpp:32-38` ⟵ `src/engine/ui/cmd/do_where.cpp:24-30`
  - [diverged within-file w=0.69 line=0.30] `src/engine/core/handler.cpp:1112-1135` ⟵ `src/engine/core/handler.cpp:1065-1084`
  - [diverged within-file w=0.68 line=0.28] `src/engine/ui/interpreter.cpp:1635-1695` ⟵ `src/engine/ui/interpreter.cpp:1773-1829`
  - [diverged within-file w=0.68 line=0.54] `src/engine/core/config.cpp:766-798` ⟵ `src/engine/core/config.cpp:697-720`
  - [diverged within-file w=0.63 line=0.43] `src/engine/ui/interpreter.cpp:1968-1991` ⟵ `src/engine/ui/interpreter.cpp:2106-2123`
  - [diverged within-file w=0.62 line=0.33] `tests/main.cpp:25-38` ⟵ `tests/main.cpp:18-24`
  - [diverged within-file w=0.61 line=0.36] `src/gameplay/mechanics/celebrates.cpp:576-623` ⟵ `src/gameplay/mechanics/celebrates.cpp:640-685`

## shifty81/FreshVoxel — 1762 коммитов | циклы=0(SCC≤1) | копипаст=82

### `c93674b629ee` — "Merge pull request #377 from shifty81/copilot/investigate-viewport-issues" (shifty81, 2026-04-11) [82 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/shifty81_FreshVoxel show c93674b629ee`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/ShaderManager.cpp:15-30` ⟵ `src/renderer/ShaderManager.cpp:16-31`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:1028-1048` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:761-781`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:951-990` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:681-720`
  - [near cross-file w=1.00 line=0.81] `engine/renderer/backends/DirectX12RenderContext.cpp:715-737` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:597-618`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:613-622` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:491-500`
  - [near cross-file w=1.00 line=0.93] `engine/renderer/backends/DirectX12RenderContext.cpp:404-441` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:284-321`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/DirectX11RenderContext.cpp:691-727` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:580-616`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/DirectX11RenderContext.cpp:661-686` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:551-576`
  - [diverged cross-file w=1.00 line=0.57] `engine/renderer/backends/DirectX11RenderContext.cpp:571-596` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:456-488`
  - [diverged cross-file w=1.00 line=0.24] `engine/renderer/TextureManager.cpp:73-115` ⟵ `src/renderer/TextureManager.cpp:67-89`
  - [diverged cross-file w=1.00 line=0.58] `engine/renderer/backends/DirectX12RenderContext.cpp:1052-1068` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:784-797`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/RenderContext.cpp:22-52` ⟵ `src/renderer/RenderContext.cpp:20-50`
  - [verbatim cross-file w=1.00 line=1.00] `engine/physics/PhysicsSystemEnhanced.cpp:154-204` ⟵ `src/physics/PhysicsSystemEnhanced.cpp:138-188`
  - [diverged cross-file w=1.00 line=0.75] `engine/physics/PhysicsSystemEnhanced.cpp:80-96` ⟵ `src/physics/PhysicsSystemEnhanced.cpp:72-88`
  - [verbatim cross-file w=1.00 line=1.00] `engine/physics/PhysicsSystemEnhanced.cpp:30-64` ⟵ `src/physics/PhysicsSystemEnhanced.cpp:24-58`
  - [diverged cross-file w=1.00 line=0.74] `engine/physics/CollisionDetection.cpp:383-405` ⟵ `src/physics/CollisionDetection.cpp:332-355`
  - [near cross-file w=1.00 line=0.84] `engine/physics/CollisionDetection.cpp:276-297` ⟵ `src/physics/CollisionDetection.cpp:253-273`
  - [near cross-file w=1.00 line=0.83] `engine/networking/NetworkMessage.cpp:130-153` ⟵ `src/networking/NetworkMessage.cpp:114-138`
  - [verbatim cross-file w=1.00 line=1.00] `engine/networking/NetworkMessage.cpp:42-46` ⟵ `src/networking/NetworkMessage.cpp:36-40`
  - [verbatim cross-file w=1.00 line=1.00] `engine/rpg/TradingSystem.cpp:42-58` ⟵ `src/rpg/TradingSystem.cpp:34-50`
  - [near cross-file w=1.00 line=0.82] `engine/voxel/ChunkStreamer.cpp:122-145` ⟵ `src/voxel/ChunkStreamer.cpp:99-121`
  - [near cross-file w=1.00 line=0.80] `engine/voxel/ChunkStreamer.cpp:25-42` ⟵ `src/voxel/ChunkStreamer.cpp:20-36`
  - [verbatim cross-file w=1.00 line=1.00] `engine/ui/MainMenu.cpp:121-148` ⟵ `src/ui/MainMenu.cpp:112-139`
  - [near cross-file w=1.00 line=0.85] `engine/ui/MainMenu.cpp:83-117` ⟵ `src/ui/MainMenu.cpp:75-109`
  - [verbatim cross-file w=1.00 line=1.00] `engine/ui/MainMenu.cpp:57-73` ⟵ `src/ui/MainMenu.cpp:53-69`
  - [diverged cross-file w=1.00 line=0.75] `engine/ui/MainMenu.cpp:22-37` ⟵ `src/ui/MainMenu.cpp:20-35`
  - [diverged cross-file w=1.00 line=0.54] `engine/scripting/lua/ScriptingEngineImpl.cpp:42-61` ⟵ `src/scripting/lua/ScriptingEngineImpl.cpp:36-62`
  - [verbatim cross-file w=1.00 line=1.00] `engine/rpg/TradingSystem.cpp:63-80` ⟵ `src/rpg/TradingSystem.cpp:54-71`
  - [near cross-file w=1.00 line=0.81] `engine/assets/AssetManager.cpp:117-175` ⟵ `src/assets/AssetManager.cpp:112-170`
  - [verbatim cross-file w=1.00 line=1.00] `engine/rpg/ProgressionComponent.cpp:50-61` ⟵ `src/rpg/ProgressionComponent.cpp:39-50`
  - [verbatim cross-file w=1.00 line=1.00] `engine/rpg/LootSystem.cpp:75-102` ⟵ `src/rpg/LootSystem.cpp:64-91`
  - [verbatim cross-file w=1.00 line=1.00] `engine/rpg/LootSystem.cpp:52-59` ⟵ `src/rpg/LootSystem.cpp:44-51`
  - [diverged cross-file w=1.00 line=0.15] `engine/renderer/backends/OpenGLRenderContext.cpp:517-549` ⟵ `src/renderer/backends/OpenGLRenderContext.cpp:401-416`
  - [diverged cross-file w=1.00 line=0.79] `engine/renderer/backends/OpenGLRenderContext.cpp:488-513` ⟵ `src/renderer/backends/OpenGLRenderContext.cpp:375-398`
  - [near cross-file w=1.00 line=0.82] `engine/renderer/backends/OpenGLRenderContext.cpp:426-482` ⟵ `src/renderer/backends/OpenGLRenderContext.cpp:317-370`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/OpenGLRenderContext.cpp:404-420` ⟵ `src/renderer/backends/OpenGLRenderContext.cpp:297-313`
  - [verbatim cross-file w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:1073-1089` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:801-817`
  - [near cross-file w=1.00 line=0.83] `engine/audio/AudioEngine.cpp:88-111` ⟵ `src/audio/AudioEngine.cpp:76-98`
  - [verbatim cross-file w=1.00 line=1.00] `engine/devtools/DebugConsole.cpp:138-165` ⟵ `src/devtools/DebugConsole.cpp:120-147`
  - [diverged cross-file w=1.00 line=0.33] `engine/devtools/DebugConsole.cpp:102-133` ⟵ `src/devtools/DebugConsole.cpp:93-116`
  - [verbatim cross-file w=1.00 line=1.00] `engine/core/Window.cpp:35-71` ⟵ `src/core/Window.cpp:30-66`
  - [verbatim cross-file w=1.00 line=1.00] `engine/core/ResourceManager.cpp:193-223` ⟵ `src/core/ResourceManager.cpp:133-163`
  - [verbatim cross-file w=1.00 line=1.00] `engine/core/Logger.cpp:247-253` ⟵ `src/core/Logger.cpp:214-220`
  - [near cross-file w=1.00 line=0.85] `engine/core/Logger.cpp:39-96` ⟵ `src/core/Logger.cpp:36-91`
  - [verbatim cross-file w=1.00 line=1.00] `engine/core/Engine.cpp:2671-2691` ⟵ `src/core/Engine.cpp:947-967`
  - [near cross-file w=1.00 line=0.86] `engine/core/Engine.cpp:2163-2201` ⟵ `src/core/Engine.cpp:710-746`
  - [verbatim cross-file w=1.00 line=1.00] `engine/core/Engine.cpp:2142-2159` ⟵ `src/core/Engine.cpp:690-707`
  - [verbatim cross-file w=1.00 line=1.00] `engine/networking/GameServer.cpp:192-203` ⟵ `src/networking/GameServer.cpp:178-189`
  - [near cross-file w=1.00 line=0.88] `engine/audio/AudioEngine.cpp:38-84` ⟵ `src/audio/AudioEngine.cpp:28-73`
  - [near cross-file w=1.00 line=0.87] `engine/assets/ModularAssetSystem.cpp:851-888` ⟵ `src/assets/ModularAssetSystem.cpp:441-478`
  - [near cross-file w=1.00 line=0.89] `engine/assets/ModularAssetSystem.cpp:426-452` ⟵ `src/assets/ModularAssetSystem.cpp:275-300`
  - [diverged cross-file w=1.00 line=0.63] `engine/assets/ModularAssetSystem.cpp:283-309` ⟵ `src/assets/ModularAssetSystem.cpp:138-161`
  - [verbatim cross-file w=1.00 line=1.00] `engine/assets/AssetManager.cpp:352-366` ⟵ `src/assets/AssetManager.cpp:336-350`
  - [near cross-file w=1.00 line=0.80] `engine/assets/AssetManager.cpp:261-300` ⟵ `src/assets/AssetManager.cpp:251-289`
  - [near cross-file w=1.00 line=0.80] `engine/assets/AssetManager.cpp:187-257` ⟵ `src/assets/AssetManager.cpp:180-248`
  - [near cross-file w=1.00 line=0.89] `engine/assets/AssetManager.cpp:56-100` ⟵ `src/assets/AssetManager.cpp:53-97`
  - [diverged cross-file w=1.00 line=0.34] `engine/devtools/DevToolsManager.cpp:64-117` ⟵ `src/devtools/DevToolsManager.cpp:58-99`
  - [diverged cross-file w=1.00 line=0.75] `engine/devtools/PerformanceProfiler.cpp:103-116` ⟵ `src/devtools/PerformanceProfiler.cpp:102-116`
  - [near cross-file w=1.00 line=0.86] `engine/networking/GameServer.cpp:300-331` ⟵ `src/networking/GameServer.cpp:220-250`
  - [near cross-file w=1.00 line=0.91] `engine/networking/GameServer.cpp:140-188` ⟵ `src/networking/GameServer.cpp:128-175`
  - [verbatim cross-file w=1.00 line=1.00] `engine/editor/TerraformingSystem.cpp:38-95` ⟵ `src/editor/TerraformingSystem.cpp:32-89`
  - [verbatim cross-file w=1.00 line=1.00] `engine/networking/GameServer.cpp:58-84` ⟵ `src/networking/GameServer.cpp:53-79`
  - [verbatim cross-file w=1.00 line=1.00] `engine/networking/GameServer.cpp:40-54` ⟵ `src/networking/GameServer.cpp:36-50`
  - [near cross-file w=1.00 line=0.85] `engine/networking/ClientConnection.cpp:56-84` ⟵ `src/networking/ClientConnection.cpp:51-80`
  - [verbatim cross-file w=1.00 line=1.00] `engine/networking/ClientConnection.cpp:40-52` ⟵ `src/networking/ClientConnection.cpp:36-48`
  - [verbatim cross-file w=1.00 line=1.00] `engine/generation/NoiseGenerator.cpp:24-44` ⟵ `src/generation/NoiseGenerator.cpp:24-44`
  - [verbatim cross-file w=1.00 line=1.00] `engine/interaction/VoxelInteraction.cpp:26-37` ⟵ `src/interaction/VoxelInteraction.cpp:23-34`
  - [verbatim cross-file w=1.00 line=1.00] `engine/input/InputManager.cpp:49-81` ⟵ `src/input/InputManager.cpp:44-76`
  - [diverged cross-file w=0.99 line=0.69] `engine/renderer/backends/OpenGLRenderContext.cpp:224-275` ⟵ `src/renderer/backends/OpenGLRenderContext.cpp:156-203`
  - [diverged cross-file w=0.93 line=0.71] `engine/core/Engine.cpp:2045-2101` ⟵ `src/core/Engine.cpp:626-673`
  - [near cross-file w=0.92 line=0.95] `engine/renderer/backends/DirectX11RenderContext.cpp:601-657` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:492-548`
  - [diverged cross-file w=0.91 line=0.75] `engine/renderer/backends/DirectX12RenderContext.cpp:659-711` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:536-594`
  - [near cross-file w=0.91 line=0.89] `engine/generation/TerrainGenerator.cpp:147-175` ⟵ `src/generation/TerrainGenerator.cpp:90-115`
  - [diverged cross-file w=0.90 line=0.61] `engine/voxel/ChunkStreamer.cpp:80-118` ⟵ `src/voxel/ChunkStreamer.cpp:65-96`
  - [diverged cross-file w=0.84 line=0.65] `engine/core/Engine.cpp:2205-2263` ⟵ `src/core/Engine.cpp:749-801`
  - [near cross-file w=0.83 line=0.81] `engine/renderer/backends/DirectX11RenderContext.cpp:462-503` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:324-364`
  - [diverged cross-file w=0.79 line=0.37] `engine/renderer/backends/DirectX12RenderContext.cpp:225-302` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:134-198`
  - [diverged cross-file w=0.75 line=0.20] `engine/renderer/backends/DirectX11RenderContext.cpp:508-567` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:367-453`
  - [diverged cross-file w=0.73 line=0.38] `engine/editor/EditorManager.cpp:818-878` ⟵ `src/editor/EditorManager.cpp:145-193`
  - [diverged cross-file w=0.69 line=0.59] `engine/core/Engine.cpp:1224-1266` ⟵ `src/core/Engine.cpp:384-408`
  - [diverged cross-file w=0.60 line=0.60] `engine/voxel/ChunkStreamer.cpp:149-191` ⟵ `src/voxel/ChunkStreamer.cpp:124-151`
  - [diverged cross-file w=0.60 line=0.50] `engine/gameplay/Player.cpp:19-58` ⟵ `src/gameplay/Player.cpp:15-37`

## mgradwohl/tasksmack — 1502 коммитов | циклы=0(SCC≤1) | копипаст=1

### `50b3476dfc34` — "ci(deps): bump github/codeql-action from 4.35.5 to 4.36.0 (#566)" (dependabot[bot], 2026-05-26) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/mgradwohl_tasksmack show 50b3476dfc34`
  - [diverged within-file w=0.77 line=0.21] `src/main.cpp:78-92` ⟵ `src/main.cpp:29-35`

## Tsoympet/PantheonChain — 1464 коммитов | циклы=0(SCC≤1) | копипаст=14

### `e8ba8e778aa1` — "Harden CLI config validation execution and tighten security audit signal (#504)" (Tsoympet, 2026-05-30) [14 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Tsoympet_PantheonChain show e8ba8e778aa1`
  - [diverged within-file w=1.00 line=0.75] `clients/desktop/src/receivepage.cpp:118-127` ⟵ `clients/desktop/src/receivepage.cpp:108-117`
  - [verbatim cross-file w=1.00 line=1.00] `layer1-talanton/wallet/hardware/firmware_verification.cpp:160-209` ⟵ `layer1/wallet/hardware/firmware_verification.cpp:46-95`
  - [near within-file w=1.00 line=0.88] `tests/consensus/test_consensus.cpp:245-262` ⟵ `tests/consensus/test_consensus.cpp:227-244`
  - [verbatim within-file w=1.00 line=1.00] `tests/unit/p2p/test_p2p.cpp:74-100` ⟵ `tests/unit/p2p/test_p2p.cpp:14-40`
  - [near within-file w=0.93 line=0.87] `tests/unit/primitives/test_asset.cpp:255-283` ⟵ `tests/unit/primitives/test_asset.cpp:214-241`
  - [near within-file w=0.92 line=0.86] `tests/unit/primitives/test_block.cpp:265-292` ⟵ `tests/unit/primitives/test_block.cpp:231-257`
  - [near cross-file w=0.91 line=0.85] `layer2-drachma/rollups/zk_rollup.cpp:334-349` ⟵ `layer2/rollups/zk_rollup.cpp:99-114`
  - [near within-file w=0.86 line=0.91] `tests/unit/chainstate/test_chainstate.cpp:266-278` ⟵ `tests/unit/chainstate/test_chainstate.cpp:243-254`
  - [near cross-file w=0.75 line=0.82] `layer1-talanton/wallet/hardware/firmware_verification.cpp:259-277` ⟵ `layer1/wallet/hardware/firmware_verification.cpp:112-127`
  - [diverged within-file w=0.72 line=0.55] `clients/desktop/src/mainwindow.cpp:283-294` ⟵ `clients/desktop/src/mainwindow.cpp:193-200`
  - [diverged within-file w=0.66 line=0.65] `clients/desktop/src/mainwindow.cpp:239-258` ⟵ `clients/desktop/src/mainwindow.cpp:169-181`
  - [diverged within-file w=0.65 line=0.67] `clients/desktop/src/rpc_client.cpp:262-283` ⟵ `clients/desktop/src/rpc_client.cpp:133-148`
  - [diverged within-file w=0.64 line=0.70] `clients/cli/main.cpp:280-307` ⟵ `clients/cli/main.cpp:129-150`
  - [diverged within-file w=0.61 line=0.62] `tests/unit/layer2/test_layer2.cpp:361-377` ⟵ `tests/unit/layer2/test_layer2.cpp:128-140`

## studiocollective/songbird — 1329 коммитов | циклы=0(SCC≤1) | копипаст=1

### `8f9ed331912c` — "fix(sync): dismiss LoadingScreen after WebView reload (#417)" (Ashu Desai, 2026-05-31) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/studiocollective_songbird show 8f9ed331912c`
  - [diverged cross-file w=0.97 line=0.40] `libraries/juce-host/src/sb_host_juce.cpp:498-500` ⟵ `app/ChannelStripWindow.h:194-197`

## melkyr/znineeight — 1198 коммитов | циклы=0(SCC≤1) | копипаст=7

### `589a9272bf72` — "Update ERROR_HANDLING_p2.md review summary section" (Andres Hernandez, 2026-04-27) [7 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/melkyr_znineeight show 589a9272bf72`
  - [verbatim cross-file w=1.00 line=1.00] `tests/test_runner_main.hpp:17-31` ⟵ `tests/main.cpp:9-23`
  - [diverged within-file w=0.80 line=0.71] `src/bootstrap/null_pointer_analyzer.cpp:609-628` ⟵ `src/bootstrap/null_pointer_analyzer.cpp:500-520`
  - [diverged within-file w=0.79 line=0.64] `src/bootstrap/parser.cpp:1401-1429` ⟵ `src/bootstrap/parser.cpp:893-913`
  - [near within-file w=0.77 line=0.87] `src/bootstrap/parser.cpp:564-616` ⟵ `src/bootstrap/parser.cpp:411-457`
  - [diverged within-file w=0.72 line=0.58] `src/bootstrap/parser.cpp:515-555` ⟵ `src/bootstrap/parser.cpp:353-402`
  - [diverged within-file w=0.64 line=0.57] `src/bootstrap/parser.cpp:1919-1979` ⟵ `src/bootstrap/parser.cpp:1245-1290`
  - [diverged within-file w=0.61 line=0.49] `src/bootstrap/double_free_analyzer.cpp:405-465` ⟵ `src/bootstrap/double_free_analyzer.cpp:153-184`

## xsscx/research — 1086 коммитов | циклы=0(SCC≤1) | копипаст=18

### `40d1fcceafbe` — "cfl: cover JSON dict language code shifts" (xsscx, 2026-05-26) [18 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/xsscx_research show 40d1fcceafbe`
  - [verbatim within-file w=1.00 line=1.00] `iccanalyzer-lite/icctest/lib/tests/test_runner.cpp:2436-2475` ⟵ `iccanalyzer-lite/icctest/lib/tests/test_runner.cpp:1001-1040`
  - [diverged within-file w=1.00 line=0.59] `iccanalyzer-lite/IccAnalyzerLUT.cpp:347-390` ⟵ `iccanalyzer-lite/IccAnalyzerLUT.cpp:295-339`
  - [near cross-file w=1.00 line=0.81] `colorbleed_tools/ColorBleedSafeF16.cpp:31-69` ⟵ `iccanalyzer-lite/IccDevSafeOverrides.cpp:69-103`
  - [diverged within-file w=0.97 line=0.16] `iccanalyzer-lite/IccAnalyzerCallGraph.cpp:136-160` ⟵ `iccanalyzer-lite/IccAnalyzerCallGraph.cpp:555-573`
  - [diverged within-file w=0.97 line=0.16] `iccanalyzer-lite/IccAnalyzerCallGraph.cpp:555-573` ⟵ `iccanalyzer-lite/IccAnalyzerCallGraph.cpp:136-160`
  - [diverged cross-file w=0.92 line=0.43] `iccanalyzer-lite/IccHeuristicsXmlSafety.cpp:108-151` ⟵ `iccanalyzer-lite/icctest/lib/src/checks/XmlSafetyChecks.cpp:47-86`
  - [diverged cross-file w=0.91 line=0.41] `iccanalyzer-lite/icctest/lib/src/checks/ComplianceChecks.cpp:1236-1265` ⟵ `iccanalyzer-lite/IccHeuristicsProfileCompliance.cpp:1411-1438`
  - [diverged within-file w=0.91 line=0.22] `iccanalyzer-lite/icctest/lib/src/checks/IntegrityChecks.cpp:769-807` ⟵ `iccanalyzer-lite/icctest/lib/src/checks/IntegrityChecks.cpp:19-54`
  - [near within-file w=0.85 line=0.84] `iccanalyzer-lite/IccHeuristicsRawPost.cpp:2335-2381` ⟵ `iccanalyzer-lite/IccHeuristicsRawPost.cpp:1787-1828`
  - [near within-file w=0.84 line=0.93] `iccanalyzer-lite/icctest/lib/src/IccTestRunner.cpp:202-266` ⟵ `iccanalyzer-lite/icctest/lib/src/IccTestRunner.cpp:134-198`
  - [near within-file w=0.83 line=0.87] `iccanalyzer-lite/IccHeuristicsExploitGap.cpp:1052-1066` ⟵ `iccanalyzer-lite/IccHeuristicsExploitGap.cpp:1073-1085`
  - [near within-file w=0.81 line=0.89] `colorbleed_tools/IccDumpAll.cpp:119-165` ⟵ `colorbleed_tools/IccDumpAll.cpp:54-97`
  - [near within-file w=0.79 line=0.88] `iccanalyzer-lite/IccHeuristicsXmlSafety.cpp:667-674` ⟵ `iccanalyzer-lite/IccHeuristicsXmlSafety.cpp:462-468`
  - [diverged cross-file w=0.79 line=0.28] `iccanalyzer-lite/icctest/lib/src/checks/DataValidationChecks.cpp:1730-1788` ⟵ `iccanalyzer-lite/IccHeuristicsDataValidation.cpp:1864-1923`
  - [near within-file w=0.78 line=0.82] `iccanalyzer-lite/icctest/lib/src/checks/DataValidationChecks.cpp:1934-1947` ⟵ `iccanalyzer-lite/icctest/lib/src/checks/DataValidationChecks.cpp:807-817`
  - [near within-file w=0.76 line=0.80] `iccanalyzer-lite/icctest/lib/src/ProfileView.cpp:814-874` ⟵ `iccanalyzer-lite/icctest/lib/src/ProfileView.cpp:577-621`
  - [diverged within-file w=0.69 line=0.74] `iccanalyzer-lite/icctest/lib/tests/test_profile_view.cpp:593-614` ⟵ `iccanalyzer-lite/icctest/lib/tests/test_profile_view.cpp:351-368`
  - [diverged cross-file w=0.62 line=0.14] `iccanalyzer-lite/icctest/lib/src/checks/DataValidationChecks.cpp:1615-1655` ⟵ `iccanalyzer-lite/IccHeuristicsDataValidation.cpp:1746-1801`

## ImagingTools/Acf — 1066 коммитов | циклы=0(SCC≤1) | копипаст=64

### `e17192b91541` — "CFileDialogLoaderComp: add startup directory observer and mutex protection (#216)" (Vladimir Cherepanov, 2026-05-25) [64 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ImagingTools_Acf show e17192b91541`
  - [verbatim within-file w=1.00 line=1.00] `Include/iprm/CParamsManagerCompBase.cpp:590-599` ⟵ `Include/iprm/CParamsManagerCompBase.cpp:536-545`
  - [verbatim within-file w=1.00 line=1.00] `Include/iser/CJsonWriteArchiveBase.cpp:232-238` ⟵ `Include/iser/CJsonWriteArchiveBase.cpp:181-187`
  - [verbatim within-file w=1.00 line=1.00] `Include/iser/CJsonWriteArchiveBase.cpp:207-219` ⟵ `Include/iser/CJsonWriteArchiveBase.cpp:156-168`
  - [verbatim within-file w=1.00 line=1.00] `Include/iser/CJsonReadArchiveBase.cpp:309-320` ⟵ `Include/iser/CJsonReadArchiveBase.cpp:280-291`
  - [verbatim within-file w=1.00 line=1.00] `Include/iser/CJsonReadArchiveBase.cpp:290-304` ⟵ `Include/iser/CJsonReadArchiveBase.cpp:261-275`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CConsoleGui.cpp:631-648` ⟵ `Include/iview/CConsoleGui.cpp:549-566`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CImageShape.cpp:360-382` ⟵ `Include/iview/CImageShape.cpp:314-336`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CInteractiveShapeBase.cpp:154-161` ⟵ `Include/iview/CInteractiveShapeBase.cpp:146-153`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqtgui/CGuiComponentBase.cpp:245-260` ⟵ `Include/iqtgui/CGuiComponentBase.cpp:228-243`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqtdoc/CMultiDocumentWorkspaceGuiComp.cpp:598-612` ⟵ `Include/iqtdoc/CMultiDocumentWorkspaceGuiComp.cpp:575-589`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqt2d/TPolygonBasedParamsGuiComp.h:591-601` ⟵ `Include/iqt2d/TPolygonBasedParamsGuiComp.h:574-584`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqt2d/TPolygonBasedParamsGuiComp.h:476-483` ⟵ `Include/iqt2d/TPolygonBasedParamsGuiComp.h:459-466`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqt2d/CViewProviderGuiComp.cpp:287-297` ⟵ `Include/iqt2d/CViewProviderGuiComp.cpp:204-214`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CInteractiveViewLayer.cpp:535-548` ⟵ `Include/iview/CInteractiveViewLayer.cpp:495-508`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqt/CSettingsWriteArchive.cpp:49-58` ⟵ `Include/iqt/CSettingsWriteArchive.cpp:37-46`
  - [verbatim within-file w=1.00 line=1.00] `Include/ifile/CComposedFilePersistenceComp.cpp:188-201` ⟵ `Include/ifile/CComposedFilePersistenceComp.cpp:131-144`
  - [near cross-file w=1.00 line=0.88] `Tests/TestComposedGui/main.cpp:18-40` ⟵ `Impl/Tests/ComposedGuiTest/main.cpp:39-61`
  - [verbatim within-file w=1.00 line=1.00] `Include/iwidgets/CHierarchicalHeaderView.cpp:562-570` ⟵ `Include/iwidgets/CHierarchicalHeaderView.cpp:476-484`
  - [verbatim within-file w=1.00 line=1.00] `Include/iwidgets/CHierarchicalHeaderView.cpp:477-485` ⟵ `Include/iwidgets/CHierarchicalHeaderView.cpp:561-569`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmm/CSpectrumInfo.cpp:102-112` ⟵ `Include/icmm/CSpectrumInfo.cpp:73-83`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmm/CSubstractiveColorModel.cpp:394-423` ⟵ `Include/icmm/CSubstractiveColorModel.cpp:319-348`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmm/TComposedColor.h:482-507` ⟵ `Include/icmm/TComposedColor.h:366-391`
  - [verbatim cross-file w=1.00 line=1.00] `Include/iprm/CMultiParamsManagerComp.cpp:313-322` ⟵ `Include/iprm/CParamsManagerCompBase.cpp:536-545`
  - [verbatim cross-file w=1.00 line=1.00] `Include/ifilegui/CFileDialogLoaderComp.cpp:140-153` ⟵ `Include/ifile/CComposedFilePersistenceComp.cpp:131-144`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CViewport.cpp:269-290` ⟵ `Include/iview/CViewport.cpp:215-235`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CViewBase.cpp:729-747` ⟵ `Include/iview/CViewBase.cpp:710-728`
  - [verbatim within-file w=1.00 line=1.00] `Include/ipackage/CPackagesLoaderComp.cpp:474-496` ⟵ `Include/ipackage/CPackagesLoaderComp.cpp:445-467`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CViewBase.cpp:641-649` ⟵ `Include/iview/CViewBase.cpp:622-630`
  - [verbatim within-file w=1.00 line=1.00] `Include/iview/CInteractiveViewLayer.cpp:555-571` ⟵ `Include/iview/CInteractiveViewLayer.cpp:515-531`
  - [verbatim within-file w=0.97 line=0.97] `Include/iview/CConsoleGui.cpp:653-701` ⟵ `Include/iview/CConsoleGui.cpp:571-618`
  - [diverged within-file w=0.96 line=0.78] `Include/iser/CPrimitiveTypesSerializer.cpp:134-200` ⟵ `Include/iser/CPrimitiveTypesSerializer.cpp:62-128`
  - [diverged within-file w=0.96 line=0.78] `Include/iser/CPrimitiveTypesSerializer.cpp:63-129` ⟵ `Include/iser/CPrimitiveTypesSerializer.cpp:133-199`
  - [diverged within-file w=0.95 line=0.56] `Include/iview/CAnnulusSegmentShape.cpp:451-457` ⟵ `Include/iview/CAnnulusSegmentShape.cpp:459-465`
  - [diverged within-file w=0.95 line=0.56] `Include/iview/CAnnulusSegmentShape.cpp:460-466` ⟵ `Include/iview/CAnnulusSegmentShape.cpp:450-456`
  - [near within-file w=0.94 line=0.86] `Include/iprm/CComposedParamsSetComp.cpp:232-250` ⟵ `Include/iprm/CComposedParamsSetComp.cpp:185-203`
  - [diverged cross-file w=0.89 line=0.49] `Include/ipackage/CRegistriesManagerComp.cpp:321-380` ⟵ `Include/ipackage/CPackagesLoaderComp.cpp:377-440`
  - [diverged cross-file w=0.88 line=0.56] `Include/iqtprm/COptionsManagerEditorComp.cpp:183-229` ⟵ `Include/iqtprm/COptionsListEditorComp.cpp:51-104`
  - [diverged cross-file w=0.88 line=0.56] `Include/iqtprm/COptionsListEditorComp.cpp:52-105` ⟵ `Include/iqtprm/COptionsManagerEditorComp.cpp:182-228`
  - [near within-file w=0.84 line=0.91] `Include/iview/CConsoleGui.cpp:822-867` ⟵ `Include/iview/CConsoleGui.cpp:735-780`
  - [near within-file w=0.81 line=0.82] `Docs/Tutorial/Impl/DocView/CTextEditorComp.cpp:75-88` ⟵ `Docs/Tutorial/Impl/DocView/CTextEditorComp.cpp:56-69`
  - [near within-file w=0.81 line=0.82] `Docs/Tutorial/Impl/DocView/CTextEditorComp.cpp:57-70` ⟵ `Docs/Tutorial/Impl/DocView/CTextEditorComp.cpp:74-87`
  - [diverged within-file w=0.80 line=0.41] `Include/iwidgets/CHierarchicalHeaderView.cpp:509-526` ⟵ `Include/iwidgets/CHierarchicalHeaderView.cpp:593-610`
  - [diverged within-file w=0.80 line=0.41] `Include/iwidgets/CHierarchicalHeaderView.cpp:594-611` ⟵ `Include/iwidgets/CHierarchicalHeaderView.cpp:508-525`
  - [diverged within-file w=0.80 line=0.67] `Include/istd/CSystem.cpp:331-372` ⟵ `Include/istd/CSystem.cpp:281-325`
  - [diverged within-file w=0.80 line=0.67] `Include/istd/CSystem.cpp:282-326` ⟵ `Include/istd/CSystem.cpp:330-371`
  - [diverged cross-file w=0.80 line=0.29] `Include/iview/CViewLayer.cpp:131-144` ⟵ `Include/iview/CInteractiveViewLayer.cpp:495-508`
  - [diverged within-file w=0.78 line=0.57] `Include/iview/CViewport.cpp:198-249` ⟵ `Include/iview/CViewport.cpp:157-195`
  - [diverged cross-file w=0.75 line=0.57] `Include/iqtprm/CExtComposedParamsSetGuiComp.cpp:183-206` ⟵ `Include/iqtprm/CComposedParamsSetGuiComp.cpp:638-658`
  - [diverged cross-file w=0.74 line=0.70] `Include/iqtprm/CFileNameParamGuiComp.cpp:171-224` ⟵ `Include/ifilegui/CSimpleFilePathParamGuiComp.cpp:157-233`
  - [diverged cross-file w=0.74 line=0.70] `Include/ifilegui/CSimpleFilePathParamGuiComp.cpp:158-234` ⟵ `Include/iqtprm/CFileNameParamGuiComp.cpp:170-223`
  - [diverged cross-file w=0.73 line=0.46] `Include/iview/CPolygonShape.cpp:452-502` ⟵ `Include/iview/CPolylineShape.cpp:486-552`
  - [diverged within-file w=0.73 line=0.25] `Include/iview/CColorSchemaComp.cpp:84-88` ⟵ `Include/iview/CColorSchemaComp.cpp:114-120`
  - [diverged within-file w=0.73 line=0.25] `Include/iview/CColorSchemaComp.cpp:94-98` ⟵ `Include/iview/CColorSchemaComp.cpp:105-111`
  - [diverged cross-file w=0.72 line=0.50] `Include/iqtprm/CComposedParamsSetGuiComp.cpp:643-668` ⟵ `Include/iqtprm/CExtComposedParamsSetGuiComp.cpp:182-205`
  - [diverged within-file w=0.71 line=0.77] `Include/iview/CConsoleGui.cpp:540-594` ⟵ `Include/iview/CConsoleGui.cpp:468-506`
  - [diverged cross-file w=0.69 line=0.47] `Include/iview/CPolylineShape.cpp:502-569` ⟵ `Include/iview/CPolygonShape.cpp:431-483`
  - [diverged within-file w=0.68 line=0.56] `Include/i2d/CAnnulusSegment.cpp:156-164` ⟵ `Include/i2d/CAnnulusSegment.cpp:137-145`
  - [diverged within-file w=0.68 line=0.56] `Include/i2d/CAnnulusSegment.cpp:140-148` ⟵ `Include/i2d/CAnnulusSegment.cpp:153-161`
  - [diverged within-file w=0.67 line=0.60] `Include/iqt2d/CViewProviderGuiComp.cpp:221-243` ⟵ `Include/iqt2d/CViewProviderGuiComp.cpp:174-186`
  - [diverged cross-file w=0.62 line=0.59] `Include/iwidgets/CMiniWidgetDelegate.cpp:89-124` ⟵ `Include/iwidgets/CCollapsibleGroupWidgetDelegate.cpp:98-151`
  - [diverged cross-file w=0.62 line=0.59] `Include/iwidgets/CCollapsibleGroupWidgetDelegate.cpp:99-152` ⟵ `Include/iwidgets/CMiniWidgetDelegate.cpp:88-123`
  - [diverged within-file w=0.61 line=0.20] `Include/iview/CToolsViewLayer.cpp:77-91` ⟵ `Include/iview/CToolsViewLayer.cpp:45-59`
  - [diverged within-file w=0.61 line=0.20] `Include/iview/CToolsViewLayer.cpp:52-66` ⟵ `Include/iview/CToolsViewLayer.cpp:70-84`
  - [diverged within-file w=0.60 line=0.40] `Include/ifilegui/CFileDialogLoaderComp.cpp:276-287` ⟵ `Include/ifilegui/CFileDialogLoaderComp.cpp:237-261`

## winnerspiros/UnleashedRecomp_Android — 1053 коммитов | циклы=2(SCC≤2) | копипаст=19
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/winnerspiros_UnleashedRecomp_Android`

### `80e1f2ecb46d` — "Merge pull request #454 from winnerspiros/copilot/fix-network-issue-wget-command" (winnerspiros, 2026-03-26) [19 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/winnerspiros_UnleashedRecomp_Android show 80e1f2ecb46d`
  - [diverged within-file w=1.00 line=0.36] `UnleashedRecomp/install/installer.cpp:69-78` ⟵ `UnleashedRecomp/install/installer.cpp:52-68`
  - [diverged within-file w=1.00 line=0.40] `UnleashedRecomp/install/installer.cpp:716-725` ⟵ `UnleashedRecomp/install/installer.cpp:695-705`
  - [verbatim cross-file w=1.00 line=1.00] `UnleashedRecomp/kernel/io/nt.cpp:28-41` ⟵ `UnleashedRecomp/kernel/imports.cpp:383-396`
  - [verbatim cross-file w=1.00 line=1.00] `UnleashedRecomp/kernel/threading.cpp:15-30` ⟵ `UnleashedRecomp/kernel/imports.cpp:571-586`
  - [verbatim cross-file w=1.00 line=1.00] `UnleashedRecomp/kernel/threading.cpp:39-50` ⟵ `UnleashedRecomp/kernel/imports.cpp:1423-1434`
  - [verbatim cross-file w=1.00 line=1.00] `UnleashedRecomp/kernel/threading.cpp:54-66` ⟵ `UnleashedRecomp/kernel/imports.cpp:640-652`
  - [diverged within-file w=1.00 line=0.36] `UnleashedRecomp/ui/installer_wizard.cpp:1601-1609` ⟵ `UnleashedRecomp/ui/installer_wizard.cpp:1622-1634`
  - [diverged within-file w=1.00 line=0.53] `UnleashedRecomp/ui/installer_wizard.cpp:999-1016` ⟵ `UnleashedRecomp/ui/installer_wizard.cpp:978-998`
  - [near within-file w=0.94 line=0.95] `UnleashedRecomp/ui/imgui_utils.cpp:378-452` ⟵ `UnleashedRecomp/ui/imgui_utils.cpp:482-556`
  - [near within-file w=0.93 line=0.89] `UnleashedRecomp/mod/mod_loader.cpp:709-754` ⟵ `UnleashedRecomp/mod/mod_loader.cpp:633-677`
  - [near within-file w=0.87 line=0.88] `UnleashedRecomp/hid/driver/sdl_hid.cpp:227-252` ⟵ `UnleashedRecomp/hid/driver/sdl_hid.cpp:185-210`
  - [near within-file w=0.86 line=0.83] `UnleashedRecomp/main.cpp:410-462` ⟵ `UnleashedRecomp/main.cpp:232-280`
  - [diverged cross-file w=0.76 line=0.45] `UnleashedRecomp/user/achievement_manager.cpp:100-176` ⟵ `UnleashedRecomp/user/persistent_storage_manager.cpp:39-97`
  - [near within-file w=0.73 line=0.80] `UnleashedRecomp/main.cpp:327-339` ⟵ `UnleashedRecomp/main.cpp:162-174`
  - [diverged within-file w=0.64 line=0.12] `UnleashedRecomp/ui/installer_wizard.cpp:1127-1160` ⟵ `UnleashedRecomp/ui/installer_wizard.cpp:926-939`
  - [diverged within-file w=0.64 line=0.12] `UnleashedRecomp/ui/installer_wizard.cpp:937-959` ⟵ `UnleashedRecomp/ui/installer_wizard.cpp:1132-1156`
  - [diverged cross-file w=0.63 line=0.44] `UnleashedRecomp/tests/test_config.cpp:210-241` ⟵ `UnleashedRecomp/user/config.cpp:794-829`
  - [diverged within-file w=0.62 line=0.17] `UnleashedRecomp/ui/installer_wizard.cpp:640-653` ⟵ `UnleashedRecomp/ui/installer_wizard.cpp:620-626`
  - [diverged cross-file w=0.60 line=0.14] `UnleashedRecomp/tests/mock/xbox.h:16-21` ⟵ `tools/x_decompress/x_decompress.cpp:32-43`

## Alteriom/painlessMesh — 1045 коммитов | циклы=0(SCC≤1) | копипаст=6

### `6e8999ddcda1` — "docs: add CLAUDE.md with project context and gotchas (#362)" (sparck75, 2026-03-28) [6 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Alteriom_painlessMesh show 6e8999ddcda1`
  - [verbatim within-file w=1.00 line=1.00] `src/arduino/wifi.hpp:1305-1328` ⟵ `src/arduino/wifi.hpp:208-231`
  - [verbatim within-file w=1.00 line=1.00] `src/painlessmesh/mesh.hpp:3310-3362` ⟵ `src/painlessmesh/mesh.hpp:498-550`
  - [verbatim within-file w=1.00 line=0.97] `src/painlessmesh/connection.hpp:181-226` ⟵ `src/painlessmesh/connection.hpp:53-96`
  - [diverged within-file w=0.87 line=0.71] `src/painlessmesh/connection.hpp:298-330` ⟵ `src/painlessmesh/connection.hpp:155-179`
  - [diverged within-file w=0.72 line=0.69] `src/painlessmesh/plugin.hpp:227-254` ⟵ `src/painlessmesh/plugin.hpp:158-173`
  - [diverged within-file w=0.70 line=0.50] `test/include/catch2/catch.hpp:16813-16834` ⟵ `test/include/catch2/catch.hpp:2148-2167`

## alexandrosk0/Smatchet — 983 коммитов | циклы=0(SCC≤1) | копипаст=303

### `8606f9b9578d` — "fix(build): skip json submodules + document FetchContent vcvars/cross-worktree traps (#656)" (Alexandros Konstantonis, 2026-05-31) [303 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/alexandrosk0_Smatchet show 8606f9b9578d`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetNewIssueDraftUi.cpp:294-299` ⟵ `Source_Core/src/SmatchetNewIssueDraftUi.cpp:268-273`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPerfUi.cpp:81-119` ⟵ `Source_Core/src/SmatchetPerfUi.cpp:79-117`
  - [diverged cross-file w=1.00 line=0.78] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:874-909` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:851-885`
  - [diverged cross-file w=1.00 line=0.79] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:844-861` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:822-838`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:646-683` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:644-681`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:547-558` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:545-556`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:463-494` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:461-492`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:155-201` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:153-199`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetOfflineQueueUi.cpp:112-140` ⟵ `Source_Core/src/SmatchetOfflineQueueUi.cpp:110-138`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPerfUi.cpp:257-303` ⟵ `Source_Core/src/SmatchetPerfUi.cpp:239-285`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetNewIssueDraftUi.cpp:160-196` ⟵ `Source_Core/src/SmatchetNewIssueDraftUi.cpp:136-172`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetNewIssueDraftUi.cpp:90-106` ⟵ `Source_Core/src/SmatchetNewIssueDraftUi.cpp:284-300`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetMcpServerUi.cpp:301-329` ⟵ `Source_Core/src/SmatchetMcpServerUi.cpp:294-322`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetMcpServerUi.cpp:185-191` ⟵ `Source_Core/src/SmatchetMcpServerUi.cpp:178-184`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetMcpServerUi.cpp:60-81` ⟵ `Source_Core/src/SmatchetMcpServerUi.cpp:58-79`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetImGuiHost.cpp:1131-1164` ⟵ `Source_Core/src/SmatchetImGuiHost.cpp:879-912`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetImGuiHost.cpp:620-645` ⟵ `Source_Core/src/SmatchetImGuiHost.cpp:525-550`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetImGuiHost.cpp:468-486` ⟵ `Source_Core/src/SmatchetImGuiHost.cpp:376-394`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetProjectPicker.cpp:132-149` ⟵ `Source_Core/src/SmatchetProjectPicker.cpp:133-150`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI.cpp:901-934` ⟵ `Source_Core/src/SmatchetUI.cpp:826-859`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI.cpp:366-388` ⟵ `Source_Core/src/SmatchetUI.cpp:362-384`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI.cpp:342-363` ⟵ `Source_Core/src/SmatchetUI.cpp:338-359`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI.cpp:263-294` ⟵ `Source_Core/src/SmatchetUI.cpp:259-290`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI.cpp:77-80` ⟵ `Source_Core/src/SmatchetUI.cpp:74-77`
  - [near cross-file w=1.00 line=0.94] `Source/Core/src/Ui/SmatchetThemedTextEditorPalette.cpp:46-82` ⟵ `Source_Core/src/SmatchetThemedTextEditorPalette.cpp:47-83`
  - [verbatim cross-file w=1.00 line=0.96] `Source/Core/src/Ui/SmatchetTheme.cpp:697-776` ⟵ `Source_Core/src/SmatchetTheme.cpp:697-779`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetTheme.cpp:668-692` ⟵ `Source_Core/src/SmatchetTheme.cpp:668-692`
  - [near cross-file w=1.00 line=0.93] `Source/Core/src/Ui/SmatchetImGuiHost.cpp:383-448` ⟵ `Source_Core/src/SmatchetImGuiHost.cpp:290-356`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPreferencesUi_Whisper.cpp:234-287` ⟵ `Source_Core/src/SmatchetPreferencesUi.cpp:1172-1225`
  - [diverged cross-file w=1.00 line=0.74] `Source/Core/src/Ui/SmatchetPreferencesUi_Local.cpp:207-224` ⟵ `Source_Core/src/SmatchetPreferencesUi.cpp:1862-1880`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPreferencesUi_Local.cpp:68-103` ⟵ `Source_Core/src/SmatchetPreferencesUi.cpp:116-151`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPreferencesUi_Assistant.cpp:101-140` ⟵ `Source_Core/src/SmatchetPreferencesUi.cpp:509-548`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPreferencesUi.cpp:380-396` ⟵ `Source_Core/src/SmatchetPreferencesUi.cpp:413-429`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPlanDocViewerUi.cpp:292-304` ⟵ `Source_Core/src/SmatchetPlanDocViewerUi.cpp:231-243`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPlanDocViewerUi.cpp:41-65` ⟵ `Source_Core/src/SmatchetPlanDocViewerUi.cpp:38-62`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetPerfUi.cpp:307-321` ⟵ `Source_Core/src/SmatchetPerfUi.cpp:289-303`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetActiveProjectGridUi.cpp:60-123` ⟵ `Source_Core/src/SmatchetActiveProjectGridUi.cpp:59-122`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:301-315` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:300-314`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAiAssistantUi.cpp:810-880` ⟵ `Source_Core/src/SmatchetAiAssistantUi.cpp:823-893`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAiAssistantUi.cpp:291-323` ⟵ `Source_Core/src/SmatchetAiAssistantUi.cpp:303-335`
  - [near cross-file w=1.00 line=0.94] `Source/Core/src/Ui/SmatchetAiAssistantUi.cpp:102-156` ⟵ `Source_Core/src/SmatchetAiAssistantUi.cpp:113-168`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetActiveProjectGridUi.cpp:1096-1099` ⟵ `Source_Core/src/SmatchetActiveProjectGridUi.cpp:744-747`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/include/Commands/MainThreadDispatch.h:45-64` ⟵ `Source_Core/include/Commands/MainThreadDispatch.h:50-69`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetActiveProjectGridUi.cpp:656-667` ⟵ `Source_Core/src/SmatchetActiveProjectGridUi.cpp:411-422`
  - [near cross-file w=1.00 line=0.82] `Source/Core/src/Ui/SmatchetActiveProjectGridUi.cpp:146-186` ⟵ `Source_Core/src/SmatchetActiveProjectGridUi.cpp:570-611`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:319-347` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:318-346`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:993-1022` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:993-1022`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:930-949` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:930-949`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:780-786` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:779-785`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:771-776` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:770-775`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:734-751` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:733-750`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:712-721` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:711-720`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:696-706` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:695-705`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownPreviewRender.cpp:466-496` ⟵ `Source_Core/src/MarkdownPreviewRender.cpp:465-495`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAutocompleteUi.cpp:143-177` ⟵ `Source_Core/src/SmatchetAutocompleteUi.cpp:142-176`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetImGuiFonts.cpp:156-195` ⟵ `Source_Core/src/SmatchetImGuiFonts.cpp:156-195`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetGridUiSupport.cpp:357-391` ⟵ `Source_Core/src/SmatchetGridUiSupport.cpp:362-396`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetGridUiSupport.cpp:46-61` ⟵ `Source_Core/src/SmatchetGridUiSupport.cpp:45-60`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetGridHeaderUi.cpp:405-428` ⟵ `Source_Core/src/SmatchetGridHeaderUi.cpp:402-425`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetBulkTicketsUi.cpp:613-634` ⟵ `Source_Core/src/SmatchetBulkTicketsUi.cpp:589-610`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetBulkTicketsUi.cpp:352-388` ⟵ `Source_Core/src/SmatchetBulkTicketsUi.cpp:338-374`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetBulkTicketsUi.cpp:218-228` ⟵ `Source_Core/src/SmatchetBulkTicketsUi.cpp:206-216`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAutocompleteUi.cpp:281-305` ⟵ `Source_Core/src/SmatchetAutocompleteUi.cpp:280-304`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI_Layout.cpp:249-284` ⟵ `Source_Core/src/SmatchetUI_Layout.cpp:239-274`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAutocompleteUi.cpp:85-102` ⟵ `Source_Core/src/SmatchetAutocompleteUi.cpp:84-101`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAutocompleteUi.cpp:70-75` ⟵ `Source_Core/src/SmatchetAutocompleteUi.cpp:69-74`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAuditUi.cpp:276-289` ⟵ `Source_Core/src/SmatchetAuditUi.cpp:273-286`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:717-720` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:716-719`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:660-672` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:659-671`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:560-610` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:559-609`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:520-543` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:519-542`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetAttachmentPreviewUi.cpp:417-450` ⟵ `Source_Core/src/SmatchetAttachmentPreviewUi.cpp:416-449`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Standalone/main.cpp:455-473` ⟵ `Target_Standalone/main.cpp:416-434`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:311-345` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:298-332`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:186-207` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:186-207`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:150-168` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:150-168`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:103-119` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:103-119`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:83-100` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:83-100`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:71-80` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:71-80`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:49-64` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:49-64`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Standalone/main.cpp:652-663` ⟵ `Target_Standalone/main.cpp:629-640`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:399-416` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:386-403`
  - [near cross-file w=1.00 line=0.83] `Source/Standalone/main.cpp:135-172` ⟵ `Target_Standalone/main.cpp:134-169`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Standalone/CliCommandRunner.cpp:937-944` ⟵ `Target_Standalone/CliCommandRunner.cpp:813-820`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Standalone/CliCommandRunner.cpp:558-572` ⟵ `Target_Standalone/CliCommandRunner.cpp:303-317`
  - [diverged cross-file w=1.00 line=0.37] `Source/Standalone/CliCommandRunner.cpp:413-454` ⟵ `Target_Standalone/CliCommandRunner.cpp:522-566`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Standalone/CliCommandRunner.cpp:240-281` ⟵ `Target_Standalone/CliCommandRunner.cpp:221-262`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WindowsAudioCapture.cpp:494-553` ⟵ `Plugins/Whisper/WindowsAudioCapture.cpp:515-574`
  - [diverged cross-file w=1.00 line=0.65] `Source/Plugins/Whisper/WindowsAudioCapture.cpp:181-224` ⟵ `Plugins/Whisper/WindowsAudioCapture.cpp:189-228`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:1040-1064` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:1043-1067`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/CallstackParser.test.cpp:225-274` ⟵ `tests/Source_Core/CallstackParser.test.cpp:225-274`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/WhisperConsentGate.test.cpp:93-104` ⟵ `tests/Source_Core/WhisperConsentGate.test.cpp:93-104`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/WhisperConsentGate.test.cpp:54-90` ⟵ `tests/Source_Core/WhisperConsentGate.test.cpp:54-90`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/WhisperConsentGate.test.cpp:38-51` ⟵ `tests/Source_Core/WhisperConsentGate.test.cpp:38-51`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/OfflineQueueServiceRuntime.test.cpp:283-304` ⟵ `tests/Source_Core/OfflineQueueServiceRuntime.test.cpp:283-304`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/MarkdownLanguageDefinition.test.cpp:170-187` ⟵ `tests/Source_Core/MarkdownLanguageDefinition.test.cpp:170-187`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/MarkdownLanguageDefinition.test.cpp:114-126` ⟵ `tests/Source_Core/MarkdownLanguageDefinition.test.cpp:114-126`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/MarkdownLanguageDefinition.test.cpp:72-83` ⟵ `tests/Source_Core/MarkdownLanguageDefinition.test.cpp:72-83`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/LocalCacheManager.test.cpp:160-167` ⟵ `tests/Source_Core/LocalCacheManager.test.cpp:160-167`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:837-877` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:840-880`
  - [verbatim cross-file w=1.00 line=1.00] `tests/Core/AiNdjsonParser.test.cpp:134-152` ⟵ `tests/Source_Core/AiNdjsonParser.test.cpp:134-152`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:318-330` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:308-320`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:275-315` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:265-305`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:245-272` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:235-262`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:227-241` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:217-231`
  - [diverged cross-file w=1.00 line=0.43] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:209-224` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:194-214`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:55-65` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:54-64`
  - [verbatim cross-file w=1.00 line=1.00] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:39-52` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiPluginModule.cpp:38-51`
  - [verbatim cross-file w=1.00 line=0.97] `Source/Core/src/Ui/SmatchetViewsDashboardUi_widgets.cpp:147-199` ⟵ `Source_Core/src/SmatchetViewsDashboardUi_widgets.cpp:146-199`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:321-357` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:310-346`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:294-318` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:283-307`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:283-289` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:272-278`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:264-280` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:253-269`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:216-238` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:205-227`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:193-213` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:182-202`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetWhisperOverlayUi.cpp:28-82` ⟵ `Source_Core/src/SmatchetWhisperOverlayUi.cpp:30-84`
  - [near cross-file w=1.00 line=0.86] `Source/Core/src/Ui/SmatchetViewsDashboardUi_widgets.cpp:345-371` ⟵ `Source_Core/src/SmatchetViewsDashboardUi_widgets.cpp:345-372`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:577-605` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:566-594`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetViewsDashboardUi_widgets.cpp:114-144` ⟵ `Source_Core/src/SmatchetViewsDashboardUi_widgets.cpp:113-143`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetViewsDashboardUi.cpp:427-469` ⟵ `Source_Core/src/SmatchetViewsDashboardUi.cpp:426-468`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetViewsDashboardUi.cpp:292-337` ⟵ `Source_Core/src/SmatchetViewsDashboardUi.cpp:291-336`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUtilityWindowsUi.cpp:184-192` ⟵ `Source_Core/src/SmatchetUtilityWindowsUi.cpp:171-179`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUtilityWindowsUi.cpp:148-166` ⟵ `Source_Core/src/SmatchetUtilityWindowsUi.cpp:135-153`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUtilityWindowsUi.cpp:85-111` ⟵ `Source_Core/src/SmatchetUtilityWindowsUi.cpp:84-110`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUtilityWindowsUi.cpp:22-65` ⟵ `Source_Core/src/SmatchetUtilityWindowsUi.cpp:21-64`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetUI_MainMenu.cpp:464-484` ⟵ `Source_Core/src/SmatchetUI_MainMenu.cpp:459-479`
  - [near cross-file w=1.00 line=0.93] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:342-405` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:346-410`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:747-798` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:750-801`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:469-484` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:471-486`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:347-372` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:349-374`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:203-241` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:204-242`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperPlugin.cpp:87-91` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:87-91`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperLocal.cpp:124-182` ⟵ `Plugins/Whisper/WhisperLocal.cpp:125-183`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/WhisperLocal.cpp:83-113` ⟵ `Plugins/Whisper/WhisperLocal.cpp:84-114`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:408-422` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:413-427`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/SmatchetActiveProjectGridUi.cpp:671-699` ⟵ `Source_Core/src/SmatchetActiveProjectGridUi.cpp:426-454`
  - [diverged cross-file w=1.00 line=0.70] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:302-309` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:301-309`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:291-297` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:289-295`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:249-263` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:247-261`
  - [near cross-file w=1.00 line=0.86] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:158-225` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:153-223`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Whisper/GlobalHotkey_Win32.cpp:147-155` ⟵ `Plugins/Whisper/GlobalHotkey_Win32.cpp:142-150`
  - [diverged cross-file w=1.00 line=0.64] `Source/Plugins/Mcp/McpPlugin.cpp:613-650` ⟵ `Plugins/Mcp/McpPlugin.cpp:657-697`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/Mcp/McpJsonRpcPure.cpp:199-213` ⟵ `Plugins/Mcp/McpJsonRpcPure.cpp:199-213`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:655-685` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:644-674`
  - [diverged cross-file w=1.00 line=0.11] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:416-421` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:414-419`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/DictationInsertionRouter_Whisper.cpp:168-211` ⟵ `Source_Core/src/DictationInsertionRouter_Whisper.cpp:168-211`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_Views.cpp:324-342` ⟵ `Source_Core/src/ConfigManager_Views.cpp:295-313`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:688-729` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:694-735`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:598-608` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:604-614`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:539-568` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:545-574`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:502-527` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:508-533`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:482-499` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:488-505`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:444-478` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:450-484`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Logger.cpp:135-186` ⟵ `Source_Core/src/Logger.cpp:135-186`
  - [diverged cross-file w=1.00 line=0.11] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:408-413` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:422-427`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:303-321` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:310-328`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:285-300` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:292-307`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:220-234` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:225-239`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:183-217` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:188-222`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:153-164` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:156-167`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:103-136` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:106-139`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Persistence/LocalCacheManager.cpp:226-242` ⟵ `Source_Core/src/LocalCacheManager.cpp:226-242`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SubprocessCapture.cpp:293-320` ⟵ `Source_Core/src/SubprocessCapture.cpp:293-320`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SubprocessCapture.cpp:199-223` ⟵ `Source_Core/src/SubprocessCapture.cpp:199-223`
  - [diverged cross-file w=1.00 line=0.50] `Source/Core/src/SmatchetMergeWatchNotifyServer.cpp:83-122` ⟵ `Source_Core/src/SmatchetMergeWatchNotifyServer.cpp:90-139`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SmatchetLocalization.cpp:764-782` ⟵ `Source_Core/src/SmatchetLocalization.cpp:675-693`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SmatchetLocalization.cpp:734-749` ⟵ `Source_Core/src/SmatchetLocalization.cpp:645-660`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SmatchetLocalization.cpp:713-731` ⟵ `Source_Core/src/SmatchetLocalization.cpp:624-642`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SmatchetLocalization.cpp:660-684` ⟵ `Source_Core/src/SmatchetLocalization.cpp:571-595`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SmatchetLocalization.cpp:621-644` ⟵ `Source_Core/src/SmatchetLocalization.cpp:532-555`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Persistence/LocalCacheManager.cpp:558-588` ⟵ `Source_Core/src/LocalCacheManager.cpp:558-588`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Config/ConfigManager_PathUtils.cpp:75-100` ⟵ `Source_Core/src/ConfigManager_PathUtils.cpp:78-103`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Persistence/LocalCacheManager.cpp:141-190` ⟵ `Source_Core/src/LocalCacheManager.cpp:141-190`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownConvert.cpp:715-757` ⟵ `Source_Core/src/MarkdownConvert.cpp:718-760`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Persistence/BackendAuditTrail.cpp:125-148` ⟵ `Source_Core/src/BackendAuditTrail.cpp:125-148`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Persistence/BackendAuditTrail.cpp:88-107` ⟵ `Source_Core/src/BackendAuditTrail.cpp:88-107`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/P4AnnotateParse.cpp:35-74` ⟵ `Source_Core/src/P4BlameParse.cpp:35-74`
  - [near cross-file w=1.00 line=0.94] `Source/Core/src/P4Annotate.cpp:269-311` ⟵ `Source_Core/src/P4Blame.cpp:259-301`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/P4Annotate.cpp:234-265` ⟵ `Source_Core/src/P4Blame.cpp:224-255`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Logger.cpp:204-228` ⟵ `Source_Core/src/Logger.cpp:204-228`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_CatalogAndFieldEdit.cpp:54-69` ⟵ `Source_Core/src/AppController_CatalogAndFieldEdit.cpp:54-69`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings.cpp:320-361` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:393-434`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings.cpp:288-303` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:370-385`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings.cpp:259-285` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:341-367`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings.cpp:86-120` ⟵ `Source_Core/src/AppController_LuaBindingsCore.cpp:61-95`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_Connectivity.cpp:283-294` ⟵ `Source_Core/src/AppController_Connectivity.cpp:271-282`
  - [near cross-file w=1.00 line=0.89] `Source/Core/src/AppController_Connectivity.cpp:81-124` ⟵ `Source_Core/src/AppController_Connectivity.cpp:78-121`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_CatalogAndFieldEdit.cpp:572-584` ⟵ `Source_Core/src/AppController_CatalogAndFieldEdit.cpp:567-579`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_CatalogAndFieldEdit.cpp:311-332` ⟵ `Source_Core/src/AppController_CatalogAndFieldEdit.cpp:307-328`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings.cpp:614-632` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:677-695`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController.cpp:1562-1575` ⟵ `Source_Core/src/AppController.cpp:1565-1578`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController.cpp:951-997` ⟵ `Source_Core/src/AppController.cpp:941-987`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController.cpp:597-628` ⟵ `Source_Core/src/AppController.cpp:587-618`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController.cpp:576-593` ⟵ `Source_Core/src/AppController.cpp:566-583`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController.cpp:259-288` ⟵ `Source_Core/src/AppController.cpp:254-283`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AiSseParser.cpp:40-100` ⟵ `Source_Core/src/AiSseParser.cpp:40-100`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/include/Tracker/TrackerError.h:98-120` ⟵ `Source_Core/include/TrackerError.h:101-123`
  - [diverged cross-file w=1.00 line=0.79] `Source/Core/include/SmatchetLocalizedImGui.h:102-114` ⟵ `Source_Core/include/SmatchetLocalizedImGui.h:101-114`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/Builtin/BuiltinCommands_Debug.cpp:74-91` ⟵ `Source_Core/src/Commands/Builtin/BuiltinCommands_Debug.cpp:72-89`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/ViewToggleCommands.cpp:29-39` ⟵ `Source_Core/src/Commands/ViewToggleCommands.cpp:30-40`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/Scenarios/ScenarioRunner.cpp:123-141` ⟵ `Source_Core/src/Commands/Scenarios/ScenarioRunner.cpp:120-138`
  - [near cross-file w=1.00 line=0.89] `Source/Core/src/Commands/Scenarios/ScenarioRunner.cpp:84-120` ⟵ `Source_Core/src/Commands/Scenarios/ScenarioRunner.cpp:82-117`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/CommandPaletteUi.cpp:161-181` ⟵ `Source_Core/src/Commands/CommandPaletteUi.cpp:161-181`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/CommandPaletteUi.cpp:99-140` ⟵ `Source_Core/src/Commands/CommandPaletteUi.cpp:99-140`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/Builtin/BuiltinCommands_Scenario.cpp:33-87` ⟵ `Source_Core/src/Commands/Builtin/BuiltinCommands_Scenario.cpp:35-89`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/Builtin/BuiltinCommands_Perf.cpp:86-120` ⟵ `Source_Core/src/Commands/Builtin/BuiltinCommands_Perf.cpp:86-120`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/Builtin/BuiltinCommands_Debug.cpp:122-150` ⟵ `Source_Core/src/Commands/Builtin/BuiltinCommands_Debug.cpp:120-148`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Persistence/LocalCacheManager.cpp:27-127` ⟵ `Source_Core/src/LocalCacheManager.cpp:27-127`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Commands/Builtin/BuiltinCommands_Ai.cpp:291-313` ⟵ `Source_Core/src/Commands/Builtin/BuiltinCommands_Ai.cpp:294-316`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings_Draw.cpp:1046-1101` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:2427-2482`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings_Draw.cpp:580-630` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:1998-2048`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings_Draw.cpp:400-412` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:1818-1830`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindingsCore.cpp:273-300` ⟵ `Source_Core/src/AppController_LuaBindingsCore.cpp:265-292`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindingsCore.cpp:65-99` ⟵ `Source_Core/src/AppController_LuaBindingsCore.cpp:61-95`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/AppController_LuaBindings.cpp:1268-1317` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:1307-1356`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerGridFieldDisplay.cpp:642-667` ⟵ `Source_Core/src/TrackerGridFieldDisplay.cpp:639-664`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/FieldCatalogCache.cpp:355-371` ⟵ `Source_Core/src/FieldCatalogCache.cpp:359-375`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/FieldCatalogCache.cpp:444-486` ⟵ `Source_Core/src/FieldCatalogCache.cpp:446-488`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/FieldCatalogCache.cpp:491-541` ⟵ `Source_Core/src/FieldCatalogCache.cpp:493-543`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/IssueCreatePipeline.cpp:88-134` ⟵ `Source_Core/src/IssueCreatePipeline.cpp:87-133`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/IssueCreatePipeline.cpp:221-272` ⟵ `Source_Core/src/IssueCreatePipeline.cpp:220-271`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/JiraIssueMutation.cpp:300-337` ⟵ `Source_Core/src/JiraIssueMutation.cpp:300-337`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/LabelEditDiffPure.cpp:15-22` ⟵ `Source_Core/src/LabelEditDiffPure.cpp:14-21`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/PlaneIssueMutation.cpp:46-110` ⟵ `Source_Core/src/PlaneIssueMutation.cpp:46-110`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/PlaneIssueSearch.cpp:436-486` ⟵ `Source_Core/src/PlaneIssueSearch.cpp:504-554`
  - [near cross-file w=1.00 line=0.87] `Source/Core/src/SubprocessCapture.cpp:405-454` ⟵ `Source_Core/src/SubprocessCapture.cpp:405-454`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerFieldPayloadPure.cpp:131-146` ⟵ `Source_Core/src/TrackerFieldPayloadPure.cpp:131-146`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerFieldPayloadPure.cpp:365-384` ⟵ `Source_Core/src/TrackerFieldPayloadPure.cpp:369-388`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerGridFieldDisplay.cpp:304-336` ⟵ `Source_Core/src/TrackerGridFieldDisplay.cpp:302-334`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerGridFieldDisplay.cpp:582-590` ⟵ `Source_Core/src/TrackerGridFieldDisplay.cpp:579-587`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerGridFieldDisplay.cpp:617-625` ⟵ `Source_Core/src/TrackerGridFieldDisplay.cpp:614-622`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerFieldPayloadPure.cpp:104-127` ⟵ `Source_Core/src/TrackerFieldPayloadPure.cpp:104-127`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/TrackerGridFieldDisplay.cpp:755-804` ⟵ `Source_Core/src/TrackerGridFieldDisplay.cpp:720-769`
  - [diverged cross-file w=1.00 line=0.62] `Source/Core/src/Ui/AnnotateAnalysisUi_Config.cpp:154-180` ⟵ `Source_Core/src/BlameAnalysisUi_Config.cpp:131-157`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/AnnotateAnalysisUi_Launch.cpp:143-161` ⟵ `Source_Core/src/BlameAnalysisUi_Launch.cpp:142-160`
  - [near cross-file w=1.00 line=0.93] `Source/Core/src/Ui/AnnotateAnalysisUi_Window.cpp:311-342` ⟵ `Source_Core/src/BlameAnalysisUi_Window.cpp:300-331`
  - [diverged cross-file w=1.00 line=0.42] `Source/Core/src/Ui/AnnotateAnalysisUi_Window.cpp:724-743` ⟵ `Source_Core/src/BlameAnalysisUi_Window.cpp:712-731`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/CodeColorView.cpp:219-246` ⟵ `Source_Core/src/CodeColorView.cpp:227-254`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/CodeColorView.cpp:321-348` ⟵ `Source_Core/src/CodeColorView.cpp:331-358`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/CodeColorView.cpp:370-388` ⟵ `Source_Core/src/CodeColorView.cpp:380-398`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/CodeColorView.cpp:448-467` ⟵ `Source_Core/src/CodeColorView.cpp:461-480`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/CodeColorView.cpp:562-586` ⟵ `Source_Core/src/CodeColorView.cpp:579-603`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/CodeColorView.cpp:589-653` ⟵ `Source_Core/src/CodeColorView.cpp:606-670`
  - [near cross-file w=1.00 line=0.85] `Source/Core/src/Ui/CppSyntaxHighlight.cpp:144-156` ⟵ `Source_Core/src/CppSyntaxHighlight.cpp:144-156`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownConvert.cpp:265-315` ⟵ `Source_Core/src/MarkdownConvert.cpp:266-316`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Ui/MarkdownConvert.cpp:397-453` ⟵ `Source_Core/src/MarkdownConvert.cpp:398-454`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Sync/TicketSyncService.cpp:336-344` ⟵ `Source_Core/src/TicketSyncService.cpp:330-338`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/TicketFieldEditor.cpp:792-821` ⟵ `Source_Core/src/TicketFieldEditor.cpp:926-955`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Sync/TicketSyncService.cpp:177-231` ⟵ `Source_Core/src/TicketSyncService.cpp:172-226`
  - [diverged cross-file w=1.00 line=0.75] `Source/Core/src/TicketFieldEditor_Modal.cpp:512-528` ⟵ `Source_Core/src/TicketFieldEditor.cpp:1726-1743`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Sync/TicketSyncService.cpp:60-66` ⟵ `Source_Core/src/TicketSyncService.cpp:60-66`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/TicketFieldEditor.cpp:590-601` ⟵ `Source_Core/src/TicketFieldEditor.cpp:724-735`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/TicketGridModel.cpp:116-135` ⟵ `Source_Core/src/TicketGridModel.cpp:116-135`
  - [near cross-file w=1.00 line=0.86] `Source/Core/src/TicketGridModel.cpp:378-420` ⟵ `Source_Core/src/TicketGridModel.cpp:377-420`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/TicketFieldEditor.cpp:480-518` ⟵ `Source_Core/src/TicketFieldEditor.cpp:614-652`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SubprocessCapture.cpp:573-584` ⟵ `Source_Core/src/SubprocessCapture.cpp:573-584`
  - [diverged cross-file w=1.00 line=0.43] `Source/Core/src/Tracker/FieldCatalogCache.cpp:212-225` ⟵ `Source_Core/src/FieldCatalogCache.cpp:214-231`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SubprocessCapture.cpp:515-521` ⟵ `Source_Core/src/SubprocessCapture.cpp:515-521`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/TicketFieldEditor.cpp:341-374` ⟵ `Source_Core/src/TicketFieldEditor.cpp:509-542`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/TicketFieldEditor.cpp:326-340` ⟵ `Source_Core/src/TicketFieldEditor.cpp:494-508`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/SubprocessCapture.cpp:495-503` ⟵ `Source_Core/src/SubprocessCapture.cpp:495-503`
  - [verbatim cross-file w=1.00 line=1.00] `Source/Core/src/Tracker/FieldCatalogCache.cpp:230-271` ⟵ `Source_Core/src/FieldCatalogCache.cpp:236-277`
  - [verbatim cross-file w=0.97 line=0.98] `Source/Core/src/Commands/Scenarios/UiTestScenario.cpp:64-120` ⟵ `Source_Core/src/Commands/Scenarios/UiTestScenario.cpp:64-119`
  - [verbatim cross-file w=0.97 line=0.95] `Source/Core/src/Ui/SmatchetAuditUi.cpp:169-200` ⟵ `Source_Core/src/SmatchetAuditUi.cpp:167-197`
  - [near cross-file w=0.97 line=0.94] `Source/Core/src/Tracker/TrackerHttpClient.cpp:35-85` ⟵ `Source_Core/src/TrackerHttpClient.cpp:35-85`
  - [diverged cross-file w=0.96 line=0.31] `Source/Core/src/Ui/SmatchetPlanDocViewerUi.cpp:147-169` ⟵ `Source_Core/src/SmatchetPlanDocViewerUi.cpp:132-153`
  - [near cross-file w=0.96 line=0.85] `Source/Core/src/Persistence/BackendAuditTrail.cpp:348-368` ⟵ `Source_Core/src/BackendAuditTrail.cpp:349-368`
  - [near cross-file w=0.96 line=0.92] `Source/Core/src/Ui/SmatchetGridHeaderUi.cpp:198-227` ⟵ `Source_Core/src/SmatchetGridHeaderUi.cpp:197-224`
  - [near cross-file w=0.95 line=0.90] `Source/Core/src/Tracker/TrackerFieldPayloadPure.cpp:432-444` ⟵ `Source_Core/src/TrackerFieldPayloadPure.cpp:401-414`
  - [near cross-file w=0.95 line=0.82] `Source/Core/src/Ui/SmatchetFieldRender.cpp:27-90` ⟵ `Source_Core/src/SmatchetFieldRender.cpp:27-83`
  - [near cross-file w=0.95 line=0.89] `Source/Core/src/AppController.cpp:371-430` ⟵ `Source_Core/src/AppController.cpp:366-420`
  - [verbatim cross-file w=0.95 line=0.97] `Source/Core/src/Ui/SmatchetImGuiHost.cpp:653-690` ⟵ `Source_Core/src/SmatchetImGuiHost.cpp:558-596`
  - [verbatim cross-file w=0.94 line=0.95] `Source/Core/src/Commands/BuiltinCommands.cpp:43-64` ⟵ `Source_Core/src/Commands/BuiltinCommands.cpp:32-52`
  - [near cross-file w=0.94 line=0.90] `Source/Core/src/AppController.cpp:1281-1304` ⟵ `Source_Core/src/AppController.cpp:1198-1221`
  - [near cross-file w=0.92 line=0.82] `Source/Core/src/Ui/SmatchetGridUiSupport.cpp:164-218` ⟵ `Source_Core/src/SmatchetGridUiSupport.cpp:167-223`
  - [diverged cross-file w=0.92 line=0.78] `Source/Core/src/Ui/SmatchetImGuiHost.cpp:752-813` ⟵ `Source_Core/src/SmatchetImGuiHost.cpp:653-714`
  - [diverged cross-file w=0.92 line=0.63] `Source/Standalone/StandaloneAppBootstrap.cpp:211-251` ⟵ `Target_Standalone/main.cpp:655-695`
  - [verbatim cross-file w=0.92 line=0.97] `Source/Core/src/AppController.cpp:1888-1948` ⟵ `Source_Core/src/AppController.cpp:1886-1946`
  - [verbatim cross-file w=0.91 line=0.95] `Source/Plugins/Whisper/WhisperPlugin.cpp:988-1037` ⟵ `Plugins/Whisper/WhisperPlugin.cpp:991-1040`
  - [diverged cross-file w=0.91 line=0.53] `Source/Core/src/TicketFieldEditor_Modal.cpp:291-341` ⟵ `Source_Core/src/TicketFieldEditor.cpp:1491-1544`
  - [near cross-file w=0.89 line=0.88] `Source/Core/src/Ui/SmatchetBulkTicketsUi.cpp:251-284` ⟵ `Source_Core/src/SmatchetBulkTicketsUi.cpp:238-270`
  - [near cross-file w=0.89 line=0.85] `Source/Core/src/Ui/AnnotateAnalysisUi_Worker.cpp:95-122` ⟵ `Source_Core/src/BlameAnalysisUi_Worker.cpp:95-122`
  - [near cross-file w=0.86 line=0.92] `Source/Core/src/AppController_CatalogAndFieldEdit.cpp:510-568` ⟵ `Source_Core/src/AppController_CatalogAndFieldEdit.cpp:506-563`
  - [near cross-file w=0.86 line=0.89] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:74-98` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:66-87`
  - [near cross-file w=0.84 line=0.81] `Source/Core/src/Ui/AnnotateAnalysisUi_Window.cpp:233-299` ⟵ `Source_Core/src/BlameAnalysisUi_Window.cpp:222-288`
  - [diverged cross-file w=0.83 line=0.42] `Source/Core/src/Ui/SmatchetActiveProjectGridUi.cpp:906-924` ⟵ `Source_Core/src/SmatchetActiveProjectGridUi.cpp:842-862`
  - [diverged cross-file w=0.83 line=0.75] `Source/Core/src/Ui/SmatchetPlanDocViewerUi.cpp:70-97` ⟵ `Source_Core/src/SmatchetPlanDocViewerUi.cpp:67-89`
  - [near cross-file w=0.83 line=0.85] `Source/Core/src/Ui/SmatchetUI_Layout.cpp:129-167` ⟵ `Source_Core/src/SmatchetUI_Layout.cpp:128-159`
  - [near cross-file w=0.82 line=0.93] `Source/Core/src/Commands/Scenarios/WhisperAiAssistantAutosendScenario.cpp:302-333` ⟵ `Source_Core/src/Commands/Scenarios/WhisperAiAssistantAutosendScenario.cpp:299-328`
  - [diverged cross-file w=0.82 line=0.75] `Source/Core/src/AppController_LuaBindings.cpp:720-728` ⟵ `Source_Core/src/AppController_LuaBindings.cpp:783-791`
  - [near cross-file w=0.81 line=0.89] `Source/Standalone/CliCommandRunner.cpp:1208-1252` ⟵ `Target_Standalone/CliCommandRunner.cpp:896-933`
  - [diverged cross-file w=0.81 line=0.57] `Source/Standalone/main.cpp:693-732` ⟵ `Target_Standalone/main.cpp:655-695`
  - [diverged cross-file w=0.80 line=0.71] `Source/Plugins/Mcp/McpPlugin.cpp:548-589` ⟵ `Plugins/Mcp/McpPlugin.cpp:532-570`
  - [diverged cross-file w=0.80 line=0.67] `Source/UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:213-268` ⟵ `UnrealPlugins/SmatchetImGuiPlugin/Source/SmatchetImGuiPlugin/Private/SmatchetImGuiInputProcessor.cpp:213-258`
  - [diverged cross-file w=0.76 line=0.70] `Source/Standalone/main.cpp:188-198` ⟵ `Target_Standalone/main.cpp:185-194`
  - [diverged cross-file w=0.75 line=0.71] `Source/Core/src/AppController_CatalogAndFieldEdit.cpp:1246-1270` ⟵ `Source_Core/src/AppController_CatalogAndFieldEdit.cpp:1191-1211`
  - [diverged cross-file w=0.75 line=0.47] `Source/Core/src/Ui/AnnotateAnalysisUi.cpp:100-111` ⟵ `Source_Core/src/BlameAnalysisUi.cpp:103-114`
  - [diverged cross-file w=0.73 line=0.67] `Source/Core/src/Ui/AnnotateAnalysisUi.cpp:72-81` ⟵ `Source_Core/src/BlameAnalysisUi.cpp:75-84`
  - [diverged cross-file w=0.71 line=0.53] `Source/Core/src/Ui/SmatchetMcpServerUi.cpp:95-112` ⟵ `Source_Core/src/SmatchetMcpServerUi.cpp:93-107`
  - [diverged cross-file w=0.70 line=0.70] `Source/Standalone/StandaloneAppBootstrap.cpp:198-208` ⟵ `Target_Standalone/main.cpp:185-194`
  - [diverged cross-file w=0.70 line=0.69] `Source/Plugins/Mcp/McpPlugin.cpp:790-831` ⟵ `Plugins/Mcp/McpPlugin.cpp:785-819`
  - [diverged cross-file w=0.67 line=0.65] `Source/Core/src/Commands/Builtin/BuiltinCommands_Ai.cpp:339-375` ⟵ `Source_Core/src/Commands/Builtin/BuiltinCommands_Ai.cpp:346-368`
  - [diverged cross-file w=0.62 line=0.67] `Source/Plugins/LuaConsole/LuaConsolePlugin.cpp:55-71` ⟵ `Plugins/LuaConsole/LuaConsolePlugin.cpp:53-63`

## beiklive/GBAStation — 938 коммитов | циклы=0(SCC≤1) | копипаст=10

### `582f2a2f132d` — "v0.1.12     bug修复和优化     1. 修复倍速状态下游戏声音出现杂音的问题     2. 调整着色器设置界面透明度，减少视觉色差     3. 游戏库加载速度优化" (beiklive, 2026-05-22) [10 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/beiklive_GBAStation show 582f2a2f132d`
  - [near within-file w=0.92 line=0.94] `src/ui/utils/GameView.cpp:821-864` ⟵ `src/ui/utils/GameView.cpp:685-728`
  - [near within-file w=0.89 line=0.92] `src/ui/utils/GameView.cpp:322-337` ⟵ `src/ui/utils/GameView.cpp:279-293`
  - [near within-file w=0.88 line=0.86] `src/ui/page/GameDetailPage.cpp:143-180` ⟵ `src/ui/page/GameDetailPage.cpp:181-219`
  - [near within-file w=0.88 line=0.90] `src/core/game_database.cpp:175-185` ⟵ `src/core/game_database.cpp:137-146`
  - [diverged within-file w=0.79 line=0.75] `src/game/render/ShaderCompiler.cpp:243-302` ⟵ `src/game/render/ShaderCompiler.cpp:130-193`
  - [diverged within-file w=0.79 line=0.23] `src/ui/page/GameLibraryPage.cpp:192-206` ⟵ `src/ui/page/GameLibraryPage.cpp:69-93`
  - [diverged within-file w=0.79 line=0.72] `src/ui/utils/GameView.cpp:232-261` ⟵ `src/ui/utils/GameView.cpp:190-218`
  - [diverged within-file w=0.75 line=0.67] `src/ui/page/FileListPage.cpp:555-577` ⟵ `src/ui/page/FileListPage.cpp:630-648`
  - [diverged within-file w=0.65 line=0.68] `src/game/render/GLSLPParser.cpp:322-369` ⟵ `src/game/render/GLSLPParser.cpp:222-255`
  - [diverged within-file w=0.63 line=0.13] `src/ui/page/FileListPage.cpp:596-601` ⟵ `src/ui/page/FileListPage.cpp:660-671`

## cometdom/DirettaRendererUPnP — 893 коммитов | циклы=0(SCC≤1) | копипаст=11

### `1e436caf185b` — "docs(CLAUDE.md): SDK path is auto-detected; flag Windows libs as ambiguous" (cometdom, 2026-05-30) [11 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/cometdom_DirettaRendererUPnP show 1e436caf185b`
  - [diverged within-file w=1.00 line=0.73] `src/DirettaRenderer.cpp:122-133` ⟵ `src/DirettaRenderer.cpp:30-42`
  - [near within-file w=0.97 line=0.84] `src/AudioEngine.cpp:2630-2665` ⟵ `src/AudioEngine.cpp:1316-1351`
  - [diverged within-file w=0.95 line=0.73] `src/DirettaRenderer.cpp:140-151` ⟵ `src/DirettaRenderer.cpp:70-84`
  - [diverged within-file w=0.88 line=0.50] `src/UPnPDevice.cpp:955-961` ⟵ `src/UPnPDevice.cpp:734-741`
  - [near within-file w=0.87 line=0.80] `src/UPnPDevice.cpp:441-453` ⟵ `src/UPnPDevice.cpp:351-363`
  - [diverged within-file w=0.87 line=0.47] `src/AudioEngine.cpp:666-686` ⟵ `src/AudioEngine.cpp:267-282`
  - [near within-file w=0.85 line=0.82] `src/AudioEngine.cpp:2488-2509` ⟵ `src/AudioEngine.cpp:1194-1214`
  - [diverged within-file w=0.75 line=0.68] `src/AudioEngine.cpp:1923-1966` ⟵ `src/AudioEngine.cpp:899-938`
  - [diverged within-file w=0.68 line=0.63] `src/UPnPDevice.cpp:463-508` ⟵ `src/UPnPDevice.cpp:373-409`
  - [diverged within-file w=0.64 line=0.69] `src/UPnPDevice.cpp:293-306` ⟵ `src/UPnPDevice.cpp:268-277`
  - [diverged within-file w=0.62 line=0.36] `src/DirettaRenderer.cpp:973-1020` ⟵ `src/DirettaRenderer.cpp:631-674`

## BALL-Project/ball — 886 коммитов | циклы=2(SCC≤2) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/BALL-Project_ball`

## fernandotonon/QtMeshEditor — 835 коммитов | циклы=0(SCC≤1) | копипаст=32

### `03e6067e9157` — "feat(retopo): triangle-pairing quad retopology (closes #401) (#697)" (Fernando Tonon, 2026-05-29) [32 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/fernandotonon_QtMeshEditor show 03e6067e9157`
  - [verbatim within-file w=1.00 line=1.00] `src/main.cpp:292-299` ⟵ `src/main.cpp:233-240`
  - [near within-file w=1.00 line=0.83] `src/FBX/FBXExporter.cpp:1391-1434` ⟵ `src/FBX/FBXExporter.cpp:1086-1125`
  - [diverged cross-file w=0.98 line=0.78] `src/LLMManager.cpp:435-458` ⟵ `src/SDManager.cpp:324-347`
  - [near within-file w=0.97 line=0.89] `src/PropertiesPanelController_test.cpp:254-282` ⟵ `src/PropertiesPanelController_test.cpp:190-220`
  - [diverged within-file w=0.96 line=0.74] `src/SpaceCamera.cpp:245-269` ⟵ `src/SpaceCamera.cpp:218-241`
  - [near within-file w=0.96 line=0.88] `src/PropertiesPanelController_test.cpp:310-337` ⟵ `src/PropertiesPanelController_test.cpp:250-279`
  - [near within-file w=0.94 line=0.90] `src/mainwindow.cpp:3972-4031` ⟵ `src/mainwindow.cpp:1037-1097`
  - [diverged within-file w=0.94 line=0.46] `src/OgreWidget.cpp:301-339` ⟵ `src/OgreWidget.cpp:64-115`
  - [near within-file w=0.93 line=0.81] `src/mainwindow.cpp:3076-3122` ⟵ `src/mainwindow.cpp:548-585`
  - [diverged within-file w=0.90 line=0.78] `src/LLMSettingsWidget.cpp:428-436` ⟵ `src/LLMSettingsWidget.cpp:407-415`
  - [diverged cross-file w=0.89 line=0.62] `src/ScanEngine.cpp:61-109` ⟵ `src/CLIPipeline.cpp:131-181`
  - [diverged within-file w=0.88 line=0.75] `src/CLIPipeline.cpp:866-927` ⟵ `src/CLIPipeline.cpp:131-181`
  - [diverged within-file w=0.86 line=0.71] `src/FBX/FBXExporter.cpp:949-973` ⟵ `src/FBX/FBXExporter.cpp:743-760`
  - [near within-file w=0.86 line=0.85] `src/LLMManager.cpp:719-765` ⟵ `src/LLMManager.cpp:665-702`
  - [diverged within-file w=0.86 line=0.71] `src/SkeletonDebug.cpp:111-126` ⟵ `src/SkeletonDebug.cpp:77-88`
  - [diverged cross-file w=0.85 line=0.24] `src/SubMeshTransform.cpp:269-300` ⟵ `src/MeshTransform.cpp:53-84`
  - [near within-file w=0.83 line=0.85] `src/MaterialEditorQML.cpp:205-265` ⟵ `src/MaterialEditorQML.cpp:119-169`
  - [near within-file w=0.80 line=0.87] `src/CLIPipeline.cpp:1179-1197` ⟵ `src/CLIPipeline.cpp:350-368`
  - [diverged within-file w=0.78 line=0.73] `src/OgreWidget.cpp:243-297` ⟵ `src/OgreWidget.cpp:149-189`
  - [diverged within-file w=0.78 line=0.36] `src/mainwindow.cpp:4314-4390` ⟵ `src/mainwindow.cpp:1372-1418`
  - [diverged within-file w=0.76 line=0.67] `src/FBX/FBXExporter.cpp:2591-2615` ⟵ `src/FBX/FBXExporter.cpp:1961-1985`
  - [diverged within-file w=0.73 line=0.76] `src/TransformOperator.cpp:2326-2372` ⟵ `src/TransformOperator.cpp:880-910`
  - [diverged within-file w=0.73 line=0.57] `src/mainwindow.cpp:4455-4468` ⟵ `src/mainwindow.cpp:1481-1489`
  - [diverged cross-file w=0.72 line=0.18] `src/Manager.cpp:354-376` ⟵ `src/AnimationMerger.cpp:83-110`
  - [diverged cross-file w=0.71 line=0.06] `src/AnimationControlController_test.cpp:81-90` ⟵ `src/MaterialEditorQML_test.cpp:414-422`
  - [diverged within-file w=0.70 line=0.44] `src/LLMWorker.cpp:191-207` ⟵ `src/LLMWorker.cpp:63-71`
  - [diverged within-file w=0.69 line=0.46] `src/FBX/FBXExporter.cpp:617-636` ⟵ `src/FBX/FBXExporter.cpp:449-473`
  - [diverged within-file w=0.67 line=0.20] `src/SDManager_test.cpp:540-544` ⟵ `src/SDManager_test.cpp:119-125`
  - [diverged within-file w=0.67 line=0.60] `src/AnimationMerger.cpp:939-963` ⟵ `src/AnimationMerger.cpp:245-265`
  - [diverged cross-file w=0.66 line=0.02] `src/CLIPipeline.cpp:205-239` ⟵ `src/MeshImporterExporter.cpp:390-426`
  - [diverged within-file w=0.64 line=0.20] `src/SDManager_test.cpp:532-536` ⟵ `src/SDManager_test.cpp:89-95`
  - [diverged within-file w=0.62 line=0.59] `src/MeshImporterExporter.cpp:978-998` ⟵ `src/MeshImporterExporter.cpp:518-529`

## FiveTechSoft/HarbourBuilder — 808 коммитов | циклы=0(SCC≤1) | копипаст=27

### `d7032804685c` — "bump: version 1.4.4" (Antonio Linares, 2026-05-31) [27 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/FiveTechSoft_HarbourBuilder show d7032804685c`
  - [verbatim within-file w=1.00 line=1.00] `source/backends/gtk3/gtk3_core.c:4952-4992` ⟵ `source/backends/gtk3/gtk3_core.c:4174-4214`
  - [verbatim within-file w=1.00 line=1.00] `source/backends/gtk3/gtk3_core.c:5007-5053` ⟵ `source/backends/gtk3/gtk3_core.c:4229-4275`
  - [near within-file w=1.00 line=0.89] `source/backends/gtk3/gtk3_core.c:7518-7538` ⟵ `source/backends/gtk3/gtk3_core.c:6688-6708`
  - [verbatim within-file w=1.00 line=0.96] `source/backends/gtk3/gtk3_core.c:7573-7630` ⟵ `source/backends/gtk3/gtk3_core.c:6743-6800`
  - [near within-file w=1.00 line=0.93] `source/backends/gtk3/gtk3_inspector.c:1148-1187` ⟵ `source/backends/gtk3/gtk3_inspector.c:650-689`
  - [verbatim within-file w=1.00 line=0.98] `source/cpp/tform.cpp:1030-1126` ⟵ `source/cpp/tform.cpp:879-972`
  - [near within-file w=0.98 line=0.93] `source/backends/gtk3/gtk3_inspector.c:353-394` ⟵ `source/backends/gtk3/gtk3_inspector.c:279-320`
  - [verbatim within-file w=0.98 line=0.97] `source/cpp/hbbridge.cpp:5034-5088` ⟵ `source/cpp/hbbridge.cpp:3776-3829`
  - [near within-file w=0.97 line=0.93] `source/backends/gtk3/gtk3_inspector.c:1516-1555` ⟵ `source/backends/gtk3/gtk3_inspector.c:1018-1057`
  - [near within-file w=0.97 line=0.88] `source/cpp/hbbridge.cpp:3504-3563` ⟵ `source/cpp/hbbridge.cpp:2343-2396`
  - [near within-file w=0.96 line=0.91] `source/backends/gtk3/gtk3_core.c:447-469` ⟵ `source/backends/gtk3/gtk3_core.c:405-425`
  - [near within-file w=0.96 line=0.93] `source/backends/gtk3/gtk3_inspector.c:1060-1097` ⟵ `source/backends/gtk3/gtk3_inspector.c:562-599`
  - [near within-file w=0.94 line=0.91] `source/cpp/tform.cpp:1298-1329` ⟵ `source/cpp/tform.cpp:1103-1127`
  - [near within-file w=0.94 line=0.87] `source/backends/gtk3/gtk3_core.c:2479-2529` ⟵ `source/backends/gtk3/gtk3_core.c:2307-2353`
  - [verbatim within-file w=0.94 line=0.98] `source/cpp/tform.cpp:1783-1854` ⟵ `source/cpp/tform.cpp:1510-1580`
  - [diverged within-file w=0.93 line=0.79] `source/cpp/tcontrols.cpp:2112-2158` ⟵ `source/cpp/tcontrols.cpp:1759-1799`
  - [near within-file w=0.90 line=0.92] `source/cpp/tform.cpp:744-775` ⟵ `source/cpp/tform.cpp:621-652`
  - [near within-file w=0.89 line=0.89] `source/backends/gtk3/gtk3_core.c:2141-2177` ⟵ `source/backends/gtk3/gtk3_core.c:1974-2008`
  - [near within-file w=0.85 line=0.84] `source/cpp/hbbridge.cpp:3396-3438` ⟵ `source/cpp/hbbridge.cpp:2240-2277`
  - [diverged within-file w=0.83 line=0.70] `source/cpp/hbbridge.cpp:5664-5726` ⟵ `source/cpp/hbbridge.cpp:4259-4304`
  - [diverged within-file w=0.81 line=0.05] `source/backends/gtk3/gtk3_core.c:9198-9207` ⟵ `source/backends/gtk3/gtk3_core.c:8623-8633`
  - [near within-file w=0.76 line=0.93] `source/cpp/tform.cpp:1732-1776` ⟵ `source/cpp/tform.cpp:1466-1503`
  - [diverged within-file w=0.73 line=0.62] `source/backends/gtk3/gtk3_core.c:7783-7818` ⟵ `source/backends/gtk3/gtk3_core.c:6897-6923`
  - [diverged within-file w=0.72 line=0.40] `source/backends/gtk3/gtk3_core.c:7637-7686` ⟵ `source/backends/gtk3/gtk3_core.c:6743-6800`
  - [near within-file w=0.71 line=0.87] `source/cpp/tform.cpp:1677-1725` ⟵ `source/cpp/tform.cpp:1418-1459`
  - [diverged within-file w=0.65 line=0.75] `source/cpp/tform.cpp:1426-1478` ⟵ `source/cpp/tform.cpp:1172-1241`
  - [diverged within-file w=0.65 line=0.12] `source/backends/gtk3/gtk3_core.c:13674-13709` ⟵ `source/backends/gtk3/gtk3_core.c:8831-8844`

## Lovuwer/FluffyApp-RE — 769 коммитов | циклы=0(SCC≤1) | копипаст=4

### `f8385dd4caf4` — "Update README.md" (Joe, 2026-02-12) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Lovuwer_FluffyApp-RE show f8385dd4caf4`
  - [diverged cross-file w=0.84 line=0.53] `src/Core/Memory/RegionEnumerator.cpp:60-110` ⟵ `src/Core/Memory/MemoryScanner.cpp:238-276`
  - [diverged cross-file w=0.73 line=0.38] `src/Core/Memory/RegionEnumerator.cpp:129-170` ⟵ `src/Core/Memory/MemoryScanner.cpp:238-276`
  - [diverged cross-file w=0.72 line=0.24] `src/Core/Memory/PatternScanner.cpp:253-296` ⟵ `src/Core/Memory/MemoryScanner.cpp:58-92`
  - [diverged cross-file w=0.66 line=0.25] `src/Core/Memory/PatternScanner.cpp:72-119` ⟵ `src/Core/Memory/MemoryScanner.cpp:58-92`

## b-macker/NAAb — 724 коммитов | циклы=0(SCC≤1) | копипаст=24

### `944e09328fa7` — "feat: agent governance depth — taint propagation, pressure inheritance, agent identity, exposure tracking" (Termux User, 2026-05-31) [24 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/b-macker_NAAb show 944e09328fa7`
  - [verbatim within-file w=1.00 line=1.00] `src/runtime/governance_engine.cpp:4522-4536` ⟵ `src/runtime/governance_engine.cpp:1265-1279`
  - [diverged within-file w=1.00 line=0.78] `src/repl/repl_readline.cpp:360-376` ⟵ `src/repl/repl_readline.cpp:333-349`
  - [verbatim within-file w=1.00 line=1.00] `src/interpreter/interpreter.cpp:2557-2609` ⟵ `src/interpreter/interpreter.cpp:2365-2416`
  - [diverged within-file w=0.98 line=0.51] `src/vm/vm.cpp:4775-4809` ⟵ `src/vm/vm.cpp:3489-3523`
  - [near within-file w=0.98 line=0.88] `src/runtime/governance_checks.cpp:4912-4953` ⟵ `src/runtime/governance_checks.cpp:1911-1950`
  - [verbatim within-file w=0.97 line=0.95] `src/runtime/governance_engine.cpp:653-709` ⟵ `src/runtime/governance_engine.cpp:222-276`
  - [verbatim within-file w=0.96 line=0.96] `src/runtime/governance_reports.cpp:918-971` ⟵ `src/runtime/governance_reports.cpp:750-803`
  - [verbatim within-file w=0.96 line=0.95] `src/api/rest_api.cpp:299-355` ⟵ `src/api/rest_api.cpp:212-268`
  - [near within-file w=0.95 line=0.82] `src/runtime/python_c_executor.cpp:378-418` ⟵ `src/runtime/python_c_executor.cpp:240-280`
  - [diverged within-file w=0.95 line=0.79] `src/runtime/module_resolver.cpp:185-220` ⟵ `src/runtime/module_resolver.cpp:145-175`
  - [near within-file w=0.94 line=0.94] `src/api/rest_api.cpp:243-295` ⟵ `src/api/rest_api.cpp:156-208`
  - [near within-file w=0.94 line=0.85] `src/cli/main.cpp:2154-2193` ⟵ `src/cli/main.cpp:1164-1198`
  - [near within-file w=0.94 line=0.94] `src/api/rest_api.cpp:196-239` ⟵ `src/api/rest_api.cpp:109-152`
  - [near within-file w=0.93 line=0.84] `src/cli/gov_main.cpp:391-445` ⟵ `src/cli/gov_main.cpp:238-292`
  - [near within-file w=0.92 line=0.94] `src/api/rest_api.cpp:554-572` ⟵ `src/api/rest_api.cpp:295-312`
  - [near within-file w=0.88 line=0.90] `src/cli/gov_main.cpp:452-490` ⟵ `src/cli/gov_main.cpp:299-333`
  - [near within-file w=0.86 line=0.91] `src/vm/vm.cpp:3564-3588` ⟵ `src/vm/vm.cpp:2621-2643`
  - [near within-file w=0.86 line=0.90] `src/parser/error_hints.cpp:303-339` ⟵ `src/parser/error_hints.cpp:263-295`
  - [diverged within-file w=0.83 line=0.77] `src/stdlib/env_impl.cpp:181-219` ⟵ `src/stdlib/env_impl.cpp:129-159`
  - [diverged within-file w=0.82 line=0.76] `src/stdlib/file_impl.cpp:325-346` ⟵ `src/stdlib/file_impl.cpp:207-224`
  - [diverged within-file w=0.82 line=0.69] `src/runtime/python_c_executor.cpp:585-623` ⟵ `src/runtime/python_c_executor.cpp:445-475`
  - [diverged within-file w=0.77 line=0.22] `src/cli/main.cpp:1549-1589` ⟵ `src/cli/main.cpp:1312-1361`
  - [diverged within-file w=0.68 line=0.63] `src/cli/main.cpp:2476-2506` ⟵ `src/cli/main.cpp:1312-1361`
  - [diverged within-file w=0.63 line=0.65] `src/stdlib/env_impl.cpp:223-276` ⟵ `src/stdlib/env_impl.cpp:163-198`

## graphillion/kyotodd — 721 коммитов | циклы=1(SCC≤3) | копипаст=19
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/graphillion_kyotodd`

### `ca7523520cd2` — "Address review-BDD-20260501.md: cache/getnode/repr/hash fixes" (Jun Kawahara, 2026-05-01) [19 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/graphillion_kyotodd show ca7523520cd2`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd2.cpp:827-868` ⟵ `test/test_bdd.cpp:2388-2429`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_zdd_ops2.cpp:1752-1773` ⟵ `test/test_bdd.cpp:6437-6458`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_zdd_ops2.cpp:1576-1619` ⟵ `test/test_bdd.cpp:6261-6304`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_zdd_ops2.cpp:499-546` ⟵ `test/test_bdd.cpp:5183-5230`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd_io.cpp:1207-1226` ⟵ `test/test_bdd.cpp:11055-11074`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd_io.cpp:916-924` ⟵ `test/test_bdd.cpp:10774-10782`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd_io.cpp:743-756` ⟵ `test/test_bdd.cpp:10601-10614`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd_io.cpp:307-315` ⟵ `test/test_bdd.cpp:10165-10173`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd_io.cpp:297-303` ⟵ `test/test_bdd.cpp:10155-10161`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd2.cpp:1106-1125` ⟵ `test/test_bdd.cpp:2667-2686`
  - [verbatim cross-file w=1.00 line=1.00] `test/test_bdd2.cpp:308-312` ⟵ `test/test_bdd.cpp:1873-1877`
  - [verbatim cross-file w=1.00 line=1.00] `src/zdd_adv_count.cpp:916-925` ⟵ `src/zdd_adv.cpp:850-859`
  - [verbatim cross-file w=1.00 line=1.00] `src/bdd_io_binary.cpp:372-376` ⟵ `src/bdd_io.cpp:1184-1188`
  - [near within-file w=1.00 line=0.95] `python/src/kyotodd/_binding.cpp:75-94` ⟵ `python/src/kyotodd/_binding.cpp:50-69`
  - [near cross-file w=0.98 line=0.94] `src/bdd_io_graphillion.cpp:140-178` ⟵ `src/bdd_io.cpp:1528-1566`
  - [diverged within-file w=0.92 line=0.67] `include/bdd.h:1467-1490` ⟵ `include/bdd.h:944-963`
  - [diverged cross-file w=0.91 line=0.09] `src/rotpidd.cpp:23-68` ⟵ `src/pidd.cpp:19-64`
  - [diverged cross-file w=0.91 line=0.09] `src/pidd.cpp:22-67` ⟵ `src/rotpidd.cpp:20-65`
  - [diverged within-file w=0.62 line=0.64] `python/src/kyotodd/_binding.cpp:98-110` ⟵ `python/src/kyotodd/_binding.cpp:73-82`

## ayutaz/piper-plus — 719 коммитов | циклы=1(SCC≤3) | копипаст=8
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/ayutaz_piper-plus`

### `4e7a879ee978` — "docs: drop completed/in-progress ticket + proposal directories (#536)" (yousan, 2026-05-21) [8 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ayutaz_piper-plus show 4e7a879ee978`
  - [verbatim within-file w=1.00 line=1.00] `src/cpp/main.cpp:277-285` ⟵ `src/cpp/main.cpp:137-145`
  - [verbatim within-file w=1.00 line=1.00] `src/cpp/main.cpp:303-311` ⟵ `src/cpp/main.cpp:165-173`
  - [verbatim within-file w=1.00 line=1.00] `src/cpp/openjtalk_api.c:234-264` ⟵ `src/cpp/openjtalk_api.c:36-66`
  - [verbatim within-file w=1.00 line=1.00] `src/cpp/openjtalk_api.c:267-276` ⟵ `src/cpp/openjtalk_api.c:69-78`
  - [verbatim within-file w=1.00 line=1.00] `src/cpp/openjtalk_optimized.c:491-532` ⟵ `src/cpp/openjtalk_optimized.c:444-485`
  - [diverged within-file w=1.00 line=0.38] `src/python/piper_train/vits/monotonic_align/core.c:6002-6021` ⟵ `src/python/piper_train/vits/monotonic_align/core.c:6189-6208`
  - [diverged within-file w=1.00 line=0.38] `src/python/piper_train/vits/monotonic_align/core.c:6189-6208` ⟵ `src/python/piper_train/vits/monotonic_align/core.c:6002-6021`
  - [diverged within-file w=0.91 line=0.78] `src/cpp/openjtalk_api.c:279-338` ⟵ `src/cpp/openjtalk_api.c:81-131`

## elisir80/Decodium-4.0-Core-Shannon — 717 коммитов | циклы=0(SCC≤1) | копипаст=124

### `430bf19afff9` — "Release Decodium 1.0.351" (elisir80, 2026-05-31) [124 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/elisir80_Decodium-4.0-Core-Shannon show 430bf19afff9`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19037-19051` ⟵ `DecodiumBridge.cpp:9212-9226`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19700-19744` ⟵ `DecodiumBridge.cpp:9859-9903`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19623-19636` ⟵ `DecodiumBridge.cpp:9782-9795`
  - [diverged within-file w=1.00 line=0.04] `DecodiumBridge.cpp:19601-19615` ⟵ `DecodiumBridge.cpp:9782-9795`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19507-19544` ⟵ `DecodiumBridge.cpp:9666-9703`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19497-19504` ⟵ `DecodiumBridge.cpp:9656-9663`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:34536-34571` ⟵ `DecodiumBridge.cpp:17850-17885`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19066-19072` ⟵ `DecodiumBridge.cpp:9241-9247`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19836-19863` ⟵ `DecodiumBridge.cpp:9995-10022`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:17554-17575` ⟵ `DecodiumBridge.cpp:8679-8700`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:17541-17550` ⟵ `DecodiumBridge.cpp:8666-8675`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:17487-17499` ⟵ `DecodiumBridge.cpp:8621-8633`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:35039-35059` ⟵ `DecodiumBridge.cpp:18197-18217`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:35143-35196` ⟵ `DecodiumBridge.cpp:18251-18304`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:15132-15149` ⟵ `DecodiumBridge.cpp:7337-7354`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:35340-35390` ⟵ `DecodiumBridge.cpp:18448-18498`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:13376-13437` ⟵ `DecodiumBridge.cpp:6540-6601`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:13365-13372` ⟵ `DecodiumBridge.cpp:6529-6536`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:26972-27013` ⟵ `DecodiumBridge.cpp:13640-13681`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:31790-31823` ⟵ `DecodiumBridge.cpp:16215-16248`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:31318-31354` ⟵ `DecodiumBridge.cpp:15985-16021`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:31184-31244` ⟵ `DecodiumBridge.cpp:15884-15944`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:31084-31114` ⟵ `DecodiumBridge.cpp:15784-15814`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:32791-32811` ⟵ `DecodiumBridge.cpp:16994-17014`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:30307-30316` ⟵ `DecodiumBridge.cpp:15380-15389`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:30173-30188` ⟵ `DecodiumBridge.cpp:15269-15284`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:34188-34203` ⟵ `DecodiumBridge.cpp:17566-17581`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:28093-28140` ⟵ `DecodiumBridge.cpp:14201-14248`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:13334-13361` ⟵ `DecodiumBridge.cpp:6498-6525`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:26750-26767` ⟵ `DecodiumBridge.cpp:13518-13535`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:26706-26712` ⟵ `DecodiumBridge.cpp:13474-13480`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:26507-26516` ⟵ `DecodiumBridge.cpp:13275-13284`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:26470-26503` ⟵ `DecodiumBridge.cpp:13238-13271`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:25692-25726` ⟵ `DecodiumBridge.cpp:12756-12790`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:34517-34527` ⟵ `DecodiumBridge.cpp:17831-17841`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:20333-20344` ⟵ `DecodiumBridge.cpp:10431-10442`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:19906-19914` ⟵ `DecodiumBridge.cpp:10065-10073`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:4211-4219` ⟵ `DecodiumBridge.cpp:2044-2052`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:4655-4690` ⟵ `Detector/FtxFt8Stage4.cpp:4325-4360`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:8619-8625` ⟵ `DecodiumBridge.cpp:3808-3814`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:4804-4826` ⟵ `Detector/FtxFt8Stage4.cpp:4474-4496`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:7787-7803` ⟵ `DecodiumBridge.cpp:3228-3244`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:4889-4920` ⟵ `Detector/FtxFt8Stage4.cpp:4559-4590`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxLdpc.cpp:1129-1145` ⟵ `Detector/FtxLdpc.cpp:1057-1073`
  - [diverged within-file w=1.00 line=0.78] `Network/RemoteCommandServer.cpp:3157-3175` ⟵ `Network/RemoteCommandServer.cpp:3263-3281`
  - [verbatim within-file w=1.00 line=1.00] `Transceiver/HamlibTransceiver.cpp:987-1006` ⟵ `Transceiver/HamlibTransceiver.cpp:940-959`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:4265-4307` ⟵ `DecodiumBridge.cpp:2098-2140`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumCatManager.cpp:334-346` ⟵ `DecodiumCatManager.cpp:223-235`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:3715-3759` ⟵ `DecodiumBridge.cpp:1652-1696`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:2706-2740` ⟵ `DecodiumBridge.cpp:1144-1178`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:2397-2433` ⟵ `DecodiumBridge.cpp:835-871`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:2235-2240` ⟵ `DecodiumBridge.cpp:673-678`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:2206-2229` ⟵ `DecodiumBridge.cpp:644-667`
  - [verbatim within-file w=1.00 line=1.00] `main.cpp:369-387` ⟵ `main.cpp:214-232`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:1979-2001` ⟵ `DecodiumBridge.cpp:492-514`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:1745-1770` ⟵ `DecodiumBridge.cpp:373-398`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:4471-4481` ⟵ `Detector/FtxFt8Stage4.cpp:4141-4151`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumCatManager.cpp:869-896` ⟵ `DecodiumCatManager.cpp:711-738`
  - [verbatim within-file w=1.00 line=1.00] `tests/test_qt_helpers.cpp:2706-2722` ⟵ `tests/test_qt_helpers.cpp:2612-2628`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:1958-1961` ⟵ `Detector/FtxFt8Stage4.cpp:1941-1944`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:11476-11496` ⟵ `DecodiumBridge.cpp:5639-5659`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:1964-1979` ⟵ `Detector/FtxFt8Stage4.cpp:1947-1962`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:3569-3600` ⟵ `Detector/FtxFt8Stage4.cpp:3239-3270`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:11310-11314` ⟵ `DecodiumBridge.cpp:5508-5512`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:1144-1164` ⟵ `DecodiumBridge.cpp:134-154`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:11145-11168` ⟵ `DecodiumBridge.cpp:5366-5389`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:3639-3690` ⟵ `Detector/FtxFt8Stage4.cpp:3309-3360`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:3922-3965` ⟵ `Detector/FtxFt8Stage4.cpp:3592-3635`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:10912-10941` ⟵ `DecodiumBridge.cpp:5158-5187`
  - [verbatim within-file w=1.00 line=1.00] `DecodiumBridge.cpp:10827-10844` ⟵ `DecodiumBridge.cpp:5073-5090`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:4327-4336` ⟵ `Detector/FtxFt8Stage4.cpp:3997-4006`
  - [verbatim within-file w=1.00 line=1.00] `Detector/FtxFt8Stage4.cpp:4426-4451` ⟵ `Detector/FtxFt8Stage4.cpp:4096-4121`
  - [near within-file w=1.00 line=0.90] `DecodiumBridge.cpp:34719-34743` ⟵ `DecodiumBridge.cpp:18041-18065`
  - [near within-file w=1.00 line=0.92] `DecodiumBridge.cpp:8729-8775` ⟵ `DecodiumBridge.cpp:3883-3927`
  - [near within-file w=0.98 line=0.91] `Configuration.cpp:4223-4289` ⟵ `Configuration.cpp:4088-4153`
  - [verbatim within-file w=0.98 line=0.98] `DecodiumBridge.cpp:11194-11254` ⟵ `DecodiumBridge.cpp:5415-5474`
  - [diverged within-file w=0.97 line=0.78] `DecodiumDxCluster.cpp:620-680` ⟵ `DecodiumDxCluster.cpp:518-574`
  - [near within-file w=0.96 line=0.83] `widgets/mainwindow.cpp:23618-23634` ⟵ `widgets/mainwindow.cpp:22932-22945`
  - [near within-file w=0.94 line=0.85] `DecodiumBridge.cpp:9000-9040` ⟵ `DecodiumBridge.cpp:4092-4128`
  - [near within-file w=0.93 line=0.80] `DecodiumBridge.cpp:6761-6770` ⟵ `DecodiumBridge.cpp:2463-2472`
  - [near within-file w=0.93 line=0.92] `DecodiumBridge.cpp:19299-19314` ⟵ `DecodiumBridge.cpp:9457-9473`
  - [near within-file w=0.93 line=0.93] `DecodiumBridge.cpp:34950-34976` ⟵ `DecodiumBridge.cpp:18108-18134`
  - [near within-file w=0.93 line=0.86] `DecodiumBridge.cpp:2159-2178` ⟵ `DecodiumBridge.cpp:597-616`
  - [near within-file w=0.92 line=0.89] `DecodiumBridge.cpp:8692-8701` ⟵ `DecodiumBridge.cpp:3847-3855`
  - [near within-file w=0.92 line=0.85] `DecodiumBridge.cpp:10792-10823` ⟵ `DecodiumBridge.cpp:5040-5069`
  - [diverged within-file w=0.92 line=0.66] `DecodiumBridge.cpp:17619-17661` ⟵ `DecodiumBridge.cpp:8711-8749`
  - [diverged within-file w=0.91 line=0.60] `DecodiumBridge.cpp:11825-11835` ⟵ `DecodiumBridge.cpp:5802-5812`
  - [near within-file w=0.89 line=0.81] `DecodiumBridge.cpp:13954-14003` ⟵ `DecodiumBridge.cpp:6974-7020`
  - [near within-file w=0.89 line=0.88] `DecodiumCatManager.cpp:945-984` ⟵ `DecodiumCatManager.cpp:777-814`
  - [diverged within-file w=0.89 line=0.61] `DecodiumBridge.cpp:11069-11096` ⟵ `DecodiumBridge.cpp:5301-5319`
  - [near within-file w=0.88 line=0.90] `DecodiumLegacyBackend.cpp:186-197` ⟵ `DecodiumLegacyBackend.cpp:138-148`
  - [diverged within-file w=0.87 line=0.49] `DecodiumLogging.cpp:350-393` ⟵ `DecodiumLogging.cpp:318-343`
  - [near within-file w=0.86 line=0.86] `DecodiumCatManager.cpp:375-399` ⟵ `DecodiumCatManager.cpp:264-285`
  - [diverged within-file w=0.86 line=0.76] `DecodiumBridge.cpp:16248-16272` ⟵ `DecodiumBridge.cpp:8047-8068`
  - [near within-file w=0.86 line=0.88] `PanadapterItem.cpp:2602-2635` ⟵ `PanadapterItem.cpp:585-616`
  - [diverged within-file w=0.85 line=0.40] `DecodiumBridge.cpp:11452-11472` ⟵ `DecodiumBridge.cpp:5622-5635`
  - [diverged within-file w=0.84 line=0.75] `DecodiumBridge.cpp:12322-12330` ⟵ `DecodiumBridge.cpp:6074-6082`
  - [near within-file w=0.83 line=0.88] `DecodiumDxCluster.cpp:1096-1129` ⟵ `DecodiumDxCluster.cpp:973-1002`
  - [diverged within-file w=0.83 line=0.69] `DecodiumBridge.cpp:11008-11065` ⟵ `DecodiumBridge.cpp:5253-5297`
  - [diverged within-file w=0.82 line=0.76] `DecodiumOmniRigManager.cpp:606-638` ⟵ `DecodiumOmniRigManager.cpp:581-605`
  - [diverged within-file w=0.82 line=0.76] `DecodiumOmniRigManager.cpp:774-806` ⟵ `DecodiumOmniRigManager.cpp:581-605`
  - [diverged within-file w=0.82 line=0.56] `Transceiver/HRDTransceiver.cpp:1145-1216` ⟵ `Transceiver/HRDTransceiver.cpp:534-578`
  - [diverged within-file w=0.80 line=0.73] `DecodiumBridge.cpp:9687-9723` ⟵ `DecodiumBridge.cpp:4410-4439`
  - [diverged within-file w=0.80 line=0.75] `DecodiumBridge.cpp:28661-28704` ⟵ `DecodiumBridge.cpp:14480-14510`
  - [near within-file w=0.80 line=0.81] `DecodiumBridge.cpp:32318-32382` ⟵ `DecodiumBridge.cpp:16600-16647`
  - [diverged within-file w=0.80 line=0.55] `DecodiumTransceiverManager.cpp:2067-2074` ⟵ `DecodiumTransceiverManager.cpp:1191-1199`
  - [diverged within-file w=0.79 line=0.46] `DecodiumBridge.cpp:12538-12550` ⟵ `DecodiumBridge.cpp:6102-6108`
  - [near within-file w=0.78 line=0.85] `DecodiumTransceiverManager.cpp:622-647` ⟵ `DecodiumTransceiverManager.cpp:335-355`
  - [near within-file w=0.78 line=0.83] `DecodiumCatManager.cpp:135-157` ⟵ `DecodiumCatManager.cpp:110-127`
  - [near within-file w=0.78 line=0.85] `DecodiumBridge.cpp:11389-11404` ⟵ `DecodiumBridge.cpp:5584-5597`
  - [diverged cross-file w=0.77 line=0.08] `tests/test_ftx_weak_decode.cpp:740-750` ⟵ `tests/test_tx_pipeline.cpp:379-382`
  - [diverged within-file w=0.74 line=0.68] `DecodiumBridge.cpp:7963-7995` ⟵ `DecodiumBridge.cpp:3379-3399`
  - [diverged within-file w=0.73 line=0.50] `DecodiumBridge.cpp:7394-7403` ⟵ `DecodiumBridge.cpp:2960-2967`
  - [diverged within-file w=0.71 line=0.06] `DecodiumAlertManager.cpp:60-72` ⟵ `DecodiumAlertManager.cpp:24-30`
  - [diverged within-file w=0.70 line=0.44] `DecodiumBridge.cpp:7732-7773` ⟵ `DecodiumBridge.cpp:3187-3214`
  - [diverged within-file w=0.70 line=0.75] `DecodiumAlertManager.cpp:153-170` ⟵ `DecodiumAlertManager.cpp:73-83`
  - [diverged within-file w=0.69 line=0.74] `DecodiumBridge.cpp:16019-16061` ⟵ `DecodiumBridge.cpp:7846-7875`
  - [diverged within-file w=0.69 line=0.45] `widgets/mainwindow.cpp:16549-16561` ⟵ `widgets/mainwindow.cpp:16138-16143`
  - [diverged within-file w=0.68 line=0.69] `DecodiumBridge.cpp:24645-24657` ⟵ `DecodiumBridge.cpp:12214-12222`
  - [diverged within-file w=0.67 line=0.73] `DecodiumBridge.cpp:12554-12591` ⟵ `DecodiumBridge.cpp:6112-6140`
  - [diverged within-file w=0.65 line=0.60] `DecodiumTransceiverManager.cpp:1144-1186` ⟵ `DecodiumTransceiverManager.cpp:622-646`
  - [diverged within-file w=0.64 line=0.60] `DecodiumLogging.cpp:404-434` ⟵ `DecodiumLogging.cpp:354-371`
  - [diverged within-file w=0.63 line=0.71] `widgets/mainwindow.cpp:7950-8029` ⟵ `widgets/mainwindow.cpp:7841-7891`
  - [diverged within-file w=0.61 line=0.53] `DecodiumBridge.cpp:30423-30493` ⟵ `DecodiumBridge.cpp:15468-15509`

## brooksthemaker/ProtoHUD — 698 коммитов | циклы=0(SCC≤1) | копипаст=9

### `cddf42c90761` — "Merge pull request #205 from brooksthemaker/claude/setup-docs-and-hardware-options" (SerinSong066, 2026-05-31) [9 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/brooksthemaker_ProtoHUD show cddf42c90761`
  - [verbatim within-file w=1.00 line=1.00] `src/main.cpp:9779-9785` ⟵ `src/main.cpp:611-617`
  - [diverged within-file w=0.95 line=0.59] `src/camera/dma_camera.cpp:590-600` ⟵ `src/camera/dma_camera.cpp:455-470`
  - [diverged within-file w=0.91 line=0.79] `src/camera/dma_camera.cpp:477-504` ⟵ `src/camera/dma_camera.cpp:351-379`
  - [diverged within-file w=0.91 line=0.64] `src/main.cpp:152-167` ⟵ `src/main.cpp:33-45`
  - [diverged within-file w=0.90 line=0.63] `src/vitrue/xr_display.cpp:298-319` ⟵ `src/vitrue/xr_display.cpp:186-202`
  - [near within-file w=0.84 line=0.85] `src/hud/hud_renderer.cpp:215-262` ⟵ `src/hud/hud_renderer.cpp:41-80`
  - [diverged within-file w=0.64 line=0.33] `src/vitrue/xr_display.cpp:323-365` ⟵ `src/vitrue/xr_display.cpp:206-227`
  - [diverged within-file w=0.63 line=0.18] `src/camera/dma_camera.cpp:221-271` ⟵ `src/camera/dma_camera.cpp:145-199`
  - [diverged within-file w=0.60 line=0.26] `src/main.cpp:11744-11761` ⟵ `src/main.cpp:784-800`

## esrrhs/fakelua — 681 коммитов | циклы=0(SCC≤1) | копипаст=2

### `8c2fe347eca4` — "Merge pull request #146 from esrrhs/refactor-file-level-types" (esrrhs, 2026-05-31) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/esrrhs_fakelua show 8c2fe347eca4`
  - [diverged cross-file w=0.87 line=0.78] `src/compile/my_flexer.cpp:10-18` ⟵ `src/compile/myflexer.cpp:15-23`
  - [diverged cross-file w=0.70 line=0.71] `src/compile/my_flexer.cpp:22-27` ⟵ `src/compile/myflexer.cpp:26-31`

## xadupre/onnx-light — 679 коммитов | циклы=0(SCC≤1) | копипаст=2

### `8968b9d3768d` — "backend_test: make DataType an alias for TensorProto::DataType (#1412)" (Copilot, 2026-05-31) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/xadupre_onnx-light show 8968b9d3768d`
  - [diverged within-file w=0.91 line=0.75] `onnx_light/onnx_proto/thread_pool.cc:35-42` ⟵ `onnx_light/onnx_proto/thread_pool.cc:25-32`
  - [near within-file w=0.89 line=0.84] `unittests/cc_onnx_op/test_onnx_op_traditionalml.cc:112-136` ⟵ `unittests/cc_onnx_op/test_onnx_op_traditionalml.cc:66-88`

## bobjansen/Ibex — 634 коммитов | циклы=0(SCC≤1) | копипаст=25

### `b7bd573aaaf8` — "Add plugin hint" (Bob Jansen, 2026-05-31) [25 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/bobjansen_Ibex show b7bd573aaaf8`
  - [near within-file w=1.00 line=0.92] `tests/test_e2e.cpp:1306-1366` ⟵ `tests/test_e2e.cpp:1078-1138`
  - [diverged within-file w=1.00 line=0.73] `src/runtime/interpreter.cpp:1438-1463` ⟵ `src/runtime/interpreter.cpp:1215-1240`
  - [near within-file w=1.00 line=0.86] `src/runtime/interpreter.cpp:1303-1315` ⟵ `src/runtime/interpreter.cpp:1187-1199`
  - [verbatim within-file w=1.00 line=1.00] `src/repl/repl.cpp:1330-1344` ⟵ `src/repl/repl.cpp:801-815`
  - [near within-file w=1.00 line=0.82] `src/interop/arrow_c_data.cpp:522-531` ⟵ `src/interop/arrow_c_data.cpp:213-222`
  - [near within-file w=1.00 line=0.89] `src/repl/repl.cpp:2003-2026` ⟵ `src/repl/repl.cpp:1062-1085`
  - [near within-file w=1.00 line=0.86] `src/repl/repl.cpp:1981-2001` ⟵ `src/repl/repl.cpp:1040-1060`
  - [near within-file w=1.00 line=0.88] `src/repl/repl.cpp:3141-3157` ⟵ `src/repl/repl.cpp:2006-2022`
  - [near within-file w=1.00 line=0.86] `src/repl/repl.cpp:2405-2422` ⟵ `src/repl/repl.cpp:1461-1478`
  - [near within-file w=1.00 line=0.87] `src/repl/repl.cpp:2206-2223` ⟵ `src/repl/repl.cpp:1264-1281`
  - [near within-file w=0.99 line=0.87] `src/repl/repl.cpp:2313-2330` ⟵ `src/repl/repl.cpp:1365-1382`
  - [diverged within-file w=0.96 line=0.50] `src/repl/repl.cpp:3473-3517` ⟵ `src/repl/repl.cpp:2209-2251`
  - [near within-file w=0.96 line=0.86] `src/runtime/model.cpp:625-664` ⟵ `src/runtime/model.cpp:511-548`
  - [near within-file w=0.95 line=0.93] `include/zorro.hpp:1307-1334` ⟵ `include/zorro.hpp:1181-1208`
  - [diverged within-file w=0.94 line=0.50] `src/runtime/interpreter.cpp:1715-1723` ⟵ `src/runtime/interpreter.cpp:1498-1505`
  - [near within-file w=0.89 line=0.85] `tests/test_arrow_c_data.cpp:151-182` ⟵ `tests/test_arrow_c_data.cpp:115-146`
  - [near within-file w=0.87 line=0.91] `src/parser/lower.cpp:1643-1666` ⟵ `src/parser/lower.cpp:565-588`
  - [diverged within-file w=0.86 line=0.14] `src/repl/repl.cpp:309-313` ⟵ `src/repl/repl.cpp:59-63`
  - [diverged within-file w=0.84 line=0.79] `src/parser/lower.cpp:2754-2773` ⟵ `src/parser/lower.cpp:1373-1392`
  - [diverged within-file w=0.83 line=0.78] `include/zorro.hpp:1215-1222` ⟵ `include/zorro.hpp:1089-1096`
  - [diverged within-file w=0.78 line=0.70] `src/repl/repl.cpp:2942-2984` ⟵ `src/repl/repl.cpp:1848-1882`
  - [near within-file w=0.77 line=0.94] `src/runtime/interpreter.cpp:2126-2160` ⟵ `src/runtime/interpreter.cpp:1708-1740`
  - [diverged within-file w=0.76 line=0.75] `include/zorro.hpp:1483-1489` ⟵ `include/zorro.hpp:1357-1363`
  - [diverged within-file w=0.74 line=0.45] `src/runtime/interpreter.cpp:7011-7028` ⟵ `src/runtime/interpreter.cpp:5773-5785`
  - [diverged within-file w=0.74 line=0.45] `src/runtime/interpreter.cpp:7367-7384` ⟵ `src/runtime/interpreter.cpp:5773-5785`

## adecarolis/wfweb — 627 коммитов | циклы=0(SCC≤1) | копипаст=7

### `d57de2b0f199` — "fix(standalone): prime mic permission in-gesture so RX audio starts first try" (Alain De Carolis, 2026-05-24) [7 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/adecarolis_wfweb show d57de2b0f199`
  - [near within-file w=1.00 line=0.93] `src/main.cpp:338-374` ⟵ `src/main.cpp:228-264`
  - [near within-file w=0.96 line=0.83] `src/servermain.cpp:337-369` ⟵ `src/servermain.cpp:291-323`
  - [near within-file w=0.92 line=0.91] `src/webserver.cpp:5593-5620` ⟵ `src/webserver.cpp:2250-2275`
  - [diverged within-file w=0.89 line=0.56] `src/main.cpp:94-108` ⟵ `src/main.cpp:47-61`
  - [diverged within-file w=0.80 line=0.69] `src/servermain.cpp:193-241` ⟵ `src/servermain.cpp:147-185`
  - [diverged within-file w=0.72 line=0.64] `src/webserver.cpp:2616-2634` ⟵ `src/webserver.cpp:1552-1563`
  - [near within-file w=0.67 line=0.80] `src/servermain.cpp:1029-1044` ⟵ `src/servermain.cpp:934-946`

## Aiacos/ajazz-control-center — 622 коммитов | циклы=0(SCC≤1) | копипаст=4

### `cebcd54e4e9d` — "Merge pull request #73 from Aiacos/docs/release-and-workflow" (Lorenzo Argentieri, 2026-05-25) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Aiacos_ajazz-control-center show cebcd54e4e9d`
  - [near within-file w=1.00 line=0.87] `src/app/src/application.cpp:233-249` ⟵ `src/app/src/application.cpp:119-135`
  - [diverged within-file w=0.99 line=0.70] `src/core/src/hid_transport.cpp:370-378` ⟵ `src/core/src/hid_transport.cpp:265-272`
  - [diverged within-file w=0.79 line=0.60] `src/app/src/time_sync_service.cpp:156-166` ⟵ `src/app/src/time_sync_service.cpp:79-85`
  - [diverged within-file w=0.70 line=0.33] `tests/unit/qt_app_fixture.hpp:91-113` ⟵ `tests/unit/qt_app_fixture.hpp:61-71`

## HendrikGC02/Astroray — 617 коммитов | циклы=1(SCC≤3) | копипаст=14
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/HendrikGC02_Astroray`

### `db73d2cb5d98` — "docs: round 15 wave 6 closeout — pkg104/pkg117 complete, pkg118 filed (#412)" (Hendrik Combrinck, 2026-05-31) [14 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/HendrikGC02_Astroray show db73d2cb5d98`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:834-872` ⟵ `include/astroray/gpu_materials.h:680-718`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:878-888` ⟵ `include/astroray/gpu_materials.h:724-734`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:894-903` ⟵ `include/astroray/gpu_materials.h:740-749`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:908-916` ⟵ `include/astroray/gpu_materials.h:754-762`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:976-982` ⟵ `include/astroray/gpu_materials.h:822-828`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:1008-1018` ⟵ `include/astroray/gpu_materials.h:854-864`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:1031-1040` ⟵ `include/astroray/gpu_materials.h:877-886`
  - [verbatim within-file w=1.00 line=1.00] `include/astroray/gpu_materials.h:1073-1080` ⟵ `include/astroray/gpu_materials.h:900-907`
  - [diverged within-file w=0.97 line=0.62] `plugins/integrators/restir_di.cpp:200-206` ⟵ `plugins/integrators/restir_di.cpp:174-179`
  - [near within-file w=0.92 line=0.85] `include/raytracer.h:1348-1377` ⟵ `include/raytracer.h:1227-1256`
  - [diverged within-file w=0.88 line=0.67] `include/astroray/shapes.h:280-311` ⟵ `include/astroray/shapes.h:229-259`
  - [near within-file w=0.77 line=0.85] `module/blender_module.cpp:1843-1855` ⟵ `module/blender_module.cpp:914-924`
  - [diverged within-file w=0.67 line=0.34] `plugins/materials/disney.cpp:426-460` ⟵ `plugins/materials/disney.cpp:309-332`
  - [diverged within-file w=0.66 line=0.27] `include/astroray/gpu_materials.h:710-740` ⟵ `include/astroray/gpu_materials.h:567-586`

## brontoguana/krasis — 599 коммитов | циклы=10(SCC≤13) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 10 include-циклов, крупнейший SCC=13. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/brontoguana_krasis`

## csgaokun/vscode_claude — 571 коммитов | циклы=1(SCC≤2) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/csgaokun_vscode_claude`

## fenrus75/powertop — 556 коммитов | циклы=0(SCC≤1) | копипаст=23

### `4f8f6d19f5e5` — "refactor(lib.cpp): reorganize and deduplicate include headers" (Arjan van de Ven, 2026-05-31) [23 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/fenrus75_powertop show 4f8f6d19f5e5`
  - [verbatim within-file w=0.99 line=0.96] `src/lib.cpp:456-520` ⟵ `src/lib.cpp:349-413`
  - [near within-file w=0.97 line=0.88] `src/cpu/cpu.cpp:924-941` ⟵ `src/cpu/cpu.cpp:894-911`
  - [diverged within-file w=0.97 line=0.78] `src/lib.cpp:416-426` ⟵ `src/lib.cpp:311-321`
  - [verbatim within-file w=0.96 line=0.96] `src/process/do_process.cpp:692-720` ⟵ `src/process/do_process.cpp:663-690`
  - [near within-file w=0.91 line=0.94] `src/process/do_process.cpp:1239-1257` ⟵ `src/process/do_process.cpp:1197-1214`
  - [near cross-file w=0.90 line=0.92] `src/devices/device_manager.cpp:271-282` ⟵ `src/devices/device.cpp:312-322`
  - [near within-file w=0.89 line=0.92] `src/display.cpp:285-303` ⟵ `src/display.cpp:191-208`
  - [near within-file w=0.88 line=0.92] `src/display.cpp:307-326` ⟵ `src/display.cpp:212-230`
  - [near within-file w=0.87 line=0.93] `src/lib.cpp:593-632` ⟵ `src/lib.cpp:502-541`
  - [near within-file w=0.84 line=0.90] `src/display.cpp:427-440` ⟵ `src/display.cpp:322-334`
  - [diverged within-file w=0.84 line=0.46] `src/process/do_process.cpp:644-664` ⟵ `src/process/do_process.cpp:606-635`
  - [diverged within-file w=0.84 line=0.62] `src/tuning/tuning.cpp:97-113` ⟵ `src/tuning/tuning.cpp:77-90`
  - [diverged within-file w=0.78 line=0.79] `src/display.cpp:74-91` ⟵ `src/display.cpp:58-73`
  - [diverged within-file w=0.76 line=0.43] `src/tuning/tuning.cpp:171-178` ⟵ `src/tuning/tuning.cpp:146-157`
  - [diverged cross-file w=0.72 line=0.53] `src/measurement/measurement_manager.cpp:100-118` ⟵ `src/measurement/measurement.cpp:117-134`
  - [near within-file w=0.65 line=0.80] `src/lib.cpp:580-589` ⟵ `src/lib.cpp:489-498`
  - [diverged within-file w=0.63 line=0.55] `src/cpu/abstract_cpu.cpp:356-374` ⟵ `src/cpu/abstract_cpu.cpp:405-424`
  - [diverged within-file w=0.63 line=0.33] `src/perf/perf_bundle.cpp:218-233` ⟵ `src/perf/perf_bundle.cpp:253-278`
  - [diverged within-file w=0.62 line=0.59] `src/test_framework.cpp:266-301` ⟵ `src/test_framework.cpp:116-137`
  - [diverged within-file w=0.62 line=0.32] `src/process/do_process.cpp:880-917` ⟵ `src/process/do_process.cpp:1083-1128`
  - [diverged within-file w=0.62 line=0.32] `src/process/do_process.cpp:1119-1164` ⟵ `src/process/do_process.cpp:852-889`
  - [diverged cross-file w=0.61 line=0.54] `src/cpu/cpu_rapl_device.cpp:35-45` ⟵ `src/cpu/dram_rapl_device.cpp:35-45`
  - [diverged cross-file w=0.61 line=0.54] `src/cpu/dram_rapl_device.cpp:36-46` ⟵ `src/cpu/cpu_rapl_device.cpp:34-44`

## brkronheim/RDFAnalyzerCore — 554 коммитов | циклы=0(SCC≤1) | копипаст=2

### `12ea493cc71b` — "Merge pull request #171 from brkronheim/dev" (Braden Kronheim, 2026-05-03) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/brkronheim_RDFAnalyzerCore show 12ea493cc71b`
  - [diverged within-file w=1.00 line=0.21] `core/interface/functions.h:161-170` ⟵ `core/interface/functions.h:44-54`
  - [diverged within-file w=1.00 line=0.51] `core/interface/plots.h:1171-1205` ⟵ `core/interface/plots.h:236-273`

## cschladetsch/CppKAI — 545 коммитов | циклы=2(SCC≤4) | копипаст=22
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=4. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/cschladetsch_CppKAI`

### `61d4a087d5e0` — "Normalise Rho syntax." (Christian Schladetsch, 2026-05-31) [22 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/cschladetsch_CppKAI show 61d4a087d5e0`
  - [diverged cross-file w=1.00 line=0.47] `Source/Library/Language/Hlsl/Source/HlslParser.cpp:434-456` ⟵ `Source/Library/Language/Rho/Source/RhoParser.cpp:453-479`
  - [near cross-file w=1.00 line=0.92] `Test/Source/TestEvents.cpp:54-99` ⟵ `Source/Test/Source/TestEvents.cpp:83-129`
  - [diverged within-file w=1.00 line=0.47] `Source/Library/Language/Rho/Source/RhoParser.cpp:858-880` ⟵ `Source/Library/Language/Rho/Source/RhoParser.cpp:453-479`
  - [diverged cross-file w=1.00 line=0.47] `Source/Library/Language/Lisp/Source/LispParser.cpp:346-368` ⟵ `Source/Library/Language/Rho/Source/RhoParser.cpp:453-479`
  - [diverged within-file w=1.00 line=0.27] `Include/KAI/Core/Object/Class.h:80-88` ⟵ `Include/KAI/Core/Object/Class.h:93-100`
  - [diverged within-file w=1.00 line=0.57] `Source/Library/Executor/Source/Translator/LexerBase.cpp:63-92` ⟵ `Source/Library/Executor/Source/Translator/LexerBase.cpp:96-130`
  - [diverged within-file w=1.00 line=0.38] `Source/Library/Executor/Source/Translator/LexerBase.cpp:51-60` ⟵ `Source/Library/Executor/Source/Translator/LexerBase.cpp:72-84`
  - [verbatim within-file w=1.00 line=1.00] `Source/Library/Core/Source/DotGraph.cpp:103-105` ⟵ `Source/Library/Core/Source/DotGraph.cpp:111-113`
  - [diverged cross-file w=0.99 line=0.23] `Source/App/Window/Include/ExecutorWindow.h:56-149` ⟵ `Source/App/Window/Source/ExecutorWindow.cpp:63-156`
  - [diverged within-file w=0.99 line=0.44] `Source/Library/Core/Source/Registry.cpp:525-549` ⟵ `Source/Library/Core/Source/Registry.cpp:556-583`
  - [diverged cross-file w=0.96 line=0.36] `Test/Source/TestClassScripting.cpp:13-67` ⟵ `Source/Test/Source/TestClassScripting.cpp:14-69`
  - [diverged cross-file w=0.95 line=0.71] `Source/Library/Language/Hlsl/Source/HlslTranslator.cpp:187-249` ⟵ `Source/Library/Language/Rho/Source/RhoTranslator.cpp:163-229`
  - [diverged cross-file w=0.95 line=0.71] `Source/Library/Language/Lisp/Source/LispTranslator.cpp:187-249` ⟵ `Source/Library/Language/Rho/Source/RhoTranslator.cpp:163-229`
  - [diverged cross-file w=0.90 line=0.60] `Source/Library/Language/Tau/Source/Generate/GenerateProxy.cpp:241-251` ⟵ `Source/Library/Language/Tau/Source/Generate/Proxy.cpp:74-84`
  - [diverged cross-file w=0.87 line=0.50] `Source/App/Window/Source/imgui_impl_opengl3.cpp:368-398` ⟵ `Source/Library/Imgui/Source/imgui_impl_glfw_gl3.cpp:178-199`
  - [diverged cross-file w=0.86 line=0.32] `Source/Library/Language/Lisp/Source/LispParser.cpp:133-191` ⟵ `Source/Library/Language/Rho/Source/RhoParser.cpp:149-215`
  - [diverged cross-file w=0.73 line=0.18] `Source/App/Window/Source/imgui_impl_opengl3.cpp:278-326` ⟵ `Source/Library/Imgui/Source/imgui_impl_glfw_gl3.cpp:89-113`
  - [diverged cross-file w=0.70 line=0.24] `Test/Source/TestContainer/TestList.cpp:44-64` ⟵ `Source/Test/Source/TestContainer/TestList.cpp:51-70`
  - [diverged within-file w=0.66 line=0.11] `Source/Library/Language/Tau/Source/Generate/GenerateProcess.cpp:54-60` ⟵ `Source/Library/Language/Tau/Source/Generate/GenerateProcess.cpp:18-22`
  - [diverged cross-file w=0.65 line=0.38] `Source/App/Window/Source/imgui_impl_opengl3.cpp:619-637` ⟵ `Source/Library/Imgui/Source/imgui_impl_glfw_gl3.cpp:280-302`
  - [diverged within-file w=0.63 line=0.13] `Source/Library/Core/Source/Registry.cpp:441-450` ⟵ `Source/Library/Core/Source/Registry.cpp:459-469`
  - [diverged cross-file w=0.62 line=0.22] `Source/Library/Language/Hlsl/Source/HlslParser.cpp:143-206` ⟵ `Source/Library/Language/Rho/Source/RhoParser.cpp:149-215`

## FiveTechSoft/OpenADS — 538 коммитов | циклы=0(SCC≤1) | копипаст=20

### `7c31315a6de0` — "docs(da-web): add GUIDE.md — full user guide for DA-Web" (Reinaldo Crespo-Bazán, 2026-05-30) [20 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/FiveTechSoft_OpenADS show 7c31315a6de0`
  - [near within-file w=1.00 line=0.80] `src/abi/ace_exports.cpp:2868-2877` ⟵ `src/abi/ace_exports.cpp:1561-1570`
  - [near within-file w=1.00 line=0.90] `src/engine/table.cpp:694-717` ⟵ `src/engine/table.cpp:476-499`
  - [near within-file w=1.00 line=0.83] `src/engine/table.cpp:158-171` ⟵ `src/engine/table.cpp:126-139`
  - [verbatim within-file w=1.00 line=1.00] `src/abi/ace_exports.cpp:6484-6497` ⟵ `src/abi/ace_exports.cpp:3651-3664`
  - [verbatim within-file w=1.00 line=1.00] `src/abi/ace_exports.cpp:6460-6478` ⟵ `src/abi/ace_exports.cpp:3627-3645`
  - [verbatim within-file w=1.00 line=1.00] `src/abi/ace_exports.cpp:6207-6232` ⟵ `src/abi/ace_exports.cpp:3382-3407`
  - [verbatim within-file w=1.00 line=1.00] `src/abi/ace_exports.cpp:6097-6105` ⟵ `src/abi/ace_exports.cpp:3279-3287`
  - [verbatim within-file w=1.00 line=1.00] `src/abi/ace_exports.cpp:5620-5625` ⟵ `src/abi/ace_exports.cpp:2923-2928`
  - [verbatim within-file w=1.00 line=1.00] `src/abi/ace_exports.cpp:5201-5229` ⟵ `src/abi/ace_exports.cpp:2733-2761`
  - [near within-file w=0.98 line=0.90] `src/engine/table.cpp:556-589` ⟵ `src/engine/table.cpp:339-371`
  - [near within-file w=0.95 line=0.92] `src/abi/ace_exports.cpp:6406-6421` ⟵ `src/abi/ace_exports.cpp:3574-3588`
  - [diverged within-file w=0.94 line=0.67] `src/engine/index_expr.cpp:410-441` ⟵ `src/engine/index_expr.cpp:369-399`
  - [near within-file w=0.93 line=0.94] `src/network/socket_win32.cpp:116-135` ⟵ `src/network/socket_win32.cpp:88-106`
  - [diverged within-file w=0.91 line=0.74] `src/abi/ace_exports.cpp:2301-2345` ⟵ `src/abi/ace_exports.cpp:1176-1209`
  - [near within-file w=0.87 line=0.84] `src/network/server.cpp:261-280` ⟵ `src/network/server.cpp:63-79`
  - [diverged within-file w=0.78 line=0.56] `src/network/server.cpp:304-325` ⟵ `src/network/server.cpp:95-107`
  - [near within-file w=0.76 line=0.87] `src/abi/ace_exports.cpp:4074-4104` ⟵ `src/abi/ace_exports.cpp:2446-2470`
  - [near within-file w=0.75 line=0.84] `src/abi/ace_exports.cpp:3431-3454` ⟵ `src/abi/ace_exports.cpp:2033-2051`
  - [near within-file w=0.71 line=0.81] `src/abi/ace_exports.cpp:2887-2909` ⟵ `src/abi/ace_exports.cpp:1580-1598`
  - [diverged within-file w=0.62 line=0.37] `src/engine/table.cpp:393-451` ⟵ `src/engine/table.cpp:251-274`

## CTalkobt/M65Compiler — 534 коммитов | циклы=0(SCC≤1) | копипаст=5

### `329dd50cc36c` — "Fix #100: Add signed/unsigned variants for MUL and fix right shift operators" (Craig Taylor, 2026-05-31) [5 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/CTalkobt_M65Compiler show 329dd50cc36c`
  - [near within-file w=1.00 line=0.85] `src/main/Lexer.cpp:294-332` ⟵ `src/main/Lexer.cpp:239-277`
  - [diverged within-file w=0.91 line=0.60] `src/main/AssemblerParser.cpp:890-905` ⟵ `src/main/AssemblerParser.cpp:821-836`
  - [near within-file w=0.88 line=0.86] `src/main/objdump45_main.cpp:868-881` ⟵ `src/main/objdump45_main.cpp:698-710`
  - [diverged within-file w=0.79 line=0.74] `src/main/O45Linker.cpp:583-637` ⟵ `src/main/O45Linker.cpp:424-466`
  - [diverged within-file w=0.78 line=0.73] `src/main/ca45_main.cpp:238-249` ⟵ `src/main/ca45_main.cpp:155-162`

## starseeker/obol — 525 коммитов | циклы=4(SCC≤6) | копипаст=94
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 4 include-циклов, крупнейший SCC=6. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/starseeker_obol`

### `bbe9f6aeeccf` — "Update osmesa" (Clifford Yapp, 2026-05-09) [94 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/starseeker_obol show bbe9f6aeeccf`
  - [diverged cross-file w=1.00 line=0.08] `src/misc/SoDBOSMesa.cpp:61-72` ⟵ `tests/utils/headless_utils.h:111-127`
  - [near cross-file w=1.00 line=0.83] `src/profiler/SoNodeVisualize.cpp:609-640` ⟵ `upstream/src/profiler/SoNodeVisualize.cpp:611-642`
  - [diverged cross-file w=1.00 line=0.55] `src/profiler/SoNodeVisualize.cpp:416-437` ⟵ `upstream/src/profiler/SoNodeVisualize.cpp:418-439`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:5992-6064` ⟵ `Mentor/stb_image.h:6422-6494`
  - [diverged cross-file w=1.00 line=0.68] `src/nodes/SoDepthBuffer.cpp:193-219` ⟵ `upstream/src/nodes/SoDepthBuffer.cpp:193-219`
  - [near cross-file w=1.00 line=0.90] `src/nodekits/SoBaseKit.cpp:853-920` ⟵ `upstream/src/nodekits/SoBaseKit.cpp:859-926`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:7112-7150` ⟵ `Mentor/stb_image.h:7634-7672`
  - [near cross-file w=1.00 line=0.87] `src/misc/SoPath.cpp:537-591` ⟵ `upstream/src/misc/SoPath.cpp:537-591`
  - [near cross-file w=1.00 line=0.81] `src/actions/SoAction.cpp:370-388` ⟵ `upstream/src/actions/SoAction.cpp:372-391`
  - [verbatim cross-file w=1.00 line=1.00] `src/actions/SoAction.cpp:571-578` ⟵ `upstream/src/actions/SoAction.cpp:586-593`
  - [verbatim cross-file w=1.00 line=1.00] `src/actions/SoActionP.cpp:60-78` ⟵ `upstream/src/actions/SoActionP.cpp:60-78`
  - [verbatim cross-file w=1.00 line=1.00] `src/actions/SoGLRenderAction.cpp:1135-1158` ⟵ `upstream/src/actions/SoGLRenderAction.cpp:1129-1153`
  - [near within-file w=1.00 line=0.88] `src/actions/SoGLRenderAction.cpp:1358-1400` ⟵ `src/actions/SoGLRenderAction.cpp:1300-1342`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:4858-4863` ⟵ `tests/utils/stb_image.h:5216-5221`
  - [near cross-file w=1.00 line=0.85] `src/misc/SoDB.cpp:1524-1558` ⟵ `upstream/src/misc/SoDB.cpp:1652-1686`
  - [diverged cross-file w=1.00 line=0.70] `src/misc/CoinTidbits.cpp:597-631` ⟵ `src/C/CoinTidbits.cpp:603-637`
  - [near cross-file w=1.00 line=0.87] `src/misc/CoinResources.cpp:291-309` ⟵ `upstream/src/misc/CoinResources.cpp:296-314`
  - [near cross-file w=1.00 line=0.90] `src/io/SoOutput.cpp:1504-1558` ⟵ `upstream/src/io/SoOutput.cpp:1516-1570`
  - [near within-file w=1.00 line=0.87] `src/io/SoInput.cpp:1716-1753` ⟵ `src/io/SoInput.cpp:1759-1796`
  - [near cross-file w=1.00 line=0.94] `src/fields/SoField.cpp:846-942` ⟵ `upstream/src/fields/SoField.cpp:933-1029`
  - [diverged cross-file w=1.00 line=0.69] `src/engines/SoCalculator.cpp:414-427` ⟵ `upstream/src/engines/SoCalculator.cpp:414-427`
  - [near cross-file w=1.00 line=0.90] `src/elements/GL/SoGLVertexAttributeElement.cpp:109-163` ⟵ `upstream/src/elements/GL/SoGLVertexAttributeElement.cpp:109-163`
  - [verbatim cross-file w=1.00 line=0.98] `src/caches/SoGLCacheList.cpp:180-449` ⟵ `upstream/src/caches/SoGLCacheList.cpp:182-451`
  - [near cross-file w=1.00 line=0.93] `src/caches/SoGLCacheList.cpp:569-616` ⟵ `upstream/src/caches/SoGLCacheList.cpp:571-618`
  - [near cross-file w=1.00 line=0.80] `src/caches/SoPrimitiveVertexCache.cpp:252-312` ⟵ `upstream/src/caches/SoPrimitiveVertexCache.cpp:252-312`
  - [diverged cross-file w=1.00 line=0.50] `src/caches/SoPrimitiveVertexCache.cpp:372-383` ⟵ `upstream/src/caches/SoPrimitiveVertexCache.cpp:372-383`
  - [verbatim cross-file w=1.00 line=0.96] `src/draggers/SoTransformerDragger.cpp:1779-1834` ⟵ `upstream/src/draggers/SoTransformerDragger.cpp:1782-1837`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:2603-2603` ⟵ `Mentor/stb_image.h:2866-2866`
  - [near cross-file w=1.00 line=0.81] `tests/utils/osmesa_context_manager.h:65-78` ⟵ `tests/utils/headless_utils.h:111-127`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:911-913` ⟵ `Mentor/stb_image.h:1041-1043`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:1239-1249` ⟵ `Mentor/stb_image.h:1446-1456`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:1255-1269` ⟵ `Mentor/stb_image.h:1462-1476`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:2557-2557` ⟵ `Mentor/stb_image.h:2820-2820`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:2558-2558` ⟵ `Mentor/stb_image.h:2821-2821`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:2602-2602` ⟵ `Mentor/stb_image.h:2865-2865`
  - [near cross-file w=1.00 line=0.91] `src/sensors/SoSensorManager.cpp:637-697` ⟵ `upstream/src/sensors/SoSensorManager.cpp:657-717`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:3463-3502` ⟵ `tests/utils/stb_image.h:3755-3794`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:3414-3455` ⟵ `tests/utils/stb_image.h:3706-3747`
  - [near cross-file w=1.00 line=0.91] `src/projectors/SbSphereSheetProjector.cpp:164-222` ⟵ `upstream/src/projectors/SbSphereSheetProjector.cpp:164-222`
  - [near within-file w=1.00 line=0.91] `src/rendering/SoGLImage.cpp:1499-1558` ⟵ `src/rendering/SoGLImage.cpp:1601-1660`
  - [verbatim cross-file w=1.00 line=1.00] `external/nanort/stb_image.h:3094-3114` ⟵ `tests/utils/stb_image.h:3367-3387`
  - [near cross-file w=1.00 line=0.88] `external/nanort/stb_image.h:4142-4166` ⟵ `Mentor/stb_image.h:4483-4508`
  - [near cross-file w=0.98 line=0.82] `src/shaders/SoGLSLShaderObject.cpp:84-127` ⟵ `upstream/src/shaders/SoGLSLShaderObject.cpp:84-127`
  - [near cross-file w=0.98 line=0.94] `src/misc/SoProto.cpp:822-864` ⟵ `upstream/src/misc/SoProto.cpp:867-909`
  - [near cross-file w=0.98 line=0.92] `src/draggers/SoDragger.cpp:872-900` ⟵ `upstream/src/draggers/SoDragger.cpp:875-903`
  - [diverged cross-file w=0.98 line=0.76] `src/shapenodes/SoQuadMesh.cpp:557-580` ⟵ `upstream/src/shapenodes/SoQuadMesh.cpp:564-587`
  - [verbatim cross-file w=0.96 line=0.97] `examples/Mentor/10.2.setEventCB.cpp:155-199` ⟵ `upstream/ivexamples/Mentor-headless/10.2.setEventCB.cpp:155-200`
  - [near cross-file w=0.96 line=0.81] `external/nanort/stb_image.h:974-1015` ⟵ `Mentor/stb_image.h:1139-1188`
  - [near cross-file w=0.96 line=0.95] `src/shaders/SoGeometryShader.cpp:191-244` ⟵ `upstream/src/shaders/SoGeometryShader.cpp:191-244`
  - [near cross-file w=0.96 line=0.91] `src/misc/SoProto.cpp:374-439` ⟵ `upstream/src/misc/SoProto.cpp:418-484`
  - [verbatim cross-file w=0.95 line=0.95] `src/nodes/SoExtSelection.cpp:1022-1074` ⟵ `upstream/src/nodes/SoExtSelection.cpp:1081-1133`
  - [near within-file w=0.95 line=0.81] `src/nodes/SoExtSelection.cpp:1477-1507` ⟵ `src/nodes/SoExtSelection.cpp:1522-1550`
  - [near cross-file w=0.95 line=0.80] `src/rendering/SoOffscreenRenderer.cpp:1288-1298` ⟵ `upstream/src/rendering/SoOffscreenRenderer.cpp:1032-1040`
  - [near cross-file w=0.95 line=0.94] `src/nodes/SoBumpMap.cpp:286-330` ⟵ `upstream/src/nodes/SoBumpMap.cpp:289-333`
  - [near cross-file w=0.94 line=0.84] `src/shadows/SoShadowGroup.cpp:2215-2272` ⟵ `upstream/src/shadows/SoShadowGroup.cpp:2139-2190`
  - [near cross-file w=0.94 line=0.87] `src/nodekits/SoBaseKit.cpp:2290-2307` ⟵ `upstream/src/nodekits/SoBaseKit.cpp:2317-2334`
  - [verbatim cross-file w=0.93 line=0.96] `external/nanort/stb_image.h:2866-2889` ⟵ `Mentor/stb_image.h:3129-3153`
  - [near cross-file w=0.93 line=0.94] `src/actions/SoGLRenderAction.cpp:1698-1776` ⟵ `upstream/src/actions/SoGLRenderAction.cpp:1692-1770`
  - [diverged within-file w=0.91 line=0.76] `src/rendering/SoGLImage.cpp:1424-1484` ⟵ `src/rendering/SoGLImage.cpp:1515-1586`
  - [near cross-file w=0.90 line=0.88] `src/nodes/SoVertexAttribute.cpp:252-312` ⟵ `upstream/src/nodes/SoVertexAttribute.cpp:253-315`
  - [near within-file w=0.90 line=0.93] `src/nodes/SoSceneTexture2.cpp:1520-1556` ⟵ `src/nodes/SoSceneTexture2.cpp:1324-1360`
  - [diverged cross-file w=0.90 line=0.40] `src/elements/GL/SoGLVertexAttributeElement.cpp:172-190` ⟵ `upstream/src/elements/GL/SoGLVertexAttributeElement.cpp:172-190`
  - [near cross-file w=0.89 line=0.86] `src/sensors/SoDataSensor.cpp:314-349` ⟵ `upstream/src/sensors/SoDataSensor.cpp:314-349`
  - [diverged cross-file w=0.89 line=0.67] `src/io/SoInput.cpp:215-240` ⟵ `upstream/src/io/SoInput.cpp:219-238`
  - [near within-file w=0.89 line=0.90] `src/rendering/SoOffscreenRenderer.cpp:1785-1809` ⟵ `src/rendering/SoOffscreenRenderer.cpp:1527-1551`
  - [near cross-file w=0.89 line=0.88] `src/nodes/SoWWWAnchor.cpp:231-250` ⟵ `upstream/src/nodes/SoWWWAnchor.cpp:231-250`
  - [near within-file w=0.88 line=0.86] `src/rendering/SoGLDriverDatabase.cpp:178-194` ⟵ `src/rendering/SoGLDriverDatabase.cpp:281-297`
  - [diverged cross-file w=0.88 line=0.71] `src/misc/CoinResources.cpp:186-264` ⟵ `upstream/src/misc/CoinResources.cpp:191-269`
  - [near cross-file w=0.88 line=0.82] `src/nodes/SoLevelOfDetail.cpp:366-376` ⟵ `upstream/src/nodes/SoLevelOfDetail.cpp:377-387`
  - [diverged cross-file w=0.87 line=0.75] `src/rendering/SoOffscreenRenderer.cpp:2220-2226` ⟵ `upstream/src/rendering/SoOffscreenRenderer.cpp:1737-1743`
  - [diverged cross-file w=0.86 line=0.63] `external/nanort/stb_image.h:7073-7107` ⟵ `Mentor/stb_image.h:7583-7622`
  - [diverged cross-file w=0.86 line=0.71] `src/misc/SoBase.cpp:291-337` ⟵ `upstream/src/misc/SoBase.cpp:301-343`
  - [diverged cross-file w=0.85 line=0.76] `src/errors/SoDebugError.cpp:232-257` ⟵ `upstream/src/errors/SoDebugError.cpp:237-261`
  - [diverged cross-file w=0.84 line=0.65] `src/shaders/SoGLSLShaderProgram.cpp:174-223` ⟵ `upstream/src/shaders/SoGLSLShaderProgram.cpp:152-194`
  - [diverged within-file w=0.82 line=0.76] `src/actions/SoGLRenderAction.cpp:2212-2260` ⟵ `src/actions/SoGLRenderAction.cpp:2135-2183`
  - [near within-file w=0.81 line=0.85] `src/actions/SoGLRenderAction.cpp:2051-2081` ⟵ `src/actions/SoGLRenderAction.cpp:1974-2004`
  - [near cross-file w=0.80 line=0.82] `src/rendering/CoinOffscreenGLCanvas.cpp:150-221` ⟵ `upstream/src/rendering/CoinOffscreenGLCanvas.cpp:105-167`
  - [near cross-file w=0.79 line=0.88] `external/nanort/stb_image.h:5255-5261` ⟵ `Mentor/stb_image.h:5670-5677`
  - [diverged cross-file w=0.79 line=0.40] `src/base/list.cpp:145-154` ⟵ `upstream/src/base/list.cpp:178-183`
  - [diverged cross-file w=0.77 line=0.79] `src/shapenodes/SoIndexedMarkerSet.cpp:234-299` ⟵ `upstream/src/shapenodes/SoIndexedMarkerSet.cpp:236-301`
  - [near cross-file w=0.75 line=0.81] `src/shapenodes/SoShape.cpp:1549-1600` ⟵ `upstream/src/shapenodes/SoShape.cpp:1609-1660`
  - [diverged cross-file w=0.74 line=0.54] `src/misc/SoBase.cpp:677-726` ⟵ `upstream/src/misc/SoBase.cpp:692-742`
  - [diverged cross-file w=0.70 line=0.74] `src/base/namemap.cpp:176-203` ⟵ `upstream/src/base/namemap.cpp:168-197`
  - [diverged cross-file w=0.70 line=0.72] `src/shapenodes/SoShape.cpp:1129-1170` ⟵ `upstream/src/shapenodes/SoShape.cpp:1179-1220`
  - [diverged cross-file w=0.70 line=0.70] `external/nanort/stb_image.h:3119-3155` ⟵ `tests/utils/stb_image.h:3414-3447`
  - [diverged cross-file w=0.70 line=0.77] `src/rendering/SoVertexArrayIndexer.cpp:247-305` ⟵ `upstream/src/rendering/SoVertexArrayIndexer.cpp:253-311`
  - [diverged cross-file w=0.69 line=0.47] `src/hardcopy/VectorizePSAction.cpp:387-405` ⟵ `upstream/src/hardcopy/VectorizePSAction.cpp:387-405`
  - [diverged cross-file w=0.68 line=0.41] `src/glue/gl.cpp:5629-5666` ⟵ `upstream/src/glue/gl.cpp:5057-5094`
  - [diverged cross-file w=0.67 line=0.78] `external/nanort/stb_image.h:7154-7163` ⟵ `tests/utils/stb_image.h:7676-7688`
  - [diverged within-file w=0.67 line=0.69] `src/actions/SoReorganizeAction.cpp:632-643` ⟵ `src/actions/SoReorganizeAction.cpp:692-713`
  - [diverged cross-file w=0.64 line=0.67] `src/elements/GL/SoGLUpdateAreaElement.cpp:86-101` ⟵ `upstream/src/elements/GL/SoGLUpdateAreaElement.cpp:87-101`
  - [diverged cross-file w=0.62 line=0.69] `src/elements/GL/SoGLMultiTextureImageElement.cpp:319-376` ⟵ `upstream/src/elements/GL/SoGLMultiTextureImageElement.cpp:319-376`
  - [diverged cross-file w=0.62 line=0.53] `src/misc/SoConfigSettings.cpp:66-97` ⟵ `upstream/src/misc/SoConfigSettings.cpp:72-98`
  - [diverged cross-file w=0.61 line=0.73] `src/rendering/SoGLCubeMapImage.cpp:297-344` ⟵ `upstream/src/rendering/SoGLCubeMapImage.cpp:312-359`

## Ball-Lang/ball — 493 коммитов | циклы=0(SCC≤1) | копипаст=13

### `7ca74d037c01` — "fix(ts-compiler): VarPattern with type annotation generates type-check condition" (ahmednfwela, 2026-06-01) [13 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Ball-Lang_ball show 7ca74d037c01`
  - [near within-file w=1.00 line=0.89] `cpp/compiler/src/compiler.cpp:475-493` ⟵ `cpp/compiler/src/compiler.cpp:217-235`
  - [diverged within-file w=0.92 line=0.25] `tests/snapshots/cpp/19_nested_calls.snapshot.cpp:4033-4037` ⟵ `tests/snapshots/cpp/19_nested_calls.snapshot.cpp:134-138`
  - [diverged within-file w=0.90 line=0.20] `tests/snapshots/cpp/36_math_utils.snapshot.cpp:4040-4045` ⟵ `tests/snapshots/cpp/36_math_utils.snapshot.cpp:141-146`
  - [diverged within-file w=0.90 line=0.33] `tests/snapshots/cpp/06_function.snapshot.cpp:4025-4028` ⟵ `tests/snapshots/cpp/06_function.snapshot.cpp:127-130`
  - [diverged within-file w=0.90 line=0.20] `tests/snapshots/cpp/30_deep_recursion.snapshot.cpp:4031-4036` ⟵ `tests/snapshots/cpp/30_deep_recursion.snapshot.cpp:133-138`
  - [diverged within-file w=0.90 line=0.17] `tests/snapshots/cpp/15_closure.snapshot.cpp:4021-4027` ⟵ `tests/snapshots/cpp/15_closure.snapshot.cpp:124-130`
  - [diverged within-file w=0.89 line=0.20] `tests/snapshots/cpp/22_mutual_recursion.snapshot.cpp:4032-4037` ⟵ `tests/snapshots/cpp/22_mutual_recursion.snapshot.cpp:134-139`
  - [diverged within-file w=0.89 line=0.17] `tests/snapshots/cpp/21_ternary.snapshot.cpp:4026-4032` ⟵ `tests/snapshots/cpp/21_ternary.snapshot.cpp:128-134`
  - [diverged within-file w=0.89 line=0.25] `tests/snapshots/cpp/31_closures.snapshot.cpp:4021-4025` ⟵ `tests/snapshots/cpp/31_closures.snapshot.cpp:124-128`
  - [diverged within-file w=0.85 line=0.11] `tests/snapshots/cpp/20_bitwise.snapshot.cpp:4012-4021` ⟵ `tests/snapshots/cpp/20_bitwise.snapshot.cpp:116-125`
  - [diverged within-file w=0.80 line=0.43] `cpp/compiler/src/compiler.cpp:4387-4422` ⟵ `cpp/compiler/src/compiler.cpp:1733-1751`
  - [diverged within-file w=0.76 line=0.22] `tests/snapshots/cpp/25_nested_closure.snapshot.cpp:4020-4031` ⟵ `tests/snapshots/cpp/25_nested_closure.snapshot.cpp:123-134`
  - [diverged within-file w=0.68 line=0.30] `cpp/compiler/src/compiler.cpp:1155-1221` ⟵ `cpp/compiler/src/compiler.cpp:418-458`

## ErenAri/Aegis-BPF — 487 коммитов | циклы=0(SCC≤1) | копипаст=2

### `469ad9a7d8bf` — "Merge pull request #186 from ErenAri/fix/aegis-next-arena-redefinition" (Eren Arı, 2026-06-01) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ErenAri_Aegis-BPF show 469ad9a7d8bf`
  - [diverged cross-file w=0.68 line=0.40] `src/bpf_ops.cpp:105-113` ⟵ `src/main.cpp:78-83`
  - [diverged cross-file w=0.65 line=0.40] `src/bpf_ops.cpp:164-173` ⟵ `src/main.cpp:187-192`

## ciaranm/glasgow-constraint-solver — 484 коммитов | циклы=0(SCC≤1) | копипаст=17

### `0e0b2a23048e` — "dev_docs/constraints: add duplicate-variable item to new-constraint checklist" (Ciaran McCreesh, 2026-05-25) [17 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ciaranm_glasgow-constraint-solver show 0e0b2a23048e`
  - [verbatim cross-file w=1.00 line=1.00] `gcs/constraints/lex_smart_table.cc:51-59` ⟵ `gcs/constraints/lex.cc:36-44`
  - [verbatim cross-file w=1.00 line=1.00] `gcs/constraints/innards/constraints_test_utils.hh:581-582` ⟵ `gcs/constraints/constraints_test_utils.hh:391-392`
  - [verbatim cross-file w=1.00 line=1.00] `gcs/constraints/innards/constraints_test_utils.hh:200-203` ⟵ `gcs/constraints/constraints_test_utils.hh:131-134`
  - [verbatim cross-file w=1.00 line=1.00] `gcs/constraints/innards/constraints_test_utils.hh:473-501` ⟵ `gcs/constraints/constraints_test_utils.hh:283-311`
  - [near cross-file w=1.00 line=0.94] `gcs/constraints/innards/constraints_test_utils.hh:511-547` ⟵ `gcs/constraints/constraints_test_utils.hh:321-357`
  - [near within-file w=0.99 line=0.88] `gcs/constraints/mult_bc.cc:760-792` ⟵ `gcs/constraints/mult_bc.cc:713-746`
  - [near within-file w=0.98 line=0.80] `python/gcspy.cc:216-227` ⟵ `python/gcspy.cc:204-215`
  - [near within-file w=0.97 line=0.84] `gcs/constraints/element.cc:323-369` ⟵ `gcs/constraints/element.cc:264-310`
  - [diverged within-file w=0.97 line=0.78] `minizinc/fzn_glasgow.cc:163-170` ⟵ `minizinc/fzn_glasgow.cc:149-156`
  - [near within-file w=0.94 line=0.89] `gcs/constraints/regular.cc:285-313` ⟵ `gcs/constraints/regular.cc:247-275`
  - [diverged within-file w=0.94 line=0.58] `examples/smart_table_random/smart_table_random.cc:188-224` ⟵ `examples/smart_table_random/smart_table_random.cc:124-159`
  - [diverged within-file w=0.94 line=0.58] `examples/smart_table_random/smart_table_random.cc:120-155` ⟵ `examples/smart_table_random/smart_table_random.cc:192-228`
  - [near within-file w=0.91 line=0.80] `gcs/solve.cc:218-256` ⟵ `gcs/solve.cc:158-193`
  - [diverged within-file w=0.86 line=0.72] `gcs/constraints/smart_table.cc:458-491` ⟵ `gcs/constraints/smart_table.cc:422-451`
  - [near within-file w=0.80 line=0.84] `gcs/innards/proofs/proof_model.cc:231-265` ⟵ `gcs/innards/proofs/proof_model.cc:173-204`
  - [diverged within-file w=0.73 line=0.43] `gcs/constraints/linear/linear_equality.cc:228-253` ⟵ `gcs/constraints/linear/linear_equality.cc:200-219`
  - [diverged within-file w=0.62 line=0.21] `gcs/constraints/circuit/circuit_scc.cc:465-493` ⟵ `gcs/constraints/circuit/circuit_scc.cc:406-434`

## katahiromz/onigpp — 459 коммитов | циклы=0(SCC≤1) | копипаст=4

### `eefb6962d665` — "CHANGELOG.md" (Katayama Hirofumi MZ, 2025-11-27) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/katahiromz_onigpp show eefb6962d665`
  - [diverged cross-file w=1.00 line=0.77] `tests/onigpp_test.cpp:83-123` ⟵ `onigpp_test.cpp:101-141`
  - [diverged cross-file w=1.00 line=0.51] `tests/onigpp_test.cpp:310-360` ⟵ `onigpp_test.cpp:312-362`
  - [diverged cross-file w=1.00 line=0.05] `tests/onigpp_testw.cpp:85-123` ⟵ `onigpp_test.cpp:101-141`
  - [diverged cross-file w=0.78 line=0.62] `src/onigpp.cpp:1820-1834` ⟵ `onigpp.h:533-547`

## gizmodata/gizmosql — 445 коммитов | циклы=0(SCC≤1) | копипаст=9

### `d7fdcf3061d8` — "Release v1.26.3" (Philip Moore, 2026-05-26) [9 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/gizmodata_gizmosql show d7fdcf3061d8`
  - [near cross-file w=1.00 line=0.85] `src/common/gizmosql_security.cpp:195-211` ⟵ `src/library/flight_sql_security.cpp:71-87`
  - [verbatim cross-file w=1.00 line=1.00] `src/common/gizmosql_security.cpp:229-234` ⟵ `src/library/flight_sql_security.cpp:102-107`
  - [diverged within-file w=1.00 line=0.74] `src/sqlite/sqlite_server.cc:329-348` ⟵ `src/sqlite/sqlite_server.cc:280-297`
  - [diverged within-file w=1.00 line=0.64] `src/sqlite/sqlite_server.cc:851-865` ⟵ `src/sqlite/sqlite_server.cc:700-712`
  - [diverged within-file w=0.99 line=0.55] `src/sqlite/sqlite_server.cc:525-567` ⟵ `src/sqlite/sqlite_server.cc:440-477`
  - [diverged within-file w=0.99 line=0.12] `src/sqlite/sqlite_statement_batch_reader.cc:126-131` ⟵ `src/sqlite/sqlite_statement_batch_reader.cc:118-122`
  - [diverged within-file w=0.89 line=0.33] `src/sqlite/sqlite_server.cc:937-992` ⟵ `src/sqlite/sqlite_server.cc:771-808`
  - [diverged cross-file w=0.76 line=0.57] `src/common/gizmosql_library.cpp:1116-1126` ⟵ `src/library/flight_sql_library.cpp:221-234`
  - [diverged within-file w=0.60 line=0.19] `src/duckdb/duckdb_server.cpp:1207-1258` ⟵ `src/duckdb/duckdb_server.cpp:310-345`

## fox1245/NeoGraph — 435 коммитов | циклы=0(SCC≤1) | копипаст=2

### `3e9c9036d94d` — "Merge pull request #68 from fox1245/cookbook/minimal-mcp" (fox, 2026-05-31) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/fox1245_NeoGraph show 3e9c9036d94d`
  - [verbatim cross-file w=1.00 line=1.00] `examples/40_react_async_streaming.cpp:95-101` ⟵ `examples/01_react_agent.cpp:38-44`
  - [verbatim cross-file w=1.00 line=1.00] `examples/40_react_async_streaming.cpp:133-139` ⟵ `examples/01_react_agent.cpp:80-86`

## djeada/Lightpad — 421 коммитов | циклы=0(SCC≤1) | копипаст=3

### `ede0e4fd108c` — "fix syntax hilighting" (Adam Djellouli, 2026-05-31) [3 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/djeada_Lightpad show ede0e4fd108c`
  - [verbatim cross-file w=1.00 line=1.00] `App/core/textarea.cpp:168-182` ⟵ `App/textarea.cpp:13-28`
  - [diverged cross-file w=0.71 line=0.56] `App/syntax/lightpadsyntaxhighlighter.cpp:66-112` ⟵ `App/lightpadsyntaxhighlighter.cpp:50-81`
  - [diverged cross-file w=0.62 line=0.07] `App/syntax/lightpadsyntaxhighlighter.cpp:121-185` ⟵ `App/lightpadsyntaxhighlighter.cpp:16-40`

## DataDog/java-profiler — 421 коммитов | циклы=0(SCC≤1) | копипаст=53

### `e39e3ad5b54d` — "build(deps): bump com.diffplug.spotless:spotless-plugin-gradle (#565)" (dependabot[bot], 2026-06-01) [53 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/DataDog_java-profiler show e39e3ad5b54d`
  - [verbatim within-file w=1.00 line=1.00] `ddprof-lib/src/test/cpp/elfparser_ut.cpp:53-65` ⟵ `ddprof-lib/src/test/cpp/elfparser_ut.cpp:27-39`
  - [diverged within-file w=1.00 line=0.62] `ddprof-lib/src/main/cpp/vmEntry.cpp:592-626` ⟵ `ddprof-lib/src/main/cpp/vmEntry.cpp:497-521`
  - [near within-file w=1.00 line=0.88] `ddprof-lib/src/main/cpp/symbols_linux.cpp:514-555` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:161-202`
  - [diverged within-file w=1.00 line=0.57] `ddprof-lib/src/main/cpp/os_macos.cpp:388-413` ⟵ `ddprof-lib/src/main/cpp/os_macos.cpp:276-306`
  - [verbatim within-file w=1.00 line=1.00] `ddprof-lib/src/main/cpp/os_macos.cpp:113-117` ⟵ `ddprof-lib/src/main/cpp/os_macos.cpp:124-128`
  - [verbatim cross-file w=1.00 line=1.00] `ddprof-lib/src/main/cpp/hotspot/hotspotSupport.cpp:82-119` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:551-588`
  - [verbatim cross-file w=1.00 line=1.00] `ddprof-lib/src/main/cpp/hotspot/vmStructs.cpp:958-965` ⟵ `ddprof-lib/src/main/cpp/vmStructs_dd.cpp:190-197`
  - [diverged cross-file w=1.00 line=0.60] `ddprof-lib/src/main/cpp/hotspot/hotspotSupport.cpp:992-1000` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:437-445`
  - [diverged cross-file w=1.00 line=0.62] `ddprof-lib/src/main/cpp/hotspot/hotspotSupport.cpp:1001-1034` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:446-479`
  - [near within-file w=1.00 line=0.82] `ddprof-lib/src/main/cpp/javaApi.cpp:203-220` ⟵ `ddprof-lib/src/main/cpp/javaApi.cpp:171-187`
  - [diverged within-file w=1.00 line=0.57] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1691-1697` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:1275-1281`
  - [near within-file w=0.99 line=0.88] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1014-1033` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:641-660`
  - [near within-file w=0.99 line=0.85] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1659-1672` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:1243-1256`
  - [near within-file w=0.99 line=0.80] `ddprof-lib/src/main/cpp/vmEntry.cpp:645-656` ⟵ `ddprof-lib/src/main/cpp/vmEntry.cpp:535-546`
  - [verbatim within-file w=0.98 line=0.95] `ddprof-lib/src/main/cpp/profiler.cpp:1631-1696` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:1406-1468`
  - [verbatim within-file w=0.97 line=0.97] `ddprof-lib/src/main/cpp/flightRecorder.cpp:629-668` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:323-360`
  - [diverged cross-file w=0.97 line=0.59] `ddprof-lib/src/main/cpp/hotspot/hotspotSupport.cpp:1067-1081` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:509-522`
  - [diverged within-file w=0.96 line=0.44] `ddprof-lib/src/main/cpp/os_macos.cpp:37-44` ⟵ `ddprof-lib/src/main/cpp/os_macos.cpp:57-65`
  - [diverged within-file w=0.94 line=0.41] `ddprof-lib/src/main/cpp/symbols_linux.cpp:630-652` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:331-355`
  - [near within-file w=0.94 line=0.88] `ddprof-lib/src/main/cpp/symbols_linux.cpp:1025-1044` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:704-723`
  - [diverged within-file w=0.94 line=0.68] `ddprof-lib/src/main/cpp/libraries.cpp:130-161` ⟵ `ddprof-lib/src/main/cpp/libraries.cpp:40-65`
  - [diverged within-file w=0.93 line=0.76] `ddprof-lib/src/main/cpp/symbols_linux.cpp:889-940` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:545-598`
  - [near within-file w=0.92 line=0.91] `ddprof-lib/src/main/cpp/flightRecorder.cpp:808-848` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:484-521`
  - [near cross-file w=0.92 line=0.80] `ddprof-lib/src/main/cpp/hotspot/vmStructs.cpp:1154-1197` ⟵ `ddprof-lib/src/main/cpp/vmStructs_dd.h:312-355`
  - [diverged cross-file w=0.91 line=0.60] `ddprof-lib/src/main/cpp/hotspot/vmStructs.cpp:1036-1050` ⟵ `ddprof-lib/src/main/cpp/vmStructs_dd.h:146-160`
  - [diverged within-file w=0.90 line=0.59] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1583-1603` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:1177-1194`
  - [diverged within-file w=0.89 line=0.58] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1353-1387` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:967-995`
  - [diverged within-file w=0.89 line=0.69] `ddprof-lib/src/main/cpp/symbols_linux.cpp:844-886` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:504-541`
  - [diverged cross-file w=0.89 line=0.69] `ddprof-lib/src/main/cpp/hotspot/hotspotSupport.cpp:1037-1065` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:482-507`
  - [near within-file w=0.88 line=0.85] `ddprof-lib/src/main/cpp/profiler.cpp:1527-1561` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:1290-1321`
  - [diverged within-file w=0.87 line=0.68] `ddprof-lib/src/main/cpp/profiler.cpp:1146-1172` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:1071-1097`
  - [diverged cross-file w=0.85 line=0.46] `ddprof-lib/src/main/cpp/hotspot/vmStructs.cpp:968-1028` ⟵ `ddprof-lib/src/main/cpp/vmStructs_dd.cpp:200-248`
  - [diverged cross-file w=0.85 line=0.57] `ddprof-lib/src/main/cpp/profiler.cpp:208-240` ⟵ `ddprof-lib/src/main/cpp/libraries.cpp:40-65`
  - [diverged within-file w=0.84 line=0.43] `ddprof-lib/src/main/cpp/wallClock.cpp:236-242` ⟵ `ddprof-lib/src/main/cpp/wallClock.cpp:59-63`
  - [diverged within-file w=0.84 line=0.79] `ddprof-lib/src/main/cpp/profiler.cpp:1035-1051` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:982-997`
  - [diverged within-file w=0.84 line=0.53] `ddprof-lib/src/main/cpp/profiler.cpp:1101-1133` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:1032-1058`
  - [diverged cross-file w=0.82 line=0.58] `ddprof-lib/src/main/cpp/profiler.cpp:818-827` ⟵ `ddprof-lib/src/main/cpp/profiler.h:182-192`
  - [diverged within-file w=0.81 line=0.61] `ddprof-lib/src/main/cpp/symbols_macos.cpp:204-242` ⟵ `ddprof-lib/src/main/cpp/symbols_macos.cpp:163-197`
  - [diverged within-file w=0.81 line=0.57] `ddprof-lib/src/main/cpp/symbols_linux.cpp:448-468` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:93-114`
  - [diverged cross-file w=0.79 line=0.75] `ddprof-lib/src/main/cpp/hotspot/hotspotSupport.cpp:922-944` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:352-375`
  - [diverged within-file w=0.79 line=0.63] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1507-1551` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:1107-1145`
  - [diverged within-file w=0.78 line=0.60] `ddprof-lib/src/main/cpp/symbols_linux.cpp:471-478` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:118-127`
  - [diverged within-file w=0.77 line=0.27] `ddprof-lib/src/main/cpp/flightRecorder.cpp:2053-2088` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:1576-1604`
  - [diverged within-file w=0.77 line=0.55] `ddprof-lib/src/main/cpp/symbols_linux.cpp:943-1010` ⟵ `ddprof-lib/src/main/cpp/symbols_linux.cpp:601-660`
  - [diverged within-file w=0.75 line=0.51] `ddprof-lib/src/main/cpp/profiler.cpp:1054-1098` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:1000-1029`
  - [diverged within-file w=0.74 line=0.50] `ddprof-lib/src/main/cpp/linearAllocator.cpp:137-170` ⟵ `ddprof-lib/src/main/cpp/linearAllocator.cpp:45-62`
  - [diverged within-file w=0.73 line=0.33] `ddprof-lib/src/main/cpp/profiler.cpp:968-982` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:933-941`
  - [near within-file w=0.71 line=0.81] `ddprof-lib/src/main/cpp/objectSampler.cpp:149-169` ⟵ `ddprof-lib/src/main/cpp/objectSampler.cpp:122-138`
  - [diverged within-file w=0.70 line=0.41] `ddprof-lib/src/main/cpp/profiler.cpp:990-1012` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:948-962`
  - [diverged within-file w=0.70 line=0.12] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1963-1987` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:1511-1532`
  - [diverged within-file w=0.69 line=0.58] `ddprof-lib/src/main/cpp/flightRecorder.cpp:228-249` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:161-172`
  - [diverged within-file w=0.65 line=0.43] `ddprof-lib/src/main/cpp/flightRecorder.cpp:1323-1350` ⟵ `ddprof-lib/src/main/cpp/flightRecorder.cpp:944-964`
  - [diverged within-file w=0.63 line=0.35] `ddprof-lib/src/main/cpp/profiler.cpp:1564-1609` ⟵ `ddprof-lib/src/main/cpp/profiler.cpp:1340-1384`

## andrewnakas/Boxedwine64 — 421 коммитов | циклы=3(SCC≤5) | копипаст=19
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 3 include-циклов, крупнейший SCC=5. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/andrewnakas_Boxedwine64`

### `4462366a82b3` — "docs: update README — wine64 + wineserver run headless, full IPC handshake works" (Andrew Nakas, 2026-05-31) [19 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/andrewnakas_Boxedwine64 show 4462366a82b3`
  - [near cross-file w=1.00 line=0.86] `source/opengl/glfunctions_ext1.cpp:190-203` ⟵ `source/opengl/glcommon.cpp:363-376`
  - [near within-file w=1.00 line=0.86] `source/opengl/glcommon.cpp:422-435` ⟵ `source/opengl/glcommon.cpp:363-376`
  - [verbatim within-file w=1.00 line=0.95] `source/kernel/kthread.cpp:817-889` ⟵ `source/kernel/kthread.cpp:621-693`
  - [verbatim within-file w=1.00 line=0.95] `source/kernel/devs/devdsp.cpp:185-214` ⟵ `source/kernel/devs/devdsp.cpp:118-147`
  - [near within-file w=1.00 line=0.88] `source/ui/data/globalSettings.cpp:973-993` ⟵ `source/ui/data/globalSettings.cpp:951-971`
  - [near within-file w=1.00 line=0.85] `source/kernel/kprocess.cpp:1989-2022` ⟵ `source/kernel/kprocess.cpp:1777-1810`
  - [diverged within-file w=1.00 line=0.78] `source/opengl/glfunctions_ext1.cpp:7575-7583` ⟵ `source/opengl/glfunctions_ext1.cpp:7100-7108`
  - [near within-file w=0.94 line=0.87] `source/emulation/cpu/normal/normalPlatformMultiThreaded.cpp:149-164` ⟵ `source/emulation/cpu/normal/normalPlatformMultiThreaded.cpp:79-94`
  - [near within-file w=0.93 line=0.80] `source/emulation/cpu/common/common_sse.cpp:759-772` ⟵ `source/emulation/cpu/common/common_sse.cpp:712-726`
  - [diverged within-file w=0.93 line=0.78] `source/kernel/kprocess.cpp:2078-2088` ⟵ `source/kernel/kprocess.cpp:1866-1876`
  - [diverged within-file w=0.91 line=0.67] `source/kernel/kmemory.cpp:739-761` ⟵ `source/kernel/kmemory.cpp:684-700`
  - [diverged within-file w=0.85 line=0.38] `source/kernel/devs/devdsp.cpp:97-105` ⟵ `source/kernel/devs/devdsp.cpp:65-69`
  - [diverged within-file w=0.85 line=0.79] `source/emulation/cpu/normal/normalCPU.cpp:334-376` ⟵ `source/emulation/cpu/normal/normalCPU.cpp:184-217`
  - [diverged within-file w=0.83 line=0.16] `source/opengl/glcommon.cpp:570-582` ⟵ `source/opengl/glcommon.cpp:504-514`
  - [diverged within-file w=0.83 line=0.71] `source/kernel/ksystem.cpp:448-517` ⟵ `source/kernel/ksystem.cpp:356-422`
  - [diverged within-file w=0.80 line=0.59] `source/emulation/cpu/normal/normal_strings_op.h:72-97` ⟵ `source/emulation/cpu/normal/normal_strings_op.h:54-69`
  - [diverged within-file w=0.72 line=0.30] `source/emulation/softmmu/codePageData.cpp:220-225` ⟵ `source/emulation/softmmu/codePageData.cpp:233-242`
  - [diverged cross-file w=0.63 line=0.38] `platform/sdl/kdspaudio.cpp:109-133` ⟵ `source/kernel/devs/devdsp.cpp:118-147`
  - [diverged cross-file w=0.62 line=0.34] `source/ui/controls/optionsView.cpp:157-186` ⟵ `source/ui/controls/containersView.cpp:422-446`

## MaximumTrainer/MaximumTrainer_Redux — 414 коммитов | циклы=0(SCC≤1) | копипаст=19

### `d0a6b1081962` — "chore: deploy Wasm build artifacts for v0.0.103" (github-actions[bot], 2026-05-31) [19 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/MaximumTrainer_MaximumTrainer_Redux show d0a6b1081962`
  - [near within-file w=1.00 line=0.81] `src/persistence/file/xmlutil.cpp:310-331` ⟵ `src/persistence/file/xmlutil.cpp:282-303`
  - [verbatim within-file w=1.00 line=1.00] `src/ui/mainwindow.cpp:361-382` ⟵ `src/ui/mainwindow.cpp:287-308`
  - [near within-file w=0.98 line=0.92] `src/ui/mainwindow.cpp:1681-1713` ⟵ `src/ui/mainwindow.cpp:1223-1252`
  - [diverged cross-file w=0.97 line=0.19] `src/persistence/db/selfloops_service.cpp:36-56` ⟵ `src/app/util.cpp:812-834`
  - [near within-file w=0.97 line=0.90] `src/ui/mainwindow.cpp:1379-1432` ⟵ `src/ui/mainwindow.cpp:1004-1054`
  - [near within-file w=0.97 line=0.94] `src/ui/workoutdialog.cpp:3328-3375` ⟵ `src/ui/workoutdialog.cpp:3047-3092`
  - [diverged within-file w=0.96 line=0.79] `src/ui/mainwindow.cpp:1344-1373` ⟵ `src/ui/mainwindow.cpp:971-998`
  - [near within-file w=0.93 line=0.92] `src/ui/workoutdialog.cpp:1583-1621` ⟵ `src/ui/workoutdialog.cpp:1528-1564`
  - [diverged within-file w=0.93 line=0.67] `src/ui/workoutdialog.cpp:2936-2950` ⟵ `src/ui/workoutdialog.cpp:2713-2724`
  - [near within-file w=0.93 line=0.88] `src/ui/workoutdialog.cpp:1789-1842` ⟵ `src/ui/workoutdialog.cpp:1656-1706`
  - [near within-file w=0.92 line=0.85] `src/ui/mainwindow.cpp:2120-2159` ⟵ `src/ui/mainwindow.cpp:1592-1629`
  - [near within-file w=0.90 line=0.82] `src/ui/mainwindow.cpp:2085-2105` ⟵ `src/ui/mainwindow.cpp:1556-1577`
  - [near within-file w=0.90 line=0.82] `src/ui/workoutdialog.cpp:1695-1755` ⟵ `src/ui/workoutdialog.cpp:1571-1622`
  - [near within-file w=0.89 line=0.87] `src/ui/workoutdialog.cpp:3163-3234` ⟵ `src/ui/workoutdialog.cpp:2931-2996`
  - [diverged within-file w=0.85 line=0.68] `src/ui/mainwindow.cpp:2032-2051` ⟵ `src/ui/mainwindow.cpp:1502-1521`
  - [near within-file w=0.82 line=0.83] `src/btle/webbluetooth_bridge.cpp:278-288` ⟵ `src/btle/webbluetooth_bridge.cpp:118-128`
  - [diverged within-file w=0.74 line=0.65] `src/ui/mainwindow.cpp:1438-1460` ⟵ `src/ui/mainwindow.cpp:1060-1080`
  - [diverged within-file w=0.64 line=0.21] `src/ui/mainwindow.cpp:1508-1521` ⟵ `src/ui/mainwindow.cpp:1128-1140`
  - [diverged within-file w=0.62 line=0.79] `src/ui/dialoglogin.cpp:733-763` ⟵ `src/ui/dialoglogin.cpp:365-409`

## gcol33/tulpa — 375 коммитов | циклы=0(SCC≤1) | копипаст=28

### `405fa124a974` — "chore: untrack tests/testthat/Rplots.pdf test artifact" (Gilles Colling, 2026-06-01) [28 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/gcol33_tulpa show 405fa124a974`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_nuts.h:529-546` ⟵ `src/hmc_nuts_chain.h:1051-1068`
  - [verbatim cross-file w=1.00 line=1.00] `src/tulpa_shims_stochastic.h:195-236` ⟵ `src/tulpa_shims.cpp:1816-1857`
  - [verbatim cross-file w=1.00 line=1.00] `src/tulpa_shims_sparse.h:138-146` ⟵ `src/tulpa_shims.cpp:1613-1621`
  - [verbatim cross-file w=1.00 line=1.00] `src/tulpa_shims_pg.h:157-185` ⟵ `src/tulpa_shims.cpp:1311-1339`
  - [verbatim cross-file w=1.00 line=1.00] `src/tulpa_shims_pg.h:114-129` ⟵ `src/tulpa_shims.cpp:1268-1283`
  - [verbatim cross-file w=1.00 line=1.00] `src/tulpa_shims_pg.h:78-93` ⟵ `src/tulpa_shims.cpp:1232-1247`
  - [verbatim cross-file w=1.00 line=1.00] `src/tulpa_shims_pg.h:12-46` ⟵ `src/tulpa_shims.cpp:1166-1200`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_window.h:135-144` ⟵ `src/hmc_nuts_chain.h:444-453`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_window.h:46-64` ⟵ `src/hmc_nuts_chain.h:355-373`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_window.h:14-34` ⟵ `src/hmc_nuts_chain.h:323-343`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_gp_gradients.h:333-341` ⟵ `src/hmc_gp.h:971-979`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_nuts.h:305-324` ⟵ `src/hmc_nuts_chain.h:759-778`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_nuts.h:131-152` ⟵ `src/hmc_nuts_chain.h:585-606`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_gp_nc.h:265-269` ⟵ `src/hmc_gp.h:1285-1289`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_gp_solvers.h:33-67` ⟵ `src/hmc_gp.h:231-265`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_gp_solvers.h:79-108` ⟵ `src/hmc_gp.h:277-306`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_hmc.h:13-25` ⟵ `src/hmc_nuts_chain.h:1107-1119`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_hmc.h:44-73` ⟵ `src/hmc_nuts_chain.h:1138-1167`
  - [verbatim cross-file w=1.00 line=1.00] `src/hmc_nuts_chain_iter_hmc.h:139-156` ⟵ `src/hmc_nuts_chain.h:1240-1257`
  - [near cross-file w=0.99 line=0.92] `src/hmc_nuts_chain.cpp:111-143` ⟵ `src/hmc_nuts_chain.h:1351-1383`
  - [diverged within-file w=0.92 line=0.73] `src/laplace_family_link.h:199-224` ⟵ `src/laplace_family_link.h:162-184`
  - [diverged within-file w=0.90 line=0.75] `src/laplace_core.cpp:439-467` ⟵ `src/laplace_core.cpp:540-564`
  - [diverged within-file w=0.87 line=0.54] `src/laplace_family_link.h:230-242` ⟵ `src/laplace_family_link.h:190-199`
  - [diverged within-file w=0.78 line=0.60] `src/pg_binomial.cpp:392-435` ⟵ `src/pg_binomial.cpp:507-562`
  - [near cross-file w=0.75 line=0.80] `src/hmc_gradient_fallback.cpp:81-100` ⟵ `src/hmc_gradient_fallback_impl.h:342-356`
  - [diverged cross-file w=0.70 line=0.31] `src/hmc_nuts_parallel.cpp:48-113` ⟵ `src/hmc_nuts_parallel.h:29-94`
  - [diverged within-file w=0.65 line=0.34] `src/laplace_newton.h:137-167` ⟵ `src/laplace_newton.h:77-109`
  - [diverged cross-file w=0.60 line=0.52] `src/hmc_gradient_fallback.cpp:114-144` ⟵ `src/hmc_gradient_fallback_impl.h:135-180`

## AB-lab113/hidering — 370 коммитов | циклы=0(SCC≤1) | копипаст=21

### `095889686863` — "[HRG] docs(claude-md): mark Phase 5 step 2 done (PQ key types + Dilithium3/Kyber768 ops + HFv16)" (AB-lab113, 2026-05-31) [21 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/AB-lab113_hidering show 095889686863`
  - [near within-file w=1.00 line=0.91] `src/cryptonote_core/blockchain.cpp:4626-4657` ⟵ `src/cryptonote_core/blockchain.cpp:4546-4577`
  - [near within-file w=1.00 line=0.91] `src/wallet/wallet_rpc_server.cpp:344-369` ⟵ `src/wallet/wallet_rpc_server.cpp:292-317`
  - [near within-file w=1.00 line=0.93] `src/cryptonote_core/cryptonote_core.cpp:1582-1610` ⟵ `src/cryptonote_core/cryptonote_core.cpp:1808-1836`
  - [near within-file w=1.00 line=0.92] `src/cryptonote_core/cryptonote_core.cpp:1807-1843` ⟵ `src/cryptonote_core/cryptonote_core.cpp:2033-2069`
  - [near within-file w=1.00 line=0.83] `src/simplewallet/simplewallet.cpp:10758-10811` ⟵ `src/simplewallet/simplewallet.cpp:10831-10884`
  - [near within-file w=1.00 line=0.90] `src/cryptonote_core/cryptonote_core.cpp:1394-1425` ⟵ `src/cryptonote_core/cryptonote_core.cpp:1643-1674`
  - [near within-file w=0.99 line=0.80] `src/wallet/wallet2.cpp:11708-11733` ⟵ `src/wallet/wallet2.cpp:11751-11776`
  - [near within-file w=0.98 line=0.88] `src/cryptonote_core/cryptonote_core.cpp:1184-1208` ⟵ `src/cryptonote_core/cryptonote_core.cpp:1445-1469`
  - [near within-file w=0.96 line=0.89] `src/wallet/wallet2.cpp:9738-9776` ⟵ `src/wallet/wallet2.cpp:9809-9847`
  - [near within-file w=0.94 line=0.88] `src/crypto/rx-slow-hash.c:305-326` ⟵ `src/crypto/rx-slow-hash.c:266-287`
  - [near within-file w=0.93 line=0.83] `src/wallet/wallet2.cpp:2624-2665` ⟵ `src/wallet/wallet2.cpp:2581-2622`
  - [diverged within-file w=0.92 line=0.78] `src/cryptonote_core/blockchain.cpp:4585-4620` ⟵ `src/cryptonote_core/blockchain.cpp:4502-4540`
  - [near within-file w=0.91 line=0.89] `src/wallet/wallet2.cpp:4567-4592` ⟵ `src/wallet/wallet2.cpp:4498-4523`
  - [diverged within-file w=0.90 line=0.50] `src/wallet/wallet2.cpp:2206-2215` ⟵ `src/wallet/wallet2.cpp:2174-2183`
  - [near within-file w=0.88 line=0.90] `src/wallet/wallet2.cpp:3146-3172` ⟵ `src/wallet/wallet2.cpp:3094-3120`
  - [near within-file w=0.85 line=0.86] `src/wallet/wallet_rpc_server.cpp:1234-1296` ⟵ `src/wallet/wallet_rpc_server.cpp:1156-1212`
  - [near within-file w=0.85 line=0.85] `src/wallet/wallet_rpc_server.cpp:1301-1356` ⟵ `src/wallet/wallet_rpc_server.cpp:1217-1266`
  - [near within-file w=0.84 line=0.85] `src/wallet/wallet2.cpp:2972-3008` ⟵ `src/wallet/wallet2.cpp:2917-2956`
  - [diverged within-file w=0.83 line=0.79] `src/wallet/wallet2.cpp:9017-9042` ⟵ `src/wallet/wallet2.cpp:9093-9116`
  - [near within-file w=0.83 line=0.94] `src/wallet/wallet2.cpp:6123-6167` ⟵ `src/wallet/wallet2.cpp:6071-6115`
  - [diverged within-file w=0.82 line=0.62] `src/cryptonote_core/cryptonote_core.cpp:1281-1336` ⟵ `src/cryptonote_core/cryptonote_core.cpp:1542-1594`

## FLOX-Foundation/flox — 355 коммитов | циклы=1(SCC≤3) | копипаст=9
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/FLOX-Foundation_flox`

### `e82a019dc354` — "release: v0.6.5" (github-actions[bot], 2026-05-31) [9 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/FLOX-Foundation_flox show e82a019dc354`
  - [diverged within-file w=1.00 line=0.54] `src/replay/binary_log_writer.cpp:448-514` ⟵ `src/replay/binary_log_writer.cpp:343-411`
  - [near within-file w=0.96 line=0.93] `src/replay/binary_log_reader.cpp:1798-1845` ⟵ `src/replay/binary_log_reader.cpp:547-591`
  - [diverged within-file w=0.80 line=0.71] `src/replay/segment_ops.cpp:682-741` ⟵ `src/replay/segment_ops.cpp:512-576`
  - [diverged within-file w=0.79 line=0.71] `src/replay/segment_ops.cpp:512-576` ⟵ `src/replay/segment_ops.cpp:682-741`
  - [near within-file w=0.75 line=0.88] `src/replay/binary_log_writer.cpp:671-696` ⟵ `src/replay/binary_log_writer.cpp:567-587`
  - [diverged within-file w=0.74 line=0.56] `tests/test_symbol_registry.cpp:397-436` ⟵ `tests/test_symbol_registry.cpp:331-365`
  - [diverged within-file w=0.73 line=0.75] `src/engine/symbol_registry.cpp:616-665` ⟵ `src/engine/symbol_registry.cpp:529-565`
  - [diverged within-file w=0.68 line=0.60] `src/engine/symbol_registry.cpp:773-816` ⟵ `src/engine/symbol_registry.cpp:667-697`
  - [diverged within-file w=0.65 line=0.10] `include/flox/util/eventing/event_bus.h:262-281` ⟵ `include/flox/util/eventing/event_bus.h:222-251`

## cwida/privacy — 341 коммитов | циклы=1(SCC≤2) | копипаст=17
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/cwida_privacy`

### `1f0518be3dc1` — "Merge pull request #17 from cwida/dp-smooth-sensitivity" (Ilaria Battiston, 2026-05-29) [17 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/cwida_privacy show 1f0518be3dc1`
  - [near cross-file w=1.00 line=0.91] `benchmark/clickbench/pac_clickhouse_benchmark.cpp:1470-1496` ⟵ `benchmark/pac_clickhouse_benchmark.cpp:669-695`
  - [diverged cross-file w=1.00 line=0.77] `src/core/privacy_extension.cpp:189-246` ⟵ `src/core/pac_extension.cpp:87-144`
  - [near cross-file w=1.00 line=0.82] `src/core/privacy_extension.cpp:50-74` ⟵ `src/core/pac_extension.cpp:46-70`
  - [near cross-file w=1.00 line=0.91] `src/categorical/pac_categorical_lambdas.cpp:413-441` ⟵ `src/categorical/pac_categorical_rewriter.cpp:937-965`
  - [near cross-file w=1.00 line=0.80] `src/categorical/pac_categorical_lambdas.cpp:367-376` ⟵ `src/categorical/pac_categorical_rewriter.cpp:856-865`
  - [near cross-file w=1.00 line=0.92] `src/categorical/pac_categorical_lambdas.cpp:327-350` ⟵ `src/categorical/pac_categorical_rewriter.cpp:816-839`
  - [verbatim cross-file w=1.00 line=1.00] `src/categorical/pac_categorical_lambdas.cpp:255-281` ⟵ `src/categorical/pac_categorical_rewriter.cpp:686-712`
  - [verbatim cross-file w=1.00 line=1.00] `benchmark/tpch/pac_tpch_benchmark.cpp:259-300` ⟵ `benchmark/pac_tpch_benchmark.cpp:203-244`
  - [near cross-file w=1.00 line=0.80] `benchmark/tpch/pac_tpch_benchmark.cpp:47-99` ⟵ `benchmark/pac_tpch_benchmark.cpp:45-97`
  - [diverged cross-file w=0.97 line=0.25] `benchmark/sqlstorm/pac_sqlstorm_benchmark.cpp:1226-1272` ⟵ `benchmark/pac_tpch_benchmark.cpp:45-97`
  - [verbatim within-file w=0.92 line=0.96] `src/query_processing/pac_plan_traversal.cpp:1015-1091` ⟵ `src/query_processing/pac_plan_traversal.cpp:743-819`
  - [diverged cross-file w=0.89 line=0.48] `src/categorical/pac_categorical_lambdas.cpp:29-48` ⟵ `src/categorical/pac_categorical_rewriter.cpp:488-504`
  - [diverged cross-file w=0.78 line=0.72] `benchmark/tpch/pac_tpch_benchmark.cpp:789-843` ⟵ `benchmark/pac_tpch_benchmark.cpp:528-572`
  - [diverged cross-file w=0.77 line=0.61] `src/include/categorical/pac_categorical_detection.hpp:217-236` ⟵ `src/include/categorical/pac_categorical_rewriter.hpp:201-219`
  - [diverged within-file w=0.73 line=0.49] `src/categorical/pac_categorical_rewriter.cpp:520-574` ⟵ `src/categorical/pac_categorical_rewriter.cpp:1190-1223`
  - [diverged cross-file w=0.67 line=0.50] `benchmark/clickbench/pac_clickhouse_benchmark.cpp:864-911` ⟵ `benchmark/pac_clickhouse_benchmark.cpp:292-325`
  - [diverged cross-file w=0.65 line=0.64] `src/core/privacy_optimizer.cpp:550-566` ⟵ `src/core/pac_optimizer.cpp:205-214`

## joshualongenecker/home-assistant-bridge-esphome — 340 коммитов | циклы=0(SCC≤1) | копипаст=4

### `8ed9af78e53f` — "Merge pull request #100 from joshualongenecker/updateDefault" (Joshua Longenecker, 2026-04-25) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/joshualongenecker_home-assistant-bridge-esphome show 8ed9af78e53f`
  - [diverged cross-file w=0.89 line=0.73] `components/geappliances_bridge/mqtt_bridge.cpp:27-57` ⟵ `src/mqtt_bridge.cpp:64-95`
  - [near cross-file w=0.88 line=0.89] `components/geappliances_bridge/mqtt_bridge.cpp:106-133` ⟵ `src/mqtt_bridge.cpp:128-155`
  - [diverged cross-file w=0.78 line=0.62] `components/geappliances_bridge/mqtt_bridge.cpp:61-94` ⟵ `src/mqtt_bridge.cpp:99-124`
  - [diverged cross-file w=0.74 line=0.09] `components/geappliances_bridge/esphome_uart_adapter.cpp:9-39` ⟵ `src/tiny_uart_adapter.cpp:10-24`

## dasch-swiss/sipi — 340 коммитов | циклы=0(SCC≤1) | копипаст=14

### `6d3218f07510` — "ci: hoist GH_TOKEN to job-level env in publish.yml" (Ivan Subotic, 2026-05-28) [14 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/dasch-swiss_sipi show 6d3218f07510`
  - [verbatim cross-file w=1.00 line=1.00] `src/cli/sipi.cpp:369-379` ⟵ `src/sipi.cpp:244-254`
  - [verbatim cross-file w=1.00 line=1.00] `src/cli/sipi.cpp:407-422` ⟵ `src/sipi.cpp:282-297`
  - [diverged cross-file w=1.00 line=0.04] `src/cli/sipi.cpp:1083-1091` ⟵ `src/sipi.cpp:1225-1249`
  - [verbatim cross-file w=1.00 line=1.00] `src/metadata/exif.cpp:252-261` ⟵ `src/metadata/SipiExif.cpp:131-140`
  - [verbatim cross-file w=1.00 line=1.00] `src/populate_from_image.cpp:26-39` ⟵ `include/SipiSentry.h:66-79`
  - [verbatim cross-file w=1.00 line=1.00] `src/populate_from_image.cpp:43-54` ⟵ `include/SipiSentry.h:86-97`
  - [verbatim cross-file w=1.00 line=1.00] `src/populate_from_image.cpp:60-71` ⟵ `include/SipiSentry.h:122-133`
  - [near cross-file w=0.96 line=0.88] `src/cli/sipi.cpp:1117-1142` ⟵ `src/sipi.cpp:1275-1303`
  - [diverged within-file w=0.96 line=0.77] `src/SipiImage.cpp:1475-1515` ⟵ `src/SipiImage.cpp:1387-1423`
  - [verbatim within-file w=0.95 line=0.95] `src/SipiCache.cpp:563-599` ⟵ `src/SipiCache.cpp:480-515`
  - [diverged cross-file w=0.94 line=0.46] `src/observability/sentry_init.cpp:18-45` ⟵ `src/sipi.cpp:333-356`
  - [diverged within-file w=0.90 line=0.72] `src/formats/SipiIOTiff.cpp:1860-1884` ⟵ `src/formats/SipiIOTiff.cpp:1689-1710`
  - [diverged cross-file w=0.78 line=0.08] `src/cli/commands/convert_access_file.cpp:111-135` ⟵ `src/SipiImage.cpp:1275-1304`
  - [diverged cross-file w=0.70 line=0.34] `src/metadata/essentials.cpp:38-61` ⟵ `src/metadata/SipiEssentials.cpp:59-83`

## Arsia-Mons/Silencer — 328 коммитов | циклы=3(SCC≤2) | копипаст=58
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 3 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/Arsia-Mons_Silencer`

### `af4c50c5b8ec` — "Release v00058 (#266)" (Henry Ventura, 2026-05-31) [58 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Arsia-Mons_Silencer show af4c50c5b8ec`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:639-657` ⟵ `src/renderer.cpp:509-527`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/input/keybinds.cpp:464-479` ⟵ `src/game.cpp:5199-5214`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/main.cpp:14-21` ⟵ `src/main.cpp:12-19`
  - [diverged cross-file w=1.00 line=0.63] `clients/silencer/src/main.cpp:86-141` ⟵ `src/main.cpp:84-139`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/net/lobby.cpp:67-95` ⟵ `src/lobby.cpp:34-62`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/objects/pickup.cpp:92-114` ⟵ `src/pickup.cpp:91-113`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/platform/os.cpp:68-76` ⟵ `src/os.cpp:34-42`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/projectiles/grenade.cpp:315-336` ⟵ `src/grenade.cpp:283-304`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/game/session/ambience_mixer.cpp:104-110` ⟵ `src/game.cpp:3805-3811`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/game/replay/replay.cpp:218-226` ⟵ `src/replay.cpp:216-224`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:327-335` ⟵ `src/renderer.cpp:219-227`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:1312-1320` ⟵ `src/renderer.cpp:1150-1158`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:4552-4625` ⟵ `src/player.cpp:4408-4481`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:1425-1436` ⟵ `src/renderer.cpp:1263-1274`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:4241-4257` ⟵ `src/player.cpp:4108-4124`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:3143-3174` ⟵ `src/player.cpp:3024-3055`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:2421-2446` ⟵ `src/player.cpp:2377-2402`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/resources/resources.cpp:216-226` ⟵ `src/resources.cpp:205-215`
  - [near cross-file w=1.00 line=0.93] `clients/silencer/src/actors/player/player.cpp:1021-1073` ⟵ `src/player.cpp:1007-1059`
  - [verbatim cross-file w=1.00 line=1.00] `clients/silencer/src/resources/resources.cpp:430-472` ⟵ `src/resources.cpp:410-452`
  - [near cross-file w=0.99 line=0.80] `clients/silencer/src/actors/player/player.cpp:401-413` ⟵ `src/player.cpp:331-343`
  - [near cross-file w=0.99 line=0.84] `clients/silencer/src/game/tick/tick_hostjoin.cpp:15-32` ⟵ `src/game.cpp:1875-1893`
  - [verbatim cross-file w=0.99 line=0.97] `clients/silencer/src/game/session/map_downloader.cpp:29-66` ⟵ `src/game.cpp:5383-5420`
  - [verbatim cross-file w=0.98 line=0.95] `clients/silencer/src/objects/pickup.cpp:35-90` ⟵ `src/pickup.cpp:34-89`
  - [diverged cross-file w=0.98 line=0.76] `clients/silencer/src/world/objects/world_objects.cpp:229-272` ⟵ `src/world.cpp:2380-2423`
  - [diverged cross-file w=0.98 line=0.75] `clients/silencer/src/world/network/world_network.cpp:744-761` ⟵ `src/world.cpp:1075-1092`
  - [near cross-file w=0.97 line=0.86] `clients/silencer/src/render/renderer.cpp:674-696` ⟵ `src/renderer.cpp:544-563`
  - [diverged cross-file w=0.96 line=0.39] `clients/silencer/src/world/messaging/world_messaging.cpp:88-109` ⟵ `src/world.cpp:1874-1895`
  - [diverged cross-file w=0.96 line=0.14] `clients/silencer/src/world/network/world_peer_registry.cpp:227-231` ⟵ `src/world.cpp:1439-1443`
  - [near cross-file w=0.96 line=0.95] `clients/silencer/src/world/map/map.cpp:167-188` ⟵ `src/map.cpp:155-175`
  - [diverged cross-file w=0.95 line=0.71] `clients/silencer/src/world/objects/world_objects.cpp:293-307` ⟵ `src/world.cpp:2444-2458`
  - [diverged cross-file w=0.95 line=0.50] `clients/silencer/src/world/network/world_network.cpp:787-800` ⟵ `src/world.cpp:1495-1508`
  - [diverged cross-file w=0.95 line=0.27] `clients/silencer/src/game/session/game_session.cpp:25-33` ⟵ `src/game.cpp:2191-2199`
  - [near cross-file w=0.95 line=0.80] `clients/silencer/src/objects/warper.cpp:22-50` ⟵ `src/warper.cpp:21-48`
  - [diverged cross-file w=0.94 line=0.72] `clients/silencer/src/world/objects/world_objects.cpp:29-78` ⟵ `src/world.cpp:160-209`
  - [near cross-file w=0.94 line=0.80] `clients/silencer/src/projectiles/rocketprojectile.cpp:125-151` ⟵ `src/rocketprojectile.cpp:110-135`
  - [near cross-file w=0.93 line=0.81] `clients/silencer/src/game/tick/tick_hostjoin.cpp:139-158` ⟵ `src/game.cpp:1994-2013`
  - [diverged cross-file w=0.90 line=0.67] `clients/silencer/src/game/tick/tick_singleplayer.cpp:16-48` ⟵ `src/game.cpp:1171-1204`
  - [diverged cross-file w=0.90 line=0.29] `clients/silencer/src/game/session/game_session.cpp:175-212` ⟵ `src/game.cpp:2233-2269`
  - [diverged cross-file w=0.88 line=0.33] `clients/silencer/src/world/network/world_network.cpp:199-210` ⟵ `src/world.cpp:306-317`
  - [diverged cross-file w=0.87 line=0.40] `clients/silencer/src/world/network/world_network.cpp:45-52` ⟵ `src/world.cpp:89-96`
  - [diverged cross-file w=0.86 line=0.48] `clients/silencer/src/world/physics/hittable.cpp:64-91` ⟵ `src/hittable.cpp:58-79`
  - [diverged cross-file w=0.86 line=0.71] `clients/silencer/src/world/map/map.cpp:58-96` ⟵ `src/map.cpp:54-84`
  - [diverged cross-file w=0.86 line=0.31] `clients/silencer/src/world/network/world_network.cpp:819-841` ⟵ `src/world.cpp:1527-1547`
  - [diverged cross-file w=0.85 line=0.54] `clients/silencer/src/game/loop/game_loop.cpp:294-320` ⟵ `src/game.cpp:620-644`
  - [diverged cross-file w=0.79 line=0.15] `clients/silencer/src/world/network/world_network.cpp:844-869` ⟵ `src/world.cpp:1550-1567`
  - [diverged cross-file w=0.79 line=0.65] `clients/silencer/src/game/session/map_downloader.cpp:101-130` ⟵ `src/game.cpp:5479-5511`
  - [diverged cross-file w=0.79 line=0.73] `clients/silencer/src/main.cpp:192-275` ⟵ `src/main.cpp:157-224`
  - [diverged cross-file w=0.75 line=0.58] `clients/silencer/src/world/messaging/world_messaging.cpp:35-53` ⟵ `src/world.cpp:1410-1428`
  - [diverged cross-file w=0.74 line=0.78] `clients/silencer/src/game/init/game_init.cpp:96-136` ⟵ `src/game.cpp:131-177`
  - [diverged cross-file w=0.72 line=0.05] `clients/silencer/src/game/session/game_session.cpp:36-61` ⟵ `src/game.cpp:2202-2220`
  - [diverged cross-file w=0.72 line=0.43] `clients/silencer/src/world/network/world_replication.cpp:89-126` ⟵ `src/world.cpp:910-971`
  - [diverged cross-file w=0.67 line=0.67] `clients/silencer/src/actors/player/player.cpp:2110-2156` ⟵ `src/player.cpp:2067-2112`
  - [diverged cross-file w=0.66 line=0.47] `clients/silencer/src/actors/player/player.cpp:4418-4462` ⟵ `src/player.cpp:4273-4338`
  - [diverged cross-file w=0.66 line=0.31] `clients/silencer/src/game/loop/game_loop.cpp:36-45` ⟵ `src/game.cpp:517-524`
  - [diverged cross-file w=0.66 line=0.56] `clients/silencer/src/world/network/world_network.cpp:694-711` ⟵ `src/world.cpp:761-778`
  - [diverged cross-file w=0.62 line=0.53] `clients/silencer/src/actors/player/player.cpp:455-470` ⟵ `src/player.cpp:385-399`
  - [diverged cross-file w=0.61 line=0.67] `clients/silencer/src/net/lobby.cpp:848-867` ⟵ `src/lobby.cpp:571-589`

## El-EnderJ/NeoCalculator — 322 коммитов | циклы=1(SCC≤3) | копипаст=1
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/El-EnderJ_NeoCalculator`

### `7a1a415a1405` — "docs: Add comprehensive migration guide for NumOS to STIX Two Math and a little head start" (El-EnderJ, 2026-05-30) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/El-EnderJ_NeoCalculator show 7a1a415a1405`
  - [near within-file w=0.92 line=0.85] `src/SystemApp.cpp:352-424` ⟵ `src/SystemApp.cpp:173-236`

## alphaonex86/CatchChallenger — 301 коммитов | циклы=0(SCC≤1) | копипаст=121

### `ea5021f64826` — "[Improvement] bot crash surfacing: BOT_ABORT macro + gdb wrap in benchmark/test" (BRULE Herman, 2026-05-29) [121 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/alphaonex86_CatchChallenger show ea5021f64826`
  - [diverged within-file w=1.00 line=0.75] `client/libcatchchallenger/Api_protocol_message.cpp:1419-1464` ⟵ `client/libcatchchallenger/Api_protocol_message.cpp:1675-1720`
  - [near within-file w=1.00 line=0.88] `client/qtcpu800x600/ultimate/mainwindow.cpp:1547-1586` ⟵ `client/qtcpu800x600/ultimate/mainwindow.cpp:1396-1435`
  - [near within-file w=1.00 line=0.93] `general/base/ProtocolParsingInput.cpp:309-384` ⟵ `general/base/ProtocolParsingInput.cpp:226-301`
  - [near within-file w=1.00 line=0.91] `general/base/ProtocolParsingInput.cpp:612-637` ⟵ `general/base/ProtocolParsingInput.cpp:520-545`
  - [near within-file w=1.00 line=0.91] `general/base/ProtocolParsingInput.cpp:641-665` ⟵ `general/base/ProtocolParsingInput.cpp:549-573`
  - [verbatim cross-file w=1.00 line=1.00] `general/tinyXML2/tinyxml2.cpp:1734-1740` ⟵ `general/tinyXML2/tinyxml2b.cpp:658-664`
  - [verbatim cross-file w=1.00 line=1.00] `general/tinyXML2/tinyxml2.cpp:2148-2163` ⟵ `general/tinyXML2/tinyxml2c.cpp:300-315`
  - [verbatim within-file w=1.00 line=1.00] `general/tinyXML2/tinyxml2.cpp:2172-2177` ⟵ `general/tinyXML2/tinyxml2.cpp:687-692`
  - [near cross-file w=1.00 line=0.92] `tools/bot-test-connect-to-gameserver-cli/main.cpp:26-53` ⟵ `tools/bot-test-connect-to-gameserver/main.cpp:26-53`
  - [verbatim cross-file w=1.00 line=1.00] `general/tinyXML2/tinyxml2.cpp:2639-2656` ⟵ `general/tinyXML2/tinyxml2c.cpp:723-740`
  - [verbatim cross-file w=1.00 line=1.00] `general/tinyXML2/tinyxml2.cpp:3001-3012` ⟵ `general/tinyXML2/tinyxml2c.cpp:1010-1021`
  - [near cross-file w=1.00 line=0.88] `server/base/BaseServer/BaseServer2.cpp:319-335` ⟵ `server/base/BaseServer2.cpp:287-303`
  - [verbatim cross-file w=1.00 line=1.00] `server/base/BaseServer/BaseServerLoadSQL.cpp:599-613` ⟵ `server/base/BaseServerLoadSQL.cpp:518-532`
  - [verbatim cross-file w=1.00 line=1.00] `server/base/BaseServer/BaseServerUnload.cpp:21-45` ⟵ `server/base/BaseServerUnload.cpp:21-45`
  - [verbatim cross-file w=1.00 line=1.00] `server/base/ClientEvents/LocalClientHandlerClan.cpp:649-666` ⟵ `server/base/LocalClientHandlerClan.cpp:505-522`
  - [verbatim cross-file w=1.00 line=1.00] `tools/libbot/MultipleBotConnection.cpp:19-38` ⟵ `tools/bot/MultipleBotConnection.cpp:18-37`
  - [verbatim cross-file w=1.00 line=1.00] `server/base/ClientEvents/LocalClientHandlerQuest.cpp:343-353` ⟵ `server/base/LocalClientHandlerQuest.cpp:375-385`
  - [near cross-file w=1.00 line=0.91] `server/base/ClientLoad/ClientHeavyLoadLogin.cpp:695-722` ⟵ `server/base/ClientHeavyLoadLogin.cpp:648-675`
  - [verbatim cross-file w=1.00 line=1.00] `server/cli/db/EventLoopPostgresql.cpp:697-727` ⟵ `server/epoll/db/EpollPostgresql.cpp:666-696`
  - [verbatim cross-file w=1.00 line=1.00] `server/cli/main-unix.cpp:1048-1056` ⟵ `server/epoll/main-epoll.cpp:916-924`
  - [near cross-file w=1.00 line=0.83] `server/cli/timer/TimerDdos.cpp:16-28` ⟵ `server/epoll/timer/TimerDdos.cpp:16-28`
  - [verbatim cross-file w=1.00 line=1.00] `server/qt/gui/GUIServer.cpp:298-304` ⟵ `server/qt/NormalServer.cpp:238-244`
  - [verbatim cross-file w=1.00 line=1.00] `server/qt/gui/GUIServer.cpp:268-272` ⟵ `server/qt/NormalServer.cpp:208-212`
  - [diverged cross-file w=1.00 line=0.47] `server/master/EventLoopClientLoginMaster.cpp:48-99` ⟵ `server/master/EpollClientLoginMaster.cpp:44-95`
  - [near cross-file w=1.00 line=0.83] `server/master/EventLoopClientLoginMaster.cpp:103-156` ⟵ `server/master/EpollClientLoginMaster.cpp:99-152`
  - [diverged cross-file w=1.00 line=0.37] `server/master/EventLoopClientLoginMasterProtocolParsing.cpp:23-74` ⟵ `server/master/EpollClientLoginMasterProtocolParsing.cpp:22-73`
  - [near cross-file w=1.00 line=0.91] `server/master/EventLoopClientLoginMasterProtocolParsing.cpp:910-941` ⟵ `server/master/EpollClientLoginMasterProtocolParsing.cpp:901-932`
  - [near cross-file w=1.00 line=0.92] `server/master/EventLoopClientLoginMasterProtocolParsing.cpp:969-1001` ⟵ `server/master/EpollClientLoginMasterProtocolParsing.cpp:960-992`
  - [verbatim cross-file w=1.00 line=1.00] `server/master/EventLoopClientLoginMasterProtocolParsing.cpp:1007-1015` ⟵ `server/master/EpollClientLoginMasterProtocolParsing.cpp:998-1006`
  - [verbatim cross-file w=1.00 line=1.00] `server/master/EventLoopServerLoginMaster.cpp:1038-1046` ⟵ `server/master/EpollServerLoginMaster.cpp:1023-1031`
  - [near within-file w=1.00 line=0.83] `server/qt/db/QtDatabase.cpp:278-291` ⟵ `server/qt/db/QtDatabase.cpp:232-245`
  - [diverged within-file w=1.00 line=0.71] `tools/map-procedural-generation-terrain/main.cpp:107-112` ⟵ `tools/map-procedural-generation-terrain/main.cpp:99-104`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/MapControllerMP.cpp:1075-1090` ⟵ `client/qtmaprender/MapControllerMP.cpp:1005-1020`
  - [near cross-file w=1.00 line=0.95] `client/libqtcatchchallenger/maprender/MapItem.cpp:91-143` ⟵ `client/qtmaprender/MapItem.cpp:57-109`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/MapControllerMP.cpp:1055-1071` ⟵ `client/qtmaprender/MapControllerMP.cpp:985-1001`
  - [verbatim cross-file w=1.00 line=1.00] `tools/libbot/actions/ActionsMapStep1.cpp:16-25` ⟵ `tools/bot/actions/ActionsMapStep1.cpp:15-24`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/MapVisualiserPlayer.cpp:1100-1111` ⟵ `client/qtmaprender/MapVisualiserPlayer.cpp:1169-1180`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/MapVisualiserPlayer.cpp:2214-2250` ⟵ `client/qtmaprender/MapVisualiserPlayer.cpp:2370-2406`
  - [near cross-file w=1.00 line=0.88] `tools/libbot/MultipleBotConnectionImplForGui.cpp:501-518` ⟵ `tools/bot/MultipleBotConnectionImplForGui.cpp:497-514`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/MapVisualiserThread.cpp:171-177` ⟵ `client/qtmaprender/MapVisualiserThread.cpp:280-286`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/ObjectGroupItem.cpp:14-26` ⟵ `client/qtmaprender/ObjectGroupItem.cpp:14-26`
  - [verbatim cross-file w=1.00 line=1.00] `tools/libbot/actions/ActionsMapStep1.cpp:42-67` ⟵ `tools/bot/actions/ActionsMapStep1.cpp:41-66`
  - [verbatim cross-file w=1.00 line=1.00] `tools/libbot/MultipleBotConnectionImplForGui.cpp:330-340` ⟵ `tools/bot/MultipleBotConnectionImplForGui.cpp:326-336`
  - [near cross-file w=1.00 line=0.88] `tools/libbot/MultipleBotConnectionImplForGui.cpp:304-326` ⟵ `tools/bot/MultipleBotConnectionImplForGui.cpp:300-322`
  - [verbatim cross-file w=1.00 line=1.00] `tools/libbot/MultipleBotConnectionImplForGui.cpp:239-260` ⟵ `tools/bot/MultipleBotConnectionImplForGui.cpp:235-256`
  - [verbatim cross-file w=1.00 line=1.00] `client/libqtcatchchallenger/maprender/MapController.cpp:576-615` ⟵ `client/qtmaprender/MapController.cpp:594-633`
  - [diverged within-file w=1.00 line=0.67] `tools/map-procedural-generation-terrain/main.cpp:95-99` ⟵ `tools/map-procedural-generation-terrain/main.cpp:87-91`
  - [near within-file w=0.99 line=0.91] `client/qtcpu800x600/fight/interface/BaseWindowFight.cpp:1696-1736` ⟵ `client/qtcpu800x600/fight/interface/BaseWindowFight.cpp:1764-1804`
  - [near within-file w=0.99 line=0.92] `client/qtcpu800x600/fight/interface/BaseWindowFight.cpp:645-678` ⟵ `client/qtcpu800x600/fight/interface/BaseWindowFight.cpp:712-745`
  - [diverged cross-file w=0.99 line=0.64] `server/master/EventLoopServerLoginMaster.cpp:54-86` ⟵ `server/master/EpollServerLoginMaster.cpp:46-80`
  - [diverged cross-file w=0.96 line=0.67] `client/libqtcatchchallenger/maprender/MapControllerMP.cpp:469-482` ⟵ `client/qtmaprender/MapControllerMP.cpp:435-448`
  - [near cross-file w=0.96 line=0.86] `server/cli/db/EventLoopMySQL.cpp:122-157` ⟵ `server/epoll/db/EpollMySQL.cpp:91-126`
  - [near cross-file w=0.96 line=0.88] `server/cli/db/EventLoopPostgresql.cpp:155-199` ⟵ `server/epoll/db/EpollPostgresql.cpp:154-198`
  - [near within-file w=0.96 line=0.82] `tools/bot-actions/BotTargetList.cpp:1822-1845` ⟵ `tools/bot-actions/BotTargetList.cpp:1847-1870`
  - [near within-file w=0.96 line=0.82] `tools/bot-actions/BotTargetList.cpp:1849-1872` ⟵ `tools/bot-actions/BotTargetList.cpp:1820-1843`
  - [diverged cross-file w=0.95 line=0.58] `general/tinyXML2/tinyxml2.cpp:2103-2125` ⟵ `general/tinyXML2/tinyxml2c.cpp:255-277`
  - [verbatim cross-file w=0.95 line=0.95] `tools/libbot/MultipleBotConnection.cpp:203-236` ⟵ `tools/bot/MultipleBotConnection.cpp:203-233`
  - [diverged cross-file w=0.95 line=0.57] `general/tinyXML2/tinyxml2.cpp:2579-2591` ⟵ `general/tinyXML2/tinyxml2c.cpp:684-694`
  - [near cross-file w=0.95 line=0.93] `tools/libbot/MultipleBotConnection.cpp:580-618` ⟵ `tools/bot/MultipleBotConnection.cpp:540-578`
  - [near cross-file w=0.94 line=0.88] `tools/libbot/MultipleBotConnection.cpp:550-576` ⟵ `tools/bot/MultipleBotConnection.cpp:510-536`
  - [near cross-file w=0.94 line=0.90] `server/base/BaseServer/BaseServer2.cpp:235-276` ⟵ `server/base/BaseServer2.cpp:212-253`
  - [near cross-file w=0.94 line=0.91] `server/cli/db/EventLoopPostgresql.cpp:427-453` ⟵ `server/epoll/db/EpollPostgresql.cpp:426-452`
  - [diverged cross-file w=0.94 line=0.55] `general/tinyXML2/tinyxml2.cpp:1260-1282` ⟵ `general/tinyXML2/tinyxml2b.cpp:271-294`
  - [near cross-file w=0.94 line=0.88] `server/base/BaseServer/BaseServerLoad.cpp:43-82` ⟵ `server/base/BaseServerLoad.cpp:40-79`
  - [near cross-file w=0.93 line=0.94] `tools/libbot/actions/ActionsAction.cpp:1169-1210` ⟵ `tools/bot/actions/ActionsAction.cpp:1186-1227`
  - [near within-file w=0.92 line=0.92] `tools/bot-actions/SocialChat.cpp:811-822` ⟵ `tools/bot-actions/SocialChat.cpp:767-777`
  - [near within-file w=0.92 line=0.87] `tools/bot-actions/MainWindow.cpp:708-722` ⟵ `tools/bot-actions/MainWindow.cpp:642-654`
  - [near cross-file w=0.92 line=0.88] `tools/libbot/MultipleBotConnection.cpp:525-546` ⟵ `tools/bot/MultipleBotConnection.cpp:485-506`
  - [diverged cross-file w=0.91 line=0.63] `tools/libbot/MultipleBotConnection.cpp:367-415` ⟵ `tools/bot/MultipleBotConnection.cpp:341-375`
  - [diverged cross-file w=0.90 line=0.69] `client/libqtcatchchallenger/maprender/MapVisualiserPlayer.cpp:369-402` ⟵ `client/qtmaprender/MapVisualiserPlayer.cpp:332-360`
  - [near cross-file w=0.89 line=0.90] `tools/libbot/MultipleBotConnection.cpp:145-170` ⟵ `tools/bot/MultipleBotConnection.cpp:144-169`
  - [diverged cross-file w=0.88 line=0.59] `client/libqtcatchchallenger/maprender/MapControllerMPMove.cpp:49-77` ⟵ `client/qtmaprender/MapControllerMPMove.cpp:48-76`
  - [diverged cross-file w=0.88 line=0.52] `server/base/ClientEvents/LocalClientHandlerTrade.cpp:86-163` ⟵ `server/base/LocalClientHandlerTrade.cpp:84-160`
  - [near within-file w=0.87 line=0.92] `tools/bot-actions/MainWindow.cpp:305-335` ⟵ `tools/bot-actions/MainWindow.cpp:242-272`
  - [diverged cross-file w=0.87 line=0.73] `client/libqtcatchchallenger/maprender/MapControllerMP.cpp:1034-1051` ⟵ `tools/map-visualiser/MapControllerV.cpp:499-516`
  - [diverged cross-file w=0.87 line=0.75] `server/master/EventLoopServerLoginMaster.cpp:538-580` ⟵ `server/master/EpollServerLoginMaster.cpp:536-572`
  - [near within-file w=0.86 line=0.80] `client/qtopengl/foreground/OverMapLogic.cpp:1592-1609` ⟵ `client/qtopengl/foreground/OverMapLogic.cpp:1625-1637`
  - [near cross-file w=0.86 line=0.80] `client/libqtcatchchallenger/maprender/MapVisualiserPlayer.cpp:1384-1406` ⟵ `client/qtmaprender/MapVisualiserPlayer.cpp:1460-1479`
  - [diverged within-file w=0.86 line=0.74] `client/qtcpu800x600/ultimate/mainwindow.cpp:1605-1681` ⟵ `client/qtcpu800x600/ultimate/mainwindow.cpp:1477-1538`
  - [near cross-file w=0.86 line=0.80] `client/libqtcatchchallenger/maprender/MapVisualiserPlayer.cpp:173-195` ⟵ `client/qtmaprender/MapVisualiserPlayer.cpp:136-155`
  - [diverged cross-file w=0.85 line=0.74] `tools/libbot/actions/ActionsMapStep1.cpp:99-126` ⟵ `tools/bot/actions/ActionsMapStep1.cpp:97-124`
  - [diverged cross-file w=0.85 line=0.77] `client/qtopengl/foreground/Multi.cpp:425-448` ⟵ `client/qtcpu800x600/ultimate/mainwindow.cpp:1931-1954`
  - [diverged cross-file w=0.85 line=0.58] `server/base/ClientEvents/LocalClientHandlerCommand.cpp:331-411` ⟵ `server/base/LocalClientHandlerCommand.cpp:305-377`
  - [diverged cross-file w=0.85 line=0.64] `general/tinyXML2/tinyxml2.cpp:2251-2280` ⟵ `general/tinyXML2/tinyxml2c.cpp:384-411`
  - [diverged within-file w=0.85 line=0.62] `server/base/Client.cpp:505-519` ⟵ `server/base/Client.cpp:349-363`
  - [near cross-file w=0.84 line=0.83] `tools/libbot/MultipleBotConnectionImplForGui.cpp:195-219` ⟵ `tools/bot/MultipleBotConnectionImplForGui.cpp:192-215`
  - [diverged cross-file w=0.82 line=0.76] `server/login/EventLoopClientLoginSlaveHeavyLoad.cpp:735-797` ⟵ `server/login/EpollClientLoginSlaveHeavyLoad.cpp:738-800`
  - [diverged cross-file w=0.81 line=0.75] `server/base/ClientLoad/ClientHeavyLoadMirror.cpp:197-223` ⟵ `server/base/ClientHeavyLoadMirror.cpp:109-125`
  - [diverged within-file w=0.80 line=0.45] `server/fight/LocalClientHandlerFightBattle.cpp:95-111` ⟵ `server/fight/LocalClientHandlerFightBattle.cpp:79-93`
  - [diverged cross-file w=0.79 line=0.26] `client/libqtcatchchallenger/maprender/MapControllerMP.cpp:574-621` ⟵ `client/qtmaprender/MapControllerMP.cpp:522-554`
  - [diverged cross-file w=0.79 line=0.79] `tools/libbot/MultipleBotConnection.cpp:454-521` ⟵ `tools/bot/MultipleBotConnection.cpp:414-481`
  - [diverged cross-file w=0.79 line=0.53] `server/base/ClientEvents/LocalClientHandlerCommand.cpp:279-328` ⟵ `server/base/LocalClientHandlerCommand.cpp:256-302`
  - [diverged cross-file w=0.78 line=0.55] `server/base/ClientEvents/LocalClientHandlerCity.cpp:469-513` ⟵ `server/base/LocalClientHandlerCity.cpp:418-460`
  - [near cross-file w=0.78 line=0.92] `server/base/ClientEvents/LocalClientHandlerObject.cpp:37-73` ⟵ `server/base/LocalClientHandlerObject.cpp:33-69`
  - [diverged within-file w=0.78 line=0.67] `server/gateway/LinkToGameServerProtocolParsing.cpp:1184-1214` ⟵ `server/gateway/LinkToGameServerProtocolParsing.cpp:1152-1182`
  - [diverged cross-file w=0.78 line=0.74] `client/qtcpu800x600/base/interface/BaseWindowBot.cpp:249-291` ⟵ `client/qtopengl/foreground/OverMapLogicBot.cpp:242-282`
  - [diverged cross-file w=0.78 line=0.69] `client/qtopengl/foreground/OverMapLogicBot.cpp:231-271` ⟵ `client/qtcpu800x600/base/interface/BaseWindowBot.cpp:252-294`
  - [diverged cross-file w=0.78 line=0.40] `server/master/PlayerUpdaterToLogin.cpp:15-65` ⟵ `server/game-server-alone/timer/PlayerUpdaterToMaster.cpp:18-64`
  - [diverged within-file w=0.78 line=0.68] `server/fight/LocalClientHandlerFight.cpp:666-712` ⟵ `server/fight/LocalClientHandlerFight.cpp:618-659`
  - [diverged cross-file w=0.78 line=0.76] `server/base/ClientEvents/LocalClientHandlerQuest.cpp:14-46` ⟵ `server/base/LocalClientHandlerQuest.cpp:13-47`
  - [diverged cross-file w=0.77 line=0.42] `client/libqtcatchchallenger/maprender/MapVisualiserPlayerWithFight.cpp:102-137` ⟵ `client/qtmaprender/MapVisualiserPlayerWithFight.cpp:128-168`
  - [diverged cross-file w=0.76 line=0.57] `server/base/BaseServer/BaseServer.cpp:375-390` ⟵ `server/base/BaseServer.cpp:367-388`
  - [diverged within-file w=0.76 line=0.74] `server/fight/LocalClientHandlerFight.cpp:622-646` ⟵ `server/fight/LocalClientHandlerFight.cpp:586-603`
  - [diverged within-file w=0.75 line=0.64] `general/tinyXML2/tinyxml2.cpp:259-274` ⟵ `general/tinyXML2/tinyxml2.cpp:208-223`
  - [diverged cross-file w=0.75 line=0.70] `server/login/EventLoopServerLoginSlave.cpp:478-495` ⟵ `server/login/EpollServerLoginSlave.cpp:496-517`
  - [diverged cross-file w=0.74 line=0.30] `tools/libbot/MultipleBotConnection.cpp:259-313` ⟵ `tools/bot/MultipleBotConnection.cpp:341-375`
  - [diverged cross-file w=0.73 line=0.64] `server/qt/db/QtDatabaseMySQL.cpp:19-58` ⟵ `server/qt/db/QtDatabasePostgreSQL.cpp:18-59`
  - [diverged cross-file w=0.73 line=0.64] `server/qt/db/QtDatabasePostgreSQL.cpp:18-59` ⟵ `server/qt/db/QtDatabaseMySQL.cpp:19-58`
  - [diverged within-file w=0.71 line=0.65] `client/qtcpu800x600/base/interface/BaseWindow.cpp:2020-2038` ⟵ `client/qtcpu800x600/base/interface/BaseWindow.cpp:1997-2015`
  - [diverged cross-file w=0.70 line=0.38] `general/tinyXML2/tinyxml2.cpp:1122-1209` ⟵ `general/tinyXML2/tinyxml2b.cpp:179-245`
  - [diverged cross-file w=0.69 line=0.78] `tools/bot-actions/SocialChat.cpp:115-126` ⟵ `tools/bot-actions/GenerateMapZone.cpp:47-54`
  - [diverged cross-file w=0.68 line=0.79] `client/libqtcatchchallenger/InternetUpdater.cpp:68-121` ⟵ `client/qtcpu800x600/base/InternetUpdater.cpp:68-114`
  - [diverged within-file w=0.67 line=0.65] `client/qtcpu800x600/fight/interface/BaseWindowFight.cpp:1431-1462` ⟵ `client/qtcpu800x600/fight/interface/BaseWindowFight.cpp:1501-1530`
  - [diverged cross-file w=0.67 line=0.79] `client/libqtcatchchallenger/LanBroadcastWatcher.cpp:27-70` ⟵ `client/qtcpu800x600/base/LanBroadcastWatcher.cpp:27-63`
  - [diverged cross-file w=0.67 line=0.79] `client/qtcpu800x600/base/LanBroadcastWatcher.cpp:27-70` ⟵ `client/libqtcatchchallenger/LanBroadcastWatcher.cpp:27-63`
  - [diverged cross-file w=0.67 line=0.64] `client/libqtcatchchallenger/maprender/MapControllerMP.cpp:425-465` ⟵ `client/qtmaprender/MapControllerMP.cpp:394-431`
  - [diverged cross-file w=0.67 line=0.74] `client/qtopengl/foreground/OverMapLogicBot.cpp:677-721` ⟵ `client/qtcpu800x600/base/interface/BaseWindowBot.cpp:735-781`
  - [diverged cross-file w=0.67 line=0.74] `client/qtcpu800x600/base/interface/BaseWindowBot.cpp:726-770` ⟵ `client/qtopengl/foreground/OverMapLogicBot.cpp:696-742`
  - [diverged cross-file w=0.66 line=0.55] `general/tinyXML2/tinyxml2.cpp:2446-2460` ⟵ `general/tinyXML2/tinyxml2c.cpp:579-587`
  - [diverged cross-file w=0.66 line=0.54] `general/tinyXML2/tinyxml2.cpp:1986-2041` ⟵ `general/tinyXML2/tinyxml2c.cpp:183-236`
  - [diverged within-file w=0.64 line=0.67] `client/qtopengl/foreground/Multi.cpp:868-903` ⟵ `client/qtopengl/foreground/Multi.cpp:842-867`

## lyquid/Romulus — 250 коммитов | циклы=0(SCC≤1) | копипаст=3

### `8d0f008a390f` — "Merge pull request #124 from lyquid/copilot/define-match-priority-rules" (Alex CB, 2026-04-19) [3 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/lyquid_Romulus show 8d0f008a390f`
  - [diverged within-file w=1.00 line=0.67] `lib/romulus/database/database.cpp:429-444` ⟵ `lib/romulus/database/database.cpp:292-309`
  - [diverged within-file w=0.86 line=0.67] `lib/romulus/scanner/hash_service.cpp:78-86` ⟵ `lib/romulus/scanner/hash_service.cpp:66-73`
  - [diverged within-file w=0.72 line=0.45] `lib/romulus/dat/dat_parser.cpp:183-221` ⟵ `lib/romulus/dat/dat_parser.cpp:116-141`

## AmoghS1234/CipherMesh — 244 коммитов | циклы=0(SCC≤1) | копипаст=9

### `aff36867e245` — "Created new release" (Amogh S, 2026-05-29) [9 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/AmoghS1234_CipherMesh show aff36867e245`
  - [diverged within-file w=1.00 line=0.50] `src/desktop/mainwindow.cpp:1763-1773` ⟵ `src/desktop/mainwindow.cpp:1437-1449`
  - [diverged within-file w=1.00 line=0.78] `src/p2p_webrtc/webrtcservice.cpp:679-686` ⟵ `src/p2p_webrtc/webrtcservice.cpp:412-419`
  - [diverged within-file w=0.98 line=0.67] `src/desktop/mainwindow.cpp:273-279` ⟵ `src/desktop/mainwindow.cpp:215-226`
  - [near within-file w=0.94 line=0.82] `src/desktop/mainwindow.cpp:1812-1844` ⟵ `src/desktop/mainwindow.cpp:1489-1521`
  - [near within-file w=0.94 line=0.94] `src/desktop/settingsdialog.cpp:251-291` ⟵ `src/desktop/settingsdialog.cpp:208-248`
  - [diverged within-file w=0.82 line=0.75] `src/desktop/mainwindow.cpp:282-301` ⟵ `src/desktop/mainwindow.cpp:229-245`
  - [diverged within-file w=0.72 line=0.63] `src/desktop/mainwindow.cpp:936-1004` ⟵ `src/desktop/mainwindow.cpp:778-835`
  - [diverged within-file w=0.70 line=0.20] `src/core/database.cpp:92-101` ⟵ `src/core/database.cpp:22-31`
  - [diverged within-file w=0.65 line=0.08] `src/core/database.cpp:115-121` ⟵ `src/core/database.cpp:89-96`

## remiserriere/AI-on-the-edge-device — 234 коммитов | циклы=1(SCC≤3) | копипаст=30
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/remiserriere_AI-on-the-edge-device`

### `0bc308c9b6c4` — "Merge pull request #26 from remiserriere/copilot/fix-had-custom-topic-usage" (Remi Serriere, 2026-02-08) [30 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/remiserriere_AI-on-the-edge-device show 0bc308c9b6c4`
  - [diverged within-file w=1.00 line=0.07] `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:62-104` ⟵ `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:23-59`
  - [diverged within-file w=1.00 line=0.07] `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:23-59` ⟵ `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:62-104`
  - [verbatim cross-file w=1.00 line=1.00] `code/components/jomjol_webhook/interface_webhook.cpp:139-168` ⟵ `code/components/jomjol_influxdb/interface_influxdb.cpp:102-131`
  - [diverged within-file w=1.00 line=0.79] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:204-230` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:127-154`
  - [verbatim within-file w=1.00 line=1.00] `code/components/jomjol_helper/Helper.cpp:1204-1229` ⟵ `code/components/jomjol_helper/Helper.cpp:1042-1067`
  - [diverged within-file w=1.00 line=0.12] `code/components/jomjol_helper/Helper.cpp:1121-1162` ⟵ `code/components/jomjol_helper/Helper.cpp:981-1000`
  - [diverged within-file w=1.00 line=0.44] `code/components/jomjol_helper/Helper.cpp:551-624` ⟵ `code/components/jomjol_helper/Helper.cpp:478-531`
  - [diverged within-file w=1.00 line=0.66] `code/components/jomjol_flowcontroll/MainFlowControl.cpp:1750-1812` ⟵ `code/components/jomjol_flowcontroll/MainFlowControl.cpp:1011-1068`
  - [diverged within-file w=1.00 line=0.79] `code/components/jomjol_flowcontroll/MainFlowControl.cpp:961-979` ⟵ `code/components/jomjol_flowcontroll/MainFlowControl.cpp:634-650`
  - [diverged within-file w=1.00 line=0.56] `code/components/jomjol_flowcontroll/MainFlowControl.cpp:339-376` ⟵ `code/components/jomjol_flowcontroll/MainFlowControl.cpp:145-179`
  - [diverged within-file w=1.00 line=0.48] `code/components/jomjol_controlGPIO/SmartLeds.h:262-286` ⟵ `code/components/jomjol_controlGPIO/SmartLeds.h:478-502`
  - [diverged within-file w=1.00 line=0.54] `code/components/jomjol_controlGPIO/SmartLeds.h:390-414` ⟵ `code/components/jomjol_controlGPIO/SmartLeds.h:478-502`
  - [diverged within-file w=1.00 line=0.75] `code/components/jomjol_flowcontroll/ClassFlowMQTT.cpp:232-251` ⟵ `code/components/jomjol_flowcontroll/ClassFlowMQTT.cpp:203-222`
  - [diverged within-file w=0.99 line=0.35] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:620-637` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:584-604`
  - [diverged within-file w=0.99 line=0.48] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:856-932` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:629-690`
  - [diverged within-file w=0.99 line=0.49] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:753-852` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:543-624`
  - [near within-file w=0.99 line=0.82] `code/components/jomjol_flowcontroll/ClassFlowTakeImage.cpp:600-610` ⟵ `code/components/jomjol_flowcontroll/ClassFlowTakeImage.cpp:294-304`
  - [diverged within-file w=0.99 line=0.28] `code/components/jomjol_flowcontroll/ClassFlowPostProcessing.cpp:994-1018` ⟵ `code/components/jomjol_flowcontroll/ClassFlowPostProcessing.cpp:956-978`
  - [diverged within-file w=0.98 line=0.74] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:551-580` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:504-531`
  - [diverged within-file w=0.98 line=0.59] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:488-529` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:426-463`
  - [diverged within-file w=0.98 line=0.62] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:1025-1065` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:763-793`
  - [diverged within-file w=0.95 line=0.63] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:341-385` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:269-315`
  - [near within-file w=0.94 line=0.83] `code/main/main.cpp:943-1007` ⟵ `code/main/main.cpp:753-816`
  - [diverged within-file w=0.93 line=0.40] `code/components/jomjol_flowcontroll/ClassFlowAlignment.cpp:220-230` ⟵ `code/components/jomjol_flowcontroll/ClassFlowAlignment.cpp:234-241`
  - [near within-file w=0.92 line=0.94] `code/main/main.cpp:337-357` ⟵ `code/main/main.cpp:268-287`
  - [diverged within-file w=0.87 line=0.68] `code/components/jomjol_helper/sdcard_init.c:375-441` ⟵ `code/components/jomjol_helper/sdcard_init.c:463-542`
  - [diverged within-file w=0.87 line=0.68] `code/components/jomjol_helper/sdcard_init.c:458-537` ⟵ `code/components/jomjol_helper/sdcard_init.c:380-446`
  - [diverged within-file w=0.80 line=0.64] `code/components/jomjol_flowcontroll/ClassFlowTakeImage.cpp:543-587` ⟵ `code/components/jomjol_flowcontroll/ClassFlowTakeImage.cpp:242-279`
  - [diverged within-file w=0.65 line=0.33] `code/components/jomjol_flowcontroll/ClassFlowAlignment.cpp:242-249` ⟵ `code/components/jomjol_flowcontroll/ClassFlowAlignment.cpp:256-262`
  - [diverged within-file w=0.65 line=0.12] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:573-592` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:375-395`

## philstopford/cine-encoder — 234 коммитов | циклы=0(SCC≤1) | копипаст=6

### `74f803e700e7` — "Update README.md" (philstopford, 2026-04-16) [6 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/philstopford_cine-encoder show 74f803e700e7`
  - [diverged within-file w=0.91 line=0.26] `app/mainwindow.cpp:3696-3719` ⟵ `app/mainwindow.cpp:4671-4699`
  - [diverged within-file w=0.88 line=0.28] `app/mainwindow.cpp:1088-1120` ⟵ `app/mainwindow.cpp:649-673`
  - [diverged within-file w=0.87 line=0.65] `app/mainwindow.cpp:3539-3551` ⟵ `app/mainwindow.cpp:4549-4563`
  - [diverged within-file w=0.74 line=0.27] `app/mainwindow.cpp:2951-2975` ⟵ `app/mainwindow.cpp:3417-3436`
  - [diverged within-file w=0.74 line=0.27] `app/mainwindow.cpp:3104-3109` ⟵ `app/mainwindow.cpp:3562-3570`
  - [diverged within-file w=0.73 line=0.35] `app/mainwindow.cpp:3461-3495` ⟵ `app/mainwindow.cpp:4458-4499`

## hammermaps/halflife-op4-updated-sohl — 222 коммитов | циклы=0(SCC≤1) | копипаст=123

### `d6c1fa262066` — "Merge pull request #59 from hammermaps/copilot/compare-lrc-bmodels-changes" (Lucas, 2026-04-07) [123 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/hammermaps_halflife-op4-updated-sohl show d6c1fa262066`
  - [diverged cross-file w=1.00 line=0.33] `utils/common/cmdlib.cpp:688-693` ⟵ `utils/common/cmdlib.c:750-755`
  - [diverged cross-file w=1.00 line=0.57] `utils/qcsg/qcsg.cpp:399-488` ⟵ `utils/qcsg/qcsg.c:402-495`
  - [diverged cross-file w=1.00 line=0.17] `utils/qcsg/qcsg.cpp:316-323` ⟵ `utils/qcsg/qcsg.c:319-326`
  - [diverged cross-file w=1.00 line=0.44] `utils/qcsg/gldraw.cpp:63-111` ⟵ `utils/qcsg/gldraw.c:37-86`
  - [diverged cross-file w=1.00 line=0.47] `utils/qcsg/brush.cpp:935-973` ⟵ `utils/qcsg/brush.c:943-984`
  - [diverged cross-file w=1.00 line=0.50] `utils/qbsp2/tjunc.cpp:451-468` ⟵ `utils/qbsp2/tjunc.c:454-471`
  - [diverged cross-file w=1.00 line=0.23] `utils/qbsp2/tjunc.cpp:437-447` ⟵ `utils/qbsp2/tjunc.c:440-450`
  - [diverged cross-file w=1.00 line=0.76] `dlls/squadmonster.cpp:454-513` ⟵ `dlls/COFSquadTalkMonster.cpp:520-579`
  - [diverged cross-file w=1.00 line=0.54] `utils/qbsp2/tjunc.cpp:130-191` ⟵ `utils/qbsp2/tjunc.c:131-192`
  - [diverged cross-file w=1.00 line=0.57] `utils/qbsp2/surfaces.cpp:260-316` ⟵ `utils/qbsp2/surfaces.c:264-322`
  - [diverged cross-file w=1.00 line=0.57] `utils/qbsp2/surfaces.cpp:103-128` ⟵ `utils/qbsp2/surfaces.c:103-128`
  - [diverged cross-file w=1.00 line=0.30] `utils/qbsp2/qbsp.cpp:900-933` ⟵ `utils/qbsp2/qbsp.c:897-930`
  - [diverged cross-file w=1.00 line=0.39] `utils/qbsp2/portals.cpp:405-426` ⟵ `utils/qbsp2/portals.c:412-433`
  - [diverged cross-file w=1.00 line=0.22] `utils/qbsp2/portals.cpp:381-398` ⟵ `utils/qbsp2/portals.c:388-405`
  - [diverged cross-file w=1.00 line=0.24] `utils/bspinfo/bspinfo.cpp:17-48` ⟵ `utils/bspinfo/bspinfo.c:17-48`
  - [diverged cross-file w=1.00 line=0.50] `utils/common/cmdlib.cpp:236-245` ⟵ `utils/common/cmdlib.c:298-307`
  - [diverged cross-file w=1.00 line=0.43] `utils/qbsp2/tjunc.cpp:478-517` ⟵ `utils/qbsp2/tjunc.c:481-520`
  - [diverged cross-file w=1.00 line=0.17] `utils/common/cmdlib.cpp:924-931` ⟵ `utils/common/cmdlib.c:979-986`
  - [diverged cross-file w=1.00 line=0.28] `utils/common/threads.cpp:223-273` ⟵ `utils/common/threads.c:223-274`
  - [diverged cross-file w=1.00 line=0.48] `utils/common/trilib.cpp:99-186` ⟵ `utils/common/trilib.c:94-174`
  - [diverged cross-file w=1.00 line=0.50] `utils/qbsp2/portals.cpp:338-370` ⟵ `utils/qbsp2/portals.c:343-377`
  - [diverged within-file w=1.00 line=0.40] `utils/mdlviewer/mdlviewer.cpp:159-165` ⟵ `utils/mdlviewer/mdlviewer.cpp:143-149`
  - [diverged within-file w=1.00 line=0.54] `utils/mdlviewer/mdlviewer.cpp:199-211` ⟵ `utils/mdlviewer/mdlviewer.cpp:179-191`
  - [diverged cross-file w=1.00 line=0.49] `utils/qbsp2/portals.cpp:207-251` ⟵ `utils/qbsp2/portals.c:209-254`
  - [diverged cross-file w=1.00 line=0.30] `utils/mkmovie/mkmovie.cpp:197-234` ⟵ `utils/mkmovie/mkmovie.c:197-234`
  - [diverged cross-file w=1.00 line=0.17] `utils/mkmovie/mkmovie.cpp:240-270` ⟵ `utils/mkmovie/mkmovie.c:240-270`
  - [diverged cross-file w=1.00 line=0.41] `utils/qbsp2/gldraw.cpp:83-130` ⟵ `utils/qbsp2/gldraw.c:61-109`
  - [diverged cross-file w=1.00 line=0.51] `utils/qbsp2/portals.cpp:99-151` ⟵ `utils/qbsp2/portals.c:100-152`
  - [diverged cross-file w=1.00 line=0.41] `utils/qbsp2/merge.cpp:180-203` ⟵ `utils/qbsp2/merge.c:181-204`
  - [diverged cross-file w=1.00 line=0.43] `utils/qbsp2/merge.cpp:239-252` ⟵ `utils/qbsp2/merge.c:240-253`
  - [diverged cross-file w=1.00 line=0.67] `utils/qbsp2/outside.cpp:127-170` ⟵ `utils/qbsp2/outside.c:129-172`
  - [diverged cross-file w=1.00 line=0.42] `utils/qrad/lightmap.cpp:1620-1645` ⟵ `utils/qrad/lightmap.c:1644-1669`
  - [diverged cross-file w=1.00 line=0.16] `utils/studiomdl/write.cpp:547-573` ⟵ `utils/studiomdl/write.c:540-566`
  - [diverged cross-file w=1.00 line=0.24] `utils/sprgen/sprgen.cpp:561-614` ⟵ `utils/sprgen/sprgen.c:562-616`
  - [diverged cross-file w=1.00 line=0.17] `utils/sprgen/sprgen.cpp:446-491` ⟵ `utils/sprgen/sprgen.c:447-492`
  - [diverged cross-file w=1.00 line=0.40] `utils/sprgen/sprgen.cpp:402-436` ⟵ `utils/sprgen/sprgen.c:402-437`
  - [diverged within-file w=1.00 line=0.27] `utils/smdlexp/smdlexp.cpp:692-699` ⟵ `utils/smdlexp/smdlexp.cpp:682-689`
  - [diverged cross-file w=1.00 line=0.75] `utils/qrad/vismat.cpp:323-340` ⟵ `utils/qrad/vismat.c:328-345`
  - [diverged cross-file w=1.00 line=0.65] `utils/qrad/vismat.cpp:168-241` ⟵ `utils/qrad/vismat.c:170-244`
  - [diverged cross-file w=1.00 line=0.23] `utils/qrad/qrad.cpp:1491-1499` ⟵ `utils/qrad/qrad.c:1513-1521`
  - [diverged cross-file w=1.00 line=0.17] `utils/qrad/qrad.cpp:1151-1171` ⟵ `utils/qrad/qrad.c:1167-1190`
  - [diverged cross-file w=1.00 line=0.22] `utils/qrad/qrad.cpp:984-1003` ⟵ `utils/qrad/qrad.c:994-1013`
  - [diverged cross-file w=1.00 line=0.76] `dlls/COFSquadTalkMonster.cpp:530-589` ⟵ `dlls/squadmonster.cpp:453-512`
  - [verbatim cross-file w=1.00 line=1.00] `utils/visx2/flow.cpp:525-530` ⟵ `utils/visx2/flow.c:531-536`
  - [diverged cross-file w=1.00 line=0.24] `utils/qrad/qrad.cpp:472-519` ⟵ `utils/qrad/qrad.c:476-523`
  - [diverged cross-file w=1.00 line=0.52] `utils/qrad/qrad.cpp:257-281` ⟵ `utils/qrad/qrad.c:260-284`
  - [diverged cross-file w=1.00 line=0.43] `utils/visx2/vis.cpp:151-163` ⟵ `utils/visx2/vis.c:152-164`
  - [diverged within-file w=1.00 line=0.67] `cl_dll/vgui_ScorePanel.cpp:903-908` ⟵ `cl_dll/vgui_ScorePanel.cpp:846-851`
  - [diverged cross-file w=1.00 line=0.20] `utils/qrad/lightmap.cpp:1432-1466` ⟵ `utils/qrad/lightmap.c:1451-1486`
  - [diverged cross-file w=1.00 line=0.60] `utils/qrad/lightmap.cpp:453-463` ⟵ `utils/qrad/lightmap.c:458-468`
  - [diverged cross-file w=1.00 line=0.78] `utils/qrad/lightmap.cpp:173-182` ⟵ `utils/qrad/lightmap.c:176-185`
  - [diverged cross-file w=1.00 line=0.28] `utils/qlumpy/quakegrb.cpp:152-195` ⟵ `utils/qlumpy/quakegrb.c:152-195`
  - [diverged cross-file w=1.00 line=0.11] `utils/qcsg/qcsg.cpp:597-610` ⟵ `utils/qcsg/qcsg.c:605-618`
  - [diverged cross-file w=1.00 line=0.34] `utils/qlumpy/qlumpy.cpp:244-326` ⟵ `utils/qlumpy/qlumpy.c:238-320`
  - [diverged cross-file w=1.00 line=0.29] `utils/visx2/vis.cpp:305-318` ⟵ `utils/visx2/vis.c:306-319`
  - [diverged within-file w=1.00 line=0.50] `cl_dll/vgui_ScorePanel.cpp:758-809` ⟵ `cl_dll/vgui_ScorePanel.cpp:702-753`
  - [diverged cross-file w=1.00 line=0.41] `utils/qlumpy/qlumpy.cpp:154-174` ⟵ `utils/qlumpy/qlumpy.c:149-168`
  - [verbatim cross-file w=1.00 line=0.95] `dlls/scientist.cpp:733-782` ⟵ `dlls/cleansuit_scientist.cpp:697-746`
  - [diverged cross-file w=1.00 line=0.46] `utils/qrad/lightmap.cpp:244-288` ⟵ `utils/qrad/lightmap.c:247-291`
  - [diverged cross-file w=0.99 line=0.31] `utils/qbsp2/portals.cpp:278-329` ⟵ `utils/qbsp2/portals.c:281-334`
  - [diverged cross-file w=0.99 line=0.29] `utils/qbsp2/qbsp.cpp:946-1019` ⟵ `utils/qbsp2/qbsp.c:943-1016`
  - [diverged cross-file w=0.99 line=0.38] `utils/qbsp2/qbsp.cpp:771-812` ⟵ `utils/qbsp2/qbsp.c:766-808`
  - [diverged cross-file w=0.99 line=0.23] `utils/light/light.cpp:156-226` ⟵ `utils/light/light.c:157-227`
  - [diverged cross-file w=0.99 line=0.57] `utils/common/threads.cpp:141-175` ⟵ `utils/common/threads.c:141-175`
  - [verbatim within-file w=0.98 line=0.96] `dlls/player.cpp:4865-4945` ⟵ `dlls/player.cpp:4659-4739`
  - [near cross-file w=0.98 line=0.89] `dlls/scientist.cpp:467-529` ⟵ `dlls/cleansuit_scientist.cpp:438-498`
  - [diverged cross-file w=0.98 line=0.07] `utils/common/bspfile.cpp:512-536` ⟵ `utils/common/bsplib.c:510-535`
  - [diverged cross-file w=0.97 line=0.21] `utils/qcsg/qcsg.cpp:806-819` ⟵ `utils/qcsg/qcsg.c:818-832`
  - [near within-file w=0.97 line=0.94] `dlls/util.cpp:2441-2485` ⟵ `dlls/util.cpp:1995-2039`
  - [diverged cross-file w=0.97 line=0.35] `utils/qbsp2/qbsp.cpp:822-890` ⟵ `utils/qbsp2/qbsp.c:818-887`
  - [diverged cross-file w=0.97 line=0.26] `utils/visx2/vis.cpp:446-521` ⟵ `utils/visx2/vis.c:451-526`
  - [diverged cross-file w=0.96 line=0.38] `utils/qcsg/textures.cpp:112-170` ⟵ `utils/qcsg/textures.c:113-173`
  - [near within-file w=0.96 line=0.82] `dlls/ichthyosaur.cpp:578-599` ⟵ `dlls/ichthyosaur.cpp:558-573`
  - [diverged cross-file w=0.96 line=0.29] `utils/qrad/qrad.cpp:1180-1223` ⟵ `utils/qrad/qrad.c:1199-1245`
  - [diverged cross-file w=0.95 line=0.56] `utils/qrad/qrad.cpp:699-785` ⟵ `utils/qrad/qrad.c:708-795`
  - [verbatim within-file w=0.95 line=0.95] `dlls/triggers.cpp:1776-1835` ⟵ `dlls/triggers.cpp:1658-1717`
  - [diverged cross-file w=0.95 line=0.35] `utils/qlumpy/qlumpy.cpp:384-436` ⟵ `utils/qlumpy/qlumpy.c:378-430`
  - [near within-file w=0.95 line=0.92] `dlls/player.cpp:3552-3586` ⟵ `dlls/player.cpp:3475-3509`
  - [near within-file w=0.94 line=0.87] `dlls/player.cpp:5472-5528` ⟵ `dlls/player.cpp:5243-5298`
  - [diverged cross-file w=0.94 line=0.61] `utils/qcsg/qcsg.cpp:89-166` ⟵ `utils/qcsg/qcsg.c:89-165`
  - [diverged cross-file w=0.94 line=0.36] `utils/qbsp2/qbsp.cpp:713-762` ⟵ `utils/qbsp2/qbsp.c:708-757`
  - [near within-file w=0.94 line=0.89] `dlls/util.cpp:2356-2379` ⟵ `dlls/util.cpp:1910-1933`
  - [diverged cross-file w=0.93 line=0.54] `dlls/talkmonster.cpp:1199-1289` ⟵ `dlls/COFAllyMonster.cpp:1180-1270`
  - [diverged cross-file w=0.93 line=0.54] `dlls/COFAllyMonster.cpp:1178-1268` ⟵ `dlls/talkmonster.cpp:1195-1285`
  - [near within-file w=0.93 line=0.81] `dlls/effects.cpp:2024-2048` ⟵ `dlls/effects.cpp:1962-1982`
  - [near within-file w=0.93 line=0.91] `dlls/player.cpp:4267-4292` ⟵ `dlls/player.cpp:4090-4111`
  - [near within-file w=0.92 line=0.90] `dlls/plats.cpp:2189-2214` ⟵ `dlls/plats.cpp:2116-2141`
  - [near within-file w=0.91 line=0.90] `cl_dll/inputw32.cpp:670-698` ⟵ `cl_dll/inputw32.cpp:640-668`
  - [verbatim within-file w=0.91 line=0.96] `game_shared/filesystem_utils.cpp:118-168` ⟵ `game_shared/filesystem_utils.cpp:32-77`
  - [near within-file w=0.91 line=0.80] `dlls/player.cpp:5705-5714` ⟵ `dlls/player.cpp:5475-5484`
  - [near within-file w=0.89 line=0.82] `dlls/scripted.cpp:648-661` ⟵ `dlls/scripted.cpp:601-614`
  - [diverged within-file w=0.88 line=0.60] `dlls/player.cpp:5697-5700` ⟵ `dlls/player.cpp:5467-5470`
  - [near within-file w=0.87 line=0.86] `dlls/effects.cpp:261-273` ⟵ `dlls/effects.cpp:235-247`
  - [near within-file w=0.87 line=0.81] `game_shared/filesystem_utils.cpp:260-288` ⟵ `game_shared/filesystem_utils.cpp:95-120`
  - [near within-file w=0.86 line=0.83] `dlls/scripted.cpp:601-616` ⟵ `dlls/scripted.cpp:554-569`
  - [diverged within-file w=0.86 line=0.75] `dlls/weapons.cpp:937-946` ⟵ `dlls/weapons.cpp:926-935`
  - [diverged cross-file w=0.85 line=0.22] `utils/qcsg/brush.cpp:811-851` ⟵ `utils/qcsg/brush.c:798-859`
  - [near within-file w=0.85 line=0.85] `dlls/player.cpp:3319-3393` ⟵ `dlls/player.cpp:3253-3318`
  - [near within-file w=0.85 line=0.88] `dlls/buttons.cpp:1273-1283` ⟵ `dlls/buttons.cpp:1084-1092`
  - [diverged within-file w=0.85 line=0.39] `external/SDL2/SDL_assert.h:151-162` ⟵ `external/SDL2/SDL_assert.h:130-144`
  - [diverged cross-file w=0.84 line=0.38] `utils/qrad/vismat.cpp:445-472` ⟵ `utils/qrad/vismat.c:476-506`
  - [near within-file w=0.84 line=0.83] `dlls/plats.cpp:2503-2515` ⟵ `dlls/plats.cpp:2430-2442`
  - [near within-file w=0.84 line=0.82] `dlls/plats.cpp:2483-2495` ⟵ `dlls/plats.cpp:2410-2422`
  - [near within-file w=0.84 line=0.82] `dlls/effects.cpp:697-710` ⟵ `dlls/effects.cpp:652-665`
  - [diverged cross-file w=0.82 line=0.17] `utils/common/wadlib.cpp:205-214` ⟵ `utils/common/wadlib.c:210-219`
  - [near within-file w=0.82 line=0.80] `dlls/scripted.cpp:1237-1262` ⟵ `dlls/scripted.cpp:1155-1180`
  - [diverged within-file w=0.81 line=0.75] `dlls/multiplay_gamerules.cpp:1696-1705` ⟵ `dlls/multiplay_gamerules.cpp:1749-1758`
  - [diverged within-file w=0.80 line=0.47] `dlls/scripted.cpp:1127-1143` ⟵ `dlls/scripted.cpp:1065-1081`
  - [diverged within-file w=0.80 line=0.73] `dlls/client.cpp:1189-1218` ⟵ `dlls/client.cpp:1030-1059`
  - [diverged cross-file w=0.79 line=0.59] `dlls/blkop_osprey.cpp:394-425` ⟵ `dlls/osprey.cpp:385-411`
  - [diverged within-file w=0.78 line=0.75] `dlls/util.cpp:2429-2436` ⟵ `dlls/util.cpp:1983-1990`
  - [diverged within-file w=0.78 line=0.68] `dlls/scripted.cpp:1157-1188` ⟵ `dlls/scripted.cpp:1095-1111`
  - [diverged within-file w=0.77 line=0.73] `cl_dll/hud.cpp:724-742` ⟵ `cl_dll/hud.cpp:667-685`
  - [diverged within-file w=0.77 line=0.30] `utils/mdlviewer/mdlviewer.cpp:215-234` ⟵ `utils/mdlviewer/mdlviewer.cpp:195-214`
  - [near within-file w=0.76 line=0.85] `dlls/triggers.cpp:1599-1653` ⟵ `dlls/triggers.cpp:1481-1535`
  - [diverged within-file w=0.74 line=0.65] `dlls/plats.cpp:1852-1885` ⟵ `dlls/plats.cpp:1789-1821`
  - [diverged cross-file w=0.73 line=0.54] `dlls/osprey.cpp:396-430` ⟵ `dlls/blkop_osprey.cpp:385-411`
  - [near within-file w=0.73 line=0.87] `dlls/player.cpp:3755-3860` ⟵ `dlls/player.cpp:3635-3719`
  - [diverged within-file w=0.68 line=0.62] `dlls/plats.cpp:1775-1816` ⟵ `dlls/plats.cpp:1713-1753`
  - [diverged within-file w=0.65 line=0.50] `dlls/scripted.cpp:514-525` ⟵ `dlls/scripted.cpp:467-478`
  - [diverged cross-file w=0.64 line=0.53] `dlls/squadmonster.cpp:417-444` ⟵ `dlls/COFSquadTalkMonster.cpp:486-510`
  - [diverged cross-file w=0.63 line=0.53] `dlls/COFSquadTalkMonster.cpp:496-520` ⟵ `dlls/squadmonster.cpp:416-443`
  - [diverged within-file w=0.61 line=0.60] `dlls/player.cpp:4055-4135` ⟵ `dlls/player.cpp:3914-3967`

## ismobaga/izilang — 201 коммитов | циклы=1(SCC≤2) | копипаст=2
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/ismobaga_izilang`

### `d9b21abdabd6` — "Améliorer la gestion des fonctions utilisateur et des variables capturées dans la VM" (Ismail Bagayoko, 2026-04-01) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ismobaga_izilang show d9b21abdabd6`
  - [diverged within-file w=0.73 line=0.27] `src/parse/lexer.cpp:297-309` ⟵ `src/parse/lexer.cpp:153-158`
  - [diverged within-file w=0.60 line=0.67] `src/parse/parser.cpp:319-330` ⟵ `src/parse/parser.cpp:73-80`

## DavidLee18/DArc — 186 коммитов | циклы=1(SCC≤4) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=4. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/DavidLee18_DArc`

## X4V1-code/Xorion — 171 коммитов | циклы=3(SCC≤3) | копипаст=76
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 3 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/X4V1-code_Xorion`

### `7e247080b2be` — "Merge branch 'master' of https://github.com/X5V1/Xorion" (Xavi, 2026-02-11) [76 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/X4V1-code_Xorion show 7e247080b2be`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/HudModule.cpp:81-87` ⟵ `Horion/Module/Modules/HudModule.cpp:80-86`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/DrawUtils.cpp:57-109` ⟵ `Horion/DrawUtils.cpp:54-106`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/DrawUtils.cpp:584-589` ⟵ `Horion/DrawUtils.cpp:570-575`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/DrawUtils.cpp:774-794` ⟵ `Horion/DrawUtils.cpp:760-780`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Menu/ClickGui.cpp:265-278` ⟵ `Horion/Menu/ClickGui.cpp:262-275`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Menu/ClickGui.cpp:414-456` ⟵ `Horion/Menu/ClickGui.cpp:408-450`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Menu/ClickGui.cpp:830-871` ⟵ `Horion/Menu/ClickGui.cpp:821-862`
  - [diverged cross-file w=1.00 line=0.64] `Xorion/Module/ModuleManager.cpp:345-355` ⟵ `Horion/Module/ModuleManager.cpp:319-329`
  - [verbatim within-file w=1.00 line=1.00] `Memory/GameData.cpp:199-208` ⟵ `Memory/GameData.cpp:172-180`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/Compass.cpp:28-39` ⟵ `Horion/Module/Modules/Compass.cpp:28-39`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/ESP.cpp:21-61` ⟵ `Horion/Module/Modules/ESP.cpp:21-61`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/FollowPathModule.cpp:30-46` ⟵ `Horion/Module/Modules/FollowPathModule.cpp:12-28`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/AirSwim.cpp:22-25` ⟵ `Horion/Module/Modules/AirSwim.cpp:21-24`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/HudModule.cpp:127-135` ⟵ `Horion/Module/Modules/HudModule.cpp:126-134`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/InventoryCleaner.cpp:156-168` ⟵ `Horion/Module/Modules/InventoryCleaner.cpp:153-165`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/LigmaNuts.cpp:66-76` ⟵ `Horion/Module/Modules/LigmaNuts.cpp:54-64`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/Scaffold.cpp:198-205` ⟵ `Horion/Module/Modules/Scaffold.cpp:195-202`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/Scaffold.cpp:208-220` ⟵ `Horion/Module/Modules/Scaffold.cpp:205-217`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Module/Modules/StorageESP.cpp:83-90` ⟵ `Horion/Module/Modules/StorageESP.cpp:83-90`
  - [near cross-file w=1.00 line=0.89] `Xorion/Module/Modules/Tracer.cpp:40-60` ⟵ `Horion/Module/Modules/Tracer.cpp:40-60`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/path/JoeMovementController.cpp:27-30` ⟵ `Horion/path/JoeMovementController.cpp:26-29`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/path/JoeMovementController.cpp:52-82` ⟵ `Horion/path/JoeMovementController.cpp:51-81`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/path/JoeMovementController.cpp:111-147` ⟵ `Horion/path/JoeMovementController.cpp:110-146`
  - [near cross-file w=1.00 line=0.80] `Xorion/Config/ConfigManager.cpp:46-97` ⟵ `Horion/Config/ConfigManager.cpp:45-96`
  - [near within-file w=1.00 line=0.87] `Memory/Hooks.cpp:359-376` ⟵ `Memory/Hooks.cpp:304-321`
  - [verbatim within-file w=1.00 line=1.00] `Utils/Utils.h:372-377` ⟵ `Utils/Utils.h:409-414`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Command/CommandMgr.cpp:92-105` ⟵ `Horion/Command/CommandMgr.cpp:88-101`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Command/Commands/EnchantCommand.cpp:116-140` ⟵ `Horion/Command/Commands/EnchantCommand.cpp:112-136`
  - [diverged cross-file w=1.00 line=0.79] `Xorion/Command/Commands/FriendListCommand.cpp:33-97` ⟵ `Horion/Command/Commands/FriendListCommand.cpp:32-96`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Command/Commands/HelpCommand.cpp:14-23` ⟵ `Horion/Command/Commands/HelpCommand.cpp:13-22`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Config/ConfigManager.cpp:100-118` ⟵ `Horion/Config/ConfigManager.cpp:99-117`
  - [verbatim cross-file w=1.00 line=1.00] `Xorion/Config/ConfigManager.cpp:13-36` ⟵ `Horion/Config/ConfigManager.cpp:12-35`
  - [near cross-file w=1.00 line=0.94] `Xorion/Module/Modules/BowAimbot.cpp:55-94` ⟵ `Horion/Module/Modules/BowAimbot.cpp:55-94`
  - [near cross-file w=1.00 line=0.80] `Xorion/Module/Modules/StorageESP.cpp:56-79` ⟵ `Horion/Module/Modules/StorageESP.cpp:56-79`
  - [near within-file w=1.00 line=0.86] `Memory/Hooks.cpp:894-929` ⟵ `Memory/Hooks.cpp:819-856`
  - [diverged cross-file w=1.00 line=0.41] `Xorion/Module/Modules/Fly.cpp:150-164` ⟵ `Horion/Module/Modules/Fly.cpp:144-158`
  - [near cross-file w=1.00 line=0.83] `Xorion/Module/Modules/FollowPathModule.cpp:78-106` ⟵ `Horion/Module/Modules/FollowPathModule.cpp:60-88`
  - [near cross-file w=1.00 line=0.94] `Xorion/Menu/TabGui.cpp:202-246` ⟵ `Horion/Menu/TabGui.cpp:201-245`
  - [near cross-file w=1.00 line=0.92] `Xorion/Loader.cpp:93-121` ⟵ `Horion/Loader.cpp:88-116`
  - [near cross-file w=1.00 line=0.92] `Xorion/Menu/TabGui.cpp:160-192` ⟵ `Horion/Menu/TabGui.cpp:159-191`
  - [diverged within-file w=0.99 line=0.67] `Utils/Utils.cpp:54-63` ⟵ `Utils/Utils.cpp:38-47`
  - [diverged cross-file w=0.99 line=0.68] `Xorion/Module/Modules/Killaura.cpp:94-126` ⟵ `Horion/Module/Modules/Killaura.cpp:93-125`
  - [near within-file w=0.99 line=0.80] `Memory/Hooks.cpp:345-356` ⟵ `Memory/Hooks.cpp:290-301`
  - [diverged cross-file w=0.99 line=0.68] `Xorion/Module/Modules/MidClick.cpp:17-36` ⟵ `Horion/Module/Modules/MidClick.cpp:15-34`
  - [diverged cross-file w=0.98 line=0.50] `Xorion/Module/Modules/Killaura.cpp:149-164` ⟵ `Horion/Module/Modules/Killaura.cpp:148-163`
  - [diverged within-file w=0.98 line=0.71] `Memory/Hooks.cpp:886-891` ⟵ `Memory/Hooks.cpp:811-816`
  - [diverged cross-file w=0.98 line=0.67] `Xorion/Loader.cpp:17-30` ⟵ `Horion/Loader.cpp:14-27`
  - [near cross-file w=0.97 line=0.88] `Xorion/Module/Modules/Arraylist.cpp:92-131` ⟵ `Horion/Module/Modules/Arraylist.cpp:92-131`
  - [verbatim cross-file w=0.96 line=0.96] `Xorion/Module/Modules/CrystalAura.cpp:120-185` ⟵ `Horion/Module/Modules/CrystalAura.cpp:120-185`
  - [diverged cross-file w=0.96 line=0.79] `Xorion/Module/Modules/Aimbot.cpp:82-117` ⟵ `Horion/Module/Modules/Aimbot.cpp:82-117`
  - [diverged within-file w=0.96 line=0.47] `Utils/Utils.h:383-394` ⟵ `Utils/Utils.h:435-446`
  - [near within-file w=0.96 line=0.83] `Utils/Utils.h:397-408` ⟵ `Utils/Utils.h:435-446`
  - [diverged cross-file w=0.95 line=0.23] `Xorion/Module/Modules/InventoryCleaner.cpp:236-243` ⟵ `Horion/Module/Modules/InventoryCleaner.cpp:233-240`
  - [diverged cross-file w=0.95 line=0.62] `Xorion/Command/Commands/RepairCommand.cpp:13-42` ⟵ `Horion/Command/Commands/RepairCommand.cpp:10-39`
  - [near cross-file w=0.93 line=0.90] `Xorion/Loader.cpp:37-88` ⟵ `Horion/Loader.cpp:34-83`
  - [diverged within-file w=0.93 line=0.33] `Utils/Utils.cpp:66-69` ⟵ `Utils/Utils.cpp:57-60`
  - [diverged cross-file w=0.92 line=0.75] `Xorion/Module/Modules/InventoryCleaner.cpp:202-229` ⟵ `Horion/Module/Modules/InventoryCleaner.cpp:199-226`
  - [near within-file w=0.91 line=0.82] `Memory/Hooks.cpp:412-423` ⟵ `Memory/Hooks.cpp:357-368`
  - [diverged cross-file w=0.91 line=0.78] `Xorion/Module/Modules/AutoGapple.cpp:96-135` ⟵ `Horion/Module/Modules/AutoGapple.cpp:94-133`
  - [diverged cross-file w=0.89 line=0.52] `Xorion/Module/Modules/AutoTotem.cpp:25-49` ⟵ `Horion/Module/Modules/AutoTotem.cpp:21-45`
  - [near cross-file w=0.88 line=0.82] `Xorion/Command/Commands/SpammerCommand.cpp:58-75` ⟵ `Horion/Command/Commands/SpammerCommand.cpp:57-73`
  - [diverged cross-file w=0.87 line=0.45] `Xorion/Module/Modules/AutoClicker.cpp:18-54` ⟵ `Horion/Module/Modules/AutoClicker.cpp:18-54`
  - [diverged cross-file w=0.87 line=0.77] `Xorion/Module/Modules/AutoAnchor.cpp:50-98` ⟵ `Horion/Module/Modules/AutoAnchor.cpp:53-104`
  - [diverged cross-file w=0.86 line=0.64] `Xorion/Module/Modules/AutoArmor.cpp:77-120` ⟵ `Horion/Module/Modules/AutoArmor.cpp:72-116`
  - [diverged cross-file w=0.84 line=0.44] `Xorion/Module/Modules/BedAura.cpp:100-130` ⟵ `Horion/Module/Modules/BedAura.cpp:99-135`
  - [diverged cross-file w=0.84 line=0.75] `Xorion/DrawUtils.cpp:120-145` ⟵ `Horion/DrawUtils.cpp:117-141`
  - [diverged cross-file w=0.84 line=0.52] `Xorion/Module/Modules/HudModule.cpp:105-122` ⟵ `Horion/Module/Modules/HudModule.cpp:104-121`
  - [diverged cross-file w=0.83 line=0.43] `Xorion/Module/Modules/Tower.cpp:71-102` ⟵ `Horion/Module/Modules/Tower.cpp:71-101`
  - [diverged cross-file w=0.83 line=0.67] `Xorion/Module/Modules/Scaffold.cpp:177-195` ⟵ `Horion/Module/Modules/Scaffold.cpp:175-192`
  - [diverged cross-file w=0.82 line=0.67] `Xorion/Module/Modules/InventoryCleaner.cpp:90-104` ⟵ `Horion/Module/Modules/InventoryCleaner.cpp:87-101`
  - [diverged cross-file w=0.78 line=0.38] `Xorion/Module/Modules/BedAura.cpp:134-178` ⟵ `Horion/Module/Modules/BedAura.cpp:139-183`
  - [diverged cross-file w=0.76 line=0.79] `Xorion/Module/Modules/ChestStealer.cpp:17-37` ⟵ `Horion/Module/Modules/ChestStealer.cpp:17-37`
  - [diverged within-file w=0.75 line=0.17] `Utils/Utils.cpp:177-196` ⟵ `Utils/Utils.cpp:157-170`
  - [diverged cross-file w=0.74 line=0.71] `Xorion/Module/Modules/AutoGapple.cpp:65-93` ⟵ `Horion/Module/Modules/AutoGapple.cpp:63-91`
  - [diverged cross-file w=0.68 line=0.70] `Xorion/DrawUtils.cpp:473-525` ⟵ `Horion/DrawUtils.cpp:471-510`
  - [diverged cross-file w=0.62 line=0.37] `Xorion/Module/Modules/Dicker.cpp:40-65` ⟵ `Horion/Module/Modules/Dicker.cpp:38-60`

## ImagingTools/AcfSln — 171 коммитов | циклы=0(SCC≤1) | копипаст=53

### `28335c8cbbb3` — "Merge pull request #50 from ImagingTools/copilot/fix-combobox-attribute-issue" (kirill-lepski, 2026-05-29) [53 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ImagingTools_AcfSln show 28335c8cbbb3`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:2421-2441` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:1906-1926`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1364-1373` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1312-1321`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1341-1359` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1289-1307`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1307-1316` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1255-1264`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1292-1302` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1240-1250`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1125-1150` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1073-1098`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:893-932` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:849-888`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:645-675` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:605-635`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:1287-1300` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:1244-1257`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1446-1455` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1388-1397`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:1174-1182` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:1131-1139`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:1125-1159` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:1082-1116`
  - [verbatim within-file w=1.00 line=1.00] `Include/iprocgui/CDocumentProcessingManagerCompBase.cpp:127-132` ⟵ `Include/iprocgui/CDocumentProcessingManagerCompBase.cpp:103-108`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:863-875` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:820-832`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:2914-2953` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:2381-2420`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:2357-2414` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:1842-1899`
  - [verbatim within-file w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:586-608` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:544-566`
  - [diverged within-file w=1.00 line=0.11] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:116-121` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:110-115`
  - [diverged within-file w=1.00 line=0.11] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:108-113` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:118-123`
  - [verbatim cross-file w=1.00 line=1.00] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:28-45` ⟵ `Include/icalibgui/CGeneralCalibrationShape.cpp:30-47`
  - [verbatim cross-file w=1.00 line=1.00] `Include/icalibgui/CGeneralCalibrationShape.cpp:31-48` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:30-47`
  - [verbatim within-file w=1.00 line=1.00] `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1772-1799` ⟵ `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1137-1164`
  - [verbatim within-file w=1.00 line=1.00] `Include/icalib/CPerspectiveCalibration2d.cpp:260-265` ⟵ `Include/icalib/CPerspectiveCalibration2d.cpp:251-256`
  - [near within-file w=0.98 line=0.89] `Include/icam/CCalibratedCameraComp.cpp:199-222` ⟵ `Include/icam/CCalibratedCameraComp.cpp:133-156`
  - [diverged cross-file w=0.93 line=0.77] `Include/iauth/CUsersManagerComp.cpp:168-200` ⟵ `Include/iauth/CSimpleLoginComp.cpp:146-177`
  - [diverged cross-file w=0.93 line=0.77] `Include/iauth/CSimpleLoginComp.cpp:147-178` ⟵ `Include/iauth/CUsersManagerComp.cpp:167-199`
  - [near cross-file w=0.90 line=0.92] `Include/iqtinsp/TCommonSupplierGuiCompBase.h:370-409` ⟵ `Include/iqtinsp/TSupplierGuiCompBase.h:472-511`
  - [near within-file w=0.89 line=0.86] `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1436-1477` ⟵ `Include/iqtinsp/CInspectionTaskGuiComp.cpp:843-882`
  - [diverged within-file w=0.88 line=0.28] `Include/iipr/CPerspCalibFinder.cpp:235-265` ⟵ `Include/iipr/CPerspCalibFinder.cpp:521-553`
  - [diverged within-file w=0.85 line=0.21] `Include/iipr/CPerspCalibFinder.cpp:526-558` ⟵ `Include/iipr/CPerspCalibFinder.cpp:232-262`
  - [diverged within-file w=0.84 line=0.62] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:246-296` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:170-216`
  - [diverged cross-file w=0.83 line=0.63] `Impl/XpcEditorExe/main.cpp:18-38` ⟵ `Impl/CompositorExe/main.cpp:17-41`
  - [diverged within-file w=0.78 line=0.67] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1087-1120` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1043-1068`
  - [near within-file w=0.78 line=0.81] `Include/icam/CExposureParamsComp.cpp:140-167` ⟵ `Include/icam/CExposureParamsComp.cpp:109-131`
  - [diverged cross-file w=0.76 line=0.59] `Include/iqtcam/CSnapImageParamsEditorComp.cpp:139-157` ⟵ `Include/iqtcam/CSnapImageGuiComp.cpp:123-154`
  - [diverged cross-file w=0.76 line=0.59] `Include/iqtcam/CSnapImageGuiComp.cpp:135-166` ⟵ `Include/iqtcam/CSnapImageParamsEditorComp.cpp:145-163`
  - [diverged within-file w=0.74 line=0.33] `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1713-1729` ⟵ `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1078-1094`
  - [near within-file w=0.73 line=0.86] `Include/icmpstr/CAttributeEditorComp.cpp:2133-2191` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:1628-1681`
  - [diverged cross-file w=0.72 line=0.54] `Include/imeas/TDiscreteDataSequence.h:296-352` ⟵ `Include/imeas/CGeneralDataSequence.cpp:252-293`
  - [diverged cross-file w=0.72 line=0.54] `Include/imeas/CGeneralDataSequence.cpp:265-306` ⟵ `Include/imeas/TDiscreteDataSequence.h:295-351`
  - [diverged within-file w=0.71 line=0.76] `Include/icmpstr/CVisualRegistryEditorComp.cpp:733-765` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:693-721`
  - [diverged cross-file w=0.70 line=0.63] `Include/iqtinsp/TCommonSupplierGuiCompBase.h:292-350` ⟵ `Include/iqtinsp/TSupplierGuiCompBase.h:390-447`
  - [diverged within-file w=0.69 line=0.79] `Include/ifileproc/CCopyFilesTreeComp.cpp:172-190` ⟵ `Include/ifileproc/CCopyFilesTreeComp.cpp:97-114`
  - [diverged within-file w=0.69 line=0.79] `Include/ifileproc/CCopyFilesTreeComp.cpp:99-116` ⟵ `Include/ifileproc/CCopyFilesTreeComp.cpp:170-188`
  - [diverged cross-file w=0.69 line=0.17] `Include/icmpstr/CComponentTreeComp.cpp:664-691` ⟵ `Include/icmpstr/CRegistryTreeViewComp.cpp:461-485`
  - [diverged cross-file w=0.69 line=0.57] `Impl/CompositorExe/main.cpp:18-44` ⟵ `Impl/XpcEditorExe/main.cpp:17-37`
  - [diverged within-file w=0.67 line=0.61] `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1584-1652` ⟵ `Include/iqtinsp/CInspectionTaskGuiComp.cpp:975-1017`
  - [diverged cross-file w=0.66 line=0.56] `Include/iipr/TImagePixelInterpolator.h:61-68` ⟵ `Include/iipr/CImagePolarTransformProcessorComp.cpp:25-30`
  - [diverged cross-file w=0.66 line=0.56] `Include/iipr/CImagePolarTransformProcessorComp.cpp:26-31` ⟵ `Include/iipr/TImagePixelInterpolator.h:60-67`
  - [diverged within-file w=0.66 line=0.23] `Include/icalib/CPerspectiveCalibration2dComp.cpp:19-28` ⟵ `Include/icalib/CPerspectiveCalibration2dComp.cpp:32-41`
  - [diverged within-file w=0.66 line=0.67] `Include/iqtinsp/CInspectionTaskGuiComp.cpp:444-461` ⟵ `Include/iqtinsp/CInspectionTaskGuiComp.cpp:278-289`
  - [diverged within-file w=0.62 line=0.21] `Include/icalib/CPerspectiveCalibration2dComp.cpp:33-44` ⟵ `Include/icalib/CPerspectiveCalibration2dComp.cpp:18-27`
  - [diverged within-file w=0.60 line=0.60] `Include/iqtinsp/TSupplierGuiCompBase.h:186-215` ⟵ `Include/iqtinsp/TSupplierGuiCompBase.h:556-595`

## DarthPineapple/voxel-game — 166 коммитов | циклы=0(SCC≤1) | копипаст=2

### `87d98b91ffcc` — "Merge pull request #41 from DarthPineapple/copilot/fix-mesh-generation-bugs" (DarthPineapple, 2025-10-28) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/DarthPineapple_voxel-game show 87d98b91ffcc`
  - [diverged cross-file w=0.80 line=0.45] `src/graphics/vulkan/overlay_pipeline.cpp:209-217` ⟵ `src/graphics/vulkan/pipeline.cpp:67-78`
  - [diverged within-file w=0.80 line=0.45] `src/graphics/vulkan/pipeline.cpp:244-252` ⟵ `src/graphics/vulkan/pipeline.cpp:67-78`

## hyzboy/CMMath — 165 коммитов | циклы=0(SCC≤1) | копипаст=5

### `43d3d4286b31` — "GCC兼容性修复" (HuYingzhuo(hugo), 2026-05-06) [5 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/hyzboy_CMMath show 43d3d4286b31`
  - [verbatim cross-file w=1.00 line=1.00] `inc/hgl/math/transform/Transform.h:160-171` ⟵ `inc/hgl/math/Transform.h:808-819`
  - [verbatim cross-file w=1.00 line=1.00] `inc/hgl/math/transform/TransformAction.h:648-659` ⟵ `inc/hgl/math/Transform.h:632-643`
  - [verbatim cross-file w=1.00 line=1.00] `src/Geometry/AABB.cpp:47-67` ⟵ `src/graph/Bounding/AABB.cpp:42-62`
  - [verbatim cross-file w=1.00 line=1.00] `src/Geometry/Frustum.cpp:101-106` ⟵ `src/graph/Frustum.cpp:95-100`
  - [diverged within-file w=0.70 line=0.44] `src/Math/Matrix4f.cpp:400-436` ⟵ `src/Math/Matrix4f.cpp:190-218`

## michael-haufschild-gib/MultiScoper — 154 коммитов | циклы=0(SCC≤1) | копипаст=28

### `8b898827c6e3` — "refactor: test isolation for PresetManager/ThemeManager + UI polish (#61)" (Michael Haufschild, 2026-04-21) [28 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/michael-haufschild-gib_MultiScoper show 8b898827c6e3`
  - [verbatim cross-file w=1.00 line=1.00] `tests/test_source_state.cpp:180-186` ⟵ `tests/test_source.cpp:330-336`
  - [verbatim cross-file w=1.00 line=1.00] `tests/test_source_state.cpp:164-176` ⟵ `tests/test_source.cpp:314-326`
  - [diverged within-file w=1.00 line=0.71] `test_harness/src/Main.cpp:33-38` ⟵ `test_harness/src/Main.cpp:27-32`
  - [verbatim cross-file w=1.00 line=1.00] `test_harness/src/TestHttpServerTransport.cpp:121-128` ⟵ `test_harness/src/TestHttpServer.cpp:213-220`
  - [verbatim cross-file w=1.00 line=1.00] `src/ui/layout/PaneComponent.cpp:206-221` ⟵ `src/ui/PaneComponent.cpp:178-193`
  - [verbatim cross-file w=1.00 line=1.00] `src/ui/panels/OscillatorListItemPainting.cpp:112-121` ⟵ `src/ui/OscillatorListItem.cpp:412-421`
  - [diverged cross-file w=0.99 line=0.62] `src/ui/theme/ThemeEditorWidgets.cpp:124-133` ⟵ `src/ui/ThemeEditorComponent.cpp:140-150`
  - [verbatim cross-file w=0.99 line=1.00] `test_harness/src/TestHttpServerVerification.cpp:32-66` ⟵ `test_harness/src/TestHttpServer.cpp:479-515`
  - [diverged cross-file w=0.98 line=0.38] `src/ui/theme/ThemeEditorWidgets.cpp:57-73` ⟵ `src/ui/ThemeEditorComponent.cpp:66-88`
  - [diverged cross-file w=0.94 line=0.36] `src/ui/theme/ThemeEditorActions.cpp:257-287` ⟵ `src/ui/ThemeEditorComponent.cpp:620-652`
  - [diverged cross-file w=0.93 line=0.39] `src/ui/theme/ThemeEditorActions.cpp:229-253` ⟵ `src/ui/ThemeEditorComponent.cpp:590-616`
  - [diverged cross-file w=0.89 line=0.14] `tests/test_source_buffer.cpp:39-67` ⟵ `tests/test_source.cpp:226-255`
  - [diverged cross-file w=0.81 line=0.32] `src/core/dsp/SignalProcessor.cpp:50-82` ⟵ `src/dsp/SignalProcessor.cpp:20-43`
  - [diverged cross-file w=0.78 line=0.05] `src/ui/dialogs/OscillatorConfigDialog.cpp:289-312` ⟵ `src/ui/OscillatorConfigPopup.cpp:391-412`
  - [diverged cross-file w=0.74 line=0.21] `test_harness/src/TestHttpServerTransport.cpp:185-209` ⟵ `test_harness/src/TestHttpServer.cpp:286-313`
  - [diverged cross-file w=0.74 line=0.75] `tests/test_capture_buffer_core.cpp:154-172` ⟵ `tests/test_shared_capture_buffer.cpp:162-180`
  - [diverged cross-file w=0.73 line=0.55] `test_harness/src/TestHttpServerTransport.cpp:160-181` ⟵ `test_harness/src/TestHttpServer.cpp:255-282`
  - [diverged cross-file w=0.72 line=0.22] `src/core/dsp/TimingEngine.cpp:174-208` ⟵ `src/dsp/TimingEngine.cpp:123-155`
  - [diverged cross-file w=0.72 line=0.57] `src/ui/theme/ThemeEditorComponent.cpp:256-283` ⟵ `src/ui/ThemeEditorComponent.cpp:399-426`
  - [diverged cross-file w=0.69 line=0.54] `test_harness/src/TestUIControllerSimulation.cpp:341-387` ⟵ `test_harness/src/TestUIController.cpp:315-367`
  - [diverged cross-file w=0.69 line=0.48] `src/tools/test_server/ScreenshotHandler.cpp:16-68` ⟵ `src/ui/PluginTestServer.cpp:596-639`
  - [diverged cross-file w=0.69 line=0.21] `src/ui/theme/ThemeEditorActions.cpp:175-197` ⟵ `src/ui/ThemeEditorComponent.cpp:522-554`
  - [diverged cross-file w=0.67 line=0.13] `src/ui/theme/ThemeEditorActions.cpp:154-171` ⟵ `src/ui/ThemeEditorComponent.cpp:490-518`
  - [diverged cross-file w=0.66 line=0.32] `src/ui/panels/SourceSelectorComponent.cpp:92-125` ⟵ `src/ui/SourceSelectorComponent.cpp:493-524`
  - [diverged cross-file w=0.65 line=0.67] `test_harness/src/TestHttpServerUI.cpp:181-216` ⟵ `test_harness/src/TestHttpServer.cpp:362-392`
  - [diverged cross-file w=0.64 line=0.42] `src/tools/test_server/WaveformHandler.cpp:24-44` ⟵ `src/ui/PluginTestServer.cpp:1159-1204`
  - [diverged cross-file w=0.61 line=0.02] `test_harness/src/TestHttpServerDiagnostic.cpp:199-219` ⟵ `src/ui/PluginTestServer.cpp:1159-1204`
  - [diverged cross-file w=0.60 line=0.39] `src/ui/components/SegmentedButtonBar.cpp:101-128` ⟵ `src/ui/SegmentedButtonBar.cpp:161-177`

## aapokaapo/JoyShockMapper-systemd — 130 коммитов | циклы=0(SCC≤1) | копипаст=1

### `5f5845cd90ce` — "Merge pull request #35 from aapokaapo/copilot/fix-desktop-notification-issue" (aapokaapo, 2026-05-01) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/aapokaapo_JoyShockMapper-systemd show 5f5845cd90ce`
  - [diverged cross-file w=0.70 line=0.27] `JoyShockMapper/src/win32/InputHelpers.cpp:237-244` ⟵ `JoyShockMapper/include/inputHelpers.cpp:48-53`

## grahame-org/ohal — 121 коммитов | циклы=1(SCC≤60) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=60. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/grahame-org_ohal`

## jjbudz/6502 — 108 коммитов | циклы=0(SCC≤1) | копипаст=4

### `0877729248bf` — "Merge pull request #46 from jjbudz/copilot/update-cheatsheet-with-options" (jjbudz, 2025-10-27) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/jjbudz_6502 show 0877729248bf`
  - [diverged cross-file w=0.94 line=0.50] `l6502.cpp:2762-2773` ⟵ `6502.cpp:2538-2547`
  - [diverged cross-file w=0.92 line=0.26] `l6502.cpp:3239-3255` ⟵ `6502.cpp:3218-3232`
  - [diverged cross-file w=0.77 line=0.50] `l6502.cpp:2744-2755` ⟵ `6502.cpp:2538-2547`
  - [diverged cross-file w=0.75 line=0.42] `l6502.cpp:3279-3317` ⟵ `6502.cpp:2909-2938`

## victorwitkamp/XePCI — 101 коммитов | циклы=18(SCC≤5) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 18 include-циклов, крупнейший SCC=5. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/victorwitkamp_XePCI`

## o9nn/ATenSpace — 98 коммитов | циклы=1(SCC≤7) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=7. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/o9nn_ATenSpace`

## flyingsurveyor/RTKino — 98 коммитов | циклы=0(SCC≤1) | копипаст=12

### `fa2b1c45a627` — "Merge pull request #27 from flyingsurveyor/copilot/fix-hmac-digest-byte-extraction" (flyingsurveyor, 2026-04-19) [12 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/flyingsurveyor_RTKino show fa2b1c45a627`
  - [near within-file w=1.00 line=0.82] `src/main.cpp:2445-2454` ⟵ `src/main.cpp:2169-2178`
  - [near within-file w=1.00 line=0.86] `src/main.cpp:1581-1593` ⟵ `src/main.cpp:1308-1320`
  - [near within-file w=1.00 line=0.91] `src/main.cpp:1453-1477` ⟵ `src/main.cpp:1183-1207`
  - [near within-file w=0.95 line=0.92] `src/main.cpp:2047-2067` ⟵ `src/main.cpp:1772-1791`
  - [diverged within-file w=0.94 line=0.79] `src/main.cpp:2705-2767` ⟵ `src/main.cpp:2406-2468`
  - [near within-file w=0.93 line=0.94] `src/main.cpp:2914-2955` ⟵ `src/main.cpp:2536-2575`
  - [near within-file w=0.92 line=0.83] `src/main.cpp:1034-1073` ⟵ `src/main.cpp:990-1028`
  - [near within-file w=0.90 line=0.81] `src/main.cpp:1813-1873` ⟵ `src/main.cpp:1537-1597`
  - [diverged within-file w=0.89 line=0.70] `lib/WebUI/WebUI.cpp:4040-4114` ⟵ `lib/WebUI/WebUI.cpp:3360-3431`
  - [near within-file w=0.89 line=0.80] `src/main.cpp:1655-1671` ⟵ `src/main.cpp:1383-1395`
  - [near within-file w=0.89 line=0.89] `src/main.cpp:1599-1632` ⟵ `src/main.cpp:1326-1360`
  - [diverged within-file w=0.71 line=0.69] `src/main.cpp:1116-1132` ⟵ `src/main.cpp:1073-1085`

## emeryberger/Hoard — 94 коммитов | циклы=0(SCC≤1) | копипаст=2

### `87e973255b0d` — "Merge pull request #93 from emeryberger/perf-optimizations" (Emery Berger, 2026-05-29) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/emeryberger_Hoard show 87e973255b0d`
  - [near within-file w=0.96 line=0.88] `src/include/hoard/thresholdheap.h:183-200` ⟵ `src/include/hoard/thresholdheap.h:205-222`
  - [diverged cross-file w=0.69 line=0.57] `benchmarks/cache-thrash/cache-thrash.cpp:101-139` ⟵ `benchmarks/cache-scratch/cache-scratch.cpp:101-146`

## brunusansi/NanaBox — 92 коммитов | циклы=0(SCC≤1) | копипаст=12

### `ce774438b088` — "Merge pull request #9 from brunusansi/copilot/featurehvfilter-phase3b-cpuid-msr" (bRu, 2025-12-04) [12 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/brunusansi_NanaBox show ce774438b088`
  - [verbatim within-file w=1.00 line=1.00] `NanaBox/ConfigurationManager.cpp:723-832` ⟵ `NanaBox/ConfigurationManager.cpp:126-235`
  - [verbatim within-file w=1.00 line=1.00] `NanaBox/ConfigurationManager.cpp:965-982` ⟵ `NanaBox/ConfigurationManager.cpp:368-385`
  - [verbatim within-file w=1.00 line=1.00] `NanaBox/ConfigurationManager.cpp:994-1010` ⟵ `NanaBox/ConfigurationManager.cpp:397-413`
  - [verbatim cross-file w=1.00 line=1.00] `NanaBox/MainWindow.cpp:257-298` ⟵ `NanaBox/NanaBox.cpp:566-607`
  - [near cross-file w=1.00 line=0.89] `NanaBox/SponsorPage.cpp:28-53` ⟵ `NanaBox/QuickStartPage.cpp:20-45`
  - [near within-file w=1.00 line=0.92] `NanaBox/NanaBox.cpp:360-405` ⟵ `NanaBox/NanaBox.cpp:1114-1159`
  - [diverged cross-file w=0.97 line=0.35] `NanaBox/MainWindow.cpp:1174-1214` ⟵ `NanaBox/NanaBox.cpp:410-450`
  - [diverged cross-file w=0.91 line=0.62] `NanaBox/MainWindow.cpp:405-499` ⟵ `NanaBox/NanaBox.cpp:710-789`
  - [near cross-file w=0.91 line=0.85] `NanaBox/ResizeVirtualHardDiskPage.cpp:43-60` ⟵ `NanaBox/NewVirtualHardDiskPage.cpp:37-51`
  - [near cross-file w=0.90 line=0.82] `NanaBox/MainWindow.cpp:304-378` ⟵ `NanaBox/NanaBox.cpp:613-683`
  - [diverged cross-file w=0.72 line=0.45] `NanaBox/MainWindow.cpp:1033-1096` ⟵ `NanaBox/NanaBox.cpp:311-350`
  - [diverged within-file w=0.62 line=0.32] `NanaBox/ConfigurationManager.cpp:504-538` ⟵ `NanaBox/ConfigurationManager.cpp:55-120`

## bradhawkins85/ESP32-Uptime-Monitoring-Touch — 91 коммитов | циклы=0(SCC≤1) | копипаст=7

### `b0302856b4fc` — "Merge pull request #13 from bradhawkins85/codex/prevent-service-modifications-for-unauthenticated-users" (bradhawkins85, 2025-11-27) [7 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/bradhawkins85_ESP32-Uptime-Monitoring-Touch show b0302856b4fc`
  - [near within-file w=0.91 line=0.95] `src/main.cpp:550-578` ⟵ `src/main.cpp:227-251`
  - [near within-file w=0.87 line=0.88] `src/main.cpp:299-322` ⟵ `src/main.cpp:64-84`
  - [near within-file w=0.84 line=0.89] `src/main.cpp:325-337` ⟵ `src/main.cpp:87-97`
  - [diverged within-file w=0.83 line=0.79] `src/main.cpp:1234-1262` ⟵ `src/main.cpp:388-412`
  - [near within-file w=0.83 line=0.88] `src/main.cpp:1265-1308` ⟵ `src/main.cpp:415-454`
  - [near within-file w=0.82 line=0.82] `src/main.cpp:478-545` ⟵ `src/main.cpp:170-222`
  - [near within-file w=0.78 line=0.86] `src/main.cpp:1321-1932` ⟵ `src/main.cpp:467-979`

## Josh-Archer/ESP32-Smart-Monitor — 87 коммитов | циклы=0(SCC≤1) | копипаст=1

### `87eebf6fe124` — "Automatic version update to v2.9.0" (GitHub Action, 2025-09-18) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Josh-Archer_ESP32-Smart-Monitor show 87eebf6fe124`
  - [diverged within-file w=0.63 line=0.51] `src/main.cpp:168-233` ⟵ `src/main.cpp:96-139`

## UofUEpiBio/epiworld — 82 коммитов | циклы=0(SCC≤1) | копипаст=7

### `c966f02f791b` — "Adding rash as transmission in measlesmixing (#252)" (George G. Vega Yon, 2026-05-11) [7 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/UofUEpiBio_epiworld show c966f02f791b`
  - [verbatim within-file w=1.00 line=1.00] `epiworld.hpp:3532-3549` ⟵ `epiworld.hpp:2299-2316`
  - [verbatim within-file w=1.00 line=1.00] `epiworld.hpp:7715-7723` ⟵ `epiworld.hpp:5116-5124`
  - [verbatim cross-file w=1.00 line=1.00] `include/epiworld/randgraph.hpp:624-632` ⟵ `epiworld.hpp:5116-5124`
  - [diverged within-file w=0.93 line=0.51] `epiworld.hpp:1199-1244` ⟵ `epiworld.hpp:8364-8404`
  - [diverged cross-file w=0.93 line=0.51] `include/epiworld/misc.hpp:285-330` ⟵ `epiworld.hpp:8364-8404`
  - [diverged within-file w=0.67 line=0.41] `epiworld.hpp:12883-12944` ⟵ `epiworld.hpp:7865-7926`
  - [diverged cross-file w=0.67 line=0.41] `include/epiworld/model-meat.hpp:1863-1924` ⟵ `epiworld.hpp:7865-7926`

## syoyo/tinyexr — 81 коммитов | циклы=0(SCC≤1) | копипаст=14

### `4946b5d92e13` — "Merge pull request #243 from syoyo/codex/fix-windows-arm-ci-20260331" (Syoyo Fujita, 2026-03-31) [14 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/syoyo_tinyexr show 4946b5d92e13`
  - [diverged cross-file w=1.00 line=0.50] `deps/miniz/miniz.c:199-231` ⟵ `tinyexr.h:1691-1727`
  - [diverged cross-file w=1.00 line=0.49] `deps/miniz/miniz.c:244-298` ⟵ `tinyexr.h:1740-1792`
  - [diverged cross-file w=1.00 line=0.30] `deps/miniz/miniz.c:1018-1042` ⟵ `tinyexr.h:3121-3139`
  - [diverged cross-file w=1.00 line=0.56] `deps/miniz/miniz.c:1839-1861` ⟵ `tinyexr.h:3903-3919`
  - [diverged cross-file w=1.00 line=0.48] `deps/miniz/miniz.c:3451-3467` ⟵ `tinyexr.h:4571-4587`
  - [diverged cross-file w=1.00 line=0.71] `deps/miniz/miniz.c:321-347` ⟵ `tinyexr.h:1814-1839`
  - [near within-file w=0.98 line=0.82] `tinyexr.h:2605-2650` ⟵ `tinyexr.h:7872-7919`
  - [diverged cross-file w=0.95 line=0.31] `deps/miniz/miniz.c:2649-2674` ⟵ `tinyexr.h:2434-2460`
  - [near within-file w=0.94 line=0.82] `tinyexr.h:3104-3159` ⟵ `tinyexr.h:8330-8386`
  - [diverged cross-file w=0.94 line=0.54] `deps/miniz/miniz.c:564-591` ⟵ `tinyexr.h:2035-2061`
  - [diverged cross-file w=0.91 line=0.40] `deps/miniz/miniz.c:2503-2542` ⟵ `tinyexr.h:2305-2345`
  - [diverged cross-file w=0.90 line=0.22] `deps/miniz/miniz.c:3571-3619` ⟵ `tinyexr.h:4691-4736`
  - [diverged within-file w=0.83 line=0.46] `tinyexr.h:3074-3100` ⟵ `tinyexr.h:8299-8326`
  - [diverged cross-file w=0.73 line=0.13] `deps/miniz/miniz.c:6150-6181` ⟵ `tinyexr.h:5941-5976`

## supaplextor/aqemu — 74 коммитов | циклы=1(SCC≤2) | копипаст=35
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/supaplextor_aqemu`

### `439043b0880b` — "Merge pull request #32 from supaplextor/copilot/architecture-fix-issue" (Scott Edwards, 2026-05-29) [35 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/supaplextor_aqemu show 439043b0880b`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2756-2781` ⟵ `Main_Window.cpp:4090-4115`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:3918-3924` ⟵ `VM_Wizard_Window.cpp:751-757`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:3928-3985` ⟵ `Main_Window.cpp:5212-5269`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2871-2902` ⟵ `Main_Window.cpp:4188-4219`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2837-2849` ⟵ `Main_Window.cpp:4171-4183`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2820-2832` ⟵ `Main_Window.cpp:4154-4166`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2803-2815` ⟵ `Main_Window.cpp:4137-4149`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2786-2798` ⟵ `Main_Window.cpp:4120-4132`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2549-2584` ⟵ `Main_Window.cpp:3850-3885`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2519-2525` ⟵ `Main_Window.cpp:3820-3826`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:2012-2053` ⟵ `Main_Window.cpp:3428-3469`
  - [verbatim cross-file w=1.00 line=1.00] `src/Advanced_Settings_Window.cpp:828-844` ⟵ `Advanced_Settings_Window.cpp:513-529`
  - [verbatim cross-file w=1.00 line=1.00] `src/Advanced_Settings_Window.cpp:865-885` ⟵ `Advanced_Settings_Window.cpp:554-574`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:844-857` ⟵ `Main_Window.cpp:721-734`
  - [verbatim cross-file w=1.00 line=1.00] `src/Main_Window.cpp:4622-4625` ⟵ `Main_Window.cpp:6451-6454`
  - [diverged cross-file w=1.00 line=0.79] `src/Main_Window.cpp:3683-3719` ⟵ `Main_Window.cpp:4981-5017`
  - [near cross-file w=0.99 line=0.81] `src/Main_Window.cpp:4473-4505` ⟵ `Main_Window.cpp:6299-6331`
  - [verbatim cross-file w=0.96 line=0.97] `src/Main_Window.cpp:2326-2395` ⟵ `Main_Window.cpp:3628-3696`
  - [diverged cross-file w=0.96 line=0.75] `src/Advanced_Settings_Window.cpp:356-365` ⟵ `Settings_Window.cpp:192-201`
  - [diverged cross-file w=0.92 line=0.45] `src/Main_Window.cpp:4443-4469` ⟵ `Main_Window.cpp:6269-6295`
  - [near cross-file w=0.90 line=0.85] `src/Main_Window.cpp:1260-1303` ⟵ `Main_Window.cpp:1229-1271`
  - [diverged cross-file w=0.90 line=0.53] `src/Main_Window.cpp:3222-3261` ⟵ `Main_Window.cpp:4666-4702`
  - [near cross-file w=0.88 line=0.87] `src/Main_Window.cpp:1227-1247` ⟵ `Main_Window.cpp:1197-1214`
  - [diverged cross-file w=0.87 line=0.56] `src/main.cpp:295-357` ⟵ `main.cpp:141-195`
  - [diverged cross-file w=0.86 line=0.46] `src/Embedded_Display/vncclientthread.cpp:331-343` ⟵ `Embedded_Display/vncclientthread.cpp:161-168`
  - [diverged cross-file w=0.84 line=0.48] `src/Advanced_Settings_Window.cpp:780-806` ⟵ `Advanced_Settings_Window.cpp:428-454`
  - [diverged cross-file w=0.82 line=0.38] `src/Main_Window.cpp:2991-3025` ⟵ `Main_Window.cpp:4300-4336`
  - [diverged cross-file w=0.76 line=0.10] `src/main.cpp:243-252` ⟵ `main.cpp:293-297`
  - [diverged cross-file w=0.74 line=0.66] `src/Main_Window.cpp:4519-4578` ⟵ `Main_Window.cpp:6349-6407`
  - [diverged cross-file w=0.73 line=0.43] `src/Main_Window.cpp:3151-3174` ⟵ `Main_Window.cpp:4459-4483`
  - [diverged cross-file w=0.66 line=0.50] `src/Main_Window.cpp:3824-3898` ⟵ `Main_Window.cpp:5125-5182`
  - [diverged cross-file w=0.65 line=0.63] `src/Advanced_Settings_Window.cpp:933-967` ⟵ `Advanced_Settings_Window.cpp:622-671`
  - [diverged cross-file w=0.65 line=0.38] `src/Embedded_Display/vncclientthread.cpp:147-183` ⟵ `Embedded_Display/vncclientthread.cpp:33-76`
  - [diverged cross-file w=0.63 line=0.39] `src/main.cpp:442-489` ⟵ `main.cpp:57-91`
  - [diverged cross-file w=0.61 line=0.67] `src/Main_Window.cpp:3329-3355` ⟵ `Main_Window.cpp:4715-4740`

## mi0e/subconverter — 69 коммитов | циклы=0(SCC≤1) | копипаст=59

### `808bd96d22d6` — "Merge pull request #16 from mi0e/copilot/update-config-to-true-delete-tun-settings" (mi0e, 2026-05-08) [59 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/mi0e_subconverter show 808bd96d22d6`
  - [diverged within-file w=1.00 line=0.25] `include/inja.hpp:298-303` ⟵ `include/inja.hpp:1856-1861`
  - [diverged within-file w=1.00 line=0.33] `src/handler/interfaces.cpp:725-729` ⟵ `src/handler/interfaces.cpp:1726-1730`
  - [verbatim within-file w=1.00 line=1.00] `include/nlohmann/json.hpp:11485-11493` ⟵ `include/nlohmann/json.hpp:8441-8449`
  - [diverged within-file w=1.00 line=0.78] `include/nlohmann/json.hpp:16048-16059` ⟵ `include/nlohmann/json.hpp:13802-13813`
  - [diverged within-file w=1.00 line=0.71] `include/nlohmann/json.hpp:16191-16199` ⟵ `include/nlohmann/json.hpp:13944-13952`
  - [near within-file w=1.00 line=0.93] `include/nlohmann/json.hpp:17673-17702` ⟵ `include/nlohmann/json.hpp:15224-15253`
  - [verbatim within-file w=1.00 line=1.00] `include/inja.hpp:292-295` ⟵ `include/inja.hpp:1850-1853`
  - [diverged within-file w=1.00 line=0.55] `src/parser/subparser.cpp:653-661` ⟵ `src/parser/subparser.cpp:396-404`
  - [verbatim cross-file w=1.00 line=1.00] `src/utils/lock.h:65-76` ⟵ `src/handler/webget.cpp:82-93`
  - [diverged within-file w=1.00 line=0.17] `include/inja.hpp:2889-2893` ⟵ `include/inja.hpp:4124-4125`
  - [diverged within-file w=1.00 line=0.67] `include/inja.hpp:2042-2047` ⟵ `include/inja.hpp:3307-3312`
  - [near within-file w=1.00 line=0.87] `include/quickjspp.hpp:1550-1565` ⟵ `include/quickjspp.hpp:971-986`
  - [near within-file w=1.00 line=0.86] `include/quickjspp.hpp:1386-1398` ⟵ `include/quickjspp.hpp:866-878`
  - [near within-file w=1.00 line=0.85] `include/quickjspp.hpp:1404-1415` ⟵ `include/quickjspp.hpp:884-895`
  - [near within-file w=1.00 line=0.82] `include/quickjspp.hpp:104-118` ⟵ `include/quickjspp.hpp:69-83`
  - [diverged cross-file w=0.99 line=0.60] `src/handler/settings.cpp:470-478` ⟵ `src/handler/interfaces.cpp:774-782`
  - [diverged cross-file w=0.99 line=0.78] `src/generator/config/nodemanip.cpp:470-517` ⟵ `src/generator/config/subexport.cpp:245-292`
  - [diverged within-file w=0.99 line=0.50] `include/nlohmann/json.hpp:15845-15850` ⟵ `include/nlohmann/json.hpp:13600-13605`
  - [diverged within-file w=0.99 line=0.67] `include/inja.hpp:2793-2797` ⟵ `include/inja.hpp:4031-4035`
  - [verbatim within-file w=0.98 line=0.96] `include/nlohmann/json.hpp:16974-17037` ⟵ `include/nlohmann/json.hpp:14523-14586`
  - [near within-file w=0.97 line=0.94] `include/nlohmann/json.hpp:10083-10168` ⟵ `include/nlohmann/json.hpp:8584-8668`
  - [diverged within-file w=0.97 line=0.14] `include/quickjspp.hpp:618-621` ⟵ `include/quickjspp.hpp:308-311`
  - [diverged within-file w=0.97 line=0.52] `src/generator/config/subexport.cpp:2035-2054` ⟵ `src/generator/config/subexport.cpp:1658-1674`
  - [diverged within-file w=0.97 line=0.25] `include/inja.hpp:2256-2273` ⟵ `include/inja.hpp:3487-3507`
  - [diverged within-file w=0.96 line=0.56] `include/inja.hpp:2639-2646` ⟵ `include/inja.hpp:3903-3911`
  - [near within-file w=0.96 line=0.83] `include/nlohmann/json.hpp:16082-16119` ⟵ `include/nlohmann/json.hpp:13836-13872`
  - [near within-file w=0.96 line=0.83] `include/nlohmann/json.hpp:16130-16166` ⟵ `include/nlohmann/json.hpp:13884-13919`
  - [near within-file w=0.95 line=0.88] `src/generator/template/templates.cpp:547-567` ⟵ `src/generator/template/templates.cpp:473-493`
  - [diverged within-file w=0.95 line=0.76] `include/nlohmann/json.hpp:14490-14513` ⟵ `include/nlohmann/json.hpp:12449-12472`
  - [diverged within-file w=0.94 line=0.75] `src/generator/template/templates.cpp:360-383` ⟵ `src/generator/template/templates.cpp:306-329`
  - [near within-file w=0.94 line=0.88] `include/nlohmann/json.hpp:12193-12243` ⟵ `include/nlohmann/json.hpp:10198-10249`
  - [diverged within-file w=0.92 line=0.56] `include/quickjspp.hpp:1695-1701` ⟵ `include/quickjspp.hpp:1100-1106`
  - [diverged within-file w=0.92 line=0.60] `src/handler/webget.cpp:210-231` ⟵ `src/handler/webget.cpp:232-247`
  - [near within-file w=0.92 line=0.88] `include/quickjspp.hpp:1099-1130` ⟵ `include/quickjspp.hpp:614-645`
  - [diverged within-file w=0.92 line=0.64] `include/nlohmann/json.hpp:14349-14405` ⟵ `include/nlohmann/json.hpp:12278-12336`
  - [near within-file w=0.91 line=0.89] `include/nlohmann/json.hpp:24030-24111` ⟵ `include/nlohmann/json.hpp:24838-24918`
  - [near within-file w=0.90 line=0.83] `include/nlohmann/json.hpp:9349-9412` ⟵ `include/nlohmann/json.hpp:7899-7960`
  - [diverged within-file w=0.89 line=0.56] `src/parser/subparser.cpp:390-450` ⟵ `src/parser/subparser.cpp:117-181`
  - [diverged within-file w=0.89 line=0.75] `include/nlohmann/json.hpp:7018-7050` ⟵ `include/nlohmann/json.hpp:5607-5632`
  - [diverged within-file w=0.89 line=0.72] `include/nlohmann/json.hpp:16678-16701` ⟵ `include/nlohmann/json.hpp:14253-14269`
  - [near within-file w=0.87 line=0.80] `include/nlohmann/json.hpp:4710-4720` ⟵ `include/nlohmann/json.hpp:3534-3544`
  - [near within-file w=0.86 line=0.86] `include/nlohmann/json.hpp:17850-17898` ⟵ `include/nlohmann/json.hpp:15401-15442`
  - [diverged within-file w=0.86 line=0.75] `include/nlohmann/json.hpp:9321-9334` ⟵ `include/nlohmann/json.hpp:7872-7884`
  - [near within-file w=0.86 line=0.89] `src/server/webserver_libevent.cpp:239-247` ⟵ `src/server/webserver_libevent.cpp:200-207`
  - [diverged within-file w=0.84 line=0.32] `src/generator/config/subexport.cpp:890-935` ⟵ `src/generator/config/subexport.cpp:593-637`
  - [diverged within-file w=0.84 line=0.29] `include/nlohmann/json.hpp:19558-19569` ⟵ `include/nlohmann/json.hpp:17408-17419`
  - [diverged cross-file w=0.83 line=0.46] `src/handler/settings.cpp:408-418` ⟵ `src/handler/interfaces.cpp:721-730`
  - [diverged within-file w=0.82 line=0.67] `include/nlohmann/json.hpp:11666-11742` ⟵ `include/nlohmann/json.hpp:9755-9823`
  - [diverged within-file w=0.82 line=0.69] `include/nlohmann/json.hpp:9184-9231` ⟵ `include/nlohmann/json.hpp:7738-7783`
  - [diverged cross-file w=0.79 line=0.62] `src/handler/settings.cpp:344-359` ⟵ `src/handler/interfaces.cpp:661-674`
  - [diverged within-file w=0.79 line=0.43] `include/nlohmann/json.hpp:16067-16073` ⟵ `include/nlohmann/json.hpp:13821-13827`
  - [diverged within-file w=0.78 line=0.39] `src/server/webserver_libevent.cpp:275-300` ⟵ `src/server/webserver_libevent.cpp:233-264`
  - [diverged within-file w=0.77 line=0.44] `src/utils/logger.cpp:65-71` ⟵ `src/utils/logger.cpp:41-46`
  - [diverged within-file w=0.77 line=0.75] `include/nlohmann/json.hpp:6189-6196` ⟵ `include/nlohmann/json.hpp:4825-4832`
  - [diverged within-file w=0.77 line=0.59] `src/server/webserver_libevent.cpp:306-335` ⟵ `src/server/webserver_libevent.cpp:270-300`
  - [diverged within-file w=0.75 line=0.51] `src/handler/interfaces.cpp:466-501` ⟵ `src/handler/interfaces.cpp:1447-1484`
  - [diverged cross-file w=0.66 line=0.34] `src/handler/settings.cpp:249-281` ⟵ `src/handler/interfaces.cpp:560-600`
  - [diverged within-file w=0.64 line=0.74] `include/nlohmann/json.hpp:8837-8895` ⟵ `include/nlohmann/json.hpp:7383-7466`
  - [diverged within-file w=0.63 line=0.47] `include/nlohmann/json.hpp:12566-12591` ⟵ `include/nlohmann/json.hpp:10574-10599`

## shelbeely/fallout1-ce — 65 коммитов | циклы=0(SCC≤1) | копипаст=3

### `9220f783d088` — "Update Fallout wiki cache [skip ci]" (github-actions[bot], 2026-05-18) [3 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/shelbeely_fallout1-ce show 9220f783d088`
  - [diverged within-file w=1.00 line=0.67] `src/game/inventry.cc:4622-4633` ⟵ `src/game/inventry.cc:4542-4553`
  - [diverged within-file w=0.67 line=0.61] `src/plib/db/db.cc:2565-2604` ⟵ `src/plib/db/db.cc:2607-2634`
  - [diverged within-file w=0.60 line=0.75] `src/game/scripts.cc:909-970` ⟵ `src/game/scripts.cc:811-876`

## onouh/FibonacciHeap — 64 коммитов | циклы=1(SCC≤2) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/onouh_FibonacciHeap`

## lwqwag/PclCSharp — 62 коммитов | циклы=0(SCC≤1) | копипаст=16

### `ab87843f1ecc` — "feat: Implement KDTree and Octree search functionalities" (lwq, 2026-05-22) [16 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/lwqwag_PclCSharp show ab87843f1ecc`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:73-100` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:75-102`
  - [near cross-file w=1.00 line=0.81] `src/PclSharpWrapper/PclSharpWrapper/util_module.cpp:341-366` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:675-700`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/util_module.cpp:29-45` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:363-379`
  - [diverged cross-file w=1.00 line=0.60] `src/PclSharpWrapper/PclSharpWrapper/util_module.cpp:15-23` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:349-357`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/segmentation.cpp:69-98` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:288-317`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/segmentation.cpp:18-42` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:237-261`
  - [diverged cross-file w=1.00 line=0.74] `src/PclSharpWrapper/PclSharpWrapper/sampleConsensus.cpp:15-36` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:322-343`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/filter.cpp:48-64` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:186-202`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:42-60` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:44-62`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:33-38` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:35-40`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:15-29` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:17-31`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/io.cpp:112-121` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:158-167`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/io.cpp:99-108` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:145-154`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/io.cpp:16-26` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:106-116`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/filter.cpp:85-90` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:223-228`
  - [verbatim cross-file w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/filter.cpp:70-76` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:208-214`

## cmcxn/downloader — 61 коммитов | циклы=0(SCC≤1) | копипаст=4

### `f09d15634172` — "Merge pull request #17 from cmcxn/codex/fix-hex-conversion-in-getinfohashhex" (DuKuangjin, 2025-09-20) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/cmcxn_downloader show f09d15634172`
  - [diverged within-file w=0.85 line=0.68] `cpp_xl_dl_demo/http.h:88-112` ⟵ `cpp_xl_dl_demo/http.h:58-85`
  - [diverged cross-file w=0.76 line=0.26] `cpp_xl_dl_demo/download_helper.cpp:149-160` ⟵ `cpp_xl_dl_demo/main.cpp:33-52`
  - [diverged cross-file w=0.73 line=0.26] `cpp_xl_dl_demo/download_helper.cpp:135-146` ⟵ `cpp_xl_dl_demo/main.cpp:33-52`
  - [diverged within-file w=0.64 line=0.75] `cpp_xl_dl_demo/http.h:58-85` ⟵ `cpp_xl_dl_demo/http.h:21-53`

## deramatamara-lab/cppmusic — 58 коммитов | циклы=0(SCC≤1) | копипаст=15

### `321928f611c0` — "Merge pull request #10 from deramatamara-lab/copilot/elevate-ui-to-production-grade" (deramatamara-lab, 2025-12-03) [15 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/deramatamara-lab_cppmusic show 321928f611c0`
  - [verbatim within-file w=1.00 line=1.00] `src/audio/engine/DawEngine.cpp:393-415` ⟵ `src/audio/engine/DawEngine.cpp:285-307`
  - [verbatim within-file w=1.00 line=1.00] `src/plugins/PluginHost.cpp:119-135` ⟵ `src/plugins/PluginHost.cpp:68-84`
  - [verbatim within-file w=1.00 line=1.00] `src/ui/App.cpp:80-113` ⟵ `src/ui/App.cpp:45-78`
  - [verbatim within-file w=1.00 line=1.00] `src/ui/MainWindow.cpp:370-387` ⟵ `src/ui/MainWindow.cpp:83-100`
  - [verbatim within-file w=1.00 line=1.00] `src/ui/MainWindow.cpp:391-403` ⟵ `src/ui/MainWindow.cpp:104-116`
  - [verbatim within-file w=1.00 line=1.00] `src/ui/views/PianoRollView.cpp:270-286` ⟵ `src/ui/views/PianoRollView.cpp:179-195`
  - [verbatim within-file w=1.00 line=1.00] `src/ui/views/TransportBar.cpp:500-519` ⟵ `src/ui/views/TransportBar.cpp:182-201`
  - [diverged within-file w=0.87 line=0.70] `src/ui/views/MixerStrip.cpp:262-274` ⟵ `src/ui/views/MixerStrip.cpp:169-177`
  - [diverged within-file w=0.87 line=0.70] `src/ui/views/MixerStrip.cpp:246-258` ⟵ `src/ui/views/MixerStrip.cpp:157-165`
  - [diverged within-file w=0.82 line=0.70] `src/ui/views/MixerStrip.cpp:99-135` ⟵ `src/ui/views/MixerStrip.cpp:62-90`
  - [diverged within-file w=0.80 line=0.58] `src/ai/inference/InferenceEngine.cpp:69-93` ⟵ `src/ai/inference/InferenceEngine.cpp:45-64`
  - [diverged within-file w=0.73 line=0.60] `src/ui/components/StepSequencer.cpp:89-98` ⟵ `src/ui/components/StepSequencer.cpp:73-80`
  - [diverged within-file w=0.70 line=0.62] `src/ui/views/ArrangeView.cpp:898-961` ⟵ `src/ui/views/ArrangeView.cpp:191-244`
  - [diverged within-file w=0.67 line=0.52] `src/core/utilities/Logger.cpp:63-95` ⟵ `src/core/utilities/Logger.cpp:12-29`
  - [diverged within-file w=0.66 line=0.58] `src/ui/views/PianoRollView.cpp:192-204` ⟵ `src/ui/views/PianoRollView.cpp:138-149`

## asklar/lvt — 58 коммитов | циклы=0(SCC≤1) | копипаст=1

### `f6d446e7507b` — "Fix WinUI3 headers missing in release builds" (Alexander Sklar, 2026-03-02) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/asklar_lvt show f6d446e7507b`
  - [diverged cross-file w=0.78 line=0.71] `src/target.cpp:92-97` ⟵ `src/providers/win32_provider.cpp:22-27`

## albpara/DMDDelorean — 57 коммитов | циклы=0(SCC≤1) | копипаст=2

### `b9bd7a5dbe9a` — "Merge pull request #14 from albpara/copilot/add-mqtt-logging-toggle" (albpara, 2026-03-27) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/albpara_DMDDelorean show b9bd7a5dbe9a`
  - [diverged within-file w=0.76 line=0.54] `src/main.cpp:912-936` ⟵ `src/main.cpp:392-414`
  - [diverged within-file w=0.71 line=0.45] `src/main.cpp:887-906` ⟵ `src/main.cpp:369-386`

## Zero3K20/hpsx64 — 54 коммитов | циклы=3(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 3 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/Zero3K20_hpsx64`

## ReZorg/plingua — 52 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/ReZorg_plingua`

## RegimA-Zone/RRR-P-Systems — 52 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/RegimA-Zone_RRR-P-Systems`

## Autonomy-Logic/matiec — 52 коммитов | циклы=2(SCC≤2) | копипаст=21
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/Autonomy-Logic_matiec`

### `75d3755f0ad5` — "feat: allow communication.h inclusion in simulator mode" (Thiago Alves, 2026-02-26) [21 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Autonomy-Logic_matiec show 75d3755f0ad5`
  - [verbatim cross-file w=1.00 line=1.00] `stage4/generate_c/generate_c_il.cc:657-684` ⟵ `stage4/generate_c/generate_c_st.cc:274-301`
  - [verbatim within-file w=1.00 line=1.00] `stage4/generate_c/generate_var_list.cc:975-999` ⟵ `stage4/generate_c/generate_var_list.cc:653-677`
  - [verbatim within-file w=1.00 line=1.00] `stage4/generate_c/generate_c_vardecl.cc:1206-1219` ⟵ `stage4/generate_c/generate_c_vardecl.cc:903-916`
  - [verbatim cross-file w=1.00 line=1.00] `lib/iec_std_functions.h:1045-1067` ⟵ `lib/iec_std_lib.h:1604-1626`
  - [diverged within-file w=1.00 line=0.75] `stage3/constant_folding.cc:369-401` ⟵ `stage3/constant_folding.cc:333-365`
  - [diverged within-file w=0.95 line=0.35] `stage4/generate_c/generate_c_typedecl.cc:455-492` ⟵ `stage4/generate_c/generate_c_typedecl.cc:241-277`
  - [near within-file w=0.94 line=0.84] `stage4/generate_c/generate_c.cc:1933-1994` ⟵ `stage4/generate_c/generate_c.cc:2029-2090`
  - [verbatim within-file w=0.91 line=0.95] `stage3/fill_candidate_datatypes.cc:497-549` ⟵ `stage3/fill_candidate_datatypes.cc:295-347`
  - [diverged within-file w=0.91 line=0.71] `stage4/generate_c/generate_c_sfcdecl.cc:303-342` ⟵ `stage4/generate_c/generate_c_sfcdecl.cc:239-277`
  - [diverged within-file w=0.91 line=0.79] `stage4/generate_c/generate_c_vardecl.cc:887-931` ⟵ `stage4/generate_c/generate_c_vardecl.cc:598-640`
  - [near within-file w=0.84 line=0.87] `lib/iec_std_lib.h:618-649` ⟵ `lib/iec_std_lib.h:458-489`
  - [diverged within-file w=0.82 line=0.78] `stage4/generate_c/generate_c_vardecl.cc:2240-2306` ⟵ `stage4/generate_c/generate_c_vardecl.cc:1830-1887`
  - [diverged within-file w=0.81 line=0.44] `stage3/narrow_candidate_datatypes.cc:1704-1723` ⟵ `stage3/narrow_candidate_datatypes.cc:1208-1216`
  - [near cross-file w=0.80 line=0.80] `stage4/generate_c/generate_c_st.cc:652-680` ⟵ `stage4/generate_c/generate_c_il.cc:523-550`
  - [diverged within-file w=0.77 line=0.40] `stage3/lvalue_check.cc:274-281` ⟵ `stage3/lvalue_check.cc:247-252`
  - [diverged within-file w=0.76 line=0.23] `stage4/generate_c/generate_c_typedecl.cc:402-450` ⟵ `stage4/generate_c/generate_c_typedecl.cc:195-237`
  - [diverged within-file w=0.76 line=0.57] `absyntax_utils/function_param_iterator.cc:167-205` ⟵ `absyntax_utils/function_param_iterator.cc:208-241`
  - [diverged cross-file w=0.74 line=0.20] `absyntax_utils/get_datatype_info.cc:407-412` ⟵ `absyntax_utils/search_varfb_instance_type.cc:381-396`
  - [diverged within-file w=0.68 line=0.48] `stage4/generate_c/generate_c.cc:2589-2617` ⟵ `stage4/generate_c/generate_c.cc:2355-2379`
  - [diverged within-file w=0.64 line=0.66] `stage4/generate_c/generate_c.cc:2708-2755` ⟵ `stage4/generate_c/generate_c.cc:2466-2516`
  - [diverged within-file w=0.62 line=0.48] `stage4/generate_c/generate_c_sfc.cc:416-438` ⟵ `stage4/generate_c/generate_c_sfc.cc:459-485`

## TheOriginalBytePlayer/ozz-animationCal3DWrapper — 49 коммитов | циклы=0(SCC≤1) | копипаст=25

### `280722b79f6e` — "Merge branch 'master' of https://github.com/TheOriginalBytePlayer/ozz-animationCal3DWrapper" (TheOriginalBytePlayer, 2025-10-31) [25 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/TheOriginalBytePlayer_ozz-animationCal3DWrapper show 280722b79f6e`
  - [verbatim within-file w=1.00 line=1.00] `test/options/options_tests.cc:472-504` ⟵ `test/options/options_tests.cc:435-467`
  - [diverged cross-file w=1.00 line=0.50] `test/options/options_registration_tests.cc:122-131` ⟵ `test/options/options_registration_empty_tests.cc:75-83`
  - [diverged cross-file w=1.00 line=0.50] `test/options/options_registration_empty_tests.cc:75-84` ⟵ `test/options/options_registration_tests.cc:119-127`
  - [diverged within-file w=1.00 line=0.50] `src/animation/offline/skeleton_builder.cc:126-150` ⟵ `src/animation/offline/skeleton_builder.cc:158-181`
  - [verbatim cross-file w=1.00 line=1.00] `samples/framework/internal/shader.cc:65-76` ⟵ `demo/framework/internal/renderer_impl.cc:628-639`
  - [diverged cross-file w=0.95 line=0.53] `samples/framework/internal/imgui_impl.cc:1455-1474` ⟵ `demo/framework/internal/imgui_impl.cc:1235-1253`
  - [diverged cross-file w=0.89 line=0.67] `cal3d-src/src/cal3d/tinyxmlparser.cpp:392-437` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:650-700`
  - [diverged cross-file w=0.89 line=0.50] `samples/framework/internal/camera.cc:178-200` ⟵ `demo/framework/internal/camera.cc:86-102`
  - [near cross-file w=0.85 line=0.86] `cal3d-src/src/cal3d/tinyxml.cpp:464-470` ⟵ `contrib/tinyxml/tinyxml.cpp:567-574`
  - [near within-file w=0.83 line=0.85] `test/base/maths/simd_int_math_tests.cc:372-389` ⟵ `test/base/maths/simd_int_math_tests.cc:287-301`
  - [diverged cross-file w=0.82 line=0.61] `samples/framework/internal/shader.cc:80-103` ⟵ `demo/framework/internal/renderer_impl.cc:685-707`
  - [diverged cross-file w=0.82 line=0.46] `cal3d-src/src/cal3d/tinyxmlparser.cpp:818-872` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:1181-1247`
  - [diverged within-file w=0.81 line=0.57] `include/ozz/base/maths/internal/simd_math_sse-inl.h:1582-1601` ⟵ `include/ozz/base/maths/internal/simd_math_sse-inl.h:1435-1455`
  - [diverged within-file w=0.81 line=0.57] `include/ozz/base/maths/internal/simd_math_sse-inl.h:1562-1579` ⟵ `include/ozz/base/maths/internal/simd_math_sse-inl.h:1435-1455`
  - [diverged within-file w=0.79 line=0.67] `include/ozz/base/maths/internal/simd_math_sse-inl.h:1058-1062` ⟵ `include/ozz/base/maths/internal/simd_math_sse-inl.h:974-978`
  - [diverged cross-file w=0.79 line=0.08] `src/animation/runtime/sampling_job.cc:432-453` ⟵ `src/animation/sampling_job.cc:294-315`
  - [diverged within-file w=0.76 line=0.50] `include/ozz/base/containers/intrusive_list.h:673-676` ⟵ `include/ozz/base/containers/intrusive_list.h:689-692`
  - [diverged cross-file w=0.73 line=0.28] `samples/framework/internal/shader.cc:109-148` ⟵ `demo/framework/internal/renderer_impl.cc:644-682`
  - [diverged cross-file w=0.72 line=0.47] `cal3d-src/src/cal3d/tinyxmlparser.cpp:739-811` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:1085-1174`
  - [diverged within-file w=0.70 line=0.34] `test/base/maths/vec_float_tests.cc:289-317` ⟵ `test/base/maths/vec_float_tests.cc:332-359`
  - [diverged cross-file w=0.70 line=0.26] `samples/framework/internal/imgui_impl.cc:560-627` ⟵ `demo/framework/internal/imgui_impl.cc:541-616`
  - [diverged cross-file w=0.67 line=0.60] `cal3d-src/src/cal3d/tinyxmlparser.cpp:595-692` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:941-1039`
  - [diverged cross-file w=0.66 line=0.32] `samples/framework/application.cc:605-625` ⟵ `demo/framework/application.cc:408-422`
  - [diverged cross-file w=0.63 line=0.57] `cal3d-src/src/cal3d/tinyxmlparser.cpp:520-587` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:820-899`
  - [diverged cross-file w=0.62 line=0.23] `samples/framework/application.cc:461-528` ⟵ `demo/framework/application.cc:281-340`

## RegimA-Zone/skin9ml — 48 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/RegimA-Zone_skin9ml`

## OzCog/cogutil — 45 коммитов | циклы=0(SCC≤1) | копипаст=12

### `f6dde8958c3d` — "Merge pull request #11 from OzCog/copilot/fix-10" (drzo, 2025-08-30) [12 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/OzCog_cogutil show f6dde8958c3d`
  - [verbatim within-file w=1.00 line=1.00] `opencog/util/algorithm.h:158-161` ⟵ `opencog/util/algorithm.h:165-168`
  - [diverged within-file w=1.00 line=0.20] `opencog/util/backtrace-symbols.c:88-97` ⟵ `opencog/util/backtrace-symbols.c:112-125`
  - [near within-file w=1.00 line=0.85] `opencog/util/numeric.h:384-397` ⟵ `opencog/util/numeric.h:475-488`
  - [diverged within-file w=1.00 line=0.71] `opencog/util/random.h:104-116` ⟵ `opencog/util/random.h:82-94`
  - [diverged within-file w=0.98 line=0.45] `opencog/util/backtrace-symbols.c:220-252` ⟵ `opencog/util/backtrace-symbols.c:270-303`
  - [diverged within-file w=0.90 line=0.62] `opencog/util/backtrace-symbols.c:288-335` ⟵ `opencog/util/backtrace-symbols.c:340-383`
  - [diverged within-file w=0.89 line=0.60] `opencog/util/algorithm.h:267-270` ⟵ `opencog/util/algorithm.h:253-256`
  - [diverged within-file w=0.89 line=0.50] `opencog/util/numeric.h:420-441` ⟵ `opencog/util/numeric.h:511-527`
  - [diverged cross-file w=0.75 line=0.50] `opencog/util/async_buffer.h:324-363` ⟵ `opencog/util/async_method_caller.h:165-197`
  - [near within-file w=0.75 line=0.82] `opencog/util/backtrace-symbols.c:339-352` ⟵ `opencog/util/backtrace-symbols.c:388-401`
  - [diverged within-file w=0.70 line=0.60] `opencog/util/Logger.cc:493-555` ⟵ `opencog/util/Logger.cc:403-454`
  - [diverged within-file w=0.63 line=0.63] `opencog/util/async_method_caller.h:246-282` ⟵ `opencog/util/async_method_caller.h:165-197`

## Harsha-Bhattacharyya/auh — 42 коммитов | циклы=0(SCC≤1) | копипаст=2

### `c81caf016551` — "Merge pull request #5 from Harsha-Bhattacharyya/copilot/refactor-package-management-commands" (Harsha Bhattacharyya, 2025-11-21) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Harsha-Bhattacharyya_auh show c81caf016551`
  - [diverged within-file w=1.00 line=0.40] `src/main.cpp:77-88` ⟵ `src/main.cpp:12-20`
  - [diverged within-file w=0.69 line=0.17] `src/main.cpp:149-217` ⟵ `src/main.cpp:28-60`

## DavidBechtold/MIPSCore — 42 коммитов | циклы=2(SCC≤9) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=9. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/DavidBechtold_MIPSCore`

## shadow11001/esphome — 41 коммитов | циклы=2(SCC≤3) | копипаст=16
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/shadow11001_esphome`

### `bc8c6ffa1b57` — "Merge pull request #10 from shadow11001/copilot/fix-8362c8f2-8488-4d4c-b90c-4698ef31b646" (shadow11001, 2025-09-21) [16 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/shadow11001_esphome show bc8c6ffa1b57`
  - [diverged within-file w=0.98 line=0.75] `esphome/components/midea/appliance_base.h:59-65` ⟵ `esphome/components/midea/appliance_base.h:32-38`
  - [near cross-file w=0.95 line=0.90] `esphome/components/wifi/wifi_component_esp_idf.cpp:343-382` ⟵ `esphome/components/wifi/wifi_component_esp32_arduino.cpp:234-274`
  - [near within-file w=0.88 line=0.88] `esphome/components/api/api_connection.cpp:783-801` ⟵ `esphome/components/api/api_connection.cpp:746-761`
  - [diverged cross-file w=0.88 line=0.75] `esphome/components/shutdown/button/shutdown_button.cpp:20-30` ⟵ `esphome/components/shutdown/shutdown_switch.cpp:21-35`
  - [diverged within-file w=0.86 line=0.55] `esphome/components/dsmr/dsmr.cpp:256-272` ⟵ `esphome/components/dsmr/dsmr.cpp:146-161`
  - [diverged within-file w=0.85 line=0.33] `esphome/components/ledc/ledc_output.cpp:41-51` ⟵ `esphome/components/ledc/ledc_output.cpp:24-31`
  - [diverged within-file w=0.84 line=0.42] `esphome/components/esp32/gpio_idf.cpp:84-92` ⟵ `esphome/components/esp32/gpio_idf.cpp:31-38`
  - [diverged within-file w=0.81 line=0.59] `esphome/core/scheduler.cpp:127-141` ⟵ `esphome/core/scheduler.cpp:93-108`
  - [diverged within-file w=0.79 line=0.11] `esphome/core/helpers.cpp:411-416` ⟵ `esphome/core/helpers.cpp:126-129`
  - [diverged within-file w=0.74 line=0.38] `esphome/components/dsmr/dsmr.cpp:275-290` ⟵ `esphome/components/dsmr/dsmr.cpp:164-171`
  - [diverged cross-file w=0.71 line=0.46] `esphome/components/bme280/bme280.cpp:172-213` ⟵ `esphome/components/bmp280/bmp280.cpp:125-156`
  - [diverged cross-file w=0.68 line=0.45] `esphome/components/i2c/i2c_bus_arduino.cpp:37-65` ⟵ `esphome/components/i2c/i2c_bus_esp_idf.cpp:42-74`
  - [diverged cross-file w=0.68 line=0.45] `esphome/components/i2c/i2c_bus_esp_idf.cpp:47-75` ⟵ `esphome/components/i2c/i2c_bus_arduino.cpp:32-63`
  - [diverged within-file w=0.66 line=0.43] `esphome/core/base_automation.h:232-240` ⟵ `esphome/core/base_automation.h:185-197`
  - [diverged cross-file w=0.65 line=0.36] `esphome/components/mdns/mdns_esp32_arduino.cpp:11-21` ⟵ `esphome/components/mdns/mdns_esp8266.cpp:15-25`
  - [diverged within-file w=0.61 line=0.14] `esphome/components/dsmr/dsmr.cpp:191-253` ⟵ `esphome/components/dsmr/dsmr.cpp:73-143`

## santzit/go-guitar2 — 40 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/santzit_go-guitar2`

## ReZorg/dsstne — 38 коммитов | циклы=0(SCC≤1) | копипаст=28

### `914d1cbc36dd` — "Merge pull request #11 from ReZorg/copilot/proceed-next-steps" (danregima, 2026-04-26) [28 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ReZorg_dsstne show 914d1cbc36dd`
  - [diverged within-file w=1.00 line=0.60] `src/amazon/dsstne/utils/Filters.cpp:205-216` ⟵ `src/amazon/dsstne/utils/Filters.cpp:244-255`
  - [near within-file w=1.00 line=0.80] `src/amazon/dsstne/engine/NNWeight.cpp:841-850` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:461-470`
  - [near within-file w=1.00 line=0.80] `src/amazon/dsstne/engine/NNTypes.cpp:1126-1135` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:623-632`
  - [near within-file w=1.00 line=0.94] `src/amazon/dsstne/engine/NNNetwork.cpp:540-600` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:424-484`
  - [verbatim within-file w=1.00 line=1.00] `src/amazon/dsstne/engine/NNNetwork.cpp:4143-4192` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:3593-3642`
  - [verbatim within-file w=1.00 line=1.00] `src/amazon/dsstne/engine/NNNetwork.cpp:878-907` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:759-788`
  - [near within-file w=1.00 line=0.85] `src/amazon/dsstne/engine/NNNetwork.cpp:1750-1768` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:1619-1637`
  - [diverged within-file w=0.97 line=0.60] `src/amazon/dsstne/engine/NNTypes.cpp:1858-1892` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1211-1242`
  - [verbatim within-file w=0.95 line=0.96] `src/amazon/dsstne/utils/Utils.cpp:160-192` ⟵ `src/amazon/dsstne/utils/Utils.cpp:226-257`
  - [near within-file w=0.92 line=0.85] `src/amazon/dsstne/engine/NNNetwork.cpp:1710-1744` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:1579-1613`
  - [near within-file w=0.91 line=0.83] `src/amazon/dsstne/engine/NNTypes.cpp:2393-2454` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1589-1650`
  - [diverged within-file w=0.90 line=0.59] `src/amazon/dsstne/engine/NNWeight.cpp:502-561` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:295-349`
  - [diverged within-file w=0.89 line=0.74] `src/amazon/dsstne/engine/NNNetwork.cpp:1603-1668` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:1475-1536`
  - [near within-file w=0.88 line=0.80] `src/amazon/dsstne/engine/NNLayer.cpp:912-948` ⟵ `src/amazon/dsstne/engine/NNLayer.cpp:234-266`
  - [diverged within-file w=0.87 line=0.55] `src/amazon/dsstne/engine/NNTypes.cpp:433-466` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:323-348`
  - [near within-file w=0.86 line=0.82] `src/amazon/dsstne/engine/NNTypes.cpp:2163-2214` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1371-1422`
  - [diverged within-file w=0.81 line=0.64] `src/amazon/dsstne/engine/NNLayer.cpp:1253-1284` ⟵ `src/amazon/dsstne/engine/NNLayer.cpp:481-509`
  - [diverged within-file w=0.80 line=0.76] `src/amazon/dsstne/engine/NNNetwork.cpp:913-988` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:794-865`
  - [diverged within-file w=0.78 line=0.38] `src/amazon/dsstne/engine/NNTypes.cpp:2029-2060` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1211-1242`
  - [diverged within-file w=0.77 line=0.71] `src/amazon/dsstne/engine/NNTypes.h:644-649` ⟵ `src/amazon/dsstne/engine/NNTypes.h:411-416`
  - [diverged within-file w=0.74 line=0.60] `src/amazon/dsstne/engine/NNLayer.cpp:1004-1039` ⟵ `src/amazon/dsstne/engine/NNLayer.cpp:299-330`
  - [diverged within-file w=0.73 line=0.41] `src/amazon/dsstne/engine/GpuTypes.h:489-538` ⟵ `src/amazon/dsstne/engine/GpuTypes.h:420-457`
  - [diverged within-file w=0.72 line=0.68] `src/amazon/dsstne/engine/NNNetwork.cpp:829-873` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:709-754`
  - [diverged within-file w=0.68 line=0.39] `src/amazon/dsstne/engine/NNWeight.cpp:729-759` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:401-423`
  - [diverged within-file w=0.67 line=0.70] `src/amazon/dsstne/engine/GpuTypes.cpp:437-472` ⟵ `src/amazon/dsstne/engine/GpuTypes.cpp:393-419`
  - [diverged within-file w=0.65 line=0.30] `src/amazon/dsstne/engine/NNWeight.cpp:800-827` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:401-423`
  - [diverged within-file w=0.65 line=0.62] `src/amazon/dsstne/engine/NNTypes.h:603-639` ⟵ `src/amazon/dsstne/engine/NNTypes.h:381-406`
  - [diverged within-file w=0.62 line=0.30] `src/amazon/dsstne/engine/NNWeight.cpp:764-794` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:428-448`

## Kilynho/cpu6502 — 37 коммитов | циклы=0(SCC≤1) | копипаст=2

### `ae48ac939e4f` — "Release v2.0.0 - Comprehensive Feature Release (#39)" (Kilynho, 2025-12-18) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Kilynho_cpu6502 show ae48ac939e4f`
  - [diverged cross-file w=1.00 line=0.24] `src/cpu/cpu.cpp:252-263` ⟵ `cpu.cpp:144-158`
  - [diverged cross-file w=1.00 line=0.14] `src/main/cpu_demo.cpp:9-63` ⟵ `main_6502.cpp:8-62`

## ercasta/Clockstar-v2-Firmware — 36 коммитов | циклы=0(SCC≤1) | копипаст=4

### `ff5a7d78f180` — "Merge pull request #5 from ercasta/copilot/fix-invisible-bar-and-ball" (Antonio Castaldo D'Ursi, 2026-01-31) [4 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/ercasta_Clockstar-v2-Firmware show ff5a7d78f180`
  - [diverged within-file w=1.00 line=0.75] `main/src/Notifs/Bangle.cpp:99-109` ⟵ `main/src/Notifs/Bangle.cpp:44-54`
  - [near within-file w=0.92 line=0.88] `main/src/Notifs/Bangle.cpp:155-200` ⟵ `main/src/Notifs/Bangle.cpp:86-128`
  - [diverged within-file w=0.88 line=0.78] `main/src/Devices/IMU.cpp:274-285` ⟵ `main/src/Devices/IMU.cpp:234-242`
  - [verbatim within-file w=0.73 line=0.97] `main/src/LV_Interface/FSLVGL.cpp:65-106` ⟵ `main/src/LV_Interface/FSLVGL.cpp:11-50`

## cogcities/rrp9-civic-angel — 36 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/cogcities_rrp9-civic-angel`

## CanerKaraca23/classic-axis — 35 коммитов | циклы=0(SCC≤1) | копипаст=1

### `dcb0d457bd78` — "Merge pull request #2 from CanerKaraca23/copilot/fix-windows-build-failure" (Caner Karaca, 2026-05-25) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/CanerKaraca23_classic-axis show dcb0d457bd78`
  - [diverged cross-file w=0.80 line=0.53] `vendor/GInputAPI/GInputAPI.h:278-292` ⟵ `source/shared/GInputAPI.h:261-271`

## awest813/Dewm-3 — 35 коммитов | циклы=3(SCC≤3) | копипаст=50
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 3 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/awest813_Dewm-3`

### `56c7cf96af92` — "Merge pull request #14 from awest813/cursor/gameplay-easy-fixes-ad14" (nerdretro, 2026-05-30) [50 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/awest813_Dewm-3 show 56c7cf96af92`
  - [diverged within-file w=1.00 line=0.74] `neo/tools/debugger/DebuggerWindow.cpp:460-503` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:398-440`
  - [verbatim within-file w=1.00 line=1.00] `neo/sys/win32/win_main.cpp:391-404` ⟵ `neo/sys/win32/win_main.cpp:288-301`
  - [diverged within-file w=1.00 line=0.75] `neo/framework/Common.cpp:2801-2815` ⟵ `neo/framework/Common.cpp:2728-2739`
  - [verbatim cross-file w=1.00 line=1.00] `neo/sys/aros/aros_net.cpp:557-575` ⟵ `neo/sys/posix/posix_net.cpp:514-532`
  - [verbatim cross-file w=1.00 line=1.00] `neo/sys/aros/aros_main.cpp:462-483` ⟵ `neo/sys/posix/posix_main.cpp:451-471`
  - [verbatim within-file w=1.00 line=1.00] `neo/sound/snd_system.cpp:522-540` ⟵ `neo/sound/snd_system.cpp:413-431`
  - [near within-file w=1.00 line=0.94] `neo/framework/UsercmdGen.cpp:1193-1219` ⟵ `neo/framework/UsercmdGen.cpp:935-959`
  - [verbatim within-file w=1.00 line=0.98] `neo/sys/events.cpp:487-652` ⟵ `neo/sys/events.cpp:97-258`
  - [diverged within-file w=1.00 line=0.79] `neo/framework/minizip/unzip.cpp:926-988` ⟵ `neo/framework/minizip/unzip.cpp:1042-1106`
  - [verbatim within-file w=1.00 line=0.96] `neo/renderer/RenderSystem_init.cpp:2342-2379` ⟵ `neo/renderer/RenderSystem_init.cpp:2085-2120`
  - [near within-file w=0.99 line=0.90] `neo/framework/FileSystem.cpp:3662-3686` ⟵ `neo/framework/FileSystem.cpp:3637-3661`
  - [near cross-file w=0.99 line=0.81] `neo/d3xp/MultiplayerGame.cpp:3286-3333` ⟵ `neo/game/MultiplayerGame.cpp:2594-2641`
  - [near cross-file w=0.99 line=0.81] `neo/game/MultiplayerGame.cpp:2595-2642` ⟵ `neo/d3xp/MultiplayerGame.cpp:3286-3333`
  - [near cross-file w=0.98 line=0.91] `neo/sys/aros/aros_net.cpp:140-174` ⟵ `neo/sys/posix/posix_net.cpp:133-167`
  - [diverged within-file w=0.97 line=0.75] `neo/renderer/Image_files.cpp:861-867` ⟵ `neo/renderer/Image_files.cpp:980-986`
  - [near within-file w=0.97 line=0.80] `neo/framework/minizip/unzip.cpp:353-405` ⟵ `neo/framework/minizip/unzip.cpp:443-495`
  - [verbatim within-file w=0.96 line=0.95] `neo/tools/debugger/DebuggerWindow.cpp:785-852` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:667-734`
  - [near within-file w=0.96 line=0.90] `neo/renderer/draw_common.cpp:1181-1243` ⟵ `neo/renderer/draw_common.cpp:972-1031`
  - [near within-file w=0.95 line=0.86] `neo/framework/Common.cpp:2534-2564` ⟵ `neo/framework/Common.cpp:2475-2502`
  - [near within-file w=0.94 line=0.90] `neo/framework/Session.cpp:1664-1675` ⟵ `neo/framework/Session.cpp:1632-1641`
  - [verbatim within-file w=0.94 line=0.95] `neo/ui/ChoiceWindow.cpp:415-446` ⟵ `neo/ui/ChoiceWindow.cpp:381-410`
  - [near within-file w=0.93 line=0.87] `neo/tools/debugger/DebuggerWindow.cpp:2092-2173` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:1724-1806`
  - [verbatim within-file w=0.92 line=0.95] `neo/tools/debugger/DebuggerWindow.cpp:2184-2215` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:1817-1846`
  - [near within-file w=0.92 line=0.94] `neo/framework/UsercmdGen.cpp:1332-1357` ⟵ `neo/framework/UsercmdGen.cpp:1054-1077`
  - [near within-file w=0.92 line=0.91] `neo/framework/UsercmdGen.cpp:1023-1071` ⟵ `neo/framework/UsercmdGen.cpp:767-814`
  - [near within-file w=0.91 line=0.95] `neo/renderer/RenderSystem_init.cpp:2387-2419` ⟵ `neo/renderer/RenderSystem_init.cpp:2128-2158`
  - [near within-file w=0.89 line=0.91] `neo/framework/Common.cpp:3279-3324` ⟵ `neo/framework/Common.cpp:3065-3109`
  - [near cross-file w=0.88 line=0.88] `neo/sys/aros/aros_main.cpp:371-435` ⟵ `neo/sys/posix/posix_main.cpp:364-424`
  - [near within-file w=0.88 line=0.83] `neo/sys/posix/posix_main.cpp:701-770` ⟵ `neo/sys/posix/posix_main.cpp:364-424`
  - [near cross-file w=0.88 line=0.82] `neo/sys/aros/aros_net.cpp:505-549` ⟵ `neo/sys/posix/posix_net.cpp:462-506`
  - [near within-file w=0.87 line=0.85] `neo/sys/win32/win_main.cpp:736-773` ⟵ `neo/sys/win32/win_main.cpp:568-600`
  - [diverged within-file w=0.86 line=0.30] `neo/sys/glimp.cpp:892-907` ⟵ `neo/sys/glimp.cpp:333-357`
  - [near within-file w=0.85 line=0.86] `neo/framework/Session.cpp:1471-1486` ⟵ `neo/framework/Session.cpp:1439-1454`
  - [diverged within-file w=0.83 line=0.31] `neo/sys/glimp.cpp:846-859` ⟵ `neo/sys/glimp.cpp:290-301`
  - [near cross-file w=0.82 line=0.81] `neo/sys/aros/aros_net.cpp:619-663` ⟵ `neo/sys/posix/posix_net.cpp:576-620`
  - [near cross-file w=0.81 line=0.86] `neo/game/Game_network.cpp:1472-1527` ⟵ `neo/d3xp/Game_network.cpp:1512-1572`
  - [near cross-file w=0.81 line=0.86] `neo/d3xp/Game_network.cpp:1512-1572` ⟵ `neo/game/Game_network.cpp:1472-1527`
  - [near within-file w=0.81 line=0.91] `neo/sys/glimp.cpp:770-785` ⟵ `neo/sys/glimp.cpp:256-269`
  - [diverged within-file w=0.80 line=0.72] `neo/sound/snd_world.cpp:529-555` ⟵ `neo/sound/snd_world.cpp:499-518`
  - [diverged within-file w=0.79 line=0.69] `neo/sys/win32/win_main.cpp:435-500` ⟵ `neo/sys/win32/win_main.cpp:331-386`
  - [near within-file w=0.77 line=0.85] `neo/framework/Session.cpp:1442-1462` ⟵ `neo/framework/Session.cpp:1413-1430`
  - [diverged cross-file w=0.77 line=0.76] `neo/sys/aros/aros_net.cpp:373-432` ⟵ `neo/sys/posix/posix_net.cpp:330-389`
  - [diverged within-file w=0.75 line=0.64] `neo/sys/events.cpp:1169-1179` ⟵ `neo/sys/events.cpp:457-467`
  - [near within-file w=0.75 line=0.80] `neo/framework/Session_menu.cpp:1210-1245` ⟵ `neo/framework/Session_menu.cpp:1181-1208`
  - [near within-file w=0.75 line=0.84] `neo/framework/minizip/ioapi.c:149-169` ⟵ `neo/framework/minizip/ioapi.c:189-210`
  - [diverged within-file w=0.75 line=0.71] `neo/sys/posix/posix_main.cpp:264-271` ⟵ `neo/sys/posix/posix_main.cpp:246-250`
  - [diverged within-file w=0.72 line=0.61] `neo/sys/win32/win_main.cpp:127-178` ⟵ `neo/sys/win32/win_main.cpp:70-104`
  - [diverged cross-file w=0.72 line=0.21] `neo/game/script/Script_Compiler.cpp:986-1010` ⟵ `neo/d3xp/script/Script_Compiler.cpp:985-990`
  - [diverged cross-file w=0.72 line=0.21] `neo/d3xp/script/Script_Compiler.cpp:986-1010` ⟵ `neo/game/script/Script_Compiler.cpp:985-990`
  - [diverged within-file w=0.70 line=0.36] `neo/framework/UsercmdGen.cpp:1378-1404` ⟵ `neo/framework/UsercmdGen.cpp:1054-1077`

## ZoneCog/RRR-P-Systems — 34 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/ZoneCog_RRR-P-Systems`

## deltaeecs/CSR4MPI — 34 коммитов | циклы=0(SCC≤1) | копипаст=2

### `151224a7b9c0` — "更新 petsc 适配器" (xyhe, 2026-03-10) [2 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/deltaeecs_CSR4MPI show 151224a7b9c0`
  - [near cross-file w=1.00 line=0.80] `include/MatrixMarketLoader.h:76-103` ⟵ `src/MatrixMarketLoader.cpp:70-98`
  - [diverged cross-file w=0.76 line=0.52] `include/DistributedOps.h:18-61` ⟵ `src/DistributedOps.cpp:13-63`

## xr00tlabx/xRat-System_v3-cliente — 33 коммитов | циклы=0(SCC≤1) | копипаст=6

### `b9ccb71c1d86` — "Implementa sistema de reconexão automática com backoff exponencial" (XRat Developer, 2025-08-12) [6 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/xr00tlabx_xRat-System_v3-cliente show b9ccb71c1d86`
  - [near within-file w=1.00 line=0.88] `src/forms/MainForm.cpp:490-508` ⟵ `src/forms/MainForm.cpp:394-412`
  - [near within-file w=0.95 line=0.90] `src/controllers/MainController.cpp:445-482` ⟵ `src/controllers/MainController.cpp:376-413`
  - [near within-file w=0.94 line=0.87] `src/controllers/MainController.cpp:394-409` ⟵ `src/controllers/MainController.cpp:325-340`
  - [near within-file w=0.84 line=0.81] `src/forms/MainForm.cpp:422-482` ⟵ `src/forms/MainForm.cpp:340-386`
  - [diverged within-file w=0.76 line=0.58] `src/controllers/MainController.cpp:558-581` ⟵ `src/controllers/MainController.cpp:489-511`
  - [near within-file w=0.72 line=0.80] `src/forms/MainForm.cpp:528-561` ⟵ `src/forms/MainForm.cpp:432-457`

## andrele/perse_rover-firmware-microros — 33 коммитов | циклы=4(SCC≤5) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 4 include-циклов, крупнейший SCC=5. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/andrele_perse_rover-firmware-microros`

## Ninnh-temp/DSA — 32 коммитов | циклы=0(SCC≤1) | копипаст=1

### `1c8de22ffa70` — "Merge pull request #8 from Ninnh-temp/copilot/add-avl-tree-visualization" (Ninnh-temp, 2025-12-15) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Ninnh-temp_DSA show 1c8de22ffa70`
  - [diverged within-file w=0.87 line=0.76] `DSA_AVL_tree.cpp:210-277` ⟵ `DSA_AVL_tree.cpp:138-198`

## nayutalienx/cursor-trail — 31 коммитов | циклы=0(SCC≤1) | копипаст=1

### `0a68e801e013` — "Merge pull request #2 from nayutalienx/copilot/fix-2d243eb4-254b-4344-8b38-bd0de531936b" (Viktor, 2025-08-20) [1 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/nayutalienx_cursor-trail show 0a68e801e013`
  - [diverged within-file w=0.90 line=0.60] `CursorTrail/Game.cpp:124-149` ⟵ `CursorTrail/Game.cpp:92-117`

## HyperCogWizard/rrpling — 31 коммитов | циклы=2(SCC≤3) | копипаст=0
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 2 include-циклов, крупнейший SCC=3. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/HyperCogWizard_rrpling`

## o6nn/kactl — 30 коммитов | циклы=1(SCC≤2) | копипаст=8
- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** 1 include-циклов, крупнейший SCC=2. `/home/localadm/projects/cpparch/build/debug/src/archcheck --graph /home/localadm/oss/_aidev_dense/o6nn_kactl`

### `0da961c46bbf` — "Merge pull request #6 from o6nn/copilot/fix-troubleshooter-sylvester-equation" (drzo, 2026-04-25) [8 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/o6nn_kactl show 0da961c46bbf`
  - [verbatim cross-file w=1.00 line=1.00] `stress-tests/geometry/ConvexHull.cpp:20-25` ⟵ `content/geometry/ConvexHull.h:40-45`
  - [verbatim cross-file w=1.00 line=1.00] `stress-tests/graph/DirectedMST.cpp:13-16` ⟵ `fuzz-tests/fast-delaunay.cpp:34-37`
  - [near cross-file w=0.91 line=0.80] `stress-tests/geometry/ConvexHull.cpp:8-17` ⟵ `content/geometry/ConvexHull.h:28-37`
  - [near cross-file w=0.90 line=0.89] `stress-tests/graph/oldHLD.h:77-95` ⟵ `content/graph/HLD.h:93-111`
  - [near cross-file w=0.89 line=0.83] `stress-tests/numerical/LinearRecurrence.cpp:17-42` ⟵ `fuzz-tests/linear-recurrence.cpp:27-52`
  - [diverged cross-file w=0.81 line=0.63] `stress-tests/graph/LinkCutTree.cpp:7-41` ⟵ `fuzz-tests/link-cut.cpp:17-54`
  - [diverged cross-file w=0.78 line=0.47] `stress-tests/number-theory/ModularArithmetic.cpp:6-20` ⟵ `fuzz-tests/modular-arithmetic.cpp:16-26`
  - [diverged cross-file w=0.74 line=0.57] `stress-tests/strings/SuffixArray.cpp:88-100` ⟵ `content/strings/SuffixArray.h:23-35`

## Scynode/PhantomFirmware — 29 коммитов | циклы=0(SCC≤1) | копипаст=37

### `d561b1dc6058` — "Merge pull request #7 from Scynode/copilot/fix-build-errors-after-translation" (Scynode, 2026-03-07) [37 hits]
  inspect: `git -C /home/localadm/oss/_aidev_dense/Scynode_PhantomFirmware show d561b1dc6058`
  - [diverged cross-file w=1.00 line=0.70] `src/mech/drivers.cpp:119-169` ⟵ `src/scultpureMode.cpp:3250-3304`
  - [verbatim cross-file w=1.00 line=1.00] `src/ble/ble_callbacks.cpp:637-656` ⟵ `src/BLE.cpp:881-900`
  - [near cross-file w=1.00 line=0.92] `src/ble/ble_callbacks.cpp:670-706` ⟵ `src/BLE.cpp:913-947`
  - [near cross-file w=1.00 line=0.87] `src/ble/ble_ota.cpp:165-187` ⟵ `src/BLE.cpp:567-586`
  - [diverged cross-file w=1.00 line=0.65] `src/ble/ble_ota.cpp:194-218` ⟵ `src/BLE.cpp:590-611`
  - [near cross-file w=1.00 line=0.89] `src/chess/pgn.cpp:67-77` ⟵ `src/scultpureMode.cpp:275-286`
  - [diverged cross-file w=1.00 line=0.41] `src/chess/pgn.cpp:121-140` ⟵ `src/scultpureMode.cpp:326-345`
  - [near cross-file w=1.00 line=0.90] `src/chess/pgn.cpp:284-304` ⟵ `src/scultpureMode.cpp:2733-2754`
  - [diverged cross-file w=1.00 line=0.59] `src/mech/drivers.cpp:53-98` ⟵ `src/scultpureMode.cpp:3775-3817`
  - [verbatim cross-file w=1.00 line=1.00] `src/app/play_mode.cpp:839-849` ⟵ `src/main.cpp:915-925`
  - [diverged cross-file w=1.00 line=0.46] `src/mech/electromagnets.cpp:57-84` ⟵ `src/scultpureMode.cpp:1793-1820`
  - [diverged cross-file w=1.00 line=0.47] `src/mech/electromagnets.cpp:95-113` ⟵ `src/scultpureMode.cpp:1825-1843`
  - [verbatim cross-file w=1.00 line=1.00] `src/mech/motion.cpp:407-417` ⟵ `src/scultpureMode.cpp:3644-3654`
  - [diverged cross-file w=1.00 line=0.33] `src/mech/motion.cpp:774-783` ⟵ `src/scultpureMode.cpp:1360-1369`
  - [diverged cross-file w=1.00 line=0.17] `src/mech/motion.cpp:794-817` ⟵ `src/scultpureMode.cpp:1380-1406`
  - [diverged cross-file w=1.00 line=0.55] `src/mech/trajectory.cpp:124-151` ⟵ `src/scultpureMode.cpp:2427-2454`
  - [diverged cross-file w=1.00 line=0.45] `src/sensors/sensor_io.cpp:189-210` ⟵ `src/scultpureMode.cpp:3040-3055`
  - [verbatim cross-file w=1.00 line=1.00] `src/ble/ble_callbacks.cpp:536-544` ⟵ `src/BLE.cpp:779-787`
  - [diverged cross-file w=1.00 line=0.73] `src/app/play_mode.cpp:909-945` ⟵ `src/main.cpp:963-995`
  - [diverged cross-file w=1.00 line=0.79] `src/app/play_mode.cpp:966-1012` ⟵ `src/main.cpp:1004-1049`
  - [diverged cross-file w=1.00 line=0.57] `src/app/setup.cpp:169-188` ⟵ `src/main.cpp:61-72`
  - [diverged cross-file w=1.00 line=0.57] `src/app/play_mode.cpp:876-896` ⟵ `src/main.cpp:942-959`
  - [verbatim cross-file w=1.00 line=1.00] `src/ble/ble_callbacks.cpp:578-586` ⟵ `src/BLE.cpp:819-827`
  - [near cross-file w=1.00 line=0.87] `src/ble/ble_methods.cpp:545-572` ⟵ `src/BLE.cpp:1713-1742`
  - [diverged cross-file w=0.99 line=0.65] `src/app/setup.cpp:461-546` ⟵ `src/main.cpp:246-309`
  - [diverged cross-file w=0.99 line=0.64] `src/app/play_mode.cpp:1289-1311` ⟵ `src/main.cpp:1279-1299`
  - [near cross-file w=0.99 line=0.80] `src/ble/ble_callbacks.cpp:597-627` ⟵ `src/BLE.cpp:838-871`
  - [diverged cross-file w=0.97 line=0.71] `src/ble/ble_ota.cpp:117-158` ⟵ `src/BLE.cpp:530-563`
  - [diverged cross-file w=0.97 line=0.75] `src/app/play_mode.cpp:1151-1153` ⟵ `src/main.cpp:1177-1181`
  - [diverged cross-file w=0.97 line=0.71] `src/app/play_mode.cpp:1170-1228` ⟵ `src/main.cpp:1185-1241`
  - [diverged cross-file w=0.95 line=0.54] `src/sensors/sensor_calib.cpp:89-169` ⟵ `src/scultpureMode.cpp:4245-4309`
  - [diverged cross-file w=0.95 line=0.55] `src/ble/ble_ota.cpp:70-101` ⟵ `src/BLE.cpp:493-517`
  - [diverged cross-file w=0.94 line=0.65] `src/ble/ble_ota.cpp:225-250` ⟵ `src/BLE.cpp:615-636`
  - [diverged cross-file w=0.93 line=0.42] `src/ble/ble_battery.cpp:159-240` ⟵ `src/BLE.cpp:1746-1820`
  - [diverged cross-file w=0.86 line=0.13] `src/chess/movement.cpp:132-155` ⟵ `src/scultpureMode.cpp:1040-1065`
  - [diverged cross-file w=0.83 line=0.19] `src/ble/ble_battery.cpp:65-138` ⟵ `src/BLE.cpp:979-1041`
  - [diverged cross-file w=0.75 line=0.71] `src/app/play_mode.cpp:818-831` ⟵ `src/main.cpp:896-911`
