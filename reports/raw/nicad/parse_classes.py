#!/usr/bin/env python3
# Parse a NiCad *-classes-withsource.xml into structured clone-class records.
# Emits: classid, size(nclones), nlines, similarity, cross_module(bool), vendored(bool),
#        distinct_files, member list (decoded original paths).
import re, sys, os, json

VENDOR = re.compile(r'(jpeglib|bzip2|bzlib|gregbook|pngminus|contrib/|zlib|libpng|/png|miniz|/deps/|third[_-]?party|3rd|/extern|/external/|/vendor|imgui|/lib/|expat|boost|/aMule|nanosvg|stb_|/glad|glfw|freetype|/curl|openssl|/tinyxml|/json|cereal|/tools/|/test|catch_)', re.I)

def decode(p):
    # farm path: /home/localadm/nicad_work/farm_<repo>/<relpath>.c  -> <relpath>
    m = re.sub(r'^.*?/farm_[^/]+/', '', p)
    return re.sub(r'\.c$', '', m)

def topdir(rel):
    parts = rel.split('/')
    return parts[0] if len(parts) > 1 else '.'

def main(xml):
    txt = open(xml, encoding='utf-8', errors='replace').read()
    classes = []
    for m in re.finditer(r'<class classid="(\d+)" nclones="(\d+)" nlines="(\d+)" similarity="(\d+)">(.*?)</class>', txt, re.S):
        cid, n, nl, sim, body = m.groups()
        srcs = []
        for s in re.finditer(r'<source file="([^"]+)" startline="(\d+)" endline="(\d+)"', body):
            f, a, b = s.groups()
            rel = decode(f)
            srcs.append((rel, int(a), int(b)))
        dirs = set(topdir(r) for r, _, _ in srcs)
        files = set(r for r, _, _ in srcs)
        vend = any(VENDOR.search(r) for r, _, _ in srcs)
        classes.append({
            'classid': int(cid), 'size': int(n), 'nlines': int(nl), 'similarity': int(sim),
            'cross_module': len(dirs) > 1, 'distinct_files': len(files),
            'vendored': vend, 'dirs': sorted(dirs), 'members': srcs,
        })
    return classes

if __name__ == '__main__':
    xml = sys.argv[1]
    mode = sys.argv[2] if len(sys.argv) > 2 else 'summary'
    cs = main(xml)
    if mode == 'summary':
        tot = len(cs)
        vend = sum(1 for c in cs if c['vendored'])
        xmod = sum(1 for c in cs if c['cross_module'])
        xmod_auth = sum(1 for c in cs if c['cross_module'] and not c['vendored'])
        print(f"classes={tot} vendored={vend} cross_module={xmod} cross_module_authored={xmod_auth}")
    elif mode == 'top':
        for c in sorted(cs, key=lambda c: (c['nlines']*c['size']), reverse=True)[:20]:
            tag = 'VENDOR' if c['vendored'] else ('XMOD' if c['cross_module'] else 'local')
            print(f"#{c['classid']} size={c['size']} nlines={c['nlines']} sim={c['similarity']} {tag} dirs={c['dirs']}")
            for r,a,b in c['members'][:4]:
                print(f"     {r}:{a}-{b}")
    elif mode == 'xmod':
        xs = [c for c in cs if c['cross_module']]
        for c in sorted(xs, key=lambda c: (c['nlines']*c['size']), reverse=True)[:20]:
            tag = 'VENDOR' if c['vendored'] else 'authored'
            print(f"#{c['classid']} size={c['size']} nlines={c['nlines']} sim={c['similarity']} {tag} dirs={c['dirs']}")
            for r,a,b in c['members'][:4]:
                print(f"     {r}:{a}-{b}")
    elif mode == 'json':
        print(json.dumps(cs))
