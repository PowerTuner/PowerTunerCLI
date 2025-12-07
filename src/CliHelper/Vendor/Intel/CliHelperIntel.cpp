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
#include "CliHelperIntel.h"
#include "../../../CMDParser/SettingsArguments.h"

namespace PWT::CLI {
    CliHelperIntel::CliHelperIntel(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const int coreCount,
                                    const QSharedPointer<PWTS::Intel::IntelData> &data, const QSharedPointer<UI::InputRanges> &ranges): CliHelper(cmd, daemonFeatures) {
        packetData = data;
        cpuCores = coreCount;
        inputRanges = ranges;
    }

    void CliHelperIntel::setPkgPowerLimit() const {
        const bool hasPl1 = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1Arg);
        const bool hasPl1Time = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1TimeArg);
        const bool hasPl1Clamp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1ClampArg);
        const bool hasPl1Enable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1EnableArg);
        const bool hasPl2 = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2Arg);
        const bool hasPl2Time = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2TimeArg);
        const bool hasPl2Clamp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2ClampArg);
        const bool hasPl2Enable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2EnableArg);
        const bool hasLock = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitLockArg);
        const bool hasArgs = hasPl1 || hasPl1Time || hasPl1Clamp || hasPl1Enable || hasPl2 || hasPl2Time || hasPl2Clamp || hasPl2Enable || hasLock;

        if (!hasArgs)
            return;

        if (!packetData->pkgPowerLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::PkgPowerLimit pkgLimit = packetData->pkgPowerLimit.getValue();
        const PWTS::MinMax range = inputRanges->getIntelPl();

        if (hasPl1) {
            const int pl1 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1Arg).toInt();

            pkgLimit.pl1 = qBound(range.min, pl1, range.max);
        }

        if (hasPl1Time) {
            const int time = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1TimeArg).toInt();
            constexpr int max = 56 * 1000;

            pkgLimit.pl1Time = qBound(0, time, max);
        }

        if (hasPl1Clamp)
            pkgLimit.pl1Clamp = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1ClampArg).toBool();

        if (hasPl1Enable)
            pkgLimit.pl1Enable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl1EnableArg).toBool();

        if (hasPl2) {
            const int pl2 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2Arg).toInt();

            pkgLimit.pl2 = qBound(range.min, pl2, range.max);
        }

        if (hasPl2Time) {
            const int time = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2TimeArg).toInt();
            constexpr int max = 56 * 1000;

            pkgLimit.pl2Time = qBound(0, time, max);
        }

        if (hasPl2Clamp)
            pkgLimit.pl2Clamp = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2ClampArg).toBool();

        if (hasPl2Enable)
            pkgLimit.pl2Enable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitPl2EnableArg).toBool();

        if (hasLock)
            pkgLimit.lock = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgLimitLockArg).toBool();

        packetData->pkgPowerLimit.setValue(pkgLimit, true);
    }

    void CliHelperIntel::setVrCurrentConfig() const {
        const bool hasPl4 = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrCurrentCfgPl4Arg);
        const bool hasLock = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrCurrentCfgLockArg);

        if (!hasPl4 && !hasLock)
            return;

        if (!packetData->vrCurrentCfg.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::VRCurrentConfig vrCfg = packetData->vrCurrentCfg.getValue();

        if (hasPl4) {
            const int pl4 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrCurrentCfgPl4Arg).toInt();
            const PWTS::MinMax range = inputRanges->getIntelPl4();

            vrCfg.pl4 = qBound(range.min, pl4, range.max);
        }

        if (hasLock)
            vrCfg.lock = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrCurrentCfgLockArg).toBool();

        packetData->vrCurrentCfg.setValue(vrCfg, true);
    }

    void CliHelperIntel::setPP1() const {
        const bool hasLimit = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp1CurrentLimitArg);
        const bool hasLock = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp1CurrentLockArg);

        if (!hasLimit && !hasLock)
            return;

        if (!packetData->pp1CurrentCfg.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::PP1CurrentConfig pp1 = packetData->pp1CurrentCfg.getValue();

        if (hasLimit) {
            const int limit = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp1CurrentLimitArg).toInt();
            const PWTS::MinMax range = inputRanges->getIntelPP1();

            pp1.limit = qBound(range.min, limit, range.max);
        }

        if (hasLock)
            pp1.lock = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp1CurrentLockArg).toBool();

        packetData->pp1CurrentCfg.setValue(pp1, true);
    }

    void CliHelperIntel::setTurboPowerLimit() const {
        const bool hasTdp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdpArg);
        const bool hasTdpEnable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdpEnableArg);
        const bool hasTdc = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdcArg);
        const bool hasTdcEnable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdcEnableArg);
        const bool hasArgs = hasTdp || hasTdpEnable || hasTdc || hasTdcEnable;

        if (!hasArgs)
            return;

        if (!packetData->turboPowerCurrentLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::TurboPowerCurrentLimit turboLimit = packetData->turboPowerCurrentLimit.getValue();

        if (hasTdp) {
            const int limit = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdpArg).toInt();
            const PWTS::MinMax range = inputRanges->getIntelTurboPwrCurrentTDP();

            turboLimit.tdpLimit = qBound(range.min, limit, range.max);
        }

        if (hasTdpEnable)
            turboLimit.tdpLimitOverride = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdpEnableArg).toBool();

        if (hasTdc) {
            const int limit = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdcArg).toInt();
            const PWTS::MinMax range = inputRanges->getIntelTurboPwrCurrentTDC();

            turboLimit.tdcLimit = qBound(range.min, limit, range.max);
        }

        if (hasTdcEnable)
            turboLimit.tdcLimitOverride = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboPwrCurrentTdcEnableArg).toBool();

        packetData->turboPowerCurrentLimit.setValue(turboLimit, true);
    }

    void CliHelperIntel::setCPUPowerBalance() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp0PriorityArg))
            return;

        if (!packetData->pp0Priority.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const int pp0 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp0PriorityArg).toInt();

        packetData->pp0Priority.setValue(qBound(0, pp0, 31), true);
    }

    void CliHelperIntel::setGPUPowerBalance() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp1PriorityArg))
            return;

        if (!packetData->pp1Priority.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const int pp1 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pp1PriorityArg).toInt();

        packetData->pp1Priority.setValue(qBound(0, pp1, 31), true);
    }

    void CliHelperIntel::setEnergyPerfBias() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, energyPerfBiasArg))
            return;

        if (!packetData->energyPerfBias.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        packetData->energyPerfBias.setValue(qBound(0, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, energyPerfBiasArg).toInt(), 15), true);
    }

    void CliHelperIntel::setTurboRatioLimit() const {
        const bool hasRatio1c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit1cArg);
        const bool hasRatio2c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit2cArg);
        const bool hasRatio3c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit3cArg);
        const bool hasRatio4c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit4cArg);
        const bool hasRatio5c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit5cArg);
        const bool hasRatio6c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit6cArg);
        const bool hasRatio7c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit7cArg);
        const bool hasRatio8c = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit8cArg);
        const bool hasArgs = hasRatio1c || hasRatio2c || hasRatio3c || hasRatio4c || hasRatio5c || hasRatio6c || hasRatio7c || hasRatio8c;

        if (!hasArgs)
            return;

        if (!packetData->turboRatioLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::TurboRatioLimit turboLimits = packetData->turboRatioLimit.getValue();

        if (hasRatio1c)
            turboLimits.maxRatioLimit1C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit1cArg).toInt(), 255);

        if (hasRatio2c && cpuCores > 1)
            turboLimits.maxRatioLimit2C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit2cArg).toInt(), 255);

        if (hasRatio3c && cpuCores > 2)
            turboLimits.maxRatioLimit3C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit3cArg).toInt(), 255);

        if (hasRatio4c && cpuCores > 3)
            turboLimits.maxRatioLimit4C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit4cArg).toInt(), 255);

        if (hasRatio5c && cpuCores > 4)
            turboLimits.maxRatioLimit5C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit5cArg).toInt(), 255);

        if (hasRatio6c && cpuCores > 5)
            turboLimits.maxRatioLimit6C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit6cArg).toInt(), 255);

        if (hasRatio7c && cpuCores > 6)
            turboLimits.maxRatioLimit7C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit7cArg).toInt(), 255);

        if (hasRatio8c && cpuCores > 7)
            turboLimits.maxRatioLimit8C = qBound(1, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboRatioLimit8cArg).toInt(), 255);

        packetData->turboRatioLimit.setValue(turboLimits, true);
    }

    void CliHelperIntel::setMiscProcFeatures() const {
        const bool hasSpeedstep = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, enhancedSpeedstepEnableArg);
        const bool hasTurboDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboModeDisableArg);
        const bool hasArgs = hasSpeedstep || hasTurboDisable;

        if (!hasArgs)
            return;

        if (!packetData->miscProcFeatures.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::MiscProcFeatures miscFeats = packetData->miscProcFeatures.getValue();

        if (hasSpeedstep && features.cpu.contains(PWTS::Feature::INTEL_ENHANCED_SPEEDSTEP))
            miscFeats.enhancedSpeedStep = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, enhancedSpeedstepEnableArg).toBool();

        if (hasTurboDisable && features.cpu.contains(PWTS::Feature::INTEL_TURBO_BOOST))
            miscFeats.disableTurboMode = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, turboModeDisableArg).toBool();

        packetData->miscProcFeatures.setValue(miscFeats, true);
    }

    void CliHelperIntel::setPowerCtl() const {
        const bool hasBdProcHot = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, bdProchotEnableArg);
        const bool hasC1e = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, c1eEnableArg);
        const bool hasSapmImcC2 = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, sapmImcC2PolicyArg);
        const bool hasFastBrkSnp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, fastBrkSnpEnArg);
        const bool hasPowPerfOvrd = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerPerformancePlatformOverrideArg);
        const bool hasDisableEEO = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, energyEfficiencyOptDisableArg);
        const bool hasDisableRto = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, raceToHaltDisableArg);
        const bool hasProchotOutDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, prochotOutputDisableArg);
        const bool hasProchotCfgRespEn = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, prochotCfgResponseEnableArg);
        const bool hasVrThermAlertLock = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrThermAlertDisableLockArg);
        const bool hasVrThermAlertDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrThermAlertDisableArg);
        const bool hasRingEEDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, ringEEDisableArg);
        const bool hasSAOptDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, saOptDisableArg);
        const bool hasOOKDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, ookDisableArg);
        const bool hasHwpAutoDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpAutonomousDisableArg);
        const bool hasCstatePrewakeDisable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cstatePrewakeDisableArg);
        const bool hasArgs = hasBdProcHot || hasC1e || hasSapmImcC2 || hasFastBrkSnp || hasPowPerfOvrd ||
                            hasDisableEEO || hasDisableRto || hasProchotOutDisable || hasProchotCfgRespEn ||
                            hasVrThermAlertLock || hasVrThermAlertDisable || hasRingEEDisable || hasSAOptDisable ||
                            hasOOKDisable || hasHwpAutoDisable || hasCstatePrewakeDisable;

        if (!hasArgs)
            return;

        if (!packetData->powerCtl.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::PowerCtl powCtl = packetData->powerCtl.getValue();

        if (hasBdProcHot)
            powCtl.bdProcHot = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, bdProchotEnableArg).toBool();

        if (hasC1e)
            powCtl.c1eEnable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, c1eEnableArg).toBool();

        if (hasSapmImcC2)
            powCtl.sapmImcC2Policy = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, sapmImcC2PolicyArg).toBool();

        if (hasFastBrkSnp)
            powCtl.fastBrkSnpEn = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, fastBrkSnpEnArg).toBool();

        if (hasPowPerfOvrd)
            powCtl.powerPerformancePlatformOverride = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerPerformancePlatformOverrideArg).toBool();

        if (hasDisableEEO)
            powCtl.disableEnergyEfficiencyOpt = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, energyEfficiencyOptDisableArg).toBool();

        if (hasDisableRto)
            powCtl.disableRaceToHaltOpt = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, raceToHaltDisableArg).toBool();

        if (hasProchotOutDisable)
            powCtl.prochotOutputDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, prochotOutputDisableArg).toBool();

        if (hasProchotCfgRespEn)
            powCtl.prochotConfigurableResponseEnable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, prochotCfgResponseEnableArg).toBool();

        if (hasVrThermAlertLock)
            powCtl.vrThermAlertDisableLock = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrThermAlertDisableLockArg).toBool();

        if (hasVrThermAlertDisable)
            powCtl.vrThermAlertDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrThermAlertDisableArg).toBool();

        if (hasRingEEDisable)
            powCtl.ringEEDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, ringEEDisableArg).toBool();

        if (hasSAOptDisable)
            powCtl.saOptimizationDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, saOptDisableArg).toBool();

        if (hasOOKDisable)
            powCtl.ookDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, ookDisableArg).toBool();

        if (hasHwpAutoDisable)
            powCtl.hwpAutonomousDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpAutonomousDisableArg).toBool();

        if (hasCstatePrewakeDisable)
            powCtl.cstatePrewakeDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cstatePrewakeDisableArg).toBool();

        packetData->powerCtl.setValue(powCtl, true);
    }

    void CliHelperIntel::setMiscPowerMgmt() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, eistDisableArg))
            return;

        if (!packetData->miscPwrMgmt.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::MiscPwrMgmt miscPwr = packetData->miscPwrMgmt.getValue();

        miscPwr.eistHWCoordinationDisable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, eistDisableArg).toBool();

        packetData->miscPwrMgmt.setValue(miscPwr, true);
    }

    void CliHelperIntel::setFivr() const {
        const bool hasCpuUv = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlCpuArg);
        const bool hasGpuUv = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlGpuArg);
        const bool hasCacheUv = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlCacheArg);
        const bool hasUnsliceUv = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlUnsliceArg);
        const bool hasSaUv = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlSaArg);
        const bool hasArgs = hasCpuUv || hasGpuUv || hasCacheUv || hasUnsliceUv || hasSaUv;

        if (!hasArgs)
            return;

        if (!packetData->undervoltData.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::FIVRControlUV fivr = packetData->undervoltData.getValue();
        const PWTS::MinMax range = inputRanges->getIntelFIVR();

        if (hasCpuUv && features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_CPU))
            fivr.cpu = qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlCpuArg).toInt(), range.max);

        if (hasGpuUv && features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_GPU))
            fivr.gpu = qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlGpuArg).toInt(), range.max);

        if (hasCacheUv && features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_CACHE))
            fivr.cpuCache = qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlCacheArg).toInt(), range.max);

        if (hasUnsliceUv && features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_UNSLICE))
            fivr.unslice = qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlUnsliceArg).toInt(), range.max);

        if (hasSaUv && features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_SYSAGENT))
            fivr.sa = qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, uvControlSaArg).toInt(), range.max);

        packetData->undervoltData.setValue(fivr, true);
    }

    void CliHelperIntel::setHwpEnable() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpEnableArg))
            return;

        if (!packetData->hwpEnable.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        packetData->hwpEnable.setValue(cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpEnableArg).toBool(), true);
    }

    void CliHelperIntel::setHwpPkgCtlPolarityEnable() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpPkgCtlPolarityEnableArg))
            return;

        if (!packetData->hwpPkgCtlPolarity.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        packetData->hwpPkgCtlPolarity.setValue(cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpPkgCtlPolarityEnableArg).toBool(), true);
    }

    void CliHelperIntel::setHwpRequestPkg() const {
        const bool hasMin = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgMinPerfArg);
        const bool hasMax = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgMaxPerfArg);
        const bool hasDesired = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgDesiredPerfArg);
        const bool hasEpp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgEppArg);
        const bool hasAcw = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgAcwArg);
        const bool hasArgs = hasMin || hasMax || hasDesired || hasEpp || hasAcw;

        if (!hasArgs)
            return;

        if (!packetData->hwpRequestPkg.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::HWPRequestPkg req = packetData->hwpRequestPkg.getValue();
        PWTS::Intel::HWPCapabilities pkgCaps = {INT_MAX, 0};

        for (const PWTS::Intel::IntelThreadData &thd: packetData->threadData) {
            if (!thd.hwpCapapabilities.isValid())
                continue;

            const PWTS::Intel::HWPCapabilities caps = thd.hwpCapapabilities.getValue();

            if (caps.lowestPerf < pkgCaps.lowestPerf)
                pkgCaps.lowestPerf = caps.lowestPerf;

            if (caps.highestPerf > pkgCaps.highestPerf)
                pkgCaps.highestPerf = caps.highestPerf;
        }

        if (pkgCaps.lowestPerf == INT_MAX || pkgCaps.highestPerf == 0) {
            logger->write(QStringLiteral("failed to get HWP PKG capabilities"));
            return;
        }

        if (hasMin)
            req.min = qBound(pkgCaps.lowestPerf, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgMinPerfArg).toInt(), pkgCaps.highestPerf);

        if (hasMax)
            req.max = qBound(pkgCaps.lowestPerf, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgMaxPerfArg).toInt(), pkgCaps.highestPerf);

        if (hasDesired)
            req.desired = qBound(pkgCaps.lowestPerf, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgDesiredPerfArg).toInt(), pkgCaps.highestPerf);

        if (hasEpp && features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP))
            req.epp = qBound(0, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgEppArg).toInt(), 255);

        if (hasAcw && features.cpu.contains(PWTS::Feature::INTEL_HWP_ACT_WIND))
            req.acw = qBound(0, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgAcwArg).toInt(), 127 * 1000);

        packetData->hwpRequestPkg.setValue(req, true);
    }

    void CliHelperIntel::setHwpRequest(PWTS::Intel::IntelThreadData &data, const QString &argIdx) const {
        const bool hasMin = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMinPerfArg);
        const bool hasMax = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMaxPerfArg);
        const bool hasDesired = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestDesiredPerfArg);
        const bool hasEpp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestEppArg);
        const bool hasAcw = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestAcwArg);
        const bool hasPkgControl = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgControlArg);
        const bool hasMinValid = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMinValidArg);
        const bool hasMaxValid = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMaxValidArg);
        const bool hasDesiredValid = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestDesiredValidArg);
        const bool hasEppValid = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestEppValidArg);
        const bool hasAcwValid = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestAcwValidArg);
        const bool hasArgs = hasMin || hasMax || hasDesired || hasEpp || hasAcw || hasPkgControl || hasMinValid ||
                            hasMaxValid || hasDesiredValid || hasEppValid || hasAcwValid;

        if (!hasArgs)
            return;

        if (!data.hwpCapapabilities.isValid() || !data.hwpRequest.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const PWTS::Intel::HWPCapabilities caps = data.hwpCapapabilities.getValue();
        PWTS::Intel::HWPRequest req = data.hwpRequest.getValue();

        if (hasMin) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMinPerfArg).toHash();

            if (args.contains("all"))
                req.requestPkg.min = qBound(caps.lowestPerf, args["all"].toInt(), caps.highestPerf);
            else if (args.contains(argIdx))
                req.requestPkg.min = qBound(caps.lowestPerf, args[argIdx].toInt(), caps.highestPerf);
        }

        if (hasMax) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMaxPerfArg).toHash();

            if (args.contains("all"))
                req.requestPkg.max = qBound(caps.lowestPerf, args["all"].toInt(), caps.highestPerf);
            else if (args.contains(argIdx))
                req.requestPkg.max = qBound(caps.lowestPerf, args[argIdx].toInt(), caps.highestPerf);
        }

        if (hasDesired) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestDesiredPerfArg).toHash();

            if (args.contains("all"))
                req.requestPkg.desired = qBound(caps.lowestPerf, args["all"].toInt(), caps.highestPerf);
            else if (args.contains(argIdx))
                req.requestPkg.desired = qBound(caps.lowestPerf, args[argIdx].toInt(), caps.highestPerf);
        }

        if (hasEpp && features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP)) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestEppArg).toHash();

            if (args.contains("all"))
                req.requestPkg.epp = qBound(0, args["all"].toInt(), 255);
            else if (args.contains(argIdx))
                req.requestPkg.epp = qBound(0, args[argIdx].toInt(), 255);
        }

        if (hasAcw && features.cpu.contains(PWTS::Feature::INTEL_HWP_ACT_WIND)) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestAcwArg).toHash();

            if (args.contains("all"))
                req.requestPkg.acw = qBound(0, args["all"].toInt(), 127 * 1000);
            else if (args.contains(argIdx))
                req.requestPkg.acw = qBound(0, args[argIdx].toInt(), 127 * 1000);
        }

        if (hasPkgControl && features.cpu.contains(PWTS::Feature::INTEL_HWP_REQ_PKG)) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestPkgControlArg).toHash();

            if (args.contains("all"))
                req.packageControl = args["all"].toBool();
            else if (args.contains(argIdx))
                req.packageControl = args[argIdx].toBool();
        }

        if (features.cpu.contains(PWTS::Feature::INTEL_HWP_VALID_BITS)) {
            if (hasMinValid) {
                const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMinValidArg).toHash();

                if (args.contains("all"))
                    req.minValid = args["all"].toBool();
                else if (args.contains(argIdx))
                    req.minValid = args[argIdx].toBool();
            }

            if (hasMaxValid) {
                const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestMaxValidArg).toHash();

                if (args.contains("all"))
                    req.maxValid = args["all"].toBool();
                else if (args.contains(argIdx))
                    req.maxValid = args[argIdx].toBool();
            }

            if (hasDesiredValid) {
                const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestDesiredValidArg).toHash();

                if (args.contains("all"))
                    req.desiredValid = args["all"].toBool();
                else if (args.contains(argIdx))
                    req.desiredValid = args[argIdx].toBool();
            }

            if (hasEppValid && features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP)) {
                const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestEppValidArg).toHash();

                if (args.contains("all"))
                    req.eppValid = args["all"].toBool();
                else if (args.contains(argIdx))
                    req.eppValid = args[argIdx].toBool();
            }

            if (hasAcwValid && features.cpu.contains(PWTS::Feature::INTEL_HWP_ACT_WIND)) {
                const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, hwpRequestAcwValidArg).toHash();

                if (args.contains("all"))
                    req.acwValid = args["all"].toBool();
                else if (args.contains(argIdx))
                    req.acwValid = args[argIdx].toBool();
            }
        }

        data.hwpRequest.setValue(req, true);
    }

    void CliHelperIntel::setPkgCstConfigControl(PWTS::Intel::IntelCoreData &data, const QString &argIdx) const {
        const bool hasCStateLimit = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlCstateLimArg);
        const bool hasMaxCoreCstate = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlMaxCoreCstateArg);
        const bool hasIoMWaitRedirect = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlIOMWaitRedirectEnableArg);
        const bool hasCfgLock = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlCfgLockArg);
        const bool hasC3AutoDemotion = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC3StateAutodemotionEnableArg);
        const bool hasC1AutoDemotion = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC1StateAutodemotionEnableArg);
        const bool hasC3Undemotion = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC3UndemotionEnableArg);
        const bool hasC1Undemotion = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC1UndemotionEnableArg);
        const bool hasPkgcAutodemotion = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlPkgcAutodemotionEnableArg);
        const bool hasPkgcUndemotion = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlPkgcUndemotionEnableArg);
        const bool hasTimedMwait = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlTimedMwaitEnableArg);
        const bool hasArgs = hasCStateLimit || hasMaxCoreCstate || hasIoMWaitRedirect || hasCfgLock || hasC3AutoDemotion ||
                            hasC1AutoDemotion || hasC3Undemotion || hasC1Undemotion || hasPkgcAutodemotion || hasPkgcUndemotion ||
                            hasTimedMwait;

        if (!hasArgs)
            return;

        if (!data.pkgCstConfigControl.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        PWTS::Intel::PkgCstConfigControl pkgCstConfig = data.pkgCstConfigControl.getValue();

        if (hasCStateLimit) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlCstateLimArg).toHash();
            QList<int> validList;

            if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL_SB))
                validList.append({0,1,2,3,4,5,7});
            else if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL_CU1))
                validList.append({0,1,2,3,4,5,6,7,8});

            if (args.contains("all"))
                pkgCstConfig.packageCStateLimit = args["all"].toInt();
            else if (args.contains(argIdx))
                pkgCstConfig.packageCStateLimit = args[argIdx].toInt();

            if (!validList.contains(pkgCstConfig.packageCStateLimit)) {
                logInvalidValue(pkgCstCfgControlCstateLimArg, QString::number(pkgCstConfig.packageCStateLimit));
                return;
            }
        }

        if (hasMaxCoreCstate) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlMaxCoreCstateArg).toHash();
            QList<int> validList;

            if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL_CU1))
                validList.append({1,2,3});

            if (args.contains("all"))
                pkgCstConfig.maxCoreCState = args["all"].toInt();
            else if (args.contains(argIdx))
                pkgCstConfig.maxCoreCState = args[argIdx].toInt();

            if (!validList.contains(pkgCstConfig.maxCoreCState)) {
                logInvalidValue(pkgCstCfgControlMaxCoreCstateArg, QString::number(pkgCstConfig.maxCoreCState));
                return;
            }
        }

        if (hasIoMWaitRedirect) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlIOMWaitRedirectEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.ioMwaitRedirectionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.ioMwaitRedirectionEnable = args[argIdx].toBool();
        }

        if (hasCfgLock) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlCfgLockArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.cfgLock = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.cfgLock = args[argIdx].toBool();
        }

        if (hasC3AutoDemotion) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC3StateAutodemotionEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.c3StateAutodemotionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.c3StateAutodemotionEnable = args[argIdx].toBool();
        }

        if (hasC1AutoDemotion) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC1StateAutodemotionEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.c1StateAutodemotionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.c1StateAutodemotionEnable = args[argIdx].toBool();
        }

        if (hasC3Undemotion) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC3UndemotionEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.c3UndemotionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.c3UndemotionEnable = args[argIdx].toBool();
        }

        if (hasC1Undemotion) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlC1UndemotionEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.c1UndemotionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.c1UndemotionEnable = args[argIdx].toBool();
        }

        if (hasPkgcAutodemotion) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlPkgcAutodemotionEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.pkgcAutodemotionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.pkgcAutodemotionEnable = args[argIdx].toBool();
        }

        if (hasPkgcUndemotion) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlPkgcUndemotionEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.pkgcUndemotionEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.pkgcUndemotionEnable = args[argIdx].toBool();
        }

        if (hasTimedMwait) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgCstCfgControlTimedMwaitEnableArg).toHash();

            if (args.contains("all"))
                pkgCstConfig.timedMwaitEnable = args["all"].toBool();
            else if (args.contains(argIdx))
                pkgCstConfig.timedMwaitEnable = args[argIdx].toBool();
        }

        data.pkgCstConfigControl.setValue(pkgCstConfig, true);
    }

    void CliHelperIntel::setMCHBARPkgRaplLimit() const {
        const bool hasPl1 = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1Arg);
        const bool hasPl2 = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl2Arg);
        const bool hasPl1Enable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1EnableArg);
        const bool hasPl2Enable = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl2EnableArg);
        const bool hasPl1Clamp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1ClampArg);
        const bool hasPl1Time = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1TimeArg);
        const bool hasLock = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplLockArg);
        const bool hasArgs = hasPl1 || hasPl2 || hasPl1Enable || hasPl2Enable || hasPl1Clamp ||
                            hasPl1Time || hasLock;

        if (!hasArgs)
            return;

        if (!packetData->mchbarPkgRaplLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        PWTS::Intel::MCHBARPkgRaplLimit rapl = packetData->mchbarPkgRaplLimit.getValue();
        const PWTS::MinMax range = inputRanges->getIntelPl();

        if (hasPl1) {
            const int pl1 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1Arg).toInt();

            rapl.pl1 = qBound(range.min, pl1, range.max);
        }

        if (hasPl2) {
            const int pl2 = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl2Arg).toInt();

            rapl.pl1 = qBound(range.min, pl2, range.max);
        }

        if (hasPl1Time) {
            const int time = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1TimeArg).toInt();
            constexpr int max = 56 * 1000;

            rapl.pl1Time = qBound(0, time, max);
        }

        if (hasPl1Enable)
            rapl.pl1Enable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1EnableArg).toBool();

        if (hasPl2Enable)
            rapl.pl2Enable = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl2EnableArg).toBool();

        if (hasPl1Clamp)
            rapl.pl1Clamp = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplPl1ClampArg).toBool();

        if (hasLock)
            rapl.lock = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pkgRaplLockArg).toBool();

        packetData->mchbarPkgRaplLimit.setValue(rapl, true);
    }

    void CliHelperIntel::setCoreData(PWTS::Intel::IntelCoreData &data, const QString &argIdx) const {
        if (features.cpu.contains(PWTS::Feature::INTEL_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::INTEL_PKG_CST_CONFIG_CONTROL)) {
                setPkgCstConfigControl(data, argIdx);
            }
        }
    }

    void CliHelperIntel::setThreadData(PWTS::Intel::IntelThreadData &data, const QString &argIdx) const {
        if (features.cpu.contains(PWTS::Feature::INTEL_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::INTEL_HWP_GROUP))
                setHwpRequest(data, argIdx);
        }
    }

    void CliHelperIntel::setClientPacketData() {
        if (features.cpu.contains(PWTS::Feature::INTEL_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::INTEL_PKG_POWER_LIMIT))
                setPkgPowerLimit();

            if (features.cpu.contains(PWTS::Feature::INTEL_VR_CURRENT_CFG))
                setVrCurrentConfig();

            if (features.cpu.contains(PWTS::Feature::INTEL_PP1_CURRENT_CFG))
                setPP1();

            if (features.cpu.contains(PWTS::Feature::INTEL_TURBO_POWER_CURRENT_LIMIT_RW))
                setTurboPowerLimit();

            if (features.cpu.contains(PWTS::Feature::INTEL_CPU_POWER_BALANCE))
                setCPUPowerBalance();

            if (features.cpu.contains(PWTS::Feature::INTEL_GPU_POWER_BALANCE))
                setGPUPowerBalance();

            if (features.cpu.contains(PWTS::Feature::INTEL_ENERGY_PERF_BIAS) && !features.cpu.contains(PWTS::Feature::INTEL_HWP_EPP))
                setEnergyPerfBias();

            if (features.cpu.contains(PWTS::Feature::INTEL_TURBO_RATIO_LIMIT_RW))
                setTurboRatioLimit();

            if (features.cpu.contains(PWTS::Feature::INTEL_IA32_MISC_ENABLE_GROUP))
                setMiscProcFeatures();

            if (features.cpu.contains(PWTS::Feature::INTEL_POWER_CTL))
                setPowerCtl();

            if (features.cpu.contains(PWTS::Feature::INTEL_MISC_PWR_MGMT))
                setMiscPowerMgmt();

            if (features.cpu.contains(PWTS::Feature::INTEL_UNDERVOLT_GROUP))
                setFivr();

            if (features.cpu.contains(PWTS::Feature::INTEL_HWP_GROUP)) {
                setHwpEnable();

                if (features.cpu.contains(PWTS::Feature::INTEL_HWP_CTL))
                    setHwpPkgCtlPolarityEnable();

                if (features.cpu.contains(PWTS::Feature::INTEL_HWP_REQ_PKG))
                    setHwpRequestPkg();
            }
        }

        if (features.cpu.contains(PWTS::Feature::INTEL_MCHBAR_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::INTEL_MCHBAR_PKG_RAPL_LIMIT))
                setMCHBARPkgRaplLimit();
        }

        for (int i=0,l=packetData->coreData.size(); i<l; ++i) {
            PWTS::Intel::IntelCoreData &core = packetData->coreData[i];

            setCoreData(core, QString::number(i));
        }

        for (int i=0,l=packetData->threadData.size(); i<l; ++i) {
            PWTS::Intel::IntelThreadData &thd = packetData->threadData[i];

            setThreadData(thd, QString::number(i));

            // delete ro data for this cpu
            thd.hwpCapapabilities = {};
        }
    }
}
