#!/usr/bin/env python3
# Emit authoritative reports/nicad_metrics.csv from run logs + clone XML.
import csv, glob, os, importlib.util
spec=importlib.util.spec_from_file_location('p','/home/localadm/nicad_work/parse_classes.py')
P=importlib.util.module_from_spec(spec); spec.loader.exec_module(P)
specm=importlib.util.spec_from_file_location('m','/home/localadm/nicad_work/matcher.py')
M=importlib.util.module_from_spec(specm); specm.loader.exec_module(M)

COMMIT={'Catch2':'69e0473','LibreSprite':'276fdbd','BambuStudio':'a206a95','AetherSDR':'44ce91c',
 'GWToolboxpp':'c8c0faa','Kartend':'54bfbdf','IrredenEngine':'3a825a5','hyyh619_irrlicht-1.8.3':'4280a27'}

# read run CSV
runs={}
for row in csv.DictReader(open('/home/localadm/nicad_work/metrics_raw.csv')):
    runs[(row['repo'],row['gran'])]=row

out=open('/home/localadm/projects/cpparch/reports/nicad_metrics.csv','w',newline='')
w=csv.writer(out)
w.writerow(['repo','commit','loc','granularity','wall_s','peak_mb','parse_fail_pct',
            'clone_pairs','clone_classes','largest_class','vendored_classes','authored_classes',
            'report_bytes','status'])
order=['Catch2','GWToolboxpp','Kartend','IrredenEngine','LibreSprite','hyyh619_irrlicht-1.8.3','AetherSDR','BambuStudio']
for repo in order:
    for g in ('functions','blocks'):
        r=runs.get((repo,g))
        if not r: continue
        xmls=glob.glob(f"/home/localadm/nicad_work/farm_{repo}_{g}-blind-clones/*classes-withsource.xml")
        cls=largest=vend=auth=0
        if xmls and os.path.getsize(xmls[0])>0:
            cs=P.main(xmls[0]); cls=len(cs)
            if cs: largest=max(c['size'] for c in cs)
            vend=sum(1 for c in cs if all(M.is_vendored(r0) for r0,_,_ in c['members'])); auth=cls-vend
        pres=int(r['presented']); pf=int(r['parsefail'])
        pfpct=round(100*pf/pres,1) if pres else 0
        w.writerow([repo,COMMIT[repo],r['loc'],g,r['wall_s'],r['peak_mb'],pfpct,
                    r['pairs'],cls,largest,vend,auth,r['report_bytes'],r['status']])
out.close()
print(open('/home/localadm/projects/cpparch/reports/nicad_metrics.csv').read())
