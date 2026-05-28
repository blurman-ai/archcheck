# [CONFIG][V1] Минимальный контракт конфига для post-MVP фазы

**Дата создания:** 2026-05-28
**Дата старта:** —
**Статус:** new
**Модуль:** CONFIG
**Приоритет:** major
**Сложность:** S (спека и примеры, без реализации)
**Целевой релиз:** v1 phase 1 (post-MVP)
**Блокирует:** реализацию config-loader после MVP
**Заблокирован:** —
**Related:** docs/architecture-spec.md, docs/MVP.md, future/v1_maj_agent_config_authoring_rules.md

## Цель

Зафиксировать **минимальный и реально заполняемый человеком** формат `.archcheck.yml`, не превращая первый post-MVP релиз в DSL-конструктор.

## Контекст

Обязательный конфиг в MVP создаёт слишком высокий входной барьер: пользователь
не может просто взять tool и быстро проверить value. Поэтому конфиг выносится в
следующую фазу после MVP и должен быть **узким**:

- `version`
- `modules`
- простые межмодульные правила

Без synthesis-контракта, без pattern-rules, без defaults-матриц, без сложной
иерархии severity и без "policy engine".

Нужен отдельный design-contract до реализации, иначе формат начнёт расползаться
по README, спекам и будущему loader-у.

## Референсные форматы (исследование, 2026-05-28)

Изучены живые, популярные и поддерживаемые инструменты. Ниже — что брать, что не брать, и почему.

### Deptrac — основной референс по shape

Формат: YAML. Раздельно `layers` (кто входит) и `ruleset` (кто куда может смотреть).
Активен: 2.9k stars, релиз 4.6.1 от 13 мая 2026.
Docs: https://deptrac.github.io/deptrac/configuration/
Repo: https://github.com/deptrac/deptrac

```yaml
# Deptrac: отдельно membership, отдельно constraints
layers:
  - name: Controllers
    collectors:
      - type: className
        regex: .*Controller.*
  - name: Services
    collectors:
      - type: className
        regex: .*Service.*
ruleset:
  Controllers:
    - Services
  Services: ~
```

**Что брать:** двухэтапная структура (modules → rules), human-readable, baseline/skip-known-violations.
**Что не брать:** `collectors` с regex — слишком мощно и сложно для v1; PHP-flavoured расширяемость.

---

### Import Linter — лучший пример typed contracts

Формат: TOML в `pyproject.toml`. Каждый контракт имеет явный `type`.
Активен: 1k stars, стабильная документация.
Docs (configure): https://import-linter.readthedocs.io/en/stable/get_started/configure/
Docs (contract types): https://import-linter.readthedocs.io/en/stable/contract_types/
Docs (forbidden): https://import-linter.readthedocs.io/en/v2.9/contract_types/forbidden/

```toml
# Import Linter: type-driven contracts
[[tool.importlinter.contracts]]
name = "domain-no-infra"
type = "forbidden"
source_modules = ["myapp.domain"]
forbidden_modules = ["myapp.infra"]
ignore_imports = ["myapp.domain.exceptions -> myapp.infra.logging"]
```

**Что брать:** идея `type`-driven правил — тип определяет допустимые поля; `ignore_imports` как built-in escape hatch.
**Что не брать:** INI/TOML для archcheck (хуже YAML для нашей экосистемы); Python module model.

---

### dependency-cruiser — лучший пример rule buckets и baseline

Формат: `.dependency-cruiser.js` / `.cjs` / `.json`. Tool генерирует стартовый конфиг через `--init`.
Активен: 6.7k stars, релиз v17.4.2 от 23 мая 2026.
Readme + CLI: https://github.com/sverweij/dependency-cruiser
Rules reference: https://github.com/sverweij/dependency-cruiser/blob/main/doc/rules-reference.md

```json
{
  "forbidden": [
    {
      "name": "no-circular",
      "severity": "error",
      "from": {},
      "to": { "circular": true }
    }
  ],
  "allowed": [
    { "from": { "path": "src/app" }, "to": { "path": "src/domain" } }
  ]
}
```

**Что брать:** rule buckets `forbidden` / `allowed` / `required`; baseline workflow; `--init` как UX-шаблон.
**Что не брать:** JS/JSON config как основной путь (regex-heavy, соблазняет логикой внутри конфига).

---

### Nx module boundaries — тег-based модель (future layer)

Формат: `project.json` tags + JSON depConstraints.
Активен: 28.8k stars у Nx, релиз 22.7.5 от 27 мая 2026.
Docs: https://nx.dev/docs/features/enforce-module-boundaries

```json
{ "tags": ["scope:client"] }
// depConstraints:
{ "sourceTag": "scope:client", "onlyDependOnLibsWithTags": ["scope:shared"] }
```

**Что брать:** идея labels/tags как опциональный вторичный слой поверх path-based modules.
**Что не брать:** это не standalone config; требует pre-marked repo — бесполезно для first-run.

---

### ArchUnit — словарь, не формат

Формат: Java test code, fluent DSL.
Активен: 3.7k stars, релиз 1.4.2 от 18 апреля 2026.
User guide: https://www.archunit.org/userguide/html/000_Index.html

```java
layeredArchitecture()
  .layer("Domain").definedBy("..domain..")
  .layer("App").definedBy("..app..")
  .whereLayer("App").mayOnlyBeAccessedByLayers("UI")
  .check(classes);
```

**Что брать:** vocabulary (`layer`, `mayOnlyAccessLayer`, `beFreeOfCycles`) и семантику правил.
**Что не брать:** формат — это code DSL, не CI-friendly declarative config.

---

### Bazel visibility — allow-list semantics

Docs: https://bazel.build/concepts/visibility?hl=en

**Что брать:** идею `allow-list` access model и `package_group` как именованные наборы.
**Что не брать:** это build-system visibility, не architecture config; слишком низкоуровнево.

---

## Рекомендуемый shape для archcheck v1

Синтез: shape от Deptrac, typed contracts от Import Linter, rule buckets от dependency-cruiser.

```yaml
version: 1

modules:
  domain:
    paths: ["src/domain/**"]
  app:
    paths: ["src/app/**"]
  infra:
    paths: ["src/infra/**"]

rules:
  - type: forbidden
    name: domain-no-infra
    from: [domain]
    to: [infra]

  - type: allow_only
    name: app-only-domain-and-infra
    from: [app]
    to: [domain, infra]
```

Ключевые решения этого shape:
- `modules` — membership через glob paths, без regex, без collectors
- `rules` — каждое правило имеет явный `type` (от Import Linter)
- Тип `forbidden` и `allow_only` — минимальный набор для v1
- `name` — обязателен, используется в violation output
- `from` / `to` — списки module-имён, не regexp
- `ignore` / `baseline` — v1 phase 2, не v1 phase 1

## План выполнения

- [ ] Зафиксировать минимальную YAML-схему `version/modules/rules` — shape из раздела выше как стартовая точка
- [ ] Решить формат path matching для modules: glob only, без regex (аналог Deptrac paths)
- [ ] Решить минимальный набор rule-типов: `forbidden` и `allow_only` (от Import Linter typed contracts)
- [ ] Явно выписать, чего в v1 phase 1 **нет**: pattern-rules, severity per rule, inheritance, suppressions, tags, collectors
- [ ] Добавить 3-4 reference examples: tiny, layered, legacy, mixed include/src tree
- [ ] Описать правила backwards compatibility для схемы
- [ ] Зафиксировать contract в `docs/config_format.md`

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Сначала зафиксировать shape конфига в документе, не в коде
2. Проверить, можно ли объяснить формат в README на одном коротком примере
3. После этого заводить отдельную implementation-task на `config/`

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Минимальный конфиг, не "универсальный rule DSL" | Иначе post-MVP phase сразу утонет в дизайне |
| Только modules + simple deps rules | Это ядро ценности продукта |
| Отдельный contract до кода | Не даёт формату дрейфовать от документа к документу |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| docs/config_format.md | новая спецификация минимального формата |
| README.md | краткий reference example после стабилизации формата |

