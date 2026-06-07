# Boolean-State — широкий прогон (без name-фильтров)

**Кандидатов:** 5811 структур с 5+ реальными bool-полями в 790 репозиториях.

**Фильтры:** только глубина-0 (реальные поля, не локальные переменные). Name-whitelist и blacklist УБРАНЫ. Колонки `vendor`/`state-like` — пометки, не фильтр.

## Сводка (790 репо)

| Метрика | Значение |
|---|---|
| Всего структур с 5+ bool | **5811** |
| Вендоренные | 383 (6%) |
| Свои (non-vendor) | 5428 |
| — из них **0 state-подобных имён** (конфиг/data-мешки) | **4231 (78%)** |
| — 1-2 state-имени | 1021 |
| — 3+ state, но <50% полей | 121 |
| — **3+ state, ≥50% полей** (нейминг-кандидаты в FSM) | **55** |

**Вывод по неймингу:** ~1% структур с 5+ bool выглядят машиной состояний *по именам*. 78% — заведомо конфиг (0 lifecycle-имён). Гиганты сверху (TeslaBattery 215, PQCSettings 199, SettingsCache 76) — все конфиги. Но нейминг слеп к FSM с «нестандартными» именами — реальную долю даёт только анализ использования (см. [boolean_state_usage_verdicts.md](boolean_state_usage_verdicts.md)).

| Bools | State-like | Vendor | Struct | File |
|---|---|---|---|---|
| 215 | 5 |  | `TeslaBattery` | [TESLA-BATTERY.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/battery/TESLA-BATTERY.h) |
| 199 | 1 |  | `PQCSettings` | [pqc_settings.h](file://~/oss/luspi_photoqt/cplusplus/header/pqc_settings.h) |
| 199 | 1 |  | `PQCSettings` | [pqc_settings.h](file://~/oss/luspi_photoqt/misc/pqc_settings/output/pqc_settings.h) |
| 156 | 1 |  | `uae_prefs` | [options.h](file://~/oss/tonioni_WinUAE/include/options.h) |
| 116 | 1 |  | `TestConfig` | [test_config.h](file://~/oss/aegis-aead_boringssl/ssl/test/test_config.h) |
| 115 | 3 |  | `dc_debug_options` | [dc.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/display/dc/dc.h) |
| 109 | 1 | ⚠️ | `TestConfig` | [test_config.h](file://~/oss/grpc/third_party/boringssl-with-bazel/ssl/test/test_config.h) |
| 104 | 2 |  | `MM_GCExtensionsBase` | [GCExtensionsBase.hpp](file://~/oss/eclipse-omr_omr/gc/base/GCExtensionsBase.hpp) |
| 104 | 2 |  | `MM_GCExtensionsBase` | [GCExtensionsBase.hpp](file://~/oss/eclipse-openj9_openj9-omr/gc/base/GCExtensionsBase.hpp) |
| 95 | 1 |  | `vkd3d_vulkan_info` | [vkd3d_private.h](file://~/oss/HansKristian-Work_vkd3d-proton/libs/vkd3d/vkd3d_private.h) |
| 92 | 2 |  | `FirstPersonControllerComponent` | [FirstPersonControllerComponent.h](file://~/oss/Porcupine-Factory_FirstPersonController/Code/Source/Clients/FirstPersonControllerComponent.h) |
| 89 | 0 |  | `options` | [options.h](file://~/oss/OpenVPN_openvpn/src/openvpn/options.h) |
| 89 | 0 |  | `options` | [options.h](file://~/oss/schwabe_openvpn/src/openvpn/options.h) |
| 88 | 0 |  | `CompOptions` | [MDFrameWork.h](file://~/oss/intel_intel-graphics-compiler/IGC/common/MDFrameWork.h) |
| 82 | 2 |  | `CPreferences` | [Preferences.h](file://~/oss/amule-project_amule/src/Preferences.h) |
| 82 | 1 | ⚠️ | `OperationConfig` | [tool_cfgable.h](file://~/oss/infiniflow_infinity/third_party/curl/src/tool_cfgable.h) |
| 80 | 2 |  | `vba_vars_st` | [display_mode_vba.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/display/dc/dml/display_mode_vba.h) |
| 80 | 0 |  | `PlayerbotAIConfig` | [PlayerbotAIConfig.h](file://~/oss/mod-playerbots_mod-playerbots/src/PlayerbotAIConfig.h) |
| 77 | 0 |  | `ecore_mfw_tlv_fcoe` | [ecore_mcp_api.h](file://~/oss/DPDK_dpdk/drivers/net/qede/base/ecore_mcp_api.h) |
| 77 | 0 |  | `UserInput` | [mlxlink_user_input.h](file://~/oss/Mellanox_mstflint/mlxlink/modules/mlxlink_user_input.h) |
| 76 | 0 |  | `SettingsCache` | [cache_settings.h](file://~/oss/Cockatrice_Cockatrice/cockatrice/src/client/settings/cache_settings.h) |
| 76 | 0 |  | `Abstract3DChangeBitField` | [qquickgraphsitem_p.h](file://~/oss/qt_qtgraphs/src/graphs3d/qml/qquickgraphsitem_p.h) |
| 74 | 0 |  | `qed_mfw_tlv_fcoe` | [qed_if.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/include/linux/qed/qed_if.h) |
| 73 | 0 |  | `DxcOpts` | [HLSLOptions.h](file://~/oss/microsoft_DirectXShaderCompiler/include/dxc/Support/HLSLOptions.h) |
| 68 | 1 |  | `FlintParams` | [flint_params.h](file://~/oss/Mellanox_mstflint/flint/flint_params.h) |
| 65 | 2 |  | `MachineObject` | [DeviceManager.hpp](file://~/oss/BambuStudio/src/slic3r/GUI/DeviceManager.hpp) |
| 64 | 0 |  | `amdgcn_compiler_target_state` | [config.hpp](file://~/oss/ROCm_composable_kernel/include/ck_tile/core/config.hpp) |
| 63 | 0 |  | `CodeOfPolicies` | [ast.h](file://~/oss/GaijinEntertainment_daScript/include/daScript/ast/ast.h) |
| 63 | 3 |  | `CellPowerBms` | [CELLPOWER-BMS.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/battery/CELLPOWER-BMS.h) |
| 61 | 3 |  | `DATALAYER_INFO_CELLPOWER` | [datalayer_extended.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer_extended.h) |
| 61 | 0 |  | `MPOpts` | [options.h](file://~/oss/mpv-player_mpv/options/options.h) |
| 60 | 0 |  | `IDLOptions` | [idl.h](file://~/oss/aardappel_lobster/dev/include/flatbuffers/idl.h) |
| 55 | 1 |  | `TextEdit` | [text_edit.h](file://~/oss/RebelToolbox_RebelEngine/scene/gui/text_edit.h) |
| 55 | 0 |  | `dml2_core_internal_mode_support_info` | [dml2_core_shared_types.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/display/dc/dml2/dml21/src/dml2_core/dml2_core_shared_types.h) |
| 54 | 0 |  | `vivid_dev` | [vivid-core.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/media/platform/vivid/vivid-core.h) |
| 54 | 1 |  | `Config` | [Config.h](file://~/oss/stellar_stellar-core/src/main/Config.h) |
| 54 | 0 |  | `preferences` | [pref.h](file://~/oss/subsurface_subsurface/core/pref.h) |
| 53 | 5 |  | `SpectrumWidget` | [SpectrumWidget.h](file://~/oss/AetherSDR/src/gui/SpectrumWidget.h) |
| 53 | 3 |  | `brcms_phy` | [phy_int.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/net/wireless/broadcom/brcm80211/brcmsmac/phy/phy_int.h) |
| 50 | 1 |  | `ipa3_context` | [ipa_i.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/techpack/dataipa/drivers/platform/msm/ipa/ipa_v3/ipa_i.h) |
| 50 | 0 |  | `smb_vol` | [cifsglob.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/fs/cifs/cifsglob.h) |
| 50 | 0 |  | `dml2_mode_support_info` | [dml_top_types.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/display/dc/dml2/dml21/inc/dml_top_types.h) |
| 49 | 3 |  | `DM_Out_Source_Dynamic_Mechanism_Structure` | [odm.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/staging/rtl8723bs/hal/odm.h) |
| 49 | 0 |  | `ProgramParameters` | [ProgramParameters.hpp](file://~/oss/markaren_threepp/src/threepp/renderers/gl/ProgramParameters.hpp) |
| 47 | 2 |  | `hdd_context` | [wlan_hdd_main.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/staging/qcacld-3.0/core/hdd/inc/wlan_hdd_main.h) |
| 47 | 6 |  | `smb_charger` | [smb5-lib.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/power/supply/qcom/smb5-lib.h) |
| 47 | 0 |  | `FakeAclEntry` | [FakeSaiAcl.h](file://~/oss/facebook_fboss/fboss/agent/hw/sai/fake/FakeSaiAcl.h) |
| 47 | 5 |  | `wpa_supplicant` | [wpa_supplicant_i.h](file://~/oss/sailfishos-mirror_hostap/wpa_supplicant/wpa_supplicant_i.h) |
| 47 | 0 |  | `jit_disable_opcodes` | [compemu.h](file://~/oss/tonioni_WinUAE/jit/compemu.h) |
| 46 | 0 |  | `CompileOptions` | [Driver.h](file://~/oss/intel_intel-graphics-compiler/IGC/VectorCompiler/include/vc/Driver/Driver.h) |
| 45 | 2 |  | `pe_session` | [lim_session.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/staging/qcacld-3.0/core/mac/src/pe/include/lim_session.h) |
| 45 | 0 |  | `Account` | [account.h](file://~/oss/MaximumTrainer_MaximumTrainer_Redux/src/model/account.h) |
| 45 | 2 |  | `DATALAYER_INFO_TESLA` | [datalayer_extended.h](file://~/oss/dalathegreat_Battery-Emulator/Software/src/datalayer/datalayer_extended.h) |
| 44 | 0 |  | `smt_params` | [smt_params.h](file://~/oss/Z3Prover_z3/src/params/smt_params.h) |
| 44 | 0 |  | `confdata` | [mudconf.h](file://~/oss/brazilofmux_tinymux/mux/include/mudconf.h) |
| 44 | 0 |  | `TransportSettings` | [TransportSettings.h](file://~/oss/facebook_mvfst/quic/state/TransportSettings.h) |
| 44 | 5 |  | `wpa_supplicant` | [wpa_supplicant_i.h](file://~/oss/greearb_hostap-ct/wpa_supplicant/wpa_supplicant_i.h) |
| 44 | 0 |  | `InternalOptions` | [OpenCLOptions.hpp](file://~/oss/intel_intel-graphics-compiler/IGC/Compiler/CISACodeGen/OpenCLOptions.hpp) |
| 44 | 0 |  | `zaberStage` | [zaberStage.hpp](file://~/oss/magao-x_MagAOX/apps/zaberLowLevel/zaberStage.hpp) |
| 43 | 0 |  | `XvaVariables` | [xvaanalytic.hpp](file://~/oss/OpenSourceRisk_Engine/OREAnalytics/orea/app/analytics/xvaanalytic.hpp) |
| 43 | 1 |  | `kerndat_s` | [kerndat.h](file://~/oss/checkpoint-restore_criu/criu/include/kerndat.h) |
| 43 | 1 |  | `CommandList` | [cmdlist.h](file://~/oss/intel_compute-runtime/level_zero/core/source/cmdlist/cmdlist.h) |
| 43 | 0 |  | `DBOptions` | [options.h](file://~/oss/topling_toplingdb/include/rocksdb/options.h) |
| 42 | 0 |  | `MockMemoryManager` | [mock_memory_manager.h](file://~/oss/intel_compute-runtime/shared/test/common/mocks/mock_memory_manager.h) |
| 42 | 0 |  | `PQCWindowGeometry` | [pqc_windowgeometry.h](file://~/oss/luspi_photoqt/cplusplus/header/pqc_windowgeometry.h) |
| 42 | 0 |  | `Config` | [Config.h](file://~/oss/rzeronte_brakeza3d/include/Config.h) |
| 41 | 0 |  | `Oiiotool` | [oiiotool.h](file://~/oss/AcademySoftwareFoundation_OpenImageIO/src/oiiotool/oiiotool.h) |
| 41 | 0 |  | `drxk_state` | [drxk_hard.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/media/dvb-frontends/drxk_hard.h) |
| 41 | 4 |  | `rtw_coex_stat` | [main.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/net/wireless/realtek/rtw88/main.h) |
| 41 | 3 |  | `CanvasItemEditor` | [canvas_item_editor_plugin.h](file://~/oss/RebelToolbox_RebelEngine/editor/plugins/canvas_item_editor_plugin.h) |
| 41 | 0 |  | `Schema_dumper` | [schema_dumper.h](file://~/oss/mysql_mysql-shell/modules/util/dump/schema_dumper.h) |
| 41 | 0 |  | `ImmutableDBOptions` | [db_options.h](file://~/oss/topling_toplingdb/options/db_options.h) |
| 40 | 0 |  | `vba_vars_st` | [display_mode_vba.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/gpu/drm/amd/display/dc/dml/display_mode_vba.h) |
| 40 | 0 |  | `VEvndispRunParameter` | [VEvndispRunParameter.h](file://~/oss/VERITAS-Observatory_EventDisplay_v4/inc/VEvndispRunParameter.h) |
| 40 | 3 |  | `CommandStreamReceiver` | [command_stream_receiver.h](file://~/oss/intel_compute-runtime/shared/source/command_stream/command_stream_receiver.h) |
| 40 | 0 |  | `GenXBackendOptions` | [BackendConfig.h](file://~/oss/intel_intel-graphics-compiler/IGC/VectorCompiler/include/vc/Support/BackendConfig.h) |
| 39 | 0 |  | `dc_debug_options` | [dc.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/gpu/drm/amd/display/dc/dc.h) |
| 39 | 0 |  | `Settings` | [Settings.h](file://~/oss/OpenMagnetics_MKF/src/support/Settings.h) |
| 39 | 0 |  | `intel_crtc_state` | [intel_display_types.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/i915/display/intel_display_types.h) |
| 39 | 0 |  | `SInstrTypes` | [CodeGenPublic.h](file://~/oss/intel_intel-graphics-compiler/IGC/Compiler/CodeGenPublic.h) |
| 38 | 1 |  | `GLCanvas3D` | [GLCanvas3D.hpp](file://~/oss/BambuStudio/src/slic3r/GUI/GLCanvas3D.hpp) |
| 38 | 0 |  | `Profile` | [profile.h](file://~/oss/shadps4-emu_shadPS4/src/shader_recompiler/profile.h) |
| 37 | 15 |  | `WorkoutDialog` | [workoutdialog.h](file://~/oss/MaximumTrainer_MaximumTrainer_Redux/src/ui/workoutdialog.h) |
| 36 | 1 |  | `Conf` | [EvseManager.hpp](file://~/oss/EVerest_EVerest/modules/EVSE/EvseManager/EvseManager.hpp) |
| 36 | 0 |  | `ci_power_info` | [ci_dpm.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/gpu/drm/radeon/ci_dpm.h) |
| 36 | 0 | ⚠️ | `BackendVariations` | [spirv_glsl.hpp](file://~/oss/_snap_tmp/p2_godlikepanos_anki-3d-engine/ThirdParty/SpirvCross/spirv_glsl.hpp) |
| 36 | 0 |  | `AOTCompContext` | [aot_llvm.h](file://~/oss/bytecodealliance_wasm-micro-runtime/core/iwasm/compilation/aot_llvm.h) |
| 36 | 0 |  | `ci_power_info` | [ci_dpm.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/radeon/ci_dpm.h) |
| 36 | 0 |  | `UltCommandStreamReceiver` | [ult_command_stream_receiver.h](file://~/oss/intel_compute-runtime/shared/test/common/libult/ult_command_stream_receiver.h) |
| 36 | 0 |  | `TextEditor` | [TextEditor.h](file://~/oss/jwmcglynn_donner/donner/editor/TextEditor.h) |
| 36 | 0 |  | `CodeGenInstruction` | [CodeGenInstruction.h](file://~/oss/microsoft_DirectXShaderCompiler/utils/TableGen/CodeGenInstruction.h) |
| 35 | 0 |  | `wlan_cfg_dp_soc_ctxt` | [wlan_cfg.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/staging/qca-wifi-host-cmn/wlan_cfg/wlan_cfg.h) |
| 35 | 0 |  | `Settings` | [settings.h](file://~/oss/MaximumTrainer_MaximumTrainer_Redux/src/model/settings.h) |
| 35 | 0 |  | `context` | [spacer_context.h](file://~/oss/Z3Prover_z3/src/muz/spacer/spacer_context.h) |
| 35 | 1 |  | `shell_options` | [lush.h](file://~/oss/berrym_lush/include/lush.h) |
| 35 | 6 |  | `client` | [client-common.h](file://~/oss/dovecot_core/src/login-common/client-common.h) |
| 35 | 0 |  | `CArg` | [args.h](file://~/oss/dxx-rebirth_dxx-rebirth/common/include/args.h) |
| 35 | 1 |  | `dc_config` | [dc.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/display/dc/dc.h) |
| 35 | 1 |  | `SciTEBase` | [SciTEBase.h](file://~/oss/kotuku-group_kotuku/src/scintilla/scite/src/SciTEBase.h) |
| 35 | 0 |  | `MCAsmInfo` | [MCAsmInfo.h](file://~/oss/microsoft_DirectXShaderCompiler/include/llvm/MC/MCAsmInfo.h) |
| 35 | 0 |  | `SpirvCodeGenOptions` | [SPIRVOptions.h](file://~/oss/microsoft_DirectXShaderCompiler/include/dxc/Support/SPIRVOptions.h) |
| 35 | 0 |  | `perftest_config` | [perftest_parameters.h](file://~/oss/openeuler-mirror_umdk/src/urma/tools/urma_perftest/perftest_parameters.h) |
| 34 | 0 |  | `r8192_priv` | [rtl_core.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/staging/rtl8192e/rtl8192e/rtl_core.h) |
| 34 | 0 |  | `ipa3_plat_drv_res` | [ipa_i.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/techpack/dataipa/drivers/platform/msm/ipa/ipa_v3/ipa_i.h) |
| 34 | 0 |  | `config` | [sat_config.h](file://~/oss/Z3Prover_z3/src/sat/sat_config.h) |
| 34 | 0 |  | `AOTCompContext` | [aot_llvm.h](file://~/oss/fluent_fluent-bit/lib/wasm-micro-runtime-WAMR-2.4.1/core/iwasm/compilation/aot_llvm.h) |
| 34 | 0 |  | `FormatStyle` | [Format.h](file://~/oss/microsoft_DirectXShaderCompiler/tools/clang/include/clang/Format/Format.h) |
| 34 | 0 |  | `MmcifOutputGroups` | [to_mmcif.hpp](file://~/oss/project-gemmi_gemmi/include/gemmi/to_mmcif.hpp) |
| 34 | 0 |  | `IDLOptions` | [idl.h](file://~/oss/raspberrypi_libcamera/src/ipa/rpi/cam_helper/imx500_tensor_parser/flatbuffers/idl.h) |
| 33 | 0 |  | `record_opts` | [record.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/tools/perf/util/record.h) |
| 33 | 0 |  | `unicode_code_point_data` | [unicode-data-static.h](file://~/oss/dovecot_core/src/lib/unicode-data-static.h) |
| 33 | 2 |  | `amdgpu_device` | [amdgpu.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/amdgpu/amdgpu.h) |
| 33 | 0 |  | `Sema` | [Sema.h](file://~/oss/microsoft_DirectXShaderCompiler/tools/clang/include/clang/Sema/Sema.h) |
| 33 | 0 |  | `OpenAPI_media_component_rm_s` | [media_component_rm.h](file://~/oss/open5gs_open5gs/lib/sbi/openapi/model/media_component_rm.h) |
| 32 | 0 |  | `lightningd` | [lightningd.h](file://~/oss/ElementsProject_lightning/lightningd/lightningd.h) |
| 32 | 0 |  | `HostFeatures` | [HostFeatures.h](file://~/oss/FEX-Emu_FEX/FEXCore/include/FEXCore/Core/HostFeatures.h) |
| 32 | 1 |  | `GeneralSettings` | [generalsettings.h](file://~/oss/Kartend/src/utils/app/collection/generalsettings.h) |
| 32 | 3 |  | `MainWindow` | [mainwindow.h](file://~/oss/ea4k_klog/src/mainwindow.h) |
| 32 | 0 |  | `Globals` | [ispc.h](file://~/oss/ispc_ispc/src/ispc.h) |
| 31 | 3 |  | `RadioModel` | [RadioModel.h](file://~/oss/AetherSDR/src/models/RadioModel.h) |
| 31 | 0 |  | `kv_power_info` | [kv_dpm.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/gpu/drm/amd/amdgpu/kv_dpm.h) |
| 31 | 0 |  | `kv_power_info` | [kv_dpm.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/gpu/drm/radeon/kv_dpm.h) |
| 31 | 0 |  | `ipahal_reg_clkon_cfg` | [ipahal_reg.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/techpack/dataipa/drivers/platform/msm/ipa/ipa_v3/ipahal/ipahal_reg.h) |
| 31 | 0 |  | `Config` | [Phobos.h](file://~/oss/Phobos-developers_Phobos/src/Phobos.h) |
| 31 | 0 |  | `theory_arith_params` | [theory_arith_params.h](file://~/oss/Z3Prover_z3/src/params/theory_arith_params.h) |
| 31 | 0 |  | `Device_info` | [device.hpp](file://~/oss/_cpp_archcheck/tksuoran_erhe/src/erhe/graphics/erhe_graphics/device.hpp) |
| 31 | 0 |  | `ThermostatClimate` | [thermostat_climate.h](file://~/oss/esphome_esphome/esphome/components/thermostat/thermostat_climate.h) |
| 31 | 0 |  | `kv_power_info` | [kv_dpm.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/pm/legacy-dpm/kv_dpm.h) |
| 31 | 0 |  | `smu7_hwmgr` | [smu7_hwmgr.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/amd/pm/powerplay/hwmgr/smu7_hwmgr.h) |
| 31 | 0 |  | `kv_power_info` | [kv_dpm.h](file://~/oss/freebsd_drm-kmod/drivers/gpu/drm/radeon/kv_dpm.h) |
| 31 | 0 |  | `json_number` | [json_parse.h](file://~/oss/ioccc-src_mkiocccentry/jparse/json_parse.h) |
| 31 | 0 |  | `FirtoolOptions` | [Firtool.h](file://~/oss/llvm_circt/include/circt/Firtool/Firtool.h) |
| 31 | 0 |  | `Device_info` | [device.hpp](file://~/oss/tksuoran_erhe/src/erhe/graphics/erhe_graphics/device.hpp) |
| 30 | 0 |  | `ReductionParams` | [reduction_heuristic.h](file://~/oss/NVIDIA_Fuser/csrc/scheduler/reduction_heuristic.h) |
| 30 | 0 |  | `InputParameters` | [inputparameters.hpp](file://~/oss/OpenSourceRisk_Engine/OREAnalytics/orea/app/inputparameters.hpp) |
| 30 | 0 |  | `PDFToolOptions` | [pdftoolabstractapplication.h](file://~/oss/_cpp_archcheck/JakubMelka_PDF4QT/PdfTool/pdftoolabstractapplication.h) |
| 30 | 0 |  | `cp_options` | [copy.h](file://~/oss/coreutils_coreutils/src/copy.h) |
| 30 | 0 |  | `CreatureEntity` | [creature-entity.h](file://~/oss/deskull-m_bakabakaband/src/system/creature-entity.h) |
| 30 | 0 |  | `env` | [monitor.h](file://~/oss/duanery_perf-prof/src/monitor.h) |
| 30 | 0 | ⚠️ | `dwc2_core_params` | [vmlinux.h](file://~/oss/fluent_fluent-bit/plugins/in_ebpf/traces/includes/external/gadget/amd64/vmlinux.h) |
| 30 | 0 |  | `TargetArchitecture` | [targetosarch.h](file://~/oss/grafana_pyroscope-dotnet/shared/src/native-lib/coreclr/src/inc/targetosarch.h) |
| 30 | 0 |  | `libnvme_fabric_options` | [private.h](file://~/oss/linux-nvme_nvme-cli/libnvme/src/nvme/private.h) |
| 30 | 0 |  | `Scope` | [symbol_generator.hpp](file://~/oss/memgraph_memgraph/src/query/frontend/semantic/symbol_generator.hpp) |
| 30 | 0 |  | `TargetArchitecture` | [targetosarch.h](file://~/oss/open-telemetry_opentelemetry-dotnet-instrumentation/src/OpenTelemetry.AutoInstrumentation.Native/lib/coreclr/src/inc/targetosarch.h) |
| 30 | 0 |  | `Ppu` | [ppu.h](file://~/oss/scawful_yaze/src/app/emu/video/ppu.h) |
| 30 | 0 |  | `OskPadInputFrame` | [ime_ui_shared.h](file://~/oss/shadps4-emu_shadPS4/src/core/libraries/ime/ime_ui_shared.h) |
| 30 | 3 | ⚠️ | `ImGuiContext` | [imgui_internal.h](file://~/oss/zackb_code/cpp/away/external/imgui/include/imgui_internal.h) |
| 30 | 0 |  | `Settings` | [gltfpack.h](file://~/oss/zeux_meshoptimizer/gltf/gltfpack.h) |
| 29 | 3 |  | `ImGuiContext` | [imgui_internal.h](file://~/oss/BambuStudio/src/imgui/imgui_internal.h) |
| 29 | 0 |  | `smu7_hwmgr` | [smu7_hwmgr.h](file://~/oss/LineageOS_android_kernel_asus_sm8350/drivers/gpu/drm/amd/powerplay/hwmgr/smu7_hwmgr.h) |

*(показаны топ-150 из 5811; полный список — broad_scan.csv)*

