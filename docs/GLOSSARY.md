# archcheck — глоссарий

Единый справочник терминов проекта: тезис, сигналы дрейфа, правила, архитектура
инструмента, метрики, методология корпуса. Назначение — общий язык для доков, статьи
и задач. Авторитет по конкретике: [CHANGELOG.md](../CHANGELOG.md) (что вышло),
[docs/architecture-spec.md](architecture-spec.md) (дизайн), `docs/research/` (исследование).

---

## A. Тезис и базовые понятия

- **Constraint decay** — постепенное размывание архитектурных ограничений по мере роста
  кодовой базы: контракт задан, но каждый следующий коммит чуть его ослабляет, вместо
  рефакторинга. Первоисточник — Dente et al. (EURECOM 2026): LLM-агенты теряют ~30 пп
  качества при усложнении структурных ограничений (L0→L3). Стержень мотивации archcheck.
  См. [docs/research/constraint_decay.md](research/constraint_decay.md).
- **Drift (дрейф)** — наблюдаемый след constraint decay в git-истории: добавленные
  shortcut-рёбра, выросшие циклы, копипаст, накопление булевых флагов, боковые связи
  модулей. Меряется per-commit (дельта) или per-struct/history (накопление).
- **Agentic / non-agentic** — разрез корпуса: репы/коммиты с признаками ИИ-ассистирования
  (AI-трейлеры, bot-авторство, длинные сообщения) против остальных. Контраст «agentic vs
  non-agentic» — основной способ проверить, усиливает ли ИИ дрейф.
- **Boolean accretion (накопление булей)** — добавление булевых полей в структуру со
  временем. **Нейтральный феномен, не приговор**: бывает легитимным (независимые свойства)
  и смеллом. Метрика `n_bool_field_added` (#135) меряет именно прирост.
- **Boolean blindness** — частный случай-смелл: несколько `bool` кодируют взаимоисключающие
  или коррелированные состояния, и невалидная комбинация представима в типе (просится
  `enum`/sum type). Эталон — `LambdaExpressionNode` с `constexpr ⊥ consteval`.
  См. [docs/research/boolean_state_accretion_good_vs_smell.md](research/boolean_state_accretion_good_vs_smell.md).
- **Dogfooding** — archcheck обязан проходить собственные правила: гоняется на своих
  `src/`/`include/`/`tests/`. Любой merge, ломающий свои правила, недопустим.

## B. Слои и сигналы дрейфа

Сигналы комплементарны — ловят разные классы, складывать их в один счётчик нельзя.

- **Graph drift (графовый)** — per-commit изменения include-графа: added/removed edges,
  выросшие циклы (SCC), новые cross-area зависимости. Редкий и объективный сигнал.
- **Copypaste / клон-слои** — комплементарные уровни детекта дублирования: line (#053),
  token (#056, Type-3 с нормализацией), AST (#052), precision (#059), usage (#054).
  Единый источник — [docs/duplication_architecture.md](duplication_architecture.md).
- **Boolean-state per-commit** — сколько булевых полей коммит добавил (дельта). Квант для
  корреляций; в #135 уточнён до нетто-прироста в пред-существующие структуры.
- **Boolean-state per-struct / history** — структуры, накопившие булевы поля через ≥4
  разных коммита (ось = структура, не коммит). Мера накопленного decay (#134, #089).
- **Lateral module drift** — появление первой боковой зависимости между модулями-пирами
  (правило DRIFT.4).
- **Complexity drift** — рост когнитивной сложности функции между baseline и текущей
  версией (правило DRIFT.LOCAL_COMPLEXITY, токеновый, без AST).
- **SATD** (Self-Admitted Technical Debt) — добавленные `TODO`/`FIXME`/`HACK` в строках
  диффа (правила SATD.1/SATD.2).

## C. Правила archcheck

Каждое правило = один класс = один файл в `src/rules/`; регистрация — фабрика в
`rule_set.cpp`. Атрибуция к источнику (Core Guidelines / Lakos / Martin) обязательна.

- **SF.7** — нет `using namespace` в глобальной области заголовка (Core Guidelines).
- **SF.8** — самодостаточность заголовка / include guard.
- **SF.9** — нет циклов среди заголовков. Единственное, что гейтит в обычном check-режиме.
- **Lakos.GodHeader** — порог fan-in (по умолчанию 50): заголовок, который включают слишком
  многие = хаб-бог.
- **Lakos.ChainLength** — слишком длинная include-цепочка.
- **DRIFT.1** — новое shortcut-ребро между файлами, оба уже были в baseline (#009). Гейтит.
- **DRIFT.2** — новый или выросший цикл против baseline (#009). Гейтит. Редкий (~0.05% коммитов).
- **DRIFT.3** — новая двусторонняя связь модулей (area A→B и B→A через разные файлы), которой
  не было в baseline; не ловится файловым циклом. Advisory (#087).
- **DRIFT.4** — первая боковая зависимость между модулями-пирами (#118). Градуирован:
  - **DRIFT.4.CYCLE** — создаёт модульный цикл с baseline-обраткой; **гейтит** в drift-режиме
    (precision ~92% на корпусе).
  - **DRIFT.4.SDP** — нарушает Stable Dependencies Principle Мартина (I(B) > I(A)+0.10). Advisory.
  - **DRIFT.4.NEW** — просто первая боковая пара. Advisory.
- **DRIFT.NEW_CLONE** — копипаст, внесённый коммитом (surfacing в `--diff`). Advisory, не гейтит.
- **DRIFT.LOCAL_COMPLEXITY** — функция, перешагнувшая порог когнитивной сложности. Advisory.
- **SATD.1 / SATD.2** — добавленный самопризнанный техдолг. Advisory.
- **TEST (co-evolution)** — prod-код изменён, тесты молчат. Advisory.
- **ARG.1** — flag-argument drift: рост числа булевых аргументов функции (#093, scan-уровень). Advisory.
- **DRIFT.BOOL_FIELD_ACCRETION** — нетто-прирост depth-0 bool-полей в структуре, существовавшей в baseline (#090/#135). Net-счёт (rename/reformat → 0); greenfield не считается. Advisory; фильтр vendored/test/generated через `authored`.
- **Gate vs advisory** — **гейт** (exit 1) дают только регрессии: DRIFT.1, DRIFT.2, DRIFT.4.CYCLE
  в drift-режиме; SF.9-цикл в обычном. Всё остальное — **advisory** (репортится, exit 0), чтобы
  легаси-долг не валил первый прогон. См. `--baseline`.

## D. Архитектура инструмента

- **Pipeline** — `scan → graph → rules → reporter`. Подсистемы: `scan/`, `graph/`, `rules/`,
  `report/`, плюс `diff/`, `git/`, `scan/duplication/`.
- **Fast (preprocessor) backend** — дефолт v0.1: быстрый скан `#include` без компиляции и без
  `compile_commands.json`. Большинство дефолтных правил include-only — не платят цену libclang.
- **libclang backend** — опция `--with-clang` (полноценно v0.2) для семантических правил.
- **Two-backend split** — сознательное разделение: спайк #043 показал libclang ×1000+ медленнее
  на ряде проектов → fast остаётся дефолтом. ADR-003.
- **Zero-config** — запуск без аргументов даёт полезный результат. YAML-конфиг парсится и
  валидируется, но enforcement модульных правил (`layers`/`independence`/`forbidden`) отложен
  в v0.2 (ADR-001).
- **Baseline** — снимок текущих нарушений/графа, от которого меряется регрессия:
  `--baseline`, `--save-baseline`, `--save-graph-baseline`, `--drift-baseline`. Позволяет
  легаси-проектам внедрять инструмент без переписывания («не 5000 нарушений на первом прогоне»).
- **Diff-режим** — `archcheck --diff <revspec>`: материализует два git-состояния и репортит
  структурные регрессии между ними. `--format=json` со стабильной схемой; `gate: ok|fail`.
- **Exit codes (контракт)** — `0` ок · `1` нарушения · `2` ошибка конфига/парсинга · `3`
  внутренняя ошибка. Менять только с версионированием.

## E. Метрики

- **CCD / ACD / NCCD** — Cumulative / Average / Normalized Cumulative Component Dependency
  (Lakos): суммарная связность графа компонентов и её нормировки.
- **Fan-in** — сколько компонентов включают данный заголовок; высокий fan-in → god-header.
- **Chain length** — длина include-цепочки.
- **Cognitive complexity** — когнитивная сложность функции (Campbell 2018), токеновый расчёт
  без AST; основа DRIFT.LOCAL_COMPLEXITY.
- **Instability I (Martin)** — I = Ce/(Ca+Ce); основа SDP (зависеть надо на более стабильное).
- **SDP** (Stable Dependencies Principle) — зависимость должна указывать на более стабильный
  модуль; нарушение — основа DRIFT.4.SDP.

## F. Корпус и методология

- **Корпус** — ~1686 локальных git-реп C++ на NVMe (`~/oss/`); per-commit таблица
  `experiments/per_commit/results_full.jsonl` (~340k записей), worklist `worklist_full.tsv`.
- **Corpus criteria gate** — ворота отбора реп (`experiments/CORPUS_CRITERIA.md`): главное —
  >300 коммитов с мая 2025. Сверяться перед каждым клоном.
- **Agentic gate** — критерии «ИИ-плотности» репы (AI-трейлеры, bot-авторство, длина сообщений).
- **Parentless / shallow boundary** — коммит, у которого `sha^` неразрешим (root или граница
  shallow-клона); diff деградирует до «весь файл новый» → такие коммиты пропускают.
- **Repo fixed effects** — контроль на репозиторий: убирает межрепные различия, оставляя
  внутрирепный сигнал. Многие «agentic-эффекты» гибнут под ним (см. lateral-drift #115).
- **Симпсон (Simpson's paradox)** — агрегатный тренд, исчезающий/разворачивающийся внутри
  групп. Главная ловушка корпус-выводов; повод сверять поштучно, а не по агрегату.
- **Size confound** — «большой коммит → много всего»: ложная корреляция от размера диффа.
  Лечится partial correlation / биннингом по `added_total`, не голым Пирсоном.
- **Eye-check** — ручная поштучная сверка N находок (вход→ожидаемый выход) независимым
  методом; обязательна перед выводом «работает правильно».
- **TP / FP** — true / false positive. Классы FP для bool-дрейфа (#135): greenfield (новый
  файл/структура), rename/typo-fix/replace (число булей не растёт), вложенные depth>0 поля.
- **Velocity (как сигнал)** — скорость коммитов как прокси агентности: **транзиентна и не
  переживает repo fixed effects** → не популяционный AI-сигнал. Классический разворот.

## G. Метрика `n_bool_field_added` (#135)

- **n_bool_field_added** — per-commit квант: нетто-прирост числа depth-0 bool-полей в
  структурах, существовавших в родителе (`sha^`). `Σ max(0, count_after − count_before)`.
- **Net-count (нетто-счёт)** — считаем прирост ЧИСЛА булей, а не added-строки `+bool`.
  Снимает FP от rename/replace/реформата (при них счёт не меняется → 0).
- **Greenfield** — bool-поле в новой структуре/файле (нет в `sha^`); НЕ дрейф, не считаем.
- **Сайдкар** — отдельный Python-проход (`experiments/boolean_state/bool_field_added_per_commit.py`),
  пишет свой JSONL по ключу `(repo, sha)` и джойнится в корпус-таблицу; бинарь не трогает.
