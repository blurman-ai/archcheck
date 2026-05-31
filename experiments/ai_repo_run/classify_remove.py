#!/usr/bin/env python3
"""Классификатор корпуса для чистки: keep / remove(no-signal) / remove(false).

Критерий пользователя (#054): убрать с диска клоны, на которых archcheck
(дубли + граф) НИЧЕГО не нашёл или нашёл ТОЛЬКО ложное (вендоринг,
fork-in-repo, data-blob, мета-агрегатор, codegen, competitive-шаблон),
включая пограничные. Источник истины — TSV-сводки прогонов, не консоль.
"""
import csv, os

BASE = os.path.dirname(os.path.abspath(__file__))

def read_tsv(path):
    with open(path, encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))

def fnum(x):
    try: return float(x)
    except (ValueError, TypeError): return 0.0

# --- ручные оверрайды ----------------------------------------------------
# Документированные АРТЕФАКТЫ / только-ложное (см. DRIFT_RUN_REPORT §3,§7.5,
# METHODOLOGY): force remove с причиной.
FALSE = {
    "CnC_Generals_Zero_Hour": "fork-in-repo (Generals/ vs GeneralsMD/), 90% dup стабилен — дублирование-by-design, не дрейф (§7.5)",
    "GacUI": "codegen, 70% dup стабилен (Δ-0.16) — генерёнка, не дрейф (§7.5)",
    "ReP_AL-3D-Lawn-Mower": "data-blob camera_index.h (546 строк hex) + ESP32 boilerplate — шум-класс #056, не код (§3,§7.5)",
    "GSL": "12.5→0 — header-only реструктуризация, фантом-«починка», не реальный спад (§7.5)",
    "SDRPlusPlusBrown": "19.8→0 при nodes→1 — смена структуры/субпуть, фантом, не починка (§7.5)",
    "mqtt_client": "весь dup = header↔impl одной компоненты (2 файла) — cross-file proxy, не архитектурный дубль (README §6)",
    "rii": "dup был раздут вендоренным extern/sse2neon.h; после excludes — плоско 3.6→4.0, сигнала нет",
    "ggml-org-central": "мета-агрегатор (llama.cpp+whisper.cpp+ggml в одной папке), вендоринг-агрегация, не одна репа (§3)",
}
# Документированные РЕАЛЬНЫЕ контроли/примеры — force keep даже если пороги
# формально не срабатывают (реальный дубль/дедуп кода, см. отчёты).
KEEP = {
    "ncnn": "SIMD-бэкенды loongarch/mips/arm — РЕАЛЬНЫЙ параллельный код, контрольная группа (§3bis,§7.5)",
    "Effekseer": "реальная дедупликация vertex-буферов (§9 DEDUP), несмотря на shader-шум",
    "opencv-mobile": "флагман копипаст-дрейфа: JPEG-энкодер cix↔rpi 600 строк (§7.1)",
    "sentry-unreal": "копипаст платформенных реализаций ISentrySubsystem (§7.1)",
}

def classify_highstar(r):
    repo = r["repo"]
    if repo in FALSE:  return "remove", "false", FALSE[repo]
    if repo in KEEP:   return "keep", "real-doc", KEEP[repo]
    dd  = fnum(r["dup_delta"]); dl = fnum(r["dup_last"])
    cl  = fnum(r["cyc_last"]); cd = fnum(r["cyc_delta"])
    ef  = fnum(r["edges_first"]); el = fnum(r["edges_last"])
    coupling = el >= 1000 and el / (ef + 1) >= 3.0
    if cl >= 2:            return "keep", "cycles", f"циклы есть (cyc_last={cl:.0f})"
    if cd >= 2:            return "keep", "cycles", f"циклы родились/выросли (Δcyc=+{cd:.0f})"
    if dd >= 5:            return "keep", "dup-growth", f"реальный рост дублей (Δ=+{dd:.1f} п.п.)"
    if dd <= -5:          return "keep", "dup-reversal", f"реальный спад дублей — контрпример (Δ={dd:.1f} п.п.)"
    if coupling:          return "keep", "coupling", f"рост связности (edges {ef:.0f}→{el:.0f})"
    if dl >= 12:           return "keep", "abs-dup", f"высокий абс. dup {dl:.0f}% — реальное срабатывание (хоть и плоско)"
    return "remove", "no-signal", f"dup плоско {dl:.0f}% (Δ={dd:+.1f}), циклов нет, связность не растёт"

def classify_lowstar(r):
    repo = r["repo"]
    if repo in FALSE:  return "remove", "false", FALSE[repo]
    dd = fnum(r["dup_delta"]); dl = fnum(r["dup_last"]); cl = fnum(r["cyc_last"])
    nodes = fnum(r.get("nodes_last", 0))
    if cl >= 2:   return "keep", "cycles", f"циклы есть (cyc_last={cl:.0f})"
    if dd >= 8:   return "keep", "dup-growth", f"заметный рост дублей (Δ=+{dd:.1f} п.п.)"
    if dd <= -8:  return "keep", "dup-reversal", f"заметный спад дублей (Δ={dd:.1f} п.п.)"
    if dl >= 15:  return "keep", "abs-dup", f"высокий абс. dup {dl:.0f}% — реальное срабатывание"
    return "remove", "no-signal", f"dup {dl:.0f}% (Δ={dd:+.1f}), циклов нет (nodes={nodes:.0f}) — нет сигнала"

OUT = []
REMOVE_ROWS = []  # (repo, corpus, tag, reason)
def dump(title, rows, fn, corpus):
    keep, rem = [], []
    for r in rows:
        dec, tag, why = fn(r)
        if dec == "remove":
            rem.append((r["repo"], tag, why)); REMOVE_ROWS.append((r["repo"], corpus, tag, why))
        else:
            keep.append((r["repo"], tag, why))
    OUT.append(f"\n===== {title}: keep={len(keep)} remove={len(rem)} =====")
    OUT.append("--- REMOVE ---")
    for repo, tag, why in sorted(rem):
        OUT.append(f"  [{tag:12}] {repo}\t{why}")
    OUT.append("--- KEEP ---")
    for repo, tag, why in sorted(keep):
        OUT.append(f"  [{tag:12}] {repo}\t{why}")
    return [x[0] for x in rem], [x[0] for x in keep]

hs = read_tsv(os.path.join(BASE, "runs_history/drift_summary_v2.tsv"))
ls = read_tsv(os.path.join(BASE, "runs_history/drift_summary_lowstar.tsv"))
rem_hs, keep_hs = dump("HIGH-STAR (_aidev_run)", hs, classify_highstar, "_aidev_run")
rem_ls, keep_ls = dump("LOW-STAR (_aidev_lowstar)", ls, classify_lowstar, "_aidev_lowstar")
with open(os.path.join(BASE, "_remove_plan.tsv"), "w", encoding="utf-8") as f:
    f.write("repo\tcorpus\ttag\treason\n")
    for repo, corpus, tag, why in sorted(REMOVE_ROWS):
        f.write(f"{repo}\t{corpus}\t{tag}\t{why}\n")
OUT.append(f"\n===== ИТОГО remove: high={len(rem_hs)} low={len(rem_ls)} всего={len(rem_hs)+len(rem_ls)} =====")

# машиночитаемые списки для удаления
with open(os.path.join(BASE, "_classify_report.txt"), "w", encoding="utf-8") as f:
    f.write("\n".join(OUT) + "\n")
with open(os.path.join(BASE, "_remove_highstar.txt"), "w", encoding="utf-8") as f:
    f.write("\n".join(sorted(rem_hs)) + "\n")
with open(os.path.join(BASE, "_remove_lowstar.txt"), "w", encoding="utf-8") as f:
    f.write("\n".join(sorted(rem_ls)) + "\n")
