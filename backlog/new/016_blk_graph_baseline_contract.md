# [GRAPH] Graph-baseline contract — формат и I/O

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** GRAPH
**Приоритет:** blocker
**Сложность:** M (1-2 дня)
**Блокирует:** #008 (dependency_graph_foundation) — закрывает graph-ветку
**Заблокирован:** #015 (graph_diff_primitives)
**Related:** #008 (dependency_graph_foundation), #009 (ai_drift_regression_rules)

## Цель

Зафиксировать минимальный устойчивый формат `graph-baseline`:
`format_version + normalized nodes + normalized edges`, плюс save/load.

## Контекст

См. §5 mini-design в #008: «`graph-baseline` хранит только nodes + resolved
project edges». Это контракт для CI: baseline лежит в репо, сравнивается с
текущим графом, любые drift-нарушения видны в diff.

Формат — детерминированный (отсортированные nodes/edges), читаемый человеком
(YAML через `ryml`, уже зависит весь core), с явной версией.

## План выполнения

- [ ] Спецификация формата (одна страница в `docs/`): `format_version`, `nodes: [string]`, `edges: [[from_idx, to_idx]]`
- [ ] `save_baseline(graph, std::ostream&)` — отсортированные пути узлов, отсортированные рёбра
- [ ] `load_baseline(std::istream&) → Baseline` с проверкой `format_version`
- [ ] Ошибки парсинга → структурированный `BaselineLoadError` (не исключения наружу)
- [ ] Тесты: round-trip (save → load → save идемпотентны), отказ на неизвестной версии, корректная детерминированность вывода на одинаковом графе

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Закрыть #008 переездом в `completed/`
2. Перейти к #009 (DRIFT.1 / DRIFT.2 прототип)

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Edges как `[from_idx, to_idx]`, а не пары полных путей | Компактнее, и диффы по baseline стабильнее при перестановках строк |
| `format_version` строкой (`"1"`), не числом | YAML-числа имеют свои сюрпризы, строка проще |
| Один YAML-документ, не два файла | Простота для пользователя; v0.1 не нуждается в шардинге |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `docs/baseline_format.md` | new (мини-спека) |
| `include/archcheck/graph/baseline.h` | new |
| `src/graph/baseline.cpp` | new |
| `tests/unit/graph/baseline_test.cpp` | new |
