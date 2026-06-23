# Check coverage matrix

_Где какая проверка живёт и в каком режиме гоняется. Источник истины — код
(`src/rules/rule_set.cpp`, `src/cli/{check,diff,preview}_commands.cpp`,
`src/scan/*`) + CHANGELOG. Обновлять при добавлении правила/режима._

Режимы:
- **check** — `archcheck <path>` (whole-repo, интринсик-правила; гейтит на SF.9).
- **drift** — `archcheck --drift-baseline` (whole-repo против сохранённого baseline).
- **diff** — `archcheck --diff <revspec>` (per-commit; то, что гонял корпус-прогон #124).
- **history** — `archcheck --history <path>` (history-аналитика, advisory).
- **duplication** — `archcheck --duplication <path>` (снимок клонов, preview).

## Реализовано

| Проверка | Правило/ID | check | drift | diff (#124) | history | примечание |
|---|---|:--:|:--:|:--:|:--:|---|
| Нет циклов включений | SF.9 / `Sf9NoCycles` | ✅ гейт | ✅ | ✅ `grown_cycles` | — | единственный gating-сигнал |
| using namespace в хедере | SF.7 / `Sf7UsingNamespace` | ✅ adv | — | ❌ интринсик | — | не дельта → нет в diff |
| header self-sufficiency | SF.8 / `Sf8IncludeGuard` | ✅ adv | — | ❌ интринсик | — | |
| God-header (fan-in) | Lakos.GodHeader | ✅ | ✅ | ✅ `new_god_headers` | — | |
| Длина цепочки включений | Lakos.ChainLength | ✅ adv | — | ✅ `chain_length_grown` | — | |
| Shortcut edge | DRIFT.1 | — | ✅ гейт | ✅ `added_edges` | — | |
| Cycle growth | DRIFT.2 | — | ✅ гейт | ✅ `grown_cycles` | — | |
| Bidir module coupling | DRIFT.3 | — | ✅ adv | ✅ `new_cross_area` | — | |
| Lateral module dep | DRIFT.4 | — | ✅ | ✅ (cross-area) | — | |
| Когнитивная сложность ф-ии | DRIFT.LOCAL_COMPLEXITY | — | — | ✅ `complexity` 17.2% | — | diff-only (нужна дельта) |
| Внесённый копипаст | DRIFT.NEW_CLONE | — | — | ✅ `newclone` 9.8% | — | снимок — в `--duplication` |
| Self-admitted tech debt | SATD.1/2 | — | — | ✅ `satd` 4.5% | — | diff-only (added lines) |
| Тесты не со-эволюционируют | TEST.1 | — | — | ✅ `test` 22.6% | — | diff-only (numstat) |
| NCCD-метрика (связность) | — | — | — | ✅ `nccd_delta` 9.2% | — | дельта метрики |
| Рост LOC файла | SIZE.1 / god_file_growth | — | — | ❌ | ✅ | **только history** |
| Магнит дефектов (fix-churn) | HIST.1 / defect_attractor | — | — | ❌ | ✅ | **только history** |

## Слепые пятна корпус-прогона #124 (`--diff`)
`--diff` НЕ запускает: **SF.7, SF.8** (интринсик, не дельта) и **SIZE.1, HIST.1**
(только `--history`). То есть покоммитная валидация их не покрывает; для них нужен
отдельный whole-repo / history-прогон по корпусу.

## Документировано/планировано, но НЕ реализовано (и почему)

| Хотелка | Статус | Причина |
|---|---|---|
| **SF.21** (аноним. namespace в хедере) + SF.2/5/10/11 | v0.2 | нужен семантический (libclang) backend; текст-скан → FP в дефолте (ADR-002) |
| **bool-поля в структурах** | v0.2 | парсинг членов = семантика (libclang); токен-backend не тянет (ADR-003) |
| **Config module rules** (`layers`/`independence`/`forbidden`) | v0.2 | парсятся+валидируются, но enforcement отложен ради zero-config adoption (ADR-001) |
| **#093 flag_argument** (рост bool-параметров) | post-release | НЕ ADR — отложено по приоритету; токен-feasible, можно вытащить |

Полная роадмапа правил: [docs/architecture-spec.md](../architecture-spec.md) §Roadmap.
