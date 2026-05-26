# [SCAN] Include scanner — наивная line-based экстракция

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008c (include_scanner_skip_comments)
**Заблокирован:** #008a (include_scanner_api_skeleton)
**Related:** #008 (dependency_graph_foundation)

## Цель

Распознавать простейшие `#include "x"` и `#include <x>` в исходнике, по одной
директиве на строку, без учёта комментариев, строковых литералов и line
continuation.

## Сделано

- **2026-05-26** — line iteration через скан `\n` с подсчётом line_no.
- **2026-05-26** — `skip_ws` для leading whitespace.
- **2026-05-26** — детектор префикса `#include`, выбор delimiter `"` / `<`, поиск closing delimiter.
- **2026-05-26** — 7 unit-тестов для happy path: quote, angle, multi, без include, leading spaces/tabs, no trailing newline. Все зелёные (9/9 общий счёт).

## Как работает

`scan_includes(source)` идёт по `source` посимвольно, нарезая на logical lines по `\n`. Для каждой строки вызывается `try_extract(line, line_no, out)`:

1. `skip_ws` — пропустить leading пробелы и табы.
2. сравнить префикс с `#include`; если нет — выйти.
3. снова `skip_ws` после ключевого слова.
4. посмотреть на первый символ: `"` → `Quote`, `<` → `Angle`, иначе — выйти.
5. найти closing delimiter (`"` или `>`).
6. собрать `IncludeDirective{kind, token, line_no}` и положить в out.

Все этапы без regex, без аллокаций кроме итогового `std::string` для токена.

## Чем управляется

- Без флагов / env.
- Поведение полностью детерминировано входной строкой.

## С чем связана

- Тот же translation unit `src/scan/include_scanner.cpp`, расширяется в 008c…g.
- Public API не изменился — все клиенты 008a продолжают работать.

## Диагностика

- Если директива не находится, проверить:
  - есть ли что-то нестандартное между `#` и `include` (не поддерживается, добавится позже только если потребуется);
  - закрыт ли delimiter на той же строке (multi-line кейс — задача 008e).
- Юнит-тесты в `tests/unit/scan/include_scanner_test.cpp` дублируют ожидания и удобны как живой контракт.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Никакой регулярки | Зависимостей минимум, разбор тривиальный |
| Не валидируем содержимое токена | Это работа resolver-а на следующих этапах |
| Closing delimiter ищется через `find`, а не покадровый цикл | Короче и понятнее, перфоманс не критичен |
| Helper `try_extract` возвращает `bool` | Готовая точка для будущих ветвлений (macro-include в 008g) |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `src/scan/include_scanner.cpp` | реализация happy path (line iteration + `try_extract` + `skip_ws`) |
| `tests/unit/scan/include_scanner_test.cpp` | 6 новых кейсов поверх smoke |
