# archcheck vs SonarQube vs NiCad

Date: 2026-07-08.

Purpose: a short comparison with no vague wording. Answers in the table are
binary: **yes** / **no**. Where a capability exists only in a different sense,
that is moved to a note below the table.

## Summary table

| Capability | archcheck | SonarQube Community Build | SonarQube Server / Cloud paid | NiCad |
|---|---:|---:|---:|---:|
| Free | yes | yes | no | yes |
| C++ analysis | yes | yes | yes | no |
| C analysis | yes | yes | yes | yes |
| Single static CLI binary | yes | no | no | no |
| Runs in a PR | yes | yes | yes | no |
| Can fail a PR | yes | yes | yes | no |
| Baseline for pre-existing violations | yes | yes | yes | no |
| Include graph | yes | no | no | no |
| Include cycles | yes | no | no | no |
| God headers | yes | no | no | no |
| Include chain length | yes | no | no | no |
| Case-mismatch include gate | yes | no | no | no |
| Unresolved local include finding | yes | no | no | no |
| New copy-paste in PR | yes | yes | yes | no |
| Copy-paste density / percent gate | no | yes | yes | no |
| Concrete clone pairs `file:line <-> file:line` | yes | no | no | yes |
| Boolean-field accretion drift | yes | no | no | no |
| Local complexity drift | yes | yes | yes | no |
| SATD delta in PR | yes | no | no | no |

## Notes

### SonarQube

SonarQube Community Build is free and supports C/C++ analysis. Its feature table
also lists pull request analysis; for Community Build the target branch condition
is limited to the main branch. SonarQube has quality gates and can fail a PR.

SonarQube has duplication analysis, but the gate is a density/percent quality-gate
signal. archcheck reports a concrete new clone pair in a PR as an advisory:
`file:line <-> file:line`, clone type, and a fix direction.

SonarQube does not provide archcheck's C++ include-architecture checks:
include graph, include cycles, god headers, include chain length, include case
mismatch, unresolved local include.

### NiCad

NiCad is a clone detector. It reports clone pairs/classes and HTML/XML reports.
It is not a PR gate and does not provide baseline/diff workflow.

NiCad supports C in the standard language list used in our validation. C++ is not
listed in the NiCad README language list. It may be extendable by adding a TXL
grammar, but that is not the same as a ready C++ PR tool.

The NiCad comparison we actually ran was on the C example `monit-4.2`, not on a C++
project. See [nicad_vs_archcheck.md](nicad_vs_archcheck.md).

## Bottom line

SonarQube is the broad quality platform: C++, PR analysis, quality gates,
duplication density, security/reliability/maintainability rules.

NiCad is the clone-research tool: strong clone-class detection, but no C++ PR gate
and no architecture checks.

archcheck is the C++ CI architecture tool: include graph, include cycles,
god headers, include chain length, baseline/diff, and concrete new-copy-paste
findings in PRs.

## Sources

- Shipped archcheck behavior: [../CHANGELOG.md](../CHANGELOG.md)
- Product framing: [../docs/architecture-spec.md](../docs/architecture-spec.md)
- Clone landscape: [../docs/research/clone_tools_landscape.md](../docs/research/clone_tools_landscape.md)
- NiCad quantitative validation: [nicad_vs_archcheck.md](nicad_vs_archcheck.md)
- SonarQube Community Build docs:
  <https://docs.sonarsource.com/sonarqube-community-build/>
- SonarQube feature comparison:
  <https://docs.sonarsource.com/sonarqube-community-build/feature-comparison-table>
- SonarQube supported languages:
  <https://docs.sonarsource.com/sonarqube-community-build/analyzing-source-code/languages/overview>
- NiCad README:
  <https://github.com/CordyJ/Open-NiCad>
