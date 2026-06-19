# Backlog Review — 2026-06-19 (четвёртый проход)

_Все задачи из `new/` и `wip/` прочитаны полностью (через параллельных sonnet-агентов). `pending/` не трогали._

## Статистика очереди

| Папка | Файлов |
|-------|--------|
| `new/` | 16 |
| `wip/` | 7 |
| `future/` | 15 |
| `completed/` | 107 |
| `pending/` | 1 (не трогаем) |

**Стухших (>30 дней):** 0 — самый старый активный файл от 2026-06-02 (#074/#066), 17 дней.
**MVP release-блокеры:** #103 (research-цифры по факту получены) + #123 (ядро + parent-guard закоммичены, остаток = фикстуры). См. `TASK_TRACKER.md`.
**Главное за проход:** #129 (unify source scan) — ядро уехало 6 коммитами (`ec5988b`, `9cc349b`, `b01707a`, +docs), но файл всё ещё лежит в `new/`. Это не new — это wip с почти закрытым планом.

---

## Протухшие / висящие / неверно размещённые

| Файл | Причина | Рекомендация |
|------|---------|--------------|
| **#129** unify_source_scan | Ядро реализовано (шаги 1–6 из плана 0–7 закоммичены: `isGeneratedPath`+SWIG, `AuthoredScope`/`SourceSnapshot`, check-mode read-once, one-snapshot-per-ref в `--diff`). Лежит в `new/` как «не начата». | **Move `new/` → `wip/`** (через `/issue`/`/checkpoint`). Остаток = шаг 7: финальный dogfood + пер-репо корпус vs golden + обновить `docs/research/agent_drift_within_repo.md` (числа сдвинулись от шагов 3–5). |
| **#103** copypaste_per_commit | Research-цель (порядок величины per-commit clone rate) достигнута; «full scan 185 реп НЕ добиваем» зафиксировано явно; остаток (Шаг 6) помечен необязательным, гипотеза agentic отвалилась. | **Кандидат на `completed/`** (через `/fix-issue`). Содержательно закрыта. |
| **#072** port_056_duplication | Все 7 модулей `src/scan/duplication/` влиты, CLI `--duplication`, FP-гардлы, 17 тестов, dogfood. Единственный открытый пункт — JSON-вывод dup pairs: «нужен ли в v0.1». | **Принять решение** (v0.1 vs v0.2 — это одно решение, не реализация) → затем `completed/`. |
| **#066** airepo_remeasure | Долгий фоновый перемер ~8k CLONEFAIL-реп был в прогоне на 2026-06-02; статус с тех пор неизвестен. По дате не протухло, но «висит» без чекпойнта. | **Проверить флаг `_aidev_state/remeasure.done`** и логи → `/checkpoint`. Если прогон умер — перезапуск. |

---

## Быстрые победы (quick_win, < 1 дня)

| Файл | Цель | Модуль |
|------|------|--------|
| **#128** sf8_guard_after_header_block | SF.8 FP когда `#ifndef`-guard стоит после длинной лицензионной шапки → читать до первой значащей строки кода, а не фиксированную «голову». Баг подтверждён пере-проверкой (openvdb, 30 FP). | RULES / SCAN |
| **#130** source_snapshot_perf_nits | Убрать транзитивный 2× пик памяти в `SourceSnapshot::read()` + заменить O(n) `findFile` на `unordered_map`. Два поведенчески-нейтральных нита, нужен замер RSS до/после. Follow-up #129. | SCAN |
| **#108** post_audit_sweep_rest | Дочистить остаток #104: вынос fork/exec-дубля + `toLowerCopy`, разбивка длинных функций (lizard-долг). Секция 3 уже закрыта. | SCAN / QUALITY |

---

## Средние задачи (medium, 1–3 дня)

| Файл | Цель | Модуль | Примечание |
|------|------|--------|------------|
| #057 lakos_fanout_coupling | fan-out god-component + avg coupling + max blast radius + SCC-размер как дешёвые граф-метрики в SF.9 | GRAPH / RULES / REPORT | следующий слой ценности после diff-core (P1 в трекере) |
| #093 flag_argument | эвристика flag-argument: bool-параметры + multiple bool-literal call sites; **создаёт shared `text_scan`/`function_signature_scan`** | SCAN / DIFF / REPORT | голова кластера сигнатур — делать первой |
| #094 param_count_and_accretion | сигнатуры >4 параметров + рост числа параметров между ревизиями | SCAN / DIFF / REPORT | **зависит от #093** (shared scanner) |
| #095 config_bag_growth | `*Config/*Options/*Settings` с избыточным ростом полей (full scan + diff accretion) | SCAN / DIFF / REPORT | **зависит от #093** (text_scan) |
| #074 ai_repo_discovery_roi | свести discovery-пайплайн AI-реп к одному контракту (giant-skip/shallow-since везде, CLONEFAIL ≠ skip) | RESEARCH / SCAN | синхронизировать backlog с кодом |
| #077 per_commit_graph_drift_export | exporter per-commit diff include-графа → jsonl+md для ручной верификации drift-кейсов | RESEARCH / DIFF | надстройка над `--diff`, не трогает продукт |
| #122 grow_corpus_to_1000 | дорастить корпус 481 → ~1000 C++-реп + реген всех измерений на новом бинаре | RESEARCH / CORPUS | Фаза 0 в работе; **блокирует #119** |
| #123 diff_new_clone_gate | new-clone detection в `--diff` (added-lines ∩ clone-spans) + parent-guard | SCAN / CLI | **MVP-блокер**; ядро+parent-guard закоммичены, остаток = фикстуры `fixtures/diff_new_clone/` |
| #124 corpus_validate_new_clone_gate | корпусная валидация new-clone-gate (precision/recall на реальных клон-коммитах) | SCAN / RESEARCH | **ждёт** полнокорпусный прогон (ETA ~10 дней с 2026-06-14); нужен `/commit` на empty-blob-фикс |
| #125 scan_extensionless_headers | видеть заголовки без расширения (`<vector>`, GSL) | SCAN | **unknown→medium**: нужен выбор варианта A/B + perf-замер перед кодом; в паре с #127 |

---

## Сложные / заблокированные (hard / blocked)

| Файл | Цель | Блокер / причина |
|------|------|------------------|
| #126 sf9_collapse_impl_into_component | схлопывать `foo.hpp+foo_impl.hpp` и `dir/impl/` в один Lakos-компонент | переход от 2-узлового спец-кейса к полной компонентной модели; quick-fix (259→16) уже в коде, тикет про полную модель |
| #127 vendor_generated_exclusion | многослойное исключение vendored/generated (gitmodules→path→LICENSE→copyright→граф-остров) | 5 слоёв, риск over-exclude; copyright-mismatch слой уже пробован и **откатан в #129** |
| #129 unify_source_scan | read-once `AuthoredScope`/`SourceSnapshot` для всех правил | **в работе**, ядро landed, остаток = шаг 7 (см. «Протухшие»); сдвигает опубликованные числа → нужен корпус-реран |
| #078 clone_cochange_harm_signal | severity клон-группы по co-change рассогласованности (git-история), не текст-сходству | кластеризация пар→группы, обход истории, follow-rename, фикстуры с историей |
| #119 unified_per_commit_drift_correlation | свести 6 drift-сигналов в один датасет + Spearman с repo fixed effects | **Заблокирован #122** (нужен корпус ~1000); реген ~1000 реп — часы–сутки |
| #054 ai_repo_duplication_run | прогон AI-корпуса через граф+дубли, зафиксировать цифры | ~85% done, ждёт ×3 прогон 160 lowstar-реп + within-repo тест (центральная гипотеза не доказана) |
| #066 airepo_remeasure_clonefail | перемер CLONEFAIL + доклон AI-плотных ≥300 коммитов/год | фоновый перемер ~8k реп, статус неизвестен (см. «Протухшие») |
| #070 checker_fp_fix_proposals | FP-фильтры дубликат-чекера, precision 42%→55–75% | ~75% done; **measurement-harness — placeholder**: не маппит пары на `fp_corpus_r2.tsv`, реальный эффект P0 не замерен, пороги P0.6 не фитированы |

---

## Без анализа (нужно исследование / решение перед кодом)

| Файл | Что не хватает |
|------|----------------|
| #125 scan_extensionless_headers | не пустой шаблон, но требует **спайка-решения** до реализации: вариант A vs B + perf-замер; naïve-фикс затянет Eigen → обязателен vendor-фильтр #127 в паре. |
| #072 (JSON-вопрос) | не код, а **одно scope-решение**: dup-pairs JSON в v0.1 или v0.2. |

_Пустых шаблонов без анализа в очереди нет — все задачи содержательны._

---

## Дубли / связанные (кластеры)

| Кластер | Файлы | Предложение |
|---------|-------|-------------|
| **Scan FP-fix — demo/dogfood блокеры** | #125, #126, #127, #128 | Корректность сканера на чужом header-only коде. #127↔#125 тесно связаны (vendor-exclude нужен до extensionless-fix, иначе Eigen). Проставить взаимные `Related:`; порядок: #128 (quick) → #127 → #125 → #126. |
| **Сигнатуры / accretion** | #093 → #094 → #095 | #094 и #095 зависят от shared `text_scan`/`signature_scan`, который создаётся в #093. **Делать #093 первой**, иначе дублирование инфраструктуры. |
| **New-clone-gate** | #123 → #124 → #103; #078 | #123 (продукт) → #124 (валидация, blocked на shipped #123) → #103 (research-precursor, по факту done). #078 — downstream severity-надстройка над дублями. |
| **Source-scan unify** | #129 + #130 | #130 — explicit perf-follow-up к #129, найден состязательным ревью архитектуры. Связать `Related: #129`. |
| **Corpus / research ops** | #066, #074, #122 → #119; #077, #054 | #119 заблокирован #122 (рост корпуса). #066/#074/#122 — discovery/corpus-ops, держать отдельно от MVP-борда (см. TASK_TRACKER §OUT). |
| **ID-clash в `completed/`** (гигиена) | #117 (×2 файла), #118 (×2 файла) | Уже отмечено в TASK_TRACKER: `diff_max_added_lines` лежит и в completed (#118), и был в new (#117/#118) — вероятный дубль, `diffMaxAddedLines` уже в коде. Чистка имён — отдельная гигиеническая микро-задача, не блокер. |

---

## Сводка

- **Всего активных:** 23 (new/ 16 + wip/ 7).
- **Протухших по дате (>30д):** 0.
- **Висящих / неверно размещённых:** 4 — #129 (new→wip), #103 (→completed), #072 (решение→completed), #066 (нужен чекпойнт фонового прогона).
- **Быстрых побед:** 3 — #128, #130, #108.
- **Заблокированных:** #119 (на #122); фактически ждут фоновых прогонов — #124, #054, #066.
- **Требует решения перед кодом:** #125 (вариант A/B + perf), #072 (JSON scope).
- **Рекомендуемый следующий ход:** #128 (quick, подтверждённый баг) либо разгрузка борда без новой работы — #129→wip, #103/#072→completed.

_Перемещения между `new/`/`wip/`/`completed/` этот скил не делает — только предлагает. Двигают `/issue`, `/checkpoint`, `/fix-issue`._
