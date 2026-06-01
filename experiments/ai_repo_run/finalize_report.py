#!/usr/bin/env python3
# Идемпотентная сборка CORPUS_CHECK_REPORT.md = свежий агрегат (из summary.tsv) + детали.
# Гонять после каждого догона чекера.
import os
HERE='~/projects/cpparch/experiments/ai_repo_run'
TSV=f'{HERE}/corpus_check_summary.tsv'
DETAIL=f'{HERE}/CORPUS_CHECK_DETAIL.md'
OUT=f'{HERE}/CORPUS_CHECK_REPORT.md'

rows=[]
for ln in open(TSV).read().splitlines()[1:]:
    p=ln.split('\t')
    if len(p)<8: continue
    rows.append({'repo':p[0],'cc':int(p[1]) if p[1].isdigit() else 0,
                 'hits':int(p[2]) if p[2].isdigit() else 0,'within':int(p[3]) if p[3].isdigit() else 0,
                 'cross':int(p[4]) if p[4].isdigit() else 0,'sccs':int(p[5]) if p[5].isdigit() else 0,
                 'lscc':int(p[6]) if p[6].isdigit() else 0,'verdict':p[7]})
tot=len(rows)
clean=sum(1 for r in rows if r['verdict']=='clean')
drift=sum(1 for r in rows if r['verdict']=='drift')
part =sum(1 for r in rows if r['verdict']=='partial')
cyc  =sum(1 for r in rows if r['sccs']>0)
cp   =[r for r in rows if r['hits']>0]
hits =sum(r['hits'] for r in cp); within=sum(r['within'] for r in cp); cross=sum(r['cross'] for r in cp)
pct=lambda n: f'{100*n/tot:.0f}%' if tot else '—'
top_cp =sorted(cp,key=lambda r:-r['hits'])[:10]
top_cyc=sorted([r for r in rows if r['sccs']>0],key=lambda r:-r['sccs'])[:8]

# подкогорта ≥300 коммитов/год
big=[r for r in rows if r['cc']>300]
def block(rs):
    n=len(rs); cl=sum(1 for r in rs if r['verdict']=='clean'); dr=sum(1 for r in rs if r['verdict']=='drift')
    pa=sum(1 for r in rs if r['verdict']=='partial'); cy=sum(1 for r in rs if r['sccs']>0)
    p=lambda x: f'{100*x/n:.0f}%' if n else '—'
    return n,cl,dr,pa,cy,p

L=[]
L.append('# Проверка корпуса AI-плотных C++ реп — агрегат + детали\n')
L.append(f'**Дата:** 2026-06-01. Прогнан **весь локальный корпус: {tot} AI-плотных C++ реп** '
         f'(чекер: #056 копипаст + archcheck циклы). Когорта «год агентской разработки» '
         f'(≥300 коммитов после мая 2025) — **{len(big)}** реп. Окно `--diff` = последние 300 коммитов. '
         'generated-skip (#065) включён.\n')
n,cl,dr,pa,cy,p = block(rows)
nb,clb,drb,pab,cyb,pb = block(big)
L.append('## Главные цифры (для статьи)\n')
L.append('| | весь корпус | % | когорта ≥300/год | % |\n|---|---|---|---|---|')
L.append(f'| проверено | {n} | 100% | {nb} | 100% |')
L.append(f'| **clean — без дрейфа** | **{cl}** | **{p(cl)}** | **{clb}** | **{pb(clb)}** |')
L.append(f'| **drift — есть дрейф** | **{dr}** | **{p(dr)}** | **{drb}** | **{pb(drb)}** |')
L.append(f'| структурный (циклы>0) | {cy} | {p(cy)} | {cyb} | {pb(cyb)} |')
L.append(f'| partial (таймаут) | {pa} | — | {pab} | — |\n')
L.append('Дрейф по доверию к сигналу:')
L.append(f'- **Структурный: {cyc} реп ({pct(cyc)})** — include-цикл вне стандартных вендор-папок '
         '(#068: third_party/vendor/external/… исключены). ВЕРХНЯЯ ОЦЕНКА: часть реп вендорит под '
         'нестандартными именами (напр. `MacKernelSDK`) → ночная верификация (#067) отделит реальные src-циклы.')
L.append(f'- **Копипаст-кандидаты (ручная проверка): {len(cp)} реп**, всего **{hits} хитов** '
         f'(within-file **{within}**, cross-file **{cross}**). #056 ~16.5% precision → claims, не факт.\n')
L.append(f'> Для статьи (ВЕРХНЯЯ оценка, ждёт верификации #067): **«до {pct(drift)} AI-плотных C++ '
         f'репозиториев показали признаки дрейфа за год; до {pct(cyc)} — структурный (include-цикл).»** '
         f'{pct(clean)} чистых по сырому чекеру.\n')
L.append('## Топ по копипаст-хитам\n')
L.append('| репа | коммитов | копипаст | within | cross | циклы |\n|---|---|---|---|---|---|')
for r in top_cp:
    L.append(f'| {r["repo"]} | {r["cc"]} | {r["hits"]} | {r["within"]} | {r["cross"]} | {r["sccs"]} |')
L.append('\n## Топ по include-циклам (структурный дрейф)\n')
L.append('| репа | циклов | крупнейший SCC | копипаст |\n|---|---|---|---|')
for r in top_cyc:
    L.append(f'| {r["repo"]} | {r["sccs"]} | {r["lscc"]} | {r["hits"]} |')
# разбивка копипаст-хитов по уверенности × scope (парс деталей)
if os.path.exists(DETAIL):
    dt=open(DETAIL).read()
    cnt=lambda s: dt.count(s)
    L.append('\n## Копипаст-кандидаты по уверенности × scope\n')
    L.append('| | within-file (в одном файле) | cross-file (между файлами) |\n|---|---|---|')
    L.append(f'| **verbatim** (line≥0.95, почти дословно) | {cnt("verbatim within-file")} | {cnt("verbatim cross-file")} |')
    L.append(f'| **near** (line 0.8–0.95) | {cnt("near within-file")} | {cnt("near cross-file")} |')
    L.append(f'| **diverged** (line<0.8, сильно правили) | {cnt("diverged within-file")} | {cnt("diverged cross-file")} |')
    L.append('\n> **verbatim within-file** = «скопировал функцию в том же файле» — целевой сигнал, высокая точность.\n'
             '> **diverged** — самый шумный класс (может быть coincidental), главный кандидат на FP.\n')
L.append('\n## Оговорки')
L.append('- Окно `--diff` 300 коммитов: для огромных реп ранний копипаст вне окна → счётчик занижен.')
L.append('- Копипаст per-commit атрибуция шумная; сами пары `file:line` точны — смотреть их.')
L.append('- Полный per-repo вердикт: `corpus_check_summary.tsv`. Детали по коммитам — ниже.\n')
L.append('---\n')

detail = open(DETAIL).read() if os.path.exists(DETAIL) else ''
open(OUT,'w').write('\n'.join(L)+'\n'+detail)
print(f'finalize: {tot} реп (clean={clean} drift={drift} partial={part}, циклы={cyc}, копипаст-реп={len(cp)}) -> {OUT}')
