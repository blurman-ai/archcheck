Cut an archcheck release: bump version, finalize CHANGELOG, tag, push — the tag triggers `release.yml`, which builds + publishes the GitHub Release asset.

Канон процесса — [`docs/dev/git_workflow.md`](../../docs/dev/git_workflow.md) §«Release-процесс». Сборка/публикация asset'а — [`.github/workflows/release.yml`](../../.github/workflows/release.yml). Эта команда доводит репо до состояния «push тега» и пушит тег; всё после тега делает CI.

Argument (обязательный): версия `X.Y.Z` (без `v`), например `/release 0.1.0`. Для prerelease — `X.Y.Z-rc1` (workflow пометит как prerelease).

## Steps

1. **Проверки готовности — упёрлись → стоп, показать причину:**
   - `git status` чист (нет незакоммиченного). Грязно → остановиться.
   - Текущая ветка `master` и синхронна с `origin/master` (`git fetch && git status`).
   - Тег `vX.Y.Z` ещё не существует: `git tag -l vX.Y.Z` пусто. Существует → стоп.
   - Аргумент валиден как SemVer (`^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$`).
   - CI на последнем коммите master зелёный (`gh run list --branch master --limit 1`). Красный → предупредить и спросить.

2. **Bump версии в [`CMakeLists.txt`](../../CMakeLists.txt):** `project(archcheck VERSION X.Y.Z ...)`. Сверить, что цифры совпадают с аргументом.

3. **Финализировать [`CHANGELOG.md`](../../CHANGELOG.md):**
   - Переименовать `## [Unreleased]` → `## [X.Y.Z] - YYYY-MM-DD` (сегодняшняя дата).
   - Завести новую пустую `## [Unreleased]` сверху с подзаголовками `### Added / Changed / Fixed` (или пустую — по образцу существующих).
   - Если в `[Unreleased]` пусто — спросить пользователя, точно ли релизить (нечего описывать).

4. **Показать пользователю diff** (`CMakeLists.txt` + `CHANGELOG.md`) и **ждать подтверждения**. Правки запрошены → переписать и показать снова.

5. **Build + smoke gate** (как в `/commit`, но Release — это то, что соберёт CI):
   ```bash
   cmake -B build/release -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
   cmake --build build/release
   got="$(./build/release/src/archcheck --version)"
   test "$got" = "archcheck X.Y.Z"   # версия бинаря совпала с тегом — release.yml проверяет то же
   ```
   Не собралось / версия не совпала → **стоп**, показать вывод.

6. **Коммит:** `chore(release): bump to X.Y.Z` (trailers как обычно). Застейджить только `CMakeLists.txt` + `CHANGELOG.md`.

7. **Аннотированный тег:** `git tag -a vX.Y.Z -m "Release X.Y.Z"`. Lightweight-тег не использовать.

8. **Подтвердить пуш с пользователем** (это outward-facing: пуш тега запускает публикацию Release). После «да»:
   ```bash
   git push origin master --follow-tags
   ```

9. **Сообщить, что дальше делает CI** и дать команду слежения:
   - `release.yml` собирает Linux x86_64, пакует `archcheck-X.Y.Z-linux-x86_64.tar.gz` + `.sha256`, создаёт GitHub Release, гоняет smoke-job.
   - Следить: `gh run watch $(gh run list --workflow release.yml --limit 1 --json databaseId -q '.[0].databaseId')`.
   - Проверить результат: `gh release view vX.Y.Z` — убедиться, что оба asset'а на месте.

## Dry-run

Чтобы прогнать весь pipeline, ничего не релизя по-настоящему: запушить временный тег `vX.Y.Z-rc1` (workflow пометит prerelease), проверить asset, затем удалить и тег, и release:
```bash
gh release delete vX.Y.Z-rc1 --yes
git push origin :refs/tags/vX.Y.Z-rc1
git tag -d vX.Y.Z-rc1
```

## IMPORTANT

- Тег — единственный триггер публикации. Не создавать GitHub Release вручную через `gh release create` (workflow сделает сам; дубль вызовет конфликт).
- `latest` как mutable-ссылку в доках/CI не рекомендовать — только пины по тегу + checksum (см. [`docs/ci_usage.md`](../../docs/ci_usage.md)).
- Версия в `CMakeLists.txt` обязана совпадать с тегом — `release.yml` падает, если `archcheck --version` ≠ тег.
- Не пушить без явного подтверждения пользователя (шаг 8).
