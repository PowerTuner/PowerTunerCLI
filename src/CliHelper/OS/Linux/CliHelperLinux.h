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

#include "../../CliHelper.h"
#include "pwtShared/Include/Data/OS/Linux/LinuxData.h"

namespace PWT::CLI {
    class CliHelperLinux final: public CliHelper {
    private:
        QSharedPointer<PWTS::LNX::LinuxData> packetData;

        void setBlockQueueSchedulers() const;
        void setMiscPmDevs() const;
        void setSMTState() const;
        void setCPUIdleGovernor() const;
        void setIntelGPURpsFreq(PWTS::LNX::LinuxIntelGPUData &data, const QString &argIdx) const;
        void setIntelGPURpsMaxBoostFreq(PWTS::LNX::LinuxIntelGPUData &data, const QString &argIdx) const;
        void setAMDGPUPowerDPMState(PWTS::LNX::LinuxAMDGPUData &data, const QString &argIdx) const;
        void setAMDGPUPowerDPMForcePerfLevel(PWTS::LNX::LinuxAMDGPUData &data, const QString &argIdx) const;
        void setCPUOfflineStatus(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const;
        void setCPUFreq(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const;
        void setCPUScalingGovernor(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const;
        void setIntelGPUData(PWTS::LNX::LinuxIntelGPUData &data, int idx) const;
        void setAMDGPUData(PWTS::LNX::LinuxAMDGPUData &data, int idx) const;
        void setThreadData(PWTS::LNX::LinuxThreadData &data, const QString &argIdx) const;

    public:
        CliHelperLinux(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const QSharedPointer<PWTS::LNX::LinuxData> &data);

        void setClientPacketData() override;
    };
}
