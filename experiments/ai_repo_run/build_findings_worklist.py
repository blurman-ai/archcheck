#!/usr/bin/env python3
# Строит verify_worklist.json для ночной верификации (#067).
# На каждую drift-репу: flagged-коммиты из детального отчёта (с хитами как подсказки)
# + добор до 5 самыми ЖИРНЫМИ коммитами истории (по insertions+deletions) для
# независимого аудита. Агент смотрит каждый коммит на перенос кусков/частичные совпадения.
# Плюс cycle-юнит на каждую репу с include-циклом.
import re, json, subprocess
HERE='/home/localadm/projects/cpparch/experiments/ai_repo_run'
DENSE='/home/localadm/oss/_aidev_dense'
DETAIL=f'{HERE}/CORPUS_CHECK_DETAIL.md'
TSV=f'{HERE}/corpus_check_summary.tsv'
OUT=f'{HERE}/verify_worklist.json'
BREADTH=5   # коммитов на репу (гарантировано)

repo_re=re.compile(r'^## (\S+) — (\d+) коммитов \| циклы=(\d+)\(SCC≤(\d+)\) \| копипаст=(\d+)')
sha_re =re.compile(r'^### `([0-9a-f]+)` — "(.*)" \(')
hit_re =re.compile(r'^\s*- \[(\w+) (within-file|cross-file) w=([\d.]+) line=([\d.]+)\] `([^`]+)` ⟵ `([^`]+)`')

# 1) flagged-коммиты из детального отчёта
det={}  # repo -> {sha -> [hits]}
cur=None; commit=None
for ln in open(DETAIL):
    m=repo_re.match(ln)
    if m: cur=m.group(1); det.setdefault(cur,{}); commit=None; continue
    if cur is None: continue
    m=sha_re.match(ln)
    if m: commit=m.group(1); det[cur].setdefault(commit,[]); continue
    m=hit_re.match(ln)
    if m and commit:
        det[cur][commit].append({'tag':m.group(1),'scope':m.group(2),'line':float(m.group(4)),
                                 'added':m.group(5),'base':m.group(6)})

# 2) список drift-реп из TSV (+ sccs)
drift=[]
for ln in open(TSV).read().splitlines()[1:]:
    p=ln.split('\t')
    if len(p)>=8 and p[7]=='drift':
        drift.append({'repo':p[0],'cc':int(p[1]) if p[1].isdigit() else 0,'sccs':int(p[5]) if p[5].isdigit() else 0})

def fat_commits(d, exclude, k):
    """top-k самых жирных post-may коммитов (insertions+deletions), не в exclude."""
    try:
        out=subprocess.run(['git','-C',d,'log','--since=2025-05-01','--no-merges',
                            '--pretty=@@%H|%s','--numstat'],capture_output=True,text=True,timeout=60).stdout
    except Exception: return []
    commits=[]; sha=sub=None; tot=0
    for line in out.splitlines():
        if line.startswith('@@'):
            if sha: commits.append((tot,sha,sub))
            h=line[2:].split('|',1); sha=h[0]; sub=h[1] if len(h)>1 else ''; tot=0
        elif '\t' in line:
            a,dl=line.split('\t')[:2]
            tot += (int(a) if a.isdigit() else 0)+(int(dl) if dl.isdigit() else 0)
    if sha: commits.append((tot,sha,sub))
    commits.sort(key=lambda x:-x[0])
    res=[]
    for tot,sha,sub in commits:
        if sha[:12] in exclude or sha in exclude: continue
        res.append({'sha':sha,'subject':sub,'churn':tot,'flagged':False,'hits':[]})
        if len(res)>=k: break
    return res

repos=[]; cyc_units=[]
for r in sorted(drift,key=lambda x:-(x['cc'])):
    d=f'{DENSE}/{r["repo"].replace("/","_",1)}'
    flagged=det.get(r['repo'],{})
    units=[{'sha':s,'subject':'(flagged)','churn':None,'flagged':True,'hits':h} for s,h in flagged.items() if h]
    if len(units)<BREADTH:
        units += fat_commits(d, set(flagged.keys()), BREADTH-len(units))
    repos.append({'repo':r['repo'],'dir':d,'cc':r['cc'],'sccs':r['sccs'],'commits':units})
    if r['sccs']>0:
        cyc_units.append({'repo':r['repo'],'dir':d,'sccs':r['sccs']})

total_commit_units=sum(len(r['commits']) for r in repos)
json.dump({'repos':repos,'cycles':cyc_units}, open(OUT,'w'), ensure_ascii=False)
print(f'worklist: drift-реп={len(repos)}, commit-юнитов={total_commit_units}, cycle-юнитов={len(cyc_units)} -> {OUT}')
