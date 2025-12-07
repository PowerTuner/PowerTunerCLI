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
#include "CliHelperFan.h"
#include "../../CMDParser/SettingsArguments.h"

namespace PWT::CLI {
    CliHelperFan::CliHelperFan(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const QMap<QString, PWTS::FanData> &data): CliHelper(cmd, daemonFeatures) {
        packetData = data;
    }

    QMap<QString, PWTS::FanData> CliHelperFan::getData() const {
        return packetData;
    }

    void CliHelperFan::setFanMode() {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, fanModeArg))
            return;

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, fanModeArg).toHash();

        for (const auto &[id, mode]: args.asKeyValueRange()) {
            const int modeI = mode.toInt();

            if (!packetData.contains(id)) {
                logger->write(QString("invalid fan id: %1").arg(id));
                continue;

            } else if (modeI < 0 || modeI > 1) {
                logInvalidValue(fanModeArg, QString::number(modeI));
                continue;
            }

            packetData[id].mode.setValue(modeI, true);
        }
    }

    void CliHelperFan::setFanCurve() {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, fanTripPointArg))
            return;

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, fanTripPointArg).toHash();
        const QMap<QString, PWTS::FanData> oldMap = packetData;

        // clear current curves so that we can set parsed data
        for (PWTS::FanData &data: packetData)
            data.curve.clear();

        for (const auto &[curveData, speed]: args.asKeyValueRange()) {
            if (curveData.contains(':')) {
                const QList<QString> data = curveData.split(':');
                QString fanID;

                if (data.size() != 2) {
                    logger->write(QStringLiteral("invalid curve data size"));
                    continue;
                }

                fanID = data[0];

                if (!packetData.contains(fanID)) {
                    logger->write(QString("invalid fan id: %1").arg(data[0]));
                    continue;
                }

                packetData[fanID].curve.append(std::make_pair(data[1].toInt(), qBound(0, speed.toInt(), 100)));

            } else { // fixed speed, curvedata is the fan id
                if (!packetData.contains(curveData)) {
                    logger->write(QString("invalid fan id: %1").arg(curveData));
                    continue;
                }

                packetData[curveData].curve.append(std::make_pair(0, qBound(0, speed.toInt(), 100)));
            }
        }

        // add back untouched curves
        for (const auto &[fanID, data]: oldMap.asKeyValueRange()) {
            if (!packetData[fanID].curve.isEmpty()) // modified curve, do not restore old data
                continue;

            packetData[fanID].curve = data.curve;
        }
    }

    void CliHelperFan::setClientPacketData() {
        if (features.fans.isEmpty())
            return;

        setFanMode();
        setFanCurve();
    }
}
