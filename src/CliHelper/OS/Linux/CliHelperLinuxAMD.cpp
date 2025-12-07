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
#include "CliHelperLinuxAMD.h"
#include "../../../CMDParser/SettingsArguments.h"

namespace PWT::CLI {
    CliHelperLinuxAMD::CliHelperLinuxAMD(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures,
                                            const QSharedPointer<PWTS::LNX::AMD::LinuxAMDData> &data): CliHelper(cmd, daemonFeatures) {
        packetData = data;
    }

    void CliHelperLinuxAMD::setPstateStatus() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pstateStatusArg))
            return;

        if (!packetData->pstateStatus.isValid()) {
            logger->write(QStringLiteral("invalid data in packet"));
            return;
        }

        const QString status = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pstateStatusArg).toString();
        const QList<QString> validList = {"disable", "active", "passive", "guided"};

        if (validList.contains(status))
            packetData->pstateStatus.setValue(status, true);
        else
            logInvalidValue(pstateStatusArg, status);
    }

    void CliHelperLinuxAMD::setPstateEpp(PWTS::LNX::AMD::LinuxAMDThreadData &data, const QString &argIdx) const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, pstateEppArg))
            return;

        if (!data.epp.isValid() || !data.pstateData.isValid()) {
            logger->write(QString("invalid data in packet for cpu %1").arg(argIdx));
            return;
        }

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, pstateEppArg).toHash();
        QString epp;

        if (args.contains("all"))
            epp = args["all"].toString();
        else if (args.contains(argIdx))
            epp = args[argIdx].toString();

        if (!data.pstateData.getValue().eppAvailablePrefs.contains(epp)) {
            logInvalidValue(pstateEppArg, epp);
            return;
        }

        data.epp.setValue(epp, true);
    }

    void CliHelperLinuxAMD::setThreadData(PWTS::LNX::AMD::LinuxAMDThreadData &data, const QString &argIdx) const {
        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_PSTATE_SYSFS))
                setPstateEpp(data, argIdx);
        }
    }

    void CliHelperLinuxAMD::setClientPacketData() {
        if (features.cpu.contains(PWTS::Feature::SYSFS_GROUP)) {
            if (features.cpu.contains(PWTS::Feature::AMD_PSTATE_SYSFS))
                setPstateStatus();
        }

        for (int i=0,l=packetData->threadData.size(); i<l; ++i) {
            PWTS::LNX::AMD::LinuxAMDThreadData &thd = packetData->threadData[i];

            setThreadData(thd, QString::number(i));

            // delete ro data for this cpu
            thd.pstateData = {};
        }
    }
}
