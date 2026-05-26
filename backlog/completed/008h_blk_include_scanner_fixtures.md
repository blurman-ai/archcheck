# [SCAN][FIXTURES] Include scanner — integration fixtures с диска

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN, FIXTURES
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** —
**Заблокирован:** #008g (include_scanner_macro_include_diagnostic)
**Related:** #008 (dependency_graph_foundation), #017 (graph_fixtures)

## Цель

Зафиксировать поведение include-scanner-а на реальных файлах с диска, не только
на inline-строках в unit-тестах.

## Контекст

После 008a…g scanner полностью покрыт unit-тестами (33 кейса). Но клиент
scanner-а в реальности будет читать `*.h` / `*.cpp` с диска через resolver и
discovery (#011/#012). Чтобы убедиться, что `scan_includes(std::string_view)`
не страдает от различий «inline literal vs file content» (BOM, trailing
whitespace, кодировка, бинарное чтение), нужен integration слой: фикстуры
лежат отдельными файлами в `fixtures/scan/include_scanner/`, тест читает их и
прогоняет через `scan_includes`.

Это **не** делает scanner правилом (rules имеют формат `pass/` + `fail_*/` per
MVP.md). Scanner — extraction primitive, поэтому фикстуры плоские: одна .cpp
на сценарий.

## Сделано

- **2026-05-26** — `fixtures/scan/include_scanner/` с 6 файлами:
  - `simple.cpp` — angle + quote + angle.
  - `comments.cpp` — `//` и многострочный `/* */`.
  - `string_literal.cpp` — `#include` внутри обычной строки.
  - `raw_string.cpp` — `#include` внутри `R"(…)"`.
  - `continuation.cpp` — `\\\n` внутри `#include`.
  - `macro_include.cpp` — `#include CONFIG_HEADER` → diagnostic.
- **2026-05-26** — `tests/integration/scan/scanner_fixtures_test.cpp` — 6 TEST_CASE, читают файл и проверяют конкретные ожидания по `directives` и `diagnostics`.
- **2026-05-26** — `tests/CMakeLists.txt` подключает integration test source, передаёт `ARCHCHECK_FIXTURES_DIR` как compile definition, и добавляет линковку `stdc++fs` для GCC < 9 (системный g++ 8.3 в среде разработки).
- **2026-05-26** — Debug-сборка чистая, ctest 39/39 зелёные.

## Как работает

Test source хранит абсолютный путь к фикстурам через макрос
`ARCHCHECK_FIXTURES_DIR`, прокинутый CMake-ом:

```cmake
target_compile_definitions(archcheck_tests PRIVATE
   ARCHCHECK_FIXTURES_DIR="${CMAKE_SOURCE_DIR}/fixtures"
)
```

В тесте helper:

```cpp
std::filesystem::path fixture(std::string_view name)
{
   return std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "scan" / "include_scanner" / name;
}
```

`read(p)` открывает файл в `std::ios::binary` и забирает всё через
`istreambuf_iterator`. Результат скармливается в `scan_includes`. Дальше —
обычные `REQUIRE` на size, kind, token, line.

## Чем управляется

- `ARCHCHECK_FIXTURES_DIR` — задаётся CMake-ом, единственный «магический» путь.
- Никаких env vars / runtime-флагов.

## С чем связана

- Public API scanner-а — без изменений.
- CMake target `archcheck_tests` теперь линкуется ещё с `stdc++fs` (на GCC ≥ 9 — no-op).
- Когда появится resolver (#012), эти же фикстуры можно переиспользовать как «маленький проект» для проверки discovery.

## Диагностика

- Если падает `REQUIRE(f.is_open())` — проверь, что `ARCHCHECK_FIXTURES_DIR` смотрит на правильный путь (CMake-cache мог застрять, `cmake --build` после `cmake -B` спасает).
- Если на новой машине билд не линкуется по `<filesystem>` — обнови CMake-проверку версии GCC.
- Если новая фикстура «не находится» — проверь имя в `fixture("...")`, оно должно совпадать с файлом дословно.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Плоская структура `fixtures/scan/include_scanner/*.cpp` | Scanner — primitive, не правило; `pass/`/`fail_*/` иерархия для него избыточна |
| Один TEST_CASE на фикстуру | Читаемее, чем data-driven; каждая фикстура отвечает на конкретный вопрос |
| `ARCHCHECK_FIXTURES_DIR` как compile def | Не нужно тащить env var и устанавливать его в CI; CMake уже знает корень репо |
| Бинарное чтение через `istreambuf_iterator` | Никакой text-mode конверсии переносов (Windows бы поломала ожидания line numbers) |
| Поддержка `stdc++fs` через CMake version check | g++ 8.3 в среде разработки требует, GCC ≥ 9 ничего не теряет |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `fixtures/scan/include_scanner/simple.cpp` | new |
| `fixtures/scan/include_scanner/comments.cpp` | new |
| `fixtures/scan/include_scanner/string_literal.cpp` | new |
| `fixtures/scan/include_scanner/raw_string.cpp` | new |
| `fixtures/scan/include_scanner/continuation.cpp` | new |
| `fixtures/scan/include_scanner/macro_include.cpp` | new |
| `tests/integration/scan/scanner_fixtures_test.cpp` | new |
| `tests/CMakeLists.txt` | + integration source, + `ARCHCHECK_FIXTURES_DIR`, + `stdc++fs` link для GCC<9 |
