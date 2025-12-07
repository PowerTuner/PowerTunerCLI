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
#include <QFile>
#include <QJsonObject>

#include "CLISettings.h"
#include "pwtShared/DaemonSettings.h"

namespace PWT::CLI {
    CLISettings::CLISettings(const QString &appDataPath) {
        if (!appDataPath.isEmpty())
            path = QString("%1/powertunercli.json").arg(appDataPath);
    }

    bool CLISettings::createDefaultSettings() {
        const QString json = QString(R"({"daemons": [{"name": "local", "adr": "127.0.0.1", "port": %2}]})").arg(PWTS::DaemonSettings::DefaultTCPPort);
        const QJsonDocument jdoc = QJsonDocument::fromJson(json.toUtf8());
        QFile settF {path};

        if (!settF.open(QFile::WriteOnly | QFile::Text)) {
            emit logMessageSent(QString("failed to create default settings file: %1").arg(settF.errorString()), MessageType::Error);
            return false;
        }

        if (settF.write(jdoc.toJson()) == -1) {
            emit logMessageSent(QString("failed to write default settings file: %1").arg(settF.errorString()), MessageType::Error);
            return false;
        }

        return true;
    }

    QJsonDocument CLISettings::load() {
        if (path.isEmpty()) {
            emit logMessageSent(QStringLiteral("cannot load settings file, path is empty"), MessageType::Error);
            return {};
        }

        QFile settF {path};
        QJsonParseError jerr;
        QJsonDocument jdoc;

        if (!settF.exists() && !createDefaultSettings())
            return {};

        if (!settF.open(QFile::ReadOnly | QFile::Text)) {
            emit logMessageSent(QString("failed to open settings file: %1").arg(settF.errorString()), MessageType::Error);
            return {};
        }

        jdoc = QJsonDocument::fromJson(settF.readAll(), &jerr);
        if (jerr.error != QJsonParseError::NoError) {
            emit logMessageSent(QString("failed to parse settings json: %1").arg(jerr.errorString()), MessageType::Error);
            return {};
        }

        return jdoc;
    }

    bool CLISettings::save(const QJsonDocument &settings) {
        QFile settF {path};

        if (!settF.open(QFile::WriteOnly | QFile::Text)) {
            emit logMessageSent(QString("failed to open settings file: %1").arg(settF.errorString()), MessageType::Error);
            return {};
        }

        return (settF.write(settings.toJson()) != -1);
    }

    bool CLISettings::resetToDefaults() {
        if (path.isEmpty()) {
            emit logMessageSent(QStringLiteral("cannot reset settings to default, path is emty"), MessageType::Error);
            return false;
        }

        return createDefaultSettings();
    }

    QJsonArray CLISettings::getDaemonList() {
        const QJsonDocument jdoc = load();
        QJsonObject jobj;

        if (jdoc.isEmpty()) {
            emit logMessageSent(QStringLiteral("cannot get daemons list from settings"), MessageType::Error);
            return {};
        }

        jobj = jdoc.object();
        if (!jobj.contains("daemons"))
            return {};

        return jobj.value("daemons").toArray();
    }

    QJsonObject CLISettings::getDaemon(const QString &name) {
        const QJsonDocument jdoc = load();
        QJsonObject jobj;

        if (jdoc.isEmpty()) {
            emit logMessageSent(QStringLiteral("cannot get daemon from settings"), MessageType::Error);
            return {};
        }

        jobj = jdoc.object();
        if (!jobj.contains("daemons"))
            return {};

        for (const auto &dm: jobj["daemons"].toArray()) {
            if (dm.toObject()["name"].toString() == name)
                return dm.toObject();
        }

        return {};
    }

    bool CLISettings::addDaemon(const QString &name, const QString &adr, const quint16 port) {
        QJsonDocument settings = load();

        if (settings.isEmpty())
            return false;

        const QJsonObject daemon = {
            std::make_pair("name", name),
            std::make_pair("adr", adr),
            std::make_pair("port", port)
        };
        QJsonObject jobj = settings.object();
        QJsonArray list = jobj["daemons"].toArray();

        list.append(daemon);

        jobj["daemons"] = list;
        settings.setObject(jobj);

        if (!save(settings)) {
            emit logMessageSent(QStringLiteral("failed to save settings"), MessageType::Error);
            return false;
        }

        return true;
    }

    bool CLISettings::removeDaemons(const QList<QString> &daemons) {
        QJsonDocument settings = load();

        if (settings.isEmpty())
            return false;

        QJsonObject jobj = settings.object();
        QJsonArray list = jobj["daemons"].toArray();

        for (const QString &nameOrAdr: daemons) {
            int idx = 0;

            for (const auto &daemon: list) {
                const QJsonObject dobj = daemon.toObject();

                if (dobj["name"] != nameOrAdr && dobj["adr"] != nameOrAdr) {
                    ++idx;
                    continue;
                }

                list.removeAt(idx);
                break;
            }
        }

        jobj["daemons"] = list;
        settings.setObject(jobj);

        if (!save(settings)) {
            emit logMessageSent(QStringLiteral("failed to save settings"), MessageType::Error);
            return false;
        }

        return true;
    }
}
