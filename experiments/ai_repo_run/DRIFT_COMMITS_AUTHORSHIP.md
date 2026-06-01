# Авторство коммитов-виновников дрейфа: ИИ или человек?

> **Постановка.** Прогон #054 (см. `DRIFT_RUN_REPORT.md` §7) локализовал коммиты,
> которые ВНЕСЛИ архитектурный дрейф (копипаст-рост или рождение циклов в
> include-графе) на корпусе AIDev (`/home/localadm/oss/_aidev_run/`). Здесь для
> каждого такого коммита по **локальному git** определяется автор/коммиттер,
> приводится полный текст сообщения и выносится вердикт «ИИ / человек / неясно»
> с честным указанием, на чём он основан.
>
> **Дата анализа:** 2026-05-31. **Источник:** только локальный git (`git show`,
> `git log`), без выхода в сеть. Инструменты локализации: `line_duplication`
> (`/tmp/line_dup_build/`), `archcheck --graph` (`build/debug/src/archcheck`),
> worktree-бисекция по first-parent.

---

## ГЛАВНЫЙ ВЫВОД (честно, до таблиц)

**По локальному git нельзя надёжно отличить ИИ-авторство от человеческого для
коммитов-виновников.** Из **11** локализованных коммитов:

- **0** имеют явный AI-authorship trailer (`Co-Authored-By: Claude/Copilot/Cursor/…`);
- **0** имеют author/committer = AI-кодинг-бот (`copilot[bot]`, `cursoragent`, `devin-ai-…`);
- **1** содержит AI-маркер в теле — но это **CodeRabbit auto-review note** (комментарий
  ревью-бота к PR, НЕ признак того, что код написал ИИ) — acts `233ba90c`;
- остальные **10** — обычные человеческие сообщения от идентифицируемых
  мейнтейнеров (личный e-mail, неформальный/шаблонный текст, иногда с опечатками).

**Парадокс корпуса AIDev.** Все 11 репозиториев — из датасета, где ИИ-агенты
заведомо использовались, и это видно на уровне РЕПЫ (FastLED: 248 trailer'ов
`Co-Authored-By: Claude Opus`, 33 коммита `Cursor Agent`; firmware: 473; diagnostics:
52; itksnap: 45). НО **ни один конкретный коммит-виновник не несёт такого маркера
на себе**. То есть: репа явно «ИИ-ассистированная», а событие дрейфа в ней
формально человеческое (или неотличимо). Это и есть честный ответ: локальный git
фиксирует ИИ только когда автор сам прикрутил trailer; для drift-коммитов этого
не случилось ни разу.

**Распределение вердиктов:** AI = **0**, человек = **9**, неясно = **2**
(acts `233ba90c` — человек-автор, но PR прошёл через AI-ревью CodeRabbit;
FastLED `fab8f476d` — человек-коммиттер в репе с массовым ИИ, конкретный коммит
без trailer'а). Подробности — ниже.

---

## Сводная таблица

| repo | SHA | дата | author | committer | дрейф (метрика до→после) | вердикт |
|---|---|---|---|---|---|---|
| acts | `233ba90c` | 2025-06-02 | Andreas Stefl `<stefl.andreas@gmail.com>` | GitHub (PR-merge) | циклы (sccs) 3→43 | **неясно** (человек-автор + CodeRabbit AI-ревью) |
| OptiScaler | `cc5d695e` | 2026-04-25 | FakeMichau `<…@users.noreply.github.com>` | FakeMichau (same) | копипаст dispatch, 23 partial-хита sim=1.0 | **человек** |
| OptiScaler | `d3e1c13a` | 2025-04-18 | cdozdil `<cdozdil@gmail.com>` | cdozdil (same) | циклы 0→2, lscc=4 (merge refactor-hooks) | **человек** |
| opencv-mobile | `dd60637` | 2023-12-10 | nihui `<shuizhuyuanluo@126.com>` | GitHub (PR-merge) | dup 11.3%→25.0% | **человек** |
| opentelemetry-cpp | `148cfe9e` | 2025-03-12 | Marc Alff `<marc.alff@oracle.com>` | GitHub (PR-merge) | dup 27.2%→41.5% (codegen semconv) | **человек** |
| sentry-unreal | `12822e19` | 2025-06-24 | Ivan Tustanivskyi `<tustanivsky@gmail.com>` | GitHub (PR-merge) | dup 13.8%→16.0% | **человек** |
| stellar-core | `ae606b93` | 2025-10-21 | mlo `<marta.lohova@gmail.com>` | GitHub (PR-merge) | dup 8.3%→9.3% (vendoring sqlite + gen-data) | **человек** |
| firmware | `30d7f188` | 2022-11-26 | Ben Meadors `<benmmeadors@gmail.com>` | GitHub (PR-merge) | циклы 0→1 (vendored JSON lib) | **человек** |
| itksnap | `ef2d558f` | 2013-10-07 | Paul Yushkevich `<pyushkevich@gmail.com>` | Paul Yushkevich (same) | циклы 0→1 (.h↔.hxx template) | **человек** (2013, до эпохи LLM) |
| diagnostics | `6629b442` | 2022-02-07 | Mike McLaughlin `<mikem@microsoft.com>` | GitHub (PR-merge) | циклы 5→9 (port dbgshim из runtime-репо) | **человек** |
| FastLED | `fab8f476` | 2025-10-05 | zackees `<z@zackees.com>` | zackees (same) | циклы 4→5, lscc 44→45 | **неясно** (репа массово ИИ, коммит без trailer) |

> «GitHub (PR-merge)» в колонке committer означает `committer = GitHub
> <noreply@github.com>` — стандартный признак merge через веб-интерфейс PR, НЕ
> признак ИИ.

---

## AI-детектируемость на уровне репозитория (контекст)

Чтобы было видно, насколько «ИИ-репы» расходятся с «ИИ-коммитами»: счётчики по
всей истории каждого репо (`git log --all`). AI-кодинг-агенты = trailer'ы
`Co-Authored-By: Claude/Cursor/Copilot` + author `Copilot`/`Cursor Agent`/
`copilot-swe-agent[bot]`. Отделены от CI/dependency-ботов (`github-actions`,
`renovate`, `dependabot`, `dotnet-maestro`, `getsentry-bot`), которые к авторству
кода отношения не имеют.

| repo | всего коммитов | AI-coauthor trailer'ы | AI-кодинг-агент (author) | коммит-виновник несёт AI-маркер? |
|---|---|---|---|---|
| FastLED | 14050 | **248** (Claude Opus 4.6/4.7, Cursor) | Cursor Agent 33, Copilot 18 | **нет** |
| firmware | 11923 | **473** | Copilot 2 (остальное — CI/renovate) | нет |
| diagnostics | 4329 | 52 | Copilot 19 (остальное — dotnet-maestro) | нет |
| itksnap | 2527 | 45 | — | нет |
| stellar-core | 11533 | 15 | — | нет |
| sentry-unreal | 1261 | 10 | — (боты = getsentry/github-actions) | нет |
| acts | 9720 | 8 | — | нет (но PR прошёл CodeRabbit-ревью) |
| opentelemetry-cpp | 1962 | 2 | Copilot 2 | нет |
| opendht | 3502 | 2 | copilot-swe-agent[bot] 4 | (виновник не локализуем) |
| OptiScaler | 4063 | **0** | 0 | нет |
| opencv-mobile | 226 | 0 | 0 (боты = github-actions merge) | нет |

**Вывод:** FastLED и firmware — интенсивно ИИ-ассистированные репы (сотни
trailer'ов), но их конкретные drift-коммиты «чистые». OptiScaler и opencv-mobile
вообще не содержат AI-trailer'ов в истории — при том, что OptiScaler дал
сильнейший копипаст-дрейф (×5). **Связи «есть AI-маркеры в репе» ↔ «AI внёс
дрейф» по локальному git установить нельзя.**

---

## Полные тексты коммитов + разбор

### 1. acts — `233ba90c` (циклы sccs 3→43) — НЕЯСНО

```
SHA:       233ba90c0886da07561e885b3c311ae34d44c04a
Author:    Andreas Stefl <stefl.andreas@gmail.com>
Committer: GitHub <noreply@github.com>
Date:      Mon Jun 2 23:07:11 2025 +0200
Stat:      70 files changed, 214 insertions(+), 130 deletions(-)
```
Полный текст:
```
refactor: Have `.ipp` files include their `.hpp` file (#4291)

Currently our `.ipp` files cannot be interpreted by the compiler by themselves.
This PR adds includes to their corresponding `.hpp` files which makes the compiler
and IDEs happy. `.ipp` files are very similar to `.cpp` files but are included at
the end of the `.hpp` file. The similarity already suggests that adding an include
to the header is reasonable.

<!-- This is an auto-generated comment: release notes by coderabbit.ai -->

## Summary by CodeRabbit

- **Chores**
  - Updated and cleaned up internal header dependencies across multiple files to
    reduce unnecessary includes and improve maintainability.
  - Updated formatting configuration for improved source file identification.
  - Adjusted namespace scoping and include paths for consistency.

- **Refactor**
  - Replaced broad or unused includes with more specific or relevant headers,
    leading to a leaner and more focused codebase.

- **Style**
  - Minor adjustments to blank lines and comment placements for improved code
    readability.

No user-facing features or functional changes were introduced.

<!-- end of auto-generated comment: release notes by coderabbit.ai -->
```
**Разбор.** Текст человеческого вступления («Currently our `.ipp` files…»)
написан мейнтейнером. Структурированный блок «## Summary by CodeRabbit» —
**автогенерация ревью-бота CodeRabbit** (это указано прямо в HTML-комментарии).
Это AI-маркер, но он относится к *описанию PR*, а не к авторству кода. Поэтому
вердикт — **неясно**: код, скорее всего, написал человек, ИИ участвовал на этапе
ревью/release-notes. Дрейф: добавление `#include "X.hpp"` в каждый `X.ipp` при
существующем `hpp→ipp` замкнуло 40 двухкомпонентных циклов разом (sccs 3→43).
Перепроверка:
```bash
d=/home/localadm/oss/_aidev_run/acts; AC=.../build/debug/src/archcheck
git -C "$d" worktree add --detach /tmp/w 233ba90c^ && "$AC" --graph /tmp/w | grep sccs_cyclic  # 3
git -C "$d" worktree add --detach /tmp/w 233ba90c  && "$AC" --graph /tmp/w | grep sccs_cyclic  # 43
```

### 2. OptiScaler — `cc5d695e` (копипаст dispatch) — ЧЕЛОВЕК

```
SHA:       cc5d695e121b5ff88f82ed4c65be72809adf4815
Author:    FakeMichau <49685661+FakeMichau@users.noreply.github.com>
Committer: FakeMichau <49685661+FakeMichau@users.noreply.github.com>
Date:      Sat Apr 25 21:40:47 2026 +0200
```
Полный текст:
```
Remove D3D12Device from DX12 shaders' dispatch
```
**Разбор.** Одна строка, без trailer'ов, author = committer = личный аккаунт.
Классический человеческий коммит. Дрейф: тело `Dispatch()` синхронно правлено по
6 шейдер-пассам (Bias/DI/DS/FT/HudCopy/OS) → 23 partial-хита sim=1.0 (детектор
#056). Перепроверка:
```bash
git -C /home/localadm/oss/_aidev_run/OptiScaler show -s --format=%B cc5d695e
/tmp/partial_dup_build/partial_duplication --diff cc5d695e --repo /home/localadm/oss/_aidev_run/OptiScaler
```

### 3. OptiScaler — `d3e1c13a` (циклы 0→2, merge refactor-hooks) — ЧЕЛОВЕК

```
SHA:       d3e1c13a565f62da88cb99c7d6b0ce3938db4933
Author:    cdozdil <cdozdil@gmail.com>
Committer: cdozdil <cdozdil@gmail.com>
Date:      Fri Apr 18 09:37:03 2025 +0300
```
Полный текст:
```
Merge branch 'refactor-hooks'
```
**Разбор.** Локальный merge мейнтейнера (cdozdil = автор проекта), author =
committer, без trailer'ов. Человек. Дрейф: распил монолита `dllmain` на
`proxies/`+`hooks/` замкнул 4-компонентный цикл (sccs 0→2).

### 4. opencv-mobile — `dd60637` (dup 11.3%→25.0%) — ЧЕЛОВЕК

```
SHA:       dd6063730d76daa9ae3d2f41da515e48554245e3
Author:    nihui <shuizhuyuanluo@126.com>
Committer: GitHub <noreply@github.com>
Date:      Sun Dec 10 20:45:28 2023 +0800
Stat:      32 files changed, 6159 insertions(+), 3933 deletions(-)
```
Полный текст:
```
drawing mono font, ci matrix release, source package (#76)
```
**Разбор.** nihui — мейнтейнер opencv-mobile, краткое неформальное сообщение,
без trailer'ов. Человек. Дрейф (точка скачка найдена бисекцией: idx12 8.96% →
`dd60637` 25.0%): добавлен `jpeg_encoder_rk_mpp.cpp` с внутренним копипастом
(54+44+40 строк) и cross-file дублём в `v4l2_capture_rk_aiq.cpp`. Перепроверка:
```bash
d=/home/localadm/oss/_aidev_run/opencv-mobile
git -C "$d" worktree add --detach /tmp/w dd60637 && /tmp/line_dup_build/line_duplication /tmp/w --top 4
# dd60637^ = 45f92b8 даёт 11.28%; dd60637 = 25.00%
```
> Примечание: §7.1 отчёта приводил как top-дубль пару `jpeg_encoder_v4l_cix.cpp ↔
> jpeg_encoder_v4l_rpi.cpp` — но эти файлы добавлены позже (`722ff95`, 2025-03-23).
> На точке скачка 2023-12 виновник дублирования — `jpeg_encoder_rk_mpp.cpp`.

### 5. opentelemetry-cpp — `148cfe9e` (dup 27.2%→41.5%) — ЧЕЛОВЕК (но дрейф = codegen)

```
SHA:       148cfe9ee2db6bbbee5e8e201bc7dc24913a5fba
Author:    Marc Alff <marc.alff@oracle.com>
Committer: GitHub <noreply@github.com>
Date:      Wed Mar 12 08:55:12 2025 +0100
Stat:      55 files changed, 8915 insertions(+), 438 deletions(-)
```
Полный текст:
```
[SEMANTIC CONVENTIONS] Upgrade semantic conventions to 1.31.0 (#3297)
```
**Разбор.** Marc Alff (Oracle, мейнтейнер otel-cpp), краткое сообщение в принятом
у проекта формате `[TAG] …`, без trailer'ов. Человек. **Важно:** дрейф здесь
артефактный — +8915 строк это **codegen** семантических конвенций
(`semconv/<x>.h` vs `semconv/incubating/<x>.h` — попарно почти идентичны, до 1271
строки на пару). Это машинно-сгенерированные парные хедеры, а не «человек
скопипастил». Перепроверка:
```bash
d=/home/localadm/oss/_aidev_run/opentelemetry-cpp
git -C "$d" worktree add --detach /tmp/w 148cfe9e && /tmp/line_dup_build/line_duplication /tmp/w --top 4
# 148cfe9e^ = 27.19%; 148cfe9e = 41.45%
```

### 6. sentry-unreal — `12822e19` (dup 13.8%→16.0%) — ЧЕЛОВЕК

```
SHA:       12822e19670395f9be4c6931c57e20ba5b68d72e
Author:    Ivan Tustanivskyi <tustanivsky@gmail.com>
Committer: GitHub <noreply@github.com>
Date:      Tue Jun 24 11:49:40 2025 +0300
Stat:      65 files changed, 654 insertions(+), 283 deletions(-)
```
Полный текст:
```
Adopt generic variant type in public APIs (#971)

* Rename tag and extra get/set funtions for scope class
* Update scope extras to use variant instead of string
* Add scope context getter and utilize variants
* Use variant for breadcrumb data
* Use variant for global context
* Use variant for transactions and spans
* Fix lint errors
* Fix tests
* Update CHANGELOG.md
* Add workeround for setting extras on Android
* FIx json string memory leak
```
**Разбор.** Squashed PR со списком исходных коммитов. **Опечатки** («funtions»,
«workeround», «FIx») — сильный человеческий признак (LLM так не пишет в шаблонных
release-notes). Человек. Дрейф: cross-file дубли платформенных пар
(Android↔Apple subsystems/events).

### 7. stellar-core — `ae606b93` (dup 8.3%→9.3%) — ЧЕЛОВЕК (дрейф = vendoring/gen-data)

```
SHA:       ae606b93e5335f3e2d2c1472937ef5655e672934
Author:    mlo <marta.lohova@gmail.com>
Committer: GitHub <noreply@github.com>
Date:      Tue Oct 21 20:39:10 2025 +0000
Stat:      48 files changed, 11579 insertions(+), 89 deletions(-)
```
Полный текст:
```
Merge changes from v24 release branch to `master` (#4970)

# Description
Merge changes from v24 release branch to `master`.

# Checklist
- [ ] Reviewed the contributing document
- [ ] Rebased on top of master (no merge commits)
- [ ] Ran `clang-format` v8.0.0 (via `make format` or the Visual Studio extension)
- [ ] Compiles
- [ ] Ran all tests
- [ ] If change impacts performance, include supporting evidence per the
      performance document
```
**Разбор.** GitHub PR-шаблон с чеклистом (репо-специфичный template, не LLM),
автор mlo. Человек. Дрейф артефактный: top-дубли — вендоренный
`lib/sqlite/sqlite3.c` (амальгама SQLite, `.c↔.h`) и сгенерированный
`src/ledger/P23HotArchiveBugData.cpp` (276-строчный повтор тест-данных). Это
vendoring + gen-data, не ручной копипаст архитектуры.

### 8. firmware (Meshtastic) — `30d7f188` (циклы 0→1) — ЧЕЛОВЕК

```
SHA:       30d7f188e22b8a126434a62b1046faf715c6551c
Author:    Ben Meadors <benmmeadors@gmail.com>
Committer: GitHub <noreply@github.com>
Date:      Sat Nov 26 08:18:59 2022 -0600
```
Полный текст:
```
Merge pull request #1991 from meshtastic/develop

Merge develop to master
```
**Разбор.** Merge develop→master мейнтейнером Ben Meadors. Человек. Дрейф: в этот
merge вошла вендоренная JSON-библиотека (SimpleJSON-стиль) с врождённым взаимным
include `src/mqtt/JSON.h ↔ src/mqtt/JSONValue.h` (`.h` инклюдит `JSONValue.h`,
`JSONValue.h` инклюдит `JSON.h`) — это и есть первый цикл (lscc=2). Перепроверка:
```bash
d=/home/localadm/oss/_aidev_run/firmware; AC=.../build/debug/src/archcheck
git -C "$d" worktree add --detach /tmp/w 30d7f188^ && "$AC" --graph /tmp/w | grep sccs_cyclic  # 0
git -C "$d" worktree add --detach /tmp/w 30d7f188  && "$AC" --graph /tmp/w | grep sccs_cyclic  # 1
```

### 9. itksnap — `ef2d558f` (циклы 0→1) — ЧЕЛОВЕК (2013, до эпохи LLM)

```
SHA:       ef2d558f74c9c7de8412f17ddf5e240408965169
Author:    Paul Yushkevich <pyushkevich@gmail.com>
Committer: Paul Yushkevich <pyushkevich@gmail.com>
Date:      Mon Oct 7 11:26:13 2013 -0400
Stat:      116 files changed, 1984 insertions(+), 806 deletions(-)
```
Полный текст:
```
Fixed the about dialog build error, and added a new filter to speed up histogram computation
```
**Разбор.** Однозначно человек: author = committer = лид ITK-SNAP, неформальное
сообщение, **2013 год — за годы до coding-LLM**. Дрейф: новый
`ThreadedHistogramImageFilter.h ↔ .hxx` (ITK-паттерн template header/impl с
взаимным include) замкнул первый цикл. Тот же механизм, что у acts (`hpp↔ipp`).

### 10. diagnostics (dotnet) — `6629b442` (циклы 5→9) — ЧЕЛОВЕК

```
SHA:       6629b4424ac5cf9920fcc4e53d29d4c94a27c42b
Author:    Mike McLaughlin <mikem@microsoft.com>
Committer: GitHub <noreply@github.com>
Date:      Mon Feb 7 18:01:35 2022 -0800
Stat:      176 files changed, 57736 insertions(+), 4623 deletions(-)
```
Полный текст:
```
Port dbgshim to the diagnostics repo (#2842)

Port dbgshim to the diagnostics repo
runtime repo commit hash: 442a42147ef23c3b9742abcd8b997e8f472af68a
Add a subset of utilcode. Add more runtime include files.
Add PAL_RegisterForRuntimeStartup and the other PAL support it needs like
WaitForSingleObject, OpenProcess, EnumProcessModules, etc.
Add dbgshim packaging support.
Sync diagnostics/eng/native to the runtime
Make IsShipping* properties consistent
Change eeversion not to require the DAC to display runtime version
```
**Разбор.** Mike McLaughlin (Microsoft), описательное человеческое сообщение
(перечисление сделанного, ссылка на commit hash другого репо). Человек. Дрейф
импортный: 57736 строк портированы из dotnet/runtime вместе с уже существовавшими
там циклами (utilcode/PAL). Не «новый» дрейф, а перенос чужого.

### 11. FastLED — `fab8f476` (циклы 4→5) — НЕЯСНО

```
SHA:       fab8f476dc50307229c7be6a696861ab575fabd2
Author:    zackees <z@zackees.com>
Committer: zackees <z@zackees.com>
Date:      Sun Oct 5 11:07:02 2025 -0700
Stat:      4 files changed, 432 insertions(+), 334 deletions(-)
```
Полный текст:
```
refactor(parlio): separate PARLIO driver into its own module
```
**Разбор.** Сообщение в Conventional Commits (`refactor(parlio): …`), author =
committer = Zach Vorhies (мейнтейнер FastLED). На самом коммите trailer'а нет →
формально человек. НО: FastLED — **интенсивно ИИ-ассистированная** репа (248
trailer'ов `Co-Authored-By: Claude Opus 4.6/4.7`, 33 коммита `Cursor Agent`,
коммиты вида `feat(agents): add new do-agent…` в том же окне). Велика вероятность,
что код писался с ИИ, но это не зафиксировано в данном коммите. Поэтому —
**неясно**. Перепроверка:
```bash
d=/home/localadm/oss/_aidev_run/FastLED; AC=.../build/debug/src/archcheck
git -C "$d" worktree add --detach /tmp/w fab8f476^ && "$AC" --graph /tmp/w | grep sccs_cyclic  # 4
git -C "$d" worktree add --detach /tmp/w fab8f476  && "$AC" --graph /tmp/w | grep sccs_cyclic  # 5
```

---

## opendht — виновник НЕ локализуем (честная оговорка)

opendht показал копипаст-рост 3.4%→10.4%, но это **постепенная аккреция за 11
лет** (2015→2026), на снапшот-уровне всего 2 cross-file блока, top-дубли на HEAD —
within-file (`network_engine.cpp` внутренние повторы). **Единого коммита-скачка
нет** — дрейф размазан по сотням коммитов на каждый шаг (idx3→idx4 = 121 коммит).
Локализовывать «виновника» здесь нечестно: его нет. Не засчитан как
single-commit-виновник.

---

## Итоговая статистика по AI-детектируемости

| Категория | Кол-во |
|---|---|
| Локализованных коммитов-виновников | **11** |
| из них с явным AI-authorship trailer (`Co-Authored-By: Claude/…`) | **0** |
| из них author/committer = AI-кодинг-бот | **0** |
| из них с AI-маркером в теле (CodeRabbit review-note) | 1 (acts) |
| Вердикт «человек» | **9** |
| Вердикт «ИИ» | **0** |
| Вердикт «неясно» | **2** (acts — AI-ревью; FastLED — AI-репа, чистый коммит) |

**Методологический вывод для статьи.** Гипотезу «ИИ вносит архитектурный дрейф»
**нельзя проверить через git-метаданные коммитов**. Даже в репах, где ИИ
использовался массово (FastLED, firmware — сотни trailer'ов), конкретные
коммиты, ломающие граф/множащие копипаст, не несут AI-подписи. Причины: (а)
trailer ставится непоследовательно; (б) дрейф часто приходит через merge'и,
vendoring и codegen, где «авторство» вообще размыто; (в) самый частый committer —
`GitHub <noreply@github.com>` (web-merge PR), что обнуляет committer-сигнал.
**Сильная сторона archcheck именно в том, что он меряет САМО СОБЫТИЕ дрейфа в
графе/копипасте независимо от того, кто автор** — это надёжно, в отличие от
попытки атрибутировать ИИ по метаданным.

Характер дрейфа важнее авторства: из 11 виновников 4 — артефактные/импортные
(opentelemetry codegen, stellar vendoring+gen-data, diagnostics порт из чужого
репо, firmware вендоренная JSON-либа), а не «ИИ/человек написал плохой код». Это
тоже честный результат: часть «дрейфа» — это вендоринг и генерация, и его стоит
помечать отдельно (класс шума #056), а не приписывать ни ИИ, ни человеку.
