#!/usr/bin/env python3
# Clone-level overlap: archcheck pairs vs NiCad clone classes, per repo.
# Buckets: BOTH (archcheck pair covered by a NiCad class), ONLY_ARCH, ONLY_NICAD (split vendored/authored).
# Emits markdown with GitHub permalinks.
import re, sys, os, importlib.util, json

spec=importlib.util.spec_from_file_location('p','/home/localadm/nicad_work/parse_classes.py')
P=importlib.util.module_from_spec(spec); spec.loader.exec_module(P)

REPO_GH = {
 'Catch2':('catchorg/Catch2','69e0473f6e98d47c93518424c08ee69ee632c0f0'),
 'LibreSprite':('LibreSprite/LibreSprite','276fdbdb27b537a074c3e170af6afc88c244a539'),
 'BambuStudio':('bambulab/BambuStudio','a206a95399dc6a2d4c918e43f2a5b889bd1a1d83'),
 'AetherSDR':('aethersdr/AetherSDR','44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa'),
 'GWToolboxpp':('gwdevhub/GWToolboxpp','c8c0faa410206918e6f56a4dcc89f9159b615694'),
 'Kartend':('EtherAura/Kartend','54bfbdfc29c0387b466ac43fcff28e960a35dbd0'),
 'IrredenEngine':('jakildev/IrredenEngine','3a825a50d84a2ce5dda505531a72429d812a795c'),
 'hyyh619_irrlicht-1.8.3':('hyyh619/irrlicht-1.8.3','4280a27f7611b7ce4cda877e91d7d831b0b8edfe'),
}

# Comprehensive vendored / bundled-lib detector (paths seen in this corpus).
VEND = re.compile(r'(third[_-]?party|/?dependencies/|/deps/|/extern|/external/|/vendor|jpeglib|libpng|/png|bzip2|bzlib|gregbook|pngminus|zlib|miniz|minilzo|nativefiledialog|/qhull/|/libigl/|/mcut/|/hidapi/|/glu-libtess/|/agg/|/clipper|/admesh|/nanosvg|/nanovg|imgui|/glad|/glfw|/glm/|freetype|expat|/curl|openssl|/tinyxml|/cereal|/catch_?|/Catch2/|liquid-dsp|r8brain|mosquitto|libspecbleach|fixmath|/boost|/eigen|/spdlog|/fmt/|/json|/Shiny|/poly2tri|/clipper2|/igl/)', re.I)

def is_vendored(path): return bool(VEND.search('/'+path))

def gh(repo, path, lo, hi):
    owner,commit=REPO_GH[repo]
    return f"https://github.com/{owner}/blob/{commit}/{path}#L{lo}-L{hi}".replace('/blob/', f"/blob/") if False else f"https://github.com/{owner}/blob/{commit}/{path}#L{lo}-L{hi}"

def ghlink(repo, path, lo, hi):
    owner,commit=REPO_GH[repo]
    return f"https://github.com/{owner}/blob/{commit}/{path}#L{lo}-L{hi}"

def parse_arch(txtfile):
    pairs=[]
    if not os.path.exists(txtfile): return pairs
    for ln in open(txtfile, errors='replace'):
        m=re.match(r'\s*(\S+):(\d+)-(\d+)\s+<->\s+(\S+):(\d+)-(\d+)\s+\(([^,]+),\s*weighted=([0-9.]+),\s*line=([0-9.]+)\)', ln)
        if m:
            fa,a1,a2,fb,b1,b2,typ,w,li=m.groups()
            pairs.append({'fa':fa,'a1':int(a1),'a2':int(a2),'fb':fb,'b1':int(b1),'b2':int(b2),
                          'type':typ,'w':float(w),'line':float(li)})
    return pairs

def overlap(f1,l1,h1,f2,l2,h2):
    if f1!=f2: return False
    inter=min(h1,h2)-max(l1,l2)
    if inter<0: return False
    small=min(h1-l1,h2-l2)+1
    return inter+1 >= max(3, 0.30*small)

def load_nicad(repo):
    classes=[]
    for g in ('functions','blocks'):
        x=f"/home/localadm/nicad_work/farm_{repo}_{g}-blind-clones"
        import glob
        fs=glob.glob(x+"/*classes-withsource.xml")
        if fs and os.path.getsize(fs[0])>0:
            for c in P.main(fs[0]):
                c['gran']=g; classes.append(c)
    return classes

def class_covers_pair(c, pr):
    hitA=hitB=False
    for r,a,b in c['members']:
        if overlap(r,a,b, pr['fa'],pr['a1'],pr['a2']): hitA=True
        if overlap(r,a,b, pr['fb'],pr['b1'],pr['b2']): hitB=True
    return hitA and hitB

def main(repo):
    arch=parse_arch(f"/home/localadm/projects/cpparch/reports/raw/archcheck_{repo}.txt")
    nic=load_nicad(repo)
    # bucket archcheck pairs
    both=[]; only_arch=[]
    matched_classes=set()
    for pr in arch:
        hit=None
        for i,c in enumerate(nic):
            if class_covers_pair(c,pr): hit=i; break
        if hit is not None: both.append(pr); matched_classes.add(id(nic[hit]))
        else: only_arch.append(pr)
    # only-nicad classes (not matching any archcheck pair)
    only_nic=[c for c in nic if id(c) not in matched_classes]
    onv=[c for c in only_nic if all(is_vendored(r) for r,_,_ in c['members'])]
    ona=[c for c in only_nic if not all(is_vendored(r) for r,_,_ in c['members'])]
    return {'repo':repo,'arch_total':len(arch),'nicad_classes':len(nic),
            'both':both,'only_arch':only_arch,'only_nicad_vendored':onv,'only_nicad_authored':ona}

if __name__=='__main__':
    repo=sys.argv[1]; mode=sys.argv[2] if len(sys.argv)>2 else 'counts'
    r=main(repo)
    if mode=='counts':
        print(f"{repo}: archcheck={r['arch_total']} nicad_classes={r['nicad_classes']} | BOTH={len(r['both'])} ONLY_ARCH={len(r['only_arch'])} ONLY_NICAD_vendored={len(r['only_nicad_vendored'])} ONLY_NICAD_authored={len(r['only_nicad_authored'])}")
    elif mode=='authored':
        print(f"== {repo}: ONLY-NiCad AUTHORED classes ({len(r['only_nicad_authored'])}) ==")
        for c in sorted(r['only_nicad_authored'],key=lambda c:c['nlines']*c['size'],reverse=True)[:25]:
            print(f"  #{c['classid']}({c['gran']}) size={c['size']} nlines={c['nlines']} sim={c['similarity']}")
            for rr,a,b in c['members'][:3]: print(f"      {rr}:{a}-{b}")
    elif mode=='both':
        print(f"== {repo}: BOTH ({len(r['both'])}) ==")
        for pr in r['both'][:25]:
            print(f"  {pr['fa']}:{pr['a1']}-{pr['a2']} <-> {pr['fb']}:{pr['b1']}-{pr['b2']} ({pr['type']})")
    elif mode=='onlyarch':
        print(f"== {repo}: ONLY-ARCHCHECK ({len(r['only_arch'])}) ==")
        for pr in sorted(r['only_arch'],key=lambda p:-(p['a2']-p['a1']))[:25]:
            v='V' if (is_vendored(pr['fa']) or is_vendored(pr['fb'])) else 'A'
            print(f"  [{v}] {pr['fa']}:{pr['a1']}-{pr['a2']} <-> {pr['fb']}:{pr['b1']}-{pr['b2']} ({pr['type']} w={pr['w']})")
    elif mode=='md':
        n=int(sys.argv[3]) if len(sys.argv)>3 else 8
        def L(path,lo,hi):
            return f"[`{path}:{lo}-{hi}`]({ghlink(repo,path,lo,hi)})"
        print(f"#### {repo}")
        print(f"_archcheck pairs: {r['arch_total']} · NiCad classes: {r['nicad_classes']} · "
              f"**BOTH {len(r['both'])}** · **only-archcheck {len(r['only_arch'])}** · "
              f"only-NiCad vendored {len(r['only_nicad_vendored'])} · only-NiCad authored {len(r['only_nicad_authored'])}_\n")
        if r['both']:
            print("**Found by BOTH** (sample):\n")
            for pr in r['both'][:n]:
                print(f"- {L(pr['fa'],pr['a1'],pr['a2'])} ↔ {L(pr['fb'],pr['b1'],pr['b2'])} ({pr['type']})")
            print()
        print("**Only archcheck** (top by size; A=authored, V=vendored):\n")
        for pr in sorted(r['only_arch'],key=lambda p:-(p['a2']-p['a1']))[:n]:
            v='V' if (is_vendored(pr['fa']) or is_vendored(pr['fb'])) else 'A'
            print(f"- [{v}] {L(pr['fa'],pr['a1'],pr['a2'])} ↔ {L(pr['fb'],pr['b1'],pr['b2'])} ({pr['type']}, w={pr['w']})")
        print()
        if r['only_nicad_authored']:
            print("**Only NiCad — authored** (candidate value):\n")
            for c in sorted(r['only_nicad_authored'],key=lambda c:c['nlines']*c['size'],reverse=True)[:n]:
                ms=" ↔ ".join(L(rr,a,b) for rr,a,b in c['members'][:3])
                print(f"- ({c['gran']}, size {c['size']}, {c['nlines']}ln, sim {c['similarity']}) {ms}")
            print()
        if r['only_nicad_vendored']:
            print("**Only NiCad — vendored** (noise for an arch gate; sample):\n")
            for c in sorted(r['only_nicad_vendored'],key=lambda c:c['nlines']*c['size'],reverse=True)[:4]:
                ms=" ↔ ".join(L(rr,a,b) for rr,a,b in c['members'][:2])
                print(f"- ({c['gran']}, size {c['size']}) {ms}")
            print()
