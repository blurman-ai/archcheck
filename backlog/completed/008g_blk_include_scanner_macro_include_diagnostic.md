# [SCAN] Include scanner — macro include как diagnostic

**Дата создания:** 2026-05-26
**Дата старта:** 2026-05-26
**Дата завершения:** 2026-05-26
**Статус:** done
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #008 (dependency_graph_foundation) — закрывает ветку scanner
**Заблокирован:** #008f (include_scanner_first_significant_char)
**Related:** #008 (dependency_graph_foundation)

## Цель

`#include SOMETHING` (macro-based include) не порождает `IncludeDirective`, но
возвращается отдельной diagnostic-записью `macro_include`.

## Сделано

- **2026-05-26** — в `include/archcheck/scan/include_directive.h` добавлены `enum class DiagnosticKind { MacroInclude }`, `struct IncludeScanDiagnostic`, `struct ScanResult`.
- **2026-05-26** — сигнатура `scan_includes` сменилась: `std::vector<IncludeDirective>` → `ScanResult`.
- **2026-05-26** — `try_extract` сильно перестроен: ветка `"`/`<` строит directive, ветка `is_ident_start` собирает identifier и пушит diagnostic, иначе — ничего.
- **2026-05-26** — пустой `#include` без токена → ничего не возвращается (зафиксировано как «игнор»).
- **2026-05-26** — все 25 существующих тестов перешли на helper `extract_directives(...)`, добавлены 4 новых для macro-include. 33/33 зелёные.

## Как работает

`try_extract(line, line_no, ScanResult& out)`:

1. `skip_ws` → ищет `#include` → `skip_ws` снова.
2. Смотрит на `line[i]`:
   - `"` → quote include: ищет closing `"`, кладёт `IncludeDirective{Quote, ...}`.
   - `<` → angle include: ищет closing `>`, кладёт `IncludeDirective{Angle, ...}`.
   - identifier start (`_` или буква) → расширяет до конца identifier (`is_ident_cont`), кладёт `IncludeScanDiagnostic{MacroInclude, "FOO", line_no}`.
   - всё остальное (включая EOL сразу после `#include`) → return без записи.

`ScanResult` — простая пара двух vector-ов. Клиенты разделяют edges и предупреждения по делу: edges идут в граф, diagnostics — в отчёт.

В тестах добавлен helper `extract_directives(s) → vector<IncludeDirective>` для обратной совместимости с уже написанными ассертами на `.size()` / `[0]`.

## Чем управляется

- Без флагов / env. Поведение полностью детерминировано входной строкой.

## С чем связана

- Public-типы: `IncludeDirective`, `IncludeKind`, `IncludeScanDiagnostic`, `DiagnosticKind`, `ScanResult` — все в `archcheck::scan::`.
- Будущий resolver (#012) будет потреблять `ScanResult.directives`; reporter — `ScanResult.diagnostics`.

## Диагностика

- Если ожидаемая директива внезапно стала diagnostic — проверить, не съели ли кавычки/угловые скобки раньше по pipeline (комментарии/raw strings).
- Если diagnostic не возникает на `#include FOO` — проверить `is_ident_start` (нет ли там whitespace, который проглотил начало identifier-а).
- `#include 123` сейчас игнорируется (не identifier, не `"`, не `<`). Это намеренно — токен начинающийся с цифры не легальный preprocessor token и не имеет смысла.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Diagnostic, а не директива для macro include | Edge в графе должен строиться только из resolved project include (см. §5 mini-design в #008) |
| Возврат структуры `ScanResult`, не пары | Будущие категории (token pasting, conditional includes) расширят `DiagnosticKind` без breaking changes |
| Пустой `#include` → ничего | Не директива и не macro-форма; добавление diagnostic-категории «MalformedInclude» — лишнее на этом этапе |
| Helper `extract_directives` в тестах | Меньше шума в существующих ассертах при breaking change сигнатуры |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/scan/include_directive.h` | + `DiagnosticKind`, `IncludeScanDiagnostic`, `ScanResult` |
| `include/archcheck/scan/include_scanner.h` | новая сигнатура `scan_includes` |
| `src/scan/include_scanner.cpp` | `try_extract` поддерживает macro-include branch + `is_ident_*` |
| `tests/unit/scan/include_scanner_test.cpp` | helper `extract_directives`, 4 кейса macro-include |
