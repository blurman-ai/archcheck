# [RULES][SF] Уменьшить количество параметров findCycleImpl

**Дата создания:** 2026-05-28
**Дата старта:** 2026-05-28
**Статус:** done
**Модуль:** RULES
**Приоритет:** minor
**Сложность:** small
**Блокирует:** —
**Заблокирован:** —
**Related:** —

## Цель

Устранить нарушение lizard-порога (≤5 параметров) в `findCycleImpl`, не меняя алгоритм.

## Контекст

`findCycleImpl` в `src/rules/sf9_no_cycles.cpp` — внутренний DFS для поиска цикла
внутри одного SCC имела 7 параметров при пороге lizard ≤5.

### Почему возникло

Функция написана как свободный рекурсивный DFS с переносом всего mutable-состояния
через стек вызовов. `bool started` — костыль чтобы не остановиться на стартовой
вершине сразу (`cur == target`). В C++ это естественно решается через `struct`
с полями — состояние живёт в полях, а не параметрах.

## Сделано

- [x] Ввести `struct CycleFinder` в анонимном namespace
- [x] Перенести `findCycleImpl` в `CycleFinder::dfs(NodeId cur)` — 1 параметр
- [x] Убрать `bool started`; стартовый вызов идёт по соседям `start`
- [x] lizard: 0 нарушений
- [x] Тесты: 11 assertions in 4 test cases — all passed

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `src/rules/sf9_no_cycles.cpp` | `findCycleImpl` → `CycleFinder::dfs` |

---

**Дата завершения:** 2026-05-28

## Как работает

`CycleFinder` — struct в anonymous namespace. Константные поля (`g`, `members`,
`target`) инициализируются при создании. Мutable-состояние (`vis`, `path`) —
поля, накапливаются в `dfs()`. `check()` создаёт `CycleFinder` на каждый SCC
и запускает `finder.dfs(next)` по соседям стартовой вершины — это заменяет
`bool started` без дополнительной логики.

## Чем управляется

Ничем — внутренняя деталь реализации `Sf9NoCycles::check()`.

## С чем связана

`src/rules/sf9_no_cycles.cpp`, `include/archcheck/rules/sf9_no_cycles.h`,
`tests/unit/rules/sf9_no_cycles_test.cpp`.

## Диагностика

При регрессии: `lizard --arguments 5 --warnings_only src/rules/sf9_no_cycles.cpp`
должен вернуть 0 строк вывода.
