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

#include <QJsonArray>

#include "../Include/MessageType.h"

namespace PWT::CLI {
    class CLISettings final: public QObject {
        Q_OBJECT

    private:
        QString path;

        [[nodiscard]] bool createDefaultSettings();
        [[nodiscard]] QJsonDocument load();
        [[nodiscard]] bool save(const QJsonDocument &settings);

    public:
        explicit CLISettings(const QString &appDataPath);

        [[nodiscard]] bool resetToDefaults();
        [[nodiscard]] QJsonArray getDaemonList();
        [[nodiscard]] QJsonObject getDaemon(const QString &name);
        [[nodiscard]] bool addDaemon(const QString &name, const QString &adr, quint16 port);
        [[nodiscard]] bool removeDaemons(const QList<QString> &daemons);

    signals:
        void logMessageSent(const QString &msg, MessageType type);
    };
}
