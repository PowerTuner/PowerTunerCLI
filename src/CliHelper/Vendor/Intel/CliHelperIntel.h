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
#include "pwtShared/Include/Data/Vendor/Intel/IntelData.h"
#include "pwtClientCommon/InputRanges/InputRanges.h"

namespace PWT::CLI {
    class CliHelperIntel final: public CliHelper {
    private:
        QSharedPointer<PWTS::Intel::IntelData> packetData;
        QSharedPointer<UI::InputRanges> inputRanges;
        int cpuCores;

        void setPkgPowerLimit() const;
        void setVrCurrentConfig() const;
        void setPP1() const;
        void setTurboPowerLimit() const;
        void setCPUPowerBalance() const;
        void setGPUPowerBalance() const;
        void setEnergyPerfBias() const;
        void setTurboRatioLimit() const;
        void setMiscProcFeatures() const;
        void setPowerCtl() const;
        void setMiscPowerMgmt() const;
        void setFivr() const;
        void setHwpEnable() const;
        void setHwpPkgCtlPolarityEnable() const;
        void setHwpRequestPkg() const;
        void setHwpRequest(PWTS::Intel::IntelThreadData &data, const QString &argIdx) const;
        void setPkgCstConfigControl(PWTS::Intel::IntelCoreData &data, const QString &argIdx) const;
        void setMCHBARPkgRaplLimit() const;
        void setCoreData(PWTS::Intel::IntelCoreData &data, const QString &argIdx) const;
        void setThreadData(PWTS::Intel::IntelThreadData &data, const QString &argIdx) const;

    public:
        CliHelperIntel(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, int coreCount,
                        const QSharedPointer<PWTS::Intel::IntelData> &data, const QSharedPointer<UI::InputRanges> &ranges);

        void setClientPacketData() override;
    };
}
