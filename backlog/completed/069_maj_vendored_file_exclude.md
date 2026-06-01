# [SCAN][GRAPH] Исключать вендорные ФАЙЛЫ (вне third_party) из дедупа и графа

**Дата создания:** 2026-06-01
**Дата старта:** 2026-06-01
**Дата завершения:** 2026-06-01
**Статус:** done
**Модуль:** SCAN / GRAPH / RULES
**Приоритет:** major
**Related:** #064 (vendor-папки в #056), #068 (vendor-папки в archcheck-графе), #067 (верификация вскрыла класс)

## Цель

Исключать вендорные одиночные либы, лежащие НЕ в `third_party/` (а прямо в `src/`,
`plots/`, рядом с кодом), из (а) дедупликатора #056 и (б) include-графа archcheck.
Они засоряют ВСЕ сигналы: within-file копипаст (`qcustomplot.cpp` 30k строк),
cross-file, циклы. #064/#068 ловят только папки — файлы проскакивают.

## Контекст — найдено в корпусе (#054, вне вендор-папок)

`json.hpp` ×17, `stb_image*.h` и др. stb ×~18, `catch.hpp`/`doctest.h` ×8,
`imgui.cpp` ×3, `httplib.h` ×3, `qcustomplot.*` ×2, `tinyxml2`/`pugixml`/`miniz`/
`sqlite3`/`lua` ×1-2. Все — чужой код, не дрейф автора.

## Три слоя (по решению пользователя)

1. **Курируемый список имён — ДЕЛАТЬ.** База: `nothings/single_file_libs` (эталонный
   список single-file C/C++ либ) + скан корпуса. Матч по basename (case-insensitive):
   `qcustomplot.*`, `sqlite3.[ch]`, `stb_*.h`, `json.hpp`, `httplib.h`, `miniz.[ch]`,
   `imgui*.cpp`, `tinyxml2.*`, `pugixml.*`, `lua.c`, `doctest.h`, `catch.hpp`,
   `lodepng.*`, `cJSON.*`, `glad.c`, … Ловит ~80%.
2. **License-шапка — ДЕЛАТЬ.** Первые ~40 строк содержат имя чужой либы + пермиссивную
   лицензию (MIT/BSD/zlib/Apache/public domain / "Copyright (c) <не-автор>"). Ловит
   переименованные/незнакомые вендоры. Stateful-парс шапки (как SF.7/generated-skip).
3. **Размер-эвристика — ОЦЕНЕНО: размер ОДИН опасен, только размер+license.**
   Скан >5000 строк (вне вендор-папок) показал: среди крупных есть РЕАЛЬНЫЙ авторский
   код — `R5900_Recompiler.cpp` 52k и `PS2_Gpu.cpp` 40k (рукописное ядро PS2-эмулятора
   hps2x64), `mainwindow.cpp` 32k / `DecodiumBridge.cpp` 35k (авторский Qt). Чистый
   размер их выкинул бы. Более того, `mainwindow.cpp` 32k — это САМ сигнал дрейфа
   (god-file, Lakos.GodHeader), его надо ЛОВИТЬ, не исключать. → слой 3 только как
   «размер >5000 И license-шапка чужой либы» (слой 2 обязателен), либо вовсе не нужен
   при хорошем слое 1+2.

### Расширение списков (из скана корпуса)

- **Курируемые имена (слой 1) добавить:** `entt.hpp`, `miniaudio.h`, `uni_algo.h`,
  `cutlass/cute/*`, `vulkan.hpp`, `td_api.h` (TDLib), `nuklear.h`.
- **Сигнальные каталоги (дёшево в #068 дир-эксклюд):** `single_include`, `*-source`
  (напр. `qcustomplot-source`), `*_bundled` / `*-bundled`, `amalgamation`, `cutlass`.

## Прецедент — GitHub Linguist (эталон, исследовано 2026-06-01)

GitHub для подсветки языков использует Linguist; у него ровно эта задача решена:
- `lib/linguist/generated.rb` — маркеры генерёнки (protobuf `DO NOT EDIT!`, Go
  `Code generated`, Thrift, Cython…) = наш слой 2/#065; плюс **minified = средняя
  длина строки >110 симв**; плюс path-паттерны (node_modules, lock-файлы…).
- `lib/linguist/vendor.yml` — ~130-150 паттернов вендора = слой 1/#064/#068, НО
  заточен под web/JS — конкретных C/C++ либ (qcustomplot/sqlite/stb) по имени НЕТ.

**Чего у Linguist НЕТ → наши добавки:**
- **Цифровая эвристика (data-таблицы)** — отсутствует. Linguist ловит minified по
  длине строки, но не числовые таблицы (`JSC_map.cpp`/`inv.cpp` он бы пропустил).
- **C/C++ single-file либы по имени** — нужен свой список (база `nothings/single_file_libs`).

### Слой 4 (НОВЫЙ) — цифровая плотность (data-таблицы). ВАЛИДИРОВАН.

Если доля цифр/букв высокая — файл это данные, не код, не проверять. Замер на корпусе
(первые 4000 строк): код 0.03–0.13 (pii_detector, mainwindow, PS2-эмулятор), данные
0.97–**448** (JSC_map, JSC_list, GNFS `inv.cpp` — 80 букв на 35851 цифру!). Порог
**цифр/букв > ~0.4** чисто отделяет. Дёшево, ловит то, что маркеры/имена пропускают.
Это закрыло спорный `inv.cpp` (оказался числовой таблицей).

## Реализация

Общий `isVendoredFile(path, headerBytes)` (curated-имена + license-эвристика) →
- #056 `collectFromSource`: skip (как generated-skip #065);
- archcheck `discoverFiles`: пропустить файл (рядом с `isExcludedDirName`).
Список known-имён — в data/config, дополняемый. Сохранить найденный по корпусу список
как артефакт для исключения в исследовании (#054).

## Критерий приёмки

- [x] Слой 1 (имена) в #056 и archcheck; фикстуры pass/fail.
- [x] Слой 2 (license-шапка) в #056 и archcheck; фикстуры.
- [x] Слой 3: решение принято в теле задачи (§3) — только «размер+license», отдельный код не нужен.
- [x] Перемер корпуса (2026-06-01) — A/B через `--no-skip-vendored` / stash-бинарь:
  - **#056 dedup, DataPlotter** (qcustomplot.cpp 35529 строк в `src/plots/`): фрагменты
    502→1147 без скипа; репортов 34→68 — **33 пары** чистого qcustomplot within-file шума
    подавлено; дефолт не пропускает ни одной вендор-пары (0 leaks).
  - **archcheck граф, узлы:** DataPlotter 99→**97**; BambuStudio 3176→**3001** (175 вендор-файлов
    imgui/miniz/nlohmann убраны из fan-in/god-header/CCD-метрик).
  - Циклов в вендоре этих репо не было (single-file либы) → `sccs_cyclic` без изменений;
    узловой шум снят. Вендор-*каталоги* с циклами (исходный кейс #068) на этом срезе не встретились.

---

## Сделано

- Разведка инфраструктуры; **находка: #068 — фантом** (заявленный код отсутствовал, см. ниже).
- Канонический vendor-классификатор в header-only `file_classification.h`:
  `isVendoredFile` (слой 1 curated-имена + слой 2 license-шапка), `isVendoredDirName` /
  `pathHasVendoredDir` (#068, dir-эксклюд), `baseName`, `toLowerAscii`.
- archcheck граф: фильтр `filterVendored` в `buildGraphForSource` — дропает вендор-файлы И
  поддеревья вендор-каталогов до `addNode` (читает контент один раз, кэширует для include-скана).
- #056 spike: `isVendoredFile(baseName(label), src)` в `collectFromSource`, gated
  `Options::skipVendored` (`--no-skip-vendored`); общий хедер через `target_include_directories`.
- Тесты: 12 кейсов (`file_classification_test.cpp` unit + `vendor_exclude_test.cpp` integration),
  все 272 теста зелёные; lizard чисто. Spike smoke: vendored json.hpp → 0 фрагментов by default.

## Следующие шаги

1. (Опц.) Корпус-перемер (#067) — подтвердить очистку циклов/копипаста на реальных репо.
2. Коммит(ы) по ≤50 строк: (а) хедер+unit-тест, (б) graph-фильтр+integration, (в) spike-врезка.
3. Закрыть #069 → completed/; синхронизировать #068 (фантом исправлен здесь).

## Ключевые решения

- **D1. Хранение списка (слой 1):** constexpr-массив в `file_classification.h` (как
  `kExcludedDirNames`/`kMarkers`), не data-файл. SSOT, ноль рантайм-IO, единый для обоих путей.
- **D2. Точка фильтра в archcheck — `buildGraphForSource`, НЕ `discoverFiles`.** В
  `file_classification.h:13-16` стоит NOTE «vendor — graph/diff-layer concern, не enumeration»,
  и тест `project_files_test.cpp:110` требует, чтобы `discoverFiles` СУРФЕЙСИЛ `third_party/`.
  Фильтр на graph-слое не ломает это и затрагивает только граф/метрики.
- **D3. Скоуп итерации:** слой 1+2, оба пути. Слой 3/4 — отдельным шагом.
- **D4. #068 закрывается в #069:** dir-вендор-эксклюд кладётся в тот же graph-фильтр (точка B).

## Находка — #068 помечена `done`, но кода нет

`backlog/completed/068_maj_graph_vendor_exclude.md` описывает `kExcludedVendor` в
`project_files.cpp` (граф очищен от вендор-циклов). **Кода нет нигде:** `git log -S"kExcludedVendor"`
по всем веткам пусто; `discoverFiles` исключает лишь `.git/build/cmake-build-*`;
`buildGraphForSource` строит граф по ВСЕМ файлам. Работа #068 потеряна/не закоммичена →
include-граф archcheck сейчас включает вендор. #069 закрывает это по файлам И по каталогам.

## Изменённые файлы

- `include/archcheck/scan/file_classification.h` — vendor-классификатор (слои 1+2 + dir #068).
- `src/graph/graph_builder.cpp` — `filterVendored` в `buildGraphForSource`.
- `experiments/partial_duplication/{main.cpp,CMakeLists.txt}` — skip + флаг + include-path.
- `tests/unit/scan/file_classification_test.cpp`, `tests/integration/graph/vendor_exclude_test.cpp` — новые.
- `tests/CMakeLists.txt` — регистрация двух тест-файлов.
- _(не закоммичено — ждёт `/commit`)_
