# [GRAPH] Graph container — NodeId + dependency_graph

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** GRAPH
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #014 (graph_algorithms)
**Заблокирован:** —
**Related:** #008 (dependency_graph_foundation)

## Цель

Реализовать минимальный канонический контейнер file-level графа: `NodeId`,
нормализация путей, add node/edge, adjacency, reverse adjacency, edge
existence.

## Контекст

См. §«Канонический граф — file-level» в #008 и план «Спроектировать
component_graph / dependency_graph API без лишней абстракции». Никаких
template-генериков, никакой внешней graph-библиотеки — `unordered_map<NodeId,
vector<NodeId>>` достаточно.

## План выполнения

- [ ] `struct NodeId { std::uint32_t value; }` со strong typing, `operator==`, `std::hash`
- [ ] `class DependencyGraph` с операциями: `add_node(path) → NodeId`, `add_edge(from, to)`, `has_edge`, `successors(NodeId)`, `predecessors(NodeId)`, `nodes()`, `path_of(NodeId)`
- [ ] Внутреннее хранение forward + reverse adjacency (для будущих reverse reachable / Ce/Ca)
- [ ] Нормализация путей при `add_node` (POSIX-сепараторы, без `./`)
- [ ] Тесты: add/lookup, дубли nodes (idempotent), дубли edges (idempotent), reverse adjacency корректна

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Передать эстафету в #014

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Reverse adjacency хранится сразу | Lakos Ca / reverse reachable дешевле получать сразу, чем перестраивать |
| `NodeId` — `uint32_t` wrapper | Дешёвый хеш, меньше памяти, чем хранить strings везде |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/graph/node_id.h` | new |
| `include/archcheck/graph/dependency_graph.h` | new |
| `src/graph/dependency_graph.cpp` | new |
| `src/CMakeLists.txt` | подключить `src/graph/` |
| `tests/unit/graph/dependency_graph_test.cpp` | new |
