# [RULES][REPORT][CLI] Zero-config rules engine MVP

**Дата создания:** 2026-05-28
**Дата завершения:** 2026-05-28
**Статус:** completed
**Модуль:** RULES, REPORT, CLI
**Приоритет:** major
**Сложность:** L (убрали YAML-конфиг из скоупа → реализуется как zero-config)
**Related:** план `yaml-fuzzy-knuth`

## Цель

Реализовать zero-config rules engine для v0.1 MVP: `archcheck [path]` без
каких-либо настроек сразу даёт полезный результат по 5 default-правилам.

## Решение (конфиг вынесен в v0.2)

YAML-конфиг убран из MVP — высокий порог входа, никто не будет описывать
модули при первом запуске. Zero-config experience важнее для adoption.
MVP = `archcheck [path]` без конфига → полезный результат.

SF.21 (anonymous namespace) убран из MVP — требует libclang для точного
определения, approximate text-scan даёт ложные позитивы.

## Сделано

### Инфраструктура правил
- `include/archcheck/rules/violation.h` — `struct Violation{ruleId, file, line, message}` + `ViolationList`
- `include/archcheck/rules/i_rule.h` — `class IRule` (pure abstract), `check(graph, readFile lambda)`
- `include/archcheck/scan/project_files.h` + `src/scan/project_files.cpp` — `isHeaderFile()` (8 расширений: .h .hh .hpp .hxx .ipp .tpp .inl .inc)

### Правила
- **SF.9 NoCycles** — SCC ≥ 2 → violation с форматом «cycle: a → b → c → a» (DFS внутри SCC)
- **SF.7 UsingNamespace** — построчный scan заголовков, strip `//` comments, detect `using namespace`
- **SF.8 IncludeGuard** — проверка первых 30 непустых строк на `#pragma once` или `#ifndef`
- **Lakos.GodHeader** — `predecessors(id).size() > 30` → violation
- **Lakos.ChainLength** — `computeIncludeDepths()` (condensation + memoised DFS) > 10 → violation

### Graph
- `computeIncludeDepths(graph)` — longest path per node через condensation DAG

### Reporters
- `writeTextReport(violations, ostream)` — `file:line: [rule] message` + summary
- `writeJsonReport(violations, ostream)` — schema version 1, `{"version":1,"violations":[...],"summary":{...}}`

### CLI
- `run_check(root, fmt)` — строит граф, гоняет все правила, выводит отчёт
- `archcheck [path]` — zero-config entry point (было `print_help()`)
- `archcheck --format json [path]` — JSON output
- Exit 1 при нарушениях, 0 если чисто

### Fixtures
- `fixtures/sf7_using_namespace/pass/`, `fail_global/`
- `fixtures/sf8_include_guard/pass/`, `fail_missing/`
- `fixtures/sf9_no_cycles/pass/`, `fail_simple/`
- `fixtures/lakos_chain_length/pass/` (5 файлов), `fail_deep/` (12 файлов)
- `fixtures/lakos_god_headers/pass/` (5 пользователей), `fail_high_fanin/` (31 + god.h)

## Верификация

- **164/164 тестов** — 100% pass
- **Coverage**: lines 90.4%, functions 93.2%, branches 58.9% (все пороги PASS)
- **Dogfood**: `archcheck .` находит нарушения только в fixture-файлах, production-код чистый
- **JSON**: `archcheck --format json .` выдаёт валидный JSON, exit 1

## Ключевые решения

| Решение | Причина |
|---------|---------|
| readFile lambda в IRule::check() | Тесты не ходят на диск — передают строки напрямую |
| Eager string resolution (NodeId → path) при построении Violation | Нет зависимости от lifetime графа |
| computeIncludeDepths через condensation | Циклические SCCs получают одинаковую глубину без бесконечного рекурса |
| YAML-конфиг → v0.2 | Zero-config adoption важнее гибкости на старте |
| SF.21 → v0.2 | Требует libclang; approximate text-scan ненадёжен |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/rules/violation.h` | new |
| `include/archcheck/rules/i_rule.h` | new |
| `include/archcheck/rules/rule_set.h` | new |
| `include/archcheck/rules/sf{7,8,9}_*.h` | new |
| `include/archcheck/rules/lakos_*.h` | new |
| `include/archcheck/report/{text,json}_reporter.h` | new |
| `include/archcheck/scan/project_files.h` | + `isHeaderFile()` |
| `include/archcheck/graph/algorithms.h` | + `computeIncludeDepths()` |
| `src/rules/*.cpp` | new (6 файлов) |
| `src/report/*.cpp` | new (2 файла) |
| `src/graph/algorithms.cpp` | + `computeIncludeDepths()` |
| `src/CMakeLists.txt` | + новые исходники, fix stdc++fs |
| `src/main.cpp` | + run_check, --format, zero-arg entry |
| `tests/CMakeLists.txt` | + новые тесты, fix stdc++fs |
| `tests/unit/rules/*.cpp` | new (5 файлов) |
| `fixtures/` | new — 5 наборов pass/fail |
