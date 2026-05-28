# [TESTS][GRAPH] Покрыть `--diff` сценарием PR с merge-commit-ом

**Дата создания:** 2026-05-27
**Дата старта:** 2026-05-28
**Статус:** wip
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

- [x] Расширить `tests/integration/diff/git_diff_test.cpp` новым TEST_CASE-ом «merge-commit»
- [x] Сборка `A → B (adds a->c) → checkout A → C (adds a->d) → merge B` (через `git tag A`/`git tag C`)
- [x] Assert: `diffRefs("A", "HEAD")` показывает оба ребра `a->c` и `a->d` в addedEdges
- [x] Assert: `diffRefs("C", "HEAD")` показывает только `a->c` (то, что merge принёс из feat-b)
- [x] lizard и clang-format чистые

## Сделано

- Хелпер `buildMergeRepo()` собирает A→B (a->c) и A→C (a->d), мёржит feat-b в feat-c
  с ручным разрешением конфликта на a.h (union обоих includes). HEAD — настоящий
  merge-commit, проверяется `git rev-parse HEAD^2`.
- TEST_CASE `merge-commit HEAD → A..M sees union of edges from both parents`
  `[diff][git][integration][merge]`: 26 assertions, зелёный.
- SHA через `git rev-parse` не понадобился — `git tag A` / `git tag C` на нужных
  коммитах решают вопрос без расширения `runIn`.
- Подтверждено: `git worktree add --detach` (внутри `materializeRef`) корректно
  материализует merge-snapshot, а не один из родителей.

## Следующие шаги

1. `/commit` — `test(diff): cover merge-commit PR scenario in --diff (#022)`
2. Закрыть задачу (перевести в `backlog/completed/`).

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Только тест, без правок продакшен-кода | Гипотеза: `git worktree add` уже корректно обрабатывает merge-commit. Если тест зелёный — поведение зафиксировано регрессией; если красный — становится понятно, что чинить |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `tests/integration/diff/git_diff_test.cpp` | + 1-2 TEST_CASE-а; возможно helper `commitAllAndTag` или `captureSha` |
