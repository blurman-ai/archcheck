# AI-reviewed drift PRs: concrete dependency deltas

_Generated 2026-07-07 from `experiments/trending_run/inverted_corpus.jsonl` by rerunning `archcheck --diff <sha>~1..<sha>` on local `/home/localadm/oss_corpus/oss/*` clones._

Scope: 43 drift commits grouped into 38 PRs. `structural review?` is the manual read from `ai_hit_eyecheck2.jsonl`: whether the AI reviewer actually mentioned the structural issue, not just any keyword.

## 1. AndreasNilsson123/Astraeus PR #46

- title: Integrate PostChain into frame pipeline with config and output contract
- structural review?: **False**

### commit 1: `ac1a8af2148a` — Wire PostChain into RenderGraph execution and add config support
- archcheck counters: added_edges=4, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	engine/core/EngineContext.hpp`
  - `M	engine/renderer/RenderGraph.hpp`
  - `M	engine/renderer/passes/post/PostChain.hpp`
- new include edges:
  - `+ engine/renderer/RenderGraph.hpp  ->  engine/renderer/opengl/GLRenderDevice.hpp`
  - `+ engine/renderer/RenderGraph.hpp  ->  engine/renderer/passes/post/GammaCorrectionPass.hpp`
  - `+ engine/renderer/RenderGraph.hpp  ->  engine/renderer/passes/post/PostChain.hpp`
  - `+ engine/renderer/RenderGraph.hpp  ->  engine/renderer/passes/post/ToneMappingPass.hpp`

## 2. BPForbes/Bailey-Forbes-Flinstone PR #100

- title: Add P3 networking contract bundle (`contracts/networking`)
- structural review?: **False**

### commit 1: `3aa0006f5d1b` — Implement P3 wire types and decouple P3 from full P2 identity bundle
- archcheck counters: added_edges=21, removed_edges=13, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	contracts/networking/README.txt`
  - `M	contracts/networking/contract_networking.h`
  - `M	contracts/networking/contract_p3_arp.h`
  - `M	contracts/networking/contract_p3_dhcp.h`
  - `M	contracts/networking/contract_p3_dns.h`
  - `M	contracts/networking/contract_p3_ipv4.h`
  - `M	contracts/networking/contract_p3_ipv6_deferred.h`
  - `M	contracts/networking/contract_p3_loopback.h`
  - `M	contracts/networking/contract_p3_netdev.h`
  - `M	contracts/networking/contract_p3_tap.h`
  - `M	contracts/networking/contract_p3_tcp.h`
  - `M	contracts/networking/contract_p3_tls_hosted.h`
- new include edges:
  - `+ contracts/networking/contract_networking.h  ->  contracts/foundations/contract_extend.h`
  - `+ contracts/networking/contract_networking.h  ->  contracts/networking/contract_p3_trust.h`
  - `+ contracts/networking/contract_networking.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_arp.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_dhcp.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_dns.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_ipv4.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_ipv6_deferred.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_loopback.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_netdev.h  ->  contracts/networking/contract_p3_trust.h`
  - `+ contracts/networking/contract_p3_netdev.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_tap.h  ->  contracts/networking/contract_p3_trust.h`
  - `+ contracts/networking/contract_p3_tap.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_tcp.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_tls_hosted.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_trust.h  ->  contracts/identity/contract_p2_authz.h`
  - `+ contracts/networking/contract_p3_udp.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_wifi_deferred.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ contracts/networking/contract_p3_wire.h  ->  contracts/foundations/contract_extend.h`
  - `+ kernel/include/fl/driver/net.h  ->  contracts/networking/contract_p3_wire.h`
  - `+ userland/command/cmd_contracts.c  ->  contracts/networking/contract_networking.h`

## 3. BPForbes/Bailey-Forbes-Flinstone PR #168

- title: Merge develop: 4.1.0 GA with promoted version (DEV 4)
- structural review?: **True**
- reviewer structural lines:
  - and add any necessary `#include` or forward declare so that the canonical identity

### commit 1: `cabb788064e9` — Merge develop: 4.1.0 GA with promoted version (DEV 4) (#168)
- archcheck counters: added_edges=89, removed_edges=1, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	.coderabbit.yaml`
  - `M	.github/workflows/c-cpp.yml`
  - `M	.gitignore`
  - `A	.replit`
  - `M	AGENTS.md`
  - `D	ARM/alloc/alloc.h`
  - `D	ARM/alloc/alloc_core.s`
  - `D	ARM/alloc/alloc_free.s`
  - `D	ARM/alloc/alloc_malloc.s`
  - `D	ARM/drivers/port_io.h`
  - `D	ARM/drivers/port_io.s`
  - `D	ARM/mem_asm.h`
- new include edges:
  - `+ contracts/foundations/contract_foundations.h  ->  contracts/foundations/contract_p0_fs_jail.h`
  - `+ contracts/foundations/contract_p0_fs_jail.h  ->  contracts/foundations/contract_extend.h`
  - `+ kernel/core/identity/elevation.c  ->  kernel/core/identity/elevation.h`
  - `+ kernel/core/identity/elevation.c  ->  kernel/core/identity/user_db.h`
  - `+ kernel/core/identity/elevation.c  ->  kernel/core/mm/mem_domain.h`
  - `+ kernel/core/identity/elevation.c  ->  kernel/core/time/timekeeping.h`
  - `+ kernel/core/identity/elevation.c  ->  kernel/include/fl/fl_stack_asm.h`
  - `+ kernel/core/identity/elevation.h  ->  contracts/foundations/contract_result.h`
  - `+ kernel/core/identity/elevation.h  ->  contracts/identity/contract_p2_elevation.h`
  - `+ kernel/core/identity/path_property.c  ->  kernel/core/identity/path_property.h`
  - `+ kernel/core/identity/path_property.c  ->  kernel/core/mm/mem_domain.h`
  - `+ kernel/core/identity/path_property.h  ->  contracts/identity/contract_p2_authz.h`
  - `+ kernel/core/identity/path_property.h  ->  contracts/identity/contract_p2_principal.h`
  - `+ kernel/core/identity/session.c  ->  kernel/core/identity/elevation.h`
  - `+ kernel/core/identity/session.c  ->  kernel/core/identity/session.h`
  - `+ kernel/core/identity/session.h  ->  contracts/identity/contract_identity.h`
  - `+ kernel/core/identity/session.h  ->  kernel/core/identity/user_db.h`
  - `+ kernel/core/identity/user_db.c  ->  kernel/core/identity/user_db.h`
  - `+ kernel/core/identity/user_db.c  ->  kernel/core/mm/mem_domain.h`
  - `+ kernel/core/identity/user_db.c  ->  userland/identity/password_hash.h`
  - `+ kernel/core/identity/user_db.h  ->  contracts/foundations/contract_result.h`
  - `+ kernel/core/identity/user_db.h  ->  contracts/identity/contract_p2_credential_store.h`
  - `+ kernel/core/identity/user_db.h  ->  userland/identity/password_hash.h`
  - `+ kernel/core/memory/exec_context.c  ->  kernel/core/memory/exec_context.h`
  - `+ kernel/core/memory/exec_context.c  ->  kernel/core/mm/mem_domain.h`
  - `+ kernel/core/memory/exec_context.h  ->  contracts/runtime/contract_p1_execution.h`
  - `+ kernel/core/memory/fl_stack.c  ->  kernel/core/memory/fl_stack.h`
  - `+ kernel/core/memory/fl_stack.c  ->  kernel/include/fl/fl_stack_asm.h`
  - `+ kernel/core/memory/fl_stack.h  ->  contracts/runtime/contract_p1_pmm.h`
  - `+ kernel/core/mm/pmm.c  ->  kernel/core/memory/fl_stack.h`
  - ... 59 more

## 4. FeJS8888/FeEGELib PR #14

- title: 现在 InputBox 可以正确处理左右方向键的移动和鼠标点击了。
- structural review?: **True**
- reviewer structural lines:
  - Prefer forward declaration and remove the include to prevent cycles with Layout.
  - +// forward declaration below is sufficient; include Layout.
  - **Break the circular include; rely on forward declarations**

### commit 1: `af62a6276ea2` — 现在 InputBox 可以正确处理左右方向键的移动和鼠标点击了。 现在 Button 可以正确处理长按，波纹持续显示。 调整了 Button 的消息处理时机，现在松开鼠标时才触发点击事件。 新增了 Slider 的步进功能，可以设置每次移动的增量。
- archcheck counters: added_edges=3, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	README.md`
  - `M	include/Base.h`
  - `M	include/FeEGELib.h`
  - `A	include/Layout.h`
  - `M	include/Widget.h`
  - `M	lib/libgraphics.a`
  - `A	src/Layout.cpp`
  - `M	src/Widget.cpp`
- new include edges:
  - `+ include/Layout.h  ->  include/Widget.h`
  - `+ include/Widget.h  ->  include/Layout.h`
  - `+ src/Layout.cpp  ->  include/Layout.h`

## 5. GBALATRO/balatro-gba PR #449

- title: [Feature] Implement options menu and reading/writing values from/to the SRAM
- structural review?: **False**

### commit 1: `e7f550ce22e2` — [Feature] Implement options menu and reading/writing values from/to the SRAM (#449)
- archcheck counters: added_edges=29, removed_edges=1, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	Doxyfile`
  - `M	Makefile`
  - `M	graphics/background_main_menu_gfx.png`
  - `A	graphics/background_options_menu_gfx.grit`
  - `A	graphics/background_options_menu_gfx.png`
  - `M	graphics/deck_gfx.grit`
  - `M	graphics/deck_gfx.png`
  - `A	graphics/high_contrast_deck_pal_gfx.grit`
  - `A	graphics/high_contrast_deck_pal_gfx.png`
  - `M	include/affine_background.h`
  - `M	include/audio_utils.h`
  - `M	include/button.h`
- new include edges:
  - `+ include/game.h  ->  include/blind.h`
  - `+ include/game/options_menu.h  ->  include/game_variables.h`
  - `+ include/save.h  ->  include/game_variables.h`
  - `+ source/audio_utils.c  ->  include/game_variables.h`
  - `+ source/game.c  ->  include/game/options_menu.h`
  - `+ source/game.c  ->  include/save.h`
  - `+ source/game/common_ui.c  ->  include/game/options_menu.h`
  - `+ source/game/main_menu.c  ->  include/selection_grid.h`
  - `+ source/game/main_menu.c  ->  include/version.h`
  - `+ source/game/options_menu.c  ->  include/affine_background.h`
  - `+ source/game/options_menu.c  ->  include/audio_utils.h`
  - `+ source/game/options_menu.c  ->  include/button.h`
  - `+ source/game/options_menu.c  ->  include/game.h`
  - `+ source/game/options_menu.c  ->  include/game/common_ui.h`
  - `+ source/game/options_menu.c  ->  include/game/options_menu.h`
  - `+ source/game/options_menu.c  ->  include/game_variables.h`
  - `+ source/game/options_menu.c  ->  include/graphic_utils.h`
  - `+ source/game/options_menu.c  ->  include/save.h`
  - `+ source/game/options_menu.c  ->  include/selection_grid.h`
  - `+ source/game/options_menu.c  ->  include/util.h`
  - `+ source/joker.c  ->  include/game_variables.h`
  - `+ source/save.c  ->  include/audio_utils.h`
  - `+ source/save.c  ->  include/game.h`
  - `+ source/save.c  ->  include/joker.h`
  - `+ source/save.c  ->  include/list.h`
  - `+ source/save.c  ->  include/save.h`
  - `+ source/save.c  ->  include/util.h`
  - `+ source/save.c  ->  include/version.h`
  - `+ source/sprite.c  ->  include/game_variables.h`

## 6. GregorGullwi/FlashCpp PR #815

- title: Refactor CodeGen to classical header/implementation split; fix mixed indentation
- structural review?: **False**

### commit 1: `52125d78b2ad` — Refactor CodeGen to classical header/implementation split; fix mixed indentation (#815)
- archcheck counters: added_edges=21, removed_edges=5, grown_cycles=1, new_area_deps=0
- changed files:
  - `A	src/AstToIr.h`
  - `M	src/CodeGen.h`
  - `M	src/CodeGen_Call_Direct.cpp`
  - `M	src/CodeGen_Call_Indirect.cpp`
  - `M	src/CodeGen_Expr_Conversions.cpp`
  - `M	src/CodeGen_Expr_Operators.cpp`
  - `M	src/CodeGen_Expr_Primitives.cpp`
  - `M	src/CodeGen_Lambdas.cpp`
  - `M	src/CodeGen_MemberAccess.cpp`
  - `M	src/CodeGen_NewDeleteCast.cpp`
  - `M	src/CodeGen_Stmt_Control.cpp`
  - `M	src/CodeGen_Stmt_Decl.cpp`
- new include edges:
  - `+ src/AstToIr.h  ->  src/CodeGen.h`
  - `+ src/CodeGen.h  ->  src/AstToIr.h`
  - `+ src/CodeGen_Call_Direct.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Call_Indirect.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Expr_Conversions.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Expr_Operators.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Expr_Primitives.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Lambdas.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_MemberAccess.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_NewDeleteCast.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Stmt_Control.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Stmt_Decl.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Stmt_TryCatchSeh.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Visitors_Decl.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Visitors_Namespace.cpp  ->  src/CodeGen.h`
  - `+ src/CodeGen_Visitors_TypeInit.cpp  ->  src/CodeGen.h`
  - `+ src/FlashCppUnity.h  ->  src/CodeGen_Expressions.cpp`
  - `+ src/FlashCppUnity.h  ->  src/CodeGen_Functions.cpp`
  - `+ src/FlashCppUnity.h  ->  src/CodeGen_Lambdas.cpp`
  - `+ src/FlashCppUnity.h  ->  src/CodeGen_Statements.cpp`
  - `+ src/FlashCppUnity.h  ->  src/CodeGen_Visitors.cpp`

## 7. GregorGullwi/FlashCpp PR #1480

- title: Refactor template binding storage: InlineVector args + unified InstantiationContext bindin
- structural review?: **False**

### commit 1: `9380f39fb9fc` — Refactor template binding storage: InlineVector args + unified InstantiationContext bindings (#1480)
- archcheck counters: added_edges=1, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	src/AstNodeTypes.cpp`
  - `M	src/AstNodeTypes_DeclNodes.h`
  - `M	src/AstNodeTypes_Template.h`
  - `M	src/TemplateEnvironment.cpp`
  - `M	src/TemplateEnvironment.h`
- new include edges:
  - `+ src/TemplateEnvironment.h  ->  src/TemplateRegistry_Types.h`

## 8. GregorGullwi/FlashCpp PR #1530

- title: Fix dependent NTTP expression identity collapse for unevaluated dependent expressions
- structural review?: **False**

### commit 1: `9da4c8b47211` — Fix dependent NTTP expression identity collapse for unevaluated dependent expressions (#1530)
- archcheck counters: added_edges=8, removed_edges=1, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	FlashCppMSVC.vcxproj`
  - `M	FlashCppMSVC.vcxproj.filters`
  - `M	docs/2026-05-12-template-argument-architecture-audit.md`
  - `M	docs/2026-05-12-template-argument-standard-conformance-investigation.md`
  - `M	docs/KNOWN_ISSUES.md`
  - `M	src/AstNodeTypes.cpp`
  - `M	src/CompilerIncludes.h`
  - `A	src/TemplateExpressionEquivalence.cpp`
  - `A	src/TemplateExpressionEquivalence.h`
  - `M	src/TemplateRegistry_Types.h`
  - `M	src/TemplateTypes.h`
  - `A	tests/test_dependent_nttp_expression_identity_noexcept_delete_ret0.cpp`
- new include edges:
  - `+ src/CompilerIncludes.h  ->  src/TemplateExpressionEquivalence.cpp`
  - `+ src/TemplateExpressionEquivalence.cpp  ->  src/CompileError.h`
  - `+ src/TemplateExpressionEquivalence.cpp  ->  src/TemplateExpressionEquivalence.h`
  - `+ src/TemplateExpressionEquivalence.cpp  ->  src/TemplateRegistry_Types.h`
  - `+ src/TemplateExpressionEquivalence.cpp  ->  src/TemplateTypes.h`
  - `+ src/TemplateExpressionEquivalence.h  ->  src/AstNodeTypes.h`
  - `+ src/TemplateTypes.h  ->  src/StringBuilder.h`
  - `+ src/TemplateTypes.h  ->  src/TemplateExpressionEquivalence.h`

## 9. GregorGullwi/FlashCpp PR #1654

- title: Improve C++20 prefixed string-literal typing to unblock std header semantic progress
- structural review?: **False**

### commit 1: `0e8b826b79c9` — Improve C++20 prefixed string-literal typing to unblock std header semantic progress (#1654)
- archcheck counters: added_edges=4, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	src/Lexer.h`
  - `M	src/Parser_Expr_PrimaryExpr.cpp`
  - `M	src/Parser_Expr_QualLookup.cpp`
  - `M	src/SemanticAnalysis.cpp`
  - `M	src/StringLiteralTokenUtils.h`
  - `M	tests/std/README_STANDARD_HEADERS.md`
  - `A	tests/test_prefixed_literal_lexer_ret0.cpp`
  - `A	tests/test_wide_string_literal_overload_ret0.cpp`
- new include edges:
  - `+ src/Lexer.h  ->  src/StringLiteralTokenUtils.h`
  - `+ src/Parser_Expr_PrimaryExpr.cpp  ->  src/StringLiteralTokenUtils.h`
  - `+ src/Parser_Expr_QualLookup.cpp  ->  src/StringLiteralTokenUtils.h`
  - `+ src/StringLiteralTokenUtils.h  ->  src/AstNodeTypes_TypeSystem.h`

## 10. GregorGullwi/FlashCpp PR #1748

- title: Fix Modular build configuration
- structural review?: **False**

### commit 1: `12ea8147ab33` — Fix Modular build configuration (#1748)
- archcheck counters: added_edges=14, removed_edges=2, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	FlashCpp.vcxproj`
  - `M	FlashCppMSVC.vcxproj`
  - `M	docs/KNOWN_ISSUES.md`
  - `M	src/AstNodeTypes_Expr.h`
  - `M	src/AstNodeTypes_Template.h`
  - `M	src/AstNodeTypes_TypeSystem.h`
  - `M	src/CodeViewDebug.h`
  - `M	src/CompileContext.h`
  - `M	src/CompilerIncludes.h`
  - `M	src/ConstExprEvaluator.h`
  - `M	src/ConstExprEvaluator_Core.cpp`
  - `M	src/ConstExprEvaluator_Members.cpp`
- new include edges:
  - `+ src/CompilerIncludes.h  ->  src/StringLiteralTokenUtils.cpp`
  - `+ src/ExpressionSubstitutor.h  ->  src/Parser.h`
  - `+ src/IRConverter_ConvertMain.cpp  ->  src/IrGenerator.h`
  - `+ src/IROperandHelpers.h  ->  src/IRTypes.h`
  - `+ src/IrGenerator_Call_Indirect.cpp  ->  src/SemanticAnalysis.h`
  - `+ src/IrGenerator_Expr_Operators.cpp  ->  src/TypeSizeQuery.h`
  - `+ src/IrGenerator_Expr_Primitives.cpp  ->  src/CallNodeHelpers.h`
  - `+ src/IrGenerator_NewDeleteCast.cpp  ->  src/SemanticAnalysis.h`
  - `+ src/IrGenerator_Visitors_TypeInit.cpp  ->  src/SemanticAnalysis.h`
  - `+ src/ParserInternal.h  ->  src/ParserScopeGuards.h`
  - `+ src/Parser_Templates_Inst_MemberFunc.cpp  ->  src/ExpressionSubstitutor.h`
  - `+ src/StringLiteralTokenUtils.cpp  ->  src/AstNodeTypes_TypeSystem.h`
  - `+ src/StringLiteralTokenUtils.cpp  ->  src/StringLiteralTokenUtils.h`
  - `+ src/TemplateInstantiationHelper.h  ->  src/Parser.h`

## 11. K1tox-Inc/kernel-legacy PR #66

- title: 41 proc foundations   define process memory layout vmm interface
- structural review?: **False**

### commit 1: `ddf95def5214` — dev(userspace): need kmap
- archcheck counters: added_edges=4, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `R100	include/signal.h	include/proc/signal.h`
  - `M	include/proc/task.h`
  - `M	include/proc/userspace.h`
  - `M	src/proc/userspace.c`
- new include edges:
  - `+ include/proc/signal.h  ->  include/types.h`
  - `+ include/proc/task.h  ->  include/proc/userspace.h`
  - `+ include/proc/userspace.h  ->  include/proc/task.h`
  - `+ src/proc/userspace.c  ->  lib/libk/libk.h`

### commit 2: `3cf42e87f02b` — refactor(userspace): remove obsolete task forward declaration now that section has its own header
- archcheck counters: added_edges=1, removed_edges=1, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	include/proc/task.h`
  - `M	include/proc/userspace.h`
  - `M	src/proc/userspace.c`
- new include edges:
  - `+ include/proc/userspace.h  ->  include/proc/task.h`

## 12. MaaXYZ/MaaFramework PR #794

- title: Feat/quickjs nodejs binding
- structural review?: **False**

### commit 1: `be88651f15b7` — Feat/quickjs nodejs binding (#794)
- archcheck counters: added_edges=150, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	.github/workflows/build.yml`
  - `M	.github/workflows/test.yml`
  - `M	.gitmodules`
  - `M	.vscode/launch.json`
  - `M	.vscode/settings.json`
  - `M	3rdparty/CMakeLists.txt`
  - `A	3rdparty/quickjs`
  - `M	CMakeLists.txt`
  - `M	include/MaaFramework/Instance/MaaCustomController.h`
  - `M	sample/nodejs/main.ts`
  - `M	sample/nodejs/server.ts`
  - `M	source/binding/CMakeLists.txt`
- new include edges:
  - `+ source/binding/NodeJS/src/apis/buffer.h  ->  include/MaaFramework/MaaAPI.h`
  - `+ source/binding/NodeJS/src/apis/buffer.h  ->  source/binding/NodeJS/src/foundation/spec.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/buffer.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/callback.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/context.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/controller.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/convert.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/resource.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/apis/tasker.h`
  - `+ source/binding/NodeJS/src/apis/callback.cpp  ->  source/binding/NodeJS/src/foundation/spec.h`
  - `+ source/binding/NodeJS/src/apis/callback.h  ->  include/MaaFramework/MaaAPI.h`
  - `+ source/binding/NodeJS/src/apis/client.cpp  ->  source/binding/NodeJS/src/apis/buffer.h`
  - `+ source/binding/NodeJS/src/apis/client.cpp  ->  source/binding/NodeJS/src/apis/client.h`
  - `+ source/binding/NodeJS/src/apis/client.cpp  ->  source/binding/NodeJS/src/apis/ext.h`
  - `+ source/binding/NodeJS/src/apis/client.cpp  ->  source/binding/NodeJS/src/apis/loader.h`
  - `+ source/binding/NodeJS/src/apis/client.cpp  ->  source/binding/NodeJS/src/foundation/spec.h`
  - `+ source/binding/NodeJS/src/apis/client.h  ->  include/MaaAgentClient/MaaAgentClientAPI.h`
  - `+ source/binding/NodeJS/src/apis/client.h  ->  include/MaaFramework/MaaAPI.h`
  - `+ source/binding/NodeJS/src/apis/client.h  ->  source/binding/NodeJS/src/apis/controller.h`
  - `+ source/binding/NodeJS/src/apis/client.h  ->  source/binding/NodeJS/src/apis/resource.h`
  - `+ source/binding/NodeJS/src/apis/client.h  ->  source/binding/NodeJS/src/apis/tasker.h`
  - `+ source/binding/NodeJS/src/apis/client.h  ->  source/binding/NodeJS/src/foundation/spec.h`
  - `+ source/binding/NodeJS/src/apis/constant.cpp  ->  include/MaaFramework/MaaAPI.h`
  - `+ source/binding/NodeJS/src/apis/constant.cpp  ->  source/binding/NodeJS/src/apis/loader.h`
  - `+ source/binding/NodeJS/src/apis/constant.cpp  ->  source/binding/NodeJS/src/foundation/spec.h`
  - `+ source/binding/NodeJS/src/apis/context.cpp  ->  include/MaaFramework/MaaAPI.h`
  - `+ source/binding/NodeJS/src/apis/context.cpp  ->  source/binding/NodeJS/src/apis/buffer.h`
  - `+ source/binding/NodeJS/src/apis/context.cpp  ->  source/binding/NodeJS/src/apis/context.h`
  - `+ source/binding/NodeJS/src/apis/context.cpp  ->  source/binding/NodeJS/src/apis/convert.h`
  - `+ source/binding/NodeJS/src/apis/context.cpp  ->  source/binding/NodeJS/src/apis/ext.h`
  - ... 120 more

## 13. Matthew-McRaven/Pepp PR #1003

- title: Integrate `libriscv` for RISC-V instruction set simulation
- structural review?: **False**

### commit 1: `de71f42c1230` — fix: attempt to avoid undefined references to Multithreading CTOR
- archcheck counters: added_edges=1, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	lib/sim3/systems/notraced_riscv_isa3_system.cpp`
  - `M	lib/sim3/systems/notraced_riscv_isa3_system/threads.hpp`
  - `M	lib/sim3/systems/notraced_riscv_isa3_system/threads_impl.hpp`
- new include edges:
  - `+ lib/sim3/systems/notraced_riscv_isa3_system/threads.hpp  ->  lib/sim3/systems/notraced_riscv_isa3_system/threads_impl.hpp`

## 14. PrincetonUniversity/SPECFEMPP PR #877

- title: Adds chunked domain iterator
- structural review?: **False**

### commit 1: `9e21f4512c26` — Adds chunked domain iterator
- archcheck counters: added_edges=15, removed_edges=2, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	include/algorithms/divergence.hpp`
  - `M	include/algorithms/gradient.hpp`
  - `A	include/chunk_element/index.hpp`
  - `M	include/compute/fields/data_access.tpp`
  - `M	include/compute/fields/simulation_field.hpp`
  - `M	include/domain_view.hpp`
  - `A	include/execution/chunked_domain_iterator.hpp`
  - `M	include/kokkos_kernels/impl/compute_mass_matrix.tpp`
  - `M	include/kokkos_kernels/impl/compute_stiffness_interaction.tpp`
  - `M	include/medium/impl/accessor.hpp`
- new include edges:
  - `+ include/algorithms/divergence.hpp  ->  include/execution/for_each_level.hpp`
  - `+ include/algorithms/gradient.hpp  ->  include/execution/for_each_level.hpp`
  - `+ include/chunk_element/index.hpp  ->  include/enumerations/interface.hpp`
  - `+ include/chunk_element/index.hpp  ->  include/execution/chunked_domain_iterator.hpp`
  - `+ include/compute/fields/data_access.tpp  ->  include/execution/for_each_level.hpp`
  - `+ include/execution/chunked_domain_iterator.hpp  ->  core/specfem/point.hpp`
  - `+ include/execution/chunked_domain_iterator.hpp  ->  include/chunk_element/index.hpp`
  - `+ include/execution/chunked_domain_iterator.hpp  ->  include/execution/policy.hpp`
  - `+ include/execution/chunked_domain_iterator.hpp  ->  include/execution/void_iterator.hpp`
  - `+ include/execution/chunked_domain_iterator.hpp  ->  include/macros.hpp`
  - `+ include/kokkos_kernels/impl/compute_mass_matrix.tpp  ->  include/execution/chunked_domain_iterator.hpp`
  - `+ include/kokkos_kernels/impl/compute_mass_matrix.tpp  ->  include/execution/for_all.hpp`
  - `+ include/kokkos_kernels/impl/compute_stiffness_interaction.tpp  ->  include/execution/chunked_domain_iterator.hpp`
  - `+ include/kokkos_kernels/impl/compute_stiffness_interaction.tpp  ->  include/execution/for_all.hpp`
  - `+ include/kokkos_kernels/impl/compute_stiffness_interaction.tpp  ->  include/execution/for_each_level.hpp`

## 15. PrincetonUniversity/SPECFEMPP PR #887

- title: Issue 702 material derivatives
- structural review?: **False**

### commit 1: `abea95fbb266` — Added accessors to index implementations
- archcheck counters: added_edges=6, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/point/index.hpp`
  - `M	core/specfem/point/mapped_index.hpp`
  - `M	include/chunk_element/index.hpp`
  - `A	include/chunk_element/mapped_index.hpp`
  - `M	include/compute/sources/sources.hpp`
  - `M	include/enumerations/accessor.hpp`
  - `M	include/enumerations/data_class.hpp`
  - `M	include/execution/mapped_chunked_domain_iterator.hpp`
  - `M	tests/unit-tests/point/index_tests.cpp`
- new include edges:
  - `+ include/chunk_element/mapped_index.hpp  ->  include/enumerations/interface.hpp`
  - `+ include/chunk_element/mapped_index.hpp  ->  include/execution/mapped_chunked_domain_iterator.hpp`
  - `+ include/execution/mapped_chunked_domain_iterator.hpp  ->  core/specfem/point.hpp`
  - `+ include/execution/mapped_chunked_domain_iterator.hpp  ->  include/chunk_element/mapped_index.hpp`
  - `+ include/execution/mapped_chunked_domain_iterator.hpp  ->  include/execution/policy.hpp`
  - `+ include/execution/mapped_chunked_domain_iterator.hpp  ->  include/execution/void_iterator.hpp`

## 16. PrincetonUniversity/SPECFEMPP PR #943

- title: Single include header for assembly module
- structural review?: **True**
- reviewer structural lines:
  - hpp`, breaks circular dependencies via forward declarations, and relocates the `properties_container` implementation into a `.
  - - Introduced forward declarations in public headers (`source.
  - hpp`) to cut circular includes.

### commit 1: `64c6dbdf25a6` — Single include header for assembly module
- archcheck counters: added_edges=15, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/assembly.hpp`
  - `M	core/specfem/assembly/assembly.cpp`
  - `M	core/specfem/assembly/boundaries.cpp`
  - `M	core/specfem/assembly/boundaries.hpp`
  - `M	core/specfem/assembly/boundaries/acoustic_free_surface.cpp`
  - `M	core/specfem/assembly/boundaries/acoustic_free_surface.hpp`
  - `M	core/specfem/assembly/properties.cpp`
  - `D	core/specfem/assembly_mesh.hpp`
  - `M	include/medium/properties_container.hpp`
  - `A	include/medium/properties_container.tpp`
  - `M	include/receiver/receiver.hpp`
  - `M	include/source/adjoint_source.hpp`
- new include edges:
  - `+ core/specfem/assembly.hpp  ->  core/specfem/assembly/element_types.hpp`
  - `+ core/specfem/assembly/properties.cpp  ->  include/medium/properties_container.hpp`
  - `+ core/specfem/assembly/properties.cpp  ->  include/medium/properties_container.tpp`
  - `+ include/medium/properties_container.tpp  ->  include/medium/properties_container.hpp`
  - `+ include/source/cosserat_force_source.hpp  ->  core/specfem/assembly.hpp`
  - `+ include/source/external.hpp  ->  core/specfem/assembly.hpp`
  - `+ include/source/force_source.hpp  ->  core/specfem/assembly.hpp`
  - `+ include/source/moment_tensor_source.hpp  ->  core/specfem/assembly.hpp`
  - `+ src/receiver/receiver.cpp  ->  core/specfem/assembly.hpp`
  - `+ src/receiver/receiver.cpp  ->  include/receiver/receiver.hpp`
  - `+ src/source/adjoint_source.cpp  ->  core/specfem/assembly.hpp`
  - `+ src/source/cosserat_force_source.cpp  ->  core/specfem/assembly.hpp`
  - `+ src/source/external.cpp  ->  core/specfem/assembly.hpp`
  - `+ src/source/force_source.cpp  ->  core/specfem/assembly.hpp`
  - `+ src/source/moment_tensor_source.cpp  ->  core/specfem/assembly.hpp`

## 17. PrincetonUniversity/SPECFEMPP PR #992

- title: Implements 2D assembly
- structural review?: **False**

### commit 1: `1dac68cf9d71` — Template specialization for 2D assembly
- archcheck counters: added_edges=23, removed_edges=14, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/assembly/assembly.hpp`
  - `R096	core/specfem/assembly/assembly.cpp	core/specfem/assembly/assembly/assembly.cpp`
  - `A	core/specfem/assembly/assembly/assembly.hpp`
  - `R081	core/specfem/assembly/compute_wavefield.cpp	core/specfem/assembly/assembly/compute_wavefield/compute_wavefield.cpp`
  - `R100	core/specfem/assembly/compute_wavefield/helper.hpp	core/specfem/assembly/assembly/compute_wavefield/helper.hpp`
- new include edges:
  - `+ core/specfem/assembly/assembly.hpp  ->  core/specfem/assembly/assembly/assembly.hpp`
  - `+ core/specfem/assembly/assembly/assembly.cpp  ->  core/specfem/assembly/assembly/assembly.hpp`
  - `+ core/specfem/assembly/assembly/assembly.cpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/assembly/assembly/assembly.cpp  ->  include/io/reader.hpp`
  - `+ core/specfem/assembly/assembly/assembly.cpp  ->  include/mesh/mesh.hpp`
  - `+ core/specfem/assembly/assembly/assembly.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/assembly/assembly/assembly.hpp  ->  include/io/reader.hpp`
  - `+ core/specfem/assembly/assembly/assembly.hpp  ->  include/mesh/mesh.hpp`
  - `+ core/specfem/assembly/assembly/assembly.hpp  ->  include/receiver/interface.hpp`
  - `+ core/specfem/assembly/assembly/assembly.hpp  ->  include/source/interface.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/compute_wavefield.cpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/compute_wavefield.cpp  ->  core/specfem/assembly/assembly/compute_wavefield/helper.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  core/specfem/point.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/algorithms/gradient.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/chunk_element/field.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/enumerations/dimension.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/enumerations/medium.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/enumerations/wavefield.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/execution/chunked_domain_iterator.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/execution/for_each_level.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/medium/medium.hpp`
  - `+ core/specfem/assembly/assembly/compute_wavefield/helper.hpp  ->  include/parallel_configuration/chunk_config.hpp`

## 18. PrincetonUniversity/SPECFEMPP PR #1039

- title: Issue 990 Part 6 - templated 2d source
- structural review?: **False**

### commit 1: `0bdc84115b94` — Removed the fwd and common
- archcheck counters: added_edges=16, removed_edges=12, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/source.hpp`
  - `D	core/specfem/source/common/types.hpp`
  - `M	core/specfem/source/dim2/source.hpp`
  - `M	core/specfem/source/dim2/tensor_source.hpp`
  - `M	core/specfem/source/dim2/tensor_source/moment_tensor_source.cpp`
  - `M	core/specfem/source/dim2/tensor_source/moment_tensor_source.hpp`
  - `M	core/specfem/source/dim2/vector_source.hpp`
  - `M	core/specfem/source/dim2/vector_source/adjoint_source.cpp`
  - `M	core/specfem/source/dim2/vector_source/adjoint_source.hpp`
  - `M	core/specfem/source/dim2/vector_source/cosserat_force_source.cpp`
  - `M	core/specfem/source/dim2/vector_source/cosserat_force_source.hpp`
  - `M	core/specfem/source/dim2/vector_source/external.cpp`
- new include edges:
  - `+ core/specfem/source.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim2/source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/tensor_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/tensor_source.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim2/tensor_source.hpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim2/tensor_source/moment_tensor_source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/tensor_source/moment_tensor_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/adjoint_source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/adjoint_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/cosserat_force_source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/cosserat_force_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/external.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/external.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/force_source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/vector_source/force_source.hpp  ->  core/specfem/source.hpp`

## 19. PrincetonUniversity/SPECFEMPP PR #1041

- title: Issue 990 Part 7- Implemented 3D force, moment tensor, readers and associated tests
- structural review?: **False**

### commit 1: `b4bc4356ab2d` — Added 3d force and moment tensor source
- archcheck counters: added_edges=36, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/source.hpp`
  - `A	core/specfem/source/dim3/source.cpp`
  - `A	core/specfem/source/dim3/source.hpp`
  - `A	core/specfem/source/dim3/tensor_source.hpp`
  - `A	core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp`
  - `A	core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp`
  - `A	core/specfem/source/dim3/vector_source.hpp`
  - `A	core/specfem/source/dim3/vector_source/force_source.cpp`
  - `A	core/specfem/source/dim3/vector_source/force_source.hpp`
- new include edges:
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim3/source.hpp`
  - `+ core/specfem/source/dim3/source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/source.cpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim3/source.cpp  ->  include/specfem_mpi/interface.hpp`
  - `+ core/specfem/source/dim3/source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/source.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim3/source.hpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim3/source.hpp  ->  include/source_time_function/interface.hpp`
  - `+ core/specfem/source/dim3/source.hpp  ->  include/utilities/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp  ->  include/globals.h`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp  ->  include/source_time_function/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  include/quadrature/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  include/source_time_function/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  include/specfem_mpi/interface.hpp`
  - `+ core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp  ->  include/utilities/interface.hpp`
  - `+ core/specfem/source/dim3/vector_source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/vector_source.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim3/vector_source/force_source.cpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/vector_source/force_source.cpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/dim3/vector_source/force_source.cpp  ->  include/globals.h`
  - `+ core/specfem/source/dim3/vector_source/force_source.cpp  ->  include/source_time_function/interface.hpp`
  - `+ core/specfem/source/dim3/vector_source/force_source.cpp  ->  include/utilities/interface.hpp`
  - `+ core/specfem/source/dim3/vector_source/force_source.hpp  ->  core/specfem/source.hpp`
  - ... 6 more

### commit 2: `a70e139c44b1` — Added IO for the sources and read tests
- archcheck counters: added_edges=10, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	CMakeLists.txt`
  - `M	core/specfem/source.hpp`
  - `M	core/specfem/source/dim3/tensor_source/moment_tensor_source.cpp`
  - `M	core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp`
  - `M	core/specfem/source/dim3/vector_source.hpp`
  - `M	core/specfem/source/dim3/vector_source/force_source.cpp`
  - `M	core/specfem/source/dim3/vector_source/force_source.hpp`
  - `M	include/io/interface.hpp`
  - `R100	src/io/sources.cpp	src/io/sources/dim2/read_sources.cpp`
  - `A	src/io/sources/dim3/read_sources.cpp`
  - `R100	tests/unit-tests/io/sources/data/single_cosserat_force.yaml	tests/unit-tests/io/sources/data/dim2/single_cosserat_force.yaml`
  - `R100	tests/unit-tests/io/sources/data/single_force.yaml	tests/unit-tests/io/sources/data/dim2/single_force.yaml`
- new include edges:
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim3/tensor_source.hpp`
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim3/tensor_source/moment_tensor_source.hpp`
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim3/vector_source.hpp`
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim3/vector_source/force_source.hpp`
  - `+ src/io/sources/dim2/read_sources.cpp  ->  core/specfem/source.hpp`
  - `+ src/io/sources/dim2/read_sources.cpp  ->  include/io/interface.hpp`
  - `+ src/io/sources/dim2/read_sources.cpp  ->  include/utilities/interface.hpp`
  - `+ src/io/sources/dim3/read_sources.cpp  ->  core/specfem/source.hpp`
  - `+ src/io/sources/dim3/read_sources.cpp  ->  include/io/interface.hpp`
  - `+ src/io/sources/dim3/read_sources.cpp  ->  include/utilities/interface.hpp`

## 20. PrincetonUniversity/SPECFEMPP PR #1042

- title: Issue 982 update mass matrix
- structural review?: **False**

### commit 1: `d2fba2978559` — Update mass matrix computation routines
- archcheck counters: added_edges=14, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/assembly/fields/data_access.hpp`
  - `A	core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp`
  - `A	core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp`
  - `M	core/specfem/assembly/fields/dim2/impl/load_on_device.hpp`
  - `A	core/specfem/assembly/fields/impl/atomic_add_on_device.hpp`
  - `M	core/specfem/assembly/fields/impl/load_on_device.hpp`
  - `M	core/specfem/point/impl/field.hpp`
  - `M	include/boundary_conditions/boundary_conditions.hpp`
  - `M	include/boundary_conditions/stacey/stacey.tpp`
  - `M	include/kokkos_kernels/impl/compute_mass_matrix.tpp`
  - `M	include/medium/compute_mass_matrix.hpp`
  - `M	include/medium/dim2/acoustic/isotropic/mass_matrix.hpp`
- new include edges:
  - `+ core/specfem/assembly/fields/data_access.hpp  ->  core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp`
  - `+ core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields.hpp`
  - `+ core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp`
  - `+ core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/atomic_add_on_device.hpp`
  - `+ core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/check_accessor_compatibility.hpp`
  - `+ core/specfem/assembly/fields/dim2/atomic_add_on_device.hpp  ->  core/specfem/data_access.hpp`
  - `+ core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields.hpp`
  - `+ core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/atomic_add_on_device.hpp`
  - `+ core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/check_accessor_compatibility.hpp`
  - `+ core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/field_impl.hpp`
  - `+ core/specfem/assembly/fields/dim2/impl/atomic_add_on_device.hpp  ->  core/specfem/data_access.hpp`
  - `+ core/specfem/assembly/fields/impl/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/check_accessor_compatibility.hpp`
  - `+ core/specfem/assembly/fields/impl/atomic_add_on_device.hpp  ->  core/specfem/assembly/fields/impl/field_impl.hpp`
  - `+ core/specfem/assembly/fields/impl/atomic_add_on_device.hpp  ->  include/enumerations/macros.hpp`

## 21. PrincetonUniversity/SPECFEMPP PR #1056

- title: Issue 990 Part 8 - Dimension-Consolidated Source, Vector_Source, Tensor_Source
- structural review?: **False**

### commit 1: `5111df10b25a` — Updated the source to be fully templated
- archcheck counters: added_edges=16, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/source.hpp`
  - `M	core/specfem/source/dim2/source.cpp`
  - `D	core/specfem/source/dim2/source.hpp`
  - `A	core/specfem/source/dim2/source.tpp`
  - `M	core/specfem/source/dim3/source.cpp`
  - `A	core/specfem/source/dim3/source.tpp`
  - `R071	core/specfem/source/dim3/source.hpp	core/specfem/source/source.hpp`
  - `M	tests/unit-tests/source/typed_source_tests.cpp`
- new include edges:
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim2/source.tpp`
  - `+ core/specfem/source.hpp  ->  core/specfem/source/dim3/source.tpp`
  - `+ core/specfem/source.hpp  ->  core/specfem/source/source.hpp`
  - `+ core/specfem/source/dim2/source.tpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim2/source.tpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim2/source.tpp  ->  include/specfem_mpi/interface.hpp`
  - `+ core/specfem/source/dim3/source.tpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/dim3/source.tpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/dim3/source.tpp  ->  include/specfem_mpi/interface.hpp`
  - `+ core/specfem/source/source.hpp  ->  core/specfem/point.hpp`
  - `+ core/specfem/source/source.hpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/source.hpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/source/source.hpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/source.hpp  ->  include/quadrature/interface.hpp`
  - `+ core/specfem/source/source.hpp  ->  include/source_time_function/interface.hpp`
  - `+ core/specfem/source/source.hpp  ->  include/utilities/interface.hpp`

### commit 2: `0f88bc8ccce6` — Unified set_forcing_function.
- archcheck counters: added_edges=4, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/source.hpp`
  - `M	core/specfem/source/dim2/source.tpp`
  - `M	core/specfem/source/source.hpp`
  - `A	core/specfem/source/source.tpp`
- new include edges:
  - `+ core/specfem/source.hpp  ->  core/specfem/source/source.tpp`
  - `+ core/specfem/source/source.tpp  ->  core/specfem/source.hpp`
  - `+ core/specfem/source/source.tpp  ->  include/kokkos_abstractions.h`
  - `+ core/specfem/source/source.tpp  ->  include/specfem_mpi/interface.hpp`

## 22. PrincetonUniversity/SPECFEMPP PR #1238

- title: MESHFEM3D mesh reader for control nodes and materials 
- structural review?: **False**

### commit 1: `fa938cecc6f7` — Move all dim3 mesh related entities to generate database
- archcheck counters: added_edges=51, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	CMakeLists.txt`
  - `M	include/io/interface.hpp`
  - `M	include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `M	include/io/mesh/impl/fortran/dim3/generate_database/utilities.hpp`
  - `M	include/medium/material.hpp`
  - `M	include/mesh/dim3/generate_database/coordinates/coordinates.hpp`
  - `D	include/mesh/dim3/generate_database/element_types.hpp`
  - `M	include/mesh/dim3/generate_database/mesh.hpp`
  - `M	include/mesh/dim3/generate_database/tags/tags.hpp`
  - `M	include/mesh/dim3/meshfem3d/materials/materials.hpp`
  - `M	src/io/mesh/impl/fortran/dim2/mesh.cpp`
  - `R100	src/io/mesh/impl/fortran/dim3/mesh.cpp	src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp`
- new include edges:
  - `+ include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.tpp`
  - `+ include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/utilities.hpp`
  - `+ include/io/mesh/impl/fortran/dim3/generate_database/utilities.hpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ include/mesh/dim3/generate_database/coordinates/coordinates.hpp  ->  include/mesh/dim3/generate_database/mapping/mapping.hpp`
  - `+ include/mesh/dim3/generate_database/mesh.hpp  ->  include/mesh/dim3/generate_database/mapping/mapping.hpp`
  - `+ include/mesh/dim3/generate_database/tags/tags.hpp  ->  include/mesh/dim3/generate_database/boundaries/boundaries.hpp`
  - `+ include/mesh/dim3/generate_database/tags/tags.hpp  ->  include/mesh/dim3/generate_database/materials/materials.hpp`
  - `+ src/io/mesh/impl/fortran/dim2/mesh.cpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/enumerations/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/io/fortranio/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/io/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/kokkos_abstractions.h`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/medium/material.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/mesh/mesh.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/mesh.cpp  ->  include/specfem_mpi/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_coordinates.cpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_jacobian_matrix.cpp  ->  include/enumerations/dimension.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_jacobian_matrix.cpp  ->  include/io/fortranio/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_jacobian_matrix.cpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_jacobian_matrix.cpp  ->  include/mesh/mesh.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_jacobian_matrix.cpp  ->  include/specfem_mpi/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_parameters.cpp  ->  include/enumerations/dimension.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_parameters.cpp  ->  include/io/fortranio/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_parameters.cpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_parameters.cpp  ->  include/mesh/mesh.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/read_parameters.cpp  ->  include/specfem_mpi/interface.hpp`
  - `+ src/io/mesh/impl/fortran/dim3/generate_database/utilities.cpp  ->  include/io/mesh/impl/fortran/dim3/generate_database/interface.hpp`
  - `+ src/mesh/dim3/generate_database/boundaries/absorbing_boundary.cpp  ->  include/mesh/mesh.hpp`
  - `+ src/mesh/dim3/generate_database/boundaries/acoustic_free_surface.cpp  ->  include/mesh/mesh.hpp`
  - ... 21 more

## 23. PrincetonUniversity/SPECFEMPP PR #1303

- title: Issue 1289 - Implements 3D VTK writing.
- structural review?: **True**
- reviewer structural lines:
  - hpp` | Simplifies to forward declaration and includes for dimension-specific implementations |

### commit 1: `ae231e313d83` — Updated periodic tasks
- archcheck counters: added_edges=8, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/jacobian/CMakeLists.txt`
  - `M	core/specfem/periodic_tasks/CMakeLists.txt`
  - `M	core/specfem/periodic_tasks/check_signal.cpp`
  - `M	core/specfem/periodic_tasks/check_signal.hpp`
  - `M	core/specfem/periodic_tasks/periodic_task.hpp`
  - `A	core/specfem/periodic_tasks/plot_wavefield.hpp`
  - `M	core/specfem/periodic_tasks/plot_wavefield/dim2/plot_wavefield.cpp`
  - `M	core/specfem/periodic_tasks/plot_wavefield/dim2/plot_wavefield.hpp`
  - `M	core/specfem/periodic_tasks/plotter.hpp`
  - `M	core/specfem/periodic_tasks/wavefield_reader.cpp`
  - `M	core/specfem/periodic_tasks/wavefield_reader.hpp`
  - `M	core/specfem/periodic_tasks/wavefield_writer.cpp`
- new include edges:
  - `+ core/specfem/periodic_tasks.hpp  ->  core/specfem/periodic_tasks/plot_wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  core/specfem/periodic_tasks/plot_wavefield/dim2/plot_wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  core/specfem/periodic_tasks/plotter.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  include/enumerations/display.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  include/enumerations/wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  include/specfem_mpi/interface.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim2/plot_wavefield.hpp  ->  core/specfem/periodic_tasks/plot_wavefield.hpp`

### commit 2: `4872a1fe6223` — Added 3D VTK writing, but it shows the nans.
- archcheck counters: added_edges=13, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	benchmarks/src/dim3/homogeneous_halfspace/CMakeFiles/Par_File.in`
  - `M	benchmarks/src/dim3/homogeneous_halfspace/CMakeFiles/specfem_config.yaml.in`
  - `M	core/specfem/periodic_tasks/plot_wavefield.hpp`
  - `A	core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp`
  - `A	core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp`
  - `M	include/parameter_parser/setup.hpp`
  - `M	src/parameter_parser/writer/plot_wavefield.cpp`
  - `M	src/specfem3d.cpp`
- new include edges:
  - `+ core/specfem/periodic_tasks/plot_wavefield.hpp  ->  core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp  ->  core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp  ->  core/specfem/periodic_tasks/plotter.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp  ->  include/enumerations/display.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp  ->  include/enumerations/interface.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.cpp  ->  include/utilities/strings.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp  ->  core/specfem/periodic_tasks/plot_wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp  ->  core/specfem/periodic_tasks/plotter.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp  ->  include/enumerations/display.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp  ->  include/enumerations/wavefield.hpp`
  - `+ core/specfem/periodic_tasks/plot_wavefield/dim3/plot_wavefield.hpp  ->  include/specfem_mpi/interface.hpp`

## 24. PrincetonUniversity/SPECFEMPP PR #1535

- title: Issue 1391 - Updates the documentation for timescheme
- structural review?: **False**

### commit 1: `403dec14eb2e` — Updated the timescheme documentation
- archcheck counters: added_edges=6, removed_edges=2, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	core/specfem/timescheme.hpp`
  - `A	core/specfem/timescheme/impl/time_iterators.hpp`
  - `M	core/specfem/timescheme/newmark.hpp`
  - `M	core/specfem/timescheme/newmark.tpp`
  - `A	core/specfem/timescheme/timescheme.hpp`
  - `M	docs/Doxyfile.in`
  - `M	docs/sections/api/index.rst`
  - `M	docs/sections/api/specfem/index.rst`
  - `A	docs/sections/api/specfem/timescheme/index.rst`
  - `A	docs/sections/api/specfem/timescheme/newmark.rst`
  - `A	docs/sections/api/specfem/timescheme/newmark_combined.rst`
  - `A	docs/sections/api/specfem/timescheme/newmark_forward.rst`
- new include edges:
  - `+ core/specfem/timescheme.hpp  ->  core/specfem/timescheme/newmark.hpp`
  - `+ core/specfem/timescheme.hpp  ->  core/specfem/timescheme/timescheme.hpp`
  - `+ core/specfem/timescheme/newmark.hpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/timescheme/newmark.tpp  ->  core/specfem/assembly.hpp`
  - `+ core/specfem/timescheme/timescheme.hpp  ->  core/specfem/timescheme/impl/time_iterators.hpp`
  - `+ core/specfem/timescheme/timescheme.hpp  ->  include/enumerations/interface.hpp`

## 25. TinyMUSH/TinyMUSH PR #31

- title: Convert warning() and fatal() to standard logging infrastructure
- structural review?: **False**

### commit 1: `41fd9800821d` — Optimize string concatenation: use memcpy with pointer tracking instead of strcat
- archcheck counters: added_edges=67, removed_edges=0, grown_cycles=0, new_area_deps=1
- changed files:
  - `A	_codeql_build_dir/configs/netmush.conf`
  - `A	_codeql_build_dir/configs/netmush.conf.example`
  - `A	_codeql_build_dir/include/config.h`
  - `A	_codeql_build_dir/src/netmush/netmush`
  - `A	_codeql_build_dir/systemd/tinymush.service`
  - `A	_codeql_detected_source_root`
  - `M	src/netmush/udb_misc.c`
- new include edges:
  - `+ src/modules/comsys/comsys.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/db_sql/db_sql_msql.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/db_sql/db_sql_mysql.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/db_sql/db_sql_none.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/db_sql/db_sql_pgsql.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/db_sql/db_sql_sqlite3.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/mail/mail.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/modules/skeleton/skeleton.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/alloc.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/ansi.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/api.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/boolexp.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/bsd.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/command.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/conf.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/cque.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/create.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/db.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/db_rw.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/eval.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/file_c.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/flags.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/fnhelper.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/functions.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/funext.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/funiter.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/funlist.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/funmath.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/funmisc.c  ->  _codeql_build_dir/include/config.h`
  - `+ src/netmush/funobj.c  ->  _codeql_build_dir/include/config.h`
  - ... 37 more
- new cross-area dependencies:
  - `* src -> _codeql_build_dir (67 edges)`
  - `e.g. src/modules/comsys/comsys.c  ->  _codeql_build_dir/include/config.h`

## 26. TinyMUSH/TinyMUSH PR #75

- title: Refactor: Split eval.c into focused sub-modules
- structural review?: **True**
- reviewer structural lines:
  - This tightly couples eval.

### commit 1: `601f9bbb8247` — Refactor: Split eval.c into focused sub-modules (eval_parse, eval_tcache, eval_gender, eval_regs)
- archcheck counters: added_edges=128, removed_edges=0, grown_cycles=0, new_area_deps=1
- changed files:
  - `A	build2/configs/netmush.conf`
  - `A	build2/configs/netmush.conf.example`
  - `A	build2/include/config.h`
  - `A	build2/src/netmush/netmush`
  - `A	build2/systemd/tinymush.service`
  - `M	src/netmush/CMakeLists.txt`
  - `M	src/netmush/eval.c`
  - `A	src/netmush/eval_gender.c`
  - `A	src/netmush/eval_parse.c`
  - `A	src/netmush/eval_regs.c`
  - `A	src/netmush/eval_tcache.c`
- new include edges:
  - `+ src/modules/comsys/comsys.c  ->  build2/include/config.h`
  - `+ src/modules/db_sql/db_sql_msql.c  ->  build2/include/config.h`
  - `+ src/modules/db_sql/db_sql_mysql.c  ->  build2/include/config.h`
  - `+ src/modules/db_sql/db_sql_none.c  ->  build2/include/config.h`
  - `+ src/modules/db_sql/db_sql_pgsql.c  ->  build2/include/config.h`
  - `+ src/modules/db_sql/db_sql_sqlite3.c  ->  build2/include/config.h`
  - `+ src/modules/mail/mail.c  ->  build2/include/config.h`
  - `+ src/modules/skeleton/skeleton.c  ->  build2/include/config.h`
  - `+ src/netmush/alloc.c  ->  build2/include/config.h`
  - `+ src/netmush/ansi.c  ->  build2/include/config.h`
  - `+ src/netmush/api.c  ->  build2/include/config.h`
  - `+ src/netmush/boolexp.c  ->  build2/include/config.h`
  - `+ src/netmush/boolexp_parse.c  ->  build2/include/config.h`
  - `+ src/netmush/bsd.c  ->  build2/include/config.h`
  - `+ src/netmush/bsd_connection.c  ->  build2/include/config.h`
  - `+ src/netmush/bsd_dns.c  ->  build2/include/config.h`
  - `+ src/netmush/bsd_io.c  ->  build2/include/config.h`
  - `+ src/netmush/bsd_signals.c  ->  build2/include/config.h`
  - `+ src/netmush/bsd_socket.c  ->  build2/include/config.h`
  - `+ src/netmush/command.c  ->  build2/include/config.h`
  - `+ src/netmush/command_access.c  ->  build2/include/config.h`
  - `+ src/netmush/command_admin.c  ->  build2/include/config.h`
  - `+ src/netmush/command_core.c  ->  build2/include/config.h`
  - `+ src/netmush/command_hooks.c  ->  build2/include/config.h`
  - `+ src/netmush/conf.c  ->  build2/include/config.h`
  - `+ src/netmush/conf_core.c  ->  build2/include/config.h`
  - `+ src/netmush/conf_data.c  ->  build2/include/config.h`
  - `+ src/netmush/conf_modules.c  ->  build2/include/config.h`
  - `+ src/netmush/cque.c  ->  build2/include/config.h`
  - `+ src/netmush/cque_entry.c  ->  build2/include/config.h`
  - ... 98 more
- new cross-area dependencies:
  - `* src -> build2 (108 edges)`
  - `e.g. src/modules/comsys/comsys.c  ->  build2/include/config.h`

## 27. X4V1-code/Xorion PR #1

- title: Add green "Try Unban Me" button to Unbanner module
- structural review?: **False**

### commit 1: `5bfe299405f0` — Fix Unbanner module: add green button with "Try Unban Me" text
- archcheck counters: added_edges=22, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	CMakeLists.txt`
  - `M	Horion/Module/ModuleManager.h`
  - `M	Horion/Module/Modules/Module.h`
  - `M	Horion/Module/Modules/Unbanner.cpp`
  - `M	Horion/Module/Modules/Unbanner.h`
- new include edges:
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/AnchorNotifier.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/AutoAnchor.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/AutoCrystal.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/BedAura.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/Breadcrumbs.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/ChestESP.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/DVDLogo.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/Dicker.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/FeetPlace.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/Fucker.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/HudEditor.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/InfiniteTreasureAura.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/InventoryViewer.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/LigmaNuts.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/MaceHit.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/NoClip.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/SeeCrystals.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/TargetHUD.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/Unbanner.h`
  - `+ Horion/Module/ModuleManager.h  ->  Horion/Module/Modules/Watermark.h`
  - `+ Horion/Module/Modules/Unbanner.cpp  ->  Horion/DrawUtils.h`
  - `+ Horion/Module/Modules/Unbanner.cpp  ->  Memory/GameData.h`

## 28. X4V1-code/Xorion PR #15

- title: Rebrand Horion to Xorion
- structural review?: **False**

### commit 1: `fe8a4d63c569` — Rename Horion/ directory to Xorion/ and update all include paths
- archcheck counters: added_edges=1099, removed_edges=0, grown_cycles=0, new_area_deps=8
- changed files:
  - `M	CMakeLists.txt`
  - `M	Memory/Hooks.cpp`
  - `M	Memory/Hooks.h`
  - `M	Memory/SignatureResolver.cpp`
  - `M	SDK/XorionNameTag.h`
  - `M	Utils/ClientColors.h`
  - `M	Utils/ColorUtil.h`
  - `M	Utils/Target.cpp`
  - `M	Xorion.vcxproj`
  - `R100	Horion/Command/CommandMgr.cpp	Xorion/Command/CommandMgr.cpp`
  - `R100	Horion/Command/CommandMgr.h	Xorion/Command/CommandMgr.h`
  - `R100	Horion/Command/Commands/BindCommand.cpp	Xorion/Command/Commands/BindCommand.cpp`
- new include edges:
  - `+ Memory/Hooks.cpp  ->  Xorion/Loader.h`
  - `+ Memory/Hooks.cpp  ->  Xorion/Menu/TabGui.h`
  - `+ Memory/Hooks.h  ->  Xorion/Command/CommandMgr.h`
  - `+ Memory/Hooks.h  ->  Xorion/Config/ConfigManager.h`
  - `+ Memory/Hooks.h  ->  Xorion/DrawUtils.h`
  - `+ Memory/Hooks.h  ->  Xorion/Menu/ClickGui.h`
  - `+ Memory/Hooks.h  ->  Xorion/Module/ModuleManager.h`
  - `+ Memory/SignatureResolver.cpp  ->  Xorion/Render/Signatures.h`
  - `+ SDK/XorionNameTag.h  ->  Xorion/DrawUtils.h`
  - `+ Utils/ClientColors.h  ->  Xorion/DrawUtils.h`
  - `+ Utils/ColorUtil.h  ->  Xorion/DrawUtils.h`
  - `+ Utils/Target.cpp  ->  Xorion/FriendList/FriendsManager.h`
  - `+ Utils/Target.cpp  ->  Xorion/Module/ModuleManager.h`
  - `+ Xorion/Command/CommandMgr.cpp  ->  Utils/Logger.h`
  - `+ Xorion/Command/CommandMgr.cpp  ->  Utils/TextFormat.h`
  - `+ Xorion/Command/CommandMgr.cpp  ->  Xorion/Command/CommandMgr.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Memory/GameData.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/BindCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/BruhCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/CommandBlockExploitCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/ConfigCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/CoordsCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/DamageCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/DeviceIDCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/DupeCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/EjectCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/EnchantCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/ForceOpCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/FriendListCommand.h`
  - `+ Xorion/Command/CommandMgr.h  ->  Xorion/Command/Commands/GameModeCommand.h`
  - ... 1069 more
- new cross-area dependencies:
  - `* Memory -> Xorion (8 edges)`
  - `e.g. Memory/Hooks.cpp  ->  Xorion/Loader.h`
  - `* SDK -> Xorion (1 edge)`
  - `e.g. SDK/XorionNameTag.h  ->  Xorion/DrawUtils.h`
  - `* Utils -> Xorion (4 edges)`
  - `e.g. Utils/ClientColors.h  ->  Xorion/DrawUtils.h`
  - `* Xorion -> <root> (1 edge)`
  - `e.g. Xorion/Menu/ClickGui.cpp  ->  resource.h`
  - `* Xorion -> Memory (84 edges)`
  - `e.g. Xorion/Command/CommandMgr.h  ->  Memory/GameData.h`
  - `* Xorion -> SDK (155 edges)`
  - `e.g. Xorion/Command/Commands/CommandBlockExploitCommand.cpp  ->  SDK/Tag.h`
  - `* Xorion -> Utils (138 edges)`
  - `e.g. Xorion/Command/CommandMgr.cpp  ->  Utils/Logger.h`
  - `* Xorion -> include (12 edges)`
  - `e.g. Xorion/Config/AccountInformation.cpp  ->  include/WinHttpClient.h`

## 29. X4V1-code/Xorion PR #17

- title: Add FullAuto module with AI-driven automation and configurable behavior profiles
- structural review?: **False**

### commit 1: `90d9c95d7c14` — Add FullAuto module implementation
- archcheck counters: added_edges=7, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	CMakeLists.txt`
  - `M	Xorion/Module/ModuleManager.cpp`
  - `M	Xorion/Module/ModuleManager.h`
  - `A	Xorion/Module/Modules/FullAuto.cpp`
  - `A	Xorion/Module/Modules/FullAuto.h`
- new include edges:
  - `+ Xorion/Module/ModuleManager.h  ->  Xorion/Module/Modules/FullAuto.h`
  - `+ Xorion/Module/Modules/FullAuto.cpp  ->  Memory/GameData.h`
  - `+ Xorion/Module/Modules/FullAuto.cpp  ->  SDK/GameMode.h`
  - `+ Xorion/Module/Modules/FullAuto.cpp  ->  Utils/Target.h`
  - `+ Xorion/Module/Modules/FullAuto.cpp  ->  Xorion/Module/Modules/FullAuto.h`
  - `+ Xorion/Module/Modules/FullAuto.h  ->  Xorion/Module/ModuleManager.h`
  - `+ Xorion/Module/Modules/FullAuto.h  ->  Xorion/Module/Modules/Module.h`

## 30. X4V1-code/Xorion PR #27

- title: Add boat flight/speed modules, lava walking, and lava fire immunity
- structural review?: **False**

### commit 1: `56264605973c` — feat: add boat and lava modules
- archcheck counters: added_edges=17, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	CMakeLists.txt`
  - `M	Xorion/Module/ModuleManager.cpp`
  - `M	Xorion/Module/ModuleManager.h`
  - `A	Xorion/Module/Modules/BoatFly.cpp`
  - `A	Xorion/Module/Modules/BoatFly.h`
  - `A	Xorion/Module/Modules/BoatSpeed.cpp`
  - `A	Xorion/Module/Modules/BoatSpeed.h`
  - `M	Xorion/Module/Modules/Jesus.cpp`
  - `M	Xorion/Module/Modules/Jesus.h`
  - `A	Xorion/Module/Modules/NoLava.cpp`
  - `A	Xorion/Module/Modules/NoLava.h`
- new include edges:
  - `+ Xorion/Module/ModuleManager.h  ->  Xorion/Module/Modules/BoatFly.h`
  - `+ Xorion/Module/ModuleManager.h  ->  Xorion/Module/Modules/BoatSpeed.h`
  - `+ Xorion/Module/ModuleManager.h  ->  Xorion/Module/Modules/NoLava.h`
  - `+ Xorion/Module/Modules/BoatFly.cpp  ->  Memory/GameData.h`
  - `+ Xorion/Module/Modules/BoatFly.cpp  ->  SDK/GameSettingsInput.h`
  - `+ Xorion/Module/Modules/BoatFly.cpp  ->  Xorion/Module/Modules/BoatFly.h`
  - `+ Xorion/Module/Modules/BoatFly.h  ->  Xorion/Module/ModuleManager.h`
  - `+ Xorion/Module/Modules/BoatFly.h  ->  Xorion/Module/Modules/Module.h`
  - `+ Xorion/Module/Modules/BoatSpeed.cpp  ->  Memory/GameData.h`
  - `+ Xorion/Module/Modules/BoatSpeed.cpp  ->  SDK/GameSettingsInput.h`
  - `+ Xorion/Module/Modules/BoatSpeed.cpp  ->  Xorion/Module/Modules/BoatSpeed.h`
  - `+ Xorion/Module/Modules/BoatSpeed.h  ->  Xorion/Module/ModuleManager.h`
  - `+ Xorion/Module/Modules/BoatSpeed.h  ->  Xorion/Module/Modules/Module.h`
  - `+ Xorion/Module/Modules/NoLava.cpp  ->  Memory/GameData.h`
  - `+ Xorion/Module/Modules/NoLava.cpp  ->  Xorion/Module/Modules/NoLava.h`
  - `+ Xorion/Module/Modules/NoLava.h  ->  Xorion/Module/ModuleManager.h`
  - `+ Xorion/Module/Modules/NoLava.h  ->  Xorion/Module/Modules/Module.h`

## 31. antgroup/vsag PR #1178

- title: refactor: rename data_cell to datacell
- structural review?: **False**

### commit 1: `fe4fa673313a` — refactor: rename data_cell to datacell (#1178)
- archcheck counters: added_edges=218, removed_edges=0, grown_cycles=0, new_area_deps=0
- changed files:
  - `M	src/CMakeLists.txt`
  - `M	src/algorithm/brute_force.cpp`
  - `M	src/algorithm/brute_force_parameter.cpp`
  - `M	src/algorithm/hgraph.cpp`
  - `M	src/algorithm/hgraph.h`
  - `M	src/algorithm/hgraph_parameter.cpp`
  - `M	src/algorithm/hnswlib/hnswalg.cpp`
  - `M	src/algorithm/hnswlib/hnswalg.h`
  - `M	src/algorithm/inner_index_interface.h`
  - `M	src/algorithm/inner_index_parameter.cpp`
  - `M	src/algorithm/ivf.h`
  - `M	src/algorithm/ivf_parameter.h`
- new include edges:
  - `+ src/algorithm/brute_force.cpp  ->  src/datacell/attribute_inverted_interface.h`
  - `+ src/algorithm/brute_force.cpp  ->  src/datacell/flatten_datacell.h`
  - `+ src/algorithm/brute_force.cpp  ->  src/datacell/flatten_interface.h`
  - `+ src/algorithm/brute_force_parameter.cpp  ->  src/datacell/flatten_datacell_parameter.h`
  - `+ src/algorithm/hgraph.cpp  ->  src/datacell/compressed_graph_datacell_parameter.h`
  - `+ src/algorithm/hgraph.cpp  ->  src/datacell/sparse_graph_datacell.h`
  - `+ src/algorithm/hgraph.h  ->  src/datacell/attribute_inverted_interface.h`
  - `+ src/algorithm/hgraph.h  ->  src/datacell/flatten_interface.h`
  - `+ src/algorithm/hgraph.h  ->  src/datacell/graph_interface.h`
  - `+ src/algorithm/hgraph.h  ->  src/datacell/sparse_graph_datacell_parameter.h`
  - `+ src/algorithm/hgraph_parameter.cpp  ->  src/datacell/extra_info_datacell_parameter.h`
  - `+ src/algorithm/hgraph_parameter.cpp  ->  src/datacell/flatten_datacell_parameter.h`
  - `+ src/algorithm/hgraph_parameter.cpp  ->  src/datacell/graph_datacell_parameter.h`
  - `+ src/algorithm/hgraph_parameter.cpp  ->  src/datacell/graph_interface_parameter.h`
  - `+ src/algorithm/hgraph_parameter.cpp  ->  src/datacell/sparse_graph_datacell_parameter.h`
  - `+ src/algorithm/hgraph_parameter.cpp  ->  src/datacell/sparse_vector_datacell_parameter.h`
  - `+ src/algorithm/hnswlib/hnswalg.cpp  ->  src/datacell/graph_interface.h`
  - `+ src/algorithm/hnswlib/hnswalg.h  ->  src/datacell/flatten_interface.h`
  - `+ src/algorithm/hnswlib/hnswalg.h  ->  src/datacell/graph_interface.h`
  - `+ src/algorithm/inner_index_interface.h  ->  src/datacell/extra_info_interface.h`
  - `+ src/algorithm/inner_index_parameter.cpp  ->  src/datacell/extra_info_datacell_parameter.h`
  - `+ src/algorithm/inner_index_parameter.cpp  ->  src/datacell/flatten_datacell_parameter.h`
  - `+ src/algorithm/ivf.h  ->  src/datacell/attribute_bucket_inverted_datacell.h`
  - `+ src/algorithm/ivf.h  ->  src/datacell/bucket_datacell.h`
  - `+ src/algorithm/ivf.h  ->  src/datacell/flatten_interface.h`
  - `+ src/algorithm/ivf_parameter.h  ->  src/datacell/bucket_datacell_parameter.h`
  - `+ src/algorithm/ivf_parameter.h  ->  src/datacell/flatten_datacell_parameter.h`
  - `+ src/algorithm/ivf_partition/gno_imi_parameter.h  ->  src/datacell/bucket_datacell_parameter.h`
  - `+ src/algorithm/ivf_partition/ivf_partition_strategy_parameter.h  ->  src/datacell/bucket_datacell_parameter.h`
  - `+ src/algorithm/pyramid.cpp  ->  src/datacell/flatten_interface.h`
  - ... 188 more

## 32. antgroup/vsag PR #2076

- title: feat: add MultiVectorDataCell for ColBERT-style multi-vector storage
- structural review?: **False**

### commit 1: `bf22d9c908e5` — feat: add MultiVectorDataCell for ColBERT-style multi-vector storage (#2076)
- archcheck counters: added_edges=24, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	src/algorithm/hgraph.cpp`
  - `M	src/datacell/flatten_interface.cpp`
  - `M	src/datacell/flatten_interface_parameter.cpp`
  - `A	src/datacell/multi_vector_datacell.h`
  - `A	src/datacell/multi_vector_datacell.inl`
  - `A	src/datacell/multi_vector_datacell_parameter.h`
  - `A	src/datacell/multi_vector_datacell_test.cpp`
  - `M	src/impl/reorder/flatten_reorder.cpp`
  - `M	src/impl/reorder/flatten_reorder.h`
  - `M	src/impl/reorder/reorder.h`
  - `M	src/inner_string_params.h`
  - `M	src/quantization/CMakeLists.txt`
- new include edges:
  - `+ src/datacell/flatten_interface.cpp  ->  src/datacell/multi_vector_datacell.h`
  - `+ src/datacell/flatten_interface_parameter.cpp  ->  src/datacell/multi_vector_datacell_parameter.h`
  - `+ src/datacell/multi_vector_datacell.h  ->  include/vsag/dataset.h`
  - `+ src/datacell/multi_vector_datacell.h  ->  src/datacell/flatten_interface.h`
  - `+ src/datacell/multi_vector_datacell.h  ->  src/datacell/multi_vector_datacell.inl`
  - `+ src/datacell/multi_vector_datacell.h  ->  src/io/basic_io.h`
  - `+ src/datacell/multi_vector_datacell.h  ->  src/io/memory_block_io.h`
  - `+ src/datacell/multi_vector_datacell.h  ->  src/quantization/multi_vector_computer.h`
  - `+ src/datacell/multi_vector_datacell.inl  ->  include/vsag/options.h`
  - `+ src/datacell/multi_vector_datacell.inl  ->  src/datacell/multi_vector_datacell.h`
  - `+ src/datacell/multi_vector_datacell.inl  ->  src/utils/byte_buffer.h`
  - `+ src/datacell/multi_vector_datacell_parameter.h  ->  src/datacell/flatten_interface_parameter.h`
  - `+ src/datacell/multi_vector_datacell_parameter.h  ->  src/impl/logger/logger.h`
  - `+ src/datacell/multi_vector_datacell_parameter.h  ->  src/inner_string_params.h`
  - `+ src/datacell/multi_vector_datacell_parameter.h  ->  src/quantization/fp32_quantizer_parameter.h`
  - `+ src/datacell/multi_vector_datacell_parameter.h  ->  src/utils/pointer_define.h`
  - `+ src/quantization/multi_vector_computer.cpp  ->  src/quantization/multi_vector_computer.h`
  - `+ src/quantization/multi_vector_computer.cpp  ->  src/simd/fp32_simd.h`
  - `+ src/quantization/multi_vector_computer.cpp  ->  src/vsag_exception.h`
  - `+ src/quantization/multi_vector_computer.h  ->  include/vsag/allocator.h`
  - `+ src/quantization/multi_vector_computer.h  ->  src/metric_type.h`
  - `+ src/quantization/multi_vector_computer.h  ->  src/quantization/computer.h`
  - `+ src/quantization/multi_vector_computer.h  ->  src/typing.h`
  - `+ src/quantization/multi_vector_computer.h  ->  src/utils/pointer_define.h`

## 33. community-shaders/skyrim-community-shaders PR #1178

- title: refactor(ui): improve menu layout and organization
- structural review?: **True**
- reviewer structural lines:
  - ✅ **Proper separation of concerns**: Settings vs.

### commit 1: `d19fd3b61fa6` — refactor(ui): improve menu layout and organization (#1178)
- archcheck counters: added_edges=1, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	src/Menu.cpp`
  - `M	src/Menu.h`
  - `M	src/Utils/UI.cpp`
  - `M	src/Utils/UI.h`
- new include edges:
  - `+ src/Menu.h  ->  src/Feature.h`

## 34. community-shaders/skyrim-community-shaders PR #1630

- title: feat: weather and imagespace editor
- structural review?: **True**
- reviewer structural lines:
  - Forward declaration follows established pattern.
  - The WeatherEditor forward declaration is consistent with other feature forward declarations in the file.
  - The separation of concerns with `ApplyChanges`/`RevertChanges` and the bidirectional synchronization methods (`SetLightingTemplateValues`/`LoadLightingTemplateValues`) follows good UI design patterns.

### commit 1: `7d0dd10d9c1c` — feat: weather and imagespace editor (#1630)
- archcheck counters: added_edges=100, removed_edges=0, grown_cycles=0, new_area_deps=1
- changed files:
  - `A	docs/weather-system-docs/WeatherVariableRegistration.md`
  - `A	docs/weather-system-docs/examples/ExampleWeatherFeature.cpp`
  - `A	docs/weather-system-docs/examples/ExampleWeatherFeature.h`
  - `A	features/Weather Editor/CORE`
  - `A	features/Weather Editor/Shaders/Features/WeatherEditor.ini`
  - `A	package/Interface/CommunityShaders/Icons/Action Icons/apply-to-game.png`
  - `A	package/Interface/CommunityShaders/Icons/Action Icons/delete.png`
  - `A	package/Interface/CommunityShaders/Icons/Action Icons/pause.png`
  - `A	package/Interface/CommunityShaders/Icons/Action Icons/undo.png`
  - `M	package/Interface/CommunityShaders/Icons/Categories/sky.png`
  - `M	src/Deferred.cpp`
  - `M	src/Feature.cpp`
- new include edges:
  - `+ docs/weather-system-docs/examples/ExampleWeatherFeature.cpp  ->  docs/weather-system-docs/examples/ExampleWeatherFeature.h`
  - `+ docs/weather-system-docs/examples/ExampleWeatherFeature.cpp  ->  src/WeatherVariableRegistry.h`
  - `+ docs/weather-system-docs/examples/ExampleWeatherFeature.h  ->  src/Feature.h`
  - `+ src/Deferred.cpp  ->  src/Features/WeatherEditor.h`
  - `+ src/Feature.cpp  ->  src/Features/WeatherEditor.h`
  - `+ src/Features/WeatherEditor.cpp  ->  src/Deferred.h`
  - `+ src/Features/WeatherEditor.cpp  ->  src/Features/WeatherEditor.h`
  - `+ src/Features/WeatherEditor.cpp  ->  src/State.h`
  - `+ src/Features/WeatherEditor.cpp  ->  src/Util.h`
  - `+ src/Features/WeatherEditor.cpp  ->  src/WeatherEditor/EditorWindow.h`
  - `+ src/Features/WeatherEditor.cpp  ->  src/WeatherManager.h`
  - `+ src/Features/WeatherEditor.h  ->  src/Buffer.h`
  - `+ src/Features/WeatherEditor.h  ->  src/Feature.h`
  - `+ src/Features/WeatherEditor.h  ->  src/State.h`
  - `+ src/Globals.cpp  ->  src/Features/WeatherEditor.h`
  - `+ src/Menu.cpp  ->  src/WeatherEditor/EditorWindow.h`
  - `+ src/State.cpp  ->  src/Features/WeatherEditor.h`
  - `+ src/State.cpp  ->  src/WeatherManager.h`
  - `+ src/Util.h  ->  src/Utils/Form.h`
  - `+ src/Utils/Form.cpp  ->  src/Utils/Form.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/Features/WeatherEditor.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/Menu.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/State.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/Utils/UI.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/WeatherEditor/EditorWindow.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/WeatherEditor/PaletteWindow.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/WeatherEditor/Weather/LightingTemplateWidget.h`
  - `+ src/WeatherEditor/EditorWindow.cpp  ->  src/WeatherEditor/WeatherUtils.h`
  - `+ src/WeatherEditor/EditorWindow.h  ->  src/Buffer.h`
  - `+ src/WeatherEditor/EditorWindow.h  ->  src/WeatherEditor/Weather/CellLightingWidget.h`
  - ... 70 more
- new cross-area dependencies:
  - `* docs -> src (2 edges)`
  - `e.g. docs/weather-system-docs/examples/ExampleWeatherFeature.cpp  ->  src/WeatherVariableRegistry.h`

## 35. drsnuggles8/OloEngineBase PR #91

- title: Feature/memory allocator
- structural review?: **True**
- reviewer structural lines:
  - h | Forward declarations for all container types and allocators |

### commit 1: `5e16d53ce40c` — feat(templates): Port UE5.7 type traits and template utilities
- archcheck counters: added_edges=14, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `A	OloEngine/src/OloEngine/Templates/FunctionRef.h`
  - `A	OloEngine/src/OloEngine/Templates/IdentityFunctor.h`
  - `A	OloEngine/src/OloEngine/Templates/Projection.h`
  - `A	OloEngine/src/OloEngine/Templates/RetainedRef.h`
  - `A	OloEngine/src/OloEngine/Templates/ReversePredicate.h`
  - `A	OloEngine/src/OloEngine/Templates/Sorting.h`
  - `A	OloEngine/src/OloEngine/Templates/TypeHash.h`
  - `A	OloEngine/src/OloEngine/Templates/UnrealTemplate.h`
  - `A	OloEngine/src/OloEngine/Templates/UnrealTypeTraits.h`
- new include edges:
  - `+ OloEngine/src/OloEngine/Memory/LockFreeList.h  ->  OloEngine/src/OloEngine/Templates/FunctionRef.h`
  - `+ OloEngine/src/OloEngine/Memory/MemoryOps.h  ->  OloEngine/src/OloEngine/Templates/UnrealTypeTraits.h`
  - `+ OloEngine/src/OloEngine/Memory/TypeTraits.h  ->  OloEngine/src/OloEngine/Templates/UnrealTemplate.h`
  - `+ OloEngine/src/OloEngine/Templates/FunctionRef.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/IdentityFunctor.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/Projection.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/ReversePredicate.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/Sorting.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/Sorting.h  ->  OloEngine/src/OloEngine/Templates/UnrealTypeTraits.h`
  - `+ OloEngine/src/OloEngine/Templates/TypeHash.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/UnrealTemplate.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/UnrealTemplate.h  ->  OloEngine/src/OloEngine/Templates/UnrealTypeTraits.h`
  - `+ OloEngine/src/OloEngine/Templates/UnrealTypeTraits.h  ->  OloEngine/src/OloEngine/Core/Base.h`
  - `+ OloEngine/src/OloEngine/Templates/UnrealTypeTraits.h  ->  OloEngine/src/OloEngine/Templates/UnrealTemplate.h`

## 36. jank-lang/jank PR #757

- title: Add pointer tagging for inline integers
- structural review?: **False**

### commit 1: `a494904daa6d` — Add pointer tagging for inline integers (#757)
- archcheck counters: added_edges=12, removed_edges=3, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	.clang-tidy`
  - `M	compiler+runtime/CMakeLists.txt`
  - `M	compiler+runtime/include/cpp/jank/codegen/api.hpp`
  - `M	compiler+runtime/include/cpp/jank/hash.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/behavior/comparable.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/convert/builtin.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/core/make_box.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/core/math.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/core/truthy.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/obj/atom.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/obj/big_decimal.hpp`
  - `M	compiler+runtime/include/cpp/jank/runtime/obj/big_integer.hpp`
- new include edges:
  - `+ compiler+runtime/include/cpp/jank/runtime/core/truthy.hpp  ->  compiler+runtime/include/cpp/jank/runtime/oref.hpp`
  - `+ compiler+runtime/include/cpp/jank/runtime/oref.hpp  ->  compiler+runtime/include/cpp/jank/runtime/obj/nil.hpp`
  - `+ compiler+runtime/include/cpp/jank/runtime/oref.hpp  ->  compiler+runtime/include/cpp/jank/runtime/obj/number.hpp`
  - `+ compiler+runtime/src/cpp/jank/analyze/processor.cpp  ->  compiler+runtime/include/cpp/jank/runtime/core/math.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/obj/array_chunk.cpp  ->  compiler+runtime/include/cpp/jank/runtime/core/math.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/obj/nil.cpp  ->  compiler+runtime/include/cpp/jank/runtime/oref.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/obj/number.cpp  ->  compiler+runtime/include/cpp/jank/runtime/oref.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/obj/persistent_string.cpp  ->  compiler+runtime/include/cpp/jank/runtime/core/math.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/obj/persistent_vector.cpp  ->  compiler+runtime/include/cpp/jank/runtime/core/math.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/obj/transient_vector.cpp  ->  compiler+runtime/include/cpp/jank/runtime/core/math.hpp`
  - `+ compiler+runtime/src/cpp/jank/runtime/rtti.cpp  ->  compiler+runtime/include/cpp/jank/runtime/rtti.hpp`
  - `+ compiler+runtime/src/cpp/jtl/string_builder.cpp  ->  compiler+runtime/include/cpp/jank/runtime/obj/ratio.hpp`

## 37. linux-nvme/nvme-cli PR #3391

- title: Split the nvme commands and types headers
- structural review?: **False**

### commit 1: `dafe0407f7d0` — nvme-types: move fabrics definitions into separate header
- archcheck counters: added_edges=3, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	libnvme/libnvme/nvme.i`
  - `M	libnvme/src/meson.build`
  - `A	libnvme/src/nvme/nvme-types-fabrics.h`
  - `M	libnvme/src/nvme/nvme-types.h`
  - `M	libnvme/test/meson.build`
- new include edges:
  - `+ libnvme/src/nvme/nvme-types-fabrics.h  ->  libnvme/src/nvme/nvme-types.h`
  - `+ libnvme/src/nvme/nvme-types-fabrics.h  ->  libnvme/src/nvme/types.h`
  - `+ libnvme/src/nvme/nvme-types.h  ->  libnvme/src/nvme/nvme-types-fabrics.h`

### commit 2: `cf8b22d9a606` — nvme-types: move mi definitions into separate header
- archcheck counters: added_edges=3, removed_edges=0, grown_cycles=1, new_area_deps=0
- changed files:
  - `M	libnvme/libnvme/nvme.i`
  - `M	libnvme/src/meson.build`
  - `A	libnvme/src/nvme/nvme-types-mi.h`
  - `M	libnvme/src/nvme/nvme-types.h`
  - `M	libnvme/test/meson.build`
- new include edges:
  - `+ libnvme/src/nvme/nvme-types-mi.h  ->  libnvme/src/nvme/nvme-types.h`
  - `+ libnvme/src/nvme/nvme-types-mi.h  ->  libnvme/src/nvme/types.h`
  - `+ libnvme/src/nvme/nvme-types.h  ->  libnvme/src/nvme/nvme-types-mi.h`

## 38. ros-navigation/navigation2 PR #5288

- title: Large Nav2 Node, Utils, and Interface Refactor
- structural review?: **False**

### commit 1: `a9fcc5b37d93` — Large Nav2 Node, Utils, and Interface Refactor (#5288)
- archcheck counters: added_edges=316, removed_edges=0, grown_cycles=0, new_area_deps=28
- changed files:
  - `M	.circleci/config.yml`
  - `M	nav2_amcl/CMakeLists.txt`
  - `M	nav2_amcl/include/nav2_amcl/amcl_node.hpp`
  - `M	nav2_amcl/package.xml`
  - `M	nav2_amcl/src/amcl_node.cpp`
  - `M	nav2_behavior_tree/CMakeLists.txt`
  - `M	nav2_behavior_tree/include/nav2_behavior_tree/behavior_tree_engine.hpp`
  - `M	nav2_behavior_tree/include/nav2_behavior_tree/bt_action_node.hpp`
  - `M	nav2_behavior_tree/include/nav2_behavior_tree/bt_action_server.hpp`
  - `M	nav2_behavior_tree/include/nav2_behavior_tree/bt_action_server_impl.hpp`
  - `M	nav2_behavior_tree/include/nav2_behavior_tree/bt_cancel_action_node.hpp`
  - `M	nav2_behavior_tree/include/nav2_behavior_tree/bt_service_node.hpp`
- new include edges:
  - `+ nav2_amcl/include/nav2_amcl/amcl_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_amcl/include/nav2_amcl/amcl_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/service_server.hpp`
  - `+ nav2_amcl/src/amcl_node.cpp  ->  nav2_ros_common/include/nav2_ros_common/validate_messages.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/behavior_tree_engine.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_action_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/node_utils.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_action_server.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_action_server.hpp  ->  nav2_ros_common/include/nav2_ros_common/simple_action_server.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_action_server_impl.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_action_server_impl.hpp  ->  nav2_ros_common/include/nav2_ros_common/node_utils.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_cancel_action_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/node_utils.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_service_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_service_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/node_utils.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_service_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/service_client.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/bt_utils.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/append_goal_pose_to_goals_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/assisted_teleop_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/assisted_teleop_cancel_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/back_up_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/back_up_cancel_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/clear_costmap_service.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/compute_and_track_route_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/compute_and_track_route_cancel_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/compute_path_through_poses_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/compute_path_to_pose_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/compute_route_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/concatenate_paths_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/controller_cancel_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/controller_selector_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/drive_on_heading_action.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `+ nav2_behavior_tree/include/nav2_behavior_tree/plugins/action/drive_on_heading_cancel_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - ... 286 more
- new cross-area dependencies:
  - `* nav2_amcl -> nav2_ros_common (3 edges)`
  - `e.g. nav2_amcl/include/nav2_amcl/amcl_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_behavior_tree -> nav2_ros_common (64 edges)`
  - `e.g. nav2_behavior_tree/include/nav2_behavior_tree/behavior_tree_engine.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_behaviors -> nav2_ros_common (6 edges)`
  - `e.g. nav2_behaviors/include/nav2_behaviors/behavior_server.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_bt_navigator -> nav2_ros_common (2 edges)`
  - `e.g. nav2_bt_navigator/include/nav2_bt_navigator/bt_navigator.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_collision_monitor -> nav2_ros_common (14 edges)`
  - `e.g. nav2_collision_monitor/include/nav2_collision_monitor/collision_detector_node.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_constrained_smoother -> nav2_ros_common (4 edges)`
  - `e.g. nav2_constrained_smoother/include/nav2_constrained_smoother/constrained_smoother.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_controller -> nav2_ros_common (13 edges)`
  - `e.g. nav2_controller/include/nav2_controller/controller_server.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_core -> nav2_ros_common (8 edges)`
  - `e.g. nav2_core/include/nav2_core/behavior.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_costmap_2d -> nav2_ros_common (18 edges)`
  - `e.g. nav2_costmap_2d/include/nav2_costmap_2d/clear_costmap_service.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_docking -> nav2_ros_common (16 edges)`
  - `e.g. nav2_docking/opennav_docking/include/opennav_docking/controller.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_dwb_controller -> nav2_ros_common (23 edges)`
  - `e.g. nav2_dwb_controller/dwb_core/include/dwb_core/publisher.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_graceful_controller -> nav2_ros_common (3 edges)`
  - `e.g. nav2_graceful_controller/include/nav2_graceful_controller/parameter_handler.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_lifecycle_manager -> nav2_ros_common (5 edges)`
  - `e.g. nav2_lifecycle_manager/include/nav2_lifecycle_manager/lifecycle_manager.hpp  ->  nav2_ros_common/include/nav2_ros_common/node_thread.hpp`
  - `* nav2_map_server -> nav2_ros_common (5 edges)`
  - `e.g. nav2_map_server/include/nav2_map_server/costmap_filter_info_server.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_mppi_controller -> nav2_ros_common (7 edges)`
  - `e.g. nav2_mppi_controller/include/nav2_mppi_controller/controller.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_navfn_planner -> nav2_ros_common (2 edges)`
  - `e.g. nav2_navfn_planner/include/nav2_navfn_planner/navfn_planner.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_planner -> nav2_ros_common (4 edges)`
  - `e.g. nav2_planner/include/nav2_planner/planner_server.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_regulated_pure_pursuit_controller -> nav2_ros_common (8 edges)`
  - `e.g. nav2_regulated_pure_pursuit_controller/include/nav2_regulated_pure_pursuit_controller/collision_checker.hpp  ->  nav2_ros_common/include/nav2_ros_common/lifecycle_node.hpp`
  - `* nav2_rotation_shim_controller -> nav2_ros_common (1 edge)`
  - `e.g. nav2_rotation_shim_controller/src/nav2_rotation_shim_controller.cpp  ->  nav2_ros_common/include/nav2_ros_common/node_utils.hpp`
  - `* nav2_route -> nav2_ros_common (52 edges)`
  - `e.g. nav2_route/include/nav2_route/edge_scorer.hpp  ->  nav2_ros_common/include/nav2_ros_common/node_utils.hpp`
  - ... 16 more

