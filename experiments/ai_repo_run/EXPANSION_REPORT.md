# Расширение корпуса: +23 AI-плотные репы с ≥300 коммитов после мая (ночь 2026-06-01)

## Задача
Добрать репы по уточнённому критерию **≥300 коммитов после 2025-05** (≈≥1 коммит/день —
сам по себе сигнал ИИ/риска дрейфа), скопировать в `_aidev_dense`, потолок **5 ГБ**,
максимум репрезентативности.

## Что было исчерпано
Все 53 репы с ai≥300 из старого пула `qualified_all` (992, bot-PR-search) **уже склонированы**.
Под ≥300 старый funnel пуст.

## Новая ось discovery (ключевое)
Bot-PR-search НЕ видит репы, где ИИ **коммитит напрямую** (Codex под человеком, локальный
Claude Code, прямые пуши). Их ловит **repo-search активных C++ реп + замер плотности по
git-трейлерам**. Конвейер:
```
gh search repos (star-бакеты + помесячные срезы) → 24641 кандидата
  → measure: blobless-клон → коммитов после мая → при ≥300 замер AI-плотности (метод D)
  → tier-отбор (гейт ≥300; приоритет по AI) → клон под 5 ГБ
```

## Числа прогона
- Измерено: **~6128 / 24641** кандидатов (фаза 1 P=8 + фаза 2 P=14, таймаут 2ч).
- **Хитов ≥300 коммитов после мая: 530** (hit-rate ~8.6%). Пул далеко не исчерпан (домерено ~25%).
- tier1 (conc≥50 ∨ ai≥150): **29**. Clonelist (conc≥5): 55.
- **Склонировано: 23 репы, 5023 МБ** (бюджет отрезал; 32 из clonelist не влезли).
- Clone-fail: 5 (NAAb, Unreal-Engine-5-MCP, dilithion, sipi, Alchemy — полный клон упал).
- CLONEFAIL-ретрай сработал: bakabakaband изначально упал на blobless, прошёл на ретрае.

## Добавленные репы (отсортировано по AI-плотности)
| репа | post-may коммитов | ИИ | conc% | тир |
|---|---|---|---|---|
| Ball-Lang/ball | 479 | 430 | 89 | 1 |
| aethersdr/AetherSDR | 2003 | 1613 | 80 | 1 |
| deltahdl/deltahdl | **9911** | 7664 | 77 | 1 |
| alexandrosk0/Smatchet | 980 | 740 | 75 | 1 |
| adecarolis/wfweb | 627 | 466 | 74 | 1 |
| battlesloth/AstrOs.ESP | 482 | 350 | 72 | 1 |
| boydsoftprez/NereusSDR | 2788 | 1998 | 71 | 1 |
| ademclk/dotlanth-deprecated | 307 | 213 | 69 | 1 |
| brooksthemaker/ProtoHUD | 698 | 477 | 68 | 1 |
| BALL-Project/ball | 886 | 577 | 65 | 1 |
| brontoguana/krasis | 599 | 357 | 59 | 1 |
| ciaranm/glasgow-constraint-solver | 484 | 284 | 58 | 1 |
| CrispStrobe/CrispASR | 2292 | 1262 | 55 | 1 |
| Aiacos/ajazz-control-center | 622 | 336 | 54 | 1 |
| CTalkobt/M65Compiler | 534 | 285 | 53 | 1 |
| alphaonex86/CatchChallenger | 301 | 161 | 53 | 1 |
| cpp-for-everything/MPAPP | 343 | 173 | 50 | 1 |
| ayutaz/piper-plus | 719 | 317 | 44 | 2 |
| bylins/mud | 1886 | 772 | 40 | 2 |
| bobjansen/Ibex | 634 | 173 | 27 | 2 |
| cometdom/DirettaRendererUPnP | 893 | 158 | 17 | 3 |
| danielraffel/pulp | 2392 | 364 | 15 | 3 |
| deskull-m/bakabakaband | (ретрай) | — | — | 1-3 |

## Корпус после фазы 1
- **317 реп / 19 ГБ** (было 282 / 14 ГБ; +11 мелких tail-реп из старого пула до разворота, +23 discovery).

## Фаза 2 (+ещё 5 ГБ активных ИИ-проектов, 2026-06-01 день)
Запрос: «пусть будет ещё 5 ГБ активных ИИ-проектов, что не подходит — отбрасывай».
Домерили ещё остаток (P=16, таймаут 2ч): измерено **8291/24641**, хитов ≥300 стало **765**,
tier1 вырос 29→**42**, clonelist(conc≥5) 55→**78**.

**Склонировано +16 реп / 5332 МБ** (бюджет отрезал; conc<5 отброшены):
| репа | коммитов | ИИ | conc |
|---|---|---|---|
| FiveTechSoft/OpenADS | 538 | 490 | 91 |
| b-macker/NAAb | 724 | 655 | 90 (в фазе 1 падал клон — здесь прошёл) |
| FiveTechSoft/HarbourBuilder | 808 | 711 | 87 |
| dilithion/dilithion | 2040 | 1685 | 82 |
| fuddlesworth/PlasmaZones | 3757 | 3032 | 80 |
| gcol33/tulpa | 375 | 260 | 69 |
| fernandotonon/QtMeshEditor | 835 | 523 | 62 |
| fenrus75/powertop | 556 | 335 | 60 |
| ErenAri/Aegis-BPF | 487 | 224 | 45 |
| dasch-swiss/sipi | 340 | 153 | 45 |
| gizmodata/gizmosql | 445 | 193 | 43 |
| FLOX-Foundation/flox | 355 | 150 | 42 |
| etkecc/komai | 3419 | 1368 | 40 |
| elisir80/Decodium-4.0-Core-Shannon | 717 | 221 | 30 |
| ericflo/kiln | 4444 | 983 | 22 (high-velocity tier3) |
| AlchemyViewer/Alchemy | 2740 | 289 | 10 |

## Корпус итог (после обеих фаз)
- **333 репы / 25 ГБ** (старт сессии 282 / 14 ГБ). Добавлено за сессию: 11 tail + 23 + 16 = **50 реп / ~11 ГБ**.
- Хитов ≥300 найдено **765**, склонировано ≥300-AI-плотных **39** (бюджет 2×5 ГБ). Не измерено ещё **16350** кандидатов → пул далеко не исчерпан.

## Связанный фикс
- **#065 (generated-skip)** в `partial_duplication/main.cpp`: дедупликатор по умолчанию пропускает
  in-tree сгенерённый код (protobuf/moc/flatbuffers/ggml-templates) по header-маркеру. Проверено:
  ball.pb.cc — 0 пар (дефолт) vs 26 (`--no-skip-generated`). 50/4443 файлов помечены, все действительно generated.

## Артефакты
- `new300_measured.tsv` — все измерения (repo, post-may, ai, conc).
- `new300_winners.tsv` (29) / `new300_clonelist.tsv` (55) — отбор.
- `harvest_repos.sh`, `measure_candidates.sh`, `discover_finish.sh`/`discover_finish2.sh`, `clone_expand.sh`.
- `GENERATED_CODE_EXAMPLES.md` — класс FP сгенерённого кода (на ревью).

## Запас на потом
530 хитов найдено, склонировано 23 (лимит 5 ГБ). Ещё ~75% кандидатов не домерено →
если поднять бюджет, пул легко даёт ещё сотни AI-плотных ≥300-репов.
