# [SCAN] Include scanner — игнор строковых литералов

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008e (include_scanner_line_continuation)
**Заблокирован:** #008c (include_scanner_skip_comments)
**Related:** #008 (dependency_graph_foundation)

## Цель

`#include` внутри `"…"`, `'…'` и raw string literal `R"delim(…)delim"` не должен
распознаваться как директива.

## Сделано

- **2026-05-26** — добавлен `consume_raw_string`, поддерживающий пустой и custom delimiter.
- **2026-05-26** — `preprocess` отлавливает `"` сразу после `R` и переключается на raw-string consume.
- **2026-05-26** — 5 unit-тестов: ordinary string, char literal, single-line `R"(…)"`, custom delimiter `R"d(…)d"`, multi-line raw string. 19/19 общий счёт зелёный.

## Как работает

В `preprocess` добавлена ветка: если текущий символ — `"`, и предыдущий символ был `R`, это начало raw string. `consume_raw_string`:

1. Читает delimiter — всё между `"` и `(`.
2. Заменяет содержимое raw string пробелами (newlines сохраняются → line numbers корректны).
3. Закрывающая последовательность собирается как `")" + delim + "\""` и ищется через `string_view::compare`.

Ordinary string `"…"` и char literal `'…'` **не стрипаются** — и это намеренно. Анализ:

- `#include` ловится только когда `#` стоит первым значимым символом строки.
- В `const char* s = "#include \"x\"";` символ `#` расположен после `"`, не первым на строке → не false-positive.
- Ordinary string не может содержать «свежей строки начинающейся с #include», потому что обычные строки не переносятся через `\n` (только через line continuation — это 008e).

То есть в этой подзадаче актуально только raw-string consume; для `"…"` и `'…'` достаточно того, что в текущей форме scanner физически не может на них сработать.

## Чем управляется

- Без флагов / env.

## С чем связана

- Та же реализация в `src/scan/include_scanner.cpp`.
- 008e (line continuation) добавит дополнительный нюанс: continuation внутри обычной строки превратит её в multi-line, и тогда ordinary string snippet с `#include` после `\` может выйти на следующую строку. Это решается в 008e через pre-pass continuation.

## Диагностика

- Если `#include` ловится внутри raw string — проверить, что delimiter в `R"d(…)d"` действительно совпадает, и что `)d"` не разорвано.
- Если raw string «не съел» текст — проверить, что после `R` сразу стоит `"` (без пробелов).
- На текущий момент `'1'` (digit-separator префикс на самом деле редок и в текущем препроцессе игнорируется как «не стрипается») — записать как edge case для будущих v0.x, если кому-то понадобится. Сейчас сценариев таких в C++ коде, который ломал бы scanner, не встретилось.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Полная поддержка raw strings | В тестах archcheck это будет встречаться часто, в реальном коде тоже (ABI-доки, JSON-embedded и т.п.) |
| Ordinary string / char literal не стрипаются | Не могут породить false-positive в текущей грамматике scanner-а; экономия LOC и нет риска сломать `#include "x"` |
| `R` без проверки prefix (`u8R`, `LR`, …) | `R` + `"` — достаточный сигнал; identifier-suffix кейсы крайне редки и легко решаются позже |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `src/scan/include_scanner.cpp` | `consume_raw_string` + dispatch в `preprocess` |
| `tests/unit/scan/include_scanner_test.cpp` | 5 новых кейсов для strings / chars / raw |
