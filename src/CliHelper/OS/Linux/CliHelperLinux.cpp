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
#include "CliHelperLinux.h"
#include "../../../CMDParser/SettingsArguments.h"

namespace PWT::CLI {
    CliHelperLinux::CliHelperLinux(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const QSharedPointer<PWTS::LNX::LinuxData> &data): CliHelper(cmd, daemonFeatures) {
        packetData = data;
    }

    void CliHelperLinux::setBlockQueueSchedulers() const {
        const QHash<QString, QVariant> devMap = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, blockDevQueSchedArg).toHash();

        for (const auto &[device, scheduler]: devMap.asKeyValueRange()) {
            if (!packetData->blockDevicesQueSched.contains(device))
                continue;

            const QString sched = scheduler.toString();

            if (!packetData->blockDevicesQueSched[device].availableQueueSchedulers.contains(sched)) {
                logger->write(QString("invalid scheduler '%1' for device '%2'").arg(sched, device));
                continue;
            }

            packetData->blockDevicesQueSched[device].scheduler = sched;

            // delete ro data
            packetData->blockDevicesQueSched[device].availableQueueSchedulers.clear();
            packetData->blockDevicesQueSched[device].name.clear();
        }
    }

    void CliHelperLinux::setMiscPmDevs() const {
        const QHash<QString, QVariant> devMap = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, miscPmDevsArg).toHash();

        for (PWTS::LNX::MiscPMDevice &dev: packetData->miscPMDevices) {
            if (devMap.contains(dev.control))
                dev.controlValue = devMap[dev.control].toString();
        }
    }

    void CliHelperLinux::setSMTState() const {
        if (!packetData->smtState.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const QString smt = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, smtStateArg).toString();
        const QList<QString> validList = {"on", "off", "forceoff"};

        if (validList.contains(smt))
            packetData->smtState.setValue(smt, true);
        else
            logInvalidValue(smtStateArg, smt);
    }

    void CliHelperLinux::setCPUIdleGovernor() const {
        if (!packetData->cpuIdleAvailableGovernors.isValid() || !packetData->cpuIdleGovernor.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const QString gov = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuIdleGovernorArg).toString();
        const QList<QString> validList = packetData->cpuIdleAvailableGovernors.getValue();

        if (validList.contains(gov))
            packetData->cpuIdleGovernor.setValue(gov, true);
        else
            logInvalidValue(cpuIdleGovernorArg, gov);

        // delete ro data
        packetData->cpuIdleAvailableGovernors = {};
    }

    void CliHelperLinux::setIntelGPURpsFreq(PWTS::LNX::LinuxIntelGPUData &data, const QString &argIdx) const {
        const bool hasRpsMin = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, gpuRpsMinFreqArg);
        const bool hasRpsMax = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, gpuRpsMaxFreqArg);

        if (!hasRpsMin && !hasRpsMax)
            return;

        if (!data.rpsLimits.isValid() || !data.frequency.isValid()) {
            logger->write(QString("invalid data in packet for gpu %1").arg(argIdx));
            return;
        }

        const PWTS::LNX::Intel::GPURPSLimits limits = data.rpsLimits.getValue();
        PWTS::MinMax freq = data.frequency.getValue();

        if (hasRpsMin) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, gpuRpsMinFreqArg).toHash();

            if (argData.contains("all"))
                freq.min = argData["all"].toInt();
            else if (argData.contains(argIdx))
                freq.min = argData[argIdx].toInt();
        }

        if (hasRpsMax) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, gpuRpsMaxFreqArg).toHash();

            if (argData.contains("all"))
                freq.max = argData["all"].toInt();
            else if (argData.contains(argIdx))
                freq.max = argData[argIdx].toInt();
        }

        data.frequency.setValue({
            qBound(limits.rpn, freq.min, limits.rp0),
            qBound(limits.rpn, freq.max, limits.rp0)
        }, true);
    }

    void CliHelperLinux::setIntelGPURpsMaxBoostFreq(PWTS::LNX::LinuxIntelGPUData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, gpuRpsMaxBoostArg))
            return;

        if (!data.rpsLimits.isValid() || !data.boostFrequency.isValid()) {
            logger->write(QString("invalid data in packet for gpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, gpuRpsMaxBoostArg).toHash();
        const PWTS::LNX::Intel::GPURPSLimits limits = data.rpsLimits.getValue();
        int boost;

        if (argData.contains("all"))
            boost = argData["all"].toInt();
        else if (argData.contains(argIdx))
            boost = argData[argIdx].toInt();
        else
            return;

        data.boostFrequency.setValue(qBound(limits.rpn, boost, limits.rp0), true);
    }

    void CliHelperLinux::setAMDGPUPowerDPMState(PWTS::LNX::LinuxAMDGPUData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmStateArg))
            return;

        if (!data.powerDpmState.isValid()) {
            logger->write(QString("invalid data in packet for gpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmStateArg).toHash();
        const QList<QString> validList = {"battery", "balanced", "performance"};
        QString state;

        if (argData.contains("all"))
            state = argData["all"].toString();
        else if (argData.contains(powerDpmStateArg))
            state = argData[argIdx].toString();
        else
            return;

        if (validList.contains(state))
            data.powerDpmState.setValue(state, true);
        else
           logInvalidValue(powerDpmStateArg, state);
    }

    void CliHelperLinux::setAMDGPUPowerDPMForcePerfLevel(PWTS::LNX::LinuxAMDGPUData &data, const QString &argIdx) const {
        const bool hasDpmPerfLvlLvl = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfLvlArg);
        const bool hasDpmPerfLvlSclkMin = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfMinSclkArg);
        const bool hasDpmPerfLvlSclkMax = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfMaxSclkArg);
        const bool hasDpmPerfLvlReset = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfResetArg);
        const bool hasArgs = hasDpmPerfLvlLvl || hasDpmPerfLvlSclkMin || hasDpmPerfLvlSclkMax || hasDpmPerfLvlReset;

        if (!hasArgs)
            return;

        if (!data.odRanges.isValid() || !data.dpmForcePerfLevel.isValid()) {
            logger->write(QString("invalid data in packet for gpu %1").arg(argIdx));
            return;
        }

        const PWTS::LNX::AMD::GPUODRanges limits = data.odRanges.getValue();
        PWTS::LNX::AMD::GPUDPMForcePerfLevel dpmData = data.dpmForcePerfLevel.getValue();

        if (hasDpmPerfLvlLvl) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfLvlArg).toHash();
            const QList<QString> validList = {"auto", "low", "high", "manual", "profile_standard", "profile_min_sclk", "profile_min_mclk", "profile_peak"};
            QString lvl;

            if (argData.contains("all"))
                lvl = argData["all"].toString();
            else if (argData.contains(argIdx))
                lvl = argData[argIdx].toString();

            if (!lvl.isEmpty()) {
                if (validList.contains(lvl))
                    dpmData.level = lvl;
                else
                    logInvalidValue(powerDpmForcePerfLvlArg, lvl);
            }
        }

        if (hasDpmPerfLvlSclkMin) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfMinSclkArg).toHash();
            int min = -1;

            if (argData.contains("all"))
                min = argData["all"].toInt();
            else if (argData.contains(argIdx))
                min = argData[argIdx].toInt();

            if (min != -1)
                dpmData.sclk.min = qBound(limits.sclk.min, min, limits.sclk.max);
        }

        if (hasDpmPerfLvlSclkMax) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfMaxSclkArg).toHash();
            int max = -1;

            if (argData.contains("all"))
                max = argData["all"].toInt();
            else if (argData.contains(argIdx))
                max = argData[argIdx].toInt();

            if (max != -1)
                dpmData.sclk.max = qBound(limits.sclk.min, max, limits.sclk.max);
        }

        if (hasDpmPerfLvlReset) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerDpmForcePerfResetArg).toHash();

            if (argData.contains("all"))
                dpmData.reset = argData["all"].toBool();
            else if (argData.contains(argIdx))
                dpmData.reset =argData[argIdx].toBool();
        }

        data.dpmForcePerfLevel.setValue(dpmData, true);
    }

    void CliHelperLinux::setCPUOfflineStatus(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuStatusArg))
            return;

        if (!data.cpuOnlineStatus.isValid() || !data.cpuLogicalOffAvailable.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;

        } else if (!data.cpuLogicalOffAvailable.getValue()) {
            return;
        }

        const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuStatusArg).toHash();
        int status;

        if (argData.contains("all"))
            status = argData["all"].toBool();
        else if (argData.contains(argIdx))
            status = argData[argIdx].toBool();
        else
            return;

        data.cpuOnlineStatus.setValue(status, true);
    }

    void CliHelperLinux::setCPUFreq(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const {
        const bool hasMinFreq = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuMinFreqArg);
        const bool hasMaxFreq = cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuMaxFreqArg);
        const bool hasArgs = hasMinFreq || hasMaxFreq;

        if (!hasArgs)
            return;

        if (!data.cpuFrequencyLimits.isValid() || !data.cpuFrequency.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const PWTS::LNX::CPUFrequencyLimits limits = data.cpuFrequencyLimits.getValue();
        PWTS::MinMax freq = data.cpuFrequency.getValue();

        if (hasMinFreq) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuMinFreqArg).toHash();

            if (argData.contains("all"))
                freq.min = argData["all"].toInt();
            else if (argData.contains(argIdx))
                freq.min = argData[argIdx].toInt();
        }

        if (hasMaxFreq) {
            const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuMaxFreqArg).toHash();

            if (argData.contains("all"))
                freq.max = argData["all"].toInt();
            else if (argData.contains(argIdx))
                freq.max = argData[argIdx].toInt();
        }

        data.cpuFrequency.setValue({
            qBound(limits.limit.min, freq.min, limits.limit.max),
            qBound(limits.limit.min, freq.max, limits.limit.max),
        }, true);
    }

    void CliHelperLinux::setCPUScalingGovernor(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuScalingGovernorArg))
            return;

        if (!data.scalingAvailableGovernors.isValid() || !data.scalingGovernor.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> argData = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuScalingGovernorArg).toHash();
        const QList<QString> validList = data.scalingAvailableGovernors.getValue().availableGovernors;
        QString gov;

        if (argData.contains("all"))
            gov = argData["all"].toString();
        else if (argData.contains(argIdx))
            gov = argData[argIdx].toString();
        else
            return;

        if (validList.contains(gov))
            data.scalingGovernor.setValue(gov, true);
        else
            logInvalidValue(cpuScalingGovernorArg, gov);
    }

    void CliHelperLinux::setIntelGPUData(PWTS::LNX::LinuxIntelGPUData &data, const int idx) const {
        const QSet<PWTS::Feature> &gpuFeats = features.gpus[idx].second;
        const QString &argIdx = QString::number(idx);

        if (gpuFeats.contains(PWTS::Feature::INTEL_GPU_SYSFS_GROUP)) {
            if (gpuFeats.contains(PWTS::Feature::INTEL_GPU_RPS_FREQ_SYSFS))
                setIntelGPURpsFreq(data, argIdx);

            if (gpuFeats.contains(PWTS::Feature::INTEL_GPU_BOOST_SYSFS))
                setIntelGPURpsMaxBoostFreq(data, argIdx);
        }
    }

    void CliHelperLinux::setAMDGPUData(PWTS::LNX::LinuxAMDGPUData &data, const int idx) const {
        const QSet<PWTS::Feature> &gpuFeats = features.gpus[idx].second;
        const QString &argIdx = QString::number(idx);

        if (gpuFeats.contains(PWTS::Feature::AMD_GPU_SYSFS_GROUP)) {
            if (gpuFeats.contains(PWTS::Feature::AMD_GPU_POWER_DPM_STATE_SYSFS))
                setAMDGPUPowerDPMState(data, argIdx);

            if (gpuFeats.contains(PWTS::Feature::AMD_GPU_DPM_FORCE_PERF_LEVEL_SYSFS))
                setAMDGPUPowerDPMForcePerfLevel(data, argIdx);
        }
    }

    void CliHelperLinux::setThreadData(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const {
        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::CPU_PARK_SYSFS))
                setCPUOfflineStatus(data, argIdx);

            if (features.cpu.contains(PWTS::Feature::CPUFREQ_SYSFS)) {
                setCPUFreq(data, argIdx);
                setCPUScalingGovernor(data, argIdx);
            }
        }
    }

    void CliHelperLinux::setClientPacketData() {
        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, smtStateArg) && features.cpu.contains(PWTS::Feature::CPU_SMT_SYSFS))
                setSMTState();

            if (cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, cpuIdleGovernorArg) && features.cpu.contains(PWTS::Feature::CPUIDLE_GOV_SYSFS))
                setCPUIdleGovernor();
        }

        if (cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, blockDevQueSchedArg))
            setBlockQueueSchedulers();

        if (cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, miscPmDevsArg))
            setMiscPmDevs();

        for (const auto &[gpuIdx, gpuData]: packetData->intelGpuData.asKeyValueRange()) {
            if (features.gpus.contains(gpuIdx))
                setIntelGPUData(gpuData, gpuIdx);
            else
                logger->write(QString("Intel GPU %1: no features for this GPU").arg(gpuIdx));

            // delete ro data for this gpu
            gpuData.rpsLimits = {};
        }

        for (const auto &[gpuIdx, gpuData]: packetData->amdGpuData.asKeyValueRange()) {
            if (features.gpus.contains(gpuIdx))
                setAMDGPUData(gpuData, gpuIdx);
            else
                logger->write(QString("AMD GPU %1: no features for this GPU").arg(gpuIdx));

            // delete ro data for this gpu
            gpuData.odRanges = {};
        }

        for (int i=0,l=packetData->threadData.size(); i<l; ++i) {
            PWTS::LNX::LinuxThreadData &thd = packetData->threadData[i];

            setThreadData(thd, QString::number(i));

            // delete ro data for this cpu
            thd.coreID = {};
            thd.cpuFrequencyLimits = {};
            thd.scalingAvailableGovernors = {};
            thd.cpuLogicalOffAvailable = {};
        }
    }
}
