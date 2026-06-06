# [RULES][DRIFT] DRIFT.2 (cycle growth) → default blocking gate

**Дата создания:** 2026-06-06
**Дата старта:** 2026-06-06
**Дата завершения:** 2026-06-06
**Статус:** completed
**Модуль:** RULES/DRIFT
**Приоритет:** major
**Сложность:** M
**Блокирует:** —
**Заблокирован:** —
**Related:** #009 (DRIFT-правила), #082 (alignment umbrella), [docs/research/drift_signal_validation.md](../../docs/research/drift_signal_validation.md) (corpus-доказательство)

## Цель

Сделать DRIFT.2 (рост/появление циклов зависимостей) **default blocking gate**
(exit `1`), а не просто репорт.

## Обоснование (данными)

Corpus-валидация ([drift_signal_validation.md](../../docs/research/drift_signal_validation.md)):
DRIFT.2 срабатывает на **72 из 135 092 коммитов = 0.05%** по 310 репозиториям.
Это исключительно низкий false-alarm rate, а новый цикл — объективная архитектурная
регрессия (Lakos physical design). Сочетание «редко + объективно» = созрело для
жёсткого гейта. Обычные линтеры этот класс не ловят.

## Что нужно

1. Подтвердить текущее поведение DRIFT.2 в `--drift-baseline` (сейчас репортит; какой
   exit code?).
2. Сделать DRIFT.2 blocking по умолчанию в drift-режиме (exit `1` при росте циклов).
3. Проверить, что pre-existing циклы (в baseline) НЕ ломают — гейтим только **новые/
   выросшие** циклы, не legacy.
4. Fixtures: `fixtures/drift_cycle_growth/` уже есть — дополнить pass/fail под gate-семантику.
5. Документировать exit-контракт во всех слоях (help/docs/CHANGELOG).

## Acceptance criteria

- [x] DRIFT.2 ломает сборку (exit 1) при появлении/росте цикла в drift-режиме.
- [x] Pre-existing циклы из baseline не вызывают ложного fail.
- [x] Fixtures pass/fail покрывают gate-поведение (`drift_cycle_growth` + live-верификация exit).
- [x] Контракт отражён в help/docs/CHANGELOG.

## Как сделано (2026-06-06)

**Находка при старте:** `--drift-baseline` экзитил `all.empty() ? 0 : 1`, то есть падал
на **любом** нарушении, включая legacy SF/Lakos. На LibreSprite это exit 1 из-за 259
legacy, а не из-за дрейфа → как gate бесполезен. Это и было настоящей проблемой.

**Фикс** (`src/main.cpp` `applyBaselineAndReport`): в drift-режиме (`baseline.driftFile`)
exit определяется **только** числом DRIFT.1/DRIFT.2 (регрессии). Legacy intrinsic
(SF.*/Lakos.*) и advisory DRIFT.3 репортятся, но не гейтят. Печатается строка
`drift gate: N gating regression(s) ...`.

**Политика (обоснована corpus-валидацией):**
- **gating:** DRIFT.1 (new shortcut), DRIFT.2 (new/grown cycle) — точные, редкие, объективные.
- **advisory:** DRIFT.3 (шумит на restructure — см. eyeball в `drift_signal_validation.md`),
  pre-existing intrinsic (это долг, не регрессия диффа).

**Live-верификация** (LibreSprite):
- before `60eed0f` → after `276fdbd` (DRIFT.1=1): **exit 1** ✓
- baseline == текущее состояние (259 legacy, 0 дрейфа): **exit 0** ✓ (раньше было 1)

Help + CHANGELOG обновлены. Gate: clang-format/cppcheck/lizard чисто, 347 тестов, coverage PASS.

## Заметки

- Не трогать DRIFT.1 семантику в этой задаче (отдельный сигнал).
- Cross-area raw НЕ гейтить (см. валидацию — ~50% FP).
