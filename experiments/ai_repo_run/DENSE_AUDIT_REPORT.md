# Аудит dense-корпуса: сигналы чекера для ГЛАЗНОЙ проверки

> **Чекеру (спайки #056/#053, archcheck) на данный момент НЕ доверяем как истине.**
> Ниже — его *claims* для проверки глазами: `cd <repo>; git checkout <HEAD-SHA>`,
> открыть указанные `file:line` — реальная копипаста/цикл или ложняк.
>
> - **Дубли:** детектор #056 token-precise (selective normalization + token-LCS@0.80,
>   FP-фильтры) — пары `[token-LCS=K] file:a-b <-> file:c-d`, только **cross-file**.
>   `line<1` при `LCS=1` = diverged-копия (переименовано/правлено).
> - **Циклы:** archcheck SF.9 — цепочка `A → … → A` (include-цикл).
> - **Траектория dup%:** грубый тренд line-#053 по 25 снапшотам (только направление роста).
> Дата: 2026-05-31.

## ahmed-abdelhaleem_echo
- HEAD: `3b12546` (2026-05-26), subpath: `.`
- Траектория (claim): dup 0.0%→0.0% (Δ+0.0, line-#053), nodes 0→16, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## AmoghS1234_CipherMesh
- HEAD: `aff3686` (2026-05-29), subpath: `src`
- Траектория (claim): dup 7.4%→7.3% (Δ-0.1, line-#053), nodes 38→51, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] core/vault.cpp:58-70 <-> mobile/app/src/main/cpp/native-lib.cpp:104-116
[token-LCS=0.982659] core/vault.cpp:92-120 <-> mobile/app/src/main/cpp/native-lib.cpp:53-75
[token-LCS=0.962687] core/vault.cpp:73-89 <-> p2p_webrtc/webrtcservice.cpp:42-56
[token-LCS=0.941176] desktop/mainwindow.cpp:1757-1760 <-> desktop/toast.cpp:20-26
[token-LCS=0.930057] desktop/breach_checker.cpp:12-49 <-> services/breach_service.cpp:49-89
[token-LCS=0.920455] desktop/newentrydialog.cpp:280-294 <-> desktop/passwordgeneratordialog.cpp:173-187
```

## arran4_kgithub-notify
- HEAD: `4852a46` (2026-05-22), subpath: `src`
- Траектория (claim): dup 0.0%→7.7% (Δ+7.7, line-#053), nodes 0→36, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.966667] ActionWindow.cpp:14-20 <-> PullRequestWindow.cpp:102-108
[token-LCS=0.85342] NotificationListWidget.cpp:117-142 <-> NotificationWindow.cpp:243-262
[token-LCS=0.85342] NotificationListWidget.cpp:117-142 <-> NotificationWindow.cpp:187-206
[token-LCS=0.850277] RepoListWindow.cpp:120-153 <-> WorkItemWindow.cpp:251-285
[token-LCS=0.828866] ActionWindow.cpp:84-109 <-> PullRequestWindow.cpp:467-489
```

## arran4_kjules
- HEAD: `3db1ae5` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→14.8% (Δ+14.8, line-#053), nodes 0→66, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] activitylogwindow.cpp:42-45 <-> sourcesrefreshprogresswindow.cpp:69-72
[token-LCS=1] errorsmodel.cpp:102-115 <-> templatesmodel.cpp:116-129
[token-LCS=1] errorsmodel.cpp:102-115 <-> sourcemodel.cpp:679-692
[token-LCS=1] sourcemodel.cpp:679-692 <-> templatesmodel.cpp:116-129
[token-LCS=1] queuedelegate.cpp:19-21 <-> templatedelegate.cpp:19-21
[token-LCS=0.97619] draftsmodel.cpp:76-82 <-> errorsmodel.cpp:75-81
[token-LCS=0.97619] draftsmodel.cpp:76-82 <-> templatesmodel.cpp:87-93
[token-LCS=0.97619] errorsmodel.cpp:75-81 <-> templatesmodel.cpp:87-93
```

## arran4_kllamabooks
- HEAD: `b16c4d9` (2026-05-29), subpath: `src`
- Траектория (claim): dup 0.0%→17.1% (Δ+17.1, line-#053), nodes 0→58, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] AIOperationsDialog.cpp:207-236 <-> AiActionDialog.cpp:66-95
[token-LCS=1] AiActionDialog.cpp:111-138 <-> MergeDocumentsDialog.cpp:497-524
[token-LCS=1] AiActionDialog.cpp:66-95 <-> MergeDocumentsDialog.cpp:452-481
[token-LCS=1] AIOperationsEditorWidget.cpp:180-184 <-> DocumentTemplatesEditorWidget.cpp:201-205
[token-LCS=1] AIOperationsDialog.cpp:252-279 <-> AiActionDialog.cpp:111-138
[token-LCS=1] AIOperationsDialog.cpp:252-279 <-> MergeDocumentsDialog.cpp:497-524
[token-LCS=1] AIOperationsDialog.cpp:207-236 <-> MergeDocumentsDialog.cpp:452-481
[token-LCS=1] AIOperationsDialog.cpp:183-188 <-> NewDocumentDialog.cpp:238-243
```

## arran4_KMagMux
- HEAD: `1fee0a1` (2026-05-21), subpath: `src`
- Траектория (claim): dup 0.0%→14.8% (Δ+14.8, line-#053), nodes 0→51, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] plugins/putio/PutIoConnector.cpp:37-80 <-> plugins/torbox/TorBoxConnector.cpp:38-83
[token-LCS=1] plugins/realdebrid/RealDebridConnector.cpp:137-144 <-> plugins/torbox/TorBoxConnector.cpp:107-114
[token-LCS=1] plugins/realdebrid/RealDebridConnector.cpp:227-265 <-> plugins/torbox/TorBoxConnector.cpp:151-187
[token-LCS=1] plugins/realdebrid/RealDebridConnector.cpp:268-292 <-> plugins/torbox/TorBoxConnector.cpp:190-214
[token-LCS=1] plugins/putio/PutIoConnector.cpp:134-158 <-> plugins/torbox/TorBoxConnector.cpp:190-214
[token-LCS=1] plugins/putio/PutIoConnector.cpp:134-158 <-> plugins/realdebrid/RealDebridConnector.cpp:268-292
[token-LCS=1] plugins/putio/PutIoConnector.cpp:94-131 <-> plugins/torbox/TorBoxConnector.cpp:151-187
[token-LCS=1] plugins/putio/PutIoConnector.cpp:94-131 <-> plugins/realdebrid/RealDebridConnector.cpp:227-265
```

## Autonomy-Logic_matiec
- HEAD: `75d3755` (2026-02-26), subpath: `lib`
- Траектория (claim): dup 0.0%→36.4% (Δ+36.4, line-#053), nodes 0→15, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.929155] p1am_FB.h:323-353 <-> sm_cards.h:542-586
[token-LCS=0.891213] p1am_FB.h:273-295 <-> sm_cards.h:473-510
[token-LCS=0.884615] p1am_FB.h:424-453 <-> sm_cards.h:410-452
[token-LCS=0.833333] p1am_FB.h:374-395 <-> sm_cards.h:1249-1283
[token-LCS=0.833333] p1am_FB.h:374-395 <-> sm_cards.h:347-380
```

## averyizatt_DIYComfortControlModule
- HEAD: `b66b233` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→6.4% (Δ+6.4, line-#053), nodes 0→44, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.905172] sensors/pressure_sensor.cpp:28-50 <-> sensors/thermistor_sensor.cpp:30-51
```

## awest813_Dewm-3
- HEAD: `56c7cf9` (2026-05-30), subpath: `.`
- Траектория (claim): dup 42.0%→46.2% (Δ+4.2, line-#053), nodes 1367→1108, макс циклов=3, макс SCC=4
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] neo/d3xp/gamesys/Class.cpp:354-364 <-> neo/game/gamesys/Class.cpp:354-364
[token-LCS=1] neo/d3xp/gamesys/SaveGame.cpp:98-116 <-> neo/game/gamesys/SaveGame.cpp:98-116
[token-LCS=1] neo/d3xp/gamesys/Event.cpp:864-911 <-> neo/game/gamesys/Event.cpp:721-768
[token-LCS=1] neo/d3xp/gamesys/Event.cpp:749-792 <-> neo/game/gamesys/Event.cpp:623-666
[token-LCS=1] neo/d3xp/gamesys/Event.cpp:709-722 <-> neo/game/gamesys/Event.cpp:583-596
[token-LCS=1] neo/d3xp/gamesys/Event.cpp:675-701 <-> neo/game/gamesys/Event.cpp:549-575
[token-LCS=1] neo/d3xp/gamesys/Event.cpp:335-355 <-> neo/game/gamesys/Event.cpp:332-352
[token-LCS=1] neo/d3xp/gamesys/DebugGraph.cpp:50-54 <-> neo/game/gamesys/DebugGraph.cpp:50-54
```
- Циклы включений (claim):
```
neo/renderer/Interaction.h → neo/renderer/tr_local.h → neo/renderer/Interaction.h
neo/tools/compilers/aas/AASBuild_local.h → neo/tools/compilers/aas/AASCluster.h → neo/tools/compilers/aas/AASBuild_local.h
neo/ui/GuiScript.h → neo/ui/Window.h → neo/ui/GuiScript.h
```

## CanerKaraca23_classic-axis
- HEAD: `dcb0d45` (2026-05-25), subpath: `source`
- Траектория (claim): dup 65.4%→11.0% (Δ-54.4, line-#053), nodes 327→6, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## canonical_shopkeeper
- HEAD: `943fbd9` (2026-05-23), subpath: `src`
- Траектория (claim): dup 0.0%→8.8% (Δ+8.8, line-#053), nodes 0→53, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.914894] interfacepanel.cpp:90-100 <-> tunnelpanel.cpp:153-162
[token-LCS=0.867925] inprojectsdkwizard.cpp:372-381 <-> sketchsdkpanel.cpp:465-475
[token-LCS=0.86758] inprojectsdkwizard.cpp:359-368 <-> sketchsdkpanel.cpp:465-475
[token-LCS=0.86758] inprojectsdkwizard.cpp:346-355 <-> sketchsdkpanel.cpp:465-475
[token-LCS=0.866359] inprojectsdkwizard.cpp:372-381 <-> sketchsdkpanel.cpp:479-489
[token-LCS=0.866071] inprojectsdkwizard.cpp:359-368 <-> sketchsdkpanel.cpp:479-489
[token-LCS=0.866071] inprojectsdkwizard.cpp:346-355 <-> sketchsdkpanel.cpp:479-489
```

## chrxh_alien
- HEAD: `caeee1608` (2026-05-31), subpath: `source`
- Траектория (claim): dup 0.0%→18.2% (Δ+18.2, line-#053), nodes 0→208, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] Gui/ImageToPatternDialog.cpp:28-33 <-> Gui/PatternEditorWindow.cpp:35-40
[token-LCS=1] Gui/InspectionWindow.cpp:774-781 <-> Gui/NodeEditorWidget.cpp:587-595
[token-LCS=1] Gui/SimulationParametersLayerWidget.cpp:19-36 <-> Gui/SimulationParametersSourceWidget.cpp:20-38
```

## csgaokun_vscode_claude
- HEAD: `282fa4aa` (2026-05-30), subpath: `.`
- Траектория (claim): dup 0.0%→4.3% (Δ+4.3, line-#053), nodes 0→1406, макс циклов=6, макс SCC=2
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] qtcreator/hldplugin-src/src/plugins/coreplugin/dialogs/externaltoolconfig.cpp:828-832 <-> qtcreator/hldplugin-src/src/plugins/coreplugin/systemsettings.cpp:262-266
[token-LCS=1] qtcreator/hldplugin-src/src/plugins/coreplugin/mimetypesettings.cpp:652-657 <-> qtcreator/hldplugin-src/src/plugins/texteditor/snippets/snippetssettingspage.cpp:563-568
[token-LCS=1] qtcreator/hldplugin-src/src/plugins/coreplugin/generalsettings.cpp:55-75 <-> qtcreator/hldplugin-src/src/plugins/coreplugin/themechooser.cpp:56-76
[token-LCS=1] qtcreator/hldplugin-src/src/libs/utils/treemodel.cpp:557-559 <-> qtcreator/hldplugin-src/src/shared/modeltest/modeltest.cpp:494-496
[token-LCS=1] qtcreator/hldplugin-src/src/libs/utils/treemodel.cpp:392-450 <-> qtcreator/hldplugin-src/src/shared/modeltest/modeltest.cpp:329-387
[token-LCS=1] qtcreator/hldplugin-src/src/libs/utils/treemodel.cpp:304-343 <-> qtcreator/hldplugin-src/src/shared/modeltest/modeltest.cpp:241-280
[token-LCS=1] qtcreator/hldplugin-src/src/libs/utils/treemodel.cpp:247-263 <-> qtcreator/hldplugin-src/src/shared/modeltest/modeltest.cpp:184-200
[token-LCS=1] qtcreator/hldplugin-src/src/libs/utils/treemodel.cpp:204-221 <-> qtcreator/hldplugin-src/src/shared/modeltest/modeltest.cpp:141-158
```
- Циклы включений (claim):
```
qtcreator/hldplugin-src/src/plugins/coreplugin/actionmanager/actioncontainer_p.h → qtcreator/hldplugin-src/src/plugins/coreplugin/actionmanager/actionmanager_p.h → qtcreator/hldplugin-src/src/plugins/coreplugin/actionmanager/actioncontainer_p.h
```

## DavidBechtold_MIPSCore
- HEAD: `4df315b` (2026-05-29), subpath: `.`
- Траектория (claim): dup 0.0%→40.3% (Δ+40.3, line-#053), nodes 0→1354, макс циклов=2, макс SCC=9
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_hashtable_size.h:66-75 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_vector_size.h:66-75
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/bits/forward_list.h:572-578 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/bits/stl_vector.h:450-456
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_hash_func.h:133-141 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_vector_to_list.h:199-208
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/ext/pb_ds/detail/rb_tree_map_/erase_fn_imps.hpp:58-67 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/ext/pb_ds/detail/splay_tree_/erase_fn_imps.hpp:58-66
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/bits/forward_list.h:572-578 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/bits/hashtable.h:456-462
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/mips-linux-gnu/uclibc/bits/messages_members.h:38-92 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/mips-linux-gnu/uclibc/el/bits/messages_members.h:38-92
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/map.h:265-269 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/set.h:240-244
[token-LCS=1] MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_map_to_unordered_map.h:206-214 <-> MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_vector_to_list.h:199-208
```
- Циклы включений (claim):
```
MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/parallel/balanced_quicksort.h → MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/parallel/partition.h → MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/parallel/sort.h → MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/parallel/balanced_quicksort.h
MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler.h → MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler_hash_func.h → MIPSCore/Compiler/Compiler/mips-linux-gnu/include/c++/5.3.0/profile/impl/profiler.h
```

## Deivs117_IoT_CNC_Monitoring
- HEAD: `e148a11` (2026-05-29), subpath: `.`
- Траектория (claim): dup 0.0%→0.0% (Δ+0.0, line-#053), nodes 0→2, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## djeada_Lightpad
- HEAD: `ede0e4f` (2026-05-31), subpath: `.`
- Траектория (claim): dup 0.0%→11.3% (Δ+11.3, line-#053), nodes 0→418, макс циклов=1, макс SCC=2
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] tests/unit/test_document.cpp:27-35 <-> tests/unit/test_filemanager.cpp:28-36
[token-LCS=1] App/format_templates/formattemplatemanager.cpp:457-464 <-> App/run_templates/runtemplatemanager.cpp:546-553
[token-LCS=1] App/ui/dialogs/commandpalette.cpp:19-42 <-> App/ui/dialogs/gotosymboldialog.cpp:14-37
[token-LCS=1] App/format_templates/formattemplatemanager.cpp:93-145 <-> App/run_templates/runtemplatemanager.cpp:119-171
[token-LCS=1] App/format_templates/formattemplatemanager.cpp:35-89 <-> App/run_templates/runtemplatemanager.cpp:62-115
[token-LCS=1] App/format_templates/formattemplatemanager.cpp:20-31 <-> App/run_templates/runtemplatemanager.cpp:47-58
[token-LCS=1] App/ui/dialogs/commandpalette.cpp:19-42 <-> App/ui/dialogs/recentfilesdialog.cpp:21-44
[token-LCS=1] App/ui/dialogs/commandpalette.cpp:86-95 <-> App/ui/dialogs/filequickopen.cpp:92-101
```

## djeada_Standard-of-Iron
- HEAD: `7a0588bb` (2026-05-30), subpath: `.`
- Траектория (claim): dup 0.0%→16.1% (Δ+16.1, line-#053), nodes 0→1273, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] render/equipment/horse_equipment_archetype.cpp:117-160 <-> render/equipment/humanoid_equipment_archetype.cpp:117-160
[token-LCS=1] render/equipment/horse_equipment_archetype.cpp:71-77 <-> render/equipment/humanoid_equipment_archetype.cpp:71-77
[token-LCS=1] tests/render/render_thread_boundary_test.cpp:19-27 <-> tests/render/shader_source_test.cpp:20-28
[token-LCS=1] render/entity/spearman_renderer_common.cpp:518-531 <-> render/entity/swordsman_renderer_common.cpp:185-198
[token-LCS=1] render/entity/spearman_renderer_common.cpp:53-56 <-> render/entity/swordsman_renderer_common.cpp:39-42
[token-LCS=1] render/entity/nations/roman/builder_renderer.cpp:824-827 <-> render/entity/swordsman_renderer_common.cpp:39-42
[token-LCS=1] render/entity/nations/roman/builder_renderer.cpp:824-827 <-> render/entity/spearman_renderer_common.cpp:53-56
[token-LCS=1] render/entity/nations/roman/ballista_renderer.cpp:59-79 <-> render/entity/nations/roman/catapult_renderer.cpp:59-79
```

## elijahjfrierson-prog_humhouse-drum-vst
- HEAD: `20db90c` (2026-05-23), subpath: `.`
- Траектория (claim): dup 0.0%→10.1% (Δ+10.1, line-#053), nodes 0→42, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] humhouse-vocals/Source/VocalDelay.h:35-35 <-> humhouse-vocals/Source/VocalEQ.h:31-33
[token-LCS=1] Source/ArrangementStrip.h:457-460 <-> Source/PatternStrip.h:45-47
[token-LCS=0.903226] humhouse-vocals/Source/LoFiFilter.h:98-101 <-> humhouse-vocals/Source/VocalReverb.h:37-42
[token-LCS=0.888889] Source/ArrangementStrip.h:706-708 <-> Source/GothicLookAndFeel.h:329-331
[token-LCS=0.886792] humhouse-vocals/Source/DeEsser.h:17-23 <-> humhouse-vocals/Source/LoFiFilter.h:18-25
[token-LCS=0.830508] humhouse-vocals/Source/OutputLimiter.h:17-26 <-> humhouse-vocals/Source/VocalDoubler.h:19-26
```

## enlorik_pearl-proof
- HEAD: `7ff32aa` (2026-05-29), subpath: `.`
- Траектория (claim): dup 0.0%→2.3% (Δ+2.3, line-#053), nodes 0→5, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## EricBorges2019_software_rendering
- HEAD: `d49a5e1` (2026-03-10), subpath: `src`
- Траектория (claim): dup 0.0%→0.0% (Δ+0.0, line-#053), nodes 0→9, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## esrrhs_fakelua
- HEAD: `8c2fe34` (2026-05-31), subpath: `src`
- Траектория (claim): dup 0.0%→10.3% (Δ+10.3, line-#053), nodes 0→62, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.967742] compile/bison/parser.cpp:268-308 <-> compile/bison/parser.h:2301-2339
[token-LCS=0.954907] compile/bison/parser.cpp:221-263 <-> compile/bison/parser.h:2301-2339
[token-LCS=0.953608] compile/bison/parser.cpp:268-308 <-> compile/bison/parser.h:2363-2403
[token-LCS=0.941476] compile/bison/parser.cpp:221-263 <-> compile/bison/parser.h:2363-2403
[token-LCS=0.938462] compile/bison/parser.cpp:314-355 <-> compile/bison/parser.h:2363-2403
[token-LCS=0.930481] compile/bison/parser.cpp:314-355 <-> compile/bison/parser.h:2301-2339
[token-LCS=0.924242] compile/bison/parser.cpp:360-403 <-> compile/bison/parser.h:2363-2403
[token-LCS=0.920635] compile/bison/parser.cpp:314-355 <-> compile/bison/parser.h:700-738
```

## eunhhu_RexPlayer
- HEAD: `ee729e9` (2026-04-24), subpath: `src`
- Траектория (claim): dup 9.4%→8.3% (Δ-1.2, line-#053), nodes 38→82, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] vmm/boot_arm64.cpp:41-50 <-> vmm/boot_x86.cpp:31-40
```

## FranciscoLi2000_CPP_Modules
- HEAD: `5a1c12f` (2026-05-29), subpath: `.`
- Траектория (claim): dup 0.0%→44.1% (Δ+44.1, line-#053), nodes 0→191, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] CPP_Module_02/ex00/Fixed.cpp:45-55 <-> CPP_Module_02/ex01/Fixed.cpp:44-50
[token-LCS=1] CPP_Module_05/ex02/Form.cpp:92-96 <-> CPP_Module_05/ex03/Form.cpp:92-96
[token-LCS=1] CPP_Module_05/ex02/Form.cpp:13-18 <-> CPP_Module_05/ex03/Form.cpp:13-18
[token-LCS=1] CPP_Module_05/ex02/Bureaucrat.cpp:12-17 <-> CPP_Module_05/ex03/Bureaucrat.cpp:12-17
[token-LCS=1] CPP_Module_05/ex01/Form.cpp:81-85 <-> CPP_Module_05/ex03/Form.cpp:92-96
[token-LCS=1] CPP_Module_05/ex01/Form.cpp:81-85 <-> CPP_Module_05/ex02/Form.cpp:92-96
[token-LCS=1] CPP_Module_05/ex01/Form.cpp:13-18 <-> CPP_Module_05/ex03/Form.cpp:13-18
[token-LCS=1] CPP_Module_05/ex01/Form.cpp:13-18 <-> CPP_Module_05/ex02/Form.cpp:13-18
```

## GizzZmo_Infiniti
- HEAD: `fc8e014` (2026-05-31), subpath: `src`
- Траектория (claim): dup 0.0%→3.1% (Δ+3.1, line-#053), nodes 0→15, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## GizzZmo_Security-Sentinel
- HEAD: `65ef58a` (2026-05-23), subpath: `src`
- Траектория (claim): dup 0.0%→2.0% (Δ+2.0, line-#053), nodes 0→13, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.886792] NetworkMonitor.cpp:210-228 <-> SecurityMonitor.cpp:242-263
[token-LCS=0.825] JsonReporting.cpp:11-18 <-> ViewManager.cpp:51-56
[token-LCS=0.807453] Dashboard.cpp:47-56 <-> ThreatProtection.cpp:61-72
```

## GregorGullwi_FlashCpp
- HEAD: `3fab4690` (2026-05-28), subpath: `src`
- Траектория (claim): dup 0.0%→19.2% (Δ+19.2, line-#053), nodes 5→184, макс циклов=4, макс SCC=8
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] Parser_Templates_Inst_ClassTemplate.cpp:9247-9260 <-> Parser_Templates_Params.cpp:2853-2866
[token-LCS=1] Parser_Templates_Inst_ClassTemplate.cpp:14526-14530 <-> Parser_Templates_Lazy.cpp:1132-1136
[token-LCS=1] ExpressionSubstitutor.cpp:123-133 <-> Parser_Expr_PrimaryExpr.cpp:825-835
```
- Циклы включений (claim):
```
AstNodeTypes.h → AstNodeTypes_Template.h → TemplateEnvironment.h → TemplateRegistry_Types.h → AstNodeTypes.h
AstNodeTypes_Core.h → Lexer.h → StringLiteralTokenUtils.h → AstNodeTypes_TypeSystem.h → AstNodeTypes_Core.h
AstToIr.h → IrGenerator.h → AstToIr.h
TemplateRegistry.h → TemplateRegistry_Registry.h → TemplateRegistry_Lazy.h → TemplateRegistry.h
```

## hgopalan_massconsistent_amr
- HEAD: `8b9ed7e` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→22.6% (Δ+22.6, line-#053), nodes 0→4, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.98263] wind_solver.cpp:875-897 <-> wind_solver_api.cpp:479-496
[token-LCS=0.979757] wind_solver.cpp:199-223 <-> wind_solver_api.cpp:197-221
[token-LCS=0.97852] wind_solver.cpp:166-187 <-> wind_solver_api.cpp:131-153
[token-LCS=0.969838] wind_solver.cpp:900-925 <-> wind_solver_api.cpp:498-517
[token-LCS=0.94854] wind_solver.cpp:1689-1713 <-> wind_solver_api.cpp:771-795
[token-LCS=0.910284] wind_solver.cpp:199-223 <-> wind_solver_api.cpp:131-153
[token-LCS=0.908629] wind_solver.cpp:273-299 <-> wind_solver_api.cpp:162-188
```

## hgopalan_wildfire_levelset
- HEAD: `2926c43` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→8.5% (Δ+8.5, line-#053), nodes 0→5, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## huberp_phu-fair-kid-67
- HEAD: `0fad74f` (2026-05-12), subpath: `src`
- Траектория (claim): dup 0.0%→3.5% (Δ+3.5, line-#053), nodes 0→17, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## ignaciosgithub_GameEngineDarkest
- HEAD: `6fe798c` (2025-11-04), subpath: `src`
- Траектория (claim): dup 0.0%→9.9% (Δ+9.9, line-#053), nodes 0→188, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] Networking/Socket.cpp:125-139 <-> Networking/TCPSocket.cpp:162-176
[token-LCS=1] Networking/Socket.cpp:125-139 <-> Networking/TCPSocket.cpp:179-193
[token-LCS=1] Networking/TCPSocket.cpp:26-43 <-> Networking/UDPSocket.cpp:13-30
[token-LCS=1] Rendering/Pipelines/DeferredRenderPipeline.cpp:572-578 <-> Rendering/Pipelines/ForwardRenderPipeline.cpp:588-594
[token-LCS=1] Rendering/Pipelines/ForwardRenderPipeline.cpp:439-452 <-> Rendering/Pipelines/RaytracingPipeline.cpp:131-144
[token-LCS=0.979592] Rendering/Pipelines/DeferredRenderPipeline.cpp:614-641 <-> Rendering/Pipelines/ForwardRenderPipeline.cpp:949-974
[token-LCS=0.979167] Networking/TCPSocket.cpp:162-176 <-> Networking/UDPSocket.cpp:119-134
[token-LCS=0.979167] Networking/Socket.cpp:125-139 <-> Networking/UDPSocket.cpp:119-134
```

## ImagingTools_Acf
- HEAD: `e17192b91` (2026-05-25), subpath: `.`
- Траектория (claim): dup 0.0%→14.0% (Δ+14.0, line-#053), nodes 9→1486, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] Include/ifile/TDeviceBasedSerializerComp.h:107-113 <-> Include/ifile/TFileSerializerComp.h:100-106
[token-LCS=1] Include/iwidgets/CCollapsibleGroupWidgetDelegate.cpp:332-345 <-> Include/iwidgets/CMiniWidgetDelegate.cpp:280-293
[token-LCS=1] Include/iwidgets/CCollapsiblePage.cpp:48-55 <-> Include/iwidgets/CMiniWidgetPage.cpp:42-49
[token-LCS=1] Include/ilog/CLogCompBase.cpp:19-27 <-> Include/iqtgui/CSplashScreenGuiComp.cpp:16-24
[token-LCS=1] Include/ifile/Test/CFileArchiveTest.cpp:106-137 <-> Include/ifile/Test/CFileSecureArchiveTest.cpp:108-139
[token-LCS=1] Include/ifile/Test/CCompactXmlFileArchiveTest.cpp:49-63 <-> Include/ifile/Test/CSimpleXmlFileArchiveTest.cpp:49-63
[token-LCS=1] Include/ifile/Test/CCompactXmlDeviceArchiveTest.cpp:76-101 <-> Include/ifile/Test/CJsonDeviceArchiveTest.cpp:76-101
[token-LCS=1] Include/ifile/TFileSerializerComp.h:180-185 <-> Include/ifile/TXmlFileSerializerComp.h:204-209
```

## ImagingTools_AcfSln
- HEAD: `28335c8c` (2026-05-29), subpath: `.`
- Траектория (claim): dup 18.5%→14.3% (Δ-4.2, line-#053), nodes 66→848, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] Include/iedge/CEdgeBasedFeaturesSupplierComp.cpp:386-395 <-> Include/iipr/CProcessedBitmapSupplierComp.cpp:151-160
[token-LCS=1] Include/iedgegui/CEdgeLinesSupplierEditorComp.cpp:75-80 <-> Include/iqtcam/CMultiBitmapSupplierGuiComp.cpp:189-195
[token-LCS=1] Include/iprocgui/CProcessorControlGuiComp.cpp:23-25 <-> Include/iqtsig/CSampleAcquisitionGuiComp.cpp:20-22
```

## jimhansson_vibe-requirements
- HEAD: `4b1b010` (2026-05-27), subpath: `src`
- Траектория (claim): dup 0.0%→13.1% (Δ+13.1, line-#053), nodes 0→44, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] tests/test_main.cpp:81-105 <-> tests/test_validate.cpp:37-61
[token-LCS=1] tests/test_coverage.cpp:52-59 <-> tests/test_triplet_store.cpp:20-27
[token-LCS=1] tests/test_coverage.cpp:68-95 <-> tests/test_main.cpp:49-73
[token-LCS=1] tests/test_coverage.cpp:68-95 <-> tests/test_main.cpp:81-105
[token-LCS=1] tests/test_coverage.cpp:68-95 <-> tests/test_status.cpp:34-57
[token-LCS=1] tests/test_coverage.cpp:68-95 <-> tests/test_validate.cpp:37-61
[token-LCS=1] tests/test_coverage.cpp:68-95 <-> tests/test_validate.cpp:65-89
[token-LCS=1] tests/test_main.cpp:81-105 <-> tests/test_validate.cpp:65-89
```

## jlaustill_ovgt
- HEAD: `47bb565` (2026-05-28), subpath: `src`
- Траектория (claim): dup 0.0%→8.4% (Δ+8.4, line-#053), nodes 0→22, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] sensors/citSensor.cpp:13-19 <-> sensors/cotSensor.cpp:13-19
[token-LCS=1] sensors/citSensor.cpp:13-19 <-> sensors/titSensor.cpp:12-18
[token-LCS=1] sensors/citSensor.cpp:13-19 <-> sensors/totSensor.cpp:13-19
[token-LCS=1] sensors/citSensor.cpp:22-30 <-> sensors/cotSensor.cpp:22-30
[token-LCS=1] sensors/citSensor.cpp:22-30 <-> sensors/titSensor.cpp:21-29
[token-LCS=1] sensors/citSensor.cpp:22-30 <-> sensors/totSensor.cpp:22-30
[token-LCS=1] sensors/cotSensor.cpp:13-19 <-> sensors/titSensor.cpp:12-18
[token-LCS=1] sensors/cotSensor.cpp:13-19 <-> sensors/totSensor.cpp:13-19
```

## JuKimpfler_BotConectingWifiSystem
- HEAD: `80b4d14` (2026-05-22), subpath: `.`
- Траектория (claim): dup 0.0%→13.2% (Δ+13.2, line-#053), nodes 0→32, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] ESP_Hub/src/EspNowManager.cpp:89-95 <-> ESP_Satellite/src/EspNowBridge.cpp:83-89
[token-LCS=1] ESP_Hub/src/EspNowManager.cpp:126-132 <-> ESP_Satellite/src/EspNowBridge.cpp:120-126
[token-LCS=1] ESP_Hub/src/EspNowManager.cpp:239-252 <-> ESP_Satellite/src/EspNowBridge.cpp:229-242
[token-LCS=0.970085] ESP_Hub/src/EspNowManager.cpp:48-86 <-> ESP_Satellite/src/EspNowBridge.cpp:40-80
[token-LCS=0.845238] ESP_Satellite/src/main.cpp:398-408 <-> Teensy_lib/src/BotConnect.cpp:243-255
[token-LCS=0.809668] ESP_Hub/src/EspNowManager.cpp:146-177 <-> ESP_Satellite/src/EspNowBridge.cpp:140-171
```

## junpark12_retro-shooter-agents
- HEAD: `678d19d` (2026-05-25), subpath: `.`
- Траектория (claim): dup 0.0%→2.2% (Δ+2.2, line-#053), nodes 1→35, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.878788] game/src/game.cpp:35-42 <-> game/src/sprites.cpp:204-210
[token-LCS=0.801471] game/src/hud.cpp:228-236 <-> game/src/menu.cpp:110-121
[token-LCS=0.801471] game/src/hud.cpp:314-322 <-> game/src/menu.cpp:110-121
```

## Kadino_voice2vocalsynth
- HEAD: `9a158fa` (2026-05-14), subpath: `src`
- Траектория (claim): dup 0.0%→5.1% (Δ+5.1, line-#053), nodes 0→16, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] VoicebankAliasIndex.cpp:63-65 <-> VoicebankScanner.cpp:75-77
[token-LCS=1] AppSettings.cpp:55-79 <-> DebugTimeline.cpp:13-37
[token-LCS=1] AppSettings.cpp:137-150 <-> LatencyBudget.cpp:140-154
[token-LCS=1] VoicebankAliasIndex.cpp:19-26 <-> VoicebankScanner.cpp:64-71
[token-LCS=1] PhonemeFallbackMapper.cpp:31-34 <-> VoicebankScanner.cpp:118-121
[token-LCS=1] PhonemeFallbackMapper.cpp:23-27 <-> VoicebankScanner.cpp:21-25
[token-LCS=0.98] PhonemeFallbackMapper.cpp:15-19 <-> VoicebankScanner.cpp:21-25
[token-LCS=0.954128] AppSettings.cpp:20-27 <-> VoicebankScanner.cpp:64-71
```

## kahlertfr_trajectory-spatialhash
- HEAD: `b8998f6` (2026-05-27), subpath: `.`
- Траектория (claim): dup 0.0%→17.5% (Δ+17.5, line-#053), nodes 0→27, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] Source/SpatialHashedTrajectory/Private/SampleTrajectoryNiagaraActor.cpp:61-73 <-> Source/SpatialHashedTrajectory/Private/TrajectoryQueryNiagaraActor.cpp:25-37
[token-LCS=0.916667] Source/SpatialHashedTrajectory/Private/RunTrajectoryQueryAsyncAction.cpp:11-16 <-> Source/SpatialHashedTrajectory/Private/SpatialHashTableBuilderAsyncTask.cpp:11-20
[token-LCS=0.822134] Source/SpatialHashedTrajectory/Private/SampleTrajectoryNiagaraActor.cpp:42-57 <-> Source/SpatialHashedTrajectory/Private/SpatialHashTableManager.cpp:131-150
[token-LCS=0.8125] Source/SpatialHashedTrajectory/Private/SpatialHashQueryAsyncTasks.cpp:19-28 <-> Source/SpatialHashedTrajectory/Private/SpatialHashTableBuilderAsyncTask.cpp:11-20
```

## kahlertfr_ue-plugin-trajectory-data
- HEAD: `33796ce` (2026-05-27), subpath: `.`
- Траектория (claim): dup 0.0%→12.4% (Δ+12.4, line-#053), nodes 0→26, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## KRcpl88_WinAmy4D
- HEAD: `038878a` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→19.3% (Δ+19.3, line-#053), nodes 0→45, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## Kutabarik_olymp-sandbox
- HEAD: `2194694` (2026-05-27), subpath: `src`
- Траектория (claim): dup 0.0%→0.0% (Δ+0.0, line-#053), nodes 0→13, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## lostdogg_CAM-Expert1.0
- HEAD: `73ac717` (2026-05-28), subpath: `src`
- Траектория (claim): dup 0.0%→5.3% (Δ+5.3, line-#053), nodes 0→115, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] copilot/CopilotEngine.cpp:65-83 <-> copilot/GeometricTokenizer.cpp:37-55
[token-LCS=1] cad/FileImporter.cpp:12-15 <-> cam/MaterialLibrary.cpp:255-258
[token-LCS=1] cad/FileImporter.cpp:12-15 <-> cam/PostProcessor.cpp:25-28
[token-LCS=1] cad/FileImporter.cpp:12-15 <-> cam/SqlToolDatabase.cpp:8-11
[token-LCS=1] ui/CopilotPanel.cpp:293-308 <-> ui/SelectionBar.cpp:138-150
[token-LCS=1] cam/MaterialLibrary.cpp:255-258 <-> cam/PostProcessor.cpp:25-28
[token-LCS=1] cam/MaterialLibrary.cpp:255-258 <-> cam/SqlToolDatabase.cpp:8-11
[token-LCS=1] ui/CopilotPanel.cpp:293-308 <-> ui/Viewport3D.cpp:1176-1187
```

## LuuuXXX_cpp2rust-demo
- HEAD: `b90f6ad` (2026-05-31), subpath: `.`
- Траектория (claim): dup 0.0%→16.3% (Δ+16.3, line-#053), nodes 0→377, макс циклов=1, макс SCC=2
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] references/rapidjson-refactoring/rapidjson_legacy/thirdparty/gtest/googletest/samples/sample7_unittest.cc:99-105 <-> references/rapidjson-refactoring/rapidjson_legacy/thirdparty/gtest/googletest/samples/sample8_unittest.cc:140-146
[token-LCS=1] references/rapidjson-refactoring/rapidjson_legacy/example/filterkey/filterkey.cpp:61-71 <-> references/rapidjson-refactoring/rapidjson_legacy/example/filterkeydom/filterkeydom.cpp:62-72
[token-LCS=1] references/rapidjson-refactoring/rapidjson_legacy/test/perftest/schematest.cpp:21-48 <-> references/rapidjson-refactoring/rapidjson_legacy/test/unittest/schematest.cpp:2021-2048
[token-LCS=1] references/rapidjson-refactoring/rapidjson_legacy/test/unittest/prettywritertest.cpp:263-279 <-> references/rapidjson-refactoring/rapidjson_legacy/test/unittest/writertest.cpp:485-501
```
- Циклы включений (claim):
```
references/rapidjson-refactoring/rapidjson_legacy/thirdparty/gtest/googletest/include/gtest/gtest.h → references/rapidjson-refactoring/rapidjson_legacy/thirdparty/gtest/googletest/include/gtest/gtest_pred_impl.h → references/rapidjson-refactoring/rapidjson_legacy/thirdparty/gtest/googletest/include/gtest/gtest.h
```

## makr-code_ThemisDB
- HEAD: `8dbcebff06` (2026-05-31), subpath: `src`
- Траектория (claim): dup 21.5%→14.3% (Δ-7.1, line-#053), nodes 22→1461, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] importers/kafka_importer.cpp:275-280 <-> importers/mysql_importer.cpp:257-262
[token-LCS=1] importers/flatfile_importer.cpp:238-245 <-> importers/sqlite_importer.cpp:96-103
[token-LCS=1] importers/flatfile_importer.cpp:350-355 <-> importers/kafka_importer.cpp:275-280
[token-LCS=1] importers/flatfile_importer.cpp:350-355 <-> importers/mongo_importer.cpp:241-246
[token-LCS=1] importers/flatfile_importer.cpp:350-355 <-> importers/mysql_importer.cpp:257-262
[token-LCS=1] importers/flatfile_importer.cpp:350-355 <-> importers/oracle_importer.cpp:235-240
[token-LCS=1] importers/flatfile_importer.cpp:350-355 <-> importers/postgres_importer.cpp:373-378
[token-LCS=1] importers/flatfile_importer.cpp:350-355 <-> importers/s3_importer.cpp:330-335
```

## martinberlin_FastJsonDL
- HEAD: `a1ee998` (2026-05-31), subpath: `src`
- Траектория (claim): dup 0.0%→7.5% (Δ+7.5, line-#053), nodes 0→1, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## melkyr_znineeight
- HEAD: `589a9272` (2026-04-27), subpath: `src`
- Траектория (claim): dup 0.0%→14.6% (Δ+14.6, line-#053), nodes 0→99, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] bootstrap/double_free_analyzer.cpp:138-146 <-> bootstrap/null_pointer_analyzer.cpp:93-101
[token-LCS=1] bootstrap/ast_lifter.cpp:1527-1537 <-> include/ast_utils.hpp:80-90
```

## Mikester9000_Creation-Engine
- HEAD: `1046cbf` (2026-05-28), subpath: `src`
- Траектория (claim): dup 0.0%→2.2% (Δ+2.2, line-#053), nodes 0→21, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## Mikester9000_GameRewritten
- HEAD: `0bd8cb3` (2026-05-25), subpath: `src`
- Траектория (claim): dup 0.0%→4.8% (Δ+4.8, line-#053), nodes 0→124, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.971014] assets/AssetLoader.cpp:27-44 <-> world/WorldGrid.cpp:28-44
[token-LCS=0.852713] game/Forest.cpp:123-132 <-> game/PrimitiveRenderer.cpp:185-197
```

## NamecoinGithub_LLL-TAO
- HEAD: `b62a2f180` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→16.2% (Δ+16.2, line-#053), nodes 0→730, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] LLP/include/legacy_lane_handler.h:117-129 <-> LLP/include/stateless_lane_handler.h:157-169
[token-LCS=1] Legacy/legacy.cpp:804-811 <-> TAO/Ledger/state.cpp:1716-1723
[token-LCS=1] Legacy/legacy.cpp:804-811 <-> TAO/Ledger/client.cpp:758-765
[token-LCS=1] LLP/miner.cpp:258-285 <-> LLP/stateless_miner_connection.cpp:554-581
[token-LCS=1] LLP/miner.cpp:360-370 <-> LLP/stateless_miner_connection.cpp:659-669
[token-LCS=1] LLP/miner.cpp:403-447 <-> LLP/stateless_miner_connection.cpp:708-753
[token-LCS=1] LLP/miner.cpp:856-902 <-> LLP/stateless_miner_connection.cpp:1234-1280
[token-LCS=1] LLD/ledger.cpp:1030-1046 <-> LLD/register.cpp:501-516
```

## NamecoinGithub_NexusMiner
- HEAD: `d95c17e4` (2026-05-27), subpath: `src`
- Траектория (claim): dup 13.9%→13.9% (Δ+0.1, line-#053), nodes 111→285, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] cpu/src/cpu/worker_hash.cpp:268-302 <-> cpu/src/cpu/worker_prime.cpp:377-411
[token-LCS=1] cpu/src/cpu/worker_hash.cpp:365-370 <-> cpu/src/cpu/worker_prime.cpp:414-419
[token-LCS=1] cpu/src/cpu/worker_prime.cpp:677-687 <-> gpu/src/gpu/worker_prime.cpp:413-423
[token-LCS=1] cpu/src/cpu/worker_prime.cpp:703-709 <-> gpu/src/gpu/worker_prime.cpp:438-444
[token-LCS=1] cpu/src/cpu/worker_prime.cpp:714-726 <-> gpu/src/gpu/worker_prime.cpp:448-460
[token-LCS=1] cpu/worker_hash_shutdown_test.cpp:54-59 <-> gpu/test/worker_hash_integration_test.cpp:148-153
[token-LCS=1] cpu/worker_hash_shutdown_test.cpp:54-59 <-> gpu/test/worker_prime_host_stubs_test.cpp:38-43
[token-LCS=1] gpu/src/gpu/prime/prime_tests.cpp:293-304 <-> gpu/src/gpu/prime/sieve.cpp:871-882
```

## RegimA-Zone_RRR-P-Systems
- HEAD: `31950c8` (2026-04-02), subpath: `src`
- Траектория (claim): dup 0.0%→9.4% (Δ+9.4, line-#053), nodes 0→11, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## remiserriere_ESP32-S3-Camera-EnvSensors
- HEAD: `8f9af6a` (2026-05-24), subpath: `src`
- Траектория (claim): dup 0.0%→6.7% (Δ+6.7, line-#053), nodes 0→29, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## RT-Ecosystems_mantra-lang
- HEAD: `fee1b8d` (2026-05-31), subpath: `src`
- Траектория (claim): dup 0.0%→11.7% (Δ+11.7, line-#053), nodes 0→7, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## saud552_NHAY7
- HEAD: `4434401` (2025-07-21), subpath: `.`
- Траектория (claim): dup 0.0%→34.6% (Δ+34.6, line-#053), nodes 0→8, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## shelbeely_Epaper-Journal
- HEAD: `2e7788b` (2026-05-21), subpath: `src`
- Траектория (claim): dup 0.0%→6.3% (Δ+6.3, line-#053), nodes 0→52, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] journal/JournalExport.cpp:50-58 <-> web/WebApi.cpp:222-230
```

## shridharbhandiwad_DDSEnabledRadar
- HEAD: `add5da9` (2025-07-20), subpath: `.`
- Траектория (claim): dup 0.0%→15.2% (Δ+15.2, line-#053), nodes 0→47, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] apps/radar_signal_processor/detection_processor.cpp:34-56 <-> apps/radar_signal_processor/signal_processor.cpp:33-55
[token-LCS=1] apps/radar_data_processor/main.cpp:455-500 <-> apps/radar_signal_processor/main.cpp:301-346
[token-LCS=1] apps/radar_data_processor/data_processor.cpp:246-250 <-> apps/radar_signal_processor/detection_processor.cpp:205-209
[token-LCS=1] shared/processing/association/src/gnn.cpp:56-77 <-> shared/processing/association/src/nearest_neighbor.cpp:56-77
[token-LCS=1] shared/processing/association/src/gnn.cpp:206-218 <-> shared/processing/association/src/nearest_neighbor.cpp:328-340
[token-LCS=0.991266] apps/radar_data_processor/main.cpp:47-88 <-> apps/radar_signal_processor/main.cpp:43-84
[token-LCS=0.985294] apps/radar_data_processor/data_processor.cpp:38-63 <-> apps/radar_data_processor/tracking_manager.cpp:41-66
[token-LCS=0.98] apps/radar_data_processor/main.cpp:152-177 <-> apps/radar_signal_processor/main.cpp:140-162
```

## stevenhao_perfectclear
- HEAD: `dcbb156` (2025-06-05), subpath: `.`
- Траектория (claim): dup 0.0%→17.0% (Δ+17.0, line-#053), nodes 0→11, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## studiocollective_songbird
- HEAD: `8f9ed331` (2026-05-31), subpath: `.`
- Траектория (claim): dup 15.3%→0.0% (Δ-15.3, line-#053), nodes 144→11, макс циклов=4, макс SCC=7
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## supaplextor_aqemu
- HEAD: `439043b` (2026-05-29), subpath: `src`
- Траектория (claim): dup 0.0%→8.6% (Δ+8.6, line-#053), nodes 0→110, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```
- Циклы включений (claim):
```
docopt/docopt.cpp → docopt/docopt.h → docopt/docopt.cpp
```

## swatso_GCodeDriver
- HEAD: `9f18b42` (2026-05-30), subpath: `src`
- Траектория (claim): dup 0.0%→0.0% (Δ+0.0, line-#053), nodes 0→1, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## Tsoympet_PantheonChain
- HEAD: `e8ba8e7` (2026-05-30), subpath: `.`
- Траектория (claim): dup 0.0%→7.9% (Δ+7.9, line-#053), nodes 0→311, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] layer1-talanton/core/storage/utxo_storage.cpp:31-43 <-> layer1-talanton/rpc/rpc_server.cpp:183-195
[token-LCS=1] layer1-talanton/core/crypto/hardware_crypto.cpp:78-85 <-> tools/mobile_sdks/mobile_sdk.cpp:503-510
[token-LCS=1] layer1-talanton/core/storage/utxo_storage.cpp:16-28 <-> layer1-talanton/rpc/rpc_server.cpp:183-195
[token-LCS=1] layer1-talanton/core/storage/block_storage.cpp:35-47 <-> layer1-talanton/core/storage/utxo_storage.cpp:76-88
[token-LCS=1] bridge/l1_l2/l1_l2_bridge.cpp:35-43 <-> bridge/l2_l3/l2_l3_bridge.cpp:34-42
[token-LCS=1] layer2-drachma/consensus/pos_consensus.cpp:43-56 <-> layer3-obolos/consensus/pos_consensus.cpp:42-55
[token-LCS=1] bridge/l1_l2/l1_l2_bridge.cpp:50-69 <-> bridge/l2_l3/l2_l3_bridge.cpp:49-68
[token-LCS=1] bridge/l1_l2/l1_l2_bridge.cpp:50-69 <-> tests/adversarial/bridge_test_helpers.h:54-73
```

## typoyceman-star_project_-_cs2
- HEAD: `35749ad` (2026-05-06), subpath: `.`
- Траектория (claim): dup 0.0%→8.8% (Δ+8.8, line-#053), nodes 0→358, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] OXRANALOUATABACLIENT/minhook_src/hde/hde32.c:218-234 <-> OXRANALOUATABACLIENT/minhook_src/hde/hde64.c:234-250
```

## umetsu_eyedrops-everyday
- HEAD: `2361ecb` (2025-06-26), subpath: `.`
- Траектория (claim): dup 0.0%→0.0% (Δ+0.0, line-#053), nodes 0→16, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

## winnerspiros_UnleashedRecomp_Android
- HEAD: `80e1f2e` (2026-03-26), subpath: `.`
- Траектория (claim): dup 3.0%→8.1% (Δ+5.1, line-#053), nodes 31→317, макс циклов=2, макс SCC=5
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] UnleashedRecomp/kernel/imports.cpp:1027-1047 <-> UnleashedRecomp/kernel/synchronization.cpp:349-369
[token-LCS=1] UnleashedRecomp/kernel/imports.cpp:648-660 <-> UnleashedRecomp/kernel/threading.cpp:54-66
[token-LCS=1] UnleashedRecomp/kernel/imports.cpp:675-681 <-> UnleashedRecomp/kernel/threading.cpp:75-81
[token-LCS=1] UnleashedRecomp/kernel/imports.cpp:693-701 <-> UnleashedRecomp/kernel/synchronization.cpp:255-263
[token-LCS=1] build_overrides/thirdparty/nativefiledialog-extended/src/nfd_gtk.cpp:576-581 <-> build_overrides/thirdparty/nativefiledialog-extended/src/nfd_win.cpp:435-440
[token-LCS=1] UnleashedRecomp/kernel/imports.cpp:705-722 <-> UnleashedRecomp/kernel/synchronization.cpp:267-284
[token-LCS=1] build_overrides/thirdparty/nativefiledialog-extended/src/nfd_gtk.cpp:576-581 <-> build_overrides/thirdparty/nativefiledialog-extended/src/nfd_portal.cpp:1482-1487
[token-LCS=1] UnleashedRecomp/kernel/imports.cpp:777-787 <-> UnleashedRecomp/kernel/synchronization.cpp:221-231
```
- Циклы включений (claim):
```
UnleashedRecomp/os/android/registry_android.inl → UnleashedRecomp/os/registry.h → UnleashedRecomp/os/android/registry_android.inl
UnleashedRecomp/sdl_events.h → UnleashedRecomp/ui/game_window.h → UnleashedRecomp/sdl_events.h
```

## xadupre_onnx-light
- HEAD: `8968b9d` (2026-05-31), subpath: `.`
- Траектория (claim): dup 0.0%→35.4% (Δ+35.4, line-#053), nodes 0→679, макс циклов=0, макс SCC=1
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=1] onnx_light/onnx_op/operator_sets_optional.cc:17-21 <-> onnx_light/onnx_op/operator_sets_sequence.cc:17-21
[token-LCS=1] onnx_light/onnx_lib/defs/generator/defs.cc:89-102 <-> onnx_light/onnx_lib/defs/generator/old.cc:570-581
[token-LCS=1] onnx_light/onnx_lib/defs/generator/defs.cc:89-102 <-> onnx_light/onnx_lib/defs/generator/old.cc:616-627
[token-LCS=1] onnx_light/onnx_lib/defs/generator/defs.cc:89-102 <-> onnx_light/onnx_lib/defs/generator/old.cc:660-671
[token-LCS=1] onnx_light/onnx_lib/defs/generator/defs.cc:89-102 <-> onnx_light/onnx_lib/defs/generator/old.cc:704-715
[token-LCS=1] onnx_light/onnx_lib/defs/generator/defs.cc:89-102 <-> onnx_light/onnx_lib/defs/generator/old.cc:746-757
[token-LCS=1] onnx_light/onnx_lib/defs/math/defs.cc:2108-2122 <-> onnx_light/onnx_lib/defs/math/old.cc:1905-1919
[token-LCS=1] onnx_light/onnx_lib/defs/math/defs.cc:2339-2356 <-> onnx_light/onnx_lib/defs/math/old.cc:3239-3256
```

## Xyrces_voxels
- HEAD: `c847edc` (2025-05-15), subpath: `src`
- Траектория (claim): dup 0.0%→9.6% (Δ+9.6, line-#053), nodes 0→7, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
[token-LCS=0.882353] Materials/LiquidMaterial.cpp:23-26 <-> Materials/PlasmaMaterial.cpp:23-26
```

## zyzzyva8421_spef_corr
- HEAD: `6d6d8a5` (2026-05-23), subpath: `src`
- Траектория (claim): dup 0.0%→9.4% (Δ+9.4, line-#053), nodes 0→3, макс циклов=0, макс SCC=0
- Cross-file дубли #056 token-precise (claim, проверь глазами):
```
```

