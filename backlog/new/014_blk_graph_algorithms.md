# [GRAPH] Graph algorithms — SCC + reachability

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** GRAPH
**Приоритет:** blocker
**Сложность:** M (1-2 дня)
**Блокирует:** #015 (diff_primitives)
**Заблокирован:** #013 (graph_container)
**Related:** #008 (dependency_graph_foundation)

## Цель

Реализовать алгоритмы-примитивы: SCC (Tarjan), forward reachable set, reverse
reachable set, path existence. Каждый — отдельная функция, без общего
«engine».

## Контекст

См. план #008: «graph algorithms, нужные как примитивы». На этих примитивах
позже будут строиться правила cycle detection и Lakos CCD/ACD/NCCD, а также
diff-примитивы (#015).

## План выполнения

- [ ] `std::vector<std::vector<NodeId>> compute_scc(graph)` — Tarjan iterative (без рекурсии — стек C++ на 100k+ файлов опасен)
- [ ] `std::unordered_set<NodeId> reachable_from(graph, NodeId)` — BFS по successors
- [ ] `std::unordered_set<NodeId> reverse_reachable_from(graph, NodeId)` — BFS по predecessors
- [ ] `bool has_path(graph, NodeId from, NodeId to)` — ранний выход BFS
- [ ] Тесты: DAG без циклов (все SCC размера 1), один большой SCC, два независимых компонента, reachability в обе стороны

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Передать эстафету в #015

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Tarjan iterative, не recursive | На больших графах рекурсия упадёт по stack overflow |
| Отдельные free functions, не методы класса | См. план #008: «без лишней абстракции», и тестируется проще |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/graph/algorithms.h` | new |
| `src/graph/algorithms.cpp` | new |
| `tests/unit/graph/algorithms_test.cpp` | new |
