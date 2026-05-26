# [FIXTURES] Graph integration fixtures

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** FIXTURES
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008 (dependency_graph_foundation) — закрывает fixtures-ветку
**Заблокирован:** #012 (include_resolver), #015 (graph_diff_primitives)
**Related:** #008 (dependency_graph_foundation)

## Цель

Собрать минимальный набор integration-фикстур, на которых проверяется сборка
графа из реального кода и diff-примитивы.

## Контекст

См. список в #008 («Fixtures»). Это не unit-тесты компонентов (они уже идут с
каждой подзадачей), а end-to-end сценарии: `discover_files → scan → resolve
→ build_graph → diff`. Из [MVP.md](../../docs/MVP.md): «If feature cannot be
tested with fixtures — do not implement it.»

## План выполнения

- [ ] `fixtures/graph/minimal_dag/` — 3 файла, линейная зависимость
- [ ] `fixtures/graph/single_scc/` — 3 файла, цикл a→b→c→a
- [ ] `fixtures/graph/new_edge/` — два состояния (baseline / current), добавлено ребро
- [ ] `fixtures/graph/shortcut_edge/` — добавлено ребро поверх существующего пути
- [ ] `fixtures/graph/cycle_growth/` — SCC размера 2 → 3
- [ ] `fixtures/graph/unresolved_include/` — `#include "missing.h"` без кандидата
- [ ] `fixtures/graph/ambiguous_include/` — два кандидата с одинаковым suffix
- [ ] Integration тесты `tests/integration/graph/end_to_end_test.cpp` против каждой фикстуры

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Прогнать end-to-end, удостовериться что pipeline зеленый
2. Закрыть #008

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Фикстуры — реальные `.h` / `.cpp`, не in-memory строки | Это и есть проверка zero-config дискавери |
| `baseline` / `current` для diff-сценариев — два подкаталога | Простой способ показать «до» и «после» без git-магии |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `fixtures/graph/*/` | new (7 сценариев) |
| `tests/integration/graph/end_to_end_test.cpp` | new |
| `tests/CMakeLists.txt` | подключить integration test source |
