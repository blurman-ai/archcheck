# [SCAN][GRAPH][PERF] In-memory FS: читать блобы git-а напрямую, без worktree-add

**Дата создания:** 2026-05-27
**Дата старта:** 2026-05-27
**Статус:** wip
**Модуль:** SCAN, GRAPH, PERF
**Приоритет:** major
**Сложность:** L (3-5 дней)
**Блокирует:** —
**Заблокирован:** —
**Related:** #018 (git_diff_analysis), #023 (skip_when_no_cpp_changes)

## Цель

Научить `scan/` и `graph/graph_builder` принимать **абстракцию файловой
системы** (источник «список файлов + читалка содержимого»), и для `--diff`
использовать реализацию поверх `git cat-file --batch` / libgit2, читающую
блобы напрямую из object DB без записи на диск.

Цель — снять с `--diff` основной cost: 32-35 с × 2 worktree-add на каждый
прогон. Грубая оценка: <2 с на оба прохода вместо ~70 с.

## Контекст

[scripts/bench_materialize.sh](../../scripts/bench_materialize.sh) сравнил
5 способов материализовать ref в каталог на `gm` (2089 C++-файлов, Release):

| Метод | Materialize | Scan |
|---|---|---|
| `git worktree add` | 34.8 с | 0.18 с |
| `git archive | tar -x` | 32.5 с | 0.15 с |
| `git checkout-index` | 36.1 с | 0.17 с |
| `git restore --source` | 33.2 с | 0.17 с |
| `git clone --shared` | 47.1 с | 0.44 с |

**Все disk-based методы упёрты в I/O** (запись 2089 файлов в /tmp). Сам
сканер — ~170 мс. Менять `worktree add` на `archive` даст 7% — не стоит
свеч сам по себе.

Существующие инструменты для many-commit-анализа (PyDriller, CodeMaat)
**не делают checkout** — читают блобы напрямую из object DB через
pygit2/libgit2 или `git cat-file --batch`. Это правильный путь.

## План выполнения

- [x] Спроектировать `archcheck::scan::FileSource` (без `I`-префикса — стиль-гайд запрещает): `list() -> vector<ProjectFile>`, `read(path) -> string`
- [x] Перевести `scan::discoverFiles` + `buildGraphForPath` на работу через эту абстракцию (нынешний код становится `DiskFileSource`)
- [ ] Добавить `git::GitObjectFileSource(repo, ref)`: enumerate через `git ls-tree -r <ref>`, content через `git cat-file --batch` (один пайп, batch-протокол)
- [ ] В `run_diff` использовать `GitObjectFileSource` для baseline и (опционально) для current, оставив `DiskFileSource` для `WORKTREE`
- [ ] Бенчмарк: на том же `gm` `--diff` должен сократиться с ~75 с до <5 с
- [ ] Тесты:
  - unit для `GitObjectFileSource` на temp git-репо (читает блобы, видит правильное дерево)
  - integration `--diff` через in-memory путь — все 6 кейсов из `git_diff_test.cpp` должны зелениться без модификации (тестируется через тот же API `diffRefs`)
- [ ] CLI-флаг `--diff-mode=disk|memory` для отката, default `memory`
- [ ] Удалить `git worktree add` из `materializeRef` для не-WORKTREE кейса (но оставить как fallback через `--diff-mode=disk`)
- [ ] Перепрогнать `bench_materialize.sh` с новой 6-й методикой `git-objects` (in-memory чтение, scan читает из map<path, content>)

## Сделано

- **2026-05-27** — `scan::FileSource` (абстрактная база) + `scan::DiskFileSource` (обёртка над текущим `discoverFiles` + `ifstream`). Названо без `I`-префикса согласно [docs/code_style.md](../../docs/code_style.md) §«Что мы НЕ делаем».
- **2026-05-27** — `graph::buildGraphForSource(FileSource&)` стал source-of-truth pipeline, `buildGraphForPath(root)` — тонкий wrapper через `DiskFileSource(root)`. Поведение байт-в-байт прежнее.
- Все 131 тест зелёные после рефактора. Lizard `--CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/` — clean.

## В работе

- (нет активного шага — готов перейти к `GitObjectFileSource`)

## Следующие шаги

1. `git::GitObjectFileSource(repoRoot, ref)`: один долгоживущий `git cat-file --batch` процесс; `list()` через `git ls-tree -r --name-only <ref>` + фильтр по тем же расширениям, что и `discoverFiles`.
2. Unit-тесты на temp git-репо (списки совпадают с disk; содержимое блобов совпадает с `ifstream`).
3. Подключить к `run_diff` (с CLI-флагом `--diff-mode=disk|memory`, default `memory`); проверить, что 6 интеграционных кейсов в `git_diff_test.cpp` зелёные без модификации.
4. Замерить через обновлённый `bench_materialize.sh`. Если ≥ 10× — обсудить libgit2.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| In-memory путь через `git cat-file --batch`, не libgit2 | Не тянем новую зависимость (libgit2). `cat-file --batch` — один процесс, batch-protocol, минимальный overhead по сравнению с per-file `git show`. libgit2 — оптимизация ещё на следующем шаге, если потребуется |
| `IFileSource` абстракция, не два разных pipeline-а | DRY: тесты, sanity checks, граф-логика — одни. Бенчмарк уже показал что scan стоит 0.17 с, переход на абстракцию не должен это сильно ухудшить |
| `--diff-mode=disk` как escape hatch | На случай странных репо (submodules? LFS? sparse?) у пользователя должен быть путь откатиться к проверенному worktree-методу. Удалить флаг можно после v0.2 если жалоб нет |
| `WORKTREE` (текущее рабочее дерево) остаётся через DiskFileSource | Рабочее дерево — это именно файлы на диске (могут быть uncommitted), там по определению нет blob-ов. Не пытаться унифицировать |
| Не идти в libgit2 на этом шаге | YAGNI. Сначала измерить `cat-file --batch` выигрыш; если < 10× — libgit2 не оправдан. Если ≥ 10× — обсуждать libgit2 как next step |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/scan/file_source.h` | new — концепт/интерфейс `IFileSource` |
| `include/archcheck/scan/disk_file_source.h` | new — обёртка над `std::filesystem` + `std::ifstream` |
| `src/scan/disk_file_source.cpp` | new — реализация (move-нутая логика из `discoverFiles` + readFile) |
| `include/archcheck/git/git_object_file_source.h` | new — `GitObjectFileSource(repo, ref)` |
| `src/git/git_object_file_source.cpp` | new — реализация через `git cat-file --batch` |
| `include/archcheck/graph/graph_builder.h` | + перегрузка `buildGraphForSource(IFileSource&)` |
| `src/graph/graph_builder.cpp` | + реализация через source-abstract pipeline |
| `src/main.cpp` | run_diff: использовать `GitObjectFileSource` для git-ref-ов; добавить `--diff-mode` |
| `tests/unit/git/git_object_file_source_test.cpp` | new — unit |
| `tests/integration/diff/git_diff_test.cpp` | + проверка что in-memory path даёт те же результаты что disk path (parametrize) |
| `scripts/bench_materialize.sh` | + 6-й метод `git-objects` (in-memory, без записи на диск) |
