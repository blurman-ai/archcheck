# [CLI][GRAPH] Git-based анализ: считать дельту графа, а не полный snapshot

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-27
**Статус:** wip
**Модуль:** CLI, GRAPH
**Приоритет:** critical
**Сложность:** L (3-5 дней)
**Блокирует:** —
**Заблокирован:** — (#015, #016 закрыты)
**Related:** #008 (dependency_graph_foundation), #009 (ai_drift_regression_rules, future)

## Цель

Сделать так, чтобы archcheck в CI говорил не «в проекте 27 циклов» (бесполезно
для PR-ревью legacy-проекта), а **«эта PR ввела 1 новый цикл и 3 новых
unresolved-импорта»**. То есть рассчитывать архитектурную **дельту** по
git-ref-у (commit, branch, range), не по полному snapshot-у.

## Контекст

Сейчас `archcheck --graph <path>` строит и анализирует **всё** дерево
проекта. На реальном `gm` (2192 файла) находит 2 цикла в Unigine SDK,
708 unresolved, 138 ambiguous. В PR-context это шум: новых нарушений нет,
старые исторические. CI должен фокусироваться на **изменениях, которые
внесла именно эта PR**.

Есть два пути сравнения, оба валидны:

1. **Baseline-файл (#016)** — `.archcheck/graph-baseline.yaml` в репо.
   PR-валидация: построил граф текущего состояния, сравнил с baseline,
   рапортовал только дельту. Минус: нужно «прикреплять» baseline к main и
   обновлять его при сознательных архитектурных изменениях.

2. **Git-based (эта задача)** — `archcheck --diff main..HEAD` или
   `archcheck --since HEAD~1`. Tool сам делает `git checkout` (или
   `git show:file`) для двух состояний, строит два графа, считает дельту.
   Минус: дороже (два прохода), нужна git-интеграция.

Часто эти подходы **дополняют** друг друга:
- baseline — для длинных периодов и больших разовых рефакторингов;
- git-diff — для каждой PR в режиме «не ухудшается ли тут».

Задача — реализовать git-diff путь. baseline-путь идёт в #016 параллельно
и переиспользует те же primitives из #015.

## План выполнения

- [x] Определить CLI-форму: канонический `--diff <revspec>` (`a..b` или одиночный `<ref>` = `<ref>..WORKTREE`). `--since`/`--vs-base` решено НЕ вводить — git revspec покрывает оба сценария
- [x] Решить как читать «прошлое» состояние: `git worktree add --detach --quiet <tmp> <ref>` (fork/exec). Для текущего worktree — sentinel `WORKTREE` без checkout-а. Без libgit2; libgit2 — будущая оптимизация по требованию
- [x] Реализовать `include/archcheck/git/git_state.h` / `src/git/git_state.cpp`: RAII `Worktree`, `materializeRef`, `findRepoRoot`, `parseRevspec`. Без shell-injection (fork/exec + execvp с argv)
- [x] Построить два графа (baseline, current), переиспользовать pipeline из `--graph` через новый helper `archcheck::graph::buildGraphForPath()`
- [x] Применить diff-примитивы из #015 (`addedEdges`, `removedEdges`, `grownSccs`) внутри `archcheck::diff::buildRegressionReport`
- [x] Reporter: `archcheck::diff::writeTextReport` — «added_edges/removed_edges/grown_cycles», далее перечисление с file:line (пока без line, line будет когда в EdgeRef появятся location-метаданные)
- [x] Exit codes: 0 = `!hasRegression`, 1 = `hasRegression` (новые рёбра ИЛИ выросшие циклы; removed-only ≠ регрессия), 2 = invalid revspec / not-a-repo / git failure
- [x] Тесты: 6 интеграционных через `mkdtemp` + git CLI: добавленное ребро, замыкание цикла, no-op (docs), single-ref→WORKTREE, findRepoRoot из подкаталога, invalid ref
- [x] CI-пример: `.github/workflows/example_archcheck_pr.yml` — `archcheck --diff origin/main..HEAD .` + upload-artifact

## Сделано

- **CLI**: добавлен `--diff <revspec> [path]` в [src/main.cpp](src/main.cpp) (path по умолчанию = cwd).
- **Парсер revspec**: `parseRevspec` ([include/archcheck/git/git_state.h:17](include/archcheck/git/git_state.h#L17)); `a..b`, `<ref>`, отвергает `...`/пустые стороны.
- **Git-материализация**: RAII `Worktree` (move-only) + `materializeRef` через fork/exec `git worktree add --detach`. Cleanup через `git worktree remove --force` + `fs::remove_all` (best-effort). Sentinel `WORKTREE` для текущего рабочего дерева без checkout-а.
- **Graph build pipeline извлечён**: `buildGraphForPath()` ([include/archcheck/graph/graph_builder.h](include/archcheck/graph/graph_builder.h)). `--graph` и оба прохода `--diff` теперь дёргают один helper.
- **Регрессионный отчёт**: `RegressionReport` ([include/archcheck/diff/regression_report.h](include/archcheck/diff/regression_report.h)) — DTO + `buildRegressionReport` (резолвит NodeId в строки) + `writeTextReport`.
- **Тесты**: 20 новых `[diff]`-кейсов (parseRevspec, regression_report, end-to-end через temp git-репо). Всего 131 кейс, 380 ассертов, 100% pass. Lizard 0 warnings (CCN ≤15, length ≤30). clang-format-18 чист.
- **CI-пример**: [.github/workflows/example_archcheck_pr.yml](.github/workflows/example_archcheck_pr.yml).
- **Smoke на собственном репо**: `archcheck --diff HEAD .` корректно показывает 19 новых рёбер и 1 удалённое от текущей PR; exit code 1.

## В работе

- (пусто) — задача готова к коммиту/ревью.

## Следующие шаги

1. **Готово к `/commit`** — задача целиком в одном merge, как просил пользователь (вне обычного режима «≤50 строк/коммит»).
2. **Дальше (вне этой задачи)**: line-числа в `EdgeRef` (сейчас reporter показывает только пути; для file:line нужно тянуть location из scanner-а — отдельная задача).
3. **Оптимизация (по требованию)**: переход с fork/exec на libgit2 — когда профилирование покажет, что worktree add доминирует.
4. **GitHub-комментирование PR**: пример workflow выкладывает артефакт, но не комментирует PR — добавить step через `actions/github-script` можно позже, когда определимся с форматом.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Git-diff и baseline — два равноправных пути | Покрывают разные сценарии CI / разные команды; не конкурирующие |
| Сначала fork/exec git, потом если нужно — libgit2 | Меньше зависимостей, проще на старте; libgit2 — оптимизация по требованию |
| Exit code 1 только на «регрессии», не на «старых нарушениях» | CI должен пропускать PR, которая ничего не ухудшила, даже если legacy грязный |
| Передавать ref-ы как git revspec (`a..b`, `HEAD~1`) | Знакомый формат, парсит сам git |
| Канонический CLI: `--diff <revspec>`; одиночный `<ref>` → `<ref>..WORKTREE` | `--since` и `--vs-base` синтаксически избыточны: revspec покрывает оба. Одна форма — меньше путаницы в документации |
| `git worktree add --detach <tmp> <ref>` vs `git show :file` | worktree даёт реальное дерево → переиспользуется существующий pipeline `buildGraphForPath`. `git show` пришлось бы оборачивать в виртуальный FS-провайдер — не оправдано на старте |
| Sentinel `WORKTREE` — текущий рабочий каталог, без checkout-а | Для локального запуска и `--diff <ref>` пропускаем worktree add (бесплатно). RAII-обёртка дегенерирует в non-owning |
| Removed-only diff ≠ регрессия | PR, которая только убирает рёбра, делает архитектуру строже — CI должен пропустить |
| Resolved NodeId → paths в `RegressionReport` upfront | Отчёт переживёт baseline/current графы; reporter не зависит от lifetime двух DependencyGraph |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/graph/graph_builder.h` | new — публичный helper `buildGraphForPath()` (общий entry для `--graph` и `--diff`) |
| `src/graph/graph_builder.cpp` | new — реализация (move-нутая логика из main.cpp) |
| `include/archcheck/git/git_state.h` | new — `Revspec`, `Worktree` (RAII), `parseRevspec`, `findRepoRoot`, `materializeRef` |
| `src/git/git_state.cpp` | new — fork/exec git (`execvp`, без shell). Helpers: `execChild`, `drainFd`, `collectChild` |
| `include/archcheck/diff/regression_report.h` | new — `RegressionReport` (paths-resolved), `buildRegressionReport`, `writeTextReport` |
| `src/diff/regression_report.cpp` | new |
| `src/main.cpp` | + `--diff <revspec> [path]` dispatch; `run_graph` упрощён до вызова buildGraphForPath; добавлен `--diff` в `print_help` |
| `src/CMakeLists.txt` | + 3 новых исходника в `archcheck_core` |
| `tests/CMakeLists.txt` | + 3 новых test-юнита |
| `tests/unit/git/git_state_test.cpp` | new — 6 кейсов parseRevspec |
| `tests/unit/diff/regression_report_test.cpp` | new — 4 кейса buildRegressionReport/writeTextReport |
| `tests/integration/diff/git_diff_test.cpp` | new — 6 end-to-end кейсов через `mkdtemp` + git CLI |
| `.github/workflows/example_archcheck_pr.yml` | new — пример CI job для пользователей |
| `backlog/wip/018_crt_git_diff_analysis.md` | move из `new/` + checkpoint |

**НЕ тронуты в этой PR**: `docs/architecture-spec.md` — уточнение про два пути (baseline+git-diff, как они сосуществуют) ждёт пока соберётся достаточный опыт от пользователей, чтобы написать осмысленный раздел. Пометить отдельной задачей.
