#!/usr/bin/env python3
# Классификация dense-реп по drift-сигналу. Критерий пользователя:
# keep = большой сигнал ИЛИ пограничный (рост графа без циклов); remove только совсем плоские.
import csv, glob, os
rows=[]
for tsv in sorted(glob.glob('runs_history/dense/*.tsv')):
    name=os.path.basename(tsv)[:-4]
    snaps=[]
    with open(tsv) as f:
        for r in csv.DictReader(f, delimiter='\t'):
            try: snaps.append({k:float(r[k]) for k in ('dup_ratio','nodes','sccs_cyclic','largest_scc')})
            except: pass
    if not snaps: 
        rows.append((name,'NO-DATA',0,0,0,0,0)); continue
    dup=[s['dup_ratio'] for s in snaps]; nodes=[s['nodes'] for s in snaps]
    cyc=[s['sccs_cyclic'] for s in snaps]; scc=[s['largest_scc'] for s in snaps]
    dup_d=dup[-1]-dup[0]; node_d=nodes[-1]-nodes[0]
    cyc_ever=max(cyc)>0; scc_ever=max(scc)>=2
    # критерий keep (лёгкий, держим пограничные)
    keep = (dup_d>=3) or cyc_ever or scc_ever or (node_d>=5) or (dup[-1]>=25)
    # причина
    why=[]
    if dup_d>=3: why.append(f'dup+{dup_d:.0f}')
    if dup[-1]>=25: why.append(f'dup={dup[-1]:.0f}%')
    if cyc_ever: why.append(f'cyc≤{int(max(cyc))}')
    if scc_ever: why.append(f'scc≤{int(max(scc))}')
    if node_d>=5: why.append(f'nodes+{int(node_d)}')
    label='KEEP' if keep else 'FLAT'
    rows.append((name,label,round(dup[0],1),round(dup[-1],1),round(dup_d,1),int(node_d),','.join(why) or 'плоско'))
rows.sort(key=lambda x:(x[1]!='KEEP', -x[4]))
print(f"{'repo':38} {'lbl':5} {'d0':>5} {'d1':>5} {'Δdup':>5} {'Δnod':>5}  why")
for r in rows: print(f"{r[0][:38]:38} {r[1]:5} {r[2]:>5} {r[3]:>5} {r[4]:>5} {r[5]:>5}  {r[6]}")
keep=sum(1 for r in rows if r[1]=='KEEP'); flat=sum(1 for r in rows if r[1]=='FLAT')
print(f"\nИТОГ: KEEP={keep}  FLAT(remove)={flat}  NO-DATA={sum(1 for r in rows if r[1]=='NO-DATA')}  всего={len(rows)}")
