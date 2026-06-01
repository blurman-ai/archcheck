#!/usr/bin/env python3
# Генератор per-commit отчётов (#060). Для каждой dense-репы гоняет #056 --diff по
# ограниченному агентскому окну, собирает коммиты с partial_hits>0 (= коммит привнёс
# Type-3 дубль уже существовавшего кода). Разброс по репам ради разнообразия FP-классов.
# Выход: per_commit_hits.jsonl (для выборки/проверки) + per_commit_hits.md (для чтения).
import subprocess, json, os, sys, glob

DENSE = '~/oss/_aidev_dense'
P56 = '~/projects/cpparch/experiments/partial_duplication/build/partial_duplication'
OUT_JSONL = '~/projects/cpparch/experiments/ai_repo_run/per_commit_hits.jsonl'
OUT_MD = '~/projects/cpparch/experiments/ai_repo_run/per_commit_hits.md'

TARGET = int(sys.argv[1]) if len(sys.argv) > 1 else 320   # сколько подозрительных коммитов набрать
PER_REPO_CAP = 8        # не больше N подозрительных коммитов с одной репы (разброс)
RANGE_CAP = 300         # окно коммитов (HEAD~K..HEAD) на репу
REPO_TIMEOUT = 300      # сек на --diff одной репы

def git(repo, *args):
    return subprocess.run(['git', '-C', repo, *args], capture_output=True, text=True, timeout=60).stdout

def repo_range(repo):
    n = git(repo, 'rev-list', '--count', '--first-parent', 'HEAD').strip()
    if not n.isdigit():
        return None
    n = int(n)
    k = min(n - 1, RANGE_CAP)
    if k < 1:
        return None
    base = git(repo, 'rev-parse', f'HEAD~{k}').strip()
    head = git(repo, 'rev-parse', 'HEAD').strip()
    return f'{base}..{head}' if base and head else None

def parse_diff(text):
    """yield (sha, [hit-lines]) для коммитов с >=1 hit."""
    cur_sha, cur_hits = None, []
    for line in text.splitlines():
        if line.startswith('commit='):
            if cur_sha and cur_hits:
                yield cur_sha, cur_hits
            cur_sha, cur_hits = line.split()[0].split('=', 1)[1], []
        elif line.lstrip().startswith('[weighted=') and 'ADDED' in line:
            cur_hits.append(line.strip())
    if cur_sha and cur_hits:
        yield cur_sha, cur_hits

def parse_hit(h):
    # [weighted=0.96] ADDED <f:l-l> <-> BASE <f:l-l> tokens a/b weighted=.. plain=.. line=.. shared-rare=..
    import re
    m = re.search(r'ADDED\s+(\S+)\s+<->\s+BASE\s+(\S+).*?weighted=([\d.]+).*?line=([\d.]+)', h)
    if not m:
        return None
    return {'added': m.group(1), 'base': m.group(2),
            'weighted': float(m.group(3)), 'line': float(m.group(4))}

def main():
    # порядок: по study_grade_all (большая история = больше агентских коммитов сверху)
    order = []
    sg = '~/projects/cpparch/experiments/ai_repo_run/study_grade_all.tsv'
    if os.path.exists(sg):
        for ln in open(sg).read().splitlines()[1:]:
            order.append(ln.split('\t')[0].replace('/', '_'))
    have = set(os.path.basename(d.rstrip('/')) for d in glob.glob(DENSE + '/*/'))
    repos = [r for r in order if r in have] + [r for r in sorted(have) if r not in order]

    total = 0
    jf = open(OUT_JSONL, 'w')
    mf = open(OUT_MD, 'w')
    mf.write('# Per-commit отчёты: коммиты, привнёсшие Type-3 дубль (#056 --diff)\n\n')
    mf.write('> Claims чекера #056 для глазной проверки. `cd <repo>; git checkout <sha>`,\n')
    mf.write('> открыть ADDED и BASE — реальная копипаста или FP (idiom/header↔impl/data/coincidental).\n\n')
    nrepos = 0
    for name in repos:
        if total >= TARGET:
            break
        repo = f'{DENSE}/{name}'
        if not os.path.isdir(repo + '/.git'):
            continue
        rng = repo_range(repo)
        if not rng:
            continue
        try:
            res = subprocess.run([P56, '--diff', rng, '--repo', repo],
                                 capture_output=True, text=True, timeout=REPO_TIMEOUT)
        except subprocess.TimeoutExpired:
            continue
        nrepos += 1
        cnt = 0
        for sha, hits in parse_diff(res.stdout):
            if cnt >= PER_REPO_CAP or total >= TARGET:
                break
            parsed = [p for p in (parse_hit(h) for h in hits) if p]
            if not parsed:
                continue
            rec = {'repo': name.replace('_', '/', 1), 'dir': repo, 'sha': sha, 'hits': parsed}
            jf.write(json.dumps(rec) + '\n')
            jf.flush()
            mf.write(f'## {rec["repo"]} @ `{sha}`  ({len(parsed)} hits)\n')
            for p in parsed:
                tag = 'verbatim' if p['line'] >= 0.95 else ('diverged' if p['line'] < 0.8 else 'near')
                mf.write(f'- [{tag} w={p["weighted"]:.2f} line={p["line"]:.2f}] `{p["added"]}` ⟵ `{p["base"]}`\n')
            mf.write('\n')
            mf.flush()
            cnt += 1
            total += 1
    jf.close(); mf.close()
    print(f'repos processed: {nrepos}, suspicious commits collected: {total}')
    print(f'-> {OUT_JSONL}\n-> {OUT_MD}')

if __name__ == '__main__':
    main()
