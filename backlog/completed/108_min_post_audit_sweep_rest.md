# [CLEANUP][QUALITY] Post-audit sweep: секции 2–4 (отложено из #104)

**Дата создания:** 2026-06-11
**Дата старта:** 2026-06-19
**Дата завершения:** 2026-06-19
**Статус:** done
**Модуль:** SCAN / QUALITY
**Приоритет:** minor
**Сложность:** quick_win (по факту)
**Related:** #104 (секция 1 закрыта, be56245), #107, #105 (security hardening — затронуто)

Отложенный остаток #104 (формулировки и grep-доказательства — в
`backlog/completed/104_min_post_audit_cleanup_sweep.md`).

## Итог разбора (2026-06-19) — описания #104 проверены по коду перед работой

Большинство пунктов оказались **устаревшими** (код ушёл вперёд). Skeptic-правило
сработало: не верить агрегату #104, сверять каждый случай.

1. **Секция 2 — дубли >5 строк** — разобрано:
   - **fork/exec** (`git_exec.cpp` ↔ `git_object_file_source.cpp`): skeleton fork/pipe/
     dup2/lifecycle **по-настоящему разный** (one-shot stdout+stderr vs долгоживущий
     bidirectional stdin+stdout для `cat-file --batch`). Слияние = дырявая абстракция,
     НЕ сделано (extractability-тест провален). НО внутри нашёлся **реальный** дубль:
     hardening-флаги `-c core.hooksPath=… -c core.fsmonitor= -c core.pager=cat`
     идентичны в обоих и security-значимы (#105) — расхождение = дыра. ✅ **Вынесены
     в `include/archcheck/git/git_hardening.h` (single source of truth)**, оба сайта
     итерируют общий массив. fork/pipe-плумбинг оставлен раздельным.
   - **toLowerCopy**: ❌ **уже не дубль**. В `duplication_scanner.cpp` `tolower` больше
     нет; осталось одно вхождение в `area_of.h:52` + отдельный общий
     `file_classification.h::toLowerAscii` (5 мест). Дублирования нет → новый
     string-util заголовок НЕ нужен (был бы абстракцией без надобности).
   - **plainJaccard**: ❌ снят ещё 2026-06-13 (не дублируется).
   - JSON-сериализация: ❌ уже дедуплицирована в #055 (json_escape). Vendor/test-фильтр
     унифицирован в #129 (`AuthoredScope`). Оба пункта закрыты другими задачами.
2. **Секция 3 — гигиена duplication_scanner.cpp** — ✅ сделано 2026-06-13.
3. **Секция 4 — lizard-долг** — ✅ **долга нет**. Точный CI-гейт
   (`lizard --CCN 15 -T nloc=30 --arguments 5 src/ include/`, ci.yml:154) **проходит
   чисто**. Шум `--length 30` — физическая длина, которую CI намеренно НЕ гейтит
   (ci.yml: «иначе плотно документированный код штрафуется за комментарии»). Дробить
   NLOC-30 функции = churn против решения проекта. НЕ делаю.

**Проверка:** 540/540 тестов (включая 47 git), dogfood `src/ include/ tests/` = 0,
lizard CI-гейт чист, clang-format чист.

## Изменённые файлы

- `include/archcheck/git/git_hardening.h` — новый: общий `kGitHardeningArgs` (commit `50871c2`)
- `src/git/git_exec.cpp` — использует общий массив, локальный удалён
- `src/git/git_object_file_source.cpp` — argv итерирует общий массив

## Итог

**Статус:** completed. Реальной правкой оказался только вынос hardening-флагов в
single source of truth (security-смысл). Остальное (toLowerCopy, JSON, fork/exec
skeleton, lizard §4) — устарело или ложный дубль; описания #104 проверены по коду
и сняты с доказательствами. Skeptic-правило сработало: не делать по описанию.
