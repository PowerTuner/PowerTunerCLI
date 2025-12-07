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
#include "AppCommands.h"
#include "pwtClientCommon/CommonUtils.h"
#include "pwtShared/Utils.h"

namespace PWT::CLI {
    [[nodiscard]]
    static QString logLevelToString(const PWTS::LogLevel &lvl) {
        switch (lvl) {
            case PWTS::LogLevel::Info:
                return "Info";
            case PWTS::LogLevel::Warning:
                return "Warning";
            case PWTS::LogLevel::Error:
                return "Error";
            case PWTS::LogLevel::Service:
                return "Service";
            case PWTS::LogLevel::None:
                return "None";
            default:
                break;
        }

        return "Invalid";
    }

    bool addDaemons(const QList<QString> &data, const QScopedPointer<CLISettings> &cliSettings, const QSharedPointer<FileLogger> &logger) {
        bool ret = true;

        for (int i=0,l=data.size(); i<l; i+=3) {
            bool res;
            const quint16 portI = data[i+2].toUInt(&res);

            if (!res || !PWTS::isValidPort(portI)) {
                logger->write(QString("failed to add daemon \"%1\": invalid port").arg(data[i]));
                ret = false;

            } else if (!cliSettings->addDaemon(data[i], data[i+1], portI)) {
                logger->write(QString("failed to add daemon: \"%1\"").arg(data[i]));
                ret = false;
            }
        }

        return ret;
    }

    [[nodiscard]]
    static QJsonObject getDaemonSettingsJson(const QSharedPointer<PWTS::DaemonSettings> &daemonSettings) {
        QJsonObject jobj;

        jobj.insert("start_profile", daemonSettings->getOnStartProfile());
        jobj.insert("battery_profile", daemonSettings->getOnBatteryProfile());
        jobj.insert("power_supply_profile", daemonSettings->getOnPowerSupplyProfile());
        jobj.insert("address", daemonSettings->getAddress());
        jobj.insert("apply_interval", daemonSettings->getApplyInterval());
        jobj.insert("apply_on_wake_from_sleep", daemonSettings->getApplyOnWakeFromSleep());
        jobj.insert("ignore_battery_event", daemonSettings->getIgnoreBatteryEvent());
        jobj.insert("log_level", static_cast<int>(daemonSettings->getLogLevel()));
        jobj.insert("log_level_name", logLevelToString(daemonSettings->getLogLevel()));
        jobj.insert("max_log_files", daemonSettings->getMaxLogFiles());
        jobj.insert("tcp_port", daemonSettings->getSocketTcpPort());
        jobj.insert("udp_port", daemonSettings->getSocketUdpPort());

        return jobj;
    }

#ifdef WITH_INTEL
    [[nodiscard]]
    static QJsonObject getInputRangesIntelJson(const QSet<PWTS::Feature> &features, const QSharedPointer<UI::InputRanges> &inputRanges) {
        QJsonObject rangesDB;

        if (features.contains(PWTS::Feature::INTEL_PKG_POWER_LIMIT)) {
            const PWTS::MinMax limit = inputRanges->getIntelPl();
            QJsonObject rangeObj;

            rangeObj.insert("min", limit.min);
            rangeObj.insert("max", limit.max);
            rangesDB.insert("pkg_power_limit", rangeObj);
        }

        if (features.contains(PWTS::Feature::INTEL_VR_CURRENT_CFG)) {
            const PWTS::MinMax limit = inputRanges->getIntelPl4();
            QJsonObject rangeObj;

            rangeObj.insert("min", limit.min);
            rangeObj.insert("max", limit.max);
            rangesDB.insert("vr_current_config", rangeObj);
        }

        if (features.contains(PWTS::Feature::INTEL_PP1_CURRENT_CFG)) {
            const PWTS::MinMax limit = inputRanges->getIntelPP1();
            QJsonObject rangeObj;

            rangeObj.insert("min", limit.min);
            rangeObj.insert("max", limit.max);
            rangesDB.insert("pp1_current_config", rangeObj);
        }

        if (features.contains(PWTS::Feature::INTEL_UNDERVOLT_GROUP)) {
            const PWTS::MinMax limit = inputRanges->getIntelFIVR();
            QJsonObject rangeObj;

            rangeObj.insert("min", limit.min);
            rangeObj.insert("max", limit.max);
            rangesDB.insert("undervolt", rangeObj);
        }

        return rangesDB;
    }
#endif

#ifdef WITH_AMD
    [[nodiscard]]
    static QJsonObject getInputRangesAMDJson(const QSet<PWTS::Feature> &features, const QSharedPointer<UI::InputRanges> &inputRanges) {
        QJsonObject rangesDB;

        if (features.contains(PWTS::Feature::AMD_CPU_RY_GROUP)) {
            if (features.contains(PWTS::Feature::AMD_RY_SLOW_LIMIT) || features.contains(PWTS::Feature::AMD_RY_FAST_LIMIT) || features.contains(PWTS::Feature::AMD_RY_STAPM_LIMIT)) {
                const PWTS::MinMax limit = inputRanges->getRADJPl();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_power_limit", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_TCTL_TEMP)) {
                const PWTS::MinMax limit = inputRanges->getRADJTctl();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_tctl", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_APU_SLOW)) {
                const PWTS::MinMax limit = inputRanges->getRADJAPUSlow();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_apu_slow_limit", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_APU_SKIN_TEMP)) {
                const PWTS::MinMax limit = inputRanges->getRADJAPUSkinTemp();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_apu_skin_temp", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_DGPU_SKIN_TEMP)) {
                const PWTS::MinMax limit = inputRanges->getRADJDGPUSkinTemp();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_dgpu_skin_temp", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_VRM_CURRENT)) {
                const PWTS::MinMax limit = inputRanges->getRADJVrmCurrent();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_vrm_current", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_VRM_SOC_CURRENT)) {
                const PWTS::MinMax limit = inputRanges->getRADJVrmSocCurrent();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_vrm_soc_current", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_STATIC_GFX_CLK) || features.contains(PWTS::Feature::AMD_RY_MIN_GFX_CLOCK) || features.contains(PWTS::Feature::AMD_RY_MAX_GFX_CLOCK)) {
                const PWTS::MinMax limit = inputRanges->getRADJGfxClock();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_gfx_clock", rangeObj);
            }

            if (features.contains(PWTS::Feature::AMD_RY_CO_ALL) || features.contains(PWTS::Feature::AMD_RY_CO_PER)) {
                const PWTS::MinMax limit = inputRanges->getRADJCO();
                QJsonObject rangeObj;

                rangeObj.insert("min", limit.min);
                rangeObj.insert("max", limit.max);
                rangesDB.insert("ryzenadj_curve_optimizer", rangeObj);
            }
        }

        return rangesDB;
    }
#endif

    [[nodiscard]]
    static QJsonObject getDeviceInfoJson(const PWTS::DeviceInfoPacket &packet, const QSharedPointer<PWTS::DaemonSettings> &daemonSettings, const QSharedPointer<UI::InputRanges> &inputRanges) {
        QJsonObject jobj = PWTS::getDeviceInfoJson(packet);
        QJsonObject jDaemon = jobj["daemon"].toObject();
        QJsonObject rangesDB;

        switch (packet.cpuInfo.vendor) {
#ifdef WITH_INTEL
            case PWTS::CPUVendor::Intel:
                rangesDB = getInputRangesIntelJson(packet.features.cpu, inputRanges);
                break;
#endif
#ifdef WITH_AMD
            case PWTS::CPUVendor::AMD:
                rangesDB = getInputRangesAMDJson(packet.features.cpu, inputRanges);
                break;
#endif
            default:
                break;
        }

        jDaemon.insert("settings", getDaemonSettingsJson(daemonSettings));
        jobj.insert("daemon", jDaemon);
        jobj.insert("input_ranges", rangesDB);

        return jobj;
    }

#ifdef WITH_INTEL
    static void setIntelCoreDataJson(const QSharedPointer<PWTS::Intel::IntelData> &data, const PWTS::Features &features, QJsonObject &jobj) {
        QJsonObject pkgCstConfig;
        int i = 0;

        // gather all data
        for (const PWTS::Intel::IntelCoreData &core: data->coreData) {
            const QString cpuStr = QString("core_%1").arg(i++);

            if (features.cpu.contains(PWTS::Feature::INTEL_CPU_GROUP)) {
                if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL) && core.pkgCstConfigControl.isValid()) {
                    const PWTS::Intel::PkgCstConfigControl val = core.pkgCstConfigControl.getValue();
                    QJsonObject obj;

                    if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL_SB)) {
                        obj.insert("package_cstate_limit", val.packageCStateLimit);
                        obj.insert("io_mwait_redirection_enable", val.ioMwaitRedirectionEnable);
                        obj.insert("cfg_lock", val.cfgLock);
                        obj.insert("c3_state_auto_demotion_enable", val.c3StateAutodemotionEnable);
                        obj.insert("c1_state_auto_demotion_enable", val.c1StateAutodemotionEnable);
                        obj.insert("c3_undemotion_enable", val.c3UndemotionEnable);
                        obj.insert("c1_undemotion_enable", val.c1UndemotionEnable);

                    } else if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL_CU1)) {
                        obj.insert("package_cstate_limit", val.packageCStateLimit);
                        obj.insert("max_core_cstate", val.maxCoreCState);
                        obj.insert("io_mwait_redirection_enable", val.ioMwaitRedirectionEnable);
                        obj.insert("cfg_lock", val.cfgLock);
                        obj.insert("c3_state_auto_demotion_enable", val.c3StateAutodemotionEnable);
                        obj.insert("c1_state_auto_demotion_enable", val.c1StateAutodemotionEnable);
                        obj.insert("c3_undemotion_enable", val.c3UndemotionEnable);
                        obj.insert("c1_undemotion_enable", val.c1UndemotionEnable);
                        obj.insert("package_cstate_auto_demotion_enable", val.pkgcAutodemotionEnable);
                        obj.insert("package_cstate_undemotion_enable", val.pkgcUndemotionEnable);
                        obj.insert("timed_mwait_enable", val.timedMwaitEnable);
                    }

                    pkgCstConfig.insert(cpuStr, obj);
                }
            }
        }

        // insert data into main obj
        if (features.cpu.contains(PWTS::Feature::INTEL_CPU_GROUP)) {
            if (!pkgCstConfig.isEmpty())
                jobj.insert("pkg_cst_config_control", pkgCstConfig);
        }
    }

    static void setIntelThreadDataJson(const QSharedPointer<PWTS::Intel::IntelData> &data, const PWTS::Features &features, QJsonObject &jobj) {
        QJsonObject hwpObj;
        int i = 0;

        // gather all data
        for (const PWTS::Intel::IntelThreadData &thd: data->threadData) {
            const QString cpuStr = QString("cpu_%1").arg(i++);

            if (features.cpu.contains(PWTS::Feature::INTEL_HWP_GROUP)) {
                QJsonObject hwpCpuObj;

                if (thd.hwpCapapabilities.isValid()) {
                    const PWTS::Intel::HWPCapabilities val = thd.hwpCapapabilities.getValue();
                    QJsonObject obj;

                    obj.insert("lowest_performance", val.lowestPerf);
                    obj.insert("highest_performance", val.highestPerf);
                    hwpCpuObj.insert("capabilities", obj);
                }

                if (thd.hwpRequest.isValid()) {
                    const PWTS::Intel::HWPRequest val = thd.hwpRequest.getValue();
                    QJsonObject obj;

                    obj.insert("min_performance", val.requestPkg.min);
                    obj.insert("max_performance", val.requestPkg.max);
                    obj.insert("desired_performance", val.requestPkg.desired);

                    if (features.cpu.contains(PWTS::Feature::INTEL_HWP_ACT_WIND))
                        obj.insert("activity_window", val.requestPkg.acw);

                    if (features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP))
                        obj.insert("energy_performance_preference", val.requestPkg.epp);

                    if (features.cpu.contains(PWTS::Feature::INTEL_HWP_VALID_BITS)) {
                        obj.insert("min_valid", val.minValid);
                        obj.insert("max_valid", val.maxValid);
                        obj.insert("desired_valid", val.desiredValid);

                        if (features.cpu.contains(PWTS::Feature::INTEL_HWP_ACT_WIND))
                            obj.insert("activity_window_valid", val.acwValid);

                        if (features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP))
                            obj.insert("energy_performance_preference_valid", val.eppValid);

                        if (features.cpu.contains(PWTS::Feature::INTEL_HWP_REQ_PKG))
                            obj.insert("package_control", val.packageControl);
                    }

                    hwpCpuObj.insert("request", obj);
                }

                if (!hwpCpuObj.isEmpty())
                    hwpObj.insert(cpuStr, hwpCpuObj);
            }
        }

        // insert data into main obj
        if (features.cpu.contains(PWTS::Feature::INTEL_HWP_GROUP)) {
            if (!hwpObj.isEmpty())
                jobj.insert("hwp_request", hwpObj);
        }
    }

    [[nodiscard]]
    static QJsonObject getIntelDataJson(const QSharedPointer<PWTS::Intel::IntelData> &data, const PWTS::Features &features, const int coreCount) {
        QJsonObject jobj;

        setIntelCoreDataJson(data, features, jobj);
        setIntelThreadDataJson(data, features, jobj);

        if (features.cpu.contains(PWTS::Feature::INTEL_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::INTEL_VR_CURRENT_CFG) && data->vrCurrentCfg.isValid()) {
                const PWTS::Intel::VRCurrentConfig val = data->vrCurrentCfg.getValue();
                QJsonObject obj;

                obj.insert("lock", val.lock);
                obj.insert("pl4_limit", val.pl4);
                jobj.insert("vr_current_config", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_CPU_POWER_BALANCE) && data->pp0Priority.isValid())
                jobj.insert("pp0_priority", data->pp0Priority.getValue());

            if (features.cpu.contains(PWTS::Feature::INTEL_GPU_POWER_BALANCE) && data->pp1Priority.isValid())
                jobj.insert("pp1_priority", data->pp1Priority.getValue());

            if (features.cpu.contains(PWTS::Feature::INTEL_ENERGY_PERF_BIAS) && data->energyPerfBias.isValid())
                jobj.insert("performance_energy_bias_hint", data->energyPerfBias.getValue());

            if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_GROUP) && data->undervoltData.isValid()) {
                const PWTS::Intel::FIVRControlUV val = data->undervoltData.getValue();
                QJsonObject obj;

                if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_CPU))
                    obj.insert("cpu", val.cpu);

                if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_GPU))
                    obj.insert("gpu", val.gpu);

                if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_CACHE))
                    obj.insert("cache", val.cpuCache);

                if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_SYSAGENT))
                    obj.insert("system_agent", val.sa);

                if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_UNSLICE))
                    obj.insert("unslice", val.unslice);

                jobj.insert("fivr", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_MISC_PWR_MGMT) && data->miscPwrMgmt.isValid()) {
                const PWTS::Intel::MiscPwrMgmt val = data->miscPwrMgmt.getValue();
                QJsonObject obj;

                if (features.cpu.contains(PWTS::Feature::INTEL_MISC_PWR_MGMT_NHLM))
                    obj.insert("eist_hardware_coordination_disable", val.eistHWCoordinationDisable);

                jobj.insert("misc_pwr_mgmt", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_IA32_MISC_ENABLE_GROUP) && data->miscProcFeatures.isValid()) {
                const PWTS::Intel::MiscProcFeatures val = data->miscProcFeatures.getValue();
                QJsonObject obj;

                if (features.cpu.contains(PWTS::Feature::INTEL_ENHANCED_SPEEDSTEP))
                    obj.insert("enhanced_speedstep", val.enhancedSpeedStep);

                if (features.cpu.contains(PWTS::Feature::INTEL_TURBO_BOOST))
                    obj.insert("turbo_boost_disable", val.disableTurboMode);

                jobj.insert("misc_processor_features", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_POWER_CTL) && data->powerCtl.isValid()) {
                const PWTS::Intel::PowerCtl val = data->powerCtl.getValue();
                QJsonObject obj;

                if (features.cpu.contains(PWTS::Feature::INTEL_POWER_CTL_NHLM)) {
                    obj.insert("c1_enhanced_enable", val.c1eEnable);

                } else if (features.cpu.contains(PWTS::Feature::INTEL_POWER_CTL_SB)) {
                    obj.insert("bidirectional_prochot", val.bdProcHot);
                    obj.insert("c1_enhanced_enable", val.c1eEnable);
                    obj.insert("energy_efficiency_optimization_disable", val.disableEnergyEfficiencyOpt);
                    obj.insert("race_to_halt_optimization_disable", val.disableRaceToHaltOpt);

                } else if (features.cpu.contains(PWTS::Feature::INTEL_POWER_CTL_CU1)) {
                    obj.insert("bidirectional_prochot", val.bdProcHot);
                    obj.insert("c1_enhanced_enable", val.c1eEnable);
                    obj.insert("self_refresh_pkg_c2_state", val.sapmImcC2Policy);
                    obj.insert("fast_vid_swing_rate", val.fastBrkSnpEn);
                    obj.insert("power_perf_platform_override", val.powerPerformancePlatformOverride);
                    obj.insert("energy_efficiency_optimization_disable", val.disableEnergyEfficiencyOpt);
                    obj.insert("race_to_halt_optimization_disable", val.disableRaceToHaltOpt);
                    obj.insert("prochot_output_disable", val.prochotOutputDisable);
                    obj.insert("prochot_configurable_response_enable", val.prochotConfigurableResponseEnable);
                    obj.insert("prochot_bits_lock_enable", val.vrThermAlertDisableLock);
                    obj.insert("vr_therm_alert_signaling_disable", val.vrThermAlertDisable);
                    obj.insert("ring_ee_disable", val.ringEEDisable);
                    obj.insert("sa_opt_disable", val.saOptimizationDisable);
                    obj.insert("ook_disable", val.ookDisable);
                    obj.insert("hwp_autonomous_disable", val.hwpAutonomousDisable);
                    obj.insert("cstate_prewake_disable", val.cstatePrewakeDisable);
                }

                jobj.insert("power_ctl", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_PKG_POWER_LIMIT) && data->pkgPowerLimit.isValid()) {
                const PWTS::Intel::PkgPowerLimit val = data->pkgPowerLimit.getValue();
                QJsonObject obj;

                obj.insert("lock", val.lock);
                obj.insert("pl1_limit", val.pl1);
                obj.insert("pl1_time", val.pl1Time);
                obj.insert("pl1_clamp", val.pl1Clamp);
                obj.insert("pl1_enable", val.pl1Enable);
                obj.insert("pl2_limit", val.pl2);
                obj.insert("pl2_time", val.pl2Time);
                obj.insert("pl2_clamp", val.pl2Clamp);
                obj.insert("pl2_enable", val.pl2Enable);
                jobj.insert("pkg_power_limit", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_PP1_CURRENT_CFG) && data->pp1CurrentCfg.isValid()) {
                const PWTS::Intel::PP1CurrentConfig val = data->pp1CurrentCfg.getValue();
                QJsonObject obj;

                obj.insert("lock", val.lock);
                obj.insert("power_limit", val.limit);
                jobj.insert("pp1_current_config", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_TURBO_POWER_CURRENT_LIMIT) && data->turboPowerCurrentLimit.isValid()) {
                const PWTS::Intel::TurboPowerCurrentLimit val = data->turboPowerCurrentLimit.getValue();
                QJsonObject obj;

                obj.insert("tdp_limit", val.tdpLimit);
                obj.insert("tdp_limit_override_enable", val.tdpLimitOverride);
                obj.insert("tdc_limit", val.tdcLimit);
                obj.insert("tdc_limit_override_enable", val.tdcLimitOverride);
                jobj.insert("turbo_power_current_limit", obj);
            }

            if (features.cpu.contains(PWTS::Feature::INTEL_TURBO_RATIO_LIMIT) && data->turboRatioLimit.isValid()) {
                const PWTS::Intel::TurboRatioLimit val = data->turboRatioLimit.getValue();
                const QList<int> ratioList = {
                    val.maxRatioLimit1C, val.maxRatioLimit2C, val.maxRatioLimit3C,
                    val.maxRatioLimit4C, val.maxRatioLimit5C, val.maxRatioLimit6C,
                    val.maxRatioLimit7C, val.maxRatioLimit8C
                };
                QJsonObject obj;

                for (int i=0,l=ratioList.size(); i<coreCount && i<l; ++i)
                    obj.insert(QString("max_ratio_limit_%1_core").arg(i+1), ratioList[i]);

                jobj.insert("turbo_ratio_limit", obj);
            }
        }

        if (features.cpu.contains(PWTS::Feature::INTEL_HWP_GROUP)) {
            if (data->hwpEnable.isValid())
                jobj.insert("hwp_enable", data->hwpEnable.getValue());

            if (features.cpu.contains(PWTS::Feature::INTEL_HWP_CTL) && data->hwpPkgCtlPolarity.isValid())
                jobj.insert("hwp_pkg_ctl_polarity_enable", data->hwpPkgCtlPolarity.getValue());

            if (features.cpu.contains(PWTS::Feature::INTEL_HWP_REQ_PKG) && data->hwpRequestPkg.isValid()) {
                const PWTS::Intel::HWPRequestPkg val = data->hwpRequestPkg.getValue();
                QJsonObject pkgObj;

                pkgObj.insert("min_performance", val.min);
                pkgObj.insert("max_performance", val.max);
                pkgObj.insert("desired_performance", val.desired);

                if (features.cpu.contains(PWTS::Feature::INTEL_HWP_ACT_WIND))
                    pkgObj.insert("activity_window", val.acw);

                if (features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP))
                    pkgObj.insert("energy_performance_preference", val.epp);

                jobj.insert("hwp_request_pkg", pkgObj);
            }
        }

        if (features.cpu.contains(PWTS::Feature::INTEL_MCHBAR_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::INTEL_MCHBAR_PKG_RAPL_LIMIT) && data->mchbarPkgRaplLimit.isValid()) {
                const PWTS::Intel::MCHBARPkgRaplLimit val = data->mchbarPkgRaplLimit.getValue();
                QJsonObject raplObj;

                if (features.cpu.contains(PWTS::Feature::INTEL_MCHBAR_PKG_RAPL_LIMIT_IVB)) {
                    raplObj.insert("pl1", val.pl1);
                    raplObj.insert("pl2", val.pl2);
                    raplObj.insert("pl1_enable", val.pl1Enable);
                    raplObj.insert("pl2_enable", val.pl2Enable);
                    raplObj.insert("pl1_time", val.pl1Time);
                    raplObj.insert("lock", val.lock);

                } else if (features.cpu.contains(PWTS::Feature::INTEL_MCHBAR_PKG_RAPL_LIMIT_TGL)) {
                    raplObj.insert("pl1", val.pl1);
                    raplObj.insert("pl2", val.pl2);
                    raplObj.insert("pl1_clamp", val.pl1Clamp);
                    raplObj.insert("pl1_enable", val.pl1Enable);
                    raplObj.insert("pl2_enable", val.pl2Enable);
                    raplObj.insert("pl1_time", val.pl1Time);
                    raplObj.insert("lock", val.lock);
                }

                jobj.insert("mchbar_pkg_rapl_power_limits", raplObj);
            }
        }

        return jobj;
    }
#endif

#ifdef WITH_AMD
    static void setAmdCoreDataJson(const QSharedPointer<PWTS::AMD::AMDData> &data, const PWTS::Features &features, QJsonObject &jobj) {
        QJsonObject curveCoreObj;
        int i = 0;

        // gather all data
        for (const PWTS::AMD::AMDCoreData &core: data->coreData) {
            const QString coreStr = QString("core_%1").arg(i++);

            if (features.cpu.contains(PWTS::Feature::AMD_CPU_RY_GROUP)) {
                if (features.cpu.contains(PWTS::Feature::AMD_RY_CO_PER) && core.curveOptimizer.isValid())
                    curveCoreObj.insert(coreStr, core.curveOptimizer.getValue());
            }
        }

        // insert data into main obj
        if (features.cpu.contains(PWTS::Feature::AMD_CPU_RY_GROUP)) {
            if (!curveCoreObj.isEmpty())
                jobj.insert("curve_optimizer_core", curveCoreObj);
        }
    }

    static void setAmdThreadDataJson(const QSharedPointer<PWTS::AMD::AMDData> &data, const PWTS::Features &features, QJsonObject &jobj) {
        QJsonObject hwPstateObj;
        QJsonObject boostObj;
        QJsonObject cppcObj;
        int i = 0;

        // gather all data
        for (const PWTS::AMD::AMDThreadData &thd: data->threadData) {
            const QString cpuStr = QString("cpu_%1").arg(i++);

            if (features.cpu.contains(PWTS::Feature::AMD_CPU_GROUP)) {
                if (features.cpu.contains(PWTS::Feature::AMD_HWPSTATE) && thd.pstateCmd.isValid())
                    hwPstateObj.insert(cpuStr, thd.pstateCmd.getValue());

                if (features.cpu.contains(PWTS::Feature::AMD_CORE_PERFORMANCE_BOOST) && thd.corePerfBoost.isValid())
                    boostObj.insert(QString("%1_disable").arg(cpuStr), thd.corePerfBoost.getValue());

                if (features.cpu.contains(PWTS::Feature::AMD_CPPC)) {
                    QJsonObject cppcThdObj;

                    if (thd.cppcCapability1.isValid()) {
                        const PWTS::AMD::CPPCCapability1 val = thd.cppcCapability1.getValue();
                        QJsonObject obj;

                        obj.insert("lowest_performance", val.lowestPerf);
                        obj.insert("highest_performance", val.highestPerf);
                        obj.insert("low_non_liner_performance", val.lowNonLinPerf);
                        obj.insert("nominal_performance", val.nominalPerf);
                        cppcThdObj.insert("capability_1", obj);
                    }

                    if (thd.cppcRequest.isValid()) {
                        const PWTS::AMD::CPPCRequest val = thd.cppcRequest.getValue();
                        QJsonObject obj;

                        obj.insert("min_performance", val.minPerf);
                        obj.insert("max_performance", val.maxPerf);
                        obj.insert("desired_performance", val.desPerf);
                        obj.insert("energy_performance_preference", val.epp);
                        cppcThdObj.insert("request", obj);
                    }

                    cppcObj.insert(cpuStr, cppcThdObj);
                }
            }
        }

        // insert data into main obj
        if (features.cpu.contains(PWTS::Feature::AMD_CPU_GROUP)) {
            if (!hwPstateObj.isEmpty())
                jobj.insert("hw_pstate", hwPstateObj);

            if (!boostObj.isEmpty())
                jobj.insert("core_performance_boost", boostObj);

            if (!cppcObj.isEmpty())
                jobj.insert("cppc_request", cppcObj);
        }
    }

    [[nodiscard]]
    static QJsonObject getAmdDataJson(const QSharedPointer<PWTS::AMD::AMDData> &data, const PWTS::Features &features) {
        QJsonObject jobj;

        setAmdCoreDataJson(data, features, jobj);
        setAmdThreadDataJson(data, features, jobj);

        if (features.cpu.contains(PWTS::Feature::AMD_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_HWPSTATE) && data->pstateCurrentLimit.isValid()) {
                const PWTS::AMD::PStateCurrentLimit val = data->pstateCurrentLimit.getValue();
                QJsonObject obj;

                obj.insert("current_pstate_limit", val.curPStateLimit);
                obj.insert("pstate_max_value", val.pstateMaxValue);
                jobj.insert("hw_pstate_limits", obj);
            }

            if (features.cpu.contains(PWTS::Feature::AMD_CPPC) && data->cppcEnableBit.isValid())
                jobj.insert("cppc_enable", data->cppcEnableBit.getValue());
        }

        if (features.cpu.contains(PWTS::Feature::AMD_CPU_RY_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_RY_APU_SKIN_TEMP) && data->apuSkinTemp.isValid())
                jobj.insert("apu_skin_temp", data->apuSkinTemp.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_APU_SLOW) && data->apuSlow.isValid())
                jobj.insert("apu_slow_limit", data->apuSlow.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_DGPU_SKIN_TEMP) && data->dgpuSkinTemp.isValid())
                jobj.insert("dgpu_skin_temp", data->dgpuSkinTemp.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_FAST_LIMIT) && data->fastLimit.isValid())
                jobj.insert("fast_limit", data->fastLimit.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_MIN_GFX_CLOCK) && data->minGfxClock.isValid())
                jobj.insert("min_gfx_clock", data->minGfxClock.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_MAX_GFX_CLOCK) && data->maxGfxClock.isValid())
                jobj.insert("max_gfx_clock", data->maxGfxClock.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_SLOW_LIMIT) && data->slowLimit.isValid())
                jobj.insert("slow_limit", data->slowLimit.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_STAPM_LIMIT) && data->stapmLimit.isValid())
                jobj.insert("stapm_limit", data->stapmLimit.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_TCTL_TEMP) && data->tctlTemp.isValid())
                jobj.insert("tctl_temp", data->tctlTemp.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_CURRENT) && data->vrmCurrent.isValid())
                jobj.insert("vrm_current", data->vrmCurrent.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_MAX_CURRENT) && data->vrmMaxCurrent.isValid())
                jobj.insert("vrm_max_current", data->vrmMaxCurrent.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_SOC_CURRENT) && data->vrmSocCurrent.isValid())
                jobj.insert("vrm_soc_current", data->vrmSocCurrent.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_SOC_MAX_CURRENT) && data->vrmSocMaxCurrent.isValid())
                jobj.insert("vrm_soc_max_current", data->vrmSocMaxCurrent.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_POWER_PROFILE) && data->powerProfile.isValid())
                jobj.insert("power_profile", data->powerProfile.getValue());

            if (features.cpu.contains(PWTS::Feature::AMD_RY_CO_ALL) && data->curveOptimizer.isValid())
                jobj.insert("curve_optimizer_all", data->curveOptimizer.getValue());
        }

        return jobj;
    }
#endif

    [[nodiscard]]
    static QJsonObject getLinuxIntelGPUDataJson(const int index, const PWTS::LNX::LinuxIntelGPUData &data, const QSet<PWTS::Feature> &features) {
        QJsonObject jobj;

        jobj.insert("gpu_index", index);

        if (features.contains(PWTS::Feature::INTEL_GPU_SYSFS_GROUP)) {
            if (data.rpsLimits.isValid()) {
                const PWTS::LNX::Intel::GPURPSLimits val = data.rpsLimits.getValue();
                QJsonObject obj;

                obj.insert("rp0", val.rp0);
                obj.insert("rpn", val.rpn);
                jobj.insert("rps_limits", obj);
            }

            if (features.contains(PWTS::Feature::INTEL_GPU_RPS_FREQ_SYSFS) && data.frequency.isValid()) {
                const PWTS::MinMax val = data.frequency.getValue();
                QJsonObject obj;

                obj.insert("min", val.min);
                obj.insert("max", val.max);
                jobj.insert("frequency", obj);
            }

            if (features.contains(PWTS::Feature::INTEL_GPU_BOOST_SYSFS) && data.boostFrequency.isValid())
                jobj.insert("boost_frequency", data.boostFrequency.getValue());
        }

        return jobj;
    }

    [[nodiscard]]
    static QJsonObject getLinuxAMDGPUDataJson(const int index, const PWTS::LNX::LinuxAMDGPUData &data, const QSet<PWTS::Feature> &features) {
        QJsonObject jobj;

        jobj.insert("gpu_index", index);

        if (features.contains(PWTS::Feature::AMD_GPU_SYSFS_GROUP)) {
            if (features.contains(PWTS::Feature::AMD_GPU_DPM_FORCE_PERF_LEVEL_SYSFS)) {
                if (data.odRanges.isValid()) {
                    const PWTS::LNX::AMD::GPUODRanges val = data.odRanges.getValue();
                    QJsonObject sclkObj;
                    QJsonObject obj;

                    sclkObj.insert("min", val.sclk.min);
                    sclkObj.insert("max", val.sclk.max);
                    obj.insert("sclk", sclkObj);
                    jobj.insert("od_ranges", obj);
                }

                if (data.dpmForcePerfLevel.isValid()) {
                    const PWTS::LNX::AMD::GPUDPMForcePerfLevel val = data.dpmForcePerfLevel.getValue();
                    QJsonObject sclkObj;
                    QJsonObject obj;

                    sclkObj.insert("min", val.sclk.min);
                    sclkObj.insert("max", val.sclk.max);
                    obj.insert("sclk", sclkObj);
                    obj.insert("level", val.level);
                    jobj.insert("dpm_force_performance_level", obj);
                }
            }

            if (features.contains(PWTS::Feature::AMD_GPU_POWER_DPM_STATE_SYSFS) && data.powerDpmState.isValid())
                jobj.insert("power_dpm_state", data.powerDpmState.getValue());
        }

        return jobj;
    }

    static void setLinuxThreadDataJson(const QSharedPointer<PWTS::LNX::LinuxData> &data, const PWTS::Features &features, QJsonObject &jobj) {
        QJsonObject scalingGovObj;
        QJsonObject cpuFreqObj;
        QJsonObject cpuStatusObj;
        int i = 0;

        // gather all data
        for (const PWTS::LNX::LinuxThreadData &thd: data->threadData) {
            const QString cpuStr = QString("cpu_%1").arg(i++);

            if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
                if (features.cpu.contains(PWTS::Feature::CPUFREQ_SYSFS)) {
                    QJsonObject scalingGovThdObj;
                    QJsonObject cpuFreqThdObj;

                    if (thd.scalingAvailableGovernors.isValid()) {
                        const PWTS::LNX::CPUScalingAvailableGovernors val = thd.scalingAvailableGovernors.getValue();

                        scalingGovThdObj.insert("available_governors", QJsonArray::fromStringList(val.availableGovernors));
                        scalingGovThdObj.insert("related_cpus", QJsonArray::fromStringList(val.relatedCPUs));
                    }

                    if (thd.scalingGovernor.isValid())
                        scalingGovThdObj.insert("governor", thd.scalingGovernor.getValue());

                    if (thd.cpuFrequencyLimits.isValid()) {
                        const PWTS::LNX::CPUFrequencyLimits val = thd.cpuFrequencyLimits.getValue();
                        QJsonObject obj;

                        obj.insert("min", val.limit.min);
                        obj.insert("max", val.limit.max);
                        obj.insert("related_cpus", QJsonArray::fromStringList(val.relatedCPUs));
                        cpuFreqThdObj.insert("limits", obj);
                    }

                    if (thd.cpuFrequency.isValid()) {
                        const PWTS::MinMax val = thd.cpuFrequency.getValue();
                        QJsonObject obj;

                        obj.insert("min", val.min);
                        obj.insert("max", val.max);
                        cpuFreqThdObj.insert("frequency", obj);
                    }

                    if (!scalingGovThdObj.isEmpty())
                        scalingGovObj.insert(cpuStr, scalingGovThdObj);

                    if (!cpuFreqThdObj.isEmpty())
                        cpuFreqObj.insert(cpuStr, cpuFreqThdObj);
                }

                if (features.cpu.contains(PWTS::Feature::CPU_PARK_SYSFS)) {
                    QJsonObject obj;

                    if (thd.cpuLogicalOffAvailable.isValid())
                        obj.insert("logical_off_support", thd.cpuLogicalOffAvailable.getValue());

                    if (thd.cpuOnlineStatus.isValid())
                        obj.insert("online_status", thd.cpuOnlineStatus.getValue());

                    if (thd.coreID.isValid())
                        obj.insert("real_core_id", thd.coreID.getValue());

                    if (!obj.isEmpty())
                        cpuStatusObj.insert(cpuStr, obj);
                }
            }
        }

        // insert data into main obj
        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (!scalingGovObj.isEmpty())
                jobj.insert("scaling_governor", scalingGovObj);

            if (!cpuFreqObj.isEmpty())
                jobj.insert("cpu_frequency", cpuFreqObj);

            if (!cpuStatusObj.isEmpty())
                jobj.insert("cpu_online_status", cpuStatusObj);
        }
    }

    [[nodiscard]]
    static QJsonObject getLinuxDataJson(const QSharedPointer<PWTS::LNX::LinuxData> &data, const PWTS::Features &features) {
        QJsonArray intelGpusArr;
        QJsonArray amdGpusArr;
        QJsonObject blockQueSchedObj;
        QJsonObject miscPmDevsObj;
        QJsonObject jobj;

        setLinuxThreadDataJson(data, features, jobj);

        for (const auto &[blkDev, devData]: data->blockDevicesQueSched.asKeyValueRange()) {
            QJsonObject obj;

            obj.insert("label", devData.name);
            obj.insert("scheduler", devData.scheduler);
            obj.insert("available_schedulers", QJsonArray::fromStringList(devData.availableQueueSchedulers));
            blockQueSchedObj.insert(blkDev, obj);
        }

        for (const PWTS::LNX::MiscPMDevice &dev: data->miscPMDevices) {
            QJsonObject obj;

            obj.insert("runtime", dev.controlValue);
            obj.insert("name", dev.name);
            miscPmDevsObj.insert(dev.control, obj);
        }

        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::CPU_SMT_SYSFS) && data->smtState.isValid())
                jobj.insert("smt", data->smtState.getValue());

            if (features.cpu.contains(PWTS::Feature::CPUIDLE_GOV_SYSFS) && data->cpuIdleGovernor.isValid()) {
                QJsonObject obj;

                if (data->cpuIdleAvailableGovernors.isValid())
                    obj.insert("available_governors", QJsonArray::fromStringList(data->cpuIdleAvailableGovernors.getValue()));

                obj.insert("governor", data->cpuIdleGovernor.getValue());
                jobj.insert("cpu_idle", obj);
            }
        }

        for (const auto &[index, gpuData]: data->intelGpuData.asKeyValueRange())
            intelGpusArr.append(getLinuxIntelGPUDataJson(index, gpuData, features.gpus[index].second));

        for (const auto &[index, gpuData]: data->amdGpuData.asKeyValueRange())
            amdGpusArr.append(getLinuxAMDGPUDataJson(index, gpuData, features.gpus[index].second));

        if (!intelGpusArr.isEmpty())
            jobj.insert("intel_gpus", intelGpusArr);

        if (!amdGpusArr.isEmpty())
            jobj.insert("amd_gpus", amdGpusArr);

        if (!blockQueSchedObj.isEmpty())
            jobj.insert("block_devices_queue_scheduler", blockQueSchedObj);

        if (!miscPmDevsObj.isEmpty())
            jobj.insert("misc_pm_devices", miscPmDevsObj);

        return jobj;
    }

#ifdef WITH_AMD
    static void setLinuxAMDThreadDataJson(const QSharedPointer<PWTS::LNX::AMD::LinuxAMDData> &data, const PWTS::Features &features, QJsonObject &jobj) {
        QJsonObject pstateObj;
        int i = 0;

        for (const PWTS::LNX::AMD::LinuxAMDThreadData &thd: data->threadData) {
            const QString cpuStr = QString("cpu_%1").arg(i++);

            if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
                if (features.cpu.contains(PWTS::Feature::AMD_PSTATE_SYSFS)) {
                    QJsonObject obj;

                    if (thd.pstateData.isValid()) {
                        const PWTS::LNX::AMD::AMDPStateData val = thd.pstateData.getValue();

                        obj.insert("epp_available_preferences", QJsonArray::fromStringList(val.eppAvailablePrefs));
                    }

                    if (thd.epp.isValid())
                        obj.insert("energy_performance_preference", thd.epp.getValue());

                    pstateObj.insert(cpuStr, obj);
                }
            }
        }

        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (!pstateObj.isEmpty())
                jobj.insert("pstate", pstateObj);
        }
    }

    [[nodiscard]]
    static QJsonObject getLinuxAMDDataJson(const QSharedPointer<PWTS::LNX::AMD::LinuxAMDData> &data, const PWTS::Features &features) {
        QJsonObject jobj;

        setLinuxAMDThreadDataJson(data, features, jobj);

        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_PSTATE_SYSFS) && data->pstateStatus.isValid())
                jobj.insert("pstate_status", data->pstateStatus.getValue());
        }

        return jobj;
    }
#endif

    [[nodiscard]]
    static QJsonObject getWindowsDataJson(const QSharedPointer<PWTS::WIN::WindowsData> &data, const PWTS::Features &features) {
        QJsonObject jobj;

        if (features.cpu.contains(PWTS::Feature::PWR_SCHEME_GROUP)) {
            QJsonObject schemesObj;

            for (const auto &[guidStr, schemeData]: data->schemes.asKeyValueRange()) {
                QJsonObject schemeSettObj;
                QJsonObject schemeDataObj;

                for (const auto &[settingGuid, setting]: schemeData.settings.asKeyValueRange()) {
                    const PWTS::WIN::PowerSchemeSettingData &settData = data->schemeOptionsData[settingGuid];
                    QJsonObject settObj;

                    settObj.insert("group_guid", setting.groupGuid);
                    settObj.insert("value_ac", setting.value.ac);
                    settObj.insert("value_dc", setting.value.dc);
                    settObj.insert("name", settData.name);
                    settObj.insert("group_title", settData.groupTitle);
                    settObj.insert("description", settData.description);
                    settObj.insert("value_unit", settData.valueUnit);
                    settObj.insert("value_increment", settData.valueIncrement);
                    settObj.insert("value_unit", settData.valueUnit);

                    if (settData.isRangeDefined) {
                        settObj.insert("value_min", settData.range.min);
                        settObj.insert("value_max", settData.range.max);
                    }

                    if (!settData.options.isEmpty()) {
                        QJsonArray settOptionsArr;

                        for (const QString &option: settData.options)
                            settOptionsArr.append(option);

                        settObj.insert("options", settOptionsArr);
                    }

                    schemeSettObj.insert(settingGuid, settObj);
                }

                schemeDataObj.insert("name", schemeData.friendlyName);
                schemeDataObj.insert("settings", schemeSettObj);
                schemesObj.insert(guidStr, schemeDataObj);
            }

            jobj.insert("active_scheme", data->activeScheme);

            if (!schemesObj.isEmpty())
                jobj.insert("schemes", schemesObj);
        }

        return jobj;
    }

    [[nodiscard]]
    static QJsonObject getFansDataJson(const PWTS::DaemonPacket &packet, const PWTS::Features &features) {
        if (features.fans.isEmpty())
            return {};

        QJsonObject fansObj;

        for (const auto &[fanID, fan]: packet.fanData.asKeyValueRange()) {
            const int mode = fan.mode.getValue();
            QJsonObject fanObj;
            QJsonObject curveObj;

            fanObj.insert("mode", mode);
            fanObj.insert("mode_str", mode == 0 ? "auto":"manual");

            for (const auto &[temp, speed]: fan.curve)
                curveObj.insert(QString("%1C").arg(temp), speed);

            fanObj.insert("curve", curveObj);
            fansObj.insert(QString("fan_%1").arg(fanID), fanObj);
        }

        return fansObj;
    }

    [[nodiscard]]
    static QJsonObject getDeviceDataJson(const PWTS::DaemonPacket &packet, const PWTS::Features &features, const int coreCount) {
        QTextStream ts(stdout, QIODevice::WriteOnly);
        QJsonObject jobj;

        switch (packet.vendor) {
#ifdef WITH_INTEL
            case PWTS::CPUVendor::Intel:
                jobj.insert("intel", getIntelDataJson(packet.intelData, features, coreCount));
                break;
#endif
#ifdef WITH_AMD
            case PWTS::CPUVendor::AMD: {
                jobj.insert("amd", getAmdDataJson(packet.amdData, features));

                switch (packet.os) {
                    case PWTS::OSType::Linux:
                        jobj.insert("linux_amd", getLinuxAMDDataJson(packet.linuxAmdData, features));
                        break;
                    default:
                        break;
                }
            }
                break;
#endif
            default:
                break;
        }

        switch (packet.os) {
            case PWTS::OSType::Linux:
                jobj.insert("linux", getLinuxDataJson(packet.linuxData, features));
                break;
            case PWTS::OSType::Windows:
                jobj.insert("windows", getWindowsDataJson(packet.windowsData, features));
                break;
            default:
                break;
        }

        jobj.insert("fan_control", getFansDataJson(packet, features));

        return jobj;
    }

    void printDataPath(const QString &path) {
        QTextStream ts(stdout, QIODevice::WriteOnly);
        QJsonObject jobj;

        jobj.insert("path", path);

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }

    void printDaemons(const QJsonArray &daemons) {
        QTextStream ts(stdout, QIODevice::WriteOnly);
        QJsonObject jobj;

        jobj.insert("daemons", daemons);

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }

    void printDeviceInfo(const PWTS::DeviceInfoPacket &packet, const QSharedPointer<FileLogger> &logger, const QSharedPointer<UI::InputRanges> &inputRanges) {
        const QSharedPointer<PWTS::DaemonSettings> daemonSettings = QSharedPointer<PWTS::DaemonSettings>::create();
        const QJsonObject jobj = getDeviceInfoJson(packet, daemonSettings, inputRanges);
        QTextStream ts(stdout, QIODevice::WriteOnly);

        if (!daemonSettings->load(packet.daemonSettings))
            logger->write(QStringLiteral("failed to load daemon settings, using defaults"));

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }

    void printDeviceData(const PWTS::DaemonPacket &packet, const PWTS::Features &features, const int coreCount) {
        const QJsonObject jobj = getDeviceDataJson(packet, features, coreCount);
        QTextStream ts(stdout, QIODevice::WriteOnly);

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }

    void printDaemonSettings(const QSharedPointer<PWTS::DaemonSettings> &daemonSettings) {
        const QJsonObject jobj = getDaemonSettingsJson(daemonSettings);
        QTextStream ts(stdout, QIODevice::WriteOnly);

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }

    void printProfileList(const QList<QString> &list) {
        const QJsonArray jarr = QJsonArray::fromStringList(list);
        QTextStream ts(stdout, QIODevice::WriteOnly);
        QJsonObject jobj;

        jobj.insert("profiles", jarr);

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }

    void printApplyResults(const QSet<PWTS::DError> &errors, const QString &profile) {
        QTextStream ts(stdout, QIODevice::WriteOnly);
        QJsonObject jobj;
        QJsonArray errList;

        for (const PWTS::DError &err: errors)
            errList.append(PWTS::getErrorStr(err));

        if (!profile.isEmpty())
            jobj.insert("profile", profile);

        jobj.insert("errors", errList);

        ts << QJsonDocument(jobj).toJson().toStdString().c_str();
    }
}
