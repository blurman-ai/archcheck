# [TESTS][GRAPH] Покрыть `--diff` сценарием PR с merge-commit-ом

**Дата создания:** 2026-05-27
**Дата старта:** —
**Статус:** new
**Модуль:** TESTS, GRAPH
**Приоритет:** minor
**Сложность:** S (1-2 часа)
**Блокирует:** —
**Заблокирован:** —
**Related:** #018 (git_diff_analysis)

## Цель

Добавить integration-тест в `tests/integration/diff/git_diff_test.cpp`, который
проверяет поведение `--diff` на PR-е, в котором HEAD — merge-commit (не
fast-forward), и убедиться что `git worktree add --detach` корректно
материализует оба родителя без сюрпризов.

## Контекст

В плане задачи #018 был пункт «PR с merge-commit-ом» — он не покрыт. На
практике `git worktree add --detach <ref>` работает с merge-commit-ом так
же, как с любым ref-ом (даёт snapshot, а не diff), но без теста легко
получить регрессию когда кто-то начнёт парсить parent-ы вручную.

Сценарий, который хочется проверить:

```
        A (baseline)
       / \
      B   C       (две feature-ветки добавляют разные рёбра)
       \ /
        M (merge)  ← HEAD
```

`--diff A..M` должен видеть **объединение** изменений из B и C (рёбра из
обеих веток), а не только diff M против одного из родителей.

## План выполнения

- [ ] Расширить `tests/integration/diff/git_diff_test.cpp` новым TEST_CASE-ом «merge-commit»
- [ ] Сборка `A → B (adds a->c) → checkout A → C (adds a->d) → merge B`
- [ ] Assert: `diffRefs(repo, "A-sha", "HEAD")` показывает оба ребра `a->c` и `a->d` в addedEdges
- [ ] Дополнительно: `diffRefs(repo, "C-sha", "HEAD")` показывает только `a->c` (изменения из B, которые принёс merge)
- [ ] Проверить что lizard и clang-format остаются чистыми

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Использовать существующий helper `commitAll` + ручные `git checkout -b` / `git merge --no-ff` через `runIn`
2. Сохранить SHA через `git rev-parse HEAD` (нужно расширить `runIn` чтобы возвращал stdout, либо использовать `git tag` для именованных меток)

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Только тест, без правок продакшен-кода | Гипотеза: `git worktree add` уже корректно обрабатывает merge-commit. Если тест зелёный — поведение зафиксировано регрессией; если красный — становится понятно, что чинить |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `tests/integration/diff/git_diff_test.cpp` | + 1-2 TEST_CASE-а; возможно helper `commitAllAndTag` или `captureSha` |
