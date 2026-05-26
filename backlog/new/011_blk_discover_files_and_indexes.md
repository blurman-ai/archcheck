# [SCAN] Discover files + project indexes

**Дата создания:** 2026-05-26
**Дата старта:** —
**Статус:** new
**Модуль:** SCAN
**Приоритет:** blocker
**Сложность:** S (< 1 дня)
**Блокирует:** #012 (include_resolver)
**Заблокирован:** —
**Related:** #008 (dependency_graph_foundation)

## Цель

Обойти project root и собрать набор project files, плюс построить
`exact_path_index` и `suffix_index`, которые потребуются resolver-у.

## Контекст

См. §1 и §4 mini-design в #008. Без discovery и индексов resolver вообще не
может работать. Эта подзадача — следующий слой над textual scanner (#008a…g),
но независимая от него: scanner работает с одним файлом, discovery — со всем
репо.

## План выполнения

- [ ] `discover_files(root)` → `std::vector<ProjectFile>` с расширениями из §1 mini-design
- [ ] Исключаемые директории: `.git/`, `build/`, `cmake-build-*/`, `.cache/`, `.idea/`, `.vscode/`, `out/`
- [ ] `exact_path_index`: `unordered_map<std::string, NodeId>` (repo-relative POSIX-нормализованный путь)
- [ ] `suffix_index`: `unordered_map<std::string, std::vector<NodeId>>` (постфиксы пути по `/`-сегментам)
- [ ] Тесты: discovery на маленьком фикстурном дереве, исключение builds, exact/suffix lookup, multi-candidate suffix

## Сделано

- (пусто)

## В работе

- (пусто)

## Следующие шаги

1. Передать эстафету в #012 (resolver)

## Ключевые решения

| Решение | Причина |
|---------|---------|
| Suffix index по `/`-сегментам, не по подстрокам | Совпадает с тем, как `#include` работает — путь, а не произвольная подстрока |
| Без авто-исключения `third_party/` | См. §1 mini-design: эвристики тихо не зашиваем |

## Изменённые файлы

| Файл | Изменение |
|------|-----------|
| `include/archcheck/scan/project_files.h` | new |
| `src/scan/project_files.cpp` | new |
| `tests/unit/scan/project_files_test.cpp` | new |
| `tests/CMakeLists.txt` | подключить новый test source |
