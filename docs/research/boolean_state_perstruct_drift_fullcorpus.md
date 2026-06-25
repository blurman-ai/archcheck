# boolean_state per-struct accretion — полный корпус (#134, из нативных событий #090)

**Дата:** 2026-06-25. **Метод:** агрегат struct-уровневых событий `DRIFT.BOOL_FIELD_ACCRETION` (#090) по 10735 bool-коммитам корпуса (не leaky-Python `perstruct_drift.py` — нативный фильтр vendored/test/generated). Метрика: структура, накопившая були в **≥4 разных коммитах** (in-window).

**Сводка:** 8280 (repo,struct) накопили хоть один буль; **499** прошли ≥4 коммитов (**335** content-drift + **163** config-bag + **1** churn) в **240** репах. Всего событий накопления: 13315. Дедуп по (repo,struct) — переезды файлов смёрджены.

> Метрика — НЕ #134-буква (`perstruct_drift.py` blame, MIN_FIELDS=4): тут считается число РАЗНЫХ коммитов, доливших були в структуру, в окне корпус-прогона; нативный фильтр строже Python-VEND. Greenfield (первое появление структуры) не считается — правило #090 фаярит только если структура была в родителе.

## Content-структуры — кандидаты на boolean-state drift (config/churn вынесены) (топ-40)

| # | repo | struct | файл | коммитов | полей | поля |
|---|---|---|---|---|---|---|
| 1 | Exoridus_exosnap | `MainWindow` | app/MainWindow.h; apps/exosnap/MainWindow.h | 7 | 14 | recording_active_, runtime_window_icon_bound_, resizable_style_appl... |
| 2 | djeada_Lightpad | `Terminal` | App/ui/panels/terminal.h | 6 | 14 | m_processRunning, m_autoRestartEnabled, m_linkDetectionEnabled, m_r... |
| 3 | Conceptual-Machines_magda-core | `TrackContentPanel` | daw/ui/components/tracks/TrackContentPanel.hpp; magda/daw/ui/components/tracks/TrackContentPanel.hpp | 11 | 13 | isCreatingSelection, isShiftHeld, isMovingSelection, isMarqueeActiv... |
| 4 | deltahdl_deltahdl | `RtlirVariable` | src/elaboration/rtlir.h; src/elaborator/rtlir.h | 11 | 13 | is_event, is_real, is_string, is_signed, is_descending, is_queue, i... |
| 5 | mne-tools_mne-cpp | `BrainView` | src/examples/ex_brain_view/brainview.h; src/examples/ex_brain_view/view/brainview.h | 6 | 13 | m_dipolesVisible, m_applySensorTrans, m_hasIntersection, m_isLoadin... |
| 6 | luspi_photoqt | `PQCExtensionInfo` | cplusplus/header/pqc_extensioninfo.h; cplusplus/header/pqc_extensionshandler.h | 5 | 13 | allowIntegrated, allowPopout, fixSizeToContent, letMeHandleMouseEve... |
| 7 | conradhuebler_curcuma | `GFNFF` | src/core/energy_calculators/ff_methods/gfnff.h | 10 | 12 | m_comparing_gradients, m_use_workspace, m_rep_diag, m_static_topolo... |
| 8 | etkecc_komai | `MatrixTimelineItem` | src/matrix/backend/MatrixBackendRuntimeService.h; src/matrix/backend/MatrixBackendRuntimeServiceTypes.h | 9 | 12 | mediaIsEncrypted, thumbnailIsEncrypted, isEdited, cachedIsStateEven... |
| 9 | danielraffel_pulp | `View` | core/view/include/pulp/view/view.hpp | 8 | 12 | needs_layer_, has_transform_matrix_, white_space_nowrap_, backface_... |
| 10 | LiveTrack-X_DirectPipe | `MainComponent` | host/Source/MainComponent.h | 6 | 12 | loadingSlot_, preMuteVCableEnabled_, preMuteMonitorEnabled_, cached... |
| 11 | drsnuggles8_OloEngineBase | `Sandbox3D` | Sandbox3D/src/Sandbox3D.h | 5 | 12 | m_AnimBlendRequested, m_ShowSingleBoneTest, m_ShowMultiBoneTest, m_... |
| 12 | drsnuggles8_OloEngineBase | `UniformBufferRegistry` | OloEngine/src/OloEngine/Renderer/UniformBufferRegistry.h | 5 | 12 | m_IsTemplate, m_IsClone, m_UseSetPriority, m_AutoAssignSets, m_Defa... |
| 13 | beiklive_GBAStation | `GameGridView` | src/ui/utils/RecyclingGrid.hpp | 4 | 12 | m_interactionDisabled, m_prevUp, m_prevDown, m_prevLeft, m_prevRigh... |
| 14 | Conceptual-Machines_magda-core | `MainView` | daw/ui/views/MainView.hpp; magda/daw/ui/views/MainView.hpp | 10 | 11 | isUpdatingTrackSelection, isUpdatingFromZoom, isDragging, isResizin... |
| 15 | community-shaders_skyrim-community-shaders | `Menu` | src/Menu.h | 10 | 11 | settingOverlayToggleKey, pendingFontChange, IsEnabled, pendingFontR... |
| 16 | b-macker_NAAb | `GovernanceEngine` | include/naab/governance.h | 10 | 11 | calibration_loaded_, baselines_dirty_, last_return_tainted_, baseli... |
| 17 | RyAnPr1Me_omscript | `AnalysisValidity` | include/opt_context.h | 9 | 11 | rangeAnalysis, rlc, dce, cse, copyProp, constFold, algSimp, borrowC... |
| 18 | FantasyNetworkCN_NekoMusicForPc | `PlayerPage` | src/ui/playerpage.h | 9 | 11 | m_titleIsPlaceholder, m_artistIsPlaceholder, m_videoRenderBusy, m_c... |
| 19 | djeada_Lightpad | `MainWindow` | App/ui/mainwindow.h | 7 | 11 | m_syncingTreeState, m_vimCommandPanelActive, m_treeScrollValueIniti... |
| 20 | Conceptual-Machines_magda-core | `NodeComponent` | magda/daw/ui/components/chain/NodeComponent.hpp | 7 | 11 | collapsed_, wasSelectedOnMouseDown_, selected_, mouseDownForSelecti... |
| 21 | ChrisLauinger77_QontrolPanel | `QuickSoundSwitcher` | QuickSoundSwitcher/QuickSoundSwitcher.h; include/quicksoundswitcher.h | 6 | 11 | hiding, disableNotification, disableOverlay, potatoMode, isMuted, h... |
| 22 | Attraccess_Attraccess | `Application` | apps/attractap-firmware/src/application/application.hpp; apps/attractap/firmware/src/application/application.hpp | 6 | 11 | selectedResourceChanged, resourceListUpdated, hasPendingFormRequest... |
| 23 | Conceptual-Machines_magda-core | `TimelineComponent` | daw/ui/components/TimelineComponent.hpp; daw/ui/components/timeline/TimelineComponent.hpp | 6 | 11 | isDraggingSection, isDraggingEdge, isDraggingStart, arrangementLock... |
| 24 | lyquid_Romulus | `GuiApp` | apps/gui/gui_app.hpp | 5 | 11 | sort_ascending_, scroll_checklist_top_, scroll_checklist_bottom_, g... |
| 25 | viperx1_Usagi-dono | `AniDBApi` | usagi/src/anidbapi.h | 5 | 11 | isExportQueued, watcherEnabled, watcherAutoStart, autoFetchEnabled,... |
| 26 | viperx1_Usagi-dono | `status_codes_` | usagi/src/anidbapi.h | 5 | 11 | isExportQueued, watcherEnabled, watcherAutoStart, autoFetchEnabled,... |
| 27 | beiklive_GBAStation | `FileListView` | src/ui/utils/FileListView.hpp | 4 | 11 | m_interactionDisabled, m_prevLeft, m_prevRight, m_prevA, m_filterAc... |
| 28 | godot42x_ya | `App` | Engine/Source/Core/App.h; Engine/Source/Core/App/App.h | 10 | 10 | useModel, bRunning, _bPause, _bMinimized, bBasicPostProcessor, bHas... |
| 29 | CTalkobt_M65Compiler | `FunctionDeclaration` | include/AST.hpp | 9 | 10 | isSigned, isNoreturn, isPrototype, isStatic, isVariadic, isFastcall... |
| 30 | derekbsnider_madc | `Program` | include/madc.h | 9 | 10 | _include_iostream, _include_stdio, parsing_extern_decl, jit_source_... |
| 31 | LiveTrack-X_DirectPipe | `AppState` | host/Source/Control/StateBroadcaster.h | 8 | 10 | outputMuted, monitorEnabled, recording, ipcEnabled, deviceLost, mon... |
| 32 | djeada_Standard-of-Iron | `GameEngine` | app/core/game_engine.h; app/game_engine.h | 8 | 10 | m_initialized, m_paused, m_followSelectionEnabled, m_mapsLoading, m... |
| 33 | fernandotonon_QtMeshEditor | `TexturePaintController` | src/TexturePaintController.h | 8 | 10 | m_strokeJustBegan, m_smudgeHavePrev, m_previewRefreshScheduled, m_u... |
| 34 | viperx1_Usagi-dono | `Window` | usagi/src/window.h | 8 | 10 | isCheckingNotifications, mylistSortAscending, mylistUseCardView, an... |
| 35 | mpc-qt_mpc-qt | `MainWindow` | mainwindow.h; src/mainwindow.h | 8 | 10 | osdTimerOnSeek, timeShortMode, mainwindowIsClosing, isVideo_, title... |
| 36 | GregorGullwi_FlashCpp | `FunctionDeclarationNode` | src/AstNodeTypes.h; src/AstNodeTypes_DeclNodes.h | 8 | 10 | is_member_function_, has_template_body_, is_variadic_, is_constexpr... |
| 37 | danielraffel_pulp | `InspectorOverlay` | inspect/include/pulp/inspect/inspector_overlay.hpp | 8 | 10 | dragging_enabled_, pass_viewer_enabled_, tweaks_panel_visible_, eye... |
| 38 | etkecc_komai | `RoomlistModel` | src/timeline/RoomlistModel.h | 8 | 10 | startupMaterializationTrackingActive_, startupMaterializationWarnin... |
| 39 | MOLAorg_mola_lidar_odometry | `Visualization` | module/include/mola_lidar_odometry/LidarOdometry.h | 7 | 10 | camera_follows_vehicle, camera_rotates_with_vehicle, show_ground_gr... |
| 40 | brooksthemaker_ProtoHUD | `SystemHealth` | src/app_state.h | 7 | 10 | mpu9250_ok, cam_usb1_overlay, cam_usb2_overlay, wifi_ok, ssh_active... |

## Config/flag-bag (вынесены — легитимное накопление, не смелл) (топ-20)

| # | repo | struct | файл | коммитов | полей | поля |
|---|---|---|---|---|---|---|
| 1 | alexandrosk0_Smatchet | `UiDrawSession` | Source/Core/include/Ui/SmatchetUiSession.h; Source_Core/include/SmatchetUiSession.h | 37 | 99 | showBlameAnalysis, inFlightCommitStarted, cachedSortValid, pendingV... |
| 2 | mod-playerbots_mod-playerbots | `PlayerbotAIConfig` | src/PlayerbotAIConfig.h | 34 | 42 | disableDeathKnightLogin, randomBotFixedLevel, sayWhenCollectingItem... |
| 3 | Phobos-developers_Phobos | `ExtData` | src/Ext/Anim/Body.h; src/Ext/Building/Body.h | 34 | 40 | CanCloakDuringRearm, AE_HasTint, AE_ReflectDamage, Reflected, HasCa... |
| 4 | Porcupine-Factory_FirstPersonController | `FirstPersonControllerComponent` | Code/Source/Clients/FirstPersonControllerComponent.h | 32 | 49 | m_crouchPendJumps, m_crouchJumpPending, m_jumpWhileCrouched, m_crou... |
| 5 | Cockatrice_Cockatrice | `SettingsCache` | cockatrice/src/client/settings/cache_settings.h; cockatrice/src/settings/cache_settings.h | 32 | 41 | openDeckInNewTab, zoneViewPileView, showShortcuts, clickPlaysAllSel... |
| 6 | tkadauke_raytracer | `Renderer` | tools/rendercli/rendercli.cpp | 31 | 49 | m_threadsSet, m_queueSizeSet, m_timing, m_rasterShadowMaps, m_anima... |
| 7 | drsnuggles8_OloEngineBase | `EditorLayer` | OloEditor/src/EditorLayer.h | 31 | 43 | m_ShowAssetPackBuilder, m_Is3DMode, m_ShowAnimationPanel, m_ShowCom... |
| 8 | anakryiko_wprof | `env` | examples/c/wprof.c; src/env.h | 30 | 35 | cpu_counters, breakout_counters, pb_debug_interns, pb_disable_inter... |
| 9 | stereolabs_zed-ros2-wrapper | `ZedCamera` | zed_components/src/zed_camera/include/zed_camera_component.hpp | 29 | 59 | mCustomLabelsGood, mAsyncImageRetrieval, mResetPoseWithSvoLoop, mUs... |
| 10 | EVerest_EVerest | `Conf` | modules/DummyBankSessionTokenProvider/main/bank_session_token_providerImpl.hpp; modules/EV/EvManager/EvManager.hpp | 29 | 40 | hack_simplified_mode_limit_10A, rtscts, selftest_success, conn1_gpi... |
| 11 | luspi_photoqt | `PQCConstants` | cplusplus/header/pqc_constants.h | 24 | 57 | m_photoQtStartupDone, m_windowFullScreen, m_windowMaxAndNotWindowed... |
| 12 | stereolabs_zed-ros2-wrapper | `ZedCameraOne` | zed_components/src/zed_camera/include/zed_camera_one_component.hpp; zed_components/src/zed_camera_one/include/zed_camera_one_component.hpp | 22 | 59 | _debugVideoDepth, _debugSensors, _cameraFlip, _enableHDR, _svoRealt... |
| 13 | alexandrosk0_Smatchet | `TrackerConfig` | Source/Core/include/Config/ConfigManager.h; Source_Core/include/ConfigManager.h | 22 | 44 | McpAllowLuaExecution, ReadOnlyMode, ShowPreferencesWindow, ShowView... |
| 14 | HansKristian-Work_vkd3d-proton | `vkd3d_vulkan_info` | libs/vkd3d/vkd3d_private.h | 22 | 28 | EXT_depth_bias_control, KHR_compute_shader_derivatives, EXT_device_... |
| 15 | 41343112_Qt_11401_8 | `Qt_Chess` | qt_chess.h; src/qt_chess.h | 22 | 23 | m_pieceSelected, m_isDragging, m_wasSelectedBeforeDrag, m_isBoardFl... |
| 16 | markaren_threepp | `VulkanRenderer` | src/threepp/renderers/VulkanRenderer.cpp | 21 | 31 | envIsDefault, prevCameraValid, motionThisFrame_, cameraMovedThisFra... |
| 17 | community-shaders_skyrim-community-shaders | `Settings` | src/Features/GrassCollision.h; src/Features/LightLimitFix.h | 20 | 27 | VRMenuControllerDiagnosticsTestMode, ShowHowToUseMessage, EnableDra... |
| 18 | streetpea_chiaki-ng | `QmlMainWindow` | gui/include/qmlmainwindow.h | 20 | 23 | renderparams_changed, is_stream_window_adjustable, amd_card, is_win... |
| 19 | ErenAri_Aegis-BPF | `BpfState` | src/bpf_ops.hpp; src/main.cpp | 19 | 40 | inode_reused, cgroup_reused, block_stats_reused, deny_cgroup_stats_... |
| 20 | Barbarisch_phyxel | `Application` | editor/include/Application.h; include/Application.h | 19 | 37 | showPerformanceOverlay, projectionMatrixNeedsUpdate, hasHoveredCube... |

## Churn-suspect (commits ≫ 2×fields — поля переписывались, не накапливались) (топ-1)

| # | repo | struct | файл | коммитов | полей | поля |
|---|---|---|---|---|---|---|
| 1 | thewriterben_WildCAM_ESP32 | `SystemState` | firmware/main.cpp | 10 | 3 | ota_available, in_lockdown, power_save_mode |


## Eye-check топ-15 content (обязательно, CLAUDE.md «Самопроверка»)

Классификация по именам полей + проверка кода (вердикт по каждому):

| # | struct (repo) | вердикт | основание |
|---|---|---|---|
| 1 | MainWindow (exosnap) | **TP** | view-state: `recording_active_/win32_maximized_/resizable_style_applied_` |
| 2 | Terminal (Lightpad) | **TP** | `m_processRunning/m_runInputIndicatorActive` (+ часть config) |
| 3 | TrackContentPanel (magda-core) | **TP сильный** | interaction-машина `isCreatingSelection/isMarqueeActive/isMovingSelection` |
| 4 | RtlirVariable (deltahdl) | **TP (boolean-blindness)** | `is_real/is_string/is_event/is_queue` — взаимоисключающие type-kinds → enum |
| 5 | BrainView (mne-cpp) | TP, но **example** | `src/examples/`; view-state визуализации |
| 6 | PQCExtensionInfo (photoqt) | borderline **config** | `allow*/let*` permission-флаги |
| 7 | GFNFF (curcuma) | **TP** | cache-validity: `m_static_topology_valid/m_hbxb_fresh/m_static_state_captured` |
| 8 | MatrixTimelineItem (komai) | **TP** | message-state `isEdited/cachedIs*/mediaIsEncrypted` |
| 9 | View (pulp) | **TP** | layout-state, `border_*_set_`×4 |
| 10 | MainComponent (DirectPipe) | **TP** | audio-routing state `loadingSlot_/cached*Muted_/preMute*` |
| 11 | Sandbox3D (OloEngineBase) | **demo** | `m_Show*Test/m_UsePBRMaterials` — debug-тогглы в сэндбоксе |
| 12 | UniformBufferRegistry (OloEngineBase) | borderline | `m_IsTemplate/m_IsClone` state + config |
| 13 | GameGridView (GBAStation) | TP (механич.) | `m_prev*`×8 — per-button input → должен быть массив/битмаск |
| 14 | MainView (magda-core) | **TP сильный** | interaction `isDragging/isResizingHeaders/isUpdatingFromZoom` |
| 15 | Menu (skyrim-shaders) | TP | UI `pending*Reload` state (+ часть hotkey-config) |

**Итог eye-check:** ~11/15 чистый TP, ~2 demo/example, ~2 borderline-config. Доминирующий
паттерн реального дрейфа — **UI controller/view god-объекты** (MainWindow/Terminal/TrackContentPanel/
View/MainComponent/MainView/Menu), копящие взаимозависимые interaction/view-state флаги; плюс
computation-state кэши (GFNFF) и эталонная **boolean-blindness** (RtlirVariable: `is_real/is_string/
is_event` как N булей вместо tagged-union). Остаточные FP-классы: config-bag мимо name-фильтра
(вынесены в отдельную таблицу + эвристика по полям), churn-инфляция n_commits (поля переписывались —
1 структура, SystemState), demo/example-код, file-move split (снят дедупом по `(repo,struct)`).

## Сверка с эталонами #089

| якорь #089 | репа в корпусе? | в наших drift-результатах? |
|---|---|---|
| MethodState (MOLA) | ✓ MOLAorg_mola | **✓ c=5 f=5** — воспроизведён |
| Channel (FluidNC) | ✓ bdring_FluidNC | **✓ c=5 f=5** — воспроизведён |
| EditorShell (donner) | ✓ jwmcglynn_donner | ✗ накопление ДО окна корпус-прогона |
| HttpTransact::State (trafficserver) | ✓ apache_trafficserver | ✗ накопление ДО окна |
| Terminal (microsoft) | ≠ (у нас contour/Lightpad) | Terminal-тип воспроизведён в др. репах |

2 из 5 якорей воспроизвелись нашим **независимым** методом (нативные per-commit события vs
Python-blame по всей истории) — кросс-валидация. Отсутствие EditorShell/HttpTransact — это разница
**линз**: #089 блеймил всю историю, мы считаем накопление в окне корпус-прогона (2024-2025);
структуры, дорастившие були раньше, в окно не попадают (lower-bound на накопление, не противоречие).

## Вывод

- **Prevalence ≈ 20.2%** (240 из 1188 реп имеют структуру, дорастившую були в ≥4 коммитах в окне) —
  **независимо подтверждает ~21% из #089** на корпусе ×16 больше и другим методом.
- **Дрейф концентрируется в UI/view god-объектах** — это, а не «config-структуры», главный носитель
  boolean-state накопления; согласуется с central-finding'ом #119 (bool↔complexity: флаг→ветвление).
- Метрика остаётся **нейтральной** (accretion ≠ дефект): часть — boolean-blindness (RtlirVariable),
  часть — легитимная per-button/per-attribute механика (GameGridView), часть — config; разделяет
  только eye-check, не raw-счёт ([[project_bool_accretion_neutral_not_defect]]).
- **Граница метода:** окно = корпус-прогон, не вся история; нативный фильтр строже Python-VEND
  (#134-прототип не запускался — путь выбран ради фильтра, см. шапку).
