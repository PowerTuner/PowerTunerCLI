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
#include "pwtShared/Include/Data/OS/Linux/LinuxAMDData.h"

namespace PWT::CLI {
    class CliHelperLinuxAMD final: public CliHelper {
    private:
        QSharedPointer<PWTS::LNX::AMD::LinuxAMDData> packetData;

        void setPstateStatus() const;
        void setPstateEpp(PWTS::LNX::AMD::LinuxAMDThreadData &data, const QString &argIdx) const;
        void setThreadData(PWTS::LNX::AMD::LinuxAMDThreadData &data, const QString &argIdx) const;

    public:
        CliHelperLinuxAMD(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const QSharedPointer<PWTS::LNX::AMD::LinuxAMDData> &data);

        void setClientPacketData() override;
    };
}
