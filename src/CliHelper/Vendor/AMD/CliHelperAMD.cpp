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
#include "CliHelperAMD.h"
#include "../../../CMDParser/SettingsArguments.h"

namespace PWT::CLI {
    CliHelperAMD::CliHelperAMD(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures,
                                const QSharedPointer<PWTS::AMD::AMDData> &data, const QSharedPointer<UI::InputRanges> &ranges): CliHelper(cmd, daemonFeatures) {
        packetData = data;
        inputRanges = ranges;
    }

    void CliHelperAMD::setApuSlow() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, apuSlowArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_APU_SLOW_R))
                packetData->apuSlow.setIgnored(true);

            return;
        }

        if (!packetData->apuSlow.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJAPUSlow();

        packetData->apuSlow.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, apuSlowArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setStapmLimit() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, stapmLimitArg))
            return;

        if (!packetData->stapmLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJPl();

        packetData->stapmLimit.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, stapmLimitArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setSlowLimit() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, slowLimitArg))
            return;

        if (!packetData->slowLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJPl();

        packetData->slowLimit.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, apuSlowArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setFastLimit() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, fastLimitArg))
            return;

        if (!packetData->fastLimit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJPl();

        packetData->fastLimit.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, fastLimitArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setTctlTemp() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, tctlTempArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_TCTL_TEMP_R))
                packetData->tctlTemp.setIgnored(true);

            return;
        }

        if (!packetData->tctlTemp.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJTctl();

        packetData->tctlTemp.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, tctlTempArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setApuSkinTemp() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, apuSkinTempArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_APU_SKIN_TEMP_R))
                packetData->apuSkinTemp.setIgnored(true);

            return;
        }

        if (!packetData->apuSkinTemp.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJAPUSkinTemp();

        packetData->apuSkinTemp.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, apuSkinTempArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setDgpuSkinTemp() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, dgpuSkinTempArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_DGPU_SKIN_TEMP_R))
                packetData->dgpuSkinTemp.setIgnored(true);

            return;
        }

        if (!packetData->dgpuSkinTemp.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJDGPUSkinTemp();

        packetData->dgpuSkinTemp.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, dgpuSkinTempArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setVrmCurrent() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmCurrentArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_VRM_CURRENT_R))
                packetData->vrmCurrent.setIgnored(true);

            return;
        }

        if (!packetData->vrmCurrent.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJVrmCurrent();

        packetData->vrmCurrent.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmCurrentArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setVrmSocCurrent() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmSocCurrentArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_VRM_SOC_CURRENT_R))
                packetData->vrmSocCurrent.setIgnored(true);

            return;
        }

        if (!packetData->vrmSocCurrent.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJVrmSocCurrent();

        packetData->vrmSocCurrent.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmSocCurrentArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setVrmMaxCurrent() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmMaxCurrentArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_VRM_MAX_CURRENT_R))
                packetData->vrmMaxCurrent.setIgnored(true);

            return;
        }

        if (!packetData->vrmMaxCurrent.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJVrmCurrent();

        packetData->vrmMaxCurrent.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmMaxCurrentArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setVrmSocMaxCurrent() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmSocMaxCurrentArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_VRM_SOC_MAX_CURRENT_R))
                packetData->vrmSocMaxCurrent.setIgnored(true);

            return;
        }

        if (!packetData->vrmSocMaxCurrent.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJVrmSocCurrent();

        packetData->vrmSocMaxCurrent.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, vrmSocMaxCurrentArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setStaticGfxClock() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, staticGfxClkArg)) {
            packetData->staticGfxClock.setIgnored(true);
            return;
        }

        if (!packetData->staticGfxClock.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJGfxClock();

        packetData->staticGfxClock.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, staticGfxClkArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setMinGfxClock() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, minGfxClkArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_MIN_GFX_CLOCK_R))
                packetData->minGfxClock.setIgnored(true);

            return;
        }

        if (!packetData->minGfxClock.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJGfxClock();

        packetData->minGfxClock.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, minGfxClkArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setMaxGfxClock() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, maxGfxClkArg)) {
            if (!features.cpu.contains(PWTS::Feature::AMD_RY_MAX_GFX_CLOCK_R))
                packetData->maxGfxClock.setIgnored(true);

            return;
        }

        if (!packetData->maxGfxClock.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJGfxClock();

        packetData->maxGfxClock.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, maxGfxClkArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setPowerProfile() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, amdPowerProfileArg)) {
            packetData->powerProfile.setIgnored(true);
            return;
        }

        if (!packetData->powerProfile.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const int profile = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, amdPowerProfileArg).toInt();

        if (profile != 0 && profile != 1) {
            logInvalidValue(amdPowerProfileArg, QString::number(profile));
            return;
        }

        packetData->powerProfile.setValue(profile, true);
    }

    void CliHelperAMD::setCOAll() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, curveOptimizerArg)) {
            packetData->curveOptimizer.setIgnored(true);
            return;
        }

        if (!packetData->curveOptimizer.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const PWTS::MinMax range = inputRanges->getRADJCO();

        packetData->curveOptimizer.setValue(qBound(range.min, cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, curveOptimizerArg).toInt(), range.max), true);
    }

    void CliHelperAMD::setCPPCEnable() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcEnableArg)) {
            packetData->cppcEnableBit.setIgnored(true);
            return;
        }

        if (!packetData->cppcEnableBit.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        packetData->cppcEnableBit.setValue(cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcEnableArg).toBool(), true);
    }

    void CliHelperAMD::setCOPer(PWTS::AMD::AMDCoreData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, curveOptimizerPerArg)) {
            data.curveOptimizer.setIgnored(true);
            return;
        }

        if (!data.curveOptimizer.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, curveOptimizerPerArg).toHash();
        const PWTS::MinMax range = inputRanges->getRADJCO();

        if (args.contains("all"))
            data.curveOptimizer.setValue(qBound(range.min, args["all"].toInt(), range.max), true);
        else if (args.contains(argIdx))
            data.curveOptimizer.setValue(qBound(range.min, args[argIdx].toInt(), range.max), true);
    }

    void CliHelperAMD::setHwPstate(PWTS::AMD::AMDThreadData &data, const QString &argIdx, const PWTS::AMD::PStateCurrentLimit &limits) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pstateCmdArg))
            return;

        if (!data.pstateCmd.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pstateCmdArg).toHash();

        if (args.contains("all"))
            data.pstateCmd.setValue(qBound(limits.curPStateLimit, args["all"].toInt(), limits.pstateMaxValue), true);
        else if (args.contains(argIdx))
            data.pstateCmd.setValue(qBound(limits.curPStateLimit, args[argIdx].toInt(), limits.pstateMaxValue), true);
    }

    void CliHelperAMD::setCorePerfBoost(PWTS::AMD::AMDThreadData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, corePerfBoostArg))
            return;

        if (!data.corePerfBoost.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, corePerfBoostArg).toHash();

        if (args.contains("all"))
            data.corePerfBoost.setValue(args["all"].toInt(), true);
        else if (args.contains(argIdx))
            data.corePerfBoost.setValue(args[argIdx].toInt(), true);
    }

    void CliHelperAMD::setCPPCRequest(PWTS::AMD::AMDThreadData &data, const QString &argIdx) const {
        const bool hasMin = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestMinPerfArg);
        const bool hasMax = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestMaxPerfArg);
        const bool hasDesired = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestDesiredPerfArg);
        const bool hasEpp = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestEppArg);
        const bool hasArgs = hasMin || hasMax || hasDesired || hasEpp;

        if (!hasArgs) {
            data.cppcRequest.setIgnored(true);
            return;
        }

        if (!data.cppcRequest.isValid() || !data.cppcCapability1.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const PWTS::AMD::CPPCCapability1 caps = data.cppcCapability1.getValue();
        PWTS::AMD::CPPCRequest req = data.cppcRequest.getValue();

        if (hasMin) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestMinPerfArg).toHash();

            if (args.contains("all"))
                req.minPerf = qBound(caps.lowestPerf, args["all"].toInt(), caps.highestPerf);
            else if (args.contains(argIdx))
                req.minPerf = qBound(caps.lowestPerf, args[argIdx].toInt(), caps.highestPerf);
        }

        if (hasMax) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestMaxPerfArg).toHash();

            if (args.contains("all"))
                req.maxPerf = qBound(caps.lowestPerf, args["all"].toInt(), caps.highestPerf);
            else if (args.contains(argIdx))
                req.maxPerf = qBound(caps.lowestPerf, args[argIdx].toInt(), caps.highestPerf);
        }

        if (hasDesired) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestDesiredPerfArg).toHash();

            if (args.contains("all"))
                req.desPerf = qBound(caps.lowestPerf, args["all"].toInt(), caps.highestPerf);
            else if (args.contains(argIdx))
                req.desPerf = qBound(caps.lowestPerf, args[argIdx].toInt(), caps.highestPerf);
        }

        if (hasEpp) {
            const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cppcRequestEppArg).toHash();

            if (args.contains("all"))
                req.desPerf = qBound(0, args["all"].toInt(), 255);
            else if (args.contains(argIdx))
                req.desPerf = qBound(0, args[argIdx].toInt(), 255);
        }

        data.cppcRequest.setValue(req, true);
    }

    void CliHelperAMD::setCoreData(PWTS::AMD::AMDCoreData &data, const QString &argIdx) const {
        if (features.cpu.contains(PWTS::Feature::AMD_CPU_RY_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_RY_CO_PER_W))
                setCOPer(data, argIdx);
        }
    }

    void CliHelperAMD::setThreadData(PWTS::AMD::AMDThreadData &data, const QString &argIdx) const {
        if (features.cpu.contains(PWTS::Feature::AMD_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_HWPSTATE)) {
                if (packetData->pstateCurrentLimit.isValid())
                    setHwPstate(data, argIdx, packetData->pstateCurrentLimit.getValue());
                else
                    logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            }

            if (features.cpu.contains(PWTS::Feature::AMD_CORE_PERFORMANCE_BOOST))
                setCorePerfBoost(data, argIdx);

            if (features.cpu.contains(PWTS::Feature::AMD_CPPC))
                setCPPCRequest(data, argIdx);
        }
    }

    void CliHelperAMD::setClientPacketData() {
        if (features.cpu.contains(PWTS::Feature::AMD_CPU_RY_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_RY_APU_SLOW_W))
                setApuSlow();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_STAPM_LIMIT_W))
                setStapmLimit();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_SLOW_LIMIT_W))
                setSlowLimit();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_FAST_LIMIT_W))
                setFastLimit();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_TCTL_TEMP_W))
                setTctlTemp();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_APU_SKIN_TEMP_W))
                setApuSkinTemp();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_DGPU_SKIN_TEMP_W))
                setDgpuSkinTemp();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_CURRENT_W))
                setVrmCurrent();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_SOC_CURRENT_W))
                setVrmSocCurrent();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_MAX_CURRENT_W))
                setVrmMaxCurrent();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_VRM_SOC_MAX_CURRENT_W))
                setVrmSocMaxCurrent();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_STATIC_GFX_CLK_W))
                setStaticGfxClock();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_MIN_GFX_CLOCK_W))
                setMinGfxClock();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_MAX_GFX_CLOCK_W))
                setMaxGfxClock();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_POWER_PROFILE_W))
                setPowerProfile();

            if (features.cpu.contains(PWTS::Feature::AMD_RY_CO_ALL_W))
                setCOAll();
        }

        if (features.cpu.contains(PWTS::Feature::AMD_CPU_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_CPPC))
                setCPPCEnable();
        }

        for (int i=0,l=packetData->coreData.size(); i<l; ++i) {
            PWTS::AMD::AMDCoreData &cdata = packetData->coreData[i];

            setCoreData(cdata, QString::number(i));
        }

        for (int i=0,l=packetData->threadData.size(); i<l; ++i) {
            PWTS::AMD::AMDThreadData &thd = packetData->threadData[i];

            setThreadData(thd, QString::number(i));

            // delete ro data for this cpu
            thd.cppcCapability1 = {};
        }

        // delete ro package data
        packetData->pstateCurrentLimit = {};
    }
}
