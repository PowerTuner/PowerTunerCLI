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
#include "pwtShared/Include/Data/OS/Windows/WindowsData.h"

namespace PWT::CLI {
    class CliHelperWindows final: public CliHelper {
    private:
        QSharedPointer<PWTS::WIN::WindowsData> packetData;

        void duplicatePowerScheme() const;
        void copyPowerSchemeSettings() const;
        void setPowerSchemes() const;
        void setPowerSchemeFlags() const;
        void setPowerSchemesDeleteFlag() const;
        void setPowerSchemesResetFlag() const;
        void setActivePowerScheme() const;

    public:
        CliHelperWindows(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const QSharedPointer<PWTS::WIN::WindowsData> &data);

        void setClientPacketData() override;
    };
}
