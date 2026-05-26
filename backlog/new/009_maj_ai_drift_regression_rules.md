# [RULES/DRIFT] AI-oriented drift-regression rules

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** RULES/DRIFT
**Приоритет:** major
**Сложность:** M (2-4 дня на дизайн, реализация отдельными шагами)
**Блокирует:** —
**Заблокирован:** #008 (dependency_graph_foundation)
**Related:** #006 (spec_refactor)

## Цель

Зафиксировать и затем поэтапно реализовать новый класс правил, которые ловят структурный drift относительно baseline, особенно характерный для AI-assisted разработки.

## Контекст

В ходе продуктового обсуждения появилась новая сильная идея: между zero-config hygiene checks и полностью user-declared architecture policy есть промежуточный класс правил — `drift-regression rules`.

Их задача — отвечать не на вопрос “что у вас вообще плохо”, а на вопрос:

> какой локально-удобный, но глобально-размывающий архитектуру шаг был только что сделан в изменении?

Такие правила особенно полезны в эпоху AI-кодинга: агент часто не создаёт откровенный мусор, а делает правдоподобный shortcut, усиливает hub, раздувает public surface или blast radius.

Первичный список правил уже зафиксирован в `docs/architecture-spec.md`:

- `DRIFT.1 no_new_shortcut_edge`
- `DRIFT.2 no_new_cycle_or_cycle_growth`
- `DRIFT.3 public_surface_growth`
- `DRIFT.4 blast_radius_growth`
- `DRIFT.5 hub_reinforcement`
- `DRIFT.6 boundary_bypass_of_existing_entrypoint`
- `DRIFT.7 scattered_new_boundary_access`
- `DRIFT.8 hotspot_inflow`

## План выполнения

- [ ] Разделить `DRIFT.*` правила на классы `zero-config`, `repo-inferred`, `history-aware`
- [ ] Для каждого правила зафиксировать входные данные, алгоритм, expected output и false-positive profile
- [ ] Решить, какие `DRIFT.*` идут в первый прототип, а какие откладываются
- [ ] Зафиксировать severity и режимы `error` / `warning` / `report-only`
- [ ] Продумать baseline contract: что именно хранится и как нормализуются violation locations
- [ ] Подготовить маленькие reference-сценарии для каждого правила
- [ ] Отдельно решить, какие правила можно запускать без git history

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Сузить список до 3-5 правил первого прототипа
2. Привязать их к graph primitives из #008
3. После этого заводить отдельные implementation tasks по группам правил

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Drift rules — отдельное семейство, а не подвид SF/Lakos | У них другая продуктовая цель: ловить structural regression |
| Сначала zero-config и repo-inferred, потом history-aware | Так ниже порог входа и меньше implementation risk |
| Делать ставку на `baseline + diff`, а не на абсолютные smell thresholds | Это лучше бьётся с AI-induced architectural drift |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| docs/architecture-spec.md | список и формализация `DRIFT.*` |
| src/rules/drift/* | будущая реализация правил |
| tests/integration/drift/* | будущие сценарии |
| tests/unit/rules/drift/* | будущие unit tests |

## Fixtures (если правило)

- [ ] `fixtures/drift_shortcut_edge/pass/`
- [ ] `fixtures/drift_shortcut_edge/fail_*/`
- [ ] `fixtures/drift_cycle_growth/pass/`
- [ ] `fixtures/drift_cycle_growth/fail_*/`
- [ ] `fixtures/drift_public_surface/pass/`
- [ ] `fixtures/drift_public_surface/fail_*/`
