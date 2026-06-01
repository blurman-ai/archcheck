#!/usr/bin/env python3
# Чекер корпуса (#060). Для каждой ≥300-репы: вердикт clean/drift + per-commit детали.
# Источники:
#   #056 --diff   — копипаст per-commit (ШУМНЫЙ ~16.5% → "кандидаты на ручную проверку")
#   archcheck --graph — include-циклы (НАДЁЖНО → "структурный дрейф")
# Выход (append, идемпотентно через DONE_FILE — можно дозапускать по мере докачки):
#   CORPUS_CHECK_REPORT.md   — детальный per-commit отчёт для ручной проверки
#   corpus_check_summary.tsv — per-repo вердикт (знаменатель для "% без дрейфа")
import subprocess, os, re, sys

HERE='/home/localadm/projects/cpparch/experiments/ai_repo_run'
DENSE='/home/localadm/oss/_aidev_dense'
P56='/home/localadm/projects/cpparch/experiments/partial_duplication/build/partial_duplication'
ARCH='/home/localadm/projects/cpparch/build/debug/src/archcheck'
LIST=f'{HERE}/all_local.tsv'
OUT_MD=f'{HERE}/CORPUS_CHECK_DETAIL.md'
OUT_TSV=f'{HERE}/corpus_check_summary.tsv'
DONE=f'{HERE}/corpus_check_done.txt'

RANGE_CAP=300     # окно коммитов на репу для --diff (последние N; окно фиксируем в отчёте)
P56_TIMEOUT=600
ARCH_TIMEOUT=180

def git(repo,*a,t=60):
    try: return subprocess.run(['git','-C',repo,*a],capture_output=True,text=True,timeout=t).stdout
    except Exception: return ''

def repo_range(repo):
    n=git(repo,'rev-list','--count','--first-parent','HEAD').strip()
    if not n.isdigit(): return None
    k=min(int(n)-1,RANGE_CAP)
    if k<1: return None
    base=git(repo,'rev-parse',f'HEAD~{k}').strip(); head=git(repo,'rev-parse','HEAD').strip()
    return f'{base}..{head}' if base and head else None

def parse_diff(text):
    cur=None; hits=[]
    for line in text.splitlines():
        if line.startswith('commit='):
            if cur and hits: yield cur,hits
            cur=line.split()[0].split('=',1)[1]; hits=[]
        elif line.lstrip().startswith('[weighted=') and 'ADDED' in line:
            hits.append(line.strip())
    if cur and hits: yield cur,hits

HIT_RE=re.compile(r'ADDED\s+(\S+)\s+<->\s+BASE\s+(\S+).*?weighted=([\d.]+).*?line=([\d.]+)')
def parse_hit(h):
    m=HIT_RE.search(h)
    if not m: return None
    a,b=m.group(1),m.group(2)
    return {'added':a,'base':b,'weighted':float(m.group(3)),'line':float(m.group(4)),
            'cross': a.rsplit(':',1)[0]!=b.rsplit(':',1)[0]}

def graph_stats(repo):
    try:
        out=subprocess.run([ARCH,'--graph',repo],capture_output=True,text=True,timeout=ARCH_TIMEOUT).stdout
    except Exception: return {}
    d={}
    for line in out.splitlines():
        m=re.match(r'(\w+):\s+(\d+)',line.strip())
        if m: d[m.group(1)]=int(m.group(2))
    return d

def meta(repo,sha):
    p=git(repo,'show','-s','--format=%an|%ad|%s','--date=short',sha).strip().split('|',2)
    return (p+['?','?','?'])[:3]

def main():
    repos=[ln.split('\t')[1] for ln in open(LIST).read().splitlines() if ln.strip()]
    done=set(open(DONE).read().split()) if os.path.exists(DONE) else set()
    new_tsv = not os.path.exists(OUT_TSV) or os.path.getsize(OUT_TSV)==0
    md=open(OUT_MD,'a'); tsv=open(OUT_TSV,'a'); dn=open(DONE,'a')
    if new_tsv:
        tsv.write('repo\tcommits_postmay\tcopypaste_hits\twithin\tcross\tsccs_cyclic\tlargest_scc\tverdict\n')
    if os.path.getsize(OUT_MD)==0:
        md.write('# Детальный отчёт проверки корпуса (per-commit, для ручной проверки)\n\n')
        md.write(f'> Окно --diff: последние {RANGE_CAP} коммитов на репу. Копипаст = КАНДИДАТЫ '
                 '(#056, ~16.5% precision — глазами!). Циклы = надёжно (archcheck).\n'
                 '> Открыть коммит: команда `inspect:` под каждым.\n')
    for name in repos:
        if name in done: continue
        repo=f'{DENSE}/{name}'
        if not os.path.isdir(repo+'/.git'): continue
        cc=(git(repo,'rev-list','--count','--since=2025-05-01','HEAD').strip() or '0')
        g=graph_stats(repo); sccs=g.get('sccs_cyclic',0); lscc=g.get('largest_scc',0)
        rng=repo_range(repo); commits=[]; timed_out=False
        if rng:
            try:
                res=subprocess.run([P56,'--diff',rng,'--top','2000','--repo',repo],capture_output=True,text=True,timeout=P56_TIMEOUT)
                for sha,hh in parse_diff(res.stdout):
                    parsed=[p for p in (parse_hit(h) for h in hh) if p]
                    if parsed: commits.append((sha,parsed))
            except subprocess.TimeoutExpired: timed_out=True
        n_hits=sum(len(c[1]) for c in commits)
        n_cross=sum(1 for c in commits for p in c[1] if p['cross']); n_within=n_hits-n_cross
        # timed_out → 'partial' (НЕ clean: --diff не досчитал, нельзя записать в "без дрейфа")
        verdict='drift' if (sccs>0 or n_hits>0) else ('partial' if timed_out else 'clean')
        tsv.write(f'{name.replace("_","/",1)}\t{cc}\t{n_hits}\t{n_within}\t{n_cross}\t{sccs}\t{lscc}\t{verdict}\n'); tsv.flush()
        if verdict=='drift':
            md.write(f'\n## {name.replace("_","/",1)} — {cc} коммитов | циклы={sccs}(SCC≤{lscc}) | копипаст={n_hits}\n')
            if sccs>0:
                md.write(f'- **СТРУКТУРНЫЙ ДРЕЙФ (надёжно):** {sccs} include-циклов, крупнейший SCC={lscc}. '
                         f'`{ARCH} --graph {repo}`\n')
            for sha,parsed in commits:
                an,ad,sub=meta(repo,sha)
                md.write(f'\n### `{sha}` — "{sub}" ({an}, {ad}) [{len(parsed)} hits]\n')
                md.write(f'  inspect: `git -C {repo} show {sha}`\n')
                for p in parsed:
                    tag='verbatim' if p['line']>=0.95 else ('diverged' if p['line']<0.8 else 'near')
                    scope='cross-file' if p['cross'] else 'within-file'
                    md.write(f'  - [{tag} {scope} w={p["weighted"]:.2f} line={p["line"]:.2f}] `{p["added"]}` ⟵ `{p["base"]}`\n')
            md.flush()
        dn.write(name+'\n'); dn.flush()
        print(f'{name}: {verdict} hits={n_hits}(cross={n_cross}) cycles={sccs}',flush=True)
    md.close(); tsv.close(); dn.close()

if __name__=='__main__': main()
