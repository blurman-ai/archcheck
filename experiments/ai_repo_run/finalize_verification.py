#!/usr/bin/env python3
# Собирает VERIFICATION_REPORT.md из verify_results/*.json (вывод ночного workflow #067).
import json, glob, os
HERE='~/projects/cpparch/experiments/ai_repo_run'
RES=f'{HERE}/verify_results'
OUT=f'{HERE}/VERIFICATION_REPORT.md'

recs=[]
for f in sorted(glob.glob(f'{RES}/*.json')):
    try: recs.append(json.load(open(f)))
    except Exception: pass

n=len(recs)
conf=[r for r in recs if r.get('confirmed')]
n_conf_repos=len(conf)
total_conf=sum(len(r.get('confirmed',[])) for r in recs)
total_fp=sum(len(r.get('false_positives',[])) for r in recs)
cyc=[r for r in recs if (r.get('cycle') or {}).get('present')]
# по классам
klass={}
for r in recs:
    for c in r.get('confirmed',[]):
        klass[c.get('klass','?')]=klass.get(c.get('klass','?'),0)+1

L=[]
L.append('# Ночная глазная верификация находок дрейфа (#067)\n')
L.append(f'**Дата:** прогон workflow `verify_findings.js`. Проверено агентами **{n} drift-реп**.\n')
L.append('## Агрегат\n')
L.append(f'- Реп с ПОДТВЕРЖДЁННЫМ копипастом кусков: **{n_conf_repos}/{n}**')
L.append(f'- Всего подтверждённых случаев: **{total_conf}**, ложных: {total_fp}'
         + (f' → precision ≈ {100*total_conf/(total_conf+total_fp):.0f}%' if (total_conf+total_fp) else ''))
L.append(f'- Подтверждённых include-циклов: **{len(cyc)}**')
if klass:
    L.append('\n**Подтверждённый копипаст по классам:**')
    for k,v in sorted(klass.items(),key=lambda x:-x[1]):
        L.append(f'- {k}: {v}')
# --- FALSE POSITIVES: разбор + предложения по фиксам ---
fps=[]
for r in recs:
    for fp in r.get('false_positives',[]):
        fp=dict(fp); fp['repo']=r['repo']; fps.append(fp)
fp_by_class={}
for fp in fps:
    fp_by_class.setdefault(fp.get('fp_class','other'),[]).append(fp)
if fps:
    L.append('\n## False positives — разбор и предложения по фиксам чекера\n')
    L.append(f'Всего FP: **{len(fps)}**, по классам: ' +
             ', '.join(f'{k}={len(v)}' for k,v in sorted(fp_by_class.items(),key=lambda x:-len(x[1]))) + '\n')
    L.append('### Сгруппированные предложения по фиксам (по классам FP)\n')
    for k,v in sorted(fp_by_class.items(),key=lambda x:-len(x[1])):
        L.append(f'**{k}** ({len(v)} случаев):')
        seen=set()
        for fp in v:
            fix=fp.get('fix_proposal','').strip()
            if fix and fix.lower() not in seen:
                seen.add(fix.lower()); L.append(f'- фикс: {fix}')
        L.append('')
    L.append('### FP по одному (что обмануло детектор + где)\n')
    for fp in fps:
        dirn='~/oss/_aidev_dense/'+fp['repo'].replace('/','_',1)
        L.append(f'- **{fp["repo"]}** [{fp.get("fp_class","?")}] `{fp.get("where","?")}` @ `{str(fp.get("sha","?"))[:12]}`')
        L.append(f'  - что случилось: {fp.get("what_happened","—")}')
        L.append(f'  - фикс: {fp.get("fix_proposal","—")}')
        L.append(f'  - inspect: `git -C {dirn} show {str(fp.get("sha","?"))[:12]}`')
    L.append('')

L.append('\n---\n')
L.append('## Подтверждённые находки (для ручного просмотра)\n')
for r in sorted(recs,key=lambda r:-(len(r.get('confirmed',[])))):
    cf=r.get('confirmed',[]); cy=r.get('cycle') or {}
    if not cf and not cy.get('present'): continue
    dirn='~/oss/_aidev_dense/'+r['repo'].replace('/','_',1)
    L.append(f'### {r["repo"]} — проверено {r.get("commits_checked","?")} коммитов')
    if cy.get('present'):
        L.append(f'- **ЦИКЛ:** {cy.get("desc","?")} — {cy.get("evidence","")}')
    for c in cf:
        L.append(f'- [{c.get("klass","?")}] `{c.get("where","?")}` @ `{c.get("sha","?")[:12]}` — {c.get("note","")}')
        L.append(f'  inspect: `git -C {dirn} show {c.get("sha","?")[:12]}`')
    if r.get('verdict_summary'): L.append(f'  > {r["verdict_summary"]}')
    L.append('')
open(OUT,'w').write('\n'.join(L)+'\n')
print(f'VERIFICATION_REPORT: {n} реп, {total_conf} подтв (precision {100*total_conf/(total_conf+total_fp):.0f}% если есть FP), {len(cyc)} циклов -> {OUT}' if (total_conf+total_fp) else f'{n} реп -> {OUT}')
