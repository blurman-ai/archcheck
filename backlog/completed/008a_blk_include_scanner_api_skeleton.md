# [SCAN] Include scanner — API skeleton

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008b (include_scanner_naive_extraction)
**Заблокирован:** —
**Related:** #008 (dependency_graph_foundation)

## Цель

Зафиксировать публичную форму textual include scanner: типы данных и сигнатуру
функции, без реализации логики разбора.

## Сделано

- **2026-05-26** — создан `include/archcheck/scan/include_directive.h` (`IncludeKind`, `IncludeDirective`).
- **2026-05-26** — создан `include/archcheck/scan/include_scanner.h` (объявление `scan_includes`).
- **2026-05-26** — `src/scan/include_scanner.cpp` со заглушкой, возвращающей пустой вектор.
- **2026-05-26** — `archcheck_core` переведена из INTERFACE в STATIC, добавлен `scan/include_scanner.cpp`.
- **2026-05-26** — smoke-тест `tests/unit/scan/include_scanner_test.cpp`, подключён в `tests/CMakeLists.txt`.
- **2026-05-26** — Debug-сборка зелёная, `ctest` 3/3 (включая новый кейс).

## Как работает

Public API сидит в `include/archcheck/scan/`:
- `IncludeKind` — `Quote` / `Angle`, отличает форму директивы.
- `IncludeDirective` — `{ kind, token, line }`; `token` без обрамляющих кавычек / угловых скобок.
- `scan_includes(std::string_view source) -> std::vector<IncludeDirective>` — единственная точка входа.

Реализация на этом этапе — заглушка: вход проглатывается, возвращается пустой вектор. Семантика разбора наращивается в 008b…008g без изменения сигнатуры.

## Чем управляется

- Никаких флагов / переменных среды на этом этапе.
- Подключение через `archcheck::core` (CMake target).

## С чем связана

- Подсистема `scan/` — первый файл в `src/scan/`.
- Зависимость только на STL.
- Test target `archcheck_tests` теперь линкуется с `archcheck::core` и видит публичные заголовки `scan/`.

## Диагностика

- Если линкер не находит `scan_includes` — проверить, что `src/scan/include_scanner.cpp` в списке source files `archcheck_core` в `src/CMakeLists.txt`.
- Если тест не находит хедер — проверить, что путь репо-корень/include добавлен в `target_include_directories(archcheck_core PUBLIC …)`.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Scanner возвращает только «сырые» директивы | Resolution — отдельная стадия pipeline, см. §4 mini-design в #008 |
| `int line` (не `line+column`) | column в v0.1 не используется ни одним правилом, добавим позже при необходимости |
| `archcheck_core` STATIC, а не INTERFACE | Появилась реальная реализация, INTERFACE-target её не вмещает |
| `target_include_directories` через `PUBLIC` | Тесты должны видеть публичные заголовки без явного дублирования путей |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/scan/include_directive.h` | new |
| `include/archcheck/scan/include_scanner.h` | new |
| `src/scan/include_scanner.cpp` | new (заглушка) |
| `src/CMakeLists.txt` | INTERFACE → STATIC, добавлен `scan/include_scanner.cpp` |
| `tests/unit/scan/include_scanner_test.cpp` | new (smoke) |
| `tests/CMakeLists.txt` | подключён новый test source |
