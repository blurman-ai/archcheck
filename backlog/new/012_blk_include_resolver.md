# [SCAN] Include resolver — zero-config repo-local

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** M (1-2 дня)
**Блокирует:** #015 (diff_primitives), #017 (graph_fixtures)
**Заблокирован:** #008g (include_scanner_macro_include_diagnostic), #011 (discover_files_and_indexes)
**Related:** #008 (dependency_graph_foundation)

## Цель

Превратить `IncludeDirective` + project indexes в `ResolvedInclude` одной из
четырёх категорий: `project` / `external` / `unresolved` / `ambiguous`.

## Контекст

См. §4 mini-design в #008. Это сердце zero-config extraction layer: именно
здесь принимается решение, попадает ли include в граф ребром.

## План выполнения

- [ ] `enum class Resolution { Project, External, Unresolved, Ambiguous }`
- [ ] `struct ResolvedInclude { ... }` (директива + Resolution + target NodeId или candidates)
- [ ] Алгоритм для `#include "x"`: directory of current file → exact match → unique suffix → ambiguous/unresolved
- [ ] Алгоритм для `#include <x>`: exact match → unique suffix → ambiguous/external
- [ ] Дедупликация повторных директив на уровне `(source, target)`, но сохранение всех locations
- [ ] Тесты на каждый из четырёх исходов для quote и angle (8 случаев + multi-candidate)

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Передать эстафету в #015 (diff) и #017 (fixtures)

## Ключевые решения

| Решение | Причина |
|---------|---------|
| `angle` без project-match → `external`, не `unresolved` | Соответствует обычной ожиданной семантике system headers |
| Не пытаться различать `external` и «забытая зависимость» | Это работа отдельного будущего слоя (vendored / external manifest) |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/scan/resolved_include.h` | new |
| `src/scan/include_resolver.cpp` | new |
| `include/archcheck/scan/include_resolver.h` | new |
| `tests/unit/scan/include_resolver_test.cpp` | new |
