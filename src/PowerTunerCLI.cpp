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
#include <QStandardPaths>
#include <QDir>

#include "PowerTunerCLI.h"
#include "Utils.h"
#include "Commands/AppCommands.h"
#include "pwtShared/Utils.h"
#include "CliHelper/OS/Linux/CliHelperLinux.h"
#include "CliHelper/OS/Windows/CliHelperWindows.h"
#include "CliHelper/Misc/CliHelperFan.h"
#ifdef WITH_INTEL
#include "CliHelper/Vendor/Intel/CliHelperIntel.h"
#endif
#ifdef WITH_AMD
#include "CliHelper/Vendor/AMD/CliHelperAMD.h"
#include "CliHelper/OS/Linux/CliHelperLinuxAMD.h"
#endif

namespace PWT::CLI {
    PowerTunerCLI::PowerTunerCLI() {
        const QDir qdir;

        logger = FileLogger::getInstance();
        globalDataPath = QString("%1/PowerTuner").arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
        dataPath = QString("%1/cli").arg(globalDataPath);
        cmdParser = QSharedPointer<CMDParser>::create();

        if (!qdir.exists(dataPath) && !qdir.mkpath(dataPath))
            dataPath.clear();

        cliSettings.reset(new CLISettings(dataPath));
        logger->init(dataPath);

        QObject::connect(cliSettings.get(), &CLISettings::logMessageSent, this, &PowerTunerCLI::onLogMessageSent);
    }

    void PowerTunerCLI::setInputRanges(const PWTS::DeviceInfoPacket &packet) {
        inputRanges = UI::InputRanges::getInstance();

        inputRanges->setAppDataPath(globalDataPath);
        inputRanges->load(packet.sysInfo.product, packet.cpuInfo.brand);
    }

    void PowerTunerCLI::run(const int argc, char *argv[]) {
        if (!cmdParser->parse(argc, argv)) {
            emit quit(1);
            return;
        }

        if (cmdParser->isSet(CMDArg::GET_MODE))
            runGetCommand();
        else if (cmdParser->isSet(CMDArg::SET_MODE))
            runSetCommand();
        else
            emit quit(1);
    }

    void PowerTunerCLI::runGetCommand() {
        if (cmdParser->isSet(CMDArg::GET_DATA_PATH)) {
            printDataPath(dataPath);
            emit quit(0);

        } else if (cmdParser->isSet(CMDArg::GET_DAEMON_LIST)) {
            printDaemons(cliSettings->getDaemonList());
            emit quit(0);

        } else if (cmdParser->isSet(CMDArg::DAEMON)) {
            initService(); // this cmd requires daemon connection, lets handle it after connection
        }
    }

    void PowerTunerCLI::runSetCommand() {
        if (cmdParser->isSet(CMDArg::SET_RESET_CLI_SETTINGS)) {
            emit quit(!cliSettings->resetToDefaults());

        } else if (cmdParser->isSet(CMDArg::SET_ADD_DAEMONS)) {
            const bool ret = addDaemons(cmdParser->getCmdValue(CMDArg::SET_ADD_DAEMONS, "daemonsData").toStringList(), cliSettings, logger);

            emit quit(!ret);

        } else if (cmdParser->isSet(CMDArg::SET_REMOVE_DAEMONS)) {
            const bool ret = cliSettings->removeDaemons(cmdParser->getCmdValue(CMDArg::SET_REMOVE_DAEMONS, "daemons").toStringList());

            if (!ret)
                logger->write(QStringLiteral("failed to remove some daemons"));

            emit quit(!ret);

        } else if (cmdParser->isSet(CMDArg::DAEMON)) {
            initService(); // this cmd requires daemon connection, lets handle it after connection
        }
    }

    void PowerTunerCLI::initService() {
        const QString dname = cmdParser->getCmdValue(CMDArg::DAEMON, "name").toString();
        QString adr;
        quint16 port;

        if (dname.isEmpty()) {
            bool res;

            adr = cmdParser->getCmdValue(CMDArg::DAEMON, "adr").toString();
            port = cmdParser->getCmdValue(CMDArg::DAEMON, "port").toUInt(&res);

            if (adr.isEmpty() || !res) {
                logger->write(QStringLiteral("daemon address/port is not valid"));
                emit quit(1);
                return;
            }
        } else {
            const QJsonObject daemon = cliSettings->getDaemon(dname);

            if (daemon.isEmpty()) {
                logger->write(QString("no daemon found with the given name: %1").arg(dname));
                emit quit(1);
                return;
            }

            adr = daemon["adr"].toString();
            port = daemon["port"].toInt();
        }

        service.reset(new PWTCS::ClientService);

        QObject::connect(service.get(), &PWTCS::ClientService::logMessageSent, this, &PowerTunerCLI::onServiceLogSent);
        QObject::connect(service.get(), &PWTCS::ClientService::serviceConnected, this, &PowerTunerCLI::onServiceConnected);
        QObject::connect(service.get(), &PWTCS::ClientService::serviceError, this, &PowerTunerCLI::onServiceError);
        QObject::connect(service.get(), &PWTCS::ClientService::commandFailed, this, &PowerTunerCLI::onServiceCommandFailed);
        QObject::connect(service.get(), &PWTCS::ClientService::serviceDisconnected, this, &PowerTunerCLI::onServiceDisconnected);
        QObject::connect(service.get(), &PWTCS::ClientService::deviceInfoPacketReceived, this, &PowerTunerCLI::onServiceDeviceInfoPacketReceived);
        QObject::connect(service.get(), &PWTCS::ClientService::daemonPacketReceived, this, &PowerTunerCLI::onServiceDaemonPacketReceived);
        QObject::connect(service.get(), &PWTCS::ClientService::daemonSettingsReceived, this, &PowerTunerCLI::onServiceDaemonSettingsReceived);
        QObject::connect(service.get(), &PWTCS::ClientService::daemonSettingsApplied, this, &PowerTunerCLI::onServiceDaemonSettingsApplied);
        QObject::connect(service.get(), &PWTCS::ClientService::settingsApplied, this, &PowerTunerCLI::onServiceSettingsApplied);
        QObject::connect(service.get(), &PWTCS::ClientService::profileListReceived, this, &PowerTunerCLI::onServiceProfileListReceived);
        QObject::connect(service.get(), &PWTCS::ClientService::profileDeleted, this, &PowerTunerCLI::onServiceProfileDeleted);
        QObject::connect(service.get(), &PWTCS::ClientService::profileApplied, this, &PowerTunerCLI::onServiceProfileApplied);
        QObject::connect(service.get(), &PWTCS::ClientService::profileWritten, this, &PowerTunerCLI::onServiceProfileWritten);
        QObject::connect(service.get(), &PWTCS::ClientService::profilesExported, this, &PowerTunerCLI::onServiceProfilesExported);
        QObject::connect(service.get(), &PWTCS::ClientService::profilesImported, this, &PowerTunerCLI::onServiceProfilesImported);

        service->connectToDaemon(adr, port);
    }

    void PowerTunerCLI::importProfiles() const {
        const QList<QString> list = cmdParser->getCmdValue(CMDArg::SET_IMPORT_PROFILES, "profiles").toStringList();
        QHash<QString, QByteArray> imports;

        for (const QString &file: list) {
            if (!PWTS::isValidProfileForImport(file))
                continue;

            const QFileInfo finfo {file};
            QFile f {file};

            if (f.open(QFile::ReadOnly))
                imports.insert(finfo.baseName(), f.readAll());
            else
                logger->write(QString("cannot import profile '%1', skip").arg(file));
        }

        service->sendImportProfilesRequest(imports);
    }

    PWTS::ClientPacket PowerTunerCLI::createClientPacket(const PWTS::DaemonPacket &packet) const {
        const std::unique_ptr<CliHelperFan> fanHelper = std::make_unique<CliHelperFan>(cmdParser, features, packet.fanData);
        PWTS::ClientPacket cpacket {};

        fanHelper->setClientPacketData();

        // no gui to read values from
        // to have a complete packet, copy daemon packet and update its values
        // ro data is unused in client packet and will be deleted
        // this also allows to create valid profiles
        cpacket.os = packet.os;
        cpacket.vendor = packet.vendor;
        cpacket.fanData = fanHelper->getData();

        switch (packet.os) {
            case PWTS::OSType::Linux: {
                const std::unique_ptr<CliHelperLinux> helper = std::make_unique<CliHelperLinux>(cmdParser, features, packet.linuxData);

                cpacket.linuxData = packet.linuxData;

                helper->setClientPacketData();
            }
                break;
            case PWTS::OSType::Windows: {
                const std::unique_ptr<CliHelperWindows> helper = std::make_unique<CliHelperWindows>(cmdParser, features, packet.windowsData);

                cpacket.windowsData = packet.windowsData;

                helper->setClientPacketData();
            }
                break;
            default:
                break;
        }

        switch (packet.vendor) {
#ifdef WITH_INTEL
            case PWTS::CPUVendor::Intel: {
                const std::unique_ptr<CliHelperIntel> helper = std::make_unique<CliHelperIntel>(cmdParser, features, coreCount, packet.intelData, inputRanges);

                cpacket.intelData = packet.intelData;

                helper->setClientPacketData();
            }
                break;
#endif
#ifdef WITH_AMD
            case PWTS::CPUVendor::AMD: {
                const std::unique_ptr<CliHelperAMD> helper = std::make_unique<CliHelperAMD>(cmdParser, features, packet.amdData, inputRanges);

                cpacket.amdData = packet.amdData;

                helper->setClientPacketData();

                switch (packet.os) {
                    case PWTS::OSType::Linux: {
                        const std::unique_ptr<CliHelperLinuxAMD> helperLA = std::make_unique<CliHelperLinuxAMD>(cmdParser, features, packet.linuxAmdData);

                        cpacket.linuxAmdData = packet.linuxAmdData;

                        helperLA->setClientPacketData();
                    }
                        break;
                    default:
                        break;
                }
            }
                break;
#endif
            default:
                break;
        }

        return cpacket;
    }

    void PowerTunerCLI::applyDeviceSettings(const PWTS::DaemonPacket &packet) const {
        clientPacket = createClientPacket(packet);

        service->sendApplySettingsRequest(clientPacket);
    }

    void PowerTunerCLI::onLogMessageSent(const QString &msg, const MessageType type) const {
        switch (type) {
            case MessageType::Info:
                print(msg);
                break;
            case MessageType::Error:
                printError(msg);
                break;
            default:
                break;
        }
    }

    void PowerTunerCLI::onServiceLogSent(const QString &msg) const {
        logger->write(msg);
    }

    void PowerTunerCLI::onServiceError() {
        logger->write(QStringLiteral("service error"));
        emit quit(1);
    }

    void PowerTunerCLI::onServiceCommandFailed() {
        logger->write(QStringLiteral("command failed"));
        emit quit(1);
    }

    void PowerTunerCLI::onServiceDisconnected() {
        logger->write(QStringLiteral("service disconnected"));
        emit quit(0);
    }

    void PowerTunerCLI::onServiceConnected() const {
        logger->write(QString("connected to %1:%2").arg(service->getDaemonAddress()).arg(service->getDaemonPort()));

        // now process set/get cmds for the daemon
        if (cmdParser->isSet(CMDArg::GET_MODE)) {
            if (cmdParser->isSet(CMDArg::GET_DEVICE_INFO) || cmdParser->isSet(CMDArg::GET_DEVICE_DATA))
                service->sendGetDeviceInfoPacketRequest();
            else if (cmdParser->isSet(CMDArg::GET_DAEMON_SETTINGS))
                service->sendGetDaemonSettingsRequest();
            else if (cmdParser->isSet(CMDArg::GET_PROFILE_LIST))
                service->sendGetProfileListRequest();
            else if (cmdParser->isSet(CMDArg::GET_EXPORT_PROFILES))
                service->sendExportProfilesRequest(cmdParser->getCmdValue(CMDArg::GET_EXPORT_PROFILES, "profile").toString());

        } else {
            if (cmdParser->isSet(CMDArg::SET_DAEMON_SETTINGS))
                service->sendGetDaemonSettingsRequest();
            else if (cmdParser->isSet(CMDArg::SET_DELETE_PROFILE))
                service->sendDeleteProfileRequest(cmdParser->getCmdValue(CMDArg::SET_DELETE_PROFILE, "profile").toString());
            else if (cmdParser->isSet(CMDArg::SET_APPLY_PROFILE))
                service->sendApplyProfileRequest(cmdParser->getCmdValue(CMDArg::SET_APPLY_PROFILE, "profile").toString());
            else if (cmdParser->isSet(CMDArg::SET_IMPORT_PROFILES))
                importProfiles();
            else if (cmdParser->isSet(CMDArg::SET_DEVICE_SETTINGS))
                service->sendGetDeviceInfoPacketRequest();
        }
    }

    void PowerTunerCLI::onServiceDeviceInfoPacketReceived(const PWTS::DeviceInfoPacket &packet) {
        const bool isSetDeviceSettings = cmdParser->isSet(CMDArg::SET_DEVICE_SETTINGS);

        if (cmdParser->isSet(CMDArg::GET_DEVICE_INFO)) {
            setInputRanges(packet);
            printDeviceInfo(packet, logger, inputRanges);
            emit quit(0);

        } else if (cmdParser->isSet(CMDArg::GET_DEVICE_DATA) || isSetDeviceSettings) {
            if (isSetDeviceSettings)
                setInputRanges(packet);

            features = packet.features;
            coreCount = packet.cpuInfo.numCores;
            service->sendGetDaemonPacketRequest();
        }
    }

    void PowerTunerCLI::onServiceDaemonPacketReceived(const PWTS::DaemonPacket &packet) {
        for (const PWTS::DError &err: packet.errors)
            logger->write(PWTS::getErrorStr(err));

        if (cmdParser->isSet(CMDArg::GET_DEVICE_DATA)) {
            printDeviceData(packet, features, coreCount);
            emit quit(0);

        } else if (cmdParser->isSet(CMDArg::SET_DEVICE_SETTINGS)) {
            applyDeviceSettings(packet);
        }
    }

    void PowerTunerCLI::onServiceDaemonSettingsReceived(const QByteArray &data) {
        const QSharedPointer<PWTS::DaemonSettings> daemonSettings = QSharedPointer<PWTS::DaemonSettings>::create();

        if (!daemonSettings->load(data))
            logger->write(QStringLiteral("failed to load daemon settings, using defaults"));

        if (cmdParser->isSet(CMDArg::GET_DAEMON_SETTINGS)) {
            printDaemonSettings(daemonSettings);
            emit quit(0);

        } else {
            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "address"))
                daemonSettings->setAddress(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "address").toString());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "apply_interval"))
                daemonSettings->setApplyInterval(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "apply_interval").toInt());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "start_profile"))
                daemonSettings->setOnStartProfile(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "start_profile").toString());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "battery_profile"))
                daemonSettings->setOnBatteryProfile(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "battery_profile").toString());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "psupply_profile"))
                daemonSettings->setOnPowerSupplyProfile(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "psupply_profile").toString());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "apply_on_wake"))
                daemonSettings->setApplyOnWakeFromSleep(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "apply_on_wake").toBool());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "ignore_bat_events"))
                daemonSettings->setIgnoreBatteryEvent(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "ignore_bat_events").toBool());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "log_level"))
                daemonSettings->setLogLevel(static_cast<PWTS::LogLevel>(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "log_level").toInt()));

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "max_log_files"))
                daemonSettings->setMaxLogFiles(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "max_log_files").toInt());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "tcp_port"))
                daemonSettings->setSocketTcpPort(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "tcp_port").toUInt());

            if (cmdParser->hasCmdValue(CMDArg::SET_DAEMON_SETTINGS, "udp_port"))
                daemonSettings->setSocketUdpPort(cmdParser->getCmdValue(CMDArg::SET_DAEMON_SETTINGS, "udp_port").toUInt());

            service->sendApplyDaemonSettingsRequest(daemonSettings->getData());
        }
    }

    void PowerTunerCLI::onServiceDaemonSettingsApplied(const bool diskSaveResult) {
        if (!diskSaveResult)
            logger->write(QStringLiteral("failed to write daemon settings to disk"));

        emit quit(!diskSaveResult);
    }

    void PowerTunerCLI::onServiceSettingsApplied(const QSet<PWTS::DError> &errors) {
        for (const PWTS::DError &err: errors)
            logger->write(PWTS::getErrorStr(err));

        printApplyResults(errors);

        if (cmdParser->isSet(CMDArg::SET_MAKE_PROFILE)) {
            const QString name = cmdParser->getCmdValue(CMDArg::SET_MAKE_PROFILE, "name").toString();

            if (!name.isEmpty()) {
                service->sendWriteProfileRequest(name, clientPacket);
                return;
            }

            logger->write(QStringLiteral("profile name cannot be empty"));
        }

        emit quit(!errors.isEmpty());
    }

    void PowerTunerCLI::onServiceProfileListReceived(const QList<QString> &list) {
        printProfileList(list);
        emit quit(0);
    }

    void PowerTunerCLI::onServiceProfileDeleted(const bool result) {
        if (!result)
            logger->write(QString("failed to delete profile '%1'").arg(cmdParser->getCmdValue(CMDArg::SET_DELETE_PROFILE, "profile").toString()));

        emit quit(!result);
    }

    void PowerTunerCLI::onServiceProfileApplied(const QSet<PWTS::DError> &errors, const QString &name) {
        printApplyResults(errors, name);
        emit quit(!errors.isEmpty());
    }

    void PowerTunerCLI::onServiceProfilesExported(const QHash<QString, QByteArray> &exported) {
        const QString path = cmdParser->getCmdValue(CMDArg::GET_EXPORT_PROFILES, "path").toString();
        const QDir qdir(path);

        if (!qdir.exists() && !qdir.mkpath(path)) {
            logger->write("failed to create profiles export path");
            emit quit(1);
            return;
        }

        for (const auto &[name, data]: exported.asKeyValueRange()) {
            QFile profileF {QString("%1/%2").arg(path, name)};

            if (!profileF.open(QFile::WriteOnly)) {
                logger->write(QString("failed to export profile '%1': %2").arg(name, profileF.errorString()));
                continue;
            }

            profileF.write(data);
            profileF.close();
        }

        emit quit(0);
    }

    void PowerTunerCLI::onServiceProfilesImported(const bool result) {
        if (!result)
            logger->write(QStringLiteral("failed to import some profiles"));

        emit quit(!result);
    }

    void PowerTunerCLI::onServiceProfileWritten(const bool result) {
        if (!result)
            logger->write(QStringLiteral("failed to write profile"));

        emit quit(!result);
    }
}
