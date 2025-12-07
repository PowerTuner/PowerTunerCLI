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

#include "pwtShared/Include/Packets/DeviceInfoPacket.h"
#include "pwtShared/Include/Packets/DaemonPacket.h"
#include "pwtShared/Include/DaemonError.h"
#include "pwtShared/DaemonSettings.h"
#include "pwtClientCommon/InputRanges/InputRanges.h"
#include "../Classes/FileLogger.h"
#include "../Classes/CLISettings.h"

namespace PWT::CLI {
    [[nodiscard]] bool addDaemons(const QList<QString> &data, const QScopedPointer<CLISettings> &cliSettings, const QSharedPointer<FileLogger> &logger);
    void printDataPath(const QString &path);
    void printDaemons(const QJsonArray &daemons);
    void printDeviceInfo(const PWTS::DeviceInfoPacket &packet, const QSharedPointer<FileLogger> &logger, const QSharedPointer<UI::InputRanges> &inputRanges);
    void printDeviceData(const PWTS::DaemonPacket &packet, const PWTS::Features &features, int coreCount);
    void printDaemonSettings(const QSharedPointer<PWTS::DaemonSettings> &daemonSettings);
    void printProfileList(const QList<QString> &list);
    void printApplyResults(const QSet<PWTS::DError> &errors, const QString &profile = "");
}
