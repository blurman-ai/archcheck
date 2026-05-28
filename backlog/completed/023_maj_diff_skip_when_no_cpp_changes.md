# [CLI][GRAPH][PERF] `--diff`: пропускать всю работу если не менялся ни один C/C++ файл

**Дата создания:** 2026-05-27
**Дата старта:** 2026-05-27
**Статус:** completed
**Модуль:** CLI, GRAPH, PERF
**Приоритет:** major
**Сложность:** S (1-2 часа)
**Блокирует:** —
**Заблокирован:** —
**Related:** #018 (git_diff_analysis)

## Цель

До `git worktree add` и построения двух графов спросить git: «менялись ли
вообще `.h/.hpp/.c/.cpp/.cxx/.cc` между baseline и current?» Если нет —
сразу вернуть 0 и пустой отчёт. Архитектура не могла измениться.

## Контекст

Замер на реальном `gm` (Release-сборка, 2089 узлов, две родственные ревизии
без C++-правок):

```
$ time archcheck --diff c34a3bbb..c5dc4121 .     ← 74 секунды
$ time archcheck --graph .                       ←  1 секунда
$ time git worktree add --detach <tmp> <ref>     ← 34 секунды (одно дерево!)
```

Распределение: ~68 с уходит на два `git worktree add` (полный checkout +
hooks), ~1 с — оба прохода построения графа, остальное — `worktree remove` +
diff-логика. **Граф быстрый — bottleneck в git-операциях**, и в случае
«C++ не менялись» все 74 с — впустую.

Это типичный CI-кейс: PR с merge-commit-ом из main, где в main приехали
правки docs/yaml/python — а C++ не трогали. Платить минуту worktree+scan,
чтобы убедиться что 0 → 0, — расточительно.

Pre-check `git diff --name-only baseline..current -- '*.h' '*.hpp' '*.c' '*.cpp' '*.cxx' '*.cc'`
занимает миллисекунды. Если выход пуст — выходим с exit 0 без worktree-add.

Бонус: если изменилось 3 файла из 2089 — в будущем можно строить
**только** их под-граф, не сканируя весь проект. Это уже отдельная история
(#TODO: incremental graph build), здесь — минимальный fast-path.

## План выполнения

- [x] Добавить `git_state::changedCppFiles(repoRoot, baselineRef, currentRef) -> vector<path>` через `git diff --name-only --diff-filter=ACMRD <a> <b>` (фильтрация расширений делается в C++ через переиспользуемый `scan::hasProjectExtension`)
- [x] В `run_diff` (`src/main.cpp`) после `parseRevspec` и `findRepoRoot` — fast-path: если список пуст, печать «no C/C++ files changed; skipping graph build» и return 0
- [x] Edge-case `WORKTREE`: `git diff --name-only <baseline>` (без второго ref-а) для tracked-modified + `git ls-files --others --exclude-standard` для untracked
- [x] Расширения: вынесен публичный `archcheck::scan::hasProjectExtension()` — единый источник правды (12 расширений из `kExtensions`)
- [x] Тесты: 3 новых integration кейса в `tests/integration/diff/git_diff_test.cpp` — mixed-change, docs-only-empty, WORKTREE-modified-and-untracked
- [x] Перепрогнать на `gm`: docs-only коммит — **6 ms** (раньше 74 s, ускорение 12000×). 18 из 30 последних коммитов в gm — fast-path кандидаты

## Сделано

- **CLI fast-path** в [src/main.cpp:130-141](src/main.cpp#L130-L141) (`tryFastPathNoCppChanges`) — перед всеми git worktree операциями. Печатает `no C/C++ files changed; skipping graph build`, exit 0.
- **`changedCppFiles`** в [include/archcheck/git/git_state.h](include/archcheck/git/git_state.h) + [src/git/git_state.cpp](src/git/git_state.cpp) — две внутренние helper-функции (`collectChangedVsWorktree`, `collectChangedTwoRefs`), фильтрация через `scan::hasProjectExtension`.
- **`hasProjectExtension` выведен публично** в [include/archcheck/scan/project_files.h](include/archcheck/scan/project_files.h) — без дублирования списка расширений.
- **Тесты**: 134 кейса (было 131), +3 новых на `changedCppFiles`. Все 407 ассертов проходят.
- **Замер на gm Release**: docs-only коммит-пара (`d180d152..469d7d02`) — **0.006 s** вместо 74 s. Коммит-пара с одним `.cpp` (`c34a3bbb..c5dc4121`) — путь не сработал (правильно), полный прогон 68 s.
- **Аналитика gm**: 18/30 последних коммитов имеют `changedCppFiles = 0` → выигрывают от fast-path.
- **Lizard 0 warnings, clang-format-18 чист**.

## В работе

- (пусто) — задача завершена.

## Следующие шаги

1. **Готово к `/commit`**.
2. **Дальше**: задача [#024](backlog/new/024_maj_in_memory_fs_for_diff.md) — in-memory FS снимет оставшиеся 68 с (когда C++ всё-таки менялся, но мало).

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Pre-check через `git diff --name-only`, а не через сравнение списков от `discoverFiles` | git знает дельту в O(commits), а не O(files); работает даже без checkout-а |
| Patterns зашиты (`*.h *.hpp *.c *.cpp *.cxx *.cc`) | YAGNI: конфиг расширений приедет с общим config-loader-ом |
| Skip только when ровно ноль изменений | Если изменился хоть один C++ файл — идём по полному пути; инкрементальный граф — отдельная задача |
| Печатать явное «skipped» сообщение в stdout | CI-логи должны объяснять, почему так быстро |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/git/git_state.h` | + `changedCppFiles(repoRoot, baselineRef, currentRef)` |
| `src/git/git_state.cpp` | + реализация (один `runGit` + split lines) |
| `src/main.cpp` | + guard в `run_diff` |
| `tests/integration/diff/git_diff_test.cpp` | + кейс «PR трогает только docs → no worktree» |
