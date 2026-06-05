# archcheck — Product Vision

_2026-06-02_

## Что строим

`archcheck` — это **CI-first guardrail для архитектурного дрейфа в C++**.

Не “кто написал код”, а:

- какой `PR` добавил нежелательную зависимость;
- какой коммит создал цикл;
- где выросла связность;
- где команда нарушила собственные модульные границы.

Правильная рамка:

- **Lakos physical design + drift regression в CI**
- **zero-config сначала, config-rules потом**

Неправильная рамка:

- “AI drift detector”
- “универсальный quality platform”
- “ещё один линтер/bug finder”

## Что сейчас не так

### 1. Roadmap переоценивает duplication

Сейчас `docs/ROADMAP.md` делает duplication pass главным narrative `v0.1`.
Это неверно.

По факту:

- граф/drift-слой уже выглядит как продуктовый core;
- duplication шипается как **advisory reporting capability** (`--duplication`,
  report-only, exit 0) — это поддерживаемая фича, а не preview/эксперимент;
- но её precision недостаточен для trusted CI-gate, поэтому она остаётся
  advisory и **не блокирует CI**.

Значит duplication — поддерживаемая advisory-фича, но не главный blocker
продукта и не trusted gate.

### 2. Смешаны продукт и исследование про ИИ

Исследование уже показало:

- drift реален;
- инструмент его ловит;
- но гипотеза “ИИ вызывает дрейф” пока не доказана.

Из этого следует:

- **продукт нельзя позиционировать как detector AI-decay**;
- **исследование AI-vs-human — отдельный track**, не центр roadmap ядра.

### 3. Смешаны shipped core и research branches

Сейчас рядом живут:

- реальный zero-config graph/drift core;
- недоведённый config surface;
- duplication spikes;
- AI discovery/verification pipeline.

Это разные уровни зрелости, но в narrative они местами смешаны.

### 4. Следующий слой ценности описан неудачно

С точки зрения продукта следующий сильный шаг не в том, чтобы просто
“добавить больше AST/libclang”.

Следующий сильный шаг:

- **module boundary rules**
- `layers`
- `forbidden`
- `independence`

То есть проверка **моих архитектурных границ**, а не только generic smells.

## Каким должен быть v0.1

`v0.1` должен быть не “duplication MVP”, а:

## v0.1 — Trusted Drift Core

- zero-config include-graph checks;
- baseline / diff / drift;
- надёжные intrinsic rules;
- понятный CLI-контракт;
- детерминированный output;
- удобный PR/commit workflow.

Что входит по смыслу:

- `SF.7`, `SF.8`, `SF.9`, Lakos-style intrinsic checks;
- `DRIFT.1`, `DRIFT.2`;
- baseline save/load;
- `--diff`;
- text/json reporting;
- trust в результате.

Что НЕ должно быть центром `v0.1`:

- duplication gate;
- AI attribution;
- synthesis loop;
- “большая семантическая платформа”.

Duplication в `v0.1` шипается как **advisory reporting capability**
(`--duplication` — report-only, exit 0, поддерживаемая фича).

Чем она НЕ является в `v0.1`:

- blocking CI gate (нужен gate-grade precision — отдельная будущая работа);
- центром `v0.1` narrative (центр — trusted drift core).

## Каким должен быть v0.2

## v0.2 — Boundary Enforcement

Главная цель `v0.2`:

- превратить `archcheck` из zero-config guardrail
- в guardrail + **контрактные модульные правила**.

Значит фокус `v0.2`:

- `.archcheck.yml` как реальная runtime-фича;
- `layers`, `forbidden`, `independence`;
- нормальная интеграция в CI;
- при необходимости SARIF;
- выборочное расширение graph drift метрик:
  - fan-out growth
  - blast radius growth
  - coupling growth

`libclang` в `v0.2` допустим как supporting backend, но не как главный product story.
Он нужен только там, где реально открывает проверяемую ценность.

## Что заморозить

До прояснения product core не надо тащить в центр roadmap:

- duplication как обязательный gate;
- AI rule synthesis;
- “AI loop” как часть основного narrative;
- plugin API;
- visualization;
- regex-rule zoo;
- Martin metrics как headline-фичу;
- широкую C-support story.

Это можно держать:

- в `future/`;
- в `research/`;
- в preview-слое,

но не как текущий центр продукта.

## Как правильно формулировать продукт

Короткая формулировка:

> `archcheck` — это C++ CLI для CI, который ловит архитектурные регрессии в PR:
> новые нежелательные зависимости, циклы и нарушения модульных границ.

Короткая формулировка для roadmap:

> Сначала trusted drift core. Потом boundary rules. Потом selective semantic expansion.

## Что исправить в ROADMAP

- Убрать framing “phase: duplication pass”.
- Убрать duplication из роли главного `v0.1` narrative.
- Разделить `product core`, `preview`, `research`.
- Явно зафиксировать, что AI-specific claims пока не являются продуктовым обещанием.
- Поднять config boundary rules выше generic semantic expansion.

## Практический приоритет

Порядок должен быть таким:

1. Дочистить product contracts и alignment.
2. Укрепить trusted graph/drift core.
3. Довести config-rules до реального runtime.
4. Duplication уже шипнут как advisory-фича (`--duplication`); следующий шаг по
   нему — поднять precision до gate-grade, и только тогда рассматривать как
   blocking gate.
5. AI-attribution и synthesis держать как отдельное исследование.
