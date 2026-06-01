export const meta = {
  name: 'verify-drift-findings',
  description: 'Ночная глазная верификация находок дрейфа: каждая drift-репа → агент аудитит ≥5 коммитов (перенос кусков/частичные совпадения) + цикл, пишет verify_results/<repo>.json',
  phases: [
    { title: 'Discover', detail: 'список drift-реп из summary.tsv' },
    { title: 'Audit', detail: 'агент на репу: 5+ коммитов + цикл' },
  ],
}

const HERE = '~/projects/cpparch/experiments/ai_repo_run'
const ARCH = '~/projects/cpparch/build/debug/src/archcheck'
const DETAIL = `${HERE}/CORPUS_CHECK_DETAIL.md`
const TSV = `${HERE}/corpus_check_summary.tsv`
const RESULTS = `${HERE}/verify_results`

const REPOS_SCHEMA = {
  type: 'object',
  properties: {
    repos: {
      type: 'array',
      items: {
        type: 'object',
        properties: {
          repo: { type: 'string' }, dir: { type: 'string' },
          sccs: { type: 'integer' }, cc: { type: 'integer' },
        },
        required: ['repo', 'dir', 'sccs', 'cc'],
      },
    },
  },
  required: ['repos'],
}

const VERDICT_SCHEMA = {
  type: 'object',
  properties: {
    repo: { type: 'string' },
    commits_checked: { type: 'integer' },
    confirmed: { type: 'integer', description: 'подтверждённых случаев копипаста кусков' },
    fp: { type: 'integer', description: 'ложных срабатываний' },
    cycle_confirmed: { type: 'boolean' },
    note: { type: 'string' },
  },
  required: ['repo', 'commits_checked', 'confirmed', 'fp', 'cycle_confirmed'],
}

phase('Discover')
const disc = await agent(
  `Прочитай TSV ${TSV} (колонки: repo, commits_postmay, copypaste_hits, within, cross, sccs_cyclic, largest_scc, verdict). ` +
  `Верни ВСЕ строки с verdict=="drift" как объекты {repo, dir, sccs, cc}, где ` +
  `dir = "~/oss/_aidev_dense/" + repo с первым "/" заменённым на "_", sccs=sccs_cyclic, cc=commits_postmay. ` +
  `Используй Bash/Read. Это весь список для аудита.`,
  { schema: REPOS_SCHEMA, label: 'discover-drift' }
)
const repos = (disc && disc.repos) ? disc.repos : []
log(`drift-реп к аудиту: ${repos.length}`)

// каталог результатов
await agent(`Bash: mkdir -p ${RESULTS}`, { label: 'mkdir-results' })

phase('Audit')
const auditPrompt = (r) => {
  const safe = r.repo.replace('/', '_')
  const depth = r.cc > 1500 ? 10 : 5
  return (
`Ты аудитор копипаста в AI-написанном C++ репозитории "${r.repo}" (dir: ${r.dir}).
ЦЕЛЬ: найти реальные случаи, где КУСОК кода (функция/блок) скопирован и затем частично
изменён — ВНУТРИ одного файла (главный интерес) или между файлами. Игнорируй копии целых
вендорных файлов и сгенерённый код (protobuf/moc/ggml — у них шапка-маркер).

ВАЖНО про false positives: для КАЖДОГО ложного срабатывания обязательно опиши (а) ЧТО
ИМЕННО обмануло детектор — какой это код и почему токены совпали (общий бойлерплейт
класса? header↔impl? data-таблица? идиома? две разные функции с общим словарём?), и (б)
КОНКРЕТНОЕ предложение по фиксу #056/archcheck (порог/фильтр/эвристика). FP без разбора
и предложения бесполезен — не пиши его.

Шаги:
1. Подсказки чекера по этой репе: \`grep -A60 '^## ${r.repo} —' ${DETAIL}\` — там пары
   ADDED file:line ⟵ BASE file:line и SHA flagged-коммита. Проверь их по реальному коду.
2. Отбери НЕ МЕНЕЕ ${depth} коммитов для проверки: flagged-коммит(ы) + самые жирные ПО C++-КОДУ
   post-may коммиты. Жирность считай ТОЛЬКО по строкам .cpp/.cc/.h/.hpp/.cxx
   (\`git -C ${r.dir} log --since=2025-05-01 --no-merges --numstat --pretty=@@%H\` → суммируй
   add+del только для C++-файлов, возьми топ). Игнорируй коммиты только из доков/генерёнки.
3. Для каждого коммита: \`git -C ${r.dir} show <sha> -- '*.cpp' '*.cc' '*.h' '*.hpp'\`.
   Ищи перенос кусков (почти одинаковые функции/блоки с переименованными id / мелкими правками)
   и ЧАСТИЧНЫЕ совпадения. Подтверди flagged-пары, прочитав ADDED и BASE код.
${r.sccs > 0 ? `4. Цикл: \`${ARCH} --graph ${r.dir}\` (подтверди sccs_cyclic>0), затем найди РЕАЛЬНЫЙ
   include-цикл — какие .h/.cpp взаимно включают друг друга (grep '#include' по репе). Опиши A→B→A.
   ВАЖНО: archcheck уже исключает стандартные вендор-папки (third_party/vendor/external/…), НО
   часть проектов вендорит библиотеку/SDK под произвольным именем (напр. MacKernelSDK, imgui, фрагмент
   движка). Проверь, что цикл в АВТОРСКОМ коде, а не внутри скопированной в дерево чужой библиотеки —
   если вендор под нестандартным именем, пометь cycle.present=false и в evidence укажи имя вендор-папки.` : ''}

Запиши findings в файл ${RESULTS}/${safe}.json (Write tool) в форме:
{ "repo": "${r.repo}", "commits_checked": <N>,
  "confirmed": [ {"sha":"...","klass":"within-file-chunk|cross-file-chunk|partial-edited","where":"file:line-line","note":"<1 фраза>"} ],
  "false_positives": [ {"sha":"...","where":"file:line","fp_class":"coincidental|whole-file|generated|header-impl|data-table|idiom|other",
     "what_happened":"<что именно обмануло детектор по реальному коду>",
     "fix_proposal":"<конкретный фикс #056/archcheck: порог/фильтр/эвристика>"} ],
  "cycle": ${r.sccs > 0 ? '{"present":<bool>,"desc":"A.h → B.h → A.h","evidence":"<файлы>"}' : 'null'},
  "verdict_summary": "<1-2 фразы: реальный ли дрейф и какой>" }

Верни структурный итог: {repo, commits_checked, confirmed (число), fp (число), cycle_confirmed, note}.`
  )
}

const results = await parallel(
  repos.map((r) => () =>
    agent(auditPrompt(r), { schema: VERDICT_SCHEMA, label: `audit:${r.repo}`, phase: 'Audit' })
      .then((v) => v || { repo: r.repo, commits_checked: 0, confirmed: 0, fp: 0, cycle_confirmed: false, note: 'agent-null' })
  )
)

const ok = results.filter(Boolean)
const agg = {
  repos_audited: ok.length,
  repos_with_confirmed_copypaste: ok.filter((v) => v.confirmed > 0).length,
  total_confirmed: ok.reduce((s, v) => s + (v.confirmed || 0), 0),
  total_fp: ok.reduce((s, v) => s + (v.fp || 0), 0),
  cycles_confirmed: ok.filter((v) => v.cycle_confirmed).length,
  commits_checked: ok.reduce((s, v) => s + (v.commits_checked || 0), 0),
}
log(`ГОТОВО: реп=${agg.repos_audited}, коммитов=${agg.commits_checked}, подтв.копипаст=${agg.total_confirmed} (реп ${agg.repos_with_confirmed_copypaste}), FP=${agg.total_fp}, циклов подтв=${agg.cycles_confirmed}`)
return agg
