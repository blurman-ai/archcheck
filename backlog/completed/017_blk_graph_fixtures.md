# [FIXTURES] Graph integration fixtures

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** FIXTURES
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008 (dependency_graph_foundation) — закрывает fixtures-ветку
**Заблокирован:** #012 (include_resolver), #015 (graph_diff_primitives)
**Related:** #008 (dependency_graph_foundation)

## Цель

Собрать минимальный набор integration-фикстур, на которых проверяется сборка
графа из реального кода и diff-примитивы.

## Сделано

- **2026-05-26** — 7 фикстур в `fixtures/graph/`:
  - `minimal_dag/` — 3 файла, линейная цепочка a→b→c.
  - `single_scc/` — 3 файла, цикл a→b→c→a.
  - `new_edge/baseline/` + `new_edge/current/` — диф «появилось ребро a→c».
  - `shortcut_edge/baseline/` + `shortcut_edge/current/` — диф «добавлен shortcut a→d поверх a→b→c→d».
  - `cycle_growth/baseline/` + `cycle_growth/current/` — диф «SCC растёт с 2 до 3» (свободный c присоединяется к 2-циклу через `b→c`, `c→a`).
  - `unresolved_include/` — один `.cpp` с `#include "missing.h"`.
  - `ambiguous_include/` — `main.cpp` с `#include <util.h>`, две имплементации в `sub_a/util.h` и `sub_b/util.h`.
- **2026-05-26** — `tests/integration/graph/end_to_end_test.cpp` с 7 TEST_CASE, по одному на фикстуру. Helper `build_graph(root)` прогоняет `discover_files → scan_includes → resolve_includes → DependencyGraph::add_edge`, возвращает граф + counters external/unresolved/ambiguous.
- **2026-05-26** — `tests/CMakeLists.txt` подключает новый integration source. Полный счёт 115/115, lizard зелёный.

## Как работает

Пайплайн каждой фикстуры:

```
fixtures/graph/<scenario>/ → discover_files
                          → scan_includes per file
                          → resolve_includes (project / external / unresolved / ambiguous)
                          → DependencyGraph: add_node + add_edge (только project)
                          → compute_scc / added_edges / grown_sccs
```

Diff-сценарии (`new_edge`, `shortcut_edge`, `cycle_growth`) — два подкаталога
`baseline/` и `current/` рядом. Тест строит граф для каждого, передаёт пару в
`added_edges` или `grown_sccs`. Никакой git-магии не требуется.

`ambiguous_include` устроен через angle-include `<util.h>`: для angle resolver
сразу идёт в exact + suffix-search, минуя dir-relative. Два кандидата
`sub_a/util.h` и `sub_b/util.h` дают одинаковый суффикс `util.h` → ambiguous.

`unresolved_include` — single-file случай с `#include "missing.h"`: dir-rel
не находит, exact не находит, suffix не находит → Unresolved.

## Чем управляется

- `ARCHCHECK_FIXTURES_DIR` — compile definition из `tests/CMakeLists.txt`, путь к корню `fixtures/`.
- Других флагов нет.

## С чем связана

- Использует все слои pipeline: `scan/` (scanner + resolver + project_files), `graph/` (container + algorithms + diff).
- Не зависит от baseline-формата (#016) — diff делается между двумя графами, построенными прямо из файловых систем. Когда появится `archcheck --diff` (#018) с `git_state`, эти же фикстуры можно будет переиспользовать через git-репо.
- Не использует scanner-fixtures (#008h) — там single-file сценарии для самого scanner-а.

## Диагностика

- Если новая фикстура не находится — проверь, что путь в `graph_fixture("…")` совпадает с именем директории на диске.
- Если diff неожиданно пустой — fixtures `baseline/` и `current/` могут разойтись по структуре файлов, а не только по содержимому. Resolver сравнивает пути, поэтому переименование файла = «удалён узел + добавлен узел», а не «изменён узел».
- Если SCC-related тесты падают — `compute_scc` стабилизирован (детерминизм), но фикстуры с эквивалентными циклами могут дать разную форму SCC-векторов. Проверка идёт по `size`, не по составу.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Фикстуры — реальные `.h`/`.cpp`, не in-memory строки | Это и проверяет zero-config дискавери — если дискавери что-то пропустит, тесты заметят |
| `baseline/` + `current/` подкаталоги для diff | Простой способ показать «до» и «после» без git-машинерии. #018 потом сможет переиспользовать через `git_state` |
| Angle-include `<util.h>` для ambiguous-кейса | Quote сначала ищет в dir-relative, что усложнило бы фикстуру; angle идёт сразу в exact + suffix |
| Helper `build_graph(root)` собран в тестовом TU, не в src/ | YAGNI — пока этот объединяющий «end-to-end билдер» нужен только тестам; когда придёт #018 / CLI, выделим в src/ |
| Один TEST_CASE на фикстуру | Читаемо, легко отдельно гнать через `ctest -R`, error-message сразу указывает на конкретный сценарий |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `fixtures/graph/minimal_dag/{a,b,c}.h` | new |
| `fixtures/graph/single_scc/{a,b,c}.h` | new |
| `fixtures/graph/new_edge/{baseline,current}/{a,b,c}.h` | new |
| `fixtures/graph/shortcut_edge/{baseline,current}/{a,b,c,d}.h` | new |
| `fixtures/graph/cycle_growth/{baseline,current}/{a,b,c}.h` | new |
| `fixtures/graph/unresolved_include/main.cpp` | new |
| `fixtures/graph/ambiguous_include/main.cpp` + `sub_{a,b}/util.h` | new |
| `tests/integration/graph/end_to_end_test.cpp` | new (7 кейсов + helper) |
| `tests/CMakeLists.txt` | подключён integration test source |
