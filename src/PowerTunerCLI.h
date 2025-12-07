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

#include "CMDParser/CMDParser.h"
#include "Classes/CLISettings.h"
#include "Classes/FileLogger.h"
#include "pwtClientCommon/InputRanges/InputRanges.h"
#include "pwtClientService/ClientService.h"
#include "pwtShared/Include/Packets/ClientPacket.h"
#include "pwtShared/Include/Packets/DaemonPacket.h"

namespace PWT::CLI {
    class PowerTunerCLI final: public QObject {
        Q_OBJECT

    private:
        QSharedPointer<CMDParser> cmdParser;
        QScopedPointer<CLISettings> cliSettings;
        QSharedPointer<FileLogger> logger;
        QSharedPointer<UI::InputRanges> inputRanges;
        QScopedPointer<PWTCS::ClientService> service;
        mutable PWTS::ClientPacket clientPacket;
        PWTS::Features features;
        int coreCount;
        QString globalDataPath;
        QString dataPath;

        void setInputRanges(const PWTS::DeviceInfoPacket &packet);
        void runGetCommand();
        void runSetCommand();
        void initService();
        void importProfiles() const;
        PWTS::ClientPacket createClientPacket(const PWTS::DaemonPacket &packet) const;
        void applyDeviceSettings(const PWTS::DaemonPacket &packet) const;

    public:
        PowerTunerCLI();

        void run(int argc, char *argv[]);

    private slots:
        void onLogMessageSent(const QString &msg, MessageType type) const;
        void onServiceLogSent(const QString &msg) const;
        void onServiceError();
        void onServiceCommandFailed();
        void onServiceDisconnected();
        void onServiceConnected() const;
        void onServiceDeviceInfoPacketReceived(const PWTS::DeviceInfoPacket &packet);
        void onServiceDaemonPacketReceived(const PWTS::DaemonPacket &packet);
        void onServiceDaemonSettingsReceived(const QByteArray &data);
        void onServiceDaemonSettingsApplied(bool diskSaveResult);
        void onServiceSettingsApplied(const QSet<PWTS::DError> &errors);
        void onServiceProfileListReceived(const QList<QString> &list);
        void onServiceProfileDeleted(bool result);
        void onServiceProfileApplied(const QSet<PWTS::DError> &errors, const QString &name);
        void onServiceProfilesExported(const QHash<QString, QByteArray> &exported);
        void onServiceProfilesImported(bool result);
        void onServiceProfileWritten(bool result);

    signals:
        void quit(int code);
    };
}
