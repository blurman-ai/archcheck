#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import re
import subprocess
from dataclasses import dataclass
from pathlib import Path


EXTS = {
    ".c",
    ".cc",
    ".cpp",
    ".cxx",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
    ".ipp",
    ".tpp",
    ".inl",
    ".inc",
}

ROOTS = [
    Path("~/oss"),
    Path("~/projects"),
]

EXTRA_SCOPES = [
    Path("~/oss/fmt/include"),
    Path("~/oss/spdlog/include"),
]

SCRIPT_DIR = Path(__file__).resolve().parent
REPORT_PATH = SCRIPT_DIR / "PROJECT_EXAMPLES_REPORT.md"
EXAMPLES_DIR = SCRIPT_DIR / "examples"
DEFAULT_BINARY = Path("/tmp/line_dup_build/line_duplication")

COMMON_REPORT_EXCLUDES = [
    "examples",
    "_deps",
    "CMakeFiles",
    "CompilerIdC",
    "CompilerIdCXX",
]

SKIP_REPOS = {
    "gm_github",
}


MANUAL_NOTES = {
    "abseil-cpp": "Параллельные реализации linked_hash_map/linked_hash_set. Реальный текстовый дубль, но выглядит как осознанный twin-контейнер.",
    "AetherSDR": "ClientCompEditor и StripCompPanel содержат почти дословный UI/Qt twin. Это не генерация, а реальный ручной copy-paste-кандидат.",
    "BambuStudio": "GLGizmoAdvancedCut и GLGizmoColorCut делят большой ImGui-блок. Находка валидная и архитектурно интересная.",
    "Catch2": "После исключения extras/amalgam остался twin-код двух репортеров: junit и sonarqube. Это не packaging-артефакт, а намеренно параллельная реализация.",
    "cpparch": "Небольшой реальный дубль helper-функции jsonEscape между reporter и baseline-кодом. Сигнал валиден, но локальный по масштабу.",
    "fmt": "После выключения test-like файлов остался единичный cross-file блок у границы порога; сильного архитектурного сигнала нет.",
    "fmt_include": "Ручная проверка не нужна: scope-reference, cross-file блоков нет.",
    "folly": "SpookyHashV1 и SpookyHashV2 делят одну алгоритмическую основу. Это intentional version-fork, а не подозрительный копипаст.",
    "gm": "encoder.cpp и encoder2.cpp делят большой ручной дубль таблиц и setup-логики для variant-specific устройства.",
    "grpc": "xds_server_config_fetcher и xds_server_config_fetcher_legacy почти совпадают. Это intentional legacy-fork, не автогенерация.",
    "GWToolboxpp": "В TextUtils и PluginUtils продублирован большой справочник диакритики. Это реальный copy-paste shared data/helper-кода.",
    "IrredenEngine": "main_mouse и main_static делят bootstrap одной demo-сцены. Это intentional demo twin с небольшими отличиями по режиму запуска.",
    "Kartend": "mainwindow_dialogs.cpp и mainwindow_setup.cpp содержат большой копипаст методов MainWindow. Это реальный ручной дубль, не generated.",
    "LibreSprite": "cmd_move_mask и cmd_scroll делят одинаковую логику форматирования текста команды. Похоже на intentional command twin, который можно было бы вынести в helper.",
    "moqx": "MoqxPicoRelayServer и MoqxRelayServer имеют почти одинаковый интерфейс под два транспорта. Это intentional transport twin, не шум генерации.",
    "nlohmann_json": "Дубль между detail-header и umbrella json.hpp. Это скорее packaging/public-header aggregation artifact, чем ручной архитектурный долг.",
    "OreStudio": "iam_routes.cpp в http.core и http.server выглядят как почти полные file-copy mirror-слои. Это реальный дубль, не generated.",
    "pico-sdk": "gen4_rp2350_24.h и gen4_rp2350_24ct.h почти совпадают как board-variant headers. Это intentional hardware-family twin.",
    "samastra_itain": "bat_diagwindow и imr_diagwindow делят большой Qt/UI+UDP twin-блок. Похоже на ручной copy-paste между вариантами изделия.",
    "skyrim-community-shaders": "Логика разворачивания weather flags продублирована между WeatherEditor и Utils/Game. Это валидный helper-duplication, а не include-list шум.",
    "spectre": "GhValenciaDivClean и ValenciaDivClean делят один и тот же reconstruction path. Это intentional scientific/system twin.",
    "vmecpp": "fourier_basis_fast_poloidal и fourier_basis_fast_toroidal делят одинаковую математику conversion indices. Это intentional basis twin.",
    "spdlog": "Топовый блок daily/hourly вручную проверен. Это валидный архитектурный дубль между двумя sink-реализациями.",
    "spdlog_include": "Ручная проверка: валидные дубли daily/hourly и syslog/systemd.",
}

MANUAL_STATUS = {
    "abseil-cpp": "вручную проверено",
    "AetherSDR": "вручную проверено",
    "BambuStudio": "вручную проверено",
    "Catch2": "вручную проверено",
    "cpparch": "вручную проверено",
    "fmt": "частично вручную проверено",
    "fmt_include": "ручная проверка не требуется",
    "folly": "вручную проверено",
    "gm": "вручную проверено",
    "grpc": "вручную проверено",
    "GWToolboxpp": "вручную проверено",
    "IrredenEngine": "вручную проверено",
    "Kartend": "вручную проверено",
    "LibreSprite": "вручную проверено",
    "moqx": "вручную проверено",
    "nlohmann_json": "вручную проверено",
    "OreStudio": "вручную проверено",
    "pico-sdk": "вручную проверено",
    "samastra_itain": "вручную проверено",
    "skyrim-community-shaders": "вручную проверено",
    "spectre": "вручную проверено",
    "spdlog": "вручную проверено",
    "spdlog_include": "вручную проверено",
    "vmecpp": "вручную проверено",
}

PREFERRED_MATCHES = {
    "skyrim-community-shaders": [
        ("src/Features/WeatherEditor.cpp", "src/Utils/Game.cpp"),
    ],
    "spdlog": [
        ("include/spdlog/sinks/daily_file_sink.h", "include/spdlog/sinks/hourly_file_sink.h"),
    ],
}

SCOPE_EXTRA_EXCLUDES = {
    "Catch2": ["extras", "examples"],
    "BambuStudio": ["minilzo", "qhull", "deps", "agg", "libigl"],
    "IrredenEngine": ["glad", "gl_wrap"],
    "OreStudio": [
        "projects/ores.ore/src/domain/*",
        "projects/ores.ore.core/src/domain/*",
        "projects/ores.ore/include/ores.ore/domain/*",
        "projects/ores.ore.core/include/ores.ore.core/domain/*",
    ],
    "cpparch": ["sandbox"],
    "gm": ["thirdparty", "3rdparty", "upgrade", "codegenTemplates", "build-debug", "build-release", "bin", "bin_community"],
    "grpc": [],
    "nlohmann_json": ["single_include"],
    "pico-sdk": ["hardware_regs", "hardware_structs", "CMSIS"],
    "samastra_itain": [
        "build-debug",
        "build_bat",
        "build_imr",
        "rmi_autogen",
        "mdl_autogen",
        "yaml-cpp_autogen",
        "CMakeFiles",
        "CompilerIdCXX",
    ],
    "spdlog": ["build-cc", "_deps"],
}


@dataclass
class Scope:
    key: str
    title: str
    root: Path
    repo_root: Path


@dataclass
class Block:
    tag: str
    length: int
    file_a: str
    start_a: int
    file_b: str
    start_b: int


def run(cmd: list[str]) -> str:
    return subprocess.check_output(cmd, text=True)


def normalize(raw: str) -> str:
    return " ".join(raw.strip().split())


def is_comment_only(raw: str) -> bool:
    trimmed = raw.strip()
    return trimmed.startswith("//") or trimmed.startswith("/*") or trimmed.startswith("*") or trimmed.startswith("*/")


def is_significant(raw: str) -> bool:
    return bool(raw.strip()) and not is_comment_only(raw)


def similarity(fragment_a: list[str], fragment_b: list[str]) -> float:
    norm_a = [normalize(line) for line in fragment_a if is_significant(line)]
    norm_b = [normalize(line) for line in fragment_b if is_significant(line)]
    if not norm_a and not norm_b:
      return 100.0
    size = max(len(norm_a), len(norm_b))
    if size == 0:
      return 100.0
    matched = sum(1 for a, b in zip(norm_a, norm_b) if a == b)
    return 100.0 * matched / size


def signal_weight(length: int) -> str:
    if length >= 50:
        return "очень высокий"
    if length >= 20:
        return "высокий"
    if length >= 10:
        return "средний"
    return "низкий"


def discover_repo_scopes() -> list[Scope]:
    scopes: list[Scope] = []
    seen: set[str] = set()

    for root in ROOTS:
        if not root.is_dir():
            continue
        for entry in sorted(root.iterdir()):
            if not entry.is_dir():
                continue
            if entry.name in SKIP_REPOS:
                continue
            if not ((entry / ".git").exists() or (entry / ".hg").exists()):
                continue
            count = count_cpp_files(entry)
            if count == 0:
                continue
            key = entry.name.replace(" ", "_").replace("/", "_")
            seen.add(str(entry))
            scopes.append(Scope(key=key, title=entry.name, root=entry, repo_root=entry))

    for extra in EXTRA_SCOPES:
        if not extra.is_dir():
            continue
        repo_root = find_repo_root(extra)
        if repo_root is None:
            continue
        key = extra.name if extra.name != repo_root.name else extra.name
        if extra.name == "include" and repo_root.name == "fmt":
            scope_key = "fmt_include"
            title = "fmt/include"
        elif extra.name == "include" and repo_root.name == "spdlog":
            scope_key = "spdlog_include"
            title = "spdlog/include"
        else:
            scope_key = f"{repo_root.name}_{extra.name}"
            title = f"{repo_root.name}/{extra.name}"
        scopes.append(Scope(key=scope_key, title=title, root=extra, repo_root=repo_root))

    scopes.sort(key=lambda s: s.title.lower())
    return scopes


def count_cpp_files(root: Path) -> int:
    count = 0
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [
            d
            for d in dirnames
            if d not in {".git", ".hg", "build", "out", ".cache", ".idea", ".vscode"} and not d.startswith("cmake-build-")
        ]
        for fn in filenames:
            if Path(fn).suffix.lower() in EXTS:
                count += 1
    return count


def find_repo_root(path: Path) -> Path | None:
    current = path
    while True:
        if (current / ".git").exists():
            return current
        if current.parent == current:
            return None
        current = current.parent


def parse_summary(stdout: str) -> dict[str, float | int | str]:
    def grab(pattern: str, cast):
        match = re.search(pattern, stdout)
        return cast(match.group(1)) if match else None

    top_blocks: list[Block] = []
    for line in stdout.splitlines():
        match = re.match(r"\s+\[(.+?)\]\s+(\d+)\s+lines\s+(.+?):(\d+)\s+<->\s+(.+?):(\d+)", line)
        if not match:
            continue
        top_blocks.append(
            Block(
                tag=match.group(1),
                length=int(match.group(2)),
                file_a=match.group(3),
                start_a=int(match.group(4)),
                file_b=match.group(5),
                start_b=int(match.group(6)),
            )
        )

    return {
        "discovered": grab(r"discovered files:\s+(\d+)", int),
        "eligible": grab(r"eligible files:\s+(\d+)", int),
        "sig_loc": grab(r"significant LOC:\s+(\d+)", int),
        "dup_loc": grab(r"duplicated LOC:\s+(\d+)", int),
        "ratio_pct": grab(r"duplication ratio:\s+([0-9.]+)", float),
        "cross_file_blocks": grab(r"cross-file blocks:\s+(\d+)", int),
        "blocks": top_blocks,
    }


def run_scope(binary: Path, scope: Scope) -> dict:
    cmd = ["/usr/bin/time", "-f", "ELAPSED=%e\nRSS=%M", str(binary), str(scope.root), "--cross-only", "--top", "12"]
    for pattern in COMMON_REPORT_EXCLUDES:
        cmd.extend(["--exclude", pattern])
    for pattern in SCOPE_EXTRA_EXCLUDES.get(scope.title, []):
        cmd.extend(["--exclude", pattern])
    proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
    summary = parse_summary(proc.stdout)
    elapsed = re.search(r"ELAPSED=([0-9.]+)", proc.stderr)
    rss = re.search(r"RSS=(\d+)", proc.stderr)
    summary["elapsed_s"] = float(elapsed.group(1)) if elapsed else None
    summary["rss_kb"] = int(rss.group(1)) if rss else None
    summary["raw_stdout"] = proc.stdout
    return summary


def extract_fragment(root: Path, relative_path: str, start_line: int, significant_length: int) -> list[str]:
    path = root / relative_path
    lines = path.read_text(encoding="utf-8", errors="ignore").splitlines()
    out: list[str] = []
    sig = 0
    index = max(start_line - 1, 0)
    while index < len(lines) and sig < significant_length:
        raw = lines[index]
        out.append(raw)
        if is_significant(raw):
            sig += 1
        index += 1
    return out


def choose_example(scope: Scope, summary: dict) -> Block | None:
    blocks: list[Block] = summary["blocks"]
    preferred = PREFERRED_MATCHES.get(scope.key, [])
    for left, right in preferred:
        for block in blocks:
            if block.length <= 5:
                continue
            if block.file_a == left and block.file_b == right:
                return block
            if block.file_a == right and block.file_b == left:
                return block
    for block in blocks:
        if block.length > 5:
            return block
    return None


def code_lang(path: str) -> str:
    suffix = Path(path).suffix.lower()
    if suffix in {".h", ".hh", ".hpp", ".hxx", ".ipp", ".tpp", ".inl", ".inc", ".c", ".cc", ".cpp", ".cxx"}:
        return "cpp"
    return ""


def write_example_file(scope: Scope, block: Block, fragment_a: list[str], fragment_b: list[str], similarity_pct: float) -> Path:
    EXAMPLES_DIR.mkdir(parents=True, exist_ok=True)
    target = EXAMPLES_DIR / f"{scope.key}.md"

    note = MANUAL_NOTES.get(scope.key, "Автоматически выбран top cross-file блок > 5 строк. Ручная валидация отдельно не делалась.")
    status = MANUAL_STATUS.get(scope.key, "не проверено вручную")

    content = []
    content.append(f"# {scope.title}\n")
    content.append(f"- `Файл A`: `{block.file_a}:{block.start_a}`")
    content.append(f"- `Файл B`: `{block.file_b}:{block.start_b}`")
    content.append(f"- `Длина`: `{block.length}` значимых строк")
    content.append(f"- `Вес сигнала`: `{signal_weight(block.length)}`")
    content.append(f"- `Похожесть`: `{similarity_pct:.1f}%` по нормализованным значимым строкам")
    content.append(f"- `Проверка`: {status}")
    content.append(f"- `Комментарий`: {note}\n")
    content.append("## Исходный фрагмент\n")
    content.append(f"```{code_lang(block.file_a)}")
    content.extend(fragment_a)
    content.append("```\n")
    content.append("## Результирующий фрагмент\n")
    content.append(f"```{code_lang(block.file_b)}")
    content.extend(fragment_b)
    content.append("```")

    target.write_text("\n".join(content) + "\n", encoding="utf-8")
    return target


def build_report(scopes: list[Scope], results: list[dict]) -> str:
    lines: list[str] = []
    lines.append("# Project examples report")
    lines.append("")
    lines.append("Автогенерированный обзор top cross-file duplicated block > 5 строк для app-only режима.")
    lines.append("")
    lines.append("Режим отчёта:")
    lines.append("")
    lines.append("- test-like файлы исключаются бинарём по умолчанию")
    lines.append("- report-level excludes: `examples`, `_deps`, `CMakeFiles`, `CompilerId*`")
    lines.append("- для шумных репозиториев добавлены scope-specific excludes для amalgam/autogen/vendor деревьев")
    lines.append("")
    lines.append("## Сводка")
    lines.append("")
    lines.append("| Проект | Ratio | Cross-file blocks | Время | RSS | Пример |")
    lines.append("|--------|-------|-------------------|-------|-----|--------|")

    for scope, result in zip(scopes, results):
        if result.get("example_path"):
            example_link = f"[пример](examples/{Path(result['example_path']).name})"
        else:
            example_link = "—"
        lines.append(
            f"| `{scope.title}` | {result['ratio_pct']:.2f}% | {result['cross_file_blocks']} | "
            f"{result['elapsed_s']:.2f} s | {result['rss_kb'] / 1024:.1f} MB | {example_link} |"
        )

    lines.append("")
    lines.append("## Карточки")
    lines.append("")

    for scope, result in zip(scopes, results):
        lines.append(f"### {scope.title}")
        lines.append("")
        lines.append(f"- `discovered files`: {result['discovered']}")
        lines.append(f"- `eligible files`: {result['eligible']}")
        lines.append(f"- `significant LOC`: {result['sig_loc']}")
        lines.append(f"- `duplicated LOC`: {result['dup_loc']}")
        lines.append(f"- `duplication ratio`: {result['ratio_pct']:.2f}%")
        lines.append(f"- `cross-file blocks`: {result['cross_file_blocks']}")
        lines.append(f"- `elapsed`: {result['elapsed_s']:.2f} s")
        lines.append(f"- `peak RSS`: {result['rss_kb'] / 1024:.1f} MB")

        block: Block | None = result.get("chosen_block")
        if block is None:
            lines.append("- `пример`: нет cross-file блока длиннее 5 значимых строк")
            lines.append("")
            continue

        lines.append(f"- `файл A`: `{block.file_a}:{block.start_a}`")
        lines.append(f"- `файл B`: `{block.file_b}:{block.start_b}`")
        lines.append(f"- `длина`: `{block.length}` значимых строк")
        lines.append(f"- `вес сигнала`: `{signal_weight(block.length)}`")
        lines.append(f"- `похожесть`: `{result['similarity_pct']:.1f}%`")
        lines.append(f"- `проверка`: {MANUAL_STATUS.get(scope.key, 'не проверено вручную')}")
        lines.append(f"- `комментарий`: {MANUAL_NOTES.get(scope.key, 'Автоматически выбран top cross-file блок > 5 строк. Ручная валидация отдельно не делалась.')}")
        lines.append(f"- `полный пример`: [examples/{Path(result['example_path']).name}](examples/{Path(result['example_path']).name})")
        lines.append("")

    return "\n".join(lines) + "\n"


def main() -> int:
    binary = DEFAULT_BINARY
    if not binary.exists():
        raise SystemExit(f"binary not found: {binary}")

    scopes = discover_repo_scopes()
    results: list[dict] = []
    EXAMPLES_DIR.mkdir(parents=True, exist_ok=True)
    for path in EXAMPLES_DIR.glob("*.md"):
        path.unlink()

    for scope in scopes:
        summary = run_scope(binary, scope)
        block = choose_example(scope, summary)
        summary["chosen_block"] = block
        if block is not None:
            fragment_a = extract_fragment(scope.root, block.file_a, block.start_a, block.length)
            fragment_b = extract_fragment(scope.root, block.file_b, block.start_b, block.length)
            sim = similarity(fragment_a, fragment_b)
            example_path = write_example_file(scope, block, fragment_a, fragment_b, sim)
            summary["similarity_pct"] = sim
            summary["example_path"] = str(example_path)
        results.append(summary)

    REPORT_PATH.write_text(build_report(scopes, results), encoding="utf-8")
    print(f"wrote {REPORT_PATH}")
    print(f"wrote examples under {EXAMPLES_DIR}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
