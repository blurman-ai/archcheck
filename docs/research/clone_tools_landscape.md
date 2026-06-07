# Детекторы клонов vs archcheck — ландшафт и разграничение

_Составлено 2026-06-06. Отвечает на повторяющийся вопрос «а это не то же самое,
что PMD CPD / SonarQube / jscpd?». Дизайн-источник по нашей подсистеме —
[../duplication_architecture.md](../duplication_architecture.md); таксономия типов
клонов и формальные методы — [code_clones.md](code_clones.md); эмпирика TP —
[dedup_techniques_corpus.md](dedup_techniques_corpus.md)._

Зачем эта дока: чтобы не пересобирать рассуждение каждый раз. Короткий ответ —
**их сравнивать с нашим детектором бессмысленно: детектор у всех один и тот же
(токеновый Rabin-Karp-класс). Сравнивать надо продукты.** Они — «найди клоны».
archcheck — «держи архитектурные инварианты в CI», где копипаст лишь **один из
датчиков** рядом с циклами / SF.* / god-headers / levelization.

---

## 1. Сводная таблица

| Инструмент | Техника | Типы клонов | Что на выходе | Рантайм / упаковка | Как гейтит CI | Что это вообще |
|---|---|---|---|---|---|---|
| **PMD CPD** | токены, Karp-Rabin¹ | 1, 2, частично 3 | список блоков | JVM | exit≠0 по `--minimum-tokens` | отдельный clone-детектор |
| **jscpd** | токены, Rabin-Karp | 1, 2, частично 3 | список + % | Node | exit≠0 по `--threshold %` | отдельный clone-детектор (теперь «AI-ready»: MCP/skill) |
| **SonarQube** | свой `sonar-cpd`² | 1, 2, частично 3 | **density %**, дашборд | сервер/платформа | quality gate по **% плотности** new-code (игнор <20 новых строк) | платформа качества (много метрик) |
| **Simian** | строки/текст, dynamic pattern match | 1, 2 | список блоков | JVM | порог по строкам | отдельный clone-детектор (проприетарный) |
| **Duplo** | строки (line-based) | 1 | список блоков | standalone C++ (**заброшен**) | min line length | отдельный clone-детектор |
| **CCFinderX** | suffix-tree, токены | 2 | клоны + genealogies | research (100 MLOC) | — | research-тул |
| **NiCad** | текст + norm + LCS | 3 (**эталон точности 89–96%**) | клон-классы | research (10 MLOC) | — | research-тул |
| **Deckard** | AST-поддеревья | 3 | клоны | research (1 MLOC) | — | research-тул |
| **archcheck** | токены, rare-index + token-LCS³ | 1, 2, 3 (Type-4 — нет, осознанно) | **пары `file:line↔file:line` + тип клона + «вынеси в общее»** | **один статич. бинарь** (только libclang) | exit≠0 на **новую подтверждённую пару** + baseline | **проверка арх-инвариантов**: дубль — 1 сигнал рядом с циклами/SF.*/god-headers/levelization |

¹ PMD CPD прошёл три инкарнации: Greedy String Tiling → Burrows-Wheeler → Karp-Rabin.
Умеет `--ignore-identifiers` / `--ignore-literals` (Type-2), но нормализация **глобальная**.
² SonarQube ушёл с PMD-CPD на собственный `sonar-cpd`, работает по «statements»,
глушит отступы и строковые литералы.
³ Линия SourcererCC: инвертированный индекс на редких токенах + token-LCS confirm.
См. [../duplication_architecture.md §3](../duplication_architecture.md).

## 2. Чем archcheck реально отличается (по убыванию важности)

1. **Это не дубль-тул, а арх-гейт.** Дублирование — один сигнал в одном бинаре
   **рядом с циклами, levelization, SF.7/8/9/21, god-headers**. Ни у кого из
   списка этого нет — они умеют только клоны (или, как Sonar, всё-подряд-метрики
   на сервере). Это главное отличие; всё остальное — следствия.
2. **Один статический бинарь, без JVM/Node/сервера.** Под CI это «скачал и
   запустил». Конкуренты тянут чужой рантайм; единственный standalone-C++ (Duplo) —
   line-based и мёртвый. → это же и есть причина писать **свой** детектор
   (обернуть нечего, см. §4).
3. **Выход — конкретные пары + что чинить**, а не процент. Sonar валит по
   «плотности %» (агрегат), мы — по конкретной новой паре с указанием «вынеси в
   общий заголовок», рамка Lakos *missing reuse edge*.
4. **Selective normalization под C++-идиомы** (храним имена вызовов и case-метки).
   У PMD/Sonar нормализация глобальная «выкинь все идентификаторы» — на C++ она
   idiom-FP делает *хуже* (каждый `switch`-каркас и `var=call(args)` схлопывается
   в один скелет). Это наш единственный реальный технический делта в детекторе, и
   он имеет смысл **только потому, что мы гейтим** — точность под гейт, не под дашборд.
5. **baseline с первого дня** — легаси замораживается, гейт ловит только новое.
   (Sonar похоже умеет через new-code, остальные — нет.)

## 3. Где мы НЕ отличаемся (честно)

- **Сам детектор** — обычный токеновый Rabin-Karp-класс, как PMD/jscpd. Рва нет.
- **Нормализация Type-2** — `--ignore-identifiers`/`--ignore-literals` у PMD,
  игнор литералов у Sonar, пер-язычная у Simian. «Игнорь имена/литералы» — не наше.
- **Type-3 точность** — эталон NiCad (89–96%), мы её не бьём и не пытаемся
  (Type-4 и AST cross-TU #052 убраны осознанно — мы намеренно *уже* research-тулзов).

## 4. Развилка «свой детектор vs обёртка» — держать честной

Дифференциал archcheck стоит на трёх ногах: **selective-norm-точность под гейт +
arch-рамка (пары, не %) + один бинарь.** Если хоть одна нога не выстреливает —
например, замер покажет, что наша точность на C++ не лучше PMD с `--ignore-literal` —
то YAGNI-правильный ход: **обернуть jscpd/CPD, а не пилить свой**. Свой детектор
оправдан ровно дистрибутивным аргументом: под контракт «один статический бинарь,
runtime только libclang» обернуть нечего (PMD/Simian = JVM, jscpd = Node, Sonar =
сервер, Duplo = line-based и заброшен). Это не «у нас лучше алгоритм» — это «нечего
встроить».

**Итого одной фразой:** если выкинуть из archcheck циклы/SF.*/levelization и
оставить только дубли — мы просто ещё один CPD, и правильнее обернуть jscpd.
Отличие живёт в том, что дубликаты — **не основной продукт, а один датчик на
приборной панели** архитектурных инвариантов.

## Источники

- **PMD CPD** — <https://pmd.github.io/pmd/pmd_userdocs_cpd.html> ·
  алгоритмы/история: <https://github.com/pmd/pmd/blob/main/docs/pages/pmd/userdocs/cpd/cpd.md>
- **jscpd** — <https://github.com/kucherenko/jscpd> · <https://jscpd.dev/>
- **SonarQube** — quality gates: <https://docs.sonarsource.com/sonarqube-server/quality-standards-administration/managing-quality-gates/introduction-to-quality-gates>
  · «Manage Duplicated Code with Sonar»: <https://www.sonarsource.com/blog/manage-duplicated-code-with-sonar>
- **Simian** — <https://simian.quandarypeak.com/>
- **Duplo** — <https://github.com/dlidstrom/Duplo>
- **NiCad / CCFinderX / Deckard** (BigCloneBench) —
  <https://clones.usask.ca/pubfiles/articles/SvajlenkoEvaluatingToolsICSME2015.pdf>
- **SourcererCC** (наша линия индекса) — <https://arxiv.org/pdf/1512.06448>
