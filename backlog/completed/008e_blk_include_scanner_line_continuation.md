# [SCAN] Include scanner — logical-line continuation

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008f (include_scanner_first_significant_char)
**Заблокирован:** #008d (include_scanner_skip_string_literals)
**Related:** #008 (dependency_graph_foundation)

## Цель

Склеивать строки, заканчивающиеся на `\` + EOL, в одну logical line ещё до
лексического анализа.

## Сделано

- **2026-05-26** — введён `struct Joined { text, line_of_offset }` и pre-pass `join_continuations`, удаляющий `\\\n` и сохраняющий мэппинг offset → физическая строка.
- **2026-05-26** — `scan_includes` теперь идёт `source → join_continuations → preprocess → line iteration`, line_no берётся через `line_at(joined, line_start)`.
- **2026-05-26** — 6 unit-тестов: splice внутри `#include`, между keyword и токеном, внутри токена, несколько подряд, `\` без `\n` (не continuation), сохранение физической нумерации после splice. 25/25 общий счёт.

## Как работает

`join_continuations(source) -> Joined`:
- идёт по байтам;
- если `source[i]=='\\'` и `source[i+1]=='\n'` — оба символа выкидываются, физическая строка увеличивается, output не растёт;
- иначе символ копируется в `Joined::text`, его физическая строка кладётся в `Joined::line_of_offset[offset]`.

`Joined::line_of_offset` — `vector<int>`, один элемент на байт `text`. Это даёт O(1) lookup физической строки по offset, который позже используется в `line_at`.

`scan_includes`:
1. `joined = join_continuations(source)`.
2. `cleaned = preprocess(joined.text)` (комментарии + raw strings, длина та же).
3. Идём по `cleaned` через `\n` — для каждого segment вызываем `try_extract(segment, line_at(joined, line_start))`. Так директива сообщает физическую строку первого символа logical line.

## Чем управляется

- Без флагов / env.

## С чем связана

- Тот же `src/scan/include_scanner.cpp`.
- Public API не изменился — клиенты 008a продолжают работать.

## Диагностика

- Если directive рапортует «не ту» строку — проверить, не было ли splice выше; `line_at` берёт физическую строку первого символа logical line (что соответствует ожиданиям пользователя).
- CRLF (`\\\r\n`) пока не поддерживается — известное ограничение, нормализация будет в #011 (file reader).
- Splice применяется и внутри raw strings (что технически расходится с C++ phase-2 special rule), но для include detection это безвредно: содержимое raw string всё равно проглатывается `consume_raw_string`. Документирую как известное упрощение.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Отдельная pre-pass `join_continuations`, не интегрировано в `preprocess` | Линейно и читаемо; mapping физической строки удобно хранить рядом с text |
| `line_of_offset` как `vector<int>` | Память на 4 байта на байт исходника — приемлемо для v0.1; альтернативы (RLE, бинарный поиск по диапазонам) преждевременны |
| Splice применяется и внутри raw strings | Упрощение; не порождает false-positive в текущей грамматике |
| Без поддержки `\\\r\n` | Сейчас scanner всегда получает уже нормализованный LF-источник; CRLF — задача file reader |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `src/scan/include_scanner.cpp` | `Joined` / `join_continuations` / `line_at`, новый pipeline `scan_includes` |
| `tests/unit/scan/include_scanner_test.cpp` | 6 кейсов continuation |
