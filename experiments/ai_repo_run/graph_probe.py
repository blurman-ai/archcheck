#!/usr/bin/env python3
"""Cheap "touchable" detectors over an exported archcheck include graph.

Input: a file written by `archcheck --save-graph-baseline <out> <repo>`
  nodes:
    - path/a.hpp        # index 0
    - path/b.hpp        # index 1
  edges:
    - [0, 1]            # node 0 includes node 1  (from -> to)

Every detector points at concrete files so a finding can be opened and checked.
Stdlib only. Iterative Tarjan (graphs reach a few thousand nodes).

Usage: graph_probe.py <graph_file> [repo_label]
"""
import sys
from collections import defaultdict


def parse(path):
    nodes, edges = [], []
    section = None
    with open(path, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            s = line.rstrip("\n")
            if s.startswith("nodes:"):
                section = "n"
                continue
            if s.startswith("edges:"):
                section = "e"
                continue
            t = s.strip()
            if not t or t in ("[]",):
                continue
            if section == "n" and t.startswith("- "):
                nodes.append(t[2:].strip().strip('"'))
            elif section == "e" and t.startswith("- ["):
                a, b = t[t.index("[") + 1 : t.index("]")].split(",")
                edges.append((int(a), int(b)))
    return nodes, edges


def tarjan(n, succ):
    """Iterative Tarjan SCC. Returns list of components (lists of node ids)."""
    index = [-1] * n
    low = [0] * n
    onstack = [False] * n
    stack = []
    comps = []
    counter = 0
    for root in range(n):
        if index[root] != -1:
            continue
        work = [(root, 0)]
        while work:
            v, pi = work[-1]
            if pi == 0:
                index[v] = low[v] = counter
                counter += 1
                stack.append(v)
                onstack[v] = True
            recurse = False
            succs = succ[v]
            i = pi
            while i < len(succs):
                w = succs[i]
                if index[w] == -1:
                    work[-1] = (v, i + 1)
                    work.append((w, 0))
                    recurse = True
                    break
                if onstack[w]:
                    low[v] = min(low[v], index[w])
                i += 1
            if recurse:
                continue
            if low[v] == index[v]:
                comp = []
                while True:
                    w = stack.pop()
                    onstack[w] = False
                    comp.append(w)
                    if w == v:
                        break
                comps.append(comp)
            work.pop()
            if work:
                pv = work[-1][0]
                low[pv] = min(low[pv], low[v])
    return comps


def longest_chain(n, succ, comp_id, ncomp):
    """Longest path (in nodes) over the SCC condensation; returns node-id path."""
    cadj = defaultdict(set)
    for v in range(n):
        for w in succ[v]:
            if comp_id[v] != comp_id[w]:
                cadj[comp_id[v]].add(comp_id[w])
    # topo via DFS, memoized longest path
    best_len = [0] * ncomp
    best_next = [-1] * ncomp
    order, seen = [], [0] * ncomp  # 0=unseen 1=active 2=done
    for s in range(ncomp):
        if seen[s]:
            continue
        st = [(s, False)]
        while st:
            c, post = st.pop()
            if post:
                bl, bn = 0, -1
                for d in cadj[c]:
                    if best_len[d] + 1 > bl:
                        bl, bn = best_len[d] + 1, d
                best_len[c], best_next[c] = bl, bn
                seen[c] = 2
                continue
            if seen[c]:
                continue
            seen[c] = 1
            st.append((c, True))
            for d in cadj[c]:
                if not seen[d]:
                    st.append((d, False))
    start = max(range(ncomp), key=lambda c: best_len[c]) if ncomp else 0
    # map comp -> a representative node id
    rep = {}
    for v in range(n):
        rep.setdefault(comp_id[v], v)
    path, c = [], start
    while c != -1:
        path.append(rep[c])
        c = best_next[c]
    return path


def ext(p):
    return p.rsplit(".", 1)[-1].lower() if "." in p else ""


def stem(p):
    base = p.rsplit("/", 1)[-1]
    return base.rsplit(".", 1)[0] if "." in base else base


def build_module_fn(nodes):
    """A 'module' is a top-level directory. If the whole repo sits under one top
    dir (e.g. everything under src/), drop one level so modules stay meaningful."""
    tops = set(p.split("/")[0] for p in nodes if "/" in p)
    single_top = len(tops) == 1

    def mod(path):
        parts = path.split("/")
        if len(parts) == 1:
            return "<root>"
        if single_top and len(parts) >= 3:
            return parts[0] + "/" + parts[1]
        return parts[0]

    return mod


def main():
    gf = sys.argv[1]
    label = sys.argv[2] if len(sys.argv) > 2 else gf
    nodes, edges = parse(gf)
    n = len(nodes)
    if n == 0:
        print(f"## {label}: empty graph")
        return
    succ = [[] for _ in range(n)]
    indeg = [0] * n
    eset = set()
    selfloops = []
    for a, b in edges:
        succ[a].append(b)
        indeg[b] += 1
        eset.add((a, b))
        if a == b:
            selfloops.append(a)
    outdeg = [len(s) for s in succ]

    comps = tarjan(n, succ)
    comp_id = [0] * n
    for cid, comp in enumerate(comps):
        for v in comp:
            comp_id[v] = cid
    cyclic = [c for c in comps if len(c) > 1 or (len(c) == 1 and c[0] in succ[c[0]])]
    cyclic.sort(key=len, reverse=True)

    # mutual include pairs (2-cycles)
    mutual = []
    for a, b in eset:
        if a < b and (b, a) in eset:
            mutual.append((a, b))

    print(f"## {label}")
    print(f"nodes={n} edges={len(edges)} edges/nodes={len(edges)/n:.2f} "
          f"cyclic_sccs={len(cyclic)} largest_scc={len(cyclic[0]) if cyclic else 0} "
          f"mutual_pairs={len(mutual)} selfloops={len(selfloops)}")

    # 1. largest cyclic SCC members
    if cyclic:
        big = cyclic[0]
        print(f"\n[CYCLE] largest SCC: {len(big)} components")
        for v in sorted(big, key=lambda x: nodes[x])[:12]:
            print(f"    {nodes[v]}")
        if len(big) > 12:
            print(f"    ... (+{len(big)-12} more)")

    # 2. mutual pairs classified
    if mutual:
        klass = defaultdict(list)
        for a, b in mutual:
            ea, eb = ext(nodes[a]), ext(nodes[b])
            k = "↔".join(sorted([ea or "?", eb or "?"]))
            klass[k].append((a, b))
        print(f"\n[MUTUAL] {len(mutual)} reciprocal include pairs, by extension:")
        for k in sorted(klass, key=lambda k: -len(klass[k])):
            ex = klass[k][0]
            print(f"    {k:>12}: {len(klass[k]):4d}   e.g. {nodes[ex[0]]} <-> {nodes[ex[1]]}")

    # 3. self-loops (file includes itself)
    if selfloops:
        print(f"\n[SELF] {len(selfloops)} self-include(s):")
        for v in selfloops[:5]:
            print(f"    {nodes[v]}")

    # 3b. backdoor includes: a header that includes a .cpp/.cc translation unit
    impl_ext = ("cpp", "cc", "cxx", "c++", "c")
    hdr_ext = ("h", "hpp", "hh", "hxx", "ipp", "inl", "tpp")
    backdoor = [(a, b) for a, b in eset
                if ext(nodes[a]) in hdr_ext and ext(nodes[b]) in impl_ext]
    if backdoor:
        print(f"\n[BACKDOOR] {len(backdoor)} header(s) including a .cpp/.cc:")
        for a, b in backdoor[:6]:
            print(f"    {nodes[a]}  ->  {nodes[b]}")
        if len(backdoor) > 6:
            print(f"    ... (+{len(backdoor)-6} more)")

    # 4. god-headers (fan-in) top
    top_in = sorted(range(n), key=lambda v: -indeg[v])[:5]
    print("\n[FAN-IN] top god-headers (included by N):")
    for v in top_in:
        print(f"    {indeg[v]:5d}  {nodes[v]}")

    # 5. god-components (fan-out) top
    top_out = sorted(range(n), key=lambda v: -outdeg[v])[:5]
    print("[FAN-OUT] top god-components (includes N project files):")
    for v in top_out:
        print(f"    {outdeg[v]:5d}  {nodes[v]}")

    # 6. longest include chain (actual path)
    path = longest_chain(n, succ, comp_id, len(comps))
    if len(path) >= 4:
        print(f"\n[CHAIN] longest include chain: depth {len(path)}")
        for v in path[:14]:
            print(f"    -> {nodes[v]}")
        if len(path) > 14:
            print(f"    -> ... (+{len(path)-14})")

    # 6b. module-level cycles (directory = module): circular dep BETWEEN modules,
    #     the thing user-declared rules are actually about. Path-only, cheap.
    modfn = build_module_fn(nodes)
    mod_names = sorted(set(modfn(p) for p in nodes))
    mod_id = {m: i for i, m in enumerate(mod_names)}
    nmod = len(mod_names)
    msucc = [set() for _ in range(nmod)]
    cross = 0
    mod_edge_count = defaultdict(int)
    for a, b in eset:
        ma, mb = mod_id[modfn(nodes[a])], mod_id[modfn(nodes[b])]
        if ma != mb:
            msucc[ma].add(mb)
            cross += 1
            mod_edge_count[(ma, mb)] += 1
    if nmod > 1:
        mcomps = tarjan(nmod, [sorted(s) for s in msucc])
        mcyc = sorted([c for c in mcomps if len(c) > 1], key=len, reverse=True)
        print(f"\n[MODULE] {nmod} modules, {cross} cross-module edges, {len(mcyc)} module-cycle(s)")
        for c in mcyc[:3]:
            print("    cycle (" + str(len(c)) + "): " + " <-> ".join(mod_names[x] for x in sorted(c)))
        for (ma, mb), cnt in sorted(mod_edge_count.items(), key=lambda kv: -kv[1])[:3]:
            print(f"    {cnt:4d} edges: {mod_names[ma]}  ->  {mod_names[mb]}")

    # 6c. encapsulation bypass: a file in module A includes module B's private dir
    #     (detail/internal/impl/private). Cross-module reach into internals.
    #     Split prod-source from test/example source: tests reaching into detail/
    #     is usually legitimate, prod->prod is the real boundary violation.
    priv = ("detail", "details", "internal", "internals", "impl", "private")
    testish = ("test", "tests", "testing", "example", "examples", "benchmark", "benchmarks", "bench",
               "sample", "samples", "demo", "demos")

    def is_testish(path):
        return any(s.lower() in testish for s in path.split("/")[:-1])

    prod_bypass, test_bypass = [], []
    for a, b in eset:
        if modfn(nodes[a]) == modfn(nodes[b]):
            continue  # same-module access to its own detail/ is fine
        segs = nodes[b].split("/")[:-1]
        if any(s.lower() in priv for s in segs):
            (test_bypass if is_testish(nodes[a]) else prod_bypass).append((a, b))
    if prod_bypass or test_bypass:
        print(f"\n[ENCAP] cross-module include into a private dir: "
              f"{len(prod_bypass)} prod->prod, {len(test_bypass)} test/example->prod")
        for a, b in prod_bypass[:6]:
            print(f"    [prod] {nodes[a]}  ->  {nodes[b]}")
        if len(prod_bypass) > 6:
            print(f"    ... (+{len(prod_bypass)-6} more prod)")
        if not prod_bypass:
            for a, b in test_bypass[:3]:
                print(f"    [test] {nodes[a]}  ->  {nodes[b]}")

    # 7. orphan headers (header never included by anything in project)
    orphans = [v for v in range(n)
               if indeg[v] == 0 and ext(nodes[v]) in ("h", "hpp", "hh", "hxx", "ipp", "inl")]
    if orphans:
        print(f"\n[ORPHAN] {len(orphans)} header(s) with fan-in 0 (dead or public entry):")
        for v in sorted(orphans, key=lambda x: nodes[x])[:6]:
            print(f"    {nodes[v]}")
        if len(orphans) > 6:
            print(f"    ... (+{len(orphans)-6} more)")


if __name__ == "__main__":
    main()
