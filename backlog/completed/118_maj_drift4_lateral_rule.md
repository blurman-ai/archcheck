# [RULES][GRAPH] DRIFT.4 lateral_module_dependency — продуктовое правило бокового дрейфа

**Дата создания:** 2026-06-12
**Дата старта:** 2026-06-12
**Дата завершения:** 2026-06-12
**Статус:** done
**Модуль:** RULES][GRAPH
**Приоритет:** major
**Сложность:** medium
**Блокирует:** —
**Заблокирован:** —
**Related:** #111/#115/#117 (валидация критерия, completed), #087 (DRIFT.3 — образец area-логики), #057 (Lakos fan-out checks)

> Все file:line-якоря сняты 2026-06-12 — при старте перепроверить grep'ом.

## Цель

Перенести валидированный критерий бокового межмодульного дрейфа
([docs/research/lateral_module_drift_criterion.md](../../docs/research/lateral_module_drift_criterion.md),
прогон: [lateral_module_drift_corpus_run.md](../../docs/research/lateral_module_drift_corpus_run.md))
в продуктовое правило `DRIFT.4 lateral_module_dependency`: CYCLE-грейд гейтит,
SDP/NEW — advisory.

## Контекст

Критерий провалидирован на 479 репах: 495 событий из 21 736 сырых (подавление 108×),
eyeball TP 85%, CYCLE-precision 92%. Агентский разрез — null-result (не для правила,
для науки). В CI-режиме два костыля корпусного прогона НЕ нужны: baseline берётся из
сохранённого графа (а не реконструкцией), персистентность не нужна (гейт стоит до
вливания).

**Решение по нумерации (принято, см. «Ключевые решения»):** правило занимает **DRIFT.4**;
запланированная волна в спеке (`DRIFT.4 public_surface_growth` … `DRIFT.9 hotspot_inflow`,
[architecture-spec.md:379-384](../../docs/architecture-spec.md)) перенумеровывается в
DRIFT.5–DRIFT.10. Прецедент уже есть: спека делала так же, когда DRIFT.3 занял
bidirectional coupling (#087) — см. changelog спеки, строка ~6.

## Архитектура решения (для исполнителя — пошагово)

### Шаг 1. Класс правила (2 новых файла)

По образцу DRIFT.3 ([drift_bidirectional_coupling.h:23-33](../../include/archcheck/rules/drift_bidirectional_coupling.h#L23-L33)):

- `include/archcheck/rules/drift_lateral_module_dependency.h`:
  ```cpp
  class DriftLateralModuleDependency final : public IRule
  {
  public:
    explicit DriftLateralModuleDependency(graph::DependencyGraph baseline);
    ViolationList check(const graph::DependencyGraph &graph,
                        const std::function<std::string(std::string_view)> &readFile) const override;
    std::string_view id() const override { return "DRIFT.4"; }
  private:
    graph::DependencyGraph baseline_;
  };
  ```
- `src/rules/drift_lateral_module_dependency.cpp` — логика (Шаг 3).

Интерфейс `IRule`: [i_rule.h:13-22](../../include/archcheck/rules/i_rule.h#L13-L22).
`Violation{ruleId, file, line, message}`: [violation.h:9-15](../../include/archcheck/rules/violation.h#L9-L15);
для граф-уровня `line = 0`.

### Шаг 2. Грейды через суффикс ruleId

Гейтинг в check-режиме матчит строку ruleId
([check_command.cpp:100-109](../../src/cli/check_command.cpp#L100-L109)):
```cpp
const auto gating = std::count_if(..., [](const auto &v)
    { return v.ruleId == "DRIFT.1" || v.ruleId == "DRIFT.2"; });
```
Правило эмитит Violation с **тремя разными ruleId**: `DRIFT.4.CYCLE`, `DRIFT.4.SDP`,
`DRIFT.4.NEW` (метод `id()` возвращает `"DRIFT.4"` — это идентификатор правила,
ruleId находки несёт грейд). В условие гейта добавить `|| v.ruleId == "DRIFT.4.CYCLE"`
и обновить текст сообщения `"(DRIFT.1/DRIFT.2)"` → `"(DRIFT.1/DRIFT.2/DRIFT.4.CYCLE)"`.

### Шаг 3. Алгоритм check() — точный перенос критерия

Модуль файла: **переиспользовать `areaOf` из DRIFT.3**
([drift_bidirectional_coupling.cpp:67-89](../../src/rules/drift_bidirectional_coupling.cpp#L67-L89) —
скип wrapper-dirs `src/include/lib`, noise-dirs `build/test/vendor`, первый значащий
компонент; noise-константы там же, строки 23-44). Вынести `areaOf` в общий хидер
(например, `include/archcheck/rules/area_of.h`) — НЕ копипастить (правило дедупа).
Прим.: research-прототип использовал auto-depth (≥3 сиблингов) — это уточнение
отложено, v1 сознательно берёт shipped-логику area.

На **baseline-графе** посчитать один раз (всё на прямых рёбрах — НЕ транзитивных,
см. критерий §2 «предостережение»):

```
modEdges  : set<pair<Area,Area>>      — все межмодульные пары baseline
FID[B]    = |{A != B : (A,B) in modEdges}|
FOD[B]    = |{C != B : (B,C) in modEdges}|
I[M]      = FOD-рёбра-наружу / (входящие + исходящие)   // Martin, на модульном уровне
maxFID    = max по модулям
medianFOD = медиана по модулям
shared(B) = FID[B] >= 0.5 * maxFID  &&  FOD[B] <= medianFOD
```

Для каждого ребра из `addedEdges(baseline_, graph)`
([graph/diff.h:21](../../include/archcheck/graph/diff.h#L21)):

1. `A = areaOf(from), B = areaOf(to)`; скип если A или B пустые, или A == B;
2. скип если `B` отсутствует в baseline как модуль (рождение модуля — не дрейф;
   CYCLE для него невозможен по построению, метрики не определены);
3. скип если `shared(B)`;
4. скип если пара `(A,B)` уже есть в `modEdges` (не первая связь);
5. **mass-touch guard**: если `addedEdges` всего > 150 — правило молчит целиком
   (константа `kMassTouchEdges = 150` с комментарием-ссылкой на критерий §3);
6. грейд:
   - `(B,A) ∈ modEdges` → **DRIFT.4.CYCLE** (back-edge точный — baseline-граф
     настоящий, фантомов как в #117 здесь не бывает по построению);
   - `I[B] > I[A] + 0.10` → **DRIFT.4.SDP** (`kSdpDelta = 0.10`);
   - иначе → **DRIFT.4.NEW**.
7. Одно событие на пару (A,B) за прогон — дедуп; message:
   `"module 'A' -> 'B': first lateral dependency (via <from> -> <to>)"`,
   `file = from`, `line = 0`.

Константы (`kSharedFidRatio = 0.5`, `kSdpDelta = 0.10`, `kMassTouchEdges = 150`) —
именованные в .cpp с одной строкой комментария и ссылкой на research-док. В
`config::Thresholds` НЕ выносить (YAGNI; конфиг-плюмбинг — когда попросит юзер).

### Шаг 4. Регистрация

[rule_set.cpp:26-33](../../src/rules/rule_set.cpp#L26-L33), одна строка в
`makeDriftRuleSet` (baseline копируется в каждое правило, последнее — move; вставить
ПЕРЕД move-строкой):
```cpp
rules.push_back(std::make_unique<DriftLateralModuleDependency>(baseline));
```

### Шаг 5. Fixtures (обязательны — MVP.md)

Каталог `fixtures/drift_lateral/`, формат как у `drift_shortcut_edge/`
(`baseline.graph.yml` + заголовки; пример yaml — fixtures/drift_shortcut_edge/pass/):

| Фикстура | Baseline | Текущее состояние | Ожидание |
|---|---|---|---|
| `pass_shared_target/` | a/,b/,c/ все → utils/ (FID(utils)=3=max, FOD=0) | d/x.h добавил `#include "utils/u.h"` | 0 находок (B shared) |
| `pass_existing_pair/` | ребро a/x.h → b/y.h уже есть | a/z.h → b/y.h (вторая связь пары) | 0 находок |
| `fail_lateral_new/` | a/, b/ независимы, оба одного уровня | a/x.h → b/y.h | DRIFT.4.NEW |
| `fail_lateral_cycle/` | b/y.h → a/w.h уже есть | a/x.h → b/z.h | DRIFT.4.CYCLE |
| `fail_lateral_sdp/` | I(b) заметно > I(a): b зависит от 3 модулей, a — листовой со входящими | a/x.h → b/y.h | DRIFT.4.SDP |

В baseline-yml каждой фикстуры держать ≥3 модулей, чтобы maxFID/медиана не вырождались.

### Шаг 6. Тесты

- Unit `tests/unit/rules/drift_lateral_module_dependency_test.cpp` — синтетические
  графы (образец: drift_no_cycle_growth_test.cpp): shared-порог (ровно 0.5·max — граничный),
  медиана FOD, грейс нового модуля (п.2), mass-touch, дедуп пары, все три грейда.
- Интеграционный: `tests/integration/rules/drift_fixtures_test.cpp` —
  `run_drift_check()` (строки 63-81) уже зовёт `makeDriftRuleSet`, добавить кейсы
  на 5 фикстур.

### Шаг 7. Документация и нумерация

- `docs/architecture-spec.md`: волну DRIFT.4–9 (строки ~379-384) перенумеровать в
  DRIFT.5–10; в changelog спеки добавить строку «ID DRIFT.4 занят шипнутым
  lateral_module_dependency (#118), волна стала DRIFT.5–DRIFT.10» (по образцу
  существующей записи про DRIFT.3). Внимание: #057 ссылается на «DRIFT.4
  (blast_radius_growth)» — поправить на новый номер.
- `CHANGELOG.md` (Keep a Changelog, Unreleased/Added).
- `--help`: если там перечислены DRIFT-правила — добавить строку.
- В сообщении гейта drift-режима (check_command.cpp:104-106) упомянуть DRIFT.4.CYCLE.

### Шаг 8. Самопроверка

- `cmake --build build/debug && cd build/debug && ctest --output-on-failure`
- lizard-гейт: `lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/`
- Dogfood: `./build/debug/src/archcheck` из корня — `src/ include/ tests/` дают 0 нарушений
- Коммиты по ≤50 строк: (1) area_of вынос, (2) правило+фабрика, (3) гейтинг,
  (4) фикстуры+тесты, (5) доки/спека.

## План выполнения

- [ ] Вынести `areaOf` в общий хидер (рефактор DRIFT.3, поведение бит-в-бит, тесты зелёные)
- [ ] Класс DriftLateralModuleDependency + модульные метрики на baseline
- [ ] Грейды + гейтинг DRIFT.4.CYCLE в check_command
- [ ] 5 фикстур + unit + integration тесты
- [ ] Спека (перенумерация волны) + CHANGELOG + #057 поправка
- [ ] Самопроверка (build+tests, lizard, dogfood)

## Сделано

- `include/archcheck/rules/area_of.h` — вынос areaOf из DRIFT.3
- `src/rules/drift_bidirectional_coupling.cpp` — переход на shared area_of.h
- `include/archcheck/rules/drift_lateral_module_dependency.h` — класс правила
- `src/rules/drift_lateral_module_dependency.cpp` — check(), computeMetrics(), isShared() (strict `>` fix для int-truncation при малых графах), gradeEdge(), buildMsg()
- `src/rules/rule_set.cpp` — регистрация в makeDriftRuleSet
- `src/cli/check_command.cpp` — DRIFT.4.CYCLE в gating
- `src/CMakeLists.txt` — +drift_lateral_module_dependency.cpp
- `fixtures/drift_lateral/` — 5 фикстур (pass×2, fail_new/cycle/sdp)
- `tests/unit/rules/drift_lateral_module_dependency_test.cpp` — 6 unit-тестов
- `tests/integration/rules/drift_fixtures_test.cpp` — +5 integration-кейсов
- `docs/architecture-spec.md` v2.3 — DRIFT.4–9 → DRIFT.5–10, волна перенумерована
- `CHANGELOG.md` — Unreleased/Added
- `backlog/new/057_*.md` — поправка DRIFT.4→DRIFT.6 для blast_radius
- Самопроверка: 519/519 тестов, lizard ok, dogfood 0 нарушений

## В работе

- (пусто)

## Следующие шаги

1. Начать с выноса areaOf — это отдельный безопасный коммит.

## Ключевые решения

| Решение | Причина |
|---------|---------|
| ID = DRIFT.4, волна спеки сдвигается в DRIFT.5–10 | Прецедент: так уже делали для DRIFT.3 (#087); research-доки и так зовут правило DRIFT.4 |
| Грейд в ruleId-суффиксе (DRIFT.4.CYCLE/SDP/NEW) | Гейтинг в check_command матчит ruleId строкой — расширение без новых механизмов |
| areaOf из DRIFT.3, не auto-depth из прототипа | Boring tech: shipped и тестированная логика; auto-depth — отдельное улучшение по запросу |
| Константы в .cpp, не в config::Thresholds | YAGNI; пороги валидированы корпусом, юзер-конфиг — когда попросят |
| Только drift-режим (makeDriftRuleSet), diff-режим не трогаем | RegressionReport — отдельный механизм без IRule; lateral-секция там — отдельная задача при надобности |
| Персистентность/грейс-период не переносится | В CI baseline=точный граф, гейт до вливания — корпусные костыли не нужны (research-док §7) |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/rules/drift_lateral_module_dependency.h` | Новый |
| `src/rules/drift_lateral_module_dependency.cpp` | Новый |
| `include/archcheck/rules/area_of.h` | Вынос areaOf из DRIFT.3 |
| `src/rules/drift_bidirectional_coupling.cpp` | Переход на общий areaOf |
| `src/rules/rule_set.cpp` | +1 строка фабрики |
| `src/cli/check_command.cpp` | Гейтинг DRIFT.4.CYCLE |
| `fixtures/drift_lateral/*` | 5 фикстур |
| `tests/unit/rules/drift_lateral_module_dependency_test.cpp` | Новый |
| `tests/integration/rules/drift_fixtures_test.cpp` | +5 кейсов |
| `docs/architecture-spec.md`, `CHANGELOG.md`, `backlog/new/057_*.md` | Нумерация/доки |

## Fixtures

- [ ] `fixtures/drift_lateral/pass_shared_target/`
- [ ] `fixtures/drift_lateral/pass_existing_pair/`
- [ ] `fixtures/drift_lateral/fail_lateral_new/`
- [ ] `fixtures/drift_lateral/fail_lateral_cycle/`
- [ ] `fixtures/drift_lateral/fail_lateral_sdp/`

## Как работает (итог)

**Принцип.** `DriftLateralModuleDependency::check()` берёт `addedEdges(baseline_, graph)`
(новые рёбра по сравнению с graph-baseline), сводит каждое ребро к паре модулей через
`areaOf()`, и эмитит градуированное нарушение для первой боковой связи между равноправными
модулями. На baseline считаются модульные метрики (FID/FOD) один раз: `computeMetrics()`.

**Грейды (суффикс ruleId).** `gradeEdge()`:
- `DRIFT.4.CYCLE` — в baseline есть обратное ребро (B→A): новая связь A→B замыкает
  модульный цикл. **Гейтит** (выход ≠ 0 в `--drift`).
- `DRIFT.4.SDP` — нарушение Stable Dependencies Мартина: I(B) > I(A) + 0.10. Advisory.
- `DRIFT.4.NEW` — первая боковая пара без цикла/SDP. Advisory.

**Фильтры (молчание).** Mass-touch guard: >150 добавленных рёбер → реорг/vendor, правило
молчит целиком. Shared-слой (`isShared`: FID > 0.5·maxFID И FOD ≤ medianFOD) — инфраструктура,
не дрейф. Рождение модуля B (нет в baseMetrics) — не дрейф. Пара уже в baseline — не первая.

**Тонкость (баг, который ловили).** `isShared` использует **строгий** `>`, не `>=`:
`static_cast<int>(0.5·maxFID)` усекает (0.5·3=1.5→1), и при `>=` модуль с FID=1 в малом
графе ложно считался shared, глуша CYCLE/NEW. Строгий `>` это чинит.

## Чем управляется

Константы в `src/rules/drift_lateral_module_dependency.cpp` (НЕ в config — YAGNI):
`kSharedFidRatio = 0.50`, `kSdpDelta = 0.10`, `kMassTouchEdges = 150`. Пороги
валидированы корпусом (#111/#115/#117). Регистрация — `makeDriftRuleSet` в `rule_set.cpp`
(только drift-режим). Гейтинг CYCLE — строковый матч ruleId в `check_command.cpp`.

## С чем связана

Источник критерия — `docs/research/lateral_module_drift_criterion.md` + прогон
`lateral_module_drift_corpus_run.md`. `areaOf` общий с DRIFT.3 (#087) через
`include/archcheck/rules/area_of.h`. Python-двойник правила (корпусный сканер) —
`experiments/ai_repo_run/lateral_drift_scan.py` (#119/#121). Спека: волна DRIFT.5–10.

## Диагностика

Тесты: `tests/unit/rules/drift_lateral_module_dependency_test.cpp` (6),
`tests/integration/rules/drift_fixtures_test.cpp` (5 фикстур `fixtures/drift_lateral/`).
Запуск точечно: `ctest -R "DRIFT.4|drift.*lateral"`. Сообщение находки содержит
`(via <from> -> <to>)` — конкретное file-ребро, родившее модульную пару.
