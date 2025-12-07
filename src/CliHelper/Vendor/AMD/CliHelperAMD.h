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
#include "pwtShared/Include/Data/Vendor/AMD/AMDData.h"
#include "pwtClientCommon/InputRanges/InputRanges.h"

namespace PWT::CLI {
    class CliHelperAMD final: public CliHelper {
    private:
        QSharedPointer<PWTS::AMD::AMDData> packetData;
        QSharedPointer<UI::InputRanges> inputRanges;

        void setApuSlow() const;
        void setStapmLimit() const;
        void setSlowLimit() const;
        void setFastLimit() const;
        void setTctlTemp() const;
        void setApuSkinTemp() const;
        void setDgpuSkinTemp() const;
        void setVrmCurrent() const;
        void setVrmSocCurrent() const;
        void setVrmMaxCurrent() const;
        void setVrmSocMaxCurrent() const;
        void setStaticGfxClock() const;
        void setMinGfxClock() const;
        void setMaxGfxClock() const;
        void setPowerProfile() const;
        void setCOAll() const;
        void setCPPCEnable() const;
        void setCOPer(PWTS::AMD::AMDCoreData &data, const QString &argIdx) const;
        void setHwPstate(PWTS::AMD::AMDThreadData &data, const QString &argIdx, const PWTS::AMD::PStateCurrentLimit &limits) const;
        void setCorePerfBoost(PWTS::AMD::AMDThreadData &data, const QString &argIdx) const;
        void setCPPCRequest(PWTS::AMD::AMDThreadData &data, const QString &argIdx) const;
        void setCoreData(PWTS::AMD::AMDCoreData &data, const QString &argIdx) const;
        void setThreadData(PWTS::AMD::AMDThreadData &data, const QString &argIdx) const;

    public:
        CliHelperAMD(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures,
                        const QSharedPointer<PWTS::AMD::AMDData> &data, const QSharedPointer<UI::InputRanges> &ranges);

        void setClientPacketData() override;
    };
}
