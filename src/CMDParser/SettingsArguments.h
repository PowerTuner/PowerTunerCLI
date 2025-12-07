/*
 * This file is part of PowerTunerCLI.
 * Copyright (C) 2025 kylon
 *
 * PowerTunerCLI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PowerTunerCLI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

namespace PWT::CLI {
    // linux
    static constexpr char smtStateArg[] = "smt_state";
    static constexpr char cpuIdleGovernorArg[] = "cpu_idle_governor";
    static constexpr char blockDevQueSchedArg[] = "block_device_que_sched";
    static constexpr char miscPmDevsArg[] = "misc_device_pm";
    static constexpr char cpuMinFreqArg[] = "cpu_min_freq";
    static constexpr char cpuMaxFreqArg[] = "cpu_max_freq";
    static constexpr char cpuScalingGovernorArg[] = "cpu_scaling_governor";
    static constexpr char cpuStatusArg[] = "cpu_status";
#ifdef WITH_AMD
    static constexpr char pstateStatusArg[] = "pstate_status";
    static constexpr char pstateEppArg[] = "pstate_epp";
#endif

    // linux intel gpus
    static constexpr char gpuRpsMinFreqArg[] = "gpu_rps_min_freq";
    static constexpr char gpuRpsMaxFreqArg[] = "gpu_rps_max_freq";
    static constexpr char gpuRpsMaxBoostArg[] = "gpu_rps_max_boost";

    // linux amd gpus
    static constexpr char powerDpmStateArg[] = "power_dpm_state";
    static constexpr char powerDpmForcePerfLvlArg[] = "power_dpm_force_perf_level";
    static constexpr char powerDpmForcePerfMinSclkArg[] = "power_dpm_force_perf_min_sclk";
    static constexpr char powerDpmForcePerfMaxSclkArg[] = "power_dpm_force_perf_max_sclk";
    static constexpr char powerDpmForcePerfResetArg[] = "power_dpm_force_perf_reset";

    // windows
    static constexpr char powerSchemeArg[] = "power_scheme";

    // fan control
    static constexpr char fanModeArg[] = "fan_mode";
    static constexpr char fanTripPointArg[] = "fan_trip_point";

#ifdef WITH_INTEL
    // intel cpus
    static constexpr char pkgLimitPl1Arg[] = "pkg_limit_pl1";
    static constexpr char pkgLimitPl2Arg[] = "pkg_limit_pl2";
    static constexpr char pkgLimitPl1TimeArg[] = "pkg_limit_pl1_time";
    static constexpr char pkgLimitPl2TimeArg[] = "pkg_limit_pl2_time";
    static constexpr char pkgLimitPl1ClampArg[] = "pkg_limit_pl1_clamp";
    static constexpr char pkgLimitPl2ClampArg[] = "pkg_limit_pl2_clamp";
    static constexpr char pkgLimitPl1EnableArg[] = "pkg_limit_pl1_enable";
    static constexpr char pkgLimitPl2EnableArg[] = "pkg_limit_pl2_enable";
    static constexpr char pkgLimitLockArg[] = "pkg_limit_lock";
    static constexpr char vrCurrentCfgPl4Arg[] = "vr_current_cfg_pl4";
    static constexpr char vrCurrentCfgLockArg[] = "vr_current_cfg_lock";
    static constexpr char pp1CurrentLimitArg[] = "pp1_current_limit";
    static constexpr char pp1CurrentLockArg[] = "pp1_current_lock";
    static constexpr char turboPwrCurrentTdpArg[] = "turbo_pwr_current_tdp";
    static constexpr char turboPwrCurrentTdpEnableArg[] = "turbo_pwr_current_tdp_enable";
    static constexpr char turboPwrCurrentTdcArg[] = "turbo_pwr_current_tdc";
    static constexpr char turboPwrCurrentTdcEnableArg[] = "turbo_pwr_current_tdc_enable";
    static constexpr char turboRatioLimit1cArg[] = "turbo_ratio_limit_1c";
    static constexpr char turboRatioLimit2cArg[] = "turbo_ratio_limit_2c";
    static constexpr char turboRatioLimit3cArg[] = "turbo_ratio_limit_3c";
    static constexpr char turboRatioLimit4cArg[] = "turbo_ratio_limit_4c";
    static constexpr char turboRatioLimit5cArg[] = "turbo_ratio_limit_5c";
    static constexpr char turboRatioLimit6cArg[] = "turbo_ratio_limit_6c";
    static constexpr char turboRatioLimit7cArg[] = "turbo_ratio_limit_7c";
    static constexpr char turboRatioLimit8cArg[] = "turbo_ratio_limit_8c";
    static constexpr char enhancedSpeedstepEnableArg[] = "enhanced_speedstep_enable";
    static constexpr char turboModeDisableArg[] = "turbo_mode_disable";
    static constexpr char bdProchotEnableArg[] = "bd_prochot_enable";
    static constexpr char c1eEnableArg[] = "c1e_enable";
    static constexpr char sapmImcC2PolicyArg[] = "self_refresh_pkg_c2_state_enable";
    static constexpr char fastBrkSnpEnArg[] = "fast_vid_swing_rate_enable";
    static constexpr char powerPerformancePlatformOverrideArg[] = "power_performance_platform_override";
    static constexpr char energyEfficiencyOptDisableArg[] = "energy_efficiency_opt_disable";
    static constexpr char raceToHaltDisableArg[] = "race_to_halt_disable";
    static constexpr char prochotOutputDisableArg[] = "prochot_output_disable";
    static constexpr char prochotCfgResponseEnableArg[] = "prochot_cfg_response_enable";
    static constexpr char vrThermAlertDisableLockArg[] = "prochot_bits_lock_enable";
    static constexpr char vrThermAlertDisableArg[] = "vr_thermal_alert_signaling_disable";
    static constexpr char ringEEDisableArg[] = "ring_ee_disable";
    static constexpr char saOptDisableArg[] = "sa_opt_disable";
    static constexpr char ookDisableArg[] = "ook_disable";
    static constexpr char hwpAutonomousDisableArg[] = "hwp_autonomous_disable";
    static constexpr char cstatePrewakeDisableArg[] = "cstate_prewake_disable";
    static constexpr char eistDisableArg[] = "eist_disable";
    static constexpr char pp0PriorityArg[] = "pp0_priority";
    static constexpr char pp1PriorityArg[] = "pp1_priority";
    static constexpr char energyPerfBiasArg[] = "energy_perf_bias";
    static constexpr char uvControlCpuArg[] = "uv_control_cpu";
    static constexpr char uvControlGpuArg[] = "uv_control_gpu";
    static constexpr char uvControlCacheArg[] = "uv_control_cache";
    static constexpr char uvControlUnsliceArg[] = "uv_control_unslice";
    static constexpr char uvControlSaArg[] = "uv_control_sa";
    static constexpr char hwpEnableArg[] = "hwp_enable";
    static constexpr char hwpPkgCtlPolarityEnableArg[] = "hwp_pkg_ctl_polarity_enable";
    static constexpr char hwpRequestPkgMinPerfArg[] = "hwp_request_pkg_min_perf";
    static constexpr char hwpRequestPkgMaxPerfArg[] = "hwp_request_pkg_max_perf";
    static constexpr char hwpRequestPkgDesiredPerfArg[] = "hwp_request_pkg_desired_perf";
    static constexpr char hwpRequestPkgEppArg[] = "hwp_request_pkg_epp";
    static constexpr char hwpRequestPkgAcwArg[] = "hwp_request_pkg_acw";
    static constexpr char hwpRequestMinPerfArg[] = "hwp_request_min_perf";
    static constexpr char hwpRequestMaxPerfArg[] = "hwp_request_max_perf";
    static constexpr char hwpRequestDesiredPerfArg[] = "hwp_request_desired_perf";
    static constexpr char hwpRequestEppArg[] = "hwp_request_epp";
    static constexpr char hwpRequestAcwArg[] = "hwp_request_acw";
    static constexpr char hwpRequestPkgControlArg[] = "hwp_request_pkg_control";
    static constexpr char hwpRequestMinValidArg[] = "hwp_request_min_valid";
    static constexpr char hwpRequestMaxValidArg[] = "hwp_request_max_valid";
    static constexpr char hwpRequestDesiredValidArg[] = "hwp_request_desired_valid";
    static constexpr char hwpRequestAcwValidArg[] = "hwp_request_acw_valid";
    static constexpr char hwpRequestEppValidArg[] = "hwp_request_epp_valid";
    static constexpr char pkgCstCfgControlCstateLimArg[] = "pkg_cst_cfg_control_cstate_limit";
    static constexpr char pkgCstCfgControlMaxCoreCstateArg[] = "pkg_cst_cfg_control_max_core_cstate";
    static constexpr char pkgCstCfgControlIOMWaitRedirectEnableArg[] = "pkg_cst_cfg_control_io_mwait_redirection_enable";
    static constexpr char pkgCstCfgControlCfgLockArg[] = "pkg_cst_cfg_control_cfg_lock_enable";
    static constexpr char pkgCstCfgControlC3StateAutodemotionEnableArg[] = "pkg_cst_cfg_control_c3_autodemotion_enable";
    static constexpr char pkgCstCfgControlC1StateAutodemotionEnableArg[] = "pkg_cst_cfg_control_c1_autodemotion_enable";
    static constexpr char pkgCstCfgControlC3UndemotionEnableArg[] = "pkg_cst_cfg_control_c3_undemotion_enable";
    static constexpr char pkgCstCfgControlC1UndemotionEnableArg[] = "pkg_cst_cfg_control_c1_undemotion_enable";
    static constexpr char pkgCstCfgControlPkgcAutodemotionEnableArg[] = "pkg_cst_cfg_control_pkgc_autodemotion_enable";
    static constexpr char pkgCstCfgControlPkgcUndemotionEnableArg[] = "pkg_cst_cfg_control_pkgc_undemotion_enable";
    static constexpr char pkgCstCfgControlTimedMwaitEnableArg[] = "pkg_cst_cfg_control_timed_mwait_enable";
    static constexpr char pkgRaplPl1Arg[] = "pkg_rapl_limit_pl1";
    static constexpr char pkgRaplPl2Arg[] = "pkg_rapl_limit_pl2";
    static constexpr char pkgRaplPl1EnableArg[] = "pkg_rapl_limit_pl1_enable";
    static constexpr char pkgRaplPl2EnableArg[] = "pkg_rapl_limit_pl2_enable";
    static constexpr char pkgRaplPl1TimeArg[] = "pkg_rapl_limit_pl1_time";
    static constexpr char pkgRaplPl1ClampArg[] = "pkg_rapl_limit_pl1_clamp";
    static constexpr char pkgRaplLockArg[] = "pkg_rapl_limit_lock";
#endif
#ifdef WITH_AMD
    // amd cpus
    static constexpr char stapmLimitArg[] = "stapm_limit";
    static constexpr char fastLimitArg[] = "fast_limit";
    static constexpr char slowLimitArg[] = "slow_limit";
    static constexpr char tctlTempArg[] = "tctl_temp";
    static constexpr char apuSlowArg[] = "apu_slow";
    static constexpr char apuSkinTempArg[] = "apu_skin_temp";
    static constexpr char dgpuSkinTempArg[] = "dgpu_skin_temp";
    static constexpr char curveOptimizerArg[] = "co_all";
    static constexpr char curveOptimizerPerArg[] = "co_per";
    static constexpr char vrmCurrentArg[] = "vrm_current";
    static constexpr char vrmSocCurrentArg[] = "vrm_soc_current";
    static constexpr char vrmMaxCurrentArg[] = "vrm_max_current";
    static constexpr char vrmSocMaxCurrentArg[] = "vrm_soc_max_current";
    static constexpr char staticGfxClkArg[] = "static_gfx_clk";
    static constexpr char minGfxClkArg[] = "min_gfx_clk";
    static constexpr char maxGfxClkArg[] = "max_gfx_clk";
    static constexpr char amdPowerProfileArg[] = "amd_power_profile";
    static constexpr char corePerfBoostArg[] = "core_perf_boost_disable";
    static constexpr char pstateCmdArg[] = "pstate_cmd";
    static constexpr char cppcEnableArg[] = "cppc_enable";
    static constexpr char cppcRequestMaxPerfArg[] = "cppc_request_max_perf";
    static constexpr char cppcRequestMinPerfArg[] = "cppc_request_min_perf";
    static constexpr char cppcRequestDesiredPerfArg[] = "cppc_request_desired_perf";
    static constexpr char cppcRequestEppArg[] = "cppc_request_epp";
    static constexpr char cppcRequestPreferOSArg[] = "cppc_prefer_os";
#endif
}
