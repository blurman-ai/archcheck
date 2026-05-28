# [BUILD/TESTS] Поднять пороги coverage gate до 90% по всем метрикам

**Дата создания:** 2026-05-28
**Дата старта:** 2026-05-28
**Статус:** wip
**Модуль:** BUILD / TESTS
**Приоритет:** major
**Сложность:** medium
**Блокирует:** —
**Заблокирован:** —
**Related:** —

## Цель

Поднять пороги в `scripts/check_coverage.sh` с `70/60/40` до `90/90/90`
и написать недостающие тесты, чтобы coverage gate на этих порогах стабильно проходил.

## Контекст

Проект новый, код весь под контролем — нет причин держать низкие пороги.
Текущее состояние (production code `src/` + `include/`, после фильтрации):

| Метрика   | Сейчас | Цель | Разрыв |
|-----------|--------|------|--------|
| Lines     | 90.1%  | 90%  | уже ✓  |
| Functions | 94.2%  | 90%  | уже ✓  |
| Branches  | 58.7%  | 90%  | −31 п.п. |

Единственный барьер — **branch coverage**. Lines и functions уже проходят.

## Находки: где конкретно провалы

Данные из `lcov --list` по `src/`:

| Файл | Branches сейчас | Приоритет добивки |
|------|----------------|-------------------|
| `src/diff/regression_report.cpp` | **36.5%** | высокий |
| `src/git/git_object_file_source.cpp` | **46.6%** | высокий |
| `src/scan/disk_file_source.cpp` | **50.0%** | высокий |
| `src/git/git_state.cpp` | 52.5% | высокий |
| `src/graph/graph_builder.cpp` | 54.1% | средний |
| `src/rules/sf9_no_cycles.cpp` | 55.9% | средний |
| `src/graph/baseline.cpp` | 52.9% | средний |
| `src/graph/algorithms.cpp` | 70.5% | низкий |
| `src/graph/dependency_graph.cpp` | 68.5% | низкий |
| `src/graph/diff.cpp` | 68.6% | низкий |
| `src/rules/sf7_using_namespace.cpp` | 64.6% | низкий |
| `src/rules/sf8_include_guard.cpp` | 70.0% | низкий |
| `src/scan/include_resolver.cpp` | 69.0% | низкий |
| `src/scan/include_scanner.cpp` | 76.1% | низкий |
| `src/scan/project_files.cpp` | 65.5% | низкий |

## Причины провалов (типовые по группам)

**`regression_report.cpp` (36.5%)** — не покрыты:
- пустой список violations
- violations с одинаковыми file-путями (сортировка/сравнение)
- вывод в нестандартные потоки

**`git_object_file_source.cpp` + `git_state.cpp` (46–52%)** — не покрыты:
- ошибки при чтении git-объектов (битый sha, несуществующий ref)
- пустой worktree / detached HEAD
- репо без коммитов (init без first commit)

**`disk_file_source.cpp` (50%)** — не покрыты:
- файл не существует / нет прав чтения
- путь — директория, а не файл

**`graph_builder.cpp` (54%)** — не покрыты:
- граф с нулём вершин
- файл с нулём инклудов
- конфликт между include-путями (несколько candidates)

**`sf9_no_cycles.cpp` (55%)** — не покрыты:
- граф без цикла (pass-случай в full-pipeline)
- self-loop как частный случай цикла
- несколько независимых циклов одновременно

## Артефакты gcov (не тестировать, а исключить)

gcov добавляет ветви для implicit null-checks в operator= и деструкторах, `noexcept` throw-paths.
Такие строки пометить `// LCOV_EXCL_LINE` или блоком `// LCOV_EXCL_START / LCOV_EXCL_STOP`.

## План выполнения

- [ ] Открыть HTML-отчёт (`build/coverage/coverage_html/index.html`), пройти по красным веткам
- [ ] `regression_report.cpp` — добавить тесты: пустой список, одна violation, сортировка
- [ ] `git_object_file_source.cpp` — добавить тесты с несуществующим sha / ref
- [ ] `disk_file_source.cpp` — добавить тесты: файл не существует, нет прав
- [ ] `git_state.cpp` — добавить тесты: init-репо без коммитов, detached HEAD
- [ ] `graph_builder.cpp` — добавить тест: пустой граф, нет инклудов
- [ ] `sf9_no_cycles.cpp` — добавить fixture pass/ и тест self-loop
- [ ] Среднеприоритетные файлы — аналогично, по HTML-отчёту
- [ ] Пометить `// LCOV_EXCL_LINE` там, где ветвь — артефакт компилятора
- [ ] Убедиться `bash scripts/check_coverage.sh` — PASS
- [ ] Поднять пороги в `scripts/check_coverage.sh`: `MIN_LINES=90 MIN_FUNCTIONS=90 MIN_BRANCHES=90`
- [ ] Коммит: `test(tests): raise coverage thresholds to 90% across all metrics`

## Сделано

- (пусто)

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Пороги менять только после добивки, не до | Иначе ломаются коммиты |
| LCOV_EXCL для compiler artifacts | Законно, убирает шум без потери смысла |
| HTML-отчёт как навигатор | Открывать и смотреть красные строки, не гадать |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `scripts/check_coverage.sh` | Поднять MIN_LINES/FUNCTIONS/BRANCHES до 90 |
| `tests/unit/*/` | Новые тест-кейсы на error paths |
| `src/**/*.cpp` | LCOV_EXCL там где нужно |
