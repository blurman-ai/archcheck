# Циклы зависимостей в C++ репозиториях — разбор с кликабельными ссылками

> Источник: `experiments/ai_repo_run/structural_census.tsv`. Из census отобраны строки
> `verdict==VIOLATION_KEPT` с непустым `sccs_cyclic > 0` — **276 репозиториев** имеют хотя бы один
> циклический SCC во include-графе. Метрика `sccs_cyclic` = число сильно-связных компонент размером
> ≥2 (узлы, ссылающиеся друг на друга по кругу).

## Метод

1. Отбор 276 реп с циклами, сортировка по `sccs_cyclic` убыв.
2. Для **топ-20** — клон под `/home/localadm/oss/<owner_repo>`, `git rev-parse HEAD` (12 симв),
   `archcheck --graph` + `archcheck --format json` (правило SF.9 печатает путь цикла
   `A → B → A`).
3. Каждый цикл **проверен вручную** в коде клона — открыты реальные строки `#include`. Помечено
   ✅ (взаимный include виден в коде) или ⚠️ (вероятный FP тула / спорный случай).

## Главный вывод (честно)

archcheck SF.9 **переоценивает** число «архитектурных» циклов, потому что не отличает три класса:

- **Идиома template-split** (`foo.h` тянет `foo.inl`/`foo.ipp`/`foo.tmpl.h`/`foo.hxx` в самом низу,
  а тот через `#pragma once` тянет `foo.h` обратно). Это **один логический модуль**, разрезанный на
  декларацию+имплементацию. Технически петля есть, архитектурного цикла нет → **FP для целей
  «архитектурный долг»**. Доминирует у `nv-legate/legate` (все 163!), `acts`, частично `allwpilib`,
  `pyroscope-dotnet`/`opentelemetry` (`.inl`), `tau-lang` (`.tmpl.h`), `microsoft/WSL` (`.hxx`).
- **Mis-resolution системного заголовка** — `#include <string.h>` (libc) ошибочно сопоставлен с
  проектным `spa/utils/string.h`. Видно у PipeWire.
- **Настоящий взаимный include между разными компонентами** — `A.h` и `B.h` тянут друг друга. Это
  и есть Lakos-цикл, который SF.9 призван ловить. Видно у DPDK, strongswan, ompi, mingw, inxware,
  IncuNest, LineageOS, ROCm, TF-M, opentelemetry(coreclr), allwpilib(proto).

**Из топ-20 разобрано 19 (pyA2L дал расхождение тула — см. ниже). Из них ~12 содержат реальные
циклы (✅), ~7 — преимущественно/целиком идиома или mis-resolution (⚠️).** То есть «сырость» v0.1
существенная: верхушка рейтинга (legate 163, acts 46) — это шум `.inl`-идиомы, а не долг.

---

## Топ-20 — разбор

### 1. nv-legate/legate — `sccs_cyclic=163` ⚠️ (идиома `.h ↔ .inl`)
`sha=d473e217ed1f`. **Все 163 цикла — один и тот же паттерн**: заголовок тянет свой `.inl` внизу,
`.inl` тянет заголовок обратно. Это header+inline-impl одного модуля, не архитектурный цикл.

- [src/cpp/legate/comm/communicator.h:60](https://github.com/nv-legate/legate/blob/d473e217ed1f/src/cpp/legate/comm/communicator.h#L60)
- [src/cpp/legate/comm/communicator.inl:9](https://github.com/nv-legate/legate/blob/d473e217ed1f/src/cpp/legate/comm/communicator.inl#L9)

```cpp
// src/cpp/legate/comm/communicator.h:60  (хвост файла)
#include <legate/comm/communicator.inl>
// src/cpp/legate/comm/communicator.inl:7-9
#pragma once
#include <legate/comm/communicator.h>
```
**Вердикт: ⚠️ FP** — `#pragma once` рвёт петлю; это намеренный single-unit split. 163 «цикла» = 163
заголовка с inline-частью.

### 2. LineageOS/android_kernel_asus_sm8350 — `sccs_cyclic=73` ✅ (плюс шум вендора)
`sha=6022487bf89a`. Это ядро Linux (C, не C++), но взаимные include реальны.

- [arch/arm/plat-samsung/include/plat/map-s3c.h:74](https://github.com/LineageOS/android_kernel_asus_sm8350/blob/6022487bf89a/arch/arm/plat-samsung/include/plat/map-s3c.h#L74)
- [arch/arm/plat-samsung/include/plat/map-s5p.h:20](https://github.com/LineageOS/android_kernel_asus_sm8350/blob/6022487bf89a/arch/arm/plat-samsung/include/plat/map-s5p.h#L20)

```cpp
// plat/map-s3c.h:74
#include <plat/map-s5p.h>
// plat/map-s5p.h:20
#include <plat/map-s3c.h>
```
**Вердикт: ✅ подтверждён** (взаимный include двух platform-map заголовков). Оговорка: это
вендоренное ядро, не авторский код репы.

### 3. acts-project/acts — `sccs_cyclic=46` ⚠️ (идиома `.hpp ↔ .ipp`)
`sha=04ebfbaa26ee`. Тот же template-split, что у legate, только расширение `.ipp`.

- [Alignment/include/ActsAlignment/Kernel/Alignment.hpp:261](https://github.com/acts-project/acts/blob/04ebfbaa26ee/Alignment/include/ActsAlignment/Kernel/Alignment.hpp#L261)
- [Alignment/include/ActsAlignment/Kernel/Alignment.ipp:11](https://github.com/acts-project/acts/blob/04ebfbaa26ee/Alignment/include/ActsAlignment/Kernel/Alignment.ipp#L11)

```cpp
// Alignment.hpp:261
#include "ActsAlignment/Kernel/Alignment.ipp"
// Alignment.ipp:11
#include "ActsAlignment/Kernel/Alignment.hpp"
```
**Вердикт: ⚠️ FP** (decl/impl split).

### 4. wpilibsuite/allwpilib — `sccs_cyclic=34` ✅ (proto-serialization, спорный)
`sha=ddf93062641d`. Заголовок типа тянет свой `proto/*Proto.hpp` (сериализация), а тот — тип обратно.

- [wpimath/.../controller/ArmFeedforward.hpp:277](https://github.com/wpilibsuite/allwpilib/blob/ddf93062641d/wpimath/src/main/native/include/wpi/math/controller/ArmFeedforward.hpp#L277)
- [wpimath/.../controller/proto/ArmFeedforwardProto.hpp:8](https://github.com/wpilibsuite/allwpilib/blob/ddf93062641d/wpimath/src/main/native/include/wpi/math/controller/proto/ArmFeedforwardProto.hpp#L8)

```cpp
// ArmFeedforward.hpp:277
#include "wpi/math/controller/proto/ArmFeedforwardProto.hpp"
// proto/ArmFeedforwardProto.hpp:8
#include "wpi/math/controller/ArmFeedforward.hpp"
```
**Вердикт: ✅ подтверждён** — два *разных* заголовка ссылаются друг на друга. Это реальная
двунаправленная связь (тип ↔ его proto-traits); разорвать можно forward-декларацией.

### 5. open-telemetry/opentelemetry-dotnet-instrumentation — `sccs_cyclic=33` ✅ (но вендор coreclr)
`sha=4773f50fae67`. Циклы внутри вендоренного `lib/coreclr` (исходники .NET runtime), часть —
`.inl`-идиома, часть — реальные взаимные include.

- [.../lib/coreclr/src/inc/clrconfig.h:3376](https://github.com/open-telemetry/opentelemetry-dotnet-instrumentation/blob/4773f50fae67/src/OpenTelemetry.AutoInstrumentation.Native/lib/coreclr/src/inc/clrconfig.h#L3376)
- [.../lib/coreclr/src/inc/utilcode.h:14](https://github.com/open-telemetry/opentelemetry-dotnet-instrumentation/blob/4773f50fae67/src/OpenTelemetry.AutoInstrumentation.Native/lib/coreclr/src/inc/utilcode.h#L14)

```cpp
// clrconfig.h:3376
#include "utilcode.h"
// utilcode.h:14
#include "clrconfig.h"
```
**Вердикт: ✅ подтверждён** (взаимный include), но это **вендоренный CoreCLR**, не авторский код.

### 6. mingw-w64/mingw-w64 — `sccs_cyclic=32` ✅
`sha=b536c4fdb038`. Каждый CRT-заголовок тянет свой `sec_api/*_s.h` (Secure CRT variant), который
тянет базовый обратно.

- [mingw-w64-headers/crt/conio.h:200](https://github.com/mingw-w64/mingw-w64/blob/b536c4fdb038/mingw-w64-headers/crt/conio.h#L200)
- [mingw-w64-headers/crt/sec_api/conio_s.h:10](https://github.com/mingw-w64/mingw-w64/blob/b536c4fdb038/mingw-w64-headers/crt/sec_api/conio_s.h#L10)

```cpp
// crt/conio.h:200
#include <sec_api/conio_s.h>
// crt/sec_api/conio_s.h:10
#include <conio.h>
```
**Вердикт: ✅ подтверждён** (base ↔ secure-variant, реальная петля). Это системные заголовки —
паттерн намеренный, но архитектурно это всё равно цикл.

### 7. christoph2/pyA2L — `sccs_cyclic=22` ⚠️ (расхождение тула)
`sha=116f0cdfc7c0`. **`--graph` даёт `sccs_cyclic=22`, но правило SF.9 в `--format json` печатает 0
циклов.** Внутренняя несогласованность archcheck v0.1: graph-метрика и SF.9-репортер считают
циклы по-разному (вероятно SF.9 отфильтровал, а graph — нет). Не смог подтвердить ни один цикл
кодом за разумное время.
**Вердикт: ⚠️ нужна доработка тула** (bug: graph vs SF.9 расходятся).

### 8. IDNI/tau-lang — `sccs_cyclic=15` ⚠️/частично ✅ (идиома `.tmpl.h` + длинные цепи)
`sha=401d756bdc29`. Часть — `api.h ↔ api.tmpl.h` (template-split), но есть и **длинные цепи** через
несколько `.tmpl.h` (4-узловые), которые ближе к реальной связности.

- [src/api.h:508](https://github.com/IDNI/tau-lang/blob/401d756bdc29/src/api.h#L508) тянет `api.tmpl.*`
- [src/api.tmpl.h:3](https://github.com/IDNI/tau-lang/blob/401d756bdc29/src/api.tmpl.h#L3) → `api.h`

```text
src/ba_constants.h → src/tau_tree.h → src/tau_tree.tmpl.h → src/tau_tree_node.tmpl.h → src/ba_constants.h
```
**Вердикт: ⚠️ преимущественно идиома** (`.tmpl.h` split), но 4-узловые цепи стоит посмотреть глубже.

### 9. DPDK/dpdk — `sccs_cyclic=14` ✅ (яркий, несколько реальных)
`sha=4757b8df04b6`. Несколько настоящих взаимных include в драйверах.

**Пара roc_api ↔ roc_bphy_cgx** (cnxk common):
- [drivers/common/cnxk/roc_api.h:81](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/cnxk/roc_api.h#L81)
- [drivers/common/cnxk/roc_bphy_cgx.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/cnxk/roc_bphy_cgx.h#L10)

```cpp
// roc_api.h:81
#include "roc_bphy_cgx.h"
// roc_bphy_cgx.h:10
#include "roc_api.h"
```

**Пара cperf ↔ cperf_ops** (test-crypto-perf):
- [app/test-crypto-perf/cperf.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/app/test-crypto-perf/cperf.h#L10)
- [app/test-crypto-perf/cperf_ops.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/app/test-crypto-perf/cperf_ops.h#L10)

```cpp
// cperf.h:10
#include "cperf_ops.h"
// cperf_ops.h:10
#include "cperf.h"
```

**3-узловой цикл zsda** (device → comp_pmd → qp → device):
- [drivers/common/zsda/zsda_device.h:9](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/zsda/zsda_device.h#L9)
- [drivers/compress/zsda/zsda_comp_pmd.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/compress/zsda/zsda_comp_pmd.h#L10)
- [drivers/common/zsda/zsda_qp.h:10](https://github.com/DPDK/dpdk/blob/4757b8df04b6/drivers/common/zsda/zsda_qp.h#L10)

**Вердикт: ✅ подтверждён** — несколько настоящих 2- и 3-узловых циклов в авторском коде драйверов.

### 10. medicalopenworld/IncuNest — `sccs_cyclic=12` ✅ (SquareLine-генерация)
`sha=d08131a23f0e`. Сгенерённый SquareLine Studio UI: каждый экран — `ui.h ↔ ui_helpers.h`.

- [.../Screen_Alarms/ui.h:15](https://github.com/medicalopenworld/IncuNest/blob/d08131a23f0e/Firmware/Display_HMI/SquareLineProject/Exports/Exports%20ESP32%20HMI%20Interface%20Screen_Alarms/ui.h#L15)
- [.../Screen_Alarms/ui_helpers.h:13](https://github.com/medicalopenworld/IncuNest/blob/d08131a23f0e/Firmware/Display_HMI/SquareLineProject/Exports/Exports%20ESP32%20HMI%20Interface%20Screen_Alarms/ui_helpers.h#L13)

```cpp
// ui.h:15
#include "ui_helpers.h"
// ui_helpers.h:13
#include "ui.h"
```
**Вердикт: ✅ подтверждён** (взаимный include), но это **генерённый код** (SquareLine), повторён по
экрану — отсюда 12 одинаковых пар.

### 11. grafana/pyroscope-dotnet — `sccs_cyclic=11` ✅ (но вендор coreclr)
`sha=bd772914bd1e`. Тот же вендоренный CoreCLR, что у opentelemetry (#5): `bitmask.h ↔ bitmask.inl`
(идиома) + `clrconfig.h ↔ utilcode.h` (реальный).

- [shared/.../coreclr/src/inc/clrconfig.h](https://github.com/grafana/pyroscope-dotnet/blob/bd772914bd1e/shared/src/native-lib/coreclr/src/inc/clrconfig.h)
- [shared/.../coreclr/src/inc/utilcode.h](https://github.com/grafana/pyroscope-dotnet/blob/bd772914bd1e/shared/src/native-lib/coreclr/src/inc/utilcode.h)

**Вердикт: ✅ реальный взаимный include, но вендор** (CoreCLR), не код Grafana.

### 12. danielnachun/recipe_staging — `sccs_cyclic=11` ⚠️ (идиома `.hh ↔ .hpp`, вендор)
`sha=6476ac7a73f7`. Это conda-staging репа; циклы — в вендоренном `bcl2fastq2`, паттерн
`FastIo.hh ↔ FastIo.hpp` (decl/impl split).

- [bcl2fastq2/.../common/FastIo.hh](https://github.com/danielnachun/recipe_staging/blob/6476ac7a73f7/bcl2fastq2/bcl2fastq/src/cxx/include/common/FastIo.hh)
- [bcl2fastq2/.../common/FastIo.hpp](https://github.com/danielnachun/recipe_staging/blob/6476ac7a73f7/bcl2fastq2/bcl2fastq/src/cxx/include/common/FastIo.hpp)

**Вердикт: ⚠️ идиома + вендор** — `.hh` декларация тянет `.hpp` имплементацию.

### 13. zephyrproject-rtos/trusted-firmware-m — `sccs_cyclic=9` ✅
`sha=98d32c0f1fc9`. PSA interface: `api_broker.h ↔ client.h` — реальный взаимный include.

- [interface/include/psa/api_broker.h:14](https://github.com/zephyrproject-rtos/trusted-firmware-m/blob/98d32c0f1fc9/interface/include/psa/api_broker.h#L14)
- [interface/include/psa/client.h:14](https://github.com/zephyrproject-rtos/trusted-firmware-m/blob/98d32c0f1fc9/interface/include/psa/client.h#L14)

```cpp
// api_broker.h:14
#include "client.h"
// client.h:14
#include "psa/api_broker.h"
```
Плюс длинная цепь через `crypto.h → crypto_struct.h → ... → cc3xx_*` (вендор cryptocell-312).
**Вердикт: ✅ подтверждён** (PSA api_broker/client — авторский цикл интерфейса).

### 14. strongswan/strongswan — `sccs_cyclic=9` ✅ (яркий, авторский)
`sha=5fc403702b01`. Настоящие архитектурные циклы в libcharon.

**attribute_handler ↔ ike_sa:**
- [src/libcharon/attributes/attribute_handler.h:27](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/attributes/attribute_handler.h#L27)
- [src/libcharon/sa/ike_sa.h:37](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/sa/ike_sa.h#L37)

```cpp
// attributes/attribute_handler.h:27
#include <sa/ike_sa.h>
// sa/ike_sa.h:37
#include <attributes/attribute_handler.h>
```

**bus ↔ logger:**
- [src/libcharon/bus/bus.h:37](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/bus/bus.h#L37)
- [src/libcharon/bus/listeners/logger.h:27](https://github.com/strongswan/strongswan/blob/5fc403702b01/src/libcharon/bus/listeners/logger.h#L27)

```cpp
// bus/bus.h:37
#include <bus/listeners/logger.h>
// bus/listeners/logger.h:27
#include <bus/bus.h>
```
**Вердикт: ✅ подтверждён** — взаимные include разных подсистем (sa ↔ attributes, bus ↔ logger).
Классический Lakos-цикл в авторском коде.

### 15. ROCm/composable_kernel — `sccs_cyclic=9` ✅
`sha=2c363870d932`. `binary_element_wise_operation.hpp ↔ element_wise_operation.hpp`.

- [include/ck/tensor_operation/gpu/element/binary_element_wise_operation.hpp:7](https://github.com/ROCm/composable_kernel/blob/2c363870d932/include/ck/tensor_operation/gpu/element/binary_element_wise_operation.hpp#L7)
- [include/ck/tensor_operation/gpu/element/element_wise_operation.hpp](https://github.com/ROCm/composable_kernel/blob/2c363870d932/include/ck/tensor_operation/gpu/element/element_wise_operation.hpp)

```cpp
// binary_element_wise_operation.hpp:7
#include "ck/tensor_operation/gpu/element/element_wise_operation.hpp"
// element_wise_operation.hpp:9
#include "ck/tensor_operation/gpu/element/binary_element_wise_operation.hpp"
```
**Вердикт: ✅ подтверждён** (взаимный include двух разных операций).

### 16. PipeWire/pipewire — `sccs_cyclic=9` ✅+⚠️ (смешанный)
`sha=bb634fb0f9f1`. **Реальный** цикл type-info, и **FP** на `string.h`.

**✅ type-info (monitor ↔ utils):**
- [spa/include/spa/monitor/type-info.h:8](https://github.com/PipeWire/pipewire/blob/bb634fb0f9f1/spa/include/spa/monitor/type-info.h#L8)
- [spa/include/spa/utils/type-info.h:13](https://github.com/PipeWire/pipewire/blob/bb634fb0f9f1/spa/include/spa/utils/type-info.h#L13)

```cpp
// monitor/type-info.h:8
#include <spa/utils/type-info.h>
// utils/type-info.h:13
#include <spa/monitor/type-info.h>
```

**⚠️ defs.h ↔ string.h — FP тула:** `defs.h:11` это `#include <string.h>` (C-стандарт, угловые
скобки), но archcheck сопоставил его с проектным `spa/utils/string.h`.
- [spa/include/spa/utils/defs.h:11](https://github.com/PipeWire/pipewire/blob/bb634fb0f9f1/spa/include/spa/utils/defs.h#L11) — `#include <string.h>` (libc!)

**Вердикт: ✅ type-info реальный; ⚠️ defs/string — mis-resolution системного заголовка (bug тула).**

### 17. open-mpi/ompi — `sccs_cyclic=9` ✅
`sha=0ea6ebd628b2`. `coll_han.h ↔ coll_han_dynamic.h`.

- [ompi/mca/coll/han/coll_han.h:49](https://github.com/open-mpi/ompi/blob/0ea6ebd628b2/ompi/mca/coll/han/coll_han.h#L49)
- [ompi/mca/coll/han/coll_han_dynamic.h:27](https://github.com/open-mpi/ompi/blob/0ea6ebd628b2/ompi/mca/coll/han/coll_han_dynamic.h#L27)

```cpp
// coll_han.h:49
#include "ompi/mca/coll/han/coll_han_dynamic.h"
// coll_han_dynamic.h:27
#include "ompi/mca/coll/han/coll_han.h"
```
**Вердикт: ✅ подтверждён** (взаимный include в HAN collective).

### 18. inxware/ert-components — `sccs_cyclic=9` ✅ (автор сам признаёт цикл)
`sha=19db6242917d`. `widget.h ↔ widget_ui.h` — и в коде есть **комментарий автора про
circular-include**.

- [Common/HAL/graphics/widget.h:88](https://github.com/inxware/ert-components/blob/19db6242917d/Common/HAL/graphics/widget.h#L88)
- [Common/HAL/graphics/widget_ui.h:22](https://github.com/inxware/ert-components/blob/19db6242917d/Common/HAL/graphics/widget_ui.h#L22)

```cpp
// widget.h:51  /* Define types - must be before widget_ui.h include to avoid circular-include ordering issues */
// widget.h:88
#include "widget_ui.h"
// widget_ui.h:22
#include "widget.h" // needed for enums of widget types
```
**Вердикт: ✅ подтверждён** — самый честный кейс: автор прямо пишет про «circular-include ordering
issues» и тасует объявления, чтобы петля компилировалась.

### 19. nrfconnect/sdk-trusted-firmware-m — `sccs_cyclic=8` ✅
`sha=c9f73070ad6d`. Форк TF-M, тот же `api_broker.h ↔ client.h`, что в #13.

- [interface/include/psa/api_broker.h:14](https://github.com/nrfconnect/sdk-trusted-firmware-m/blob/c9f73070ad6d/interface/include/psa/api_broker.h#L14)
- [interface/include/psa/client.h:14](https://github.com/nrfconnect/sdk-trusted-firmware-m/blob/c9f73070ad6d/interface/include/psa/client.h#L14)

**Вердикт: ✅ подтверждён** (PSA api_broker/client).

### 20. microsoft/WSL — `sccs_cyclic=8` ⚠️ (идиома `.h ↔ .hxx`)
`sha=be1afc5f9a26`. netlinkutil: каждый класс — `Foo.h` тянет `Foo.hxx` внизу, `Foo.hxx` тянет
`Foo.h` через `#pragma once`. Template-impl split с расширением `.hxx`.

- [src/linux/netlinkutil/Forwarder.h:32](https://github.com/microsoft/WSL/blob/be1afc5f9a26/src/linux/netlinkutil/Forwarder.h#L32)
- [src/linux/netlinkutil/Forwarder.hxx:9](https://github.com/microsoft/WSL/blob/be1afc5f9a26/src/linux/netlinkutil/Forwarder.hxx#L9)

```cpp
// Forwarder.h:32  (хвост)
#include "Forwarder.hxx"
// Forwarder.hxx:3-9
#pragma once
#include "Forwarder.h"
```
**Вердикт: ⚠️ FP** (decl/impl split, как legate/acts).

---

## Сводка по топ-20

| # | репо | sccs_cyclic | вердикт | природа |
|---|---|---|---|---|
| 1 | nv-legate/legate | 163 | ⚠️ | идиома `.h↔.inl` (×163) |
| 2 | LineageOS/android_kernel_asus_sm8350 | 73 | ✅ | взаимные include (вендоренное ядро) |
| 3 | acts-project/acts | 46 | ⚠️ | идиома `.hpp↔.ipp` |
| 4 | wpilibsuite/allwpilib | 34 | ✅ | type ↔ proto-traits |
| 5 | open-telemetry/...-instrumentation | 33 | ✅ | реально, но вендор CoreCLR |
| 6 | mingw-w64/mingw-w64 | 32 | ✅ | base CRT ↔ secure-variant |
| 7 | christoph2/pyA2L | 22 | ⚠️ | расхождение тула (graph≠SF.9) |
| 8 | IDNI/tau-lang | 15 | ⚠️ | идиома `.tmpl.h` + длинные цепи |
| 9 | DPDK/dpdk | 14 | ✅ | несколько 2/3-узловых, авторские |
| 10 | medicalopenworld/IncuNest | 12 | ✅ | `ui↔ui_helpers` (генерённый SquareLine) |
| 11 | grafana/pyroscope-dotnet | 11 | ✅ | реально, но вендор CoreCLR |
| 12 | danielnachun/recipe_staging | 11 | ⚠️ | идиома `.hh↔.hpp` (вендор) |
| 13 | zephyrproject-rtos/trusted-firmware-m | 9 | ✅ | PSA api_broker↔client |
| 14 | strongswan/strongswan | 9 | ✅ | авторские циклы libcharon |
| 15 | ROCm/composable_kernel | 9 | ✅ | element-wise ops взаимно |
| 16 | PipeWire/pipewire | 9 | ✅/⚠️ | type-info реален; string.h — FP |
| 17 | open-mpi/ompi | 9 | ✅ | HAN collective взаимно |
| 18 | inxware/ert-components | 9 | ✅ | автор сам признаёт цикл |
| 19 | nrfconnect/sdk-trusted-firmware-m | 8 | ✅ | PSA api_broker↔client (форк TF-M) |
| 20 | microsoft/WSL | 8 | ⚠️ | идиома `.h↔.hxx` |

**Итог топ-20:** ~12 ✅ подтверждённых (реальный взаимный include), ~7 ⚠️ (идиома decl/impl-split
или mis-resolution), 1 расхождение тула. Половина «закольцованности» в верхушке рейтинга — это
шум `.inl/.ipp/.hxx`-идиомы, а не архитектурный долг.

**Рекомендация для archcheck:** SF.9 должен исключать петлю `X.h ↔ X.{inl,ipp,tmpl.h,hxx,tcc}`
когда оба файла — один stem и `.inl/.ipp/...` включается из хвоста `.h` (template-split идиома). Это
радикально очистит рейтинг (legate с 163 → ~0). Плюс пофиксить mis-resolution системных заголовков
(`<string.h>` ≠ проектный `string.h`) и расхождение graph-метрики с SF.9 (pyA2L).

---

## Полная таблица всех 276 реп с циклами

| репо | sccs_cyclic | ссылка |
|---|---|---|
| nv-legate/legate | 163 | [nv-legate/legate](https://github.com/nv-legate/legate) |
| LineageOS/android_kernel_asus_sm8350 | 73 | [LineageOS/android_kernel_asus_sm8350](https://github.com/LineageOS/android_kernel_asus_sm8350) |
| acts-project/acts | 46 | [acts-project/acts](https://github.com/acts-project/acts) |
| wpilibsuite/allwpilib | 34 | [wpilibsuite/allwpilib](https://github.com/wpilibsuite/allwpilib) |
| open-telemetry/opentelemetry-dotnet-instrumentation | 33 | [open-telemetry/opentelemetry-dotnet-instrumentation](https://github.com/open-telemetry/opentelemetry-dotnet-instrumentation) |
| mingw-w64/mingw-w64 | 32 | [mingw-w64/mingw-w64](https://github.com/mingw-w64/mingw-w64) |
| christoph2/pyA2L | 22 | [christoph2/pyA2L](https://github.com/christoph2/pyA2L) |
| IDNI/tau-lang | 15 | [IDNI/tau-lang](https://github.com/IDNI/tau-lang) |
| DPDK/dpdk | 14 | [DPDK/dpdk](https://github.com/DPDK/dpdk) |
| medicalopenworld/IncuNest | 12 | [medicalopenworld/IncuNest](https://github.com/medicalopenworld/IncuNest) |
| grafana/pyroscope-dotnet | 11 | [grafana/pyroscope-dotnet](https://github.com/grafana/pyroscope-dotnet) |
| danielnachun/recipe_staging | 11 | [danielnachun/recipe_staging](https://github.com/danielnachun/recipe_staging) |
| zephyrproject-rtos/trusted-firmware-m | 9 | [zephyrproject-rtos/trusted-firmware-m](https://github.com/zephyrproject-rtos/trusted-firmware-m) |
| strongswan/strongswan | 9 | [strongswan/strongswan](https://github.com/strongswan/strongswan) |
| ROCm/composable_kernel | 9 | [ROCm/composable_kernel](https://github.com/ROCm/composable_kernel) |
| PipeWire/pipewire | 9 | [PipeWire/pipewire](https://github.com/PipeWire/pipewire) |
| open-mpi/ompi | 9 | [open-mpi/ompi](https://github.com/open-mpi/ompi) |
| inxware/ert-components | 9 | [inxware/ert-components](https://github.com/inxware/ert-components) |
| nrfconnect/sdk-trusted-firmware-m | 8 | [nrfconnect/sdk-trusted-firmware-m](https://github.com/nrfconnect/sdk-trusted-firmware-m) |
| microsoft/WSL | 8 | [microsoft/WSL](https://github.com/microsoft/WSL) |
| xen-project/xen | 6 | [xen-project/xen](https://github.com/xen-project/xen) |
| sxs-collaboration/spectre | 6 | [sxs-collaboration/spectre](https://github.com/sxs-collaboration/spectre) |
| jjuran/metamage_1 | 6 | [jjuran/metamage_1](https://github.com/jjuran/metamage_1) |
| canmeng12/packages | 6 | [canmeng12/packages](https://github.com/canmeng12/packages) |
| Bipto/Nexus | 6 | [Bipto/Nexus](https://github.com/Bipto/Nexus) |
| aravindkrishnaswamy/RISE | 6 | [aravindkrishnaswamy/RISE](https://github.com/aravindkrishnaswamy/RISE) |
| webserver-llc/angie | 5 | [webserver-llc/angie](https://github.com/webserver-llc/angie) |
| Washington-University/workbench | 5 | [Washington-University/workbench](https://github.com/Washington-University/workbench) |
| RebelToolbox/RebelEngine | 5 | [RebelToolbox/RebelEngine](https://github.com/RebelToolbox/RebelEngine) |
| pmodels/mpich | 5 | [pmodels/mpich](https://github.com/pmodels/mpich) |
| mjagdis-imports/sdcc-svn | 5 | [mjagdis-imports/sdcc-svn](https://github.com/mjagdis-imports/sdcc-svn) |
| kokkos/kokkos | 5 | [kokkos/kokkos](https://github.com/kokkos/kokkos) |
| KallistiOS/KallistiOS | 5 | [KallistiOS/KallistiOS](https://github.com/KallistiOS/KallistiOS) |
| intel/intel-graphics-compiler | 5 | [intel/intel-graphics-compiler](https://github.com/intel/intel-graphics-compiler) |
| icssw-org/MeshCom-Firmware | 5 | [icssw-org/MeshCom-Firmware](https://github.com/icssw-org/MeshCom-Firmware) |
| Icinga/icinga2 | 5 | [Icinga/icinga2](https://github.com/Icinga/icinga2) |
| elogind/elogind | 5 | [elogind/elogind](https://github.com/elogind/elogind) |
| eclipse-openj9/openj9-omr | 5 | [eclipse-openj9/openj9-omr](https://github.com/eclipse-openj9/openj9-omr) |
| eclipse-omr/omr | 5 | [eclipse-omr/omr](https://github.com/eclipse-omr/omr) |
| topling/toplingdb | 4 | [topling/toplingdb](https://github.com/topling/toplingdb) |
| tksuoran/erhe | 4 | [tksuoran/erhe](https://github.com/tksuoran/erhe) |
| stillwater-sc/universal | 4 | [stillwater-sc/universal](https://github.com/stillwater-sc/universal) |
| Slicer/Slicer | 4 | [Slicer/Slicer](https://github.com/Slicer/Slicer) |
| PhoshMobi/phoc | 4 | [PhoshMobi/phoc](https://github.com/PhoshMobi/phoc) |
| OpendTect/OpendTect | 4 | [OpendTect/OpendTect](https://github.com/OpendTect/OpendTect) |
| olafhering/grub | 4 | [olafhering/grub](https://github.com/olafhering/grub) |
| meshtastic/firmware | 4 | [meshtastic/firmware](https://github.com/meshtastic/firmware) |
| Matthew-McRaven/Pepp | 4 | [Matthew-McRaven/Pepp](https://github.com/Matthew-McRaven/Pepp) |
| lttng/lttng-tools | 4 | [lttng/lttng-tools](https://github.com/lttng/lttng-tools) |
| hexagon-geo-surv/trusted-firmware-a | 4 | [hexagon-geo-surv/trusted-firmware-a](https://github.com/hexagon-geo-surv/trusted-firmware-a) |
| heatd/Onyx | 4 | [heatd/Onyx](https://github.com/heatd/Onyx) |
| GregorGullwi/FlashCpp | 4 | [GregorGullwi/FlashCpp](https://github.com/GregorGullwi/FlashCpp) |
| GNOME/mutter | 4 | [GNOME/mutter](https://github.com/GNOME/mutter) |
| facebook/openbmc | 4 | [facebook/openbmc](https://github.com/facebook/openbmc) |
| facebook/fboss | 4 | [facebook/fboss](https://github.com/facebook/fboss) |
| espressif/esp-iot-solution | 4 | [espressif/esp-iot-solution](https://github.com/espressif/esp-iot-solution) |
| Cockatrice/Cockatrice | 4 | [Cockatrice/Cockatrice](https://github.com/Cockatrice/Cockatrice) |
| ARM-software/arm-trusted-firmware | 4 | [ARM-software/arm-trusted-firmware](https://github.com/ARM-software/arm-trusted-firmware) |
| apache/axis-axis2-c-core | 4 | [apache/axis-axis2-c-core](https://github.com/apache/axis-axis2-c-core) |
| vvaltchev/tilck | 3 | [vvaltchev/tilck](https://github.com/vvaltchev/tilck) |
| tlapack/tlapack | 3 | [tlapack/tlapack](https://github.com/tlapack/tlapack) |
| sipwise/rtpengine | 3 | [sipwise/rtpengine](https://github.com/sipwise/rtpengine) |
| simonsj/haproxy | 3 | [simonsj/haproxy](https://github.com/simonsj/haproxy) |
| shader-slang/slang | 3 | [shader-slang/slang](https://github.com/shader-slang/slang) |
| OvenMediaLabs/OvenMediaEngine | 3 | [OvenMediaLabs/OvenMediaEngine](https://github.com/OvenMediaLabs/OvenMediaEngine) |
| OpenXiangShan/GEM5 | 3 | [OpenXiangShan/GEM5](https://github.com/OpenXiangShan/GEM5) |
| OpenDataPlane/odp | 3 | [OpenDataPlane/odp](https://github.com/OpenDataPlane/odp) |
| nss-dev/nss | 3 | [nss-dev/nss](https://github.com/nss-dev/nss) |
| microsoft/Teams-AdaptiveCards-Mobile | 3 | [microsoft/Teams-AdaptiveCards-Mobile](https://github.com/microsoft/Teams-AdaptiveCards-Mobile) |
| micropython/micropython | 3 | [micropython/micropython](https://github.com/micropython/micropython) |
| manticoresoftware/manticoresearch | 3 | [manticoresoftware/manticoresearch](https://github.com/manticoresoftware/manticoresearch) |
| haproxy/haproxy | 3 | [haproxy/haproxy](https://github.com/haproxy/haproxy) |
| FRRouting/frr | 3 | [FRRouting/frr](https://github.com/FRRouting/frr) |
| FreeRADIUS/freeradius-server | 3 | [FreeRADIUS/freeradius-server](https://github.com/FreeRADIUS/freeradius-server) |
| davischw/frr | 3 | [davischw/frr](https://github.com/davischw/frr) |
| daos-stack/daos | 3 | [daos-stack/daos](https://github.com/daos-stack/daos) |
| CueMol/cuemol2 | 3 | [CueMol/cuemol2](https://github.com/CueMol/cuemol2) |
| ClusterLabs/pacemaker | 3 | [ClusterLabs/pacemaker](https://github.com/ClusterLabs/pacemaker) |
| apache/mynewt-core | 3 | [apache/mynewt-core](https://github.com/apache/mynewt-core) |
| y-scope/clp | 2 | [y-scope/clp](https://github.com/y-scope/clp) |
| yandex/odyssey | 2 | [yandex/odyssey](https://github.com/yandex/odyssey) |
| tonioni/WinUAE | 2 | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) |
| surge-synthesizer/shortcircuit-xt | 2 | [surge-synthesizer/shortcircuit-xt](https://github.com/surge-synthesizer/shortcircuit-xt) |
| stellar/stellar-core | 2 | [stellar/stellar-core](https://github.com/stellar/stellar-core) |
| Starlink/ast | 2 | [Starlink/ast](https://github.com/Starlink/ast) |
| SSSD/sssd | 2 | [SSSD/sssd](https://github.com/SSSD/sssd) |
| sciapp/gr | 2 | [sciapp/gr](https://github.com/sciapp/gr) |
| schwabe/openvpn | 2 | [schwabe/openvpn](https://github.com/schwabe/openvpn) |
| revng/revng | 2 | [revng/revng](https://github.com/revng/revng) |
| qemu/ipxe | 2 | [qemu/ipxe](https://github.com/qemu/ipxe) |
| ornladios/ADIOS2 | 2 | [ornladios/ADIOS2](https://github.com/ornladios/ADIOS2) |
| openwrt/mt76 | 2 | [openwrt/mt76](https://github.com/openwrt/mt76) |
| OpenVPN/openvpn | 2 | [OpenVPN/openvpn](https://github.com/OpenVPN/openvpn) |
| OpenEnroth/OpenEnroth | 2 | [OpenEnroth/OpenEnroth](https://github.com/OpenEnroth/OpenEnroth) |
| OpenDataPlane/odp-dpdk | 2 | [OpenDataPlane/odp-dpdk](https://github.com/OpenDataPlane/odp-dpdk) |
| OpenChemistry/avogadrolibs | 2 | [OpenChemistry/avogadrolibs](https://github.com/OpenChemistry/avogadrolibs) |
| open5gs/open5gs | 2 | [open5gs/open5gs](https://github.com/open5gs/open5gs) |
| NVIDIA/nccl | 2 | [NVIDIA/nccl](https://github.com/NVIDIA/nccl) |
| NVIDIAGameWorks/dxvk-remix | 2 | [NVIDIAGameWorks/dxvk-remix](https://github.com/NVIDIAGameWorks/dxvk-remix) |
| NVIDIA/Fuser | 2 | [NVIDIA/Fuser](https://github.com/NVIDIA/Fuser) |
| microsoft/terminal | 2 | [microsoft/terminal](https://github.com/microsoft/terminal) |
| markaren/threepp | 2 | [markaren/threepp](https://github.com/markaren/threepp) |
| mariadb-corporation/mariadb-columnstore-engine | 2 | [mariadb-corporation/mariadb-columnstore-engine](https://github.com/mariadb-corporation/mariadb-columnstore-engine) |
| magao-x/MagAOX | 2 | [magao-x/MagAOX](https://github.com/magao-x/MagAOX) |
| m-ab-s/aom | 2 | [m-ab-s/aom](https://github.com/m-ab-s/aom) |
| lf-lang/reactor-c | 2 | [lf-lang/reactor-c](https://github.com/lf-lang/reactor-c) |
| leuat/TRSE | 2 | [leuat/TRSE](https://github.com/leuat/TRSE) |
| kotuku-group/kotuku | 2 | [kotuku-group/kotuku](https://github.com/kotuku-group/kotuku) |
| jdolan/quetoo | 2 | [jdolan/quetoo](https://github.com/jdolan/quetoo) |
| jank-lang/jank | 2 | [jank-lang/jank](https://github.com/jank-lang/jank) |
| ipxe/ipxe | 2 | [ipxe/ipxe](https://github.com/ipxe/ipxe) |
| ioccc-src/mkiocccentry | 2 | [ioccc-src/mkiocccentry](https://github.com/ioccc-src/mkiocccentry) |
| hexagon-geo-surv/barebox | 2 | [hexagon-geo-surv/barebox](https://github.com/hexagon-geo-surv/barebox) |
| HallofFamer/Lox2 | 2 | [HallofFamer/Lox2](https://github.com/HallofFamer/Lox2) |
| gnutls/gnutls | 2 | [gnutls/gnutls](https://github.com/gnutls/gnutls) |
| GNOME/gnome-software | 2 | [GNOME/gnome-software](https://github.com/GNOME/gnome-software) |
| GNOME/gegl | 2 | [GNOME/gegl](https://github.com/GNOME/gegl) |
| FujiNetWIFI/fujinet-firmware | 2 | [FujiNetWIFI/fujinet-firmware](https://github.com/FujiNetWIFI/fujinet-firmware) |
| flux-framework/flux-core | 2 | [flux-framework/flux-core](https://github.com/flux-framework/flux-core) |
| facebook/openr | 2 | [facebook/openr](https://github.com/facebook/openr) |
| espressif/esp-mqtt | 2 | [espressif/esp-mqtt](https://github.com/espressif/esp-mqtt) |
| dovecot/core | 2 | [dovecot/core](https://github.com/dovecot/core) |
| deskull-m/bakabakaband | 2 | [deskull-m/bakabakaband](https://github.com/deskull-m/bakabakaband) |
| cyrusimap/cyrus-imapd | 2 | [cyrusimap/cyrus-imapd](https://github.com/cyrusimap/cyrus-imapd) |
| corepunch/openwarcraft3 | 2 | [corepunch/openwarcraft3](https://github.com/corepunch/openwarcraft3) |
| CoolProp/CoolProp | 2 | [CoolProp/CoolProp](https://github.com/CoolProp/CoolProp) |
| contour-terminal/contour | 2 | [contour-terminal/contour](https://github.com/contour-terminal/contour) |
| ca2/operating_system-windows | 2 | [ca2/operating_system-windows](https://github.com/ca2/operating_system-windows) |
| barebox/barebox | 2 | [barebox/barebox](https://github.com/barebox/barebox) |
| baidxi/buildroot | 2 | [baidxi/buildroot](https://github.com/baidxi/buildroot) |
| apache/trafficserver | 2 | [apache/trafficserver](https://github.com/apache/trafficserver) |
| apache/nuttx-apps | 2 | [apache/nuttx-apps](https://github.com/apache/nuttx-apps) |
| acts-project/traccc | 2 | [acts-project/traccc](https://github.com/acts-project/traccc) |
| yhirose/culebra | 1 | [yhirose/culebra](https://github.com/yhirose/culebra) |
| Xilinx/XRT | 1 | [Xilinx/XRT](https://github.com/Xilinx/XRT) |
| xfce-mirror/xfce4-docklike-plugin | 1 | [xfce-mirror/xfce4-docklike-plugin](https://github.com/xfce-mirror/xfce4-docklike-plugin) |
| wled/WLED | 1 | [wled/WLED](https://github.com/wled/WLED) |
| wiredtiger/wiredtiger | 1 | [wiredtiger/wiredtiger](https://github.com/wiredtiger/wiredtiger) |
| VTT-ProperTune/OpenPFC | 1 | [VTT-ProperTune/OpenPFC](https://github.com/VTT-ProperTune/OpenPFC) |
| virtuoso/clap | 1 | [virtuoso/clap](https://github.com/virtuoso/clap) |
| vifm/vifm | 1 | [vifm/vifm](https://github.com/vifm/vifm) |
| Unispace365/DsQt | 1 | [Unispace365/DsQt](https://github.com/Unispace365/DsQt) |
| unikraft/unikraft | 1 | [unikraft/unikraft](https://github.com/unikraft/unikraft) |
| ts-factory/test-environment | 1 | [ts-factory/test-environment](https://github.com/ts-factory/test-environment) |
| ton-blockchain/ton | 1 | [ton-blockchain/ton](https://github.com/ton-blockchain/ton) |
| tomato64/tomato64 | 1 | [tomato64/tomato64](https://github.com/tomato64/tomato64) |
| timescale/timescaledb | 1 | [timescale/timescaledb](https://github.com/timescale/timescaledb) |
| ThomasGhione/chess_engine | 1 | [ThomasGhione/chess_engine](https://github.com/ThomasGhione/chess_engine) |
| sysprogs/openocd | 1 | [sysprogs/openocd](https://github.com/sysprogs/openocd) |
| streetpea/chiaki-ng | 1 | [streetpea/chiaki-ng](https://github.com/streetpea/chiaki-ng) |
| sswroom/SClass | 1 | [sswroom/SClass](https://github.com/sswroom/SClass) |
| spice2x/spice2x.github.io | 1 | [spice2x/spice2x.github.io](https://github.com/spice2x/spice2x.github.io) |
| shorepine/amy | 1 | [shorepine/amy](https://github.com/shorepine/amy) |
| sailfishos-mirror/valgrind | 1 | [sailfishos-mirror/valgrind](https://github.com/sailfishos-mirror/valgrind) |
| sailfishos-mirror/protobuf | 1 | [sailfishos-mirror/protobuf](https://github.com/sailfishos-mirror/protobuf) |
| sailfishos-mirror/ltp | 1 | [sailfishos-mirror/ltp](https://github.com/sailfishos-mirror/ltp) |
| sailfishos-mirror/curl | 1 | [sailfishos-mirror/curl](https://github.com/sailfishos-mirror/curl) |
| rvdbreemen/OTGW-firmware | 1 | [rvdbreemen/OTGW-firmware](https://github.com/rvdbreemen/OTGW-firmware) |
| RTEMS/sourceware-mirror-newlib-cygwin | 1 | [RTEMS/sourceware-mirror-newlib-cygwin](https://github.com/RTEMS/sourceware-mirror-newlib-cygwin) |
| ROCm/rccl | 1 | [ROCm/rccl](https://github.com/ROCm/rccl) |
| reupen/columns_ui | 1 | [reupen/columns_ui](https://github.com/reupen/columns_ui) |
| RediSearch/RediSearch | 1 | [RediSearch/RediSearch](https://github.com/RediSearch/RediSearch) |
| quakeforge/quakeforge | 1 | [quakeforge/quakeforge](https://github.com/quakeforge/quakeforge) |
| qt/qttools | 1 | [qt/qttools](https://github.com/qt/qttools) |
| qt/qtdoc | 1 | [qt/qtdoc](https://github.com/qt/qtdoc) |
| pybricks/pybricks-micropython | 1 | [pybricks/pybricks-micropython](https://github.com/pybricks/pybricks-micropython) |
| privatevoid-net/nix-super | 1 | [privatevoid-net/nix-super](https://github.com/privatevoid-net/nix-super) |
| PhoshMobi/stevia | 1 | [PhoshMobi/stevia](https://github.com/PhoshMobi/stevia) |
| paulfloyd/freebsd_valgrind | 1 | [paulfloyd/freebsd_valgrind](https://github.com/paulfloyd/freebsd_valgrind) |
| orioledb/orioledb | 1 | [orioledb/orioledb](https://github.com/orioledb/orioledb) |
| OP-TEE/optee_os | 1 | [OP-TEE/optee_os](https://github.com/OP-TEE/optee_os) |
| openzfs/zfs | 1 | [openzfs/zfs](https://github.com/openzfs/zfs) |
| openucx/ucx | 1 | [openucx/ucx](https://github.com/openucx/ucx) |
| OpenSC/OpenSC | 1 | [OpenSC/OpenSC](https://github.com/OpenSC/OpenSC) |
| openocd-org/openocd | 1 | [openocd-org/openocd](https://github.com/openocd-org/openocd) |
| open-mpi/hwloc | 1 | [open-mpi/hwloc](https://github.com/open-mpi/hwloc) |
| openbabel/openbabel | 1 | [openbabel/openbabel](https://github.com/openbabel/openbabel) |
| OpenAtom-Linyaps/linyaps | 1 | [OpenAtom-Linyaps/linyaps](https://github.com/OpenAtom-Linyaps/linyaps) |
| oktetlabs/test-environment | 1 | [oktetlabs/test-environment](https://github.com/oktetlabs/test-environment) |
| ofiwg/libfabric | 1 | [ofiwg/libfabric](https://github.com/ofiwg/libfabric) |
| ObjectVision/GeoDMS | 1 | [ObjectVision/GeoDMS](https://github.com/ObjectVision/GeoDMS) |
| nxp-imx/imx-optee-os | 1 | [nxp-imx/imx-optee-os](https://github.com/nxp-imx/imx-optee-os) |
| NVIDIA/edk2-nvidia | 1 | [NVIDIA/edk2-nvidia](https://github.com/NVIDIA/edk2-nvidia) |
| NixOS/nix | 1 | [NixOS/nix](https://github.com/NixOS/nix) |
| netxms/netxms | 1 | [netxms/netxms](https://github.com/netxms/netxms) |
| networkupstools/nut | 1 | [networkupstools/nut](https://github.com/networkupstools/nut) |
| nanomq/NanoNNG | 1 | [nanomq/NanoNNG](https://github.com/nanomq/NanoNNG) |
| mysql/mysql-shell | 1 | [mysql/mysql-shell](https://github.com/mysql/mysql-shell) |
| mne-tools/mne-cpp | 1 | [mne-tools/mne-cpp](https://github.com/mne-tools/mne-cpp) |
| MIPS/newlib | 1 | [MIPS/newlib](https://github.com/MIPS/newlib) |
| microsoft/DirectXShaderCompiler | 1 | [microsoft/DirectXShaderCompiler](https://github.com/microsoft/DirectXShaderCompiler) |
| MeshInspector/MeshLib | 1 | [MeshInspector/MeshLib](https://github.com/MeshInspector/MeshLib) |
| meganz/sdk | 1 | [meganz/sdk](https://github.com/meganz/sdk) |
| managarm/mlibc | 1 | [managarm/mlibc](https://github.com/managarm/mlibc) |
| MaaXYZ/MaaFramework | 1 | [MaaXYZ/MaaFramework](https://github.com/MaaXYZ/MaaFramework) |
| lustre/lustre-release | 1 | [lustre/lustre-release](https://github.com/lustre/lustre-release) |
| LuminariMUD/Luminari-Source | 1 | [LuminariMUD/Luminari-Source](https://github.com/LuminariMUD/Luminari-Source) |
| LouisBrunner/valgrind-macos | 1 | [LouisBrunner/valgrind-macos](https://github.com/LouisBrunner/valgrind-macos) |
| llr-cta/calin | 1 | [llr-cta/calin](https://github.com/llr-cta/calin) |
| linux-test-project/ltp | 1 | [linux-test-project/ltp](https://github.com/linux-test-project/ltp) |
| libretro/libretro-prboom | 1 | [libretro/libretro-prboom](https://github.com/libretro/libretro-prboom) |
| libcamera-org/libcamera | 1 | [libcamera-org/libcamera](https://github.com/libcamera-org/libcamera) |
| LBNL-ETA/Windows-CalcEngine | 1 | [LBNL-ETA/Windows-CalcEngine](https://github.com/LBNL-ETA/Windows-CalcEngine) |
| kromenak/gengine | 1 | [kromenak/gengine](https://github.com/kromenak/gengine) |
| kokkos/kokkos-kernels | 1 | [kokkos/kokkos-kernels](https://github.com/kokkos/kokkos-kernels) |
| KJ7LNW/xnec2c | 1 | [KJ7LNW/xnec2c](https://github.com/KJ7LNW/xnec2c) |
| Kitware/CMake | 1 | [Kitware/CMake](https://github.com/Kitware/CMake) |
| keycard-tech/keycard-shell | 1 | [keycard-tech/keycard-shell](https://github.com/keycard-tech/keycard-shell) |
| kevlu8/PZChessBot | 1 | [kevlu8/PZChessBot](https://github.com/kevlu8/PZChessBot) |
| KevinOConnor/klipper-dev | 1 | [KevinOConnor/klipper-dev](https://github.com/KevinOConnor/klipper-dev) |
| kbingham/libcamera | 1 | [kbingham/libcamera](https://github.com/kbingham/libcamera) |
| katsuster/newlib-cygwin | 1 | [katsuster/newlib-cygwin](https://github.com/katsuster/newlib-cygwin) |
| josevcm/nfc-laboratory | 1 | [josevcm/nfc-laboratory](https://github.com/josevcm/nfc-laboratory) |
| jermp/sshash | 1 | [jermp/sshash](https://github.com/jermp/sshash) |
| intel/compute-runtime | 1 | [intel/compute-runtime](https://github.com/intel/compute-runtime) |
| ilia-maslakov/mcdev | 1 | [ilia-maslakov/mcdev](https://github.com/ilia-maslakov/mcdev) |
| ibm-s390-linux/s390-tools | 1 | [ibm-s390-linux/s390-tools](https://github.com/ibm-s390-linux/s390-tools) |
| hydrobricks/hydrobricks | 1 | [hydrobricks/hydrobricks](https://github.com/hydrobricks/hydrobricks) |
| HermanChen/mpp | 1 | [HermanChen/mpp](https://github.com/HermanChen/mpp) |
| haxscramper/haxorg | 1 | [haxscramper/haxorg](https://github.com/haxscramper/haxorg) |
| HansKristian-Work/vkd3d-proton | 1 | [HansKristian-Work/vkd3d-proton](https://github.com/HansKristian-Work/vkd3d-proton) |
| grumpycoders/pcsx-redux | 1 | [grumpycoders/pcsx-redux](https://github.com/grumpycoders/pcsx-redux) |
| grommunio/gromox | 1 | [grommunio/gromox](https://github.com/grommunio/gromox) |
| GNOME/libxml2 | 1 | [GNOME/libxml2](https://github.com/GNOME/libxml2) |
| GNOME/gparted | 1 | [GNOME/gparted](https://github.com/GNOME/gparted) |
| GNOME/glib | 1 | [GNOME/glib](https://github.com/GNOME/glib) |
| GNOME/evolution-data-server | 1 | [GNOME/evolution-data-server](https://github.com/GNOME/evolution-data-server) |
| git-for-windows/msys2-runtime | 1 | [git-for-windows/msys2-runtime](https://github.com/git-for-windows/msys2-runtime) |
| gehelem/OST | 1 | [gehelem/OST](https://github.com/gehelem/OST) |
| GaijinEntertainment/daScript | 1 | [GaijinEntertainment/daScript](https://github.com/GaijinEntertainment/daScript) |
| fwupd/fwupd | 1 | [fwupd/fwupd](https://github.com/fwupd/fwupd) |
| freetype/freetype | 1 | [freetype/freetype](https://github.com/freetype/freetype) |
| freebsd/pkg | 1 | [freebsd/pkg](https://github.com/freebsd/pkg) |
| foss-for-synopsys-dwc-arc-processors/newlib | 1 | [foss-for-synopsys-dwc-arc-processors/newlib](https://github.com/foss-for-synopsys-dwc-arc-processors/newlib) |
| fluent/fluent-bit | 1 | [fluent/fluent-bit](https://github.com/fluent/fluent-bit) |
| FireSourcery/FireSourcery_Library | 1 | [FireSourcery/FireSourcery_Library](https://github.com/FireSourcery/FireSourcery_Library) |
| FEX-Emu/FEX | 1 | [FEX-Emu/FEX](https://github.com/FEX-Emu/FEX) |
| FEniCS/dolfinx | 1 | [FEniCS/dolfinx](https://github.com/FEniCS/dolfinx) |
| facebookincubator/katran | 1 | [facebookincubator/katran](https://github.com/facebookincubator/katran) |
| EVerest/EVerest | 1 | [EVerest/EVerest](https://github.com/EVerest/EVerest) |
| etlegacy/etlegacy | 1 | [etlegacy/etlegacy](https://github.com/etlegacy/etlegacy) |
| espressif/openocd-esp32 | 1 | [espressif/openocd-esp32](https://github.com/espressif/openocd-esp32) |
| esphome/esphome | 1 | [esphome/esphome](https://github.com/esphome/esphome) |
| esbmc/esbmc | 1 | [esbmc/esbmc](https://github.com/esbmc/esbmc) |
| erofs/erofs-utils | 1 | [erofs/erofs-utils](https://github.com/erofs/erofs-utils) |
| elf-audio/mzgl | 1 | [elf-audio/mzgl](https://github.com/elf-audio/mzgl) |
| digi-embedded/optee_os | 1 | [digi-embedded/optee_os](https://github.com/digi-embedded/optee_os) |
| cygwin/cygwin | 1 | [cygwin/cygwin](https://github.com/cygwin/cygwin) |
| curl/curl | 1 | [curl/curl](https://github.com/curl/curl) |
| craigbarnes/dte | 1 | [craigbarnes/dte](https://github.com/craigbarnes/dte) |
| conradhuebler/curcuma | 1 | [conradhuebler/curcuma](https://github.com/conradhuebler/curcuma) |
| compiler-research/CppInterOp | 1 | [compiler-research/CppInterOp](https://github.com/compiler-research/CppInterOp) |
| ColinIanKing/stress-ng | 1 | [ColinIanKing/stress-ng](https://github.com/ColinIanKing/stress-ng) |
| CodSpeedHQ/valgrind-codspeed | 1 | [CodSpeedHQ/valgrind-codspeed](https://github.com/CodSpeedHQ/valgrind-codspeed) |
| christianrauch/libcamera | 1 | [christianrauch/libcamera](https://github.com/christianrauch/libcamera) |
| ChristianGaser/CAT-Surface | 1 | [ChristianGaser/CAT-Surface](https://github.com/ChristianGaser/CAT-Surface) |
| cfengine/core | 1 | [cfengine/core](https://github.com/cfengine/core) |
| centreon/centreon-collect | 1 | [centreon/centreon-collect](https://github.com/centreon/centreon-collect) |
| cea-hpc/mpc | 1 | [cea-hpc/mpc](https://github.com/cea-hpc/mpc) |
| casadi/casadi | 1 | [casadi/casadi](https://github.com/casadi/casadi) |
| Cantera/cantera | 1 | [Cantera/cantera](https://github.com/Cantera/cantera) |
| bytecodealliance/wasm-micro-runtime | 1 | [bytecodealliance/wasm-micro-runtime](https://github.com/bytecodealliance/wasm-micro-runtime) |
| brazilofmux/tinymux | 1 | [brazilofmux/tinymux](https://github.com/brazilofmux/tinymux) |
| bpftrace/bpftrace | 1 | [bpftrace/bpftrace](https://github.com/bpftrace/bpftrace) |
| blender/cycles | 1 | [blender/cycles](https://github.com/blender/cycles) |
| BlazingRenderer/BRender | 1 | [BlazingRenderer/BRender](https://github.com/BlazingRenderer/BRender) |
| bertiniteam/b2 | 1 | [bertiniteam/b2](https://github.com/bertiniteam/b2) |
| BelledonneCommunications/flexisip | 1 | [BelledonneCommunications/flexisip](https://github.com/BelledonneCommunications/flexisip) |
| beerda/nuggets | 1 | [beerda/nuggets](https://github.com/beerda/nuggets) |
| bdring/FluidNC | 1 | [bdring/FluidNC](https://github.com/bdring/FluidNC) |
| bacnet-stack/bacnet-stack | 1 | [bacnet-stack/bacnet-stack](https://github.com/bacnet-stack/bacnet-stack) |
| aws/aws-ofi-nccl | 1 | [aws/aws-ofi-nccl](https://github.com/aws/aws-ofi-nccl) |
| aseprite/freetype2 | 1 | [aseprite/freetype2](https://github.com/aseprite/freetype2) |
| amule-project/amule | 1 | [amule-project/amule](https://github.com/amule-project/amule) |
| aardappel/lobster | 1 | [aardappel/lobster](https://github.com/aardappel/lobster) |
| 203-Systems/MatrixOS | 1 | [203-Systems/MatrixOS](https://github.com/203-Systems/MatrixOS) |
