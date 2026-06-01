# Per-commit отчёты: коммиты, привнёсшие Type-3 дубль (#056 --diff)

> Claims чекера #056 для глазной проверки. `cd <repo>; git checkout <sha>`,
> открыть ADDED и BASE — реальная копипаста или FP (idiom/header↔impl/data/coincidental).

## makr-code/ThemisDB @ `8dbcebff06d0`  (16 hits)
- [verbatim w=1.00 line=1.00] `src/scraper/scraper_config.cpp:111-132` ⟵ `plugins/scraper/src/scraper_config.cpp:124-145`
- [verbatim w=1.00 line=1.00] `src/scraper/scraper_metadata_writer.cpp:83-101` ⟵ `plugins/scraper/src/scraper_metadata_writer.cpp:97-115`
- [verbatim w=1.00 line=1.00] `src/scraper/scraper_plugin.cpp:369-430` ⟵ `plugins/scraper/src/scraper_plugin.cpp:383-444`
- [verbatim w=1.00 line=1.00] `src/scraper/scraper_plugin.cpp:522-572` ⟵ `plugins/scraper/src/scraper_plugin.cpp:536-586`
- [diverged w=0.91 line=0.71] `src/user_storage_encrypted/multi_level_storage.cpp:60-105` ⟵ `plugins/user_storage_encrypted/src/multi_level_storage.cpp:69-112`
- [diverged w=0.83 line=0.67] `src/ethics_ai/discourse_engine.cpp:65-114` ⟵ `plugins/ethics_ai/discourse_engine.cpp:73-126`
- [diverged w=0.76 line=0.78] `src/user_storage_encrypted/key_derivation_service.cpp:264-309` ⟵ `plugins/user_storage_encrypted/src/key_derivation_service.cpp:52-92`
- [diverged w=0.74 line=0.27] `src/exporters/parquet_exporter.cpp:491-533` ⟵ `src/exporters/arrow_ipc_exporter.cpp:868-913`
- [diverged w=0.74 line=0.26] `src/exporters/arrow_ipc_exporter.cpp:841-882` ⟵ `src/exporters/parquet_exporter.cpp:500-549`
- [diverged w=0.73 line=0.10] `src/security/timestamp_authority.cpp:905-930` ⟵ `src/security/timestamp_authority_openssl.cpp:362-373`
- [diverged w=0.71 line=0.25] `src/analytics/forecasting.cpp:596-635` ⟵ `tests/analytics/test_forecasting.cpp:690-723`
- [diverged w=0.71 line=0.35] `src/llm/lora_framework/nccl_backend.cpp:109-195` ⟵ `src/llm/lora_framework/rccl_backend.cpp:123-186`
- [diverged w=0.71 line=0.35] `src/llm/lora_framework/rccl_backend.cpp:109-194` ⟵ `src/llm/lora_framework/nccl_backend.cpp:123-186`
- [diverged w=0.70 line=0.19] `src/llm/lora_framework/nccl_backend.cpp:324-378` ⟵ `src/llm/lora_framework/rccl_backend.cpp:290-337`
- [diverged w=0.70 line=0.19] `src/llm/lora_framework/rccl_backend.cpp:323-377` ⟵ `src/llm/lora_framework/nccl_backend.cpp:290-337`
- [diverged w=0.65 line=0.51] `src/ethics_ai/ethics_evaluator.cpp:40-66` ⟵ `plugins/ethics_ai/ethics_evaluator.cpp:32-67`

## chrxh/alien @ `caeee1608c92`  (1 hits)
- [diverged w=0.60 line=0.32] `source/EngineInterface/ShapeGenerator.h:67-87` ⟵ `source/EngineInterface/ShapeGenerator.cpp:334-355`

## GregorGullwi/FlashCpp @ `3fab46902847`  (6 hits)
- [near w=0.95 line=0.83] `src/Parser_Templates_Inst_Deduction.cpp:2231-2257` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:759-784`
- [near w=0.95 line=0.83] `src/Parser_Templates_Inst_MemberFunc.cpp:1824-1850` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:2208-2233`
- [diverged w=0.70 line=0.21] `tests/for_loops_ret89.cpp:82-95` ⟵ `tests/for_loops_simple_ret85.cpp:77-85`
- [diverged w=0.64 line=0.16] `src/Parser_Templates_Inst_Deduction.cpp:2363-2388` ⟵ `src/Parser_Templates_Inst_MemberFunc.cpp:1351-1393`
- [diverged w=0.62 line=0.29] `src/ExpressionSubstitutor.cpp:3394-3437` ⟵ `src/Parser_Templates_Substitution.cpp:662-687`
- [diverged w=0.60 line=0.16] `src/Parser_Templates_Inst_MemberFunc.cpp:2530-2572` ⟵ `src/Parser_Templates_Inst_Deduction.cpp:2348-2377`

## viperx1/Usagi-dono @ `fc9decc85ebb`  (2 hits)
- [diverged w=0.68 line=0.52] `usagi/src/hashercoordinator.cpp:227-259` ⟵ `usagi/src/window.cpp:357-394`
- [diverged w=0.65 line=0.35] `usagi/src/hashercoordinator.cpp:209-223` ⟵ `usagi/src/window.cpp:338-353`

## Krilliac/SparkEngine @ `1ef9347d8d78`  (25 hits)
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesWindows.cpp:607-619` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:659-671`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemWindows.cpp:666-692` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:627-653`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineLinux.cpp:195-209` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1032-1046`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemWindows.cpp:130-146` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:125-141`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemLinux.cpp:102-110` ⟵ `SparkEngine/Source/Graphics/LightingSystem.cpp:904-912`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineLinux.cpp:560-609` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:1315-1364`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemInternalWindows.cpp:849-891` ⟵ `SparkEngine/Source/Graphics/LightingSystemInternal.cpp:844-886`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/LightingSystemInternalWindows.cpp:242-309` ⟵ `SparkEngine/Source/Graphics/LightingSystemInternal.cpp:237-304`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsStateAndSettingsWindows.cpp:427-485` ⟵ `SparkEngine/Source/Graphics/GraphicsStateAndSettings.cpp:422-480`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineWindows.cpp:289-303` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:374-388`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsStateAndSettingsWindows.cpp:339-361` ⟵ `SparkEngine/Source/Graphics/GraphicsStateAndSettings.cpp:334-356`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsStateAndSettingsWindows.cpp:250-273` ⟵ `SparkEngine/Source/Graphics/GraphicsStateAndSettings.cpp:245-268`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineWindows.cpp:587-606` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:587-606`
- [near w=1.00 line=0.93] `SparkEngine/Source/Core/Lifecycle/GameplayLifecycleShared.cpp:998-1037` ⟵ `SparkEngine/Source/Core/GameplaySystemLifecycle.cpp:640-678`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesWindows.cpp:358-400` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:410-452`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Core/SparkEngineWindows.cpp:904-919` ⟵ `SparkEngine/Source/Core/SparkEngine.cpp:831-846`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsRenderPipelinesLinux.cpp:384-420` ⟵ `SparkEngine/Source/Graphics/GraphicsRenderPipelines.cpp:1048-1084`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/AssetPipelineLinux.cpp:116-167` ⟵ `SparkEngine/Source/Graphics/AssetPipeline.cpp:534-585`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/AssetPipelineWindows.cpp:109-164` ⟵ `SparkEngine/Source/Graphics/AssetPipeline.cpp:104-159`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/FogSystem.cpp:76-84` ⟵ `SparkEngine/Source/Graphics/FogSystem.h:277-285`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsConsoleOpsLinux.cpp:98-104` ⟵ `SparkEngine/Source/Graphics/GraphicsConsoleOps.cpp:616-622`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsDeviceResourcesWindows.cpp:73-87` ⟵ `SparkEngine/Source/Graphics/GraphicsDeviceResources.cpp:73-87`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:1286-1315` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:922-951`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:988-1024` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:765-801`
- [verbatim w=1.00 line=1.00] `SparkEngine/Source/Graphics/GraphicsEngineWindows.cpp:776-829` ⟵ `SparkEngine/Source/Graphics/GraphicsEngine.cpp:612-665`

## djeada/Standard-of-Iron @ `7a0588bbe3f4`  (14 hits)
- [diverged w=1.00 line=0.25] `game/audio/miniaudio_backend.cpp:330-340` ⟵ `game/audio/MiniaudioBackend.cpp:33-43`
- [verbatim w=1.00 line=1.00] `game/audio/miniaudio_backend.cpp:406-413` ⟵ `game/audio/MiniaudioBackend.cpp:106-113`
- [verbatim w=1.00 line=1.00] `render/ground/terrain_renderer_mesh.cpp:349-364` ⟵ `render/ground/terrain_renderer.cpp:207-222`
- [diverged w=0.99 line=0.25] `render/ground/linear_feature_geometry.cpp:435-465` ⟵ `render/ground/bridge_renderer.cpp:175-205`
- [diverged w=0.97 line=0.26] `render/gl/backend/effects_command_executor.cpp:229-251` ⟵ `render/gl/backend.cpp:1600-1617`
- [diverged w=0.92 line=0.70] `game/audio/miniaudio_backend.cpp:479-512` ⟵ `game/audio/MiniaudioBackend.cpp:159-193`
- [diverged w=0.91 line=0.06] `app/core/runtime_frame_orchestrator.cpp:72-82` ⟵ `app/core/game_engine.cpp:879-889`
- [diverged w=0.84 line=0.48] `app/models/map_preview_image_provider.cpp:12-39` ⟵ `app/models/minimap_image_provider.cpp:9-37`
- [diverged w=0.84 line=0.48] `app/models/minimap_image_provider.cpp:11-39` ⟵ `app/models/map_preview_image_provider.cpp:10-37`
- [near w=0.83 line=0.93] `game/audio/miniaudio_backend.cpp:442-459` ⟵ `game/audio/MiniaudioBackend.cpp:130-146`
- [diverged w=0.83 line=0.11] `app/core/frame_ui_coordinator.cpp:87-93` ⟵ `app/core/game_engine.cpp:960-966`
- [diverged w=0.79 line=0.06] `render/entity/nations/carthage/ballista_renderer.cpp:160-215` ⟵ `render/entity/nations/roman/ballista_renderer.cpp:123-162`
- [diverged w=0.79 line=0.06] `render/entity/nations/roman/ballista_renderer.cpp:150-205` ⟵ `render/entity/nations/carthage/ballista_renderer.cpp:128-167`
- [diverged w=0.76 line=0.47] `game/audio/music_player.cpp:225-235` ⟵ `game/audio/MusicPlayer.cpp:141-151`

## shifty81/FreshVoxel @ `c93674b629ee`  (25 hits)
- [verbatim w=1.00 line=1.00] `engine/renderer/ShaderManager.cpp:15-30` ⟵ `src/renderer/ShaderManager.cpp:16-31`
- [verbatim w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:1028-1048` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:761-781`
- [verbatim w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:951-990` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:681-720`
- [near w=1.00 line=0.81] `engine/renderer/backends/DirectX12RenderContext.cpp:715-737` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:597-618`
- [verbatim w=1.00 line=1.00] `engine/renderer/backends/DirectX12RenderContext.cpp:613-622` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:491-500`
- [near w=1.00 line=0.93] `engine/renderer/backends/DirectX12RenderContext.cpp:404-441` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:284-321`
- [verbatim w=1.00 line=1.00] `engine/renderer/backends/DirectX11RenderContext.cpp:691-727` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:580-616`
- [verbatim w=1.00 line=1.00] `engine/renderer/backends/DirectX11RenderContext.cpp:661-686` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:551-576`
- [diverged w=1.00 line=0.57] `engine/renderer/backends/DirectX11RenderContext.cpp:571-596` ⟵ `src/renderer/backends/DirectX11RenderContext.cpp:456-488`
- [diverged w=1.00 line=0.24] `engine/renderer/TextureManager.cpp:73-115` ⟵ `src/renderer/TextureManager.cpp:67-89`
- [diverged w=1.00 line=0.58] `engine/renderer/backends/DirectX12RenderContext.cpp:1052-1068` ⟵ `src/renderer/backends/DirectX12RenderContext.cpp:784-797`
- [verbatim w=1.00 line=1.00] `engine/renderer/RenderContext.cpp:22-52` ⟵ `src/renderer/RenderContext.cpp:20-50`
- [verbatim w=1.00 line=1.00] `engine/physics/PhysicsSystemEnhanced.cpp:154-204` ⟵ `src/physics/PhysicsSystemEnhanced.cpp:138-188`
- [diverged w=1.00 line=0.75] `engine/physics/PhysicsSystemEnhanced.cpp:80-96` ⟵ `src/physics/PhysicsSystemEnhanced.cpp:72-88`
- [verbatim w=1.00 line=1.00] `engine/physics/PhysicsSystemEnhanced.cpp:30-64` ⟵ `src/physics/PhysicsSystemEnhanced.cpp:24-58`
- [diverged w=1.00 line=0.74] `engine/physics/CollisionDetection.cpp:383-405` ⟵ `src/physics/CollisionDetection.cpp:332-355`
- [near w=1.00 line=0.84] `engine/physics/CollisionDetection.cpp:276-297` ⟵ `src/physics/CollisionDetection.cpp:253-273`
- [near w=1.00 line=0.83] `engine/networking/NetworkMessage.cpp:130-153` ⟵ `src/networking/NetworkMessage.cpp:114-138`
- [verbatim w=1.00 line=1.00] `engine/networking/NetworkMessage.cpp:42-46` ⟵ `src/networking/NetworkMessage.cpp:36-40`
- [verbatim w=1.00 line=1.00] `engine/rpg/TradingSystem.cpp:42-58` ⟵ `src/rpg/TradingSystem.cpp:34-50`
- [near w=1.00 line=0.82] `engine/voxel/ChunkStreamer.cpp:122-145` ⟵ `src/voxel/ChunkStreamer.cpp:99-121`
- [near w=1.00 line=0.80] `engine/voxel/ChunkStreamer.cpp:25-42` ⟵ `src/voxel/ChunkStreamer.cpp:20-36`
- [verbatim w=1.00 line=1.00] `engine/ui/MainMenu.cpp:121-148` ⟵ `src/ui/MainMenu.cpp:112-139`
- [near w=1.00 line=0.85] `engine/ui/MainMenu.cpp:83-117` ⟵ `src/ui/MainMenu.cpp:75-109`
- [verbatim w=1.00 line=1.00] `engine/ui/MainMenu.cpp:57-73` ⟵ `src/ui/MainMenu.cpp:53-69`

## Tsoympet/PantheonChain @ `e8ba8e778aa1`  (3 hits)
- [verbatim w=1.00 line=1.00] `layer1-talanton/wallet/hardware/firmware_verification.cpp:160-209` ⟵ `layer1/wallet/hardware/firmware_verification.cpp:46-95`
- [near w=0.91 line=0.85] `layer2-drachma/rollups/zk_rollup.cpp:334-349` ⟵ `layer2/rollups/zk_rollup.cpp:99-114`
- [near w=0.75 line=0.82] `layer1-talanton/wallet/hardware/firmware_verification.cpp:259-277` ⟵ `layer1/wallet/hardware/firmware_verification.cpp:112-127`

## studiocollective/songbird @ `8f9ed331912c`  (1 hits)
- [diverged w=0.97 line=0.40] `libraries/juce-host/src/sb_host_juce.cpp:498-500` ⟵ `app/ChannelStripWindow.h:194-197`

## melkyr/znineeight @ `589a9272bf72`  (1 hits)
- [verbatim w=1.00 line=1.00] `tests/test_runner_main.hpp:17-31` ⟵ `tests/main.cpp:9-23`

## xsscx/research @ `40d1fcceafbe`  (5 hits)
- [near w=1.00 line=0.81] `colorbleed_tools/ColorBleedSafeF16.cpp:31-69` ⟵ `iccanalyzer-lite/IccDevSafeOverrides.cpp:69-103`
- [diverged w=0.92 line=0.43] `iccanalyzer-lite/IccHeuristicsXmlSafety.cpp:108-151` ⟵ `iccanalyzer-lite/icctest/lib/src/checks/XmlSafetyChecks.cpp:47-86`
- [diverged w=0.91 line=0.41] `iccanalyzer-lite/icctest/lib/src/checks/ComplianceChecks.cpp:1236-1265` ⟵ `iccanalyzer-lite/IccHeuristicsProfileCompliance.cpp:1411-1438`
- [diverged w=0.79 line=0.28] `iccanalyzer-lite/icctest/lib/src/checks/DataValidationChecks.cpp:1730-1788` ⟵ `iccanalyzer-lite/IccHeuristicsDataValidation.cpp:1864-1923`
- [diverged w=0.62 line=0.14] `iccanalyzer-lite/icctest/lib/src/checks/DataValidationChecks.cpp:1615-1655` ⟵ `iccanalyzer-lite/IccHeuristicsDataValidation.cpp:1746-1801`

## ImagingTools/Acf @ `e17192b91541`  (18 hits)
- [verbatim w=1.00 line=1.00] `Include/ifilegui/CFileDialogLoaderComp.cpp:140-153` ⟵ `Include/ifile/CComposedFilePersistenceComp.cpp:131-144`
- [near w=1.00 line=0.88] `Tests/TestComposedGui/main.cpp:18-40` ⟵ `Impl/Tests/ComposedGuiTest/main.cpp:39-61`
- [verbatim w=1.00 line=1.00] `Include/iprm/CParamsManagerCompBase.cpp:590-599` ⟵ `Include/iprm/CMultiParamsManagerComp.cpp:312-321`
- [verbatim w=1.00 line=1.00] `Include/iprm/CMultiParamsManagerComp.cpp:313-322` ⟵ `Include/iprm/CParamsManagerCompBase.cpp:536-545`
- [verbatim w=1.00 line=1.00] `Include/ifile/CComposedFilePersistenceComp.cpp:188-201` ⟵ `Include/ifilegui/CFileDialogLoaderComp.cpp:131-144`
- [diverged w=0.89 line=0.49] `Include/ipackage/CRegistriesManagerComp.cpp:321-380` ⟵ `Include/ipackage/CPackagesLoaderComp.cpp:377-440`
- [diverged w=0.88 line=0.56] `Include/iqtprm/COptionsManagerEditorComp.cpp:183-229` ⟵ `Include/iqtprm/COptionsListEditorComp.cpp:51-104`
- [diverged w=0.88 line=0.56] `Include/iqtprm/COptionsListEditorComp.cpp:52-105` ⟵ `Include/iqtprm/COptionsManagerEditorComp.cpp:182-228`
- [diverged w=0.80 line=0.29] `Include/iview/CInteractiveViewLayer.cpp:535-548` ⟵ `Include/iview/CViewLayer.cpp:130-143`
- [diverged w=0.80 line=0.29] `Include/iview/CViewLayer.cpp:131-144` ⟵ `Include/iview/CInteractiveViewLayer.cpp:495-508`
- [diverged w=0.75 line=0.57] `Include/iqtprm/CExtComposedParamsSetGuiComp.cpp:183-206` ⟵ `Include/iqtprm/CComposedParamsSetGuiComp.cpp:638-658`
- [diverged w=0.74 line=0.70] `Include/ifilegui/CSimpleFilePathParamGuiComp.cpp:158-234` ⟵ `Include/iqtprm/CFileNameParamGuiComp.cpp:170-223`
- [diverged w=0.74 line=0.70] `Include/iqtprm/CFileNameParamGuiComp.cpp:171-224` ⟵ `Include/ifilegui/CSimpleFilePathParamGuiComp.cpp:157-233`
- [diverged w=0.73 line=0.46] `Include/iview/CPolygonShape.cpp:452-502` ⟵ `Include/iview/CPolylineShape.cpp:486-552`
- [diverged w=0.72 line=0.50] `Include/iqtprm/CComposedParamsSetGuiComp.cpp:643-668` ⟵ `Include/iqtprm/CExtComposedParamsSetGuiComp.cpp:182-205`
- [diverged w=0.69 line=0.47] `Include/iview/CPolylineShape.cpp:502-569` ⟵ `Include/iview/CPolygonShape.cpp:431-483`
- [diverged w=0.62 line=0.59] `Include/iwidgets/CMiniWidgetDelegate.cpp:89-124` ⟵ `Include/iwidgets/CCollapsibleGroupWidgetDelegate.cpp:98-151`
- [diverged w=0.62 line=0.59] `Include/iwidgets/CCollapsibleGroupWidgetDelegate.cpp:99-152` ⟵ `Include/iwidgets/CMiniWidgetDelegate.cpp:88-123`

## winnerspiros/UnleashedRecomp_Android @ `80e1f2ecb46d`  (7 hits)
- [verbatim w=1.00 line=1.00] `UnleashedRecomp/kernel/io/nt.cpp:28-41` ⟵ `UnleashedRecomp/kernel/imports.cpp:383-396`
- [verbatim w=1.00 line=1.00] `UnleashedRecomp/kernel/threading.cpp:15-30` ⟵ `UnleashedRecomp/kernel/imports.cpp:571-586`
- [verbatim w=1.00 line=1.00] `UnleashedRecomp/kernel/threading.cpp:39-50` ⟵ `UnleashedRecomp/kernel/imports.cpp:1423-1434`
- [verbatim w=1.00 line=1.00] `UnleashedRecomp/kernel/threading.cpp:54-66` ⟵ `UnleashedRecomp/kernel/imports.cpp:640-652`
- [diverged w=0.76 line=0.45] `UnleashedRecomp/user/achievement_manager.cpp:100-176` ⟵ `UnleashedRecomp/user/persistent_storage_manager.cpp:39-97`
- [diverged w=0.63 line=0.44] `UnleashedRecomp/tests/test_config.cpp:210-241` ⟵ `UnleashedRecomp/user/config.cpp:794-829`
- [diverged w=0.61 line=0.14] `UnleashedRecomp/tests/mock/xbox.h:16-21` ⟵ `tools/x_decompress/x_decompress.cpp:32-43`

## Lovuwer/FluffyApp-RE @ `f8385dd4caf4`  (4 hits)
- [diverged w=0.84 line=0.53] `src/Core/Memory/RegionEnumerator.cpp:60-110` ⟵ `src/Core/Memory/MemoryScanner.cpp:238-276`
- [diverged w=0.73 line=0.38] `src/Core/Memory/RegionEnumerator.cpp:129-170` ⟵ `src/Core/Memory/MemoryScanner.cpp:238-276`
- [diverged w=0.72 line=0.24] `src/Core/Memory/PatternScanner.cpp:253-296` ⟵ `src/Core/Memory/MemoryScanner.cpp:58-92`
- [diverged w=0.66 line=0.25] `src/Core/Memory/PatternScanner.cpp:72-119` ⟵ `src/Core/Memory/MemoryScanner.cpp:58-92`

## esrrhs/fakelua @ `8c2fe347eca4`  (2 hits)
- [diverged w=0.87 line=0.78] `src/compile/my_flexer.cpp:10-18` ⟵ `src/compile/myflexer.cpp:15-23`
- [diverged w=0.70 line=0.71] `src/compile/my_flexer.cpp:22-27` ⟵ `src/compile/myflexer.cpp:26-31`

## starseeker/obol @ `bbe9f6aeeccf`  (25 hits)
- [verbatim w=1.00 line=1.00] `src/actions/SoActionP.cpp:60-78` ⟵ `upstream/src/actions/SoActionP.cpp:60-78`
- [verbatim w=1.00 line=1.00] `external/nanort/stb_image.h:4858-4863` ⟵ `Mentor/stb_image.h:5216-5221`
- [diverged w=1.00 line=0.68] `src/nodes/SoDepthBuffer.cpp:193-219` ⟵ `upstream/src/nodes/SoDepthBuffer.cpp:193-219`
- [near w=1.00 line=0.90] `src/nodekits/SoBaseKit.cpp:853-920` ⟵ `upstream/src/nodekits/SoBaseKit.cpp:859-926`
- [verbatim w=1.00 line=1.00] `external/nanort/stb_image.h:5992-6064` ⟵ `Mentor/stb_image.h:6422-6494`
- [near w=1.00 line=0.87] `src/misc/SoPath.cpp:537-591` ⟵ `upstream/src/misc/SoPath.cpp:537-591`
- [verbatim w=1.00 line=1.00] `external/nanort/stb_image.h:7112-7150` ⟵ `Mentor/stb_image.h:7634-7672`
- [diverged w=1.00 line=0.08] `src/misc/SoDBOSMesa.cpp:61-72` ⟵ `tests/utils/headless_utils.h:111-127`
- [near w=1.00 line=0.85] `src/misc/SoDB.cpp:1524-1558` ⟵ `upstream/src/misc/SoDB.cpp:1652-1686`
- [near w=1.00 line=0.81] `src/actions/SoAction.cpp:370-388` ⟵ `upstream/src/actions/SoAction.cpp:372-391`
- [verbatim w=1.00 line=1.00] `src/actions/SoAction.cpp:571-578` ⟵ `upstream/src/actions/SoAction.cpp:586-593`
- [near w=1.00 line=0.81] `tests/utils/osmesa_context_manager.h:65-78` ⟵ `tests/utils/headless_utils.h:111-127`
- [verbatim w=1.00 line=1.00] `src/actions/SoGLRenderAction.cpp:1135-1158` ⟵ `upstream/src/actions/SoGLRenderAction.cpp:1129-1153`
- [diverged w=1.00 line=0.70] `src/misc/CoinTidbits.cpp:597-631` ⟵ `src/C/CoinTidbits.cpp:603-637`
- [near w=1.00 line=0.87] `src/misc/CoinResources.cpp:291-309` ⟵ `upstream/src/misc/CoinResources.cpp:296-314`
- [near w=1.00 line=0.90] `src/io/SoOutput.cpp:1504-1558` ⟵ `upstream/src/io/SoOutput.cpp:1516-1570`
- [near w=1.00 line=0.94] `src/fields/SoField.cpp:846-942` ⟵ `upstream/src/fields/SoField.cpp:933-1029`
- [diverged w=1.00 line=0.69] `src/engines/SoCalculator.cpp:414-427` ⟵ `upstream/src/engines/SoCalculator.cpp:414-427`
- [near w=1.00 line=0.90] `src/elements/GL/SoGLVertexAttributeElement.cpp:109-163` ⟵ `upstream/src/elements/GL/SoGLVertexAttributeElement.cpp:109-163`
- [verbatim w=1.00 line=0.96] `src/draggers/SoTransformerDragger.cpp:1779-1834` ⟵ `upstream/src/draggers/SoTransformerDragger.cpp:1782-1837`
- [verbatim w=1.00 line=0.98] `src/caches/SoGLCacheList.cpp:180-449` ⟵ `upstream/src/caches/SoGLCacheList.cpp:182-451`
- [near w=1.00 line=0.93] `src/caches/SoGLCacheList.cpp:569-616` ⟵ `upstream/src/caches/SoGLCacheList.cpp:571-618`
- [near w=1.00 line=0.80] `src/caches/SoPrimitiveVertexCache.cpp:252-312` ⟵ `upstream/src/caches/SoPrimitiveVertexCache.cpp:252-312`
- [diverged w=1.00 line=0.50] `src/caches/SoPrimitiveVertexCache.cpp:372-383` ⟵ `upstream/src/caches/SoPrimitiveVertexCache.cpp:372-383`
- [verbatim w=1.00 line=1.00] `external/nanort/stb_image.h:2603-2603` ⟵ `tests/utils/stb_image.h:2866-2866`

## katahiromz/onigpp @ `eefb6962d665`  (4 hits)
- [diverged w=1.00 line=0.77] `tests/onigpp_test.cpp:83-123` ⟵ `onigpp_test.cpp:101-141`
- [diverged w=1.00 line=0.51] `tests/onigpp_test.cpp:310-360` ⟵ `onigpp_test.cpp:312-362`
- [diverged w=1.00 line=0.05] `tests/onigpp_testw.cpp:85-123` ⟵ `onigpp_test.cpp:101-141`
- [diverged w=0.78 line=0.62] `src/onigpp.cpp:1820-1834` ⟵ `onigpp.h:533-547`

## djeada/Lightpad @ `ede0e4fd108c`  (3 hits)
- [verbatim w=1.00 line=1.00] `App/core/textarea.cpp:168-182` ⟵ `App/textarea.cpp:13-28`
- [diverged w=0.71 line=0.56] `App/syntax/lightpadsyntaxhighlighter.cpp:66-112` ⟵ `App/lightpadsyntaxhighlighter.cpp:50-81`
- [diverged w=0.62 line=0.07] `App/syntax/lightpadsyntaxhighlighter.cpp:121-185` ⟵ `App/lightpadsyntaxhighlighter.cpp:16-40`

## MaximumTrainer/MaximumTrainer_Redux @ `d0a6b1081962`  (1 hits)
- [diverged w=0.97 line=0.19] `src/persistence/db/selfloops_service.cpp:36-56` ⟵ `src/app/util.cpp:812-834`

## joshualongenecker/home-assistant-bridge-esphome @ `8ed9af78e53f`  (4 hits)
- [diverged w=0.89 line=0.73] `components/geappliances_bridge/mqtt_bridge.cpp:27-57` ⟵ `src/mqtt_bridge.cpp:64-95`
- [near w=0.88 line=0.89] `components/geappliances_bridge/mqtt_bridge.cpp:106-133` ⟵ `src/mqtt_bridge.cpp:128-155`
- [diverged w=0.78 line=0.62] `components/geappliances_bridge/mqtt_bridge.cpp:61-94` ⟵ `src/mqtt_bridge.cpp:99-124`
- [diverged w=0.74 line=0.09] `components/geappliances_bridge/esphome_uart_adapter.cpp:9-39` ⟵ `src/tiny_uart_adapter.cpp:10-24`

## Arsia-Mons/Silencer @ `af4c50c5b8ec`  (25 hits)
- [verbatim w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:639-657` ⟵ `src/renderer.cpp:509-527`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/input/keybinds.cpp:464-479` ⟵ `src/game.cpp:5199-5214`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/main.cpp:14-21` ⟵ `src/main.cpp:12-19`
- [diverged w=1.00 line=0.63] `clients/silencer/src/main.cpp:86-141` ⟵ `src/main.cpp:84-139`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/net/lobby.cpp:67-95` ⟵ `src/lobby.cpp:34-62`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/objects/pickup.cpp:92-114` ⟵ `src/pickup.cpp:91-113`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/platform/os.cpp:68-76` ⟵ `src/os.cpp:34-42`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/projectiles/grenade.cpp:315-336` ⟵ `src/grenade.cpp:283-304`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/game/session/ambience_mixer.cpp:104-110` ⟵ `src/game.cpp:3805-3811`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/game/replay/replay.cpp:218-226` ⟵ `src/replay.cpp:216-224`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:327-335` ⟵ `src/renderer.cpp:219-227`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:1312-1320` ⟵ `src/renderer.cpp:1150-1158`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:4552-4625` ⟵ `src/player.cpp:4408-4481`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/render/renderer.cpp:1425-1436` ⟵ `src/renderer.cpp:1263-1274`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:4241-4257` ⟵ `src/player.cpp:4108-4124`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:3143-3174` ⟵ `src/player.cpp:3024-3055`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/actors/player/player.cpp:2421-2446` ⟵ `src/player.cpp:2377-2402`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/resources/resources.cpp:216-226` ⟵ `src/resources.cpp:205-215`
- [near w=1.00 line=0.93] `clients/silencer/src/actors/player/player.cpp:1021-1073` ⟵ `src/player.cpp:1007-1059`
- [verbatim w=1.00 line=1.00] `clients/silencer/src/resources/resources.cpp:430-472` ⟵ `src/resources.cpp:410-452`
- [near w=0.99 line=0.80] `clients/silencer/src/actors/player/player.cpp:401-413` ⟵ `src/player.cpp:331-343`
- [near w=0.99 line=0.84] `clients/silencer/src/game/tick/tick_hostjoin.cpp:15-32` ⟵ `src/game.cpp:1875-1893`
- [verbatim w=0.99 line=0.97] `clients/silencer/src/game/session/map_downloader.cpp:29-66` ⟵ `src/game.cpp:5383-5420`
- [verbatim w=0.98 line=0.95] `clients/silencer/src/objects/pickup.cpp:35-90` ⟵ `src/pickup.cpp:34-89`
- [diverged w=0.98 line=0.76] `clients/silencer/src/world/objects/world_objects.cpp:229-272` ⟵ `src/world.cpp:2380-2423`

## El-EnderJ/NeoCalculator @ `7a1a415a1405`  (1 hits)
- [near w=0.92 line=0.85] `src/SystemApp.cpp:352-424` ⟵ `src/SystemApp.cpp:173-236`

## lyquid/Romulus @ `8d0f008a390f`  (3 hits)
- [diverged w=1.00 line=0.67] `lib/romulus/database/database.cpp:429-444` ⟵ `lib/romulus/database/database.cpp:292-309`
- [diverged w=0.86 line=0.67] `lib/romulus/scanner/hash_service.cpp:78-86` ⟵ `lib/romulus/scanner/hash_service.cpp:66-73`
- [diverged w=0.72 line=0.45] `lib/romulus/dat/dat_parser.cpp:183-221` ⟵ `lib/romulus/dat/dat_parser.cpp:116-141`

## AmoghS1234/CipherMesh @ `aff36867e245`  (9 hits)
- [diverged w=1.00 line=0.50] `src/desktop/mainwindow.cpp:1763-1773` ⟵ `src/desktop/mainwindow.cpp:1437-1449`
- [diverged w=1.00 line=0.78] `src/p2p_webrtc/webrtcservice.cpp:679-686` ⟵ `src/p2p_webrtc/webrtcservice.cpp:412-419`
- [diverged w=0.98 line=0.67] `src/desktop/mainwindow.cpp:273-279` ⟵ `src/desktop/mainwindow.cpp:215-226`
- [near w=0.94 line=0.82] `src/desktop/mainwindow.cpp:1812-1844` ⟵ `src/desktop/mainwindow.cpp:1489-1521`
- [near w=0.94 line=0.94] `src/desktop/settingsdialog.cpp:251-291` ⟵ `src/desktop/settingsdialog.cpp:208-248`
- [diverged w=0.82 line=0.75] `src/desktop/mainwindow.cpp:282-301` ⟵ `src/desktop/mainwindow.cpp:229-245`
- [diverged w=0.72 line=0.63] `src/desktop/mainwindow.cpp:936-1004` ⟵ `src/desktop/mainwindow.cpp:778-835`
- [diverged w=0.70 line=0.20] `src/core/database.cpp:92-101` ⟵ `src/core/database.cpp:22-31`
- [diverged w=0.65 line=0.08] `src/core/database.cpp:115-121` ⟵ `src/core/database.cpp:89-96`

## remiserriere/AI-on-the-edge-device @ `0bc308c9b6c4`  (25 hits)
- [diverged w=1.00 line=0.07] `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:62-104` ⟵ `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:23-59`
- [diverged w=1.00 line=0.07] `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:23-59` ⟵ `code/test/components/jomjol-flowcontroll/test_PointerEvalAnalogToDigitNew.cpp:62-104`
- [verbatim w=1.00 line=1.00] `code/components/jomjol_webhook/interface_webhook.cpp:139-168` ⟵ `code/components/jomjol_influxdb/interface_influxdb.cpp:102-131`
- [diverged w=1.00 line=0.79] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:204-230` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:127-154`
- [verbatim w=1.00 line=1.00] `code/components/jomjol_helper/Helper.cpp:1204-1229` ⟵ `code/components/jomjol_helper/Helper.cpp:1042-1067`
- [diverged w=1.00 line=0.12] `code/components/jomjol_helper/Helper.cpp:1121-1162` ⟵ `code/components/jomjol_helper/Helper.cpp:981-1000`
- [diverged w=1.00 line=0.44] `code/components/jomjol_helper/Helper.cpp:551-624` ⟵ `code/components/jomjol_helper/Helper.cpp:478-531`
- [diverged w=1.00 line=0.66] `code/components/jomjol_flowcontroll/MainFlowControl.cpp:1750-1812` ⟵ `code/components/jomjol_flowcontroll/MainFlowControl.cpp:1011-1068`
- [diverged w=1.00 line=0.79] `code/components/jomjol_flowcontroll/MainFlowControl.cpp:961-979` ⟵ `code/components/jomjol_flowcontroll/MainFlowControl.cpp:634-650`
- [diverged w=1.00 line=0.56] `code/components/jomjol_flowcontroll/MainFlowControl.cpp:339-376` ⟵ `code/components/jomjol_flowcontroll/MainFlowControl.cpp:145-179`
- [diverged w=1.00 line=0.48] `code/components/jomjol_controlGPIO/SmartLeds.h:262-286` ⟵ `code/components/jomjol_controlGPIO/SmartLeds.h:478-502`
- [diverged w=1.00 line=0.54] `code/components/jomjol_controlGPIO/SmartLeds.h:390-414` ⟵ `code/components/jomjol_controlGPIO/SmartLeds.h:478-502`
- [diverged w=1.00 line=0.75] `code/components/jomjol_flowcontroll/ClassFlowMQTT.cpp:232-251` ⟵ `code/components/jomjol_flowcontroll/ClassFlowMQTT.cpp:203-222`
- [diverged w=0.99 line=0.35] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:620-637` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:584-604`
- [diverged w=0.99 line=0.48] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:856-932` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:629-690`
- [diverged w=0.99 line=0.49] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:753-852` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:543-624`
- [near w=0.99 line=0.82] `code/components/jomjol_flowcontroll/ClassFlowTakeImage.cpp:600-610` ⟵ `code/components/jomjol_flowcontroll/ClassFlowTakeImage.cpp:294-304`
- [diverged w=0.99 line=0.28] `code/components/jomjol_flowcontroll/ClassFlowPostProcessing.cpp:994-1018` ⟵ `code/components/jomjol_flowcontroll/ClassFlowPostProcessing.cpp:956-978`
- [diverged w=0.98 line=0.74] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:551-580` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:504-531`
- [diverged w=0.98 line=0.59] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:488-529` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:426-463`
- [diverged w=0.98 line=0.62] `code/components/jomjol_controlcamera/ClassControllCamera.cpp:1025-1065` ⟵ `code/components/jomjol_controlcamera/ClassControllCamera.cpp:763-793`
- [diverged w=0.95 line=0.63] `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:341-385` ⟵ `code/components/jomjol_flowcontroll/ClassFlowControll.cpp:269-315`
- [near w=0.94 line=0.83] `code/main/main.cpp:943-1007` ⟵ `code/main/main.cpp:753-816`
- [diverged w=0.93 line=0.40] `code/components/jomjol_flowcontroll/ClassFlowAlignment.cpp:220-230` ⟵ `code/components/jomjol_flowcontroll/ClassFlowAlignment.cpp:234-241`
- [near w=0.92 line=0.94] `code/main/main.cpp:337-357` ⟵ `code/main/main.cpp:268-287`

## philstopford/cine-encoder @ `74f803e700e7`  (6 hits)
- [diverged w=0.91 line=0.26] `app/mainwindow.cpp:3696-3719` ⟵ `app/mainwindow.cpp:4671-4699`
- [diverged w=0.88 line=0.28] `app/mainwindow.cpp:1088-1120` ⟵ `app/mainwindow.cpp:649-673`
- [diverged w=0.87 line=0.65] `app/mainwindow.cpp:3539-3551` ⟵ `app/mainwindow.cpp:4549-4563`
- [diverged w=0.74 line=0.27] `app/mainwindow.cpp:2951-2975` ⟵ `app/mainwindow.cpp:3417-3436`
- [diverged w=0.74 line=0.27] `app/mainwindow.cpp:3104-3109` ⟵ `app/mainwindow.cpp:3562-3570`
- [diverged w=0.73 line=0.35] `app/mainwindow.cpp:3461-3495` ⟵ `app/mainwindow.cpp:4458-4499`

## hammermaps/halflife-op4-updated-sohl @ `d6c1fa262066`  (25 hits)
- [diverged w=1.00 line=0.33] `utils/common/cmdlib.cpp:688-693` ⟵ `utils/common/cmdlib.c:750-755`
- [diverged w=1.00 line=0.57] `utils/qcsg/qcsg.cpp:399-488` ⟵ `utils/qcsg/qcsg.c:402-495`
- [diverged w=1.00 line=0.17] `utils/qcsg/qcsg.cpp:316-323` ⟵ `utils/qcsg/qcsg.c:319-326`
- [diverged w=1.00 line=0.44] `utils/qcsg/gldraw.cpp:63-111` ⟵ `utils/qcsg/gldraw.c:37-86`
- [diverged w=1.00 line=0.47] `utils/qcsg/brush.cpp:935-973` ⟵ `utils/qcsg/brush.c:943-984`
- [diverged w=1.00 line=0.50] `utils/qbsp2/tjunc.cpp:451-468` ⟵ `utils/qbsp2/tjunc.c:454-471`
- [diverged w=1.00 line=0.23] `utils/qbsp2/tjunc.cpp:437-447` ⟵ `utils/qbsp2/tjunc.c:440-450`
- [diverged w=1.00 line=0.76] `dlls/squadmonster.cpp:454-513` ⟵ `dlls/COFSquadTalkMonster.cpp:520-579`
- [diverged w=1.00 line=0.54] `utils/qbsp2/tjunc.cpp:130-191` ⟵ `utils/qbsp2/tjunc.c:131-192`
- [diverged w=1.00 line=0.57] `utils/qbsp2/surfaces.cpp:260-316` ⟵ `utils/qbsp2/surfaces.c:264-322`
- [diverged w=1.00 line=0.57] `utils/qbsp2/surfaces.cpp:103-128` ⟵ `utils/qbsp2/surfaces.c:103-128`
- [diverged w=1.00 line=0.30] `utils/qbsp2/qbsp.cpp:900-933` ⟵ `utils/qbsp2/qbsp.c:897-930`
- [diverged w=1.00 line=0.39] `utils/qbsp2/portals.cpp:405-426` ⟵ `utils/qbsp2/portals.c:412-433`
- [diverged w=1.00 line=0.22] `utils/qbsp2/portals.cpp:381-398` ⟵ `utils/qbsp2/portals.c:388-405`
- [diverged w=1.00 line=0.24] `utils/bspinfo/bspinfo.cpp:17-48` ⟵ `utils/bspinfo/bspinfo.c:17-48`
- [diverged w=1.00 line=0.50] `utils/common/cmdlib.cpp:236-245` ⟵ `utils/common/cmdlib.c:298-307`
- [diverged w=1.00 line=0.43] `utils/qbsp2/tjunc.cpp:478-517` ⟵ `utils/qbsp2/tjunc.c:481-520`
- [diverged w=1.00 line=0.17] `utils/common/cmdlib.cpp:924-931` ⟵ `utils/common/cmdlib.c:979-986`
- [diverged w=1.00 line=0.28] `utils/common/threads.cpp:223-273` ⟵ `utils/common/threads.c:223-274`
- [diverged w=1.00 line=0.48] `utils/common/trilib.cpp:99-186` ⟵ `utils/common/trilib.c:94-174`
- [diverged w=1.00 line=0.50] `utils/qbsp2/portals.cpp:338-370` ⟵ `utils/qbsp2/portals.c:343-377`
- [diverged w=1.00 line=0.40] `utils/mdlviewer/mdlviewer.cpp:159-165` ⟵ `utils/mdlviewer/mdlviewer.cpp:143-149`
- [diverged w=1.00 line=0.54] `utils/mdlviewer/mdlviewer.cpp:199-211` ⟵ `utils/mdlviewer/mdlviewer.cpp:179-191`
- [diverged w=1.00 line=0.49] `utils/qbsp2/portals.cpp:207-251` ⟵ `utils/qbsp2/portals.c:209-254`
- [diverged w=1.00 line=0.30] `utils/mkmovie/mkmovie.cpp:197-234` ⟵ `utils/mkmovie/mkmovie.c:197-234`

## ismobaga/izilang @ `d9b21abdabd6`  (2 hits)
- [diverged w=0.73 line=0.27] `src/parse/lexer.cpp:297-309` ⟵ `src/parse/lexer.cpp:153-158`
- [diverged w=0.60 line=0.67] `src/parse/parser.cpp:319-330` ⟵ `src/parse/parser.cpp:73-80`

## X4V1-code/Xorion @ `7e247080b2be`  (25 hits)
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/HudModule.cpp:81-87` ⟵ `Horion/Module/Modules/HudModule.cpp:80-86`
- [verbatim w=1.00 line=1.00] `Xorion/DrawUtils.cpp:57-109` ⟵ `Horion/DrawUtils.cpp:54-106`
- [verbatim w=1.00 line=1.00] `Xorion/DrawUtils.cpp:584-589` ⟵ `Horion/DrawUtils.cpp:570-575`
- [verbatim w=1.00 line=1.00] `Xorion/DrawUtils.cpp:774-794` ⟵ `Horion/DrawUtils.cpp:760-780`
- [verbatim w=1.00 line=1.00] `Xorion/Menu/ClickGui.cpp:265-278` ⟵ `Horion/Menu/ClickGui.cpp:262-275`
- [verbatim w=1.00 line=1.00] `Xorion/Menu/ClickGui.cpp:414-456` ⟵ `Horion/Menu/ClickGui.cpp:408-450`
- [verbatim w=1.00 line=1.00] `Xorion/Menu/ClickGui.cpp:830-871` ⟵ `Horion/Menu/ClickGui.cpp:821-862`
- [diverged w=1.00 line=0.64] `Xorion/Module/ModuleManager.cpp:345-355` ⟵ `Horion/Module/ModuleManager.cpp:319-329`
- [verbatim w=1.00 line=1.00] `Memory/GameData.cpp:199-208` ⟵ `Memory/GameData.cpp:172-180`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/Compass.cpp:28-39` ⟵ `Horion/Module/Modules/Compass.cpp:28-39`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/ESP.cpp:21-61` ⟵ `Horion/Module/Modules/ESP.cpp:21-61`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/FollowPathModule.cpp:30-46` ⟵ `Horion/Module/Modules/FollowPathModule.cpp:12-28`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/AirSwim.cpp:22-25` ⟵ `Horion/Module/Modules/AirSwim.cpp:21-24`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/HudModule.cpp:127-135` ⟵ `Horion/Module/Modules/HudModule.cpp:126-134`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/InventoryCleaner.cpp:156-168` ⟵ `Horion/Module/Modules/InventoryCleaner.cpp:153-165`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/LigmaNuts.cpp:66-76` ⟵ `Horion/Module/Modules/LigmaNuts.cpp:54-64`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/Scaffold.cpp:198-205` ⟵ `Horion/Module/Modules/Scaffold.cpp:195-202`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/Scaffold.cpp:208-220` ⟵ `Horion/Module/Modules/Scaffold.cpp:205-217`
- [verbatim w=1.00 line=1.00] `Xorion/Module/Modules/StorageESP.cpp:83-90` ⟵ `Horion/Module/Modules/StorageESP.cpp:83-90`
- [near w=1.00 line=0.89] `Xorion/Module/Modules/Tracer.cpp:40-60` ⟵ `Horion/Module/Modules/Tracer.cpp:40-60`
- [verbatim w=1.00 line=1.00] `Xorion/path/JoeMovementController.cpp:27-30` ⟵ `Horion/path/JoeMovementController.cpp:26-29`
- [verbatim w=1.00 line=1.00] `Xorion/path/JoeMovementController.cpp:52-82` ⟵ `Horion/path/JoeMovementController.cpp:51-81`
- [verbatim w=1.00 line=1.00] `Xorion/path/JoeMovementController.cpp:111-147` ⟵ `Horion/path/JoeMovementController.cpp:110-146`
- [near w=1.00 line=0.80] `Xorion/Config/ConfigManager.cpp:46-97` ⟵ `Horion/Config/ConfigManager.cpp:45-96`
- [near w=1.00 line=0.87] `Memory/Hooks.cpp:359-376` ⟵ `Memory/Hooks.cpp:304-321`

## ImagingTools/AcfSln @ `28335c8cbbb3`  (25 hits)
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:2421-2441` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:1906-1926`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1364-1373` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1312-1321`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1341-1359` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1289-1307`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1307-1316` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1255-1264`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1292-1302` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1240-1250`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1125-1150` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1073-1098`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:893-932` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:849-888`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:645-675` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:605-635`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:1287-1300` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:1244-1257`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CVisualRegistryEditorComp.cpp:1446-1455` ⟵ `Include/icmpstr/CVisualRegistryEditorComp.cpp:1388-1397`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:1174-1182` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:1131-1139`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:1125-1159` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:1082-1116`
- [verbatim w=1.00 line=1.00] `Include/iprocgui/CDocumentProcessingManagerCompBase.cpp:127-132` ⟵ `Include/iprocgui/CDocumentProcessingManagerCompBase.cpp:103-108`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CPackageOverviewComp.cpp:863-875` ⟵ `Include/icmpstr/CPackageOverviewComp.cpp:820-832`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:2914-2953` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:2381-2420`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:2357-2414` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:1842-1899`
- [verbatim w=1.00 line=1.00] `Include/icmpstr/CAttributeEditorComp.cpp:586-608` ⟵ `Include/icmpstr/CAttributeEditorComp.cpp:544-566`
- [diverged w=1.00 line=0.11] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:116-121` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:110-115`
- [diverged w=1.00 line=0.11] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:108-113` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:118-123`
- [verbatim w=1.00 line=1.00] `Include/icalibgui/CPerspectiveCalibrationShape.cpp:28-45` ⟵ `Include/icalibgui/CGeneralCalibrationShape.cpp:30-47`
- [verbatim w=1.00 line=1.00] `Include/icalibgui/CGeneralCalibrationShape.cpp:31-48` ⟵ `Include/icalibgui/CPerspectiveCalibrationShape.cpp:30-47`
- [verbatim w=1.00 line=1.00] `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1772-1799` ⟵ `Include/iqtinsp/CInspectionTaskGuiComp.cpp:1137-1164`
- [verbatim w=1.00 line=1.00] `Include/icalib/CPerspectiveCalibration2d.cpp:260-265` ⟵ `Include/icalib/CPerspectiveCalibration2d.cpp:251-256`
- [near w=0.98 line=0.89] `Include/icam/CCalibratedCameraComp.cpp:199-222` ⟵ `Include/icam/CCalibratedCameraComp.cpp:133-156`
- [diverged w=0.93 line=0.77] `Include/iauth/CUsersManagerComp.cpp:168-200` ⟵ `Include/iauth/CSimpleLoginComp.cpp:146-177`

## DarthPineapple/voxel-game @ `87d98b91ffcc`  (2 hits)
- [diverged w=0.80 line=0.45] `src/graphics/vulkan/overlay_pipeline.cpp:209-217` ⟵ `src/graphics/vulkan/pipeline.cpp:67-78`
- [diverged w=0.80 line=0.45] `src/graphics/vulkan/pipeline.cpp:244-252` ⟵ `src/graphics/vulkan/pipeline.cpp:67-78`

## hyzboy/CMMath @ `43d3d4286b31`  (5 hits)
- [verbatim w=1.00 line=1.00] `inc/hgl/math/transform/Transform.h:160-171` ⟵ `inc/hgl/math/Transform.h:808-819`
- [verbatim w=1.00 line=1.00] `inc/hgl/math/transform/TransformAction.h:648-659` ⟵ `inc/hgl/math/Transform.h:632-643`
- [verbatim w=1.00 line=1.00] `src/Geometry/AABB.cpp:47-67` ⟵ `src/graph/Bounding/AABB.cpp:42-62`
- [verbatim w=1.00 line=1.00] `src/Geometry/Frustum.cpp:101-106` ⟵ `src/graph/Frustum.cpp:95-100`
- [diverged w=0.70 line=0.44] `src/Math/Matrix4f.cpp:400-436` ⟵ `src/Math/Matrix4f.cpp:190-218`

## michael-haufschild-gib/MultiScoper @ `8b898827c6e3`  (25 hits)
- [verbatim w=1.00 line=1.00] `tests/test_source_state.cpp:180-186` ⟵ `tests/test_source.cpp:330-336`
- [verbatim w=1.00 line=1.00] `tests/test_source_state.cpp:164-176` ⟵ `tests/test_source.cpp:314-326`
- [diverged w=1.00 line=0.71] `test_harness/src/Main.cpp:33-38` ⟵ `test_harness/src/Main.cpp:27-32`
- [verbatim w=1.00 line=1.00] `test_harness/src/TestHttpServerTransport.cpp:121-128` ⟵ `test_harness/src/TestHttpServer.cpp:213-220`
- [verbatim w=1.00 line=1.00] `src/ui/layout/PaneComponent.cpp:206-221` ⟵ `src/ui/PaneComponent.cpp:178-193`
- [verbatim w=1.00 line=1.00] `src/ui/panels/OscillatorListItemPainting.cpp:112-121` ⟵ `src/ui/OscillatorListItem.cpp:412-421`
- [diverged w=0.99 line=0.62] `src/ui/theme/ThemeEditorWidgets.cpp:124-133` ⟵ `src/ui/ThemeEditorComponent.cpp:140-150`
- [verbatim w=0.99 line=1.00] `test_harness/src/TestHttpServerVerification.cpp:32-66` ⟵ `test_harness/src/TestHttpServer.cpp:479-515`
- [diverged w=0.98 line=0.38] `src/ui/theme/ThemeEditorWidgets.cpp:57-73` ⟵ `src/ui/ThemeEditorComponent.cpp:66-88`
- [diverged w=0.94 line=0.36] `src/ui/theme/ThemeEditorActions.cpp:257-287` ⟵ `src/ui/ThemeEditorComponent.cpp:620-652`
- [diverged w=0.93 line=0.39] `src/ui/theme/ThemeEditorActions.cpp:229-253` ⟵ `src/ui/ThemeEditorComponent.cpp:590-616`
- [diverged w=0.89 line=0.14] `tests/test_source_buffer.cpp:39-67` ⟵ `tests/test_source.cpp:226-255`
- [diverged w=0.81 line=0.32] `src/core/dsp/SignalProcessor.cpp:50-82` ⟵ `src/dsp/SignalProcessor.cpp:20-43`
- [diverged w=0.78 line=0.05] `src/ui/dialogs/OscillatorConfigDialog.cpp:289-312` ⟵ `src/ui/OscillatorConfigPopup.cpp:391-412`
- [diverged w=0.74 line=0.21] `test_harness/src/TestHttpServerTransport.cpp:185-209` ⟵ `test_harness/src/TestHttpServer.cpp:286-313`
- [diverged w=0.74 line=0.75] `tests/test_capture_buffer_core.cpp:154-172` ⟵ `tests/test_shared_capture_buffer.cpp:162-180`
- [diverged w=0.73 line=0.55] `test_harness/src/TestHttpServerTransport.cpp:160-181` ⟵ `test_harness/src/TestHttpServer.cpp:255-282`
- [diverged w=0.72 line=0.22] `src/core/dsp/TimingEngine.cpp:174-208` ⟵ `src/dsp/TimingEngine.cpp:123-155`
- [diverged w=0.72 line=0.57] `src/ui/theme/ThemeEditorComponent.cpp:256-283` ⟵ `src/ui/ThemeEditorComponent.cpp:399-426`
- [diverged w=0.69 line=0.54] `test_harness/src/TestUIControllerSimulation.cpp:341-387` ⟵ `test_harness/src/TestUIController.cpp:315-367`
- [diverged w=0.69 line=0.48] `src/tools/test_server/ScreenshotHandler.cpp:16-68` ⟵ `src/ui/PluginTestServer.cpp:596-639`
- [diverged w=0.69 line=0.21] `src/ui/theme/ThemeEditorActions.cpp:175-197` ⟵ `src/ui/ThemeEditorComponent.cpp:522-554`
- [diverged w=0.67 line=0.13] `src/ui/theme/ThemeEditorActions.cpp:154-171` ⟵ `src/ui/ThemeEditorComponent.cpp:490-518`
- [diverged w=0.66 line=0.32] `src/ui/panels/SourceSelectorComponent.cpp:92-125` ⟵ `src/ui/SourceSelectorComponent.cpp:493-524`
- [diverged w=0.65 line=0.67] `test_harness/src/TestHttpServerUI.cpp:181-216` ⟵ `test_harness/src/TestHttpServer.cpp:362-392`

## aapokaapo/JoyShockMapper-systemd @ `5f5845cd90ce`  (1 hits)
- [diverged w=0.70 line=0.27] `JoyShockMapper/src/win32/InputHelpers.cpp:237-244` ⟵ `JoyShockMapper/include/inputHelpers.cpp:48-53`

## jjbudz/6502 @ `0877729248bf`  (4 hits)
- [diverged w=0.94 line=0.50] `l6502.cpp:2762-2773` ⟵ `6502.cpp:2538-2547`
- [diverged w=0.92 line=0.26] `l6502.cpp:3239-3255` ⟵ `6502.cpp:3218-3232`
- [diverged w=0.77 line=0.50] `l6502.cpp:2744-2755` ⟵ `6502.cpp:2538-2547`
- [diverged w=0.75 line=0.42] `l6502.cpp:3279-3317` ⟵ `6502.cpp:2909-2938`

## flyingsurveyor/RTKino @ `fa2b1c45a627`  (12 hits)
- [near w=1.00 line=0.82] `src/main.cpp:2445-2454` ⟵ `src/main.cpp:2169-2178`
- [near w=1.00 line=0.86] `src/main.cpp:1581-1593` ⟵ `src/main.cpp:1308-1320`
- [near w=1.00 line=0.91] `src/main.cpp:1453-1477` ⟵ `src/main.cpp:1183-1207`
- [near w=0.95 line=0.92] `src/main.cpp:2047-2067` ⟵ `src/main.cpp:1772-1791`
- [diverged w=0.94 line=0.79] `src/main.cpp:2705-2767` ⟵ `src/main.cpp:2406-2468`
- [near w=0.93 line=0.94] `src/main.cpp:2914-2955` ⟵ `src/main.cpp:2536-2575`
- [near w=0.92 line=0.83] `src/main.cpp:1034-1073` ⟵ `src/main.cpp:990-1028`
- [near w=0.90 line=0.81] `src/main.cpp:1813-1873` ⟵ `src/main.cpp:1537-1597`
- [diverged w=0.89 line=0.70] `lib/WebUI/WebUI.cpp:4040-4114` ⟵ `lib/WebUI/WebUI.cpp:3360-3431`
- [near w=0.89 line=0.80] `src/main.cpp:1655-1671` ⟵ `src/main.cpp:1383-1395`
- [near w=0.89 line=0.89] `src/main.cpp:1599-1632` ⟵ `src/main.cpp:1326-1360`
- [diverged w=0.71 line=0.69] `src/main.cpp:1116-1132` ⟵ `src/main.cpp:1073-1085`

## emeryberger/Hoard @ `87e973255b0d`  (2 hits)
- [near w=0.96 line=0.88] `src/include/hoard/thresholdheap.h:183-200` ⟵ `src/include/hoard/thresholdheap.h:205-222`
- [diverged w=0.69 line=0.57] `benchmarks/cache-thrash/cache-thrash.cpp:101-139` ⟵ `benchmarks/cache-scratch/cache-scratch.cpp:101-146`

## brunusansi/NanaBox @ `ce774438b088`  (12 hits)
- [verbatim w=1.00 line=1.00] `NanaBox/ConfigurationManager.cpp:723-832` ⟵ `NanaBox/ConfigurationManager.cpp:126-235`
- [verbatim w=1.00 line=1.00] `NanaBox/ConfigurationManager.cpp:965-982` ⟵ `NanaBox/ConfigurationManager.cpp:368-385`
- [verbatim w=1.00 line=1.00] `NanaBox/ConfigurationManager.cpp:994-1010` ⟵ `NanaBox/ConfigurationManager.cpp:397-413`
- [verbatim w=1.00 line=1.00] `NanaBox/MainWindow.cpp:257-298` ⟵ `NanaBox/NanaBox.cpp:566-607`
- [near w=1.00 line=0.89] `NanaBox/SponsorPage.cpp:28-53` ⟵ `NanaBox/QuickStartPage.cpp:20-45`
- [near w=1.00 line=0.92] `NanaBox/NanaBox.cpp:360-405` ⟵ `NanaBox/NanaBox.cpp:1114-1159`
- [diverged w=0.97 line=0.35] `NanaBox/MainWindow.cpp:1174-1214` ⟵ `NanaBox/NanaBox.cpp:410-450`
- [diverged w=0.91 line=0.62] `NanaBox/MainWindow.cpp:405-499` ⟵ `NanaBox/NanaBox.cpp:710-789`
- [near w=0.91 line=0.85] `NanaBox/ResizeVirtualHardDiskPage.cpp:43-60` ⟵ `NanaBox/NewVirtualHardDiskPage.cpp:37-51`
- [near w=0.90 line=0.82] `NanaBox/MainWindow.cpp:304-378` ⟵ `NanaBox/NanaBox.cpp:613-683`
- [diverged w=0.72 line=0.45] `NanaBox/MainWindow.cpp:1033-1096` ⟵ `NanaBox/NanaBox.cpp:311-350`
- [diverged w=0.62 line=0.32] `NanaBox/ConfigurationManager.cpp:504-538` ⟵ `NanaBox/ConfigurationManager.cpp:55-120`

## bradhawkins85/ESP32-Uptime-Monitoring-Touch @ `b0302856b4fc`  (7 hits)
- [near w=0.91 line=0.95] `src/main.cpp:550-578` ⟵ `src/main.cpp:227-251`
- [near w=0.87 line=0.88] `src/main.cpp:299-322` ⟵ `src/main.cpp:64-84`
- [near w=0.84 line=0.89] `src/main.cpp:325-337` ⟵ `src/main.cpp:87-97`
- [diverged w=0.83 line=0.79] `src/main.cpp:1234-1262` ⟵ `src/main.cpp:388-412`
- [near w=0.83 line=0.88] `src/main.cpp:1265-1308` ⟵ `src/main.cpp:415-454`
- [near w=0.82 line=0.82] `src/main.cpp:478-545` ⟵ `src/main.cpp:170-222`
- [near w=0.78 line=0.86] `src/main.cpp:1321-1932` ⟵ `src/main.cpp:467-979`

## Josh-Archer/ESP32-Smart-Monitor @ `87eebf6fe124`  (1 hits)
- [diverged w=0.63 line=0.51] `src/main.cpp:168-233` ⟵ `src/main.cpp:96-139`

## UofUEpiBio/epiworld @ `c966f02f791b`  (7 hits)
- [verbatim w=1.00 line=1.00] `epiworld.hpp:3532-3549` ⟵ `epiworld.hpp:2299-2316`
- [verbatim w=1.00 line=1.00] `epiworld.hpp:7715-7723` ⟵ `epiworld.hpp:5116-5124`
- [verbatim w=1.00 line=1.00] `include/epiworld/randgraph.hpp:624-632` ⟵ `epiworld.hpp:5116-5124`
- [diverged w=0.93 line=0.51] `epiworld.hpp:1199-1244` ⟵ `epiworld.hpp:8364-8404`
- [diverged w=0.93 line=0.51] `include/epiworld/misc.hpp:285-330` ⟵ `epiworld.hpp:8364-8404`
- [diverged w=0.67 line=0.41] `epiworld.hpp:12883-12944` ⟵ `epiworld.hpp:7865-7926`
- [diverged w=0.67 line=0.41] `include/epiworld/model-meat.hpp:1863-1924` ⟵ `epiworld.hpp:7865-7926`

## syoyo/tinyexr @ `4946b5d92e13`  (14 hits)
- [diverged w=1.00 line=0.50] `deps/miniz/miniz.c:199-231` ⟵ `tinyexr.h:1691-1727`
- [diverged w=1.00 line=0.49] `deps/miniz/miniz.c:244-298` ⟵ `tinyexr.h:1740-1792`
- [diverged w=1.00 line=0.30] `deps/miniz/miniz.c:1018-1042` ⟵ `tinyexr.h:3121-3139`
- [diverged w=1.00 line=0.56] `deps/miniz/miniz.c:1839-1861` ⟵ `tinyexr.h:3903-3919`
- [diverged w=1.00 line=0.48] `deps/miniz/miniz.c:3451-3467` ⟵ `tinyexr.h:4571-4587`
- [diverged w=1.00 line=0.71] `deps/miniz/miniz.c:321-347` ⟵ `tinyexr.h:1814-1839`
- [near w=0.98 line=0.82] `tinyexr.h:2605-2650` ⟵ `tinyexr.h:7872-7919`
- [diverged w=0.95 line=0.31] `deps/miniz/miniz.c:2649-2674` ⟵ `tinyexr.h:2434-2460`
- [near w=0.94 line=0.82] `tinyexr.h:3104-3159` ⟵ `tinyexr.h:8330-8386`
- [diverged w=0.94 line=0.54] `deps/miniz/miniz.c:564-591` ⟵ `tinyexr.h:2035-2061`
- [diverged w=0.91 line=0.40] `deps/miniz/miniz.c:2503-2542` ⟵ `tinyexr.h:2305-2345`
- [diverged w=0.90 line=0.22] `deps/miniz/miniz.c:3571-3619` ⟵ `tinyexr.h:4691-4736`
- [diverged w=0.83 line=0.46] `tinyexr.h:3074-3100` ⟵ `tinyexr.h:8299-8326`
- [diverged w=0.73 line=0.13] `deps/miniz/miniz.c:6150-6181` ⟵ `tinyexr.h:5941-5976`

## supaplextor/aqemu @ `439043b0880b`  (25 hits)
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2756-2781` ⟵ `Main_Window.cpp:4090-4115`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:3918-3924` ⟵ `VM_Wizard_Window.cpp:751-757`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:3928-3985` ⟵ `Main_Window.cpp:5212-5269`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2871-2902` ⟵ `Main_Window.cpp:4188-4219`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2837-2849` ⟵ `Main_Window.cpp:4171-4183`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2820-2832` ⟵ `Main_Window.cpp:4154-4166`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2803-2815` ⟵ `Main_Window.cpp:4137-4149`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2786-2798` ⟵ `Main_Window.cpp:4120-4132`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2549-2584` ⟵ `Main_Window.cpp:3850-3885`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2519-2525` ⟵ `Main_Window.cpp:3820-3826`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:2012-2053` ⟵ `Main_Window.cpp:3428-3469`
- [verbatim w=1.00 line=1.00] `src/Advanced_Settings_Window.cpp:828-844` ⟵ `Advanced_Settings_Window.cpp:513-529`
- [verbatim w=1.00 line=1.00] `src/Advanced_Settings_Window.cpp:865-885` ⟵ `Advanced_Settings_Window.cpp:554-574`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:844-857` ⟵ `Main_Window.cpp:721-734`
- [verbatim w=1.00 line=1.00] `src/Main_Window.cpp:4622-4625` ⟵ `Main_Window.cpp:6451-6454`
- [diverged w=1.00 line=0.79] `src/Main_Window.cpp:3683-3719` ⟵ `Main_Window.cpp:4981-5017`
- [near w=0.99 line=0.81] `src/Main_Window.cpp:4473-4505` ⟵ `Main_Window.cpp:6299-6331`
- [verbatim w=0.96 line=0.97] `src/Main_Window.cpp:2326-2395` ⟵ `Main_Window.cpp:3628-3696`
- [diverged w=0.96 line=0.75] `src/Advanced_Settings_Window.cpp:356-365` ⟵ `Settings_Window.cpp:192-201`
- [diverged w=0.92 line=0.45] `src/Main_Window.cpp:4443-4469` ⟵ `Main_Window.cpp:6269-6295`
- [near w=0.90 line=0.85] `src/Main_Window.cpp:1260-1303` ⟵ `Main_Window.cpp:1229-1271`
- [diverged w=0.90 line=0.53] `src/Main_Window.cpp:3222-3261` ⟵ `Main_Window.cpp:4666-4702`
- [near w=0.88 line=0.87] `src/Main_Window.cpp:1227-1247` ⟵ `Main_Window.cpp:1197-1214`
- [diverged w=0.87 line=0.56] `src/main.cpp:295-357` ⟵ `main.cpp:141-195`
- [diverged w=0.86 line=0.46] `src/Embedded_Display/vncclientthread.cpp:331-343` ⟵ `Embedded_Display/vncclientthread.cpp:161-168`

## mi0e/subconverter @ `808bd96d22d6`  (25 hits)
- [diverged w=1.00 line=0.25] `include/inja.hpp:298-303` ⟵ `include/inja.hpp:1856-1861`
- [diverged w=1.00 line=0.33] `src/handler/interfaces.cpp:725-729` ⟵ `src/handler/interfaces.cpp:1726-1730`
- [verbatim w=1.00 line=1.00] `include/nlohmann/json.hpp:11485-11493` ⟵ `include/nlohmann/json.hpp:8441-8449`
- [diverged w=1.00 line=0.78] `include/nlohmann/json.hpp:16048-16059` ⟵ `include/nlohmann/json.hpp:13802-13813`
- [diverged w=1.00 line=0.71] `include/nlohmann/json.hpp:16191-16199` ⟵ `include/nlohmann/json.hpp:13944-13952`
- [near w=1.00 line=0.93] `include/nlohmann/json.hpp:17673-17702` ⟵ `include/nlohmann/json.hpp:15224-15253`
- [verbatim w=1.00 line=1.00] `include/inja.hpp:292-295` ⟵ `include/inja.hpp:1850-1853`
- [diverged w=1.00 line=0.55] `src/parser/subparser.cpp:653-661` ⟵ `src/parser/subparser.cpp:396-404`
- [verbatim w=1.00 line=1.00] `src/utils/lock.h:65-76` ⟵ `src/handler/webget.cpp:82-93`
- [diverged w=1.00 line=0.17] `include/inja.hpp:2889-2893` ⟵ `include/inja.hpp:4124-4125`
- [diverged w=1.00 line=0.67] `include/inja.hpp:2042-2047` ⟵ `include/inja.hpp:3307-3312`
- [near w=1.00 line=0.87] `include/quickjspp.hpp:1550-1565` ⟵ `include/quickjspp.hpp:971-986`
- [near w=1.00 line=0.86] `include/quickjspp.hpp:1386-1398` ⟵ `include/quickjspp.hpp:866-878`
- [near w=1.00 line=0.85] `include/quickjspp.hpp:1404-1415` ⟵ `include/quickjspp.hpp:884-895`
- [near w=1.00 line=0.82] `include/quickjspp.hpp:104-118` ⟵ `include/quickjspp.hpp:69-83`
- [diverged w=0.99 line=0.60] `src/handler/settings.cpp:470-478` ⟵ `src/handler/interfaces.cpp:774-782`
- [diverged w=0.99 line=0.78] `src/generator/config/nodemanip.cpp:470-517` ⟵ `src/generator/config/subexport.cpp:245-292`
- [diverged w=0.99 line=0.50] `include/nlohmann/json.hpp:15845-15850` ⟵ `include/nlohmann/json.hpp:13600-13605`
- [diverged w=0.99 line=0.67] `include/inja.hpp:2793-2797` ⟵ `include/inja.hpp:4031-4035`
- [verbatim w=0.98 line=0.96] `include/nlohmann/json.hpp:16974-17037` ⟵ `include/nlohmann/json.hpp:14523-14586`
- [near w=0.97 line=0.94] `include/nlohmann/json.hpp:10083-10168` ⟵ `include/nlohmann/json.hpp:8584-8668`
- [diverged w=0.97 line=0.14] `include/quickjspp.hpp:618-621` ⟵ `include/quickjspp.hpp:308-311`
- [diverged w=0.97 line=0.52] `src/generator/config/subexport.cpp:2035-2054` ⟵ `src/generator/config/subexport.cpp:1658-1674`
- [diverged w=0.97 line=0.25] `include/inja.hpp:2256-2273` ⟵ `include/inja.hpp:3487-3507`
- [diverged w=0.96 line=0.56] `include/inja.hpp:2639-2646` ⟵ `include/inja.hpp:3903-3911`

## shelbeely/fallout1-ce @ `9220f783d088`  (3 hits)
- [diverged w=1.00 line=0.67] `src/game/inventry.cc:4622-4633` ⟵ `src/game/inventry.cc:4542-4553`
- [diverged w=0.67 line=0.61] `src/plib/db/db.cc:2565-2604` ⟵ `src/plib/db/db.cc:2607-2634`
- [diverged w=0.60 line=0.75] `src/game/scripts.cc:909-970` ⟵ `src/game/scripts.cc:811-876`

## lwqwag/PclCSharp @ `ab87843f1ecc`  (16 hits)
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:73-100` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:75-102`
- [near w=1.00 line=0.81] `src/PclSharpWrapper/PclSharpWrapper/util_module.cpp:341-366` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:675-700`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/util_module.cpp:29-45` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:363-379`
- [diverged w=1.00 line=0.60] `src/PclSharpWrapper/PclSharpWrapper/util_module.cpp:15-23` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:349-357`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/segmentation.cpp:69-98` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:288-317`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/segmentation.cpp:18-42` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:237-261`
- [diverged w=1.00 line=0.74] `src/PclSharpWrapper/PclSharpWrapper/sampleConsensus.cpp:15-36` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:322-343`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/filter.cpp:48-64` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:186-202`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:42-60` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:44-62`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:33-38` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:35-40`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/pcl_util.cpp:15-29` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:17-31`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/io.cpp:112-121` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:158-167`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/io.cpp:99-108` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:145-154`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/io.cpp:16-26` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:106-116`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/filter.cpp:85-90` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:223-228`
- [verbatim w=1.00 line=1.00] `src/PclSharpWrapper/PclSharpWrapper/filter.cpp:70-76` ⟵ `src/PclSharpWrapper/PclSharpWrapper/PclDll.cpp:208-214`

## cmcxn/downloader @ `f09d15634172`  (4 hits)
- [diverged w=0.85 line=0.68] `cpp_xl_dl_demo/http.h:88-112` ⟵ `cpp_xl_dl_demo/http.h:58-85`
- [diverged w=0.76 line=0.26] `cpp_xl_dl_demo/download_helper.cpp:149-160` ⟵ `cpp_xl_dl_demo/main.cpp:33-52`
- [diverged w=0.73 line=0.26] `cpp_xl_dl_demo/download_helper.cpp:135-146` ⟵ `cpp_xl_dl_demo/main.cpp:33-52`
- [diverged w=0.64 line=0.75] `cpp_xl_dl_demo/http.h:58-85` ⟵ `cpp_xl_dl_demo/http.h:21-53`

## deramatamara-lab/cppmusic @ `321928f611c0`  (15 hits)
- [verbatim w=1.00 line=1.00] `src/audio/engine/DawEngine.cpp:393-415` ⟵ `src/audio/engine/DawEngine.cpp:285-307`
- [verbatim w=1.00 line=1.00] `src/plugins/PluginHost.cpp:119-135` ⟵ `src/plugins/PluginHost.cpp:68-84`
- [verbatim w=1.00 line=1.00] `src/ui/App.cpp:80-113` ⟵ `src/ui/App.cpp:45-78`
- [verbatim w=1.00 line=1.00] `src/ui/MainWindow.cpp:370-387` ⟵ `src/ui/MainWindow.cpp:83-100`
- [verbatim w=1.00 line=1.00] `src/ui/MainWindow.cpp:391-403` ⟵ `src/ui/MainWindow.cpp:104-116`
- [verbatim w=1.00 line=1.00] `src/ui/views/PianoRollView.cpp:270-286` ⟵ `src/ui/views/PianoRollView.cpp:179-195`
- [verbatim w=1.00 line=1.00] `src/ui/views/TransportBar.cpp:500-519` ⟵ `src/ui/views/TransportBar.cpp:182-201`
- [diverged w=0.87 line=0.70] `src/ui/views/MixerStrip.cpp:262-274` ⟵ `src/ui/views/MixerStrip.cpp:169-177`
- [diverged w=0.87 line=0.70] `src/ui/views/MixerStrip.cpp:246-258` ⟵ `src/ui/views/MixerStrip.cpp:157-165`
- [diverged w=0.82 line=0.70] `src/ui/views/MixerStrip.cpp:99-135` ⟵ `src/ui/views/MixerStrip.cpp:62-90`
- [diverged w=0.80 line=0.58] `src/ai/inference/InferenceEngine.cpp:69-93` ⟵ `src/ai/inference/InferenceEngine.cpp:45-64`
- [diverged w=0.73 line=0.60] `src/ui/components/StepSequencer.cpp:89-98` ⟵ `src/ui/components/StepSequencer.cpp:73-80`
- [diverged w=0.70 line=0.62] `src/ui/views/ArrangeView.cpp:898-961` ⟵ `src/ui/views/ArrangeView.cpp:191-244`
- [diverged w=0.67 line=0.52] `src/core/utilities/Logger.cpp:63-95` ⟵ `src/core/utilities/Logger.cpp:12-29`
- [diverged w=0.66 line=0.58] `src/ui/views/PianoRollView.cpp:192-204` ⟵ `src/ui/views/PianoRollView.cpp:138-149`

## asklar/lvt @ `f6d446e7507b`  (1 hits)
- [diverged w=0.78 line=0.71] `src/target.cpp:92-97` ⟵ `src/providers/win32_provider.cpp:22-27`

## albpara/DMDDelorean @ `b9bd7a5dbe9a`  (2 hits)
- [diverged w=0.76 line=0.54] `src/main.cpp:912-936` ⟵ `src/main.cpp:392-414`
- [diverged w=0.71 line=0.45] `src/main.cpp:887-906` ⟵ `src/main.cpp:369-386`

## Autonomy-Logic/matiec @ `75d3755f0ad5`  (21 hits)
- [verbatim w=1.00 line=1.00] `stage4/generate_c/generate_c_il.cc:657-684` ⟵ `stage4/generate_c/generate_c_st.cc:274-301`
- [verbatim w=1.00 line=1.00] `stage4/generate_c/generate_var_list.cc:975-999` ⟵ `stage4/generate_c/generate_var_list.cc:653-677`
- [verbatim w=1.00 line=1.00] `stage4/generate_c/generate_c_vardecl.cc:1206-1219` ⟵ `stage4/generate_c/generate_c_vardecl.cc:903-916`
- [verbatim w=1.00 line=1.00] `lib/iec_std_functions.h:1045-1067` ⟵ `lib/iec_std_lib.h:1604-1626`
- [diverged w=1.00 line=0.75] `stage3/constant_folding.cc:369-401` ⟵ `stage3/constant_folding.cc:333-365`
- [diverged w=0.95 line=0.35] `stage4/generate_c/generate_c_typedecl.cc:455-492` ⟵ `stage4/generate_c/generate_c_typedecl.cc:241-277`
- [near w=0.93 line=0.84] `stage4/generate_c/generate_c.cc:1933-1994` ⟵ `stage4/generate_c/generate_c.cc:2029-2090`
- [verbatim w=0.91 line=0.95] `stage3/fill_candidate_datatypes.cc:497-549` ⟵ `stage3/fill_candidate_datatypes.cc:295-347`
- [diverged w=0.91 line=0.79] `stage4/generate_c/generate_c_vardecl.cc:887-931` ⟵ `stage4/generate_c/generate_c_vardecl.cc:598-640`
- [diverged w=0.91 line=0.71] `stage4/generate_c/generate_c_sfcdecl.cc:303-342` ⟵ `stage4/generate_c/generate_c_sfcdecl.cc:239-277`
- [near w=0.84 line=0.87] `lib/iec_std_lib.h:618-649` ⟵ `lib/iec_std_lib.h:458-489`
- [diverged w=0.82 line=0.78] `stage4/generate_c/generate_c_vardecl.cc:2240-2306` ⟵ `stage4/generate_c/generate_c_vardecl.cc:1830-1887`
- [diverged w=0.82 line=0.44] `stage3/narrow_candidate_datatypes.cc:1704-1723` ⟵ `stage3/narrow_candidate_datatypes.cc:1208-1216`
- [near w=0.80 line=0.80] `stage4/generate_c/generate_c_st.cc:652-680` ⟵ `stage4/generate_c/generate_c_il.cc:523-550`
- [diverged w=0.77 line=0.40] `stage3/lvalue_check.cc:274-281` ⟵ `stage3/lvalue_check.cc:247-252`
- [diverged w=0.76 line=0.23] `stage4/generate_c/generate_c_typedecl.cc:402-450` ⟵ `stage4/generate_c/generate_c_typedecl.cc:195-237`
- [diverged w=0.76 line=0.57] `absyntax_utils/function_param_iterator.cc:167-205` ⟵ `absyntax_utils/function_param_iterator.cc:208-241`
- [diverged w=0.74 line=0.20] `absyntax_utils/get_datatype_info.cc:407-412` ⟵ `absyntax_utils/search_varfb_instance_type.cc:381-396`
- [diverged w=0.68 line=0.48] `stage4/generate_c/generate_c.cc:2589-2617` ⟵ `stage4/generate_c/generate_c.cc:2355-2379`
- [diverged w=0.64 line=0.66] `stage4/generate_c/generate_c.cc:2708-2755` ⟵ `stage4/generate_c/generate_c.cc:2466-2516`
- [diverged w=0.61 line=0.48] `stage4/generate_c/generate_c_sfc.cc:416-438` ⟵ `stage4/generate_c/generate_c_sfc.cc:459-485`

## TheOriginalBytePlayer/ozz-animationCal3DWrapper @ `280722b79f6e`  (25 hits)
- [verbatim w=1.00 line=1.00] `test/options/options_tests.cc:472-504` ⟵ `test/options/options_tests.cc:435-467`
- [diverged w=1.00 line=0.50] `test/options/options_registration_tests.cc:122-131` ⟵ `test/options/options_registration_empty_tests.cc:75-83`
- [diverged w=1.00 line=0.50] `test/options/options_registration_empty_tests.cc:75-84` ⟵ `test/options/options_registration_tests.cc:119-127`
- [diverged w=1.00 line=0.50] `src/animation/offline/skeleton_builder.cc:126-150` ⟵ `src/animation/offline/skeleton_builder.cc:158-181`
- [verbatim w=1.00 line=1.00] `samples/framework/internal/shader.cc:65-76` ⟵ `demo/framework/internal/renderer_impl.cc:628-639`
- [diverged w=0.95 line=0.53] `samples/framework/internal/imgui_impl.cc:1455-1474` ⟵ `demo/framework/internal/imgui_impl.cc:1235-1253`
- [diverged w=0.89 line=0.67] `cal3d-src/src/cal3d/tinyxmlparser.cpp:392-437` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:650-700`
- [diverged w=0.89 line=0.50] `samples/framework/internal/camera.cc:178-200` ⟵ `demo/framework/internal/camera.cc:86-102`
- [near w=0.85 line=0.86] `cal3d-src/src/cal3d/tinyxml.cpp:464-470` ⟵ `contrib/tinyxml/tinyxml.cpp:567-574`
- [near w=0.83 line=0.85] `test/base/maths/simd_int_math_tests.cc:372-389` ⟵ `test/base/maths/simd_int_math_tests.cc:287-301`
- [diverged w=0.82 line=0.61] `samples/framework/internal/shader.cc:80-103` ⟵ `demo/framework/internal/renderer_impl.cc:685-707`
- [diverged w=0.82 line=0.46] `cal3d-src/src/cal3d/tinyxmlparser.cpp:818-872` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:1181-1247`
- [diverged w=0.81 line=0.57] `include/ozz/base/maths/internal/simd_math_sse-inl.h:1582-1601` ⟵ `include/ozz/base/maths/internal/simd_math_sse-inl.h:1435-1455`
- [diverged w=0.81 line=0.57] `include/ozz/base/maths/internal/simd_math_sse-inl.h:1562-1579` ⟵ `include/ozz/base/maths/internal/simd_math_sse-inl.h:1435-1455`
- [diverged w=0.79 line=0.67] `include/ozz/base/maths/internal/simd_math_sse-inl.h:1058-1062` ⟵ `include/ozz/base/maths/internal/simd_math_sse-inl.h:974-978`
- [diverged w=0.79 line=0.08] `src/animation/runtime/sampling_job.cc:432-453` ⟵ `src/animation/sampling_job.cc:294-315`
- [diverged w=0.76 line=0.50] `include/ozz/base/containers/intrusive_list.h:673-676` ⟵ `include/ozz/base/containers/intrusive_list.h:689-692`
- [diverged w=0.73 line=0.28] `samples/framework/internal/shader.cc:109-148` ⟵ `demo/framework/internal/renderer_impl.cc:644-682`
- [diverged w=0.72 line=0.47] `cal3d-src/src/cal3d/tinyxmlparser.cpp:739-811` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:1085-1174`
- [diverged w=0.70 line=0.34] `test/base/maths/vec_float_tests.cc:289-317` ⟵ `test/base/maths/vec_float_tests.cc:332-359`
- [diverged w=0.70 line=0.26] `samples/framework/internal/imgui_impl.cc:560-627` ⟵ `demo/framework/internal/imgui_impl.cc:541-616`
- [diverged w=0.67 line=0.60] `cal3d-src/src/cal3d/tinyxmlparser.cpp:595-692` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:941-1039`
- [diverged w=0.66 line=0.32] `samples/framework/application.cc:605-625` ⟵ `demo/framework/application.cc:408-422`
- [diverged w=0.63 line=0.57] `cal3d-src/src/cal3d/tinyxmlparser.cpp:520-587` ⟵ `contrib/tinyxml/tinyxmlparser.cpp:820-899`
- [diverged w=0.62 line=0.23] `samples/framework/application.cc:461-528` ⟵ `demo/framework/application.cc:281-340`

## OzCog/cogutil @ `f6dde8958c3d`  (12 hits)
- [verbatim w=1.00 line=1.00] `opencog/util/algorithm.h:158-161` ⟵ `opencog/util/algorithm.h:165-168`
- [diverged w=1.00 line=0.20] `opencog/util/backtrace-symbols.c:88-97` ⟵ `opencog/util/backtrace-symbols.c:112-125`
- [near w=1.00 line=0.85] `opencog/util/numeric.h:384-397` ⟵ `opencog/util/numeric.h:475-488`
- [diverged w=1.00 line=0.71] `opencog/util/random.h:104-116` ⟵ `opencog/util/random.h:82-94`
- [diverged w=0.98 line=0.45] `opencog/util/backtrace-symbols.c:220-252` ⟵ `opencog/util/backtrace-symbols.c:270-303`
- [diverged w=0.90 line=0.62] `opencog/util/backtrace-symbols.c:288-335` ⟵ `opencog/util/backtrace-symbols.c:340-383`
- [diverged w=0.89 line=0.60] `opencog/util/algorithm.h:267-270` ⟵ `opencog/util/algorithm.h:253-256`
- [diverged w=0.89 line=0.50] `opencog/util/numeric.h:420-441` ⟵ `opencog/util/numeric.h:511-527`
- [diverged w=0.75 line=0.50] `opencog/util/async_buffer.h:324-363` ⟵ `opencog/util/async_method_caller.h:165-197`
- [near w=0.75 line=0.82] `opencog/util/backtrace-symbols.c:339-352` ⟵ `opencog/util/backtrace-symbols.c:388-401`
- [diverged w=0.70 line=0.60] `opencog/util/Logger.cc:493-555` ⟵ `opencog/util/Logger.cc:403-454`
- [diverged w=0.63 line=0.63] `opencog/util/async_method_caller.h:246-282` ⟵ `opencog/util/async_method_caller.h:165-197`

## Harsha-Bhattacharyya/auh @ `c81caf016551`  (2 hits)
- [diverged w=1.00 line=0.40] `src/main.cpp:77-88` ⟵ `src/main.cpp:12-20`
- [diverged w=0.69 line=0.17] `src/main.cpp:149-217` ⟵ `src/main.cpp:28-60`

## shadow11001/esphome @ `bc8c6ffa1b57`  (16 hits)
- [diverged w=0.98 line=0.75] `esphome/components/midea/appliance_base.h:59-65` ⟵ `esphome/components/midea/appliance_base.h:32-38`
- [near w=0.95 line=0.90] `esphome/components/wifi/wifi_component_esp_idf.cpp:343-382` ⟵ `esphome/components/wifi/wifi_component_esp32_arduino.cpp:234-274`
- [near w=0.88 line=0.88] `esphome/components/api/api_connection.cpp:783-801` ⟵ `esphome/components/api/api_connection.cpp:746-761`
- [diverged w=0.87 line=0.75] `esphome/components/shutdown/button/shutdown_button.cpp:20-30` ⟵ `esphome/components/shutdown/shutdown_switch.cpp:21-35`
- [diverged w=0.86 line=0.55] `esphome/components/dsmr/dsmr.cpp:256-272` ⟵ `esphome/components/dsmr/dsmr.cpp:146-161`
- [diverged w=0.85 line=0.33] `esphome/components/ledc/ledc_output.cpp:41-51` ⟵ `esphome/components/ledc/ledc_output.cpp:24-31`
- [diverged w=0.83 line=0.42] `esphome/components/esp32/gpio_idf.cpp:84-92` ⟵ `esphome/components/esp32/gpio_idf.cpp:31-38`
- [diverged w=0.81 line=0.59] `esphome/core/scheduler.cpp:127-141` ⟵ `esphome/core/scheduler.cpp:93-108`
- [diverged w=0.79 line=0.11] `esphome/core/helpers.cpp:411-416` ⟵ `esphome/core/helpers.cpp:126-129`
- [diverged w=0.73 line=0.38] `esphome/components/dsmr/dsmr.cpp:275-290` ⟵ `esphome/components/dsmr/dsmr.cpp:164-171`
- [diverged w=0.71 line=0.46] `esphome/components/bme280/bme280.cpp:172-213` ⟵ `esphome/components/bmp280/bmp280.cpp:125-156`
- [diverged w=0.68 line=0.45] `esphome/components/i2c/i2c_bus_arduino.cpp:37-65` ⟵ `esphome/components/i2c/i2c_bus_esp_idf.cpp:42-74`
- [diverged w=0.68 line=0.45] `esphome/components/i2c/i2c_bus_esp_idf.cpp:47-75` ⟵ `esphome/components/i2c/i2c_bus_arduino.cpp:32-63`
- [diverged w=0.66 line=0.43] `esphome/core/base_automation.h:232-240` ⟵ `esphome/core/base_automation.h:185-197`
- [diverged w=0.64 line=0.36] `esphome/components/mdns/mdns_esp32_arduino.cpp:11-21` ⟵ `esphome/components/mdns/mdns_esp8266.cpp:15-25`
- [diverged w=0.61 line=0.14] `esphome/components/dsmr/dsmr.cpp:191-253` ⟵ `esphome/components/dsmr/dsmr.cpp:73-143`

## ReZorg/dsstne @ `914d1cbc36dd`  (25 hits)
- [diverged w=1.00 line=0.60] `src/amazon/dsstne/utils/Filters.cpp:205-216` ⟵ `src/amazon/dsstne/utils/Filters.cpp:244-255`
- [near w=1.00 line=0.80] `src/amazon/dsstne/engine/NNWeight.cpp:841-850` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:461-470`
- [near w=1.00 line=0.80] `src/amazon/dsstne/engine/NNTypes.cpp:1126-1135` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:623-632`
- [near w=1.00 line=0.94] `src/amazon/dsstne/engine/NNNetwork.cpp:540-600` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:424-484`
- [verbatim w=1.00 line=1.00] `src/amazon/dsstne/engine/NNNetwork.cpp:4143-4192` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:3593-3642`
- [verbatim w=1.00 line=1.00] `src/amazon/dsstne/engine/NNNetwork.cpp:878-907` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:759-788`
- [near w=1.00 line=0.85] `src/amazon/dsstne/engine/NNNetwork.cpp:1750-1768` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:1619-1637`
- [diverged w=0.97 line=0.60] `src/amazon/dsstne/engine/NNTypes.cpp:1858-1892` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1211-1242`
- [verbatim w=0.95 line=0.96] `src/amazon/dsstne/utils/Utils.cpp:160-192` ⟵ `src/amazon/dsstne/utils/Utils.cpp:226-257`
- [near w=0.92 line=0.85] `src/amazon/dsstne/engine/NNNetwork.cpp:1710-1744` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:1579-1613`
- [near w=0.91 line=0.83] `src/amazon/dsstne/engine/NNTypes.cpp:2393-2454` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1589-1650`
- [diverged w=0.90 line=0.59] `src/amazon/dsstne/engine/NNWeight.cpp:502-561` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:295-349`
- [diverged w=0.89 line=0.74] `src/amazon/dsstne/engine/NNNetwork.cpp:1603-1668` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:1475-1536`
- [near w=0.88 line=0.80] `src/amazon/dsstne/engine/NNLayer.cpp:912-948` ⟵ `src/amazon/dsstne/engine/NNLayer.cpp:234-266`
- [diverged w=0.87 line=0.55] `src/amazon/dsstne/engine/NNTypes.cpp:433-466` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:323-348`
- [near w=0.86 line=0.82] `src/amazon/dsstne/engine/NNTypes.cpp:2163-2214` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1371-1422`
- [diverged w=0.81 line=0.64] `src/amazon/dsstne/engine/NNLayer.cpp:1253-1284` ⟵ `src/amazon/dsstne/engine/NNLayer.cpp:481-509`
- [diverged w=0.80 line=0.76] `src/amazon/dsstne/engine/NNNetwork.cpp:913-988` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:794-865`
- [diverged w=0.78 line=0.38] `src/amazon/dsstne/engine/NNTypes.cpp:2029-2060` ⟵ `src/amazon/dsstne/engine/NNTypes.cpp:1211-1242`
- [diverged w=0.77 line=0.71] `src/amazon/dsstne/engine/NNTypes.h:644-649` ⟵ `src/amazon/dsstne/engine/NNTypes.h:411-416`
- [diverged w=0.74 line=0.60] `src/amazon/dsstne/engine/NNLayer.cpp:1004-1039` ⟵ `src/amazon/dsstne/engine/NNLayer.cpp:299-330`
- [diverged w=0.73 line=0.41] `src/amazon/dsstne/engine/GpuTypes.h:489-538` ⟵ `src/amazon/dsstne/engine/GpuTypes.h:420-457`
- [diverged w=0.72 line=0.68] `src/amazon/dsstne/engine/NNNetwork.cpp:829-873` ⟵ `src/amazon/dsstne/engine/NNNetwork.cpp:709-754`
- [diverged w=0.68 line=0.39] `src/amazon/dsstne/engine/NNWeight.cpp:729-759` ⟵ `src/amazon/dsstne/engine/NNWeight.cpp:401-423`
- [diverged w=0.67 line=0.70] `src/amazon/dsstne/engine/GpuTypes.cpp:437-472` ⟵ `src/amazon/dsstne/engine/GpuTypes.cpp:393-419`

## Kilynho/cpu6502 @ `ae48ac939e4f`  (2 hits)
- [diverged w=1.00 line=0.24] `src/cpu/cpu.cpp:252-263` ⟵ `cpu.cpp:144-158`
- [diverged w=1.00 line=0.14] `src/main/cpu_demo.cpp:9-63` ⟵ `main_6502.cpp:8-62`

## ercasta/Clockstar-v2-Firmware @ `ff5a7d78f180`  (4 hits)
- [diverged w=1.00 line=0.75] `main/src/Notifs/Bangle.cpp:99-109` ⟵ `main/src/Notifs/Bangle.cpp:44-54`
- [near w=0.92 line=0.88] `main/src/Notifs/Bangle.cpp:155-200` ⟵ `main/src/Notifs/Bangle.cpp:86-128`
- [diverged w=0.88 line=0.78] `main/src/Devices/IMU.cpp:274-285` ⟵ `main/src/Devices/IMU.cpp:234-242`
- [verbatim w=0.73 line=0.97] `main/src/LV_Interface/FSLVGL.cpp:65-106` ⟵ `main/src/LV_Interface/FSLVGL.cpp:11-50`

## CanerKaraca23/classic-axis @ `dcb0d457bd78`  (1 hits)
- [diverged w=0.80 line=0.53] `vendor/GInputAPI/GInputAPI.h:278-292` ⟵ `source/shared/GInputAPI.h:261-271`

## awest813/Dewm-3 @ `56c7cf96af92`  (25 hits)
- [diverged w=1.00 line=0.74] `neo/tools/debugger/DebuggerWindow.cpp:460-503` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:398-440`
- [verbatim w=1.00 line=1.00] `neo/sys/win32/win_main.cpp:391-404` ⟵ `neo/sys/win32/win_main.cpp:288-301`
- [diverged w=1.00 line=0.75] `neo/framework/Common.cpp:2801-2815` ⟵ `neo/framework/Common.cpp:2728-2739`
- [verbatim w=1.00 line=1.00] `neo/sys/aros/aros_net.cpp:557-575` ⟵ `neo/sys/posix/posix_net.cpp:514-532`
- [verbatim w=1.00 line=1.00] `neo/sys/aros/aros_main.cpp:462-483` ⟵ `neo/sys/posix/posix_main.cpp:451-471`
- [verbatim w=1.00 line=1.00] `neo/sound/snd_system.cpp:522-540` ⟵ `neo/sound/snd_system.cpp:413-431`
- [near w=1.00 line=0.94] `neo/framework/UsercmdGen.cpp:1193-1219` ⟵ `neo/framework/UsercmdGen.cpp:935-959`
- [verbatim w=1.00 line=0.98] `neo/sys/events.cpp:487-652` ⟵ `neo/sys/events.cpp:97-258`
- [diverged w=1.00 line=0.79] `neo/framework/minizip/unzip.cpp:926-988` ⟵ `neo/framework/minizip/unzip.cpp:1042-1106`
- [verbatim w=1.00 line=0.96] `neo/renderer/RenderSystem_init.cpp:2342-2379` ⟵ `neo/renderer/RenderSystem_init.cpp:2085-2120`
- [near w=0.99 line=0.90] `neo/framework/FileSystem.cpp:3662-3686` ⟵ `neo/framework/FileSystem.cpp:3637-3661`
- [near w=0.99 line=0.81] `neo/d3xp/MultiplayerGame.cpp:3286-3333` ⟵ `neo/game/MultiplayerGame.cpp:2594-2641`
- [near w=0.99 line=0.81] `neo/game/MultiplayerGame.cpp:2595-2642` ⟵ `neo/d3xp/MultiplayerGame.cpp:3286-3333`
- [near w=0.98 line=0.91] `neo/sys/aros/aros_net.cpp:140-174` ⟵ `neo/sys/posix/posix_net.cpp:133-167`
- [diverged w=0.97 line=0.75] `neo/renderer/Image_files.cpp:861-867` ⟵ `neo/renderer/Image_files.cpp:980-986`
- [near w=0.97 line=0.80] `neo/framework/minizip/unzip.cpp:353-405` ⟵ `neo/framework/minizip/unzip.cpp:443-495`
- [verbatim w=0.96 line=0.95] `neo/tools/debugger/DebuggerWindow.cpp:785-852` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:667-734`
- [near w=0.96 line=0.90] `neo/renderer/draw_common.cpp:1181-1243` ⟵ `neo/renderer/draw_common.cpp:972-1031`
- [near w=0.95 line=0.86] `neo/framework/Common.cpp:2534-2564` ⟵ `neo/framework/Common.cpp:2475-2502`
- [near w=0.94 line=0.90] `neo/framework/Session.cpp:1664-1675` ⟵ `neo/framework/Session.cpp:1632-1641`
- [verbatim w=0.94 line=0.95] `neo/ui/ChoiceWindow.cpp:415-446` ⟵ `neo/ui/ChoiceWindow.cpp:381-410`
- [near w=0.93 line=0.87] `neo/tools/debugger/DebuggerWindow.cpp:2092-2173` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:1724-1806`
- [verbatim w=0.92 line=0.95] `neo/tools/debugger/DebuggerWindow.cpp:2184-2215` ⟵ `neo/tools/debugger/DebuggerWindow.cpp:1817-1846`
- [near w=0.92 line=0.94] `neo/framework/UsercmdGen.cpp:1332-1357` ⟵ `neo/framework/UsercmdGen.cpp:1054-1077`
- [near w=0.92 line=0.91] `neo/framework/UsercmdGen.cpp:1023-1071` ⟵ `neo/framework/UsercmdGen.cpp:767-814`

## deltaeecs/CSR4MPI @ `151224a7b9c0`  (2 hits)
- [near w=1.00 line=0.80] `include/MatrixMarketLoader.h:76-103` ⟵ `src/MatrixMarketLoader.cpp:70-98`
- [diverged w=0.76 line=0.52] `include/DistributedOps.h:18-61` ⟵ `src/DistributedOps.cpp:13-63`

## xr00tlabx/xRat-System_v3-cliente @ `b9ccb71c1d86`  (6 hits)
- [near w=1.00 line=0.88] `src/forms/MainForm.cpp:490-508` ⟵ `src/forms/MainForm.cpp:394-412`
- [near w=0.95 line=0.90] `src/controllers/MainController.cpp:445-482` ⟵ `src/controllers/MainController.cpp:376-413`
- [near w=0.94 line=0.87] `src/controllers/MainController.cpp:394-409` ⟵ `src/controllers/MainController.cpp:325-340`
- [near w=0.84 line=0.81] `src/forms/MainForm.cpp:422-482` ⟵ `src/forms/MainForm.cpp:340-386`
- [diverged w=0.76 line=0.58] `src/controllers/MainController.cpp:558-581` ⟵ `src/controllers/MainController.cpp:489-511`
- [near w=0.72 line=0.80] `src/forms/MainForm.cpp:528-561` ⟵ `src/forms/MainForm.cpp:432-457`

## Ninnh-temp/DSA @ `1c8de22ffa70`  (1 hits)
- [diverged w=0.87 line=0.76] `DSA_AVL_tree.cpp:210-277` ⟵ `DSA_AVL_tree.cpp:138-198`

## nayutalienx/cursor-trail @ `0a68e801e013`  (1 hits)
- [diverged w=0.90 line=0.60] `CursorTrail/Game.cpp:124-149` ⟵ `CursorTrail/Game.cpp:92-117`

## o6nn/kactl @ `0da961c46bbf`  (8 hits)
- [verbatim w=1.00 line=1.00] `stress-tests/geometry/ConvexHull.cpp:20-25` ⟵ `content/geometry/ConvexHull.h:40-45`
- [verbatim w=1.00 line=1.00] `stress-tests/graph/DirectedMST.cpp:13-16` ⟵ `fuzz-tests/fast-delaunay.cpp:34-37`
- [near w=0.91 line=0.80] `stress-tests/geometry/ConvexHull.cpp:8-17` ⟵ `content/geometry/ConvexHull.h:28-37`
- [near w=0.90 line=0.89] `stress-tests/graph/oldHLD.h:77-95` ⟵ `content/graph/HLD.h:93-111`
- [near w=0.89 line=0.83] `stress-tests/numerical/LinearRecurrence.cpp:17-42` ⟵ `fuzz-tests/linear-recurrence.cpp:27-52`
- [diverged w=0.81 line=0.63] `stress-tests/graph/LinkCutTree.cpp:7-41` ⟵ `fuzz-tests/link-cut.cpp:17-54`
- [diverged w=0.78 line=0.47] `stress-tests/number-theory/ModularArithmetic.cpp:6-20` ⟵ `fuzz-tests/modular-arithmetic.cpp:16-26`
- [diverged w=0.74 line=0.57] `stress-tests/strings/SuffixArray.cpp:88-100` ⟵ `content/strings/SuffixArray.h:23-35`

