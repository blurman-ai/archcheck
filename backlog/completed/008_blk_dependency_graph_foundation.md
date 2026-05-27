# [GRAPH] Фундамент графа зависимостей и diff-примитивы

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-27
**Статус:** done
**Модуль:** GRAPH
**Приоритет:** blocker
**Сложность:** M (2-4 дня) — фактически реализована за один день через параллельных worktree-агентов
**Блокирует:** #009 (ai_drift_regression_rules), #018 (git_diff_analysis)
**Заблокирован:** —
**Related:** #006 (spec_refactor)

## Цель

Зафиксировать и реализовать канонический file-level dependency graph и
отдельный `graph-baseline` contract, без которых нельзя надёжно реализовывать
первый прототип `DRIFT.*`.

## Финальное состояние

Подсистемы `scan/` и `graph/` полностью реализованы и проверены на боевом
коде. `archcheck --graph <path>` end-to-end строит граф из произвольного
C++ репозитория без `compile_commands.json` и libclang.

**Pipeline:**
```
discover_files → scan_includes → resolve_includes → DependencyGraph
              → compute_scc / added_edges / grown_sccs
              → save_baseline / load_baseline
```

**Метрики:**
- 115/115 тестов зелёные (unit + integration + fixture).
- lizard чист (`--CCN 15 --length 30 --arguments 5`, zero warnings).
- На реальном проекте `gm` (2192 файла): 7.7 s wall, найдены 2 цикла в Unigine SDK headers.
- На archcheck сам в себе: 0 циклов (lakos-clean).

См. [docs/milestones.md](../../docs/milestones.md) для подробного отчёта.

## Сделано (по подзадачам)

Декомпозиция #008 на 8 + 7 подзадач (textual scanner + остальной пайплайн):

**Scanner (textual, без libclang):**
- **#008a** — API skeleton: `IncludeKind`, `IncludeDirective`, `scan_includes`. STATIC core.
- **#008b** — наивная line-based экстракция (quote + angle, multi, leading ws).
- **#008c** — пропуск `//` и `/* */` комментариев (включая многострочные).
- **#008d** — пропуск raw strings `R"d(…)d"` с произвольным delimiter.
- **#008e** — logical-line continuation (`\\\n` с сохранением физической нумерации).
- **#008f** — правило «`#` первый значимый символ» (тесты — реализация уже была через `skip_ws+compare`).
- **#008g** — macro-include diagnostic, расширение API → `ScanResult{directives, diagnostics}`.
- **#008h** — integration fixtures с диска (6 файлов `fixtures/scan/include_scanner/`).

**Discovery + resolver:**
- **#011** — `discover_files` (`std::filesystem::recursive_directory_iterator` + excluded dirs), `ProjectIndex{exact_path_index, suffix_index}`.
- **#012** — `include_resolver`: `Project / External / Unresolved / Ambiguous`, политики quote/angle.

**Graph:**
- **#013** — `NodeId` (strong-typed `uint32_t`) + `DependencyGraph` (forward + reverse adjacency).
- **#014** — `compute_scc` (Tarjan iterative), `reachable_from`, `reverse_reachable_from`, `has_path`.
- **#015** — diff primitives: `added_edges`, `removed_edges`, `grown_sccs` (матчинг SCC по member-set overlap).
- **#016** — `graph-baseline` формат (YAML через ryml) + save/load со структурированной обработкой ошибок. Спецификация — [docs/baseline_format.md](../../docs/baseline_format.md).

**Fixtures:**
- **#017** — 7 integration-фикстур (minimal_dag, single_scc, new_edge, shortcut_edge, cycle_growth, unresolved_include, ambiguous_include) + end-to-end pipeline тест.

**CLI (preview, для дoгфудинга):**
- `archcheck --scan <path>` — discovery + scan, выводит сводку.
- `archcheck --graph <path>` — полный pipeline, выводит nodes/edges/sccs/etc., exit code 1 на циклы.

## Как работает (целиком)

1. **Discovery** (`scan::discover_files`) обходит репо, отфильтровывая
   `build/` / `.git/` / `cmake-build-*/` / `.cache/` / `.idea/` / `.vscode/` /
   `out/` на любой глубине. Расширения: `.c/.cc/.cpp/.cxx/.h/.hh/.hpp/.hxx/.ipp/.tpp/.inl/.inc`. Пути нормализуются в POSIX repo-relative.

2. **Indexing** (`scan::build_project_index`) строит `exact_path_index`
   (repo-relative path → NodeId) и `suffix_index` (каждый `/`-суффикс пути → vector кандидатов). Нужно для resolver-а.

3. **Scanning** (`scan::scan_includes`) — textual scanner. Pipeline:
   `join_continuations` (склейка `\\\n` с сохранением физической нумерации) →
   `preprocess` (комменты + raw strings → пробелы) →
   `try_extract` (поиск `#include` после skip_ws + классификация).
   Результат: `ScanResult{directives, diagnostics}`. Macro-includes идут в diagnostics.

4. **Resolution** (`scan::resolve_includes`) превращает каждую директиву в
   `ResolvedInclude{Resolution}`:
   - Quote: dir-relative → exact → unique suffix → Project / Ambiguous / Unresolved.
   - Angle: exact → unique suffix → Project / Ambiguous / External.

5. **Graph build** — для каждой `Resolution::Project` строится ребро в `DependencyGraph::add_edge`. Контейнер хранит forward + reverse adjacency для дешёвых reverse-reachable / Ca-метрик.

6. **Analysis** (`graph::compute_scc` + друзья) — iterative Tarjan (без рекурсии, чтобы не упасть на 100k+ файлах), детерминированный output.

7. **Diff** (`graph::added_edges/removed_edges/grown_sccs`) — сравнивает два графа по path-строкам (NodeId-ы session-local).

8. **Persistence** (`graph::save_baseline/load_baseline`) — детерминированный YAML с `format_version`, сортированными nodes и edges как `[from_idx, to_idx]`. Loader возвращает `optional<BaselineLoadError>` вместо exception-ов.

## Чем управляется

- На уровне библиотеки — ни одного флага / env. Всё поведение задаётся аргументами функций.
- CLI: `--scan <path>` и `--graph <path>`. Без config-файла, без `compile_commands.json`.
- Все компоненты собираются в `archcheck_core` (STATIC). `archcheck` бинарник — тонкая обёртка над core.

## С чем связана

- **Открывает #009** (drift-regression rules) — может опираться на готовые diff-примитивы и baseline-формат.
- **Открывает #018** (git-based PR analysis) — нижний слой полностью готов, нужна только git-интеграция и diff-reporter.
- **Открывает класс Lakos-правил** в v0.1 plan: cycles (SF.9) фактически уже реализованы (`grown_sccs` поверх `compute_scc`), god-headers и chain length — отдельные правила поверх готовых graph algorithms.
- Не блокирует `--baseline` режим v0.1 — формат + save/load готовы, нужен только CLI subcommand.

## Диагностика

- Если `archcheck --graph <path>` падает или зависает — проверь, что у компилятора есть `<filesystem>` (для GCC<9 нужен явный `-lstdc++fs`, уже прописано в CMake).
- Если на больших проектах резко много `unresolved` — посмотри, какие пути не находятся, обычно это generated headers / templates вне репо.
- Если много `ambiguous` — типично для monorepo с per-module precompiled headers (`pch.h`) или общими именами (`utils.h`); решается exclude-pattern (future) или baseline.
- Если SCC-counter не совпадает с ожиданием — `compute_scc` возвращает SCC даже размером 1 (singletons). Циклы — это `size >= 2`.
- ryml-парсер baseline-а: см. ловушку с `[[noreturn]]` error-callback в #016.

## Mini-design: fast extraction v0.1 (исторический design-документ)

Ниже — исходный рабочий draft extraction semantics. Сохранён как историческая
запись: вся семантика реализована в подзадачах 008a-h, 011, 012. Расхождений с
финальной реализацией нет.

### 1. Что сканируем

В graph pipeline попадают **все project files подходящих расширений**, а не
только `.cpp`.

Стартовый набор расширений:

- translation units: `.c`, `.cc`, `.cpp`, `.cxx`
- headers / include-like: `.h`, `.hh`, `.hpp`, `.hxx`, `.ipp`, `.tpp`, `.inl`, `.inc`

По умолчанию из discovery исключаются только явно нецелевые директории:

- `.git/`
- `build/`
- `cmake-build-*/`
- `.cache/`
- `.idea/`
- `.vscode/`
- `out/`

То есть v0.1 **не** пытается автоматически отбрасывать `third_party/`,
`vendor/` или `external/`: это отдельное решение, которое нельзя тихо
зашивать эвристикой.

### 2. Как сканируем include-директивы

Fast backend в v0.1 — это **textual include scanner**, а не полноценный
препроцессор и не AST.

Сканер должен:

- читать файл как текст;
- понимать logical lines с `\\`-continuation;
- игнорировать `#include` внутри line comments, block comments, string literals, char literals и raw string literals;
- распознавать директиву только когда `#` стоит как первый значимый символ logical line;
- извлекать только literal includes: `#include "path"` / `#include <path>`.

Macro-based include (`#include SOMETHING`) попадает в diagnostics как `macro_include`, но не порождает ребро.

### 3. Conditional compilation

Для zero-config режима у нас нет надёжного набора `-D` и compile target context-а. v0.1 принимает honest ограничение:

- literal `#include` в любой ветке `#if/#ifdef/#ifndef` считается candidate include;
- fast graph является over-approximation;
- branch-sensitive уточнение — задача adapters в v0.2+.

### 4. Resolution algorithm

Заранее строятся индексы: `exact_path_index` (repo-relative path → file), `suffix_index` (`/`-суффикс → кандидаты).

**Для `#include "x"`:** dir-relative → exact → unique suffix → Project / Ambiguous / Unresolved.

**Для `#include <x>`:** exact → unique suffix → Project / Ambiguous / External.

Resolver не «угадывает» при множественных кандидатах — честно отдаёт Ambiguous.

### 5. Что попадает в канонический graph

**Узлы:** все discovered project files.
**Рёбра:** только успешно resolved project includes.

`external` / `unresolved` / `ambiguous` / `macro_include` — diagnostics, не рёбра. `graph-baseline` хранит только nodes + project edges; warnings не размывают смысл DRIFT.*.

### 6. Diagnostics

Минимальный record: source file + line + raw token + kind (quote/angle) + resolution kind + target/candidates. Повторные includes дедуплицируются на уровне graph edge, но locations сохраняются для отчёта.

### 7. Где место готовых инструментов (v0.2+)

- **depfile adapter** — gcc/clang `-M*` output как build-resolved источник.
- **clang-scan-deps adapter** — refined source для проектов с compile context.
- **cross-check mode** — сравнение fast vs refined graph для измерения расхождений.

### 8. Открытые вопросы (для v0.2+)

- Хранить ли metadata о «include найден внутри conditional block».
- Нужен ли отдельный `vendored/generated` classification layer.
- Как оформлять suppression для хронически ambiguous includes.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Сначала graph primitives, потом rules | Иначе `DRIFT.*` и Lakos checks будут опираться на неустойчивую модель |
| Канонический граф — file-level | Упрощает семантику, устраняет дублирование первичных моделей |
| Fast extraction core делаем сами | Zero-config v0.1 требует repo-local include resolver без compile database |
| Готовые dependency extractors считаем adapters, а не ядром | `depfiles` и `clang-scan-deps` полезны, но завязаны на build context |
| Нужен именно graph diff, а не только snapshot | AI-drift rules сравнивают состояние «до/после» |
| baseline хранит snapshot, а не derived metrics | Делает формат устойчивее, не привязывает к текущему набору правил |
| Минимум зависимостей, без внешней graph-библиотеки | Совпадает с архитектурными ограничениями проекта |
| Параллельная реализация подзадач через worktree-агентов | Резко ускорило выполнение #008 (день вместо 2-4) при сохранении изолированности изменений |

## Изменённые файлы (финальный список)

| Файл | Изменение |
|------|-----------|
| `include/archcheck/scan/include_directive.h` | new — IncludeKind, IncludeDirective, DiagnosticKind, IncludeScanDiagnostic, ScanResult |
| `include/archcheck/scan/include_scanner.h` | new — `scan_includes` API |
| `include/archcheck/scan/project_files.h` | new — ProjectFile, ProjectIndex, discover_files, build_project_index |
| `include/archcheck/scan/resolved_include.h` | new — Resolution, ResolvedInclude |
| `include/archcheck/scan/include_resolver.h` | new — resolve_include / resolve_includes |
| `include/archcheck/graph/node_id.h` | new — strong-typed NodeId + std::hash |
| `include/archcheck/graph/dependency_graph.h` | new — DependencyGraph class |
| `include/archcheck/graph/algorithms.h` | new — compute_scc, reachable_from, reverse_reachable_from, has_path |
| `include/archcheck/graph/diff.h` | new — EdgeRef, added_edges, removed_edges, GrownScc, grown_sccs |
| `include/archcheck/graph/baseline.h` | new — save_baseline, load_baseline, BaselineLoadError |
| `src/scan/*.cpp` | 4 файла реализации scan-слоя |
| `src/graph/*.cpp` | 4 файла реализации graph-слоя |
| `src/main.cpp` | preview CLI: `--scan`, `--graph` |
| `src/CMakeLists.txt` | STATIC core, 8 source-ов, stdc++fs для GCC<9 |
| `tests/unit/scan/*` | 4 test source файла |
| `tests/unit/graph/*` | 4 test source файла |
| `tests/integration/scan/scanner_fixtures_test.cpp` | интеграционные тесты scanner-а |
| `tests/integration/graph/end_to_end_test.cpp` | end-to-end тесты pipeline |
| `tests/CMakeLists.txt` | подключение всех source-ов + ARCHCHECK_FIXTURES_DIR + stdc++fs |
| `fixtures/scan/include_scanner/*.cpp` | 6 фикстур scanner-а |
| `fixtures/graph/**/*` | 7 фикстур graph-pipeline |
| `docs/baseline_format.md` | new — спецификация baseline YAML |
| `docs/milestones.md` | new — лог реальных dogfood-прогонов |
| `docs/architecture-spec.md` | + секция «Прогресс разработки» со ссылкой на milestones |

## Fixtures

- [x] Минимальный DAG без циклов
- [x] Граф с одним SCC
- [x] Сценарий «появилось новое ребро»
- [x] Сценарий «shortcut edge»
- [x] Сценарий «cycle growth»
- [x] Сценарий «unresolved include»
- [x] Сценарий «ambiguous include»
