# Дешёвые граф-детекторы по корпусу: что нашли (можно пощупать)

> Цель прогона: выжать максимум **дешёвых** граф-проверок, каждая из которых
> указывает на конкретный файл/цикл/пару — чтобы находку можно было открыть и
> проверить руками. Все детекторы работают **без правок прода**: граф выгружается
> уже существующим `archcheck --save-graph-baseline`, анализ — отдельным
> Python-скриптом по выгруженному списку рёбер.

**Дата:** 2026-05-31. **Корпус:** `_aidev_run/` (69 C++ AI-реп, HEAD-снимок).
**Инструменты:** [graph_probe.py](graph_probe.py) (детекторы) + [graph_probe_all.sh](graph_probe_all.sh) (прогон).
**Сырьё:** `runs_history/probe/<repo>.txt` (находки по репам) + `summary.tsv` (сводка).

> Все числа — прямой вывод инструментов. Каждое утверждение про конкретный файл
> проверено `grep`/`sed` на исходнике (команды приведены). Где проверка
> опровергла находку — это записано как артефакт, а не замолчано.

## Детекторы (10 штук, все почти бесплатны)

| Маркер | Что находит | Стоимость |
|---|---|---|
| `[CYCLE]` | члены крупнейшего цикла (SCC) — какие файлы в клубке | Tarjan по выгрузке |
| `[MUTUAL]` | взаимные include-пары (A↔B), классиф. по расширениям | проход по рёбрам |
| `[SELF]` | файл инклюдит сам себя (баг резолвера исправлен, см. §3) | self-loop рёбра |
| `[BACKDOOR]` | заголовок инклюдит `.cpp/.cc` (тянет TU) | фильтр рёбер по ext |
| `[FAN-IN]` | god-headers (top in-degree) | счёт степеней |
| `[FAN-OUT]` | god-components — файл тянет десятки своих (#057) | счёт степеней |
| `[CHAIN]` | самая длинная include-цепочка (реальный путь) | longest path по DAG-конденсации |
| `[ORPHAN]` | заголовок с fan-in 0 (мёртвый или публичный вход) | счёт степеней |
| `[MODULE]` | **циклы между модулями** (каталог = модуль) + топ cross-module связей | Tarjan по модульному графу |
| `[ENCAP]` | **пробой инкапсуляции** — include в чужой `detail/internal/impl/private`, с prod/test-сплитом | пути + рёбра |

> `[MODULE]` и `[ENCAP]` добавлены 2-м заходом — закрывают «module boundary
> violations», единственную из 5 исходных метрик чата, которой probe не трогал.
> Обе — только пути + уже построенный граф (без чтения файлов, без 2-го снимка).

## Корпусная картина

> Числа ниже — ПОСЛЕ фикса резолвера (§3). До фикса циклы «были» у 33/68 за счёт
> 8 фантомных self-loop реп.

- **Циклы есть у 25 из 68 реп** (после фикса; было 33 — минус 8 фантомных).
  Крупный клубок (`largest_scc ≥ 10`) — у **3**: mc2, OptiScaler, FastLED.
- **`[BACKDOOR]` — 0 реп.** Заголовков, инклюдящих `.cpp`, в корпусе нет (чистый
  отрицательный результат: этот запах редок — но детектор дёшев и стоит держать).
- **Self-include — 16 реп-кандидатов оказались багом резолвера → ИСПРАВЛЕН** (§3);
  после фикса self-edges = 0/68.

### Топ клубков (`largest_scc`)

| repo | nodes | largest_scc | cyclic | mutual | что это |
|---|---:|---:|---:|---:|---|
| **mc2** | 887 | **56** | 5 | 48 | MechCommander 2 (Microsoft) — `mclib/stuff/` |
| acts | 2768 | 3 | **46** | 45 | CERN tracking — 45 `hpp↔ipp` пар |
| OptiScaler | 579 | 13 | 2 | 1 | апскейлер кадров |
| FastLED | 2502 | 11 | 4 | 1 | LED-библиотека |
| WSL | 697 | 5 | 8 | 7 | Windows Subsystem for Linux |

### Топ связности (`edges/nodes`, «тихий дрейф»)

AyuGramDesktop 6.43 · stellar-core 5.71 · opentelemetry-cpp 5.71 · acts 5.42 ·
bitcoin 5.04 — у этих каждое изменение задевает в разы больше кода.

### Чемпионы fan-in (god-headers, проверено)

`esphome/core/log.h` **1049** · `FastLED/src/fl/stl/noexcept.h` **965** ·
`opentelemetry/version.h` **764** · `AyuGram/.../lang_keys.h` **588** ·
`acts/.../Definitions/Algebra.hpp` **583** — включаются почти всем репозиторием.

### Самые длинные include-цепочки

FastLED **53** · acts 37 · stellar-core 34 · HPCC-Platform 33 · react-native-macos 32.

## Проверенные находки (открыть и убедиться)

### 1. Гигантский клубок — mc2 (56 компонент)

```bash
sed -n '/^\[CYCLE\]/,/^\[MUTUAL\]/p' runs_history/probe/mc2.txt
#   mclib/stuff/affinematrix.hpp, angle.hpp, auto_container.hpp, color.hpp,
#   database.hpp, extentbox.hpp, filestream.hpp, hash.hpp, ... (+44)
grep -A2 '^\[MUTUAL\]' runs_history/probe/mc2.txt
#   hpp↔hpp: 47   e.g. mclib/mlr/mlr.hpp <-> mclib/mlr/mlrclippingstate.hpp
```
887 узлов всего → один цикл из **56** = модуль `mclib/stuff/` целиком переплетён
взаимными include'ами (47 hpp↔hpp пар). Неразрезаемый клубок, который SF.9 видит
как «цикл есть», а probe показывает **поимённо**.

### 2. Паттерн `hpp↔ipp` — acts (45 пар, совпадает с #054)

```bash
grep -A1 '^\[MUTUAL\]' runs_history/probe/acts.txt
#   hpp↔ipp: 45   e.g. .../CombinatoricIndices.hpp <-> .../CombinatoricIndices.ipp
```
Тот самый класс, что коммит `233ba90c` довёл до 40+ циклов (DRIFT_RUN_REPORT §7.2).
Детектор перечисляет все пары — есть что показать на ревью.

### 0. Границы модулей — module-cycles + пробой инкапсуляции (2-й заход)

Закрывает «module boundary violations» — единственную из 5 исходных метрик, которой
probe не трогал. Обе проверки — только пути + уже построенный граф. Каталог
верхнего уровня = модуль (если репа вся под одним top-dir — берём 2-й уровень).

> ⚠️ В ходе прогона нашёлся и **исправлен баг самого probe**: `parse()` не снимал
> кавычки с путей выгрузки → `is_testish`/имена модулей ломались, числа были
> недостоверны. Все цифры ниже — ПОСЛЕ фикса и сверены с реальным выводом.

**`[MODULE]` — циклы между модулями** — «circular dependency between modules»,
которое user-declared rules archcheck и должны запрещать. Грубее file-level SF.9,
но отвечает на другой вопрос: «переплетены ли КРУПНЫЕ блоки».

**Module-cycles: 10/68 реп** (проверено по реальному выводу): ADIOS2
(bindings↔source), azure-sdk-for-cpp (samples↔sdk), FastLED (examples↔src↔tests),
HPCC-Platform (клубок из 14 модулей!), itksnap (Common↔GUI↔Logic↔Testing), llama.cpp
(common↔ggml↔include↔vendor), nntrainer (api↔nntrainer, 2 цикла), opendht
(include↔src), opentelemetry-cpp (exporters↔ext↔sdk↔test_common), stellar-core
(lib↔src). Проверяемые **настоящие** примеры:
```
FastLED      src <-> examples   (продуктовый src/ инклюдит пример)
  src/platforms/stub/fs_stub.hpp -> examples/XYPath/direct.h
stellar-core lib <-> src        («либа» тянется ВВЕРХ в src — layering smell)
  lib/util/stdrandom.h -> src/crypto/ShortHash.h
  src/crypto/SecretKey.cpp -> lib/util/siphash.h
```
> ⚠️ Не путать с file-level циклами (`cyclic_sccs`, 25/68): это РАЗНЫЕ величины.
> acts даёт **0** module-cycles, хотя 46 file-циклов: 5881 cross-module рёбер все
> «вниз» в Core (Tests→Core 2888 и т.п.) — слои не замкнуты, здоровый случай.
> Часть из 10 — артефакт «include/src split» одной библиотеки (opendht, nntrainer),
> часть — настоящие (FastLED, stellar-core). Каталог=модуль это не различает.

**`[ENCAP]` — пробой инкапсуляции** (include из модуля A в `detail/internal/impl/
private` модуля B), с разделением источника на **prod** и **test/example** (тесты,
лезущие в `detail/`, обычно легитимны — sample/demo тоже отнесены к test).

**Результат — двухступенчатая фильтрация, числа проверены руками:**
- Сырой детектор: сотни «пробоев» (acts 124, FastLED 25, cpprestsdk 25...).
- После prod/test-сплита prod→prod осталось у **3 реп**: cpprestsdk 15, acts 13,
  opentelemetry-cpp 2. Остальное — тесты/примеры (норма).
- **Ручная проверка этих 3 разделила их на artifact vs РЕАЛЬНЫЙ:**
  - **cpprestsdk (15) — artifact:** все `Release/src/... -> Release/include/cpprest/
    details/...`. Это раскладка ОДНОЙ либы на impl+include (single top-dir `Release/`
    → каталог-как-модуль ложно делит её на `Release/src` vs `Release/include`).
  - **acts (13) — РЕАЛЬНЫЙ кросс-модульный пробой:** разные модули лезут в
    приватный `Core/.../detail/`:
    ```
    Plugins/GeoModel/src/detail/GeoIntersectionAnnulusConverter.cpp
       -> Core/include/Acts/Surfaces/detail/AnnulusBoundsHelper.hpp
    Fatras/src/Digitization/Segmentizer.cpp
       -> Core/include/Acts/Surfaces/detail/IntersectionHelper2D.hpp
    ```
    Plugins/Fatras/Alignment — отдельные модули, инклюдят внутренности Core. Ровно
    то, что DRIFT.6 boundary_bypass и должен ловить.
  - **opentelemetry-cpp (2) — РЕАЛЬНЫЙ:** `exporters/... -> sdk/.../trace/detail/...`
    (экспортёр лезет в приватное SDK).

**Вывод:** после prod/test-сплита детектор находит **настоящие** межмодульные пробои
(acts, otel) — но их легко спутать с artifact внутренней раскладки либы (cpprestsdk).
Это **прямой аргумент за config-driven границы** (`.archcheck.yml`, #051): с явным
определением модуля «Core» исчез бы и шум cpprestsdk (одна либа = один модуль), и
остались бы реальные пробои acts/otel.

### 3. Self-include: детектор вскрыл БАГ резолвера archcheck → ИСПРАВЛЕН

Из 16 «self-include» кандидатов ручная проверка показала: **почти все — артефакт
резолвера**. Угловой `#include <basename.h>` (системный/библиотечный) схлопывался
на одноимённый проектный заголовок → ложное ребро X→X:

```bash
grep -n include _aidev_run/bitcoin/src/compat/cpuid.h   # 11: #include <cpuid.h>     (системный)
grep -n include _aidev_run/esphome/.../md5/md5.h        # 18: #include <md5.h>       (НЕ свой)
grep -n include _aidev_run/nntrainer/nntrainer/context.h# 28: #include <context.h>   (НЕ свой)
grep -n include _aidev_run/FastLED/src/fl/stl/alloca.h  # 37: #include <alloca.h>    (системный)
```
Во всех — **угловые скобки** и реально другой файл; self-loop ложный. Единственный
кандидат, похожий на настоящий, — кавычки и идентичный путь (AyuGram
`"data/data_peer_values.h"`).

**Первопричина (одно место):** `resolve_by_suffix` при единственном кандидате
возвращал его даже если это сам исходник (`src/compat/cpuid.h` суффиксно матчит
`<cpuid.h>`). Тот же класс, что #036, но для **single-candidate** angle-include,
где mirror-фильтр не срабатывал. Масштаб по корпусу: **26 ложных self-рёбер**;
у **8 реп** self-loop был ЕДИНСТВЕННЫМ «циклом» → SF.9 врал. Оформлено как
задача #058.

**Фикс (2026-05-31):** guard в `resolveInclude` ([src/scan/include_resolver.cpp](../../src/scan/include_resolver.cpp))
— компонент не зависит от себя; self-target понижается до External (`<...>`) /
Unresolved (`"..."`). Легитимное ребро на *другой* одноимённый файл не трогается.
+3 unit-теста. **Проверено на всём корпусе после фикса: self-edges 0/68; все 8
фантомных циклов исчезли** (bitcoin/cvxpy/KataGo/llama.cpp/nntrainer/
ScalableVectorSearch/scenario_simulator_v2/whisper.cpp → cyclic=0); реальные циклы
не задеты (mc2 56, acts 46, esphome 1 — на месте). Детектор окупился: нашёл не
дефект в корпусе, а **дефект в инструменте** — и привёл к его починке.

## Как воспроизвести (целиком)

```bash
cd /home/localadm/projects/cpparch
bash experiments/ai_repo_run/graph_probe_all.sh            # весь корпус
# одна репа:
build/debug/src/archcheck --save-graph-baseline /tmp/g.txt /home/localadm/oss/_aidev_run/mc2
python3 experiments/ai_repo_run/graph_probe.py /tmp/g.txt mc2
```

## Оговорки (честно)

- **`[SELF]` больше не шумит** — баг резолвера, дававший ложные self-loop, исправлен
  (§3); после фикса self-edges = 0/68. Детектор остаётся полезным: теперь любое его
  срабатывание — реальный `#include "self"`.
- **Вендоринг** попадает в граф частично; часть self-loop'ов и fan-in — в
  вендоренных Eigen/folly/FP16/nanobind, это чужой код, не дисциплина репы.
- **Граф = include-граф fast-backend** (без libclang); внешние инклюды не резолвятся.
- Снимок HEAD, не история — дрейф-по-времени тут не меряется (DRIFT/#054 §7).

## Что из этого стоит поднять в настоящие чеки

- **god-component (fan-out)** — уже в **#057** (mc2/acts дают калибровку порога).
- **`edges/nodes` в отчёт** — #057.
- **mutual `hpp↔ipp` / 2-cycle классификатор** — дёшев и точечен; кандидат в
  правило поверх SF.9 («вот пара», а не «цикл есть»).
- **Баг резолвера (угловой basename-collision)** — ИСПРАВЛЕН (§3): guard от
  self-edge в `include_resolver.cpp` + 3 теста. Убрал 26 ложных рёбер и 8 фантомных
  циклов по корпусу.
- **`[BACKDOOR]` header→.cpp** — держать как дешёвый чек: в корпусе 0, тихий, но
  срабатывание почти всегда реальный дефект.
- **`[ENCAP]` пробой инкапсуляции** — proxy для DRIFT.6 (boundary_bypass). Прогон:
  сырой сигнал ложно-громкий (acts 124), после prod/test-сплита prod→prod у 3 реп,
  из них **acts (13) и otel (2) — РЕАЛЬНЫЕ** межмодульные пробои в `Core/.../detail`,
  а cpprestsdk (15) — artifact src↔include одной либы. Урок: детектор работает, но
  ТРЕБУЕТ явного определения модулей (`.archcheck.yml`, #051) — иначе путает
  внутреннюю раскладку либы с пробоем. Сильный аргумент за config-first.
- **`[MODULE]` циклы между модулями** — proxy для «circular dep between modules»
  (spec Level 4) и Martin SDP; **10/68 реп** (≠ 25 file-циклов!), проверены
  настоящие (FastLED src↔examples, stellar-core lib↔src). Часть — артефакт
  include/src-сплита библиотеки. В проде модуль = namespace/config, не каталог.
