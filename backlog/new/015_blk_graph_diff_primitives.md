# [GRAPH] Graph diff primitives — new/removed edges, grown SCC

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** GRAPH
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #016 (graph_baseline_contract)
**Заблокирован:** #014 (graph_algorithms), #012 (include_resolver)
**Related:** #008 (dependency_graph_foundation), #009 (ai_drift_regression_rules)

## Цель

Реализовать примитивы сравнения двух графов: новые рёбра, удалённые рёбра,
вырос ли SCC относительно baseline.

## Контекст

См. план #008: «diff-примитивы для первого прототипа: new edges, removed
edges, grown SCC». Это та основа, без которой нельзя будет сделать первый
прототип `DRIFT.1` / `DRIFT.2` в #009.

«Grown SCC» = появилось SCC размера ≥ 2 там, где в baseline его не было, или
размер существующего SCC увеличился.

## План выполнения

- [ ] `struct EdgeRef { NodeId from; NodeId to; }`
- [ ] `std::vector<EdgeRef> added_edges(baseline, current)`
- [ ] `std::vector<EdgeRef> removed_edges(baseline, current)`
- [ ] `struct GrownScc { std::vector<NodeId> members; std::size_t baseline_size; std::size_t current_size; }`
- [ ] `std::vector<GrownScc> grown_sccs(baseline, current)` — сопоставление по пересечению membership
- [ ] Тесты: «появилось новое ребро», «shortcut edge», «cycle growth», «удалено ребро», «без изменений»

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Передать эстафету в #016 (baseline contract)

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Сопоставление SCC по membership, не по ID | NodeId стабилен только внутри одного запуска, baseline → current не гарантирует совпадения |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/graph/diff.h` | new |
| `src/graph/diff.cpp` | new |
| `tests/unit/graph/diff_test.cpp` | new |
