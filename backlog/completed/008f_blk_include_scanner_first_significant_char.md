# [SCAN] Include scanner — правило «`#` — первый значимый символ»

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008g (include_scanner_macro_include_diagnostic)
**Заблокирован:** #008e (include_scanner_line_continuation)
**Related:** #008 (dependency_graph_foundation)

## Цель

Распознавать `#include` только когда `#` — первый значимый символ logical line
(допустимы только ведущие whitespace).

## Сделано

- **2026-05-26** — анализ: правило уже неявно соблюдается комбинацией `skip_ws` + `string_view::compare`. Дополнительная проверка не требуется.
- **2026-05-26** — добавлены 4 unit-теста, фиксирующих поведение явно: `int x; #include …`, `; #include …`, два `#include` на одной строке, splice-joined «код-через-`\`-перед-`#include`». 29/29 общий счёт.

## Как работает

В `try_extract`:

```cpp
std::size_t i = skip_ws(line, 0);
if (line.compare(i, kIncludeKeyword.size(), kIncludeKeyword) != 0) return false;
```

`skip_ws` идёт **только** через пробелы/табы. Если на первом не-ws символе стоит не `#`, compare сразу терпит fail. Таким образом:

- `int x; #include "y"` — `skip_ws` останавливается на `i`, compare видит `int` — return false.
- `; #include "y"` — `skip_ws` останавливается на `;`, compare видит `; #i…` — return false.
- `#include "a" #include "b"` — compare находит `#include` сразу, дальше извлекается ровно один токен и `try_extract` возвращается. Второе `#include` на той же строке игнорируется (по логике one-directive-per-line).
- splice-joined `int x; \\\n#include "y"` — после `join_continuations` строка становится `int x; #include "y"`, и работает первый кейс.

## Чем управляется

- Без флагов / env.

## С чем связана

- Тот же `src/scan/include_scanner.cpp`. Кода не правил, только дополнил тесты.

## Диагностика

- Если кто-то когда-нибудь добавит проверки `find("#include")` вместо `compare(i, …)` — правило сломается. Тесты 008f этого не дадут пройти.
- Если в logical line `#` не первый значимый, но scanner отдал директиву — что-то в pipeline скиснет: либо `skip_ws` повредился, либо `join_continuations` вытворил неожиданное (CRLF? unicode whitespace?).

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Не добавлять дополнительной явной проверки — компоновки `skip_ws + compare` уже хватает | YAGNI: дополнительный if без эффекта — мусор |
| Покрыть инвариант тестами вместо комментариев в коде | Тесты документируют поведение надёжнее, чем prose-комменты |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `tests/unit/scan/include_scanner_test.cpp` | 4 кейса first-significant-char |
