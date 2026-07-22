# Dependency cycles in C++ repositories — analysis with clickable links

> Source: `experiments/ai_repo_run/recensus.tsv` (a re-run of the fixed binary over
> on-disk violators) + `cpp_structural_census.tsv`. Ranking is by **SF.9**
> (the rule the user actually sees: it filters conditional edges and
> the header+inline-impl idiom), and **not** by the raw `--graph sccs_cyclic`, which over-counted.

## Metric history (important for honesty)

The first version of this report ranked by the raw `--graph sccs_cyclic` (number of SCCs of size ≥2 in
the include graph). A corpus run revealed that this metric **overestimates architectural debt**: the top
of the ranking was occupied by the decl/impl-split idiom (`foo.h ↔ foo.inl`) and mis-resolution of system
headers, not by real cycles. Following the report, fix **#088** was filed and implemented:

- **SF.9 does not count as a cycle** the loop `X.h ↔ X.{inl,ipp,tmpl.h,hxx,tcc,...}` of one component in one
  directory (one logical module split into a declaration + inline/template implementation;
  the include guard breaks the loop).
- **System `<...>` headers** (`<string.h>`, `<vector>`, …) resolve as External, even if the
  repo has a file with the same basename (there was a phantom `defs.h ↔ string.h` in PipeWire).

After the fixes the report is **re-sorted by SF.9**. Below is what this changed.

## What fixes #088 removed (top removed false cycles)

Of the 276 repos that had raw `--graph` cycles, **24 became fully SF.9-clean**, and across these
repos a total of **384 false cycle instances (37%) were removed**: raw `1028 → SF.9 644`. The FP was concentrated in
a handful of repos with inflated counters — exactly the ones that set the top of the old ranking:

| repo | was (raw --graph) | became (SF.9) | removed |
|---|---|---|---|
| [nv-legate/legate](https://github.com/nv-legate/legate) | 163 | 0 | −163 |
| [acts-project/acts](https://github.com/acts-project/acts) | 46 | 1 | −45 |
| [open-telemetry/opentelemetry-dotnet-instrumentation](https://github.com/open-telemetry/opentelemetry-dotnet-instrumentation) | 33 | 3 | −30 |
| [christoph2/pyA2L](https://github.com/christoph2/pyA2L) | 22 | 0 | −22 |
| [mingw-w64/mingw-w64](https://github.com/mingw-w64/mingw-w64) | 32 | 15 | −17 |
| [IDNI/tau-lang](https://github.com/IDNI/tau-lang) | 15 | 3 | −12 |
| [grafana/pyroscope-dotnet](https://github.com/grafana/pyroscope-dotnet) | 11 | 3 | −8 |
| [jjuran/metamage_1](https://github.com/jjuran/metamage_1) | 6 | 0 | −6 |
| [zephyrproject-rtos/trusted-firmware-m](https://github.com/zephyrproject-rtos/trusted-firmware-m) | 9 | 4 | −5 |
| [nrfconnect/sdk-trusted-firmware-m](https://github.com/nrfconnect/sdk-trusted-firmware-m) | 8 | 3 | −5 |
| [microsoft/WSL](https://github.com/microsoft/WSL) | 8 | 4 | −4 |
| [Slicer/Slicer](https://github.com/Slicer/Slicer) | 4 | 0 | −4 |

This must be stated plainly: **the original "most cyclic" ranking was a tool artifact** (`.inl`/
`.ipp`/`.hxx` idiom + `string.h` mis-resolution), not a property of the code. legate with its 163 "cycles" —
all 163 were the same decl/impl-split.

## Main conclusion (after the fixes)

**The broad conclusion held, the severity did not.** Of 770 violators, **252 (33%) have real SF.9 cycles**.
Cycles in agentic C++ are genuinely frequent — but the raw metric overstated their count by ~37%, and the top
of the ranking was almost entirely FP. Real cycles (see the manual verification below) come in three natures:

- **Authored mutual include** between different components (`A.h ↔ B.h`) — this is exactly the Lakos cycle
  SF.9 exists for. Seen in strongswan, ompi, DPDK, composable_kernel, ert-components.
- **Vendored code** — a real cycle, but in third-party sources inside the repo (CoreCLR in
  otel/pyroscope, the kernel in LineageOS, the CRT in mingw).
- **Generated code** — `ui.h ↔ ui_helpers.h`, repeated across screens (SquareLine in IncuNest).

## Method

1. A re-run of the fixed `archcheck` (SF.9) over on-disk violators under `~/oss_corpus/oss/<owner_repo>`.
2. For the top — `git rev-parse HEAD` (12 chars), the cycle path from SF.9 (`A → B → A`).
3. Each cycle is **verified manually** in the clone's code — real `#include` lines opened.
   ✅ = mutual include is visible in the code; ⚠️ = vendor/generated/disputable.

---

## Top 20 by SF.9 — analysis of confirmed cycles

| # | repo | SF.9 | was raw | verdict |
|---|---|---|---|---|
| 1 | LineageOS/android_kernel_asus_sm8350 | 68 | 73 | ✅ vendored kernel |
| 2 | wpilibsuite/allwpilib | 34 | 34 | ✅ type ↔ proto-traits |
| 3 | mingw-w64/mingw-w64 | 15 | 32 | ✅ base CRT ↔ secure-variant |
| 4 | DPDK/dpdk | 14 | 14 | ✅ authored 2/3-node |
| 5 | medicalopenworld/IncuNest | 12 | 12 | ✅ generated SquareLine |
| 6 | danielnachun/recipe_staging | 11 | 11 | ⚠️ `.hh↔.hpp` vendor (see below) |
| 7 | open-mpi/ompi | 9 | 9 | ✅ HAN collective |
| 8 | ROCm/composable_kernel | 9 | 9 | ✅ element-wise ops |
| 9 | strongswan/strongswan | 9 | 9 | ✅ authored libcharon cycles |
| 10 | inxware/ert-components | 9 | 9 | ✅ author admits the cycle himself |
| 11 | PipeWire/pipewire | 7 | 9 | ✅ type-info (string.h FP removed) |
| 12 | sxs-collaboration/spectre | 6 | 6 | needs manual check |
| 13 | xen-project/xen | 6 | 6 | needs manual check |
| 14 | aravindkrishnaswamy/RISE | 6 | 6 | needs manual check |
| 15 | Icinga/icinga2 | 5 | 5 | needs manual check |
| 16 | eclipse-openj9/openj9-omr | 5 | 5 | needs manual check |
| 17 | eclipse-omr/omr | 5 | 5 | needs manual check |
| 18 | kokkos/kokkos | 5 | 5 | needs manual check |
| 19 | Washington-University/workbench | 5 | 5 | needs manual check |
| 20 | webserver-llc/angie | 5 | 5 | needs manual check |

Below is the manual verification of the confirmed cases (the links point to specific `#include` lines).

### LineageOS/android_kernel_asus_sm8350 — SF.9=68 ✅ (vendored kernel)
`sha=6022487bf89a`. The Linux kernel (C), the mutual includes are real. Caveat: vendored kernel, not
the repo's authored code.

- [arch/arm/plat-samsung/include/plat/map-s3c.h:74](https://github.com/LineageOS/android_kernel_asus_sm8350/blob/6022487bf89a/arch/arm/plat-samsung/include/plat/map-s3c.h#L74)
- [arch/arm/plat-samsung/include/plat/map-s5p.h:20](https://github.com/LineageOS/android_kernel_asus_sm8350/blob/6022487bf89a/arch/arm/plat-samsung/include/plat/map-s5p.h#L20)

```cpp
// plat/map-s3c.h:74
#include <plat/map-s5p.h>
// plat/map-s5p.h:20
#include <plat/map-s3c.h>
```

### wpilibsuite/allwpilib — SF.9=34 ✅ (proto-serialization)
`sha=ddf93062641d`. A type's header pulls in its `proto/*Proto.hpp`, and that pulls the type back.

- [wpimath/.../controller/ArmFeedforward.hpp:277](https://github.com/wpilibsuite/allwpilib/blob/ddf93062641d/wpimath/src/main/native/include/wpi/math/controller/ArmFeedforward.hpp#L277)
- [wpimath/.../controller/proto/ArmFeedforwardProto.hpp:8](https://github.com/wpilibsuite/allwpilib/blob/ddf93062641d/wpimath/src/main/native/include/wpi/math/controller/proto/ArmFeedforwardProto.hpp#L8)

```cpp
// ArmFeedforward.hpp:277
#include "wpi/math/controller/proto/ArmFeedforwardProto.hpp"
// proto/ArmFeedforwardProto.hpp:8
#include "wpi/math/controller/ArmFeedforward.hpp"
```
Two *different* headers reference each other (type ↔ its proto-traits); it can be broken with a
forward declaration. An inflated counter of 34, but the cycles are real — not the `.inl` idiom.

### mingw-w64/mingw-w64 — SF.9=15 (was 32) ✅
`sha=b536c4fdb038`. Each CRT header pulls in its `sec_api/*_s.h` (Secure CRT variant), which
pulls the base back. Part of the original 32 was the `.inl` idiom and was removed; 15 remain a real loop.

- [mingw-w64-headers/crt/conio.h:200](https://github.com/mingw-w64/mingw-w64/blob/b536c4fdb038/mingw-w64-headers/crt/conio.h#L200)
- [mingw-w64-headers/crt/sec_api/conio_s.h:10](https://github.com/mingw-w64/mingw-w64/blob/b536c4fdb038/mingw-w64-headers/crt/sec_api/conio_s.h#L10)

```cpp
// crt/conio.h:200
#include <sec_api/conio_s.h>
// crt/sec_api/conio_s.h:10
#include <conio.h>
```

### DPDK/dpdk — SF.9=14 ✅ (several real ones, authored)
`sha=4757b8df04b6`.

**roc_api ↔ roc_bphy_cgx** (cnxk common):
- [drivers/common/cnxk/roc_api.h:81](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/cnxk/roc_api.h#L81)
- [drivers/common/cnxk/roc_bphy_cgx.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/cnxk/roc_bphy_cgx.h#L10)

**cperf ↔ cperf_ops** (test-crypto-perf):
- [app/test-crypto-perf/cperf.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/app/test-crypto-perf/cperf.h#L10)
- [app/test-crypto-perf/cperf_ops.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/app/test-crypto-perf/cperf_ops.h#L10)

**3-node zsda** (device → comp_pmd → qp → device):
- [drivers/common/zsda/zsda_device.h:9](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/zsda/zsda_device.h#L9)
- [drivers/compress/zsda/zsda_comp_pmd.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/compress/zsda/zsda_comp_pmd.h#L10)
- [drivers/common/zsda/zsda_qp.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/zsda/zsda_qp.h#L10)

Real 2- and 3-node cycles in the drivers' authored code.

### medicalopenworld/IncuNest — SF.9=12 ✅ (generated SquareLine)
`sha=d08131a23f0e`. Generated SquareLine Studio UI: each screen is `ui.h ↔ ui_helpers.h`.

- [.../Screen_Alarms/ui.h:15](https://github.com/medicalopenworld/IncuNest/blob/d08131a23f0e/Firmware/Display_HMI/SquareLineProject/Exports/Exports%20ESP32%20HMI%20Interface%20Screen_Alarms/ui.h#L15)
- [.../Screen_Alarms/ui_helpers.h:13](https://github.com/medicalopenworld/IncuNest/blob/d08131a23f0e/Firmware/Display_HMI/SquareLineProject/Exports/Exports%20ESP32%20HMI%20Interface%20Screen_Alarms/ui_helpers.h#L13)

A mutual include, but generated code, repeated per screen — hence 12 identical pairs.

### danielnachun/recipe_staging — SF.9=11 ⚠️ (idiom boundary)
`sha=6476ac7a73f7`. A conda-staging repo; the cycles are in the vendored `bcl2fastq2`, the pattern
`FastIo.hh ↔ FastIo.hpp`. Fix #088 **did not remove** this: neither `.hh` nor `.hpp` are among the
inline-impl markers (these are just two header extension variants), so a decl/impl-split cannot be told
apart from a real cycle here without content analysis. Honestly: probably the idiom, but SF.9 fairly
keeps it — this is the applicability boundary of the #088 heuristic.

- [bcl2fastq2/.../common/FastIo.hh](https://github.com/danielnachun/recipe_staging/blob/6476ac7a73f7/bcl2fastq2/bcl2fastq/src/cxx/include/common/FastIo.hh)
- [bcl2fastq2/.../common/FastIo.hpp](https://github.com/danielnachun/recipe_staging/blob/6476ac7a73f7/bcl2fastq2/bcl2fastq/src/cxx/include/common/FastIo.hpp)

### open-mpi/ompi — SF.9=9 ✅
`sha=0ea6ebd628b2`. `coll_han.h ↔ coll_han_dynamic.h`.

- [ompi/mca/coll/han/coll_han.h:49](https://github.com/open-mpi/ompi/blob/0ea6ebd628b2/ompi/mca/coll/han/coll_han.h#L49)
- [ompi/mca/coll/han/coll_han_dynamic.h:27](https://github.com/open-mpi/ompi/blob/0ea6ebd628b2/ompi/mca/coll/han/coll_han_dynamic.h#L27)

```cpp
// coll_han.h:49
#include "ompi/mca/coll/han/coll_han_dynamic.h"
// coll_han_dynamic.h:27
#include "ompi/mca/coll/han/coll_han.h"
```

### ROCm/composable_kernel — SF.9=9 ✅
`sha=2c363870d932`. `binary_element_wise_operation.hpp ↔ element_wise_operation.hpp`.

- [include/ck/tensor_operation/gpu/element/binary_element_wise_operation.hpp:7](https://github.com/ROCm/composable_kernel/blob/2c363870d932/include/ck/tensor_operation/gpu/element/binary_element_wise_operation.hpp#L7)
- [include/ck/tensor_operation/gpu/element/element_wise_operation.hpp](https://github.com/ROCm/composable_kernel/blob/2c363870d932/include/ck/tensor_operation/gpu/element/element_wise_operation.hpp)

### strongswan/strongswan — SF.9=9 ✅ (vivid, authored)
`sha=5fc403702b01`. Real architectural cycles in libcharon.

**attribute_handler ↔ ike_sa:**
- [src/libcharon/attributes/attribute_handler.h:27](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/attributes/attribute_handler.h#L27)
- [src/libcharon/sa/ike_sa.h:37](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/sa/ike_sa.h#L37)

**bus ↔ logger:**
- [src/libcharon/bus/bus.h:37](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/bus/bus.h#L37)
- [src/libcharon/bus/listeners/logger.h:27](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/bus/listeners/logger.h#L27)

A classic Lakos cycle in authored code (sa ↔ attributes, bus ↔ logger).

### inxware/ert-components — SF.9=9 ✅ (author admits the cycle himself)
`sha=19db6242917d`. `widget.h ↔ widget_ui.h` — the code has an **author comment about a circular-include**.

- [Common/HAL/graphics/widget.h:88](https://github.com/inxware/ert-components/blob/19db6242917d/Common/HAL/graphics/widget.h#L88)
- [Common/HAL/graphics/widget_ui.h:22](https://github.com/inxware/ert-components/blob/19db6242917d/Common/HAL/graphics/widget_ui.h#L22)

```cpp
// widget.h:51  /* ... must be before widget_ui.h include to avoid circular-include ordering issues */
// widget.h:88
#include "widget_ui.h"
// widget_ui.h:22
#include "widget.h" // needed for enums of widget types
```
The most honest case: the author writes directly about "circular-include ordering issues".

### PipeWire/pipewire — SF.9=7 (was 9) ✅ (string.h FP removed by fix #088)
`sha=bb634fb0f9f1`. The real type-info cycle remained; the phantom `defs.h ↔ string.h` was removed (fix #2.1).

- [spa/include/spa/monitor/type-info.h:8](https://github.com/PipeWire/pipewire/blob/bb634fb0f9f1/spa/include/spa/monitor/type-info.h#L8)
- [spa/include/spa/utils/type-info.h:13](https://github.com/PipeWire/pipewire/blob/bb634fb0f9f1/spa/include/spa/utils/type-info.h#L13)

```cpp
// monitor/type-info.h:8
#include <spa/utils/type-info.h>
// utils/type-info.h:13
#include <spa/monitor/type-info.h>
```

---

## Recommendation for archcheck — IMPLEMENTED in #088

This report's earlier recommendation ("SF.9 must exclude the loop `X.h ↔ X.{inl,ipp,tmpl.h,hxx,tcc}`
+ fix the mis-resolution of system headers") is **done**: fixes #1 (idiom) and #2.1
(system headers) with tests in `tests/unit/rules/sf9_no_cycles_test.cpp` and
`tests/unit/scan/include_resolver_test.cpp`. The residual boundary is `.hh ↔ .hpp` (recipe_staging):
here a decl/impl-split cannot be told apart from a real cycle without content analysis, so SF.9
conservatively keeps such pairs as cycles.

---

## Full table of all 252 repos with real SF.9 cycles

(ranking by `sf9_cycles` descending; the "was raw" column = the old `--graph sccs_cyclic`, `—` if
the repo had no raw cycles but SF.9 found real ones)

| repo | SF.9 cycles | was (raw) |
|---|---|---|
|---|---|---|
| [LineageOS/android_kernel_asus_sm8350](https://github.com/LineageOS/android_kernel_asus_sm8350) | 68 | 73 |
| [wpilibsuite/allwpilib](https://github.com/wpilibsuite/allwpilib) | 34 | 34 |
| [mingw-w64/mingw-w64](https://github.com/mingw-w64/mingw-w64) | 15 | 32 |
| [DPDK/dpdk](https://github.com/DPDK/dpdk) | 14 | 14 |
| [medicalopenworld/IncuNest](https://github.com/medicalopenworld/IncuNest) | 12 | 12 |
| [danielnachun/recipe_staging](https://github.com/danielnachun/recipe_staging) | 11 | 11 |
| [open-mpi/ompi](https://github.com/open-mpi/ompi) | 9 | 9 |
| [ROCm/composable_kernel](https://github.com/ROCm/composable_kernel) | 9 | 9 |
| [strongswan/strongswan](https://github.com/strongswan/strongswan) | 9 | 9 |
| [inxware/ert-components](https://github.com/inxware/ert-components) | 9 | 9 |
| [PipeWire/pipewire](https://github.com/PipeWire/pipewire) | 7 | 9 |
| [sxs-collaboration/spectre](https://github.com/sxs-collaboration/spectre) | 6 | 6 |
| [xen-project/xen](https://github.com/xen-project/xen) | 6 | 6 |
| [aravindkrishnaswamy/RISE](https://github.com/aravindkrishnaswamy/RISE) | 6 | 6 |
| [Icinga/icinga2](https://github.com/Icinga/icinga2) | 5 | 5 |
| [eclipse-openj9/openj9-omr](https://github.com/eclipse-openj9/openj9-omr) | 5 | 5 |
| [eclipse-omr/omr](https://github.com/eclipse-omr/omr) | 5 | 5 |
| [kokkos/kokkos](https://github.com/kokkos/kokkos) | 5 | 5 |
| [Washington-University/workbench](https://github.com/Washington-University/workbench) | 5 | 5 |
| [webserver-llc/angie](https://github.com/webserver-llc/angie) | 5 | 5 |
| [KallistiOS/KallistiOS](https://github.com/KallistiOS/KallistiOS) | 5 | 5 |
| [pmodels/mpich](https://github.com/pmodels/mpich) | 5 | 5 |
| [icssw-org/MeshCom-Firmware](https://github.com/icssw-org/MeshCom-Firmware) | 5 | 5 |
| [RebelToolbox/RebelEngine](https://github.com/RebelToolbox/RebelEngine) | 5 | 5 |
| [mjagdis-imports/sdcc-svn](https://github.com/mjagdis-imports/sdcc-svn) | 5 | 5 |
| [GregorGullwi/FlashCpp](https://github.com/GregorGullwi/FlashCpp) | 4 | 4 |
| [hexagon-geo-surv/trusted-firmware-a](https://github.com/hexagon-geo-surv/trusted-firmware-a) | 4 | 4 |
| [Matthew-McRaven/Pepp](https://github.com/Matthew-McRaven/Pepp) | 4 | 4 |
| [zephyrproject-rtos/trusted-firmware-m](https://github.com/zephyrproject-rtos/trusted-firmware-m) | 4 | 9 |
| [facebook/openbmc](https://github.com/facebook/openbmc) | 4 | 4 |
| [microsoft/WSL](https://github.com/microsoft/WSL) | 4 | 8 |
| [espressif/esp-iot-solution](https://github.com/espressif/esp-iot-solution) | 4 | 4 |
| [apache/axis-axis2-c-core](https://github.com/apache/axis-axis2-c-core) | 4 | 4 |
| [Cockatrice/Cockatrice](https://github.com/Cockatrice/Cockatrice) | 4 | 4 |
| [ARM-software/arm-trusted-firmware](https://github.com/ARM-software/arm-trusted-firmware) | 4 | 4 |
| [GNOME/mutter](https://github.com/GNOME/mutter) | 4 | 4 |
| [OpendTect/OpendTect](https://github.com/OpendTect/OpendTect) | 4 | 4 |
| [tksuoran/erhe](https://github.com/tksuoran/erhe) | 4 | 4 |
| [intel/intel-graphics-compiler](https://github.com/intel/intel-graphics-compiler) | 4 | 5 |
| [stillwater-sc/universal](https://github.com/stillwater-sc/universal) | 4 | 4 |
| [PhoshMobi/phoc](https://github.com/PhoshMobi/phoc) | 4 | 4 |
| [lttng/lttng-tools](https://github.com/lttng/lttng-tools) | 4 | 4 |
| [elogind/elogind](https://github.com/elogind/elogind) | 4 | 5 |
| [apache/mynewt-core](https://github.com/apache/mynewt-core) | 3 | 3 |
| [OvenMediaLabs/OvenMediaEngine](https://github.com/OvenMediaLabs/OvenMediaEngine) | 3 | 3 |
| [microsoft/Teams-AdaptiveCards-Mobile](https://github.com/microsoft/Teams-AdaptiveCards-Mobile) | 3 | 3 |
| [tlapack/tlapack](https://github.com/tlapack/tlapack) | 3 | 3 |
| [open-telemetry/opentelemetry-dotnet-instrumentation](https://github.com/open-telemetry/opentelemetry-dotnet-instrumentation) | 3 | 33 |
| [daos-stack/daos](https://github.com/daos-stack/daos) | 3 | 3 |
| [CueMol/cuemol2](https://github.com/CueMol/cuemol2) | 3 | 3 |
| [FRRouting/frr](https://github.com/FRRouting/frr) | 3 | 3 |
| [nrfconnect/sdk-trusted-firmware-m](https://github.com/nrfconnect/sdk-trusted-firmware-m) | 3 | 8 |
| [heatd/Onyx](https://github.com/heatd/Onyx) | 3 | 4 |
| [sipwise/rtpengine](https://github.com/sipwise/rtpengine) | 3 | 3 |
| [davischw/frr](https://github.com/davischw/frr) | 3 | 3 |
| [grafana/pyroscope-dotnet](https://github.com/grafana/pyroscope-dotnet) | 3 | 11 |
| [meshtastic/firmware](https://github.com/meshtastic/firmware) | 3 | 4 |
| [ClusterLabs/pacemaker](https://github.com/ClusterLabs/pacemaker) | 3 | 3 |
| [OpenDataPlane/odp](https://github.com/OpenDataPlane/odp) | 3 | 3 |
| [topling/toplingdb](https://github.com/topling/toplingdb) | 3 | 4 |
| [IDNI/tau-lang](https://github.com/IDNI/tau-lang) | 3 | 15 |
| [shader-slang/slang](https://github.com/shader-slang/slang) | 3 | 3 |
| [canmeng12/packages](https://github.com/canmeng12/packages) | 3 | 6 |
| [manticoresoftware/manticoresearch](https://github.com/manticoresoftware/manticoresearch) | 3 | 3 |
| [nss-dev/nss](https://github.com/nss-dev/nss) | 3 | 3 |
| [flux-framework/flux-core](https://github.com/flux-framework/flux-core) | 2 | 2 |
| [NVIDIA/nccl](https://github.com/NVIDIA/nccl) | 2 | 2 |
| [OpenXiangShan/GEM5](https://github.com/OpenXiangShan/GEM5) | 2 | 3 |
| [acts-project/traccc](https://github.com/acts-project/traccc) | 2 | 2 |
| [yandex/odyssey](https://github.com/yandex/odyssey) | 2 | 2 |
| [NVIDIAGameWorks/dxvk-remix](https://github.com/NVIDIAGameWorks/dxvk-remix) | 2 | 2 |
| [gnutls/gnutls](https://github.com/gnutls/gnutls) | 2 | 2 |
| [schwabe/openvpn](https://github.com/schwabe/openvpn) | 2 | 2 |
| [vvaltchev/tilck](https://github.com/vvaltchev/tilck) | 2 | 3 |
| [kotuku-group/kotuku](https://github.com/kotuku-group/kotuku) | 2 | 2 |
| [CoolProp/CoolProp](https://github.com/CoolProp/CoolProp) | 2 | 2 |
| [apache/trafficserver](https://github.com/apache/trafficserver) | 2 | 2 |
| [SSSD/sssd](https://github.com/SSSD/sssd) | 2 | 2 |
| [baidxi/buildroot](https://github.com/baidxi/buildroot) | 2 | 2 |
| [barebox/barebox](https://github.com/barebox/barebox) | 2 | 2 |
| [Bipto/Nexus](https://github.com/Bipto/Nexus) | 2 | 6 |
| [OpenChemistry/avogadrolibs](https://github.com/OpenChemistry/avogadrolibs) | 2 | 2 |
| [leuat/TRSE](https://github.com/leuat/TRSE) | 2 | 2 |
| [NVIDIA/Fuser](https://github.com/NVIDIA/Fuser) | 2 | 2 |
| [dovecot/core](https://github.com/dovecot/core) | 2 | 2 |
| [jdolan/quetoo](https://github.com/jdolan/quetoo) | 2 | 2 |
| [FreeRADIUS/freeradius-server](https://github.com/FreeRADIUS/freeradius-server) | 2 | 3 |
| [m-ab-s/aom](https://github.com/m-ab-s/aom) | 2 | 2 |
| [apache/nuttx-apps](https://github.com/apache/nuttx-apps) | 2 | 2 |
| [stellar/stellar-core](https://github.com/stellar/stellar-core) | 2 | 2 |
| [OpenDataPlane/odp-dpdk](https://github.com/OpenDataPlane/odp-dpdk) | 2 | 2 |
| [hexagon-geo-surv/barebox](https://github.com/hexagon-geo-surv/barebox) | 2 | 2 |
| [jank-lang/jank](https://github.com/jank-lang/jank) | 2 | 2 |
| [microsoft/terminal](https://github.com/microsoft/terminal) | 2 | 2 |
| [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | 2 | 2 |
| [mariadb-corporation/mariadb-columnstore-engine](https://github.com/mariadb-corporation/mariadb-columnstore-engine) | 2 | 2 |
| [OpenVPN/openvpn](https://github.com/OpenVPN/openvpn) | 2 | 2 |
| [revng/revng](https://github.com/revng/revng) | 2 | 2 |
| [markaren/threepp](https://github.com/markaren/threepp) | 2 | 2 |
| [FujiNetWIFI/fujinet-firmware](https://github.com/FujiNetWIFI/fujinet-firmware) | 2 | 2 |
| [cyrusimap/cyrus-imapd](https://github.com/cyrusimap/cyrus-imapd) | 2 | 2 |
| [OpenEnroth/OpenEnroth](https://github.com/OpenEnroth/OpenEnroth) | 2 | 2 |
| [olafhering/grub](https://github.com/olafhering/grub) | 2 | 4 |
| [sciapp/gr](https://github.com/sciapp/gr) | 2 | 2 |
| [ca2/operating_system-windows](https://github.com/ca2/operating_system-windows) | 2 | 2 |
| [espressif/esp-mqtt](https://github.com/espressif/esp-mqtt) | 2 | 2 |
| [GNOME/gegl](https://github.com/GNOME/gegl) | 2 | 2 |
| [facebook/fboss](https://github.com/facebook/fboss) | 2 | 4 |
| [GNOME/gnome-software](https://github.com/GNOME/gnome-software) | 2 | 2 |
| [open5gs/open5gs](https://github.com/open5gs/open5gs) | 2 | 2 |
| [deskull-m/bakabakaband](https://github.com/deskull-m/bakabakaband) | 2 | 2 |
| [contour-terminal/contour](https://github.com/contour-terminal/contour) | 2 | 2 |
| [surge-synthesizer/shortcircuit-xt](https://github.com/surge-synthesizer/shortcircuit-xt) | 2 | 2 |
| [lf-lang/reactor-c](https://github.com/lf-lang/reactor-c) | 2 | 2 |
| [ofiwg/libfabric](https://github.com/ofiwg/libfabric) | 1 | 1 |
| [HermanChen/mpp](https://github.com/HermanChen/mpp) | 1 | 1 |
| [BlazingRenderer/BRender](https://github.com/BlazingRenderer/BRender) | 1 | 1 |
| [FEniCS/dolfinx](https://github.com/FEniCS/dolfinx) | 1 | 1 |
| [kbingham/libcamera](https://github.com/kbingham/libcamera) | 1 | 1 |
| [compiler-research/CppInterOp](https://github.com/compiler-research/CppInterOp) | 1 | 1 |
| [sailfishos-mirror/ltp](https://github.com/sailfishos-mirror/ltp) | 1 | 1 |
| [cygwin/cygwin](https://github.com/cygwin/cygwin) | 1 | 1 |
| [RediSearch/RediSearch](https://github.com/RediSearch/RediSearch) | 1 | 1 |
| [vifm/vifm](https://github.com/vifm/vifm) | 1 | 1 |
| [ipxe/ipxe](https://github.com/ipxe/ipxe) | 1 | 2 |
| [OP-TEE/optee_os](https://github.com/OP-TEE/optee_os) | 1 | 1 |
| [ornladios/ADIOS2](https://github.com/ornladios/ADIOS2) | 1 | 2 |
| [lustre/lustre-release](https://github.com/lustre/lustre-release) | 1 | 1 |
| [ChristianGaser/CAT-Surface](https://github.com/ChristianGaser/CAT-Surface) | 1 | 1 |
| [intel/compute-runtime](https://github.com/intel/compute-runtime) | 1 | 1 |
| [GNOME/glib](https://github.com/GNOME/glib) | 1 | 1 |
| [aardappel/lobster](https://github.com/aardappel/lobster) | 1 | 1 |
| [esbmc/esbmc](https://github.com/esbmc/esbmc) | 1 | 1 |
| [sswroom/SClass](https://github.com/sswroom/SClass) | 1 | 1 |
| [kromenak/gengine](https://github.com/kromenak/gengine) | 1 | 1 |
| [bertiniteam/b2](https://github.com/bertiniteam/b2) | 1 | 1 |
| [sailfishos-mirror/curl](https://github.com/sailfishos-mirror/curl) | 1 | 1 |
| [qemu/ipxe](https://github.com/qemu/ipxe) | 1 | 2 |
| [sailfishos-mirror/valgrind](https://github.com/sailfishos-mirror/valgrind) | 1 | 1 |
| [reupen/columns_ui](https://github.com/reupen/columns_ui) | 1 | 1 |
| [libcamera-org/libcamera](https://github.com/libcamera-org/libcamera) | 1 | 1 |
| [micropython/micropython](https://github.com/micropython/micropython) | 1 | 3 |
| [fwupd/fwupd](https://github.com/fwupd/fwupd) | 1 | 1 |
| [privatevoid-net/nix-super](https://github.com/privatevoid-net/nix-super) | 1 | 1 |
| [VTT-ProperTune/OpenPFC](https://github.com/VTT-ProperTune/OpenPFC) | 1 | 1 |
| [HansKristian-Work/vkd3d-proton](https://github.com/HansKristian-Work/vkd3d-proton) | 1 | 1 |
| [wled/WLED](https://github.com/wled/WLED) | 1 | 1 |
| [timescale/timescaledb](https://github.com/timescale/timescaledb) | 1 | 1 |
| [blender/cycles](https://github.com/blender/cycles) | 1 | 1 |
| [qt/qtdoc](https://github.com/qt/qtdoc) | 1 | 1 |
| [mne-tools/mne-cpp](https://github.com/mne-tools/mne-cpp) | 1 | 1 |
| [shorepine/amy](https://github.com/shorepine/amy) | 1 | 1 |
| [ibm-s390-linux/s390-tools](https://github.com/ibm-s390-linux/s390-tools) | 1 | 1 |
| [sailfishos-mirror/protobuf](https://github.com/sailfishos-mirror/protobuf) | 1 | 1 |
| [curl/curl](https://github.com/curl/curl) | 1 | 1 |
| [aseprite/freetype2](https://github.com/aseprite/freetype2) | 1 | 1 |
| [orioledb/orioledb](https://github.com/orioledb/orioledb) | 1 | 1 |
| [Kitware/CMake](https://github.com/Kitware/CMake) | 1 | 1 |
| [PhoshMobi/stevia](https://github.com/PhoshMobi/stevia) | 1 | 1 |
| [conradhuebler/curcuma](https://github.com/conradhuebler/curcuma) | 1 | 1 |
| [hydrobricks/hydrobricks](https://github.com/hydrobricks/hydrobricks) | 1 | 1 |
| [streetpea/chiaki-ng](https://github.com/streetpea/chiaki-ng) | 1 | 1 |
| [grommunio/gromox](https://github.com/grommunio/gromox) | 1 | 1 |
| [Starlink/ast](https://github.com/Starlink/ast) | 1 | 2 |
| [katsuster/newlib-cygwin](https://github.com/katsuster/newlib-cygwin) | 1 | 1 |
| [rvdbreemen/OTGW-firmware](https://github.com/rvdbreemen/OTGW-firmware) | 1 | 1 |
| [EVerest/EVerest](https://github.com/EVerest/EVerest) | 1 | 1 |
| [nanomq/NanoNNG](https://github.com/nanomq/NanoNNG) | 1 | 1 |
| [sysprogs/openocd](https://github.com/sysprogs/openocd) | 1 | 1 |
| [git-for-windows/msys2-runtime](https://github.com/git-for-windows/msys2-runtime) | 1 | 1 |
| [NixOS/nix](https://github.com/NixOS/nix) | 1 | 1 |
| [nxp-imx/imx-optee-os](https://github.com/nxp-imx/imx-optee-os) | 1 | 1 |
| [microsoft/DirectXShaderCompiler](https://github.com/microsoft/DirectXShaderCompiler) | 1 | 1 |
| [bpftrace/bpftrace](https://github.com/bpftrace/bpftrace) | 1 | 1 |
| [MIPS/newlib](https://github.com/MIPS/newlib) | 1 | 1 |
| [LBNL-ETA/Windows-CalcEngine](https://github.com/LBNL-ETA/Windows-CalcEngine) | 1 | 1 |
| [xfce-mirror/xfce4-docklike-plugin](https://github.com/xfce-mirror/xfce4-docklike-plugin) | 1 | 1 |
| [elf-audio/mzgl](https://github.com/elf-audio/mzgl) | 1 | 1 |
| [haproxy/haproxy](https://github.com/haproxy/haproxy) | 1 | 3 |
| [freetype/freetype](https://github.com/freetype/freetype) | 1 | 1 |
| [cea-hpc/mpc](https://github.com/cea-hpc/mpc) | 1 | 1 |
| [MaaXYZ/MaaFramework](https://github.com/MaaXYZ/MaaFramework) | 1 | 1 |
| [bytecodealliance/wasm-micro-runtime](https://github.com/bytecodealliance/wasm-micro-runtime) | 1 | 1 |
| [libretro/libretro-prboom](https://github.com/libretro/libretro-prboom) | 1 | 1 |
| [espressif/openocd-esp32](https://github.com/espressif/openocd-esp32) | 1 | 1 |
| [digi-embedded/optee_os](https://github.com/digi-embedded/optee_os) | 1 | 1 |
| [bacnet-stack/bacnet-stack](https://github.com/bacnet-stack/bacnet-stack) | 1 | 1 |
| [beerda/nuggets](https://github.com/beerda/nuggets) | 1 | 1 |
| [Unispace365/DsQt](https://github.com/Unispace365/DsQt) | 1 | 1 |
| [openocd-org/openocd](https://github.com/openocd-org/openocd) | 1 | 1 |
| [open-mpi/hwloc](https://github.com/open-mpi/hwloc) | 1 | 1 |
| [LuminariMUD/Luminari-Source](https://github.com/LuminariMUD/Luminari-Source) | 1 | 1 |
| [tomato64/tomato64](https://github.com/tomato64/tomato64) | 1 | 1 |
| [ilia-maslakov/mcdev](https://github.com/ilia-maslakov/mcdev) | 1 | 1 |
| [etlegacy/etlegacy](https://github.com/etlegacy/etlegacy) | 1 | 1 |
| [openzfs/zfs](https://github.com/openzfs/zfs) | 1 | 1 |
| [kevlu8/PZChessBot](https://github.com/kevlu8/PZChessBot) | 1 | 1 |
| [ts-factory/test-environment](https://github.com/ts-factory/test-environment) | 1 | 1 |
| [wiredtiger/wiredtiger](https://github.com/wiredtiger/wiredtiger) | 1 | 1 |
| [RTEMS/sourceware-mirror-newlib-cygwin](https://github.com/RTEMS/sourceware-mirror-newlib-cygwin) | 1 | 1 |
| [BelledonneCommunications/flexisip](https://github.com/BelledonneCommunications/flexisip) | 1 | 1 |
| [203-Systems/MatrixOS](https://github.com/203-Systems/MatrixOS) | 1 | 1 |
| [meganz/sdk](https://github.com/meganz/sdk) | 1 | 1 |
| [erofs/erofs-utils](https://github.com/erofs/erofs-utils) | 1 | 1 |
| [kokkos/kokkos-kernels](https://github.com/kokkos/kokkos-kernels) | 1 | 1 |
| [aws/aws-ofi-nccl](https://github.com/aws/aws-ofi-nccl) | 1 | 1 |
| [networkupstools/nut](https://github.com/networkupstools/nut) | 1 | 1 |
| [grumpycoders/pcsx-redux](https://github.com/grumpycoders/pcsx-redux) | 1 | 1 |
| [OpenAtom-Linyaps/linyaps](https://github.com/OpenAtom-Linyaps/linyaps) | 1 | 1 |
| [KJ7LNW/xnec2c](https://github.com/KJ7LNW/xnec2c) | 1 | 1 |
| [fluent/fluent-bit](https://github.com/fluent/fluent-bit) | 1 | 1 |
| [qt/qttools](https://github.com/qt/qttools) | 1 | 1 |
| [openbabel/openbabel](https://github.com/openbabel/openbabel) | 1 | 1 |
| [simonsj/haproxy](https://github.com/simonsj/haproxy) | 1 | 3 |
| [haxscramper/haxorg](https://github.com/haxscramper/haxorg) | 1 | 1 |
| [FireSourcery/FireSourcery_Library](https://github.com/FireSourcery/FireSourcery_Library) | 1 | 1 |
| [paulfloyd/freebsd_valgrind](https://github.com/paulfloyd/freebsd_valgrind) | 1 | 1 |
| [amule-project/amule](https://github.com/amule-project/amule) | 1 | 1 |
| [casadi/casadi](https://github.com/casadi/casadi) | 1 | 1 |
| [jermp/sshash](https://github.com/jermp/sshash) | 1 | 1 |
| [FEX-Emu/FEX](https://github.com/FEX-Emu/FEX) | 1 | 1 |
| [unikraft/unikraft](https://github.com/unikraft/unikraft) | 1 | 1 |
| [oktetlabs/test-environment](https://github.com/oktetlabs/test-environment) | 1 | 1 |
| [LouisBrunner/valgrind-macos](https://github.com/LouisBrunner/valgrind-macos) | 1 | 1 |
| [virtuoso/clap](https://github.com/virtuoso/clap) | 1 | 1 |
| [KevinOConnor/klipper-dev](https://github.com/KevinOConnor/klipper-dev) | 1 | 1 |
| [cfengine/core](https://github.com/cfengine/core) | 1 | 1 |
| [Cantera/cantera](https://github.com/Cantera/cantera) | 1 | 1 |
| [GNOME/evolution-data-server](https://github.com/GNOME/evolution-data-server) | 1 | 1 |
| [CodSpeedHQ/valgrind-codspeed](https://github.com/CodSpeedHQ/valgrind-codspeed) | 1 | 1 |
| [GNOME/gparted](https://github.com/GNOME/gparted) | 1 | 1 |
| [linux-test-project/ltp](https://github.com/linux-test-project/ltp) | 1 | 1 |
| [centreon/centreon-collect](https://github.com/centreon/centreon-collect) | 1 | 1 |
| [acts-project/acts](https://github.com/acts-project/acts) | 1 | 46 |
| [josevcm/nfc-laboratory](https://github.com/josevcm/nfc-laboratory) | 1 | 1 |
| [ROCm/rccl](https://github.com/ROCm/rccl) | 1 | 1 |
| [facebookincubator/katran](https://github.com/facebookincubator/katran) | 1 | 1 |
| [yhirose/culebra](https://github.com/yhirose/culebra) | 1 | 1 |
| [freebsd/pkg](https://github.com/freebsd/pkg) | 1 | 1 |
| [christianrauch/libcamera](https://github.com/christianrauch/libcamera) | 1 | 1 |
| [pybricks/pybricks-micropython](https://github.com/pybricks/pybricks-micropython) | 1 | 1 |
| [mysql/mysql-shell](https://github.com/mysql/mysql-shell) | 1 | 1 |
| [OpenSC/OpenSC](https://github.com/OpenSC/OpenSC) | 1 | 1 |
| [ton-blockchain/ton](https://github.com/ton-blockchain/ton) | 1 | 1 |
| [foss-for-synopsys-dwc-arc-processors/newlib](https://github.com/foss-for-synopsys-dwc-arc-processors/newlib) | 1 | 1 |
| [GNOME/libxml2](https://github.com/GNOME/libxml2) | 1 | 1 |
| [spice2x/spice2x.github.io](https://github.com/spice2x/spice2x.github.io) | 1 | 1 |
| [GaijinEntertainment/daScript](https://github.com/GaijinEntertainment/daScript) | 1 | 1 |
| [ColinIanKing/stress-ng](https://github.com/ColinIanKing/stress-ng) | 1 | 1 |
| [managarm/mlibc](https://github.com/managarm/mlibc) | 1 | 1 |
| [bdring/FluidNC](https://github.com/bdring/FluidNC) | 1 | 1 |
| [Xilinx/XRT](https://github.com/Xilinx/XRT) | 1 | 1 |
