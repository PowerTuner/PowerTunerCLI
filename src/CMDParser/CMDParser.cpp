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
#include <QCoreApplication>

#include "CMDParser.h"
#include "../../version.h"
#include "SettingsArguments.h"
#include "pwtShared/DaemonSettings.h"

namespace PWT::CLI {
    void CMDParser::nextArg(const int inc) {
        cmdArgc -= inc;
        cmdArgv += inc;
    }

    QString CMDParser::helpIndent(const int level) const {
        return QString(level * 2, ' ');
    }

    bool CMDParser::isArg(const char *arg, const char *expected) const {
        return std::strcmp(arg, expected) == 0;
    }

    bool CMDParser::parse(int argc, char *argv[]) {
        if (argc <= 1 || isArg(argv[0], "help")) {
            showHelp();
            return false;
        }

        cmdArgc = --argc;
        cmdArgv = ++argv;

        return parseMode();
    }

    bool CMDParser::parseMode() {
        if (cmdArgc <= 0) {
            showHelp();
            return false;

        } else if (isArg(cmdArgv[0], getArg)) {
            argumentsMap.insert(CMDArg::GET_MODE, {});
            nextArg();
            return parseGetCommand();

        } else if (isArg(cmdArgv[0], setArg)) {
            argumentsMap.insert(CMDArg::SET_MODE, {});
            nextArg();
            return parseSetCommand();

        } else if (isArg(cmdArgv[0], "help")) {
            nextArg();
            return parseAdvHelpCommand();
        }

        showHelp();
        return false;
    }

    bool CMDParser::parseGetCommand() {
        if (cmdArgc <= 0 || isArg(cmdArgv[0], "help")) {
            showGetHelp();
            return false;

        } else if (isArg(cmdArgv[0], listDaemonsArg)) {
            argumentsMap.insert(CMDArg::GET_DAEMON_LIST, {});
            return true;

        } else if (isArg(cmdArgv[0], dataPathArg)) {
            argumentsMap.insert(CMDArg::GET_DATA_PATH, {});
            return true;

        } else if (isArg(cmdArgv[0], daemonSettArg)) {
            nextArg();

            if (parseDaemon()) {
                argumentsMap.insert(CMDArg::GET_DAEMON_SETTINGS, {});
                return true;
            }
        } else if (isArg(cmdArgv[0], deviceInfoArg)) {
            nextArg();

            if (parseDaemon()) {
                argumentsMap.insert(CMDArg::GET_DEVICE_INFO, {});
                return true;
            }
        } else if (isArg(cmdArgv[0], profilesArg)) {
            nextArg();

            if (parseDaemon()) {
                argumentsMap.insert(CMDArg::GET_PROFILE_LIST, {});
                return true;
            }
        } else if (isArg(cmdArgv[0], exportProfilesArg)) {
            nextArg();
            return parseExportProfiles();

        } else if (isArg(cmdArgv[0], deviceDataArg)) {
            nextArg();

            if (parseDaemon()) {
                argumentsMap.insert(CMDArg::GET_DEVICE_DATA, {});
                return true;
            }
        }

        showGetHelp();
        return false;
    }

    bool CMDParser::parseSetCommand() {
        if (cmdArgc <= 0 || isArg(cmdArgv[0], "help")) {
            showSetHelp();
            return false;

        } else if (isArg(cmdArgv[0], resetCliSettArg)) {
            argumentsMap.insert(CMDArg::SET_RESET_CLI_SETTINGS, {});
            return true;

        } else if (isArg(cmdArgv[0], addDaemonsArg)) {
            nextArg();
            return parseAddDaemons();

        } else if (isArg(cmdArgv[0], removeDaemonsArg)) {
            nextArg();
            return parseRemoveDaemons();

        } else if (isArg(cmdArgv[0], daemonSettArg)) {
            nextArg();
            return parseSetDaemonSettings();

        } else if (isArg(cmdArgv[0], deleteProfileArg)) {
            nextArg();
            return parseDeleteProfile();

        } else if (isArg(cmdArgv[0], applyProfileArg)) {
            nextArg();
            return parseApplyProfile();

        } else if (isArg(cmdArgv[0], importProfilesArg)) {
            nextArg();
            return parseImportProfiles();

        } else if (isArg(cmdArgv[0], deviceSettingsArg)) {
            nextArg();
            return parseDeviceSettings();
        }

        showSetHelp();
        return false;
    }

    bool CMDParser::parseAdvHelpCommand() const {
        if (cmdArgc < 1) {
            showHelp();
            return false;
        }

        if (isArg(cmdArgv[0], helpLinuxSettingsArg))
            showLinuxSettingsListHelp();
#ifdef WITH_AMD
        else if (isArg(cmdArgv[0], helpLinuxAMDSettingsArg))
            showLinuxAMDSettingsListHelp();
        else if (isArg(cmdArgv[0], helpAMDCPUSettingsArg))
            showAMDCPUSettingsListHelp();
#endif
        else if (isArg(cmdArgv[0], helpWindowsSettingsArg))
            showWindowsSettingsListHelp();
#ifdef WITH_INTEL
        else if (isArg(cmdArgv[0], helpIntelCPUSettingsArg))
            showIntelCPUSettingsListHelp();
#endif
        else if (isArg(cmdArgv[0], helpFanControlSettingsArg))
            showFanControlSettingsListHelp();
        else
            showHelp();

        return false;
    }

    bool CMDParser::parseDaemon() {
        if (cmdArgc <= 0)
            return false;

        if (std::strchr(cmdArgv[0], ';') != nullptr) {
            const QList<QString> vals = QString(cmdArgv[0]).split(';', Qt::SkipEmptyParts);

            if (vals.size() != 2)
                return false;

            argumentsMap.insert(CMDArg::DAEMON, {
                {"adr", vals[0]},
                {"port", vals[1]}
            });
        } else {
            argumentsMap.insert(CMDArg::DAEMON, {{"name", QString(cmdArgv[0])}});
        }

        nextArg();
        return true;
    }

    bool CMDParser::parseAddDaemons() {
        if (cmdArgc < 3 || cmdArgc % 3 != 0) {
            showSetHelp();
            return false;
        }

        QList<QString> daemonsData;

        while (cmdArgc) {
            daemonsData.append(cmdArgv[0]); // name
            daemonsData.append(cmdArgv[1]); // adr
            daemonsData.append(cmdArgv[2]); // port
            nextArg(3);
        }

        argumentsMap.insert(CMDArg::SET_ADD_DAEMONS, {{"daemonsData", daemonsData}});
        return true;
    }

    bool CMDParser::parseRemoveDaemons() {
        if (cmdArgc <= 0) {
            showSetHelp();
            return false;
        }

        QList<QString> daemons;

        while (cmdArgc) {
            daemons.append(cmdArgv[0]);
            nextArg();
        }

        argumentsMap.insert(CMDArg::SET_REMOVE_DAEMONS, {{"daemons", daemons}});
        return true;
    }

    bool CMDParser::parseSetDaemonSettings() {
        if (cmdArgc < 2 || !parseDaemon()) {
            showSetHelp();
            return false;
        }

        QHash<QString, QVariant> data;

        while (cmdArgc) {
            const QList<QString> setting = QString(cmdArgv[0]).split('=', Qt::SkipEmptyParts);

            nextArg();

            if (setting.size() != 2)
                continue;

            data.insert(setting[0], setting[1]);
        }

        argumentsMap.insert(CMDArg::SET_DAEMON_SETTINGS, data);
        return true;
    }

    bool CMDParser::parseDeleteProfile() {
        if (cmdArgc < 2 || !parseDaemon()) {
            showSetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::SET_DELETE_PROFILE, {{"profile", QString(cmdArgv[0])}});
        return true;
    }

    bool CMDParser::parseApplyProfile() {
        if (cmdArgc < 2 || !parseDaemon()) {
            showSetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::SET_APPLY_PROFILE, {{"profile", QString(cmdArgv[0])}});
        return true;
    }

    bool CMDParser::parseExportProfiles() {
        if (cmdArgc < 3 || !parseDaemon()) {
            showGetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::GET_EXPORT_PROFILES, {
            {"path", QString(cmdArgv[0])},
            {"profile", QString(cmdArgv[1])}
        });
        return true;
    }

    bool CMDParser::parseImportProfiles() {
        if (cmdArgc < 2 || !parseDaemon()) {
            showSetHelp();
            return false;
        }

        QList<QString> profiles;

        while (cmdArgc) {
            profiles.append(cmdArgv[0]);
            nextArg();
        }

        argumentsMap.insert(CMDArg::SET_IMPORT_PROFILES, {{"profiles", profiles}});
        return true;
    }

    bool CMDParser::parseWindowsActiveScheme() {
        if (cmdArgc < 1) {
            showSetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::SET_WIN_ACTIVE_SCHEME, {{"guid", QString(cmdArgv[0])}});
        nextArg();
        return true;
    }

    bool CMDParser::parseWindowsDeleteSchemes() {
        if (cmdArgc < 1) {
            showSetHelp();
            return false;
        }

        const QList<QString> guids = QString(cmdArgv[0]).split(',', Qt::SkipEmptyParts);

        argumentsMap.insert(CMDArg::SET_WIN_DELETE_SCHEMES, {{"guids", guids}});
        nextArg();

        return true;
    }

    bool CMDParser::parseWindowsResetSchemes() {
        if (cmdArgc < 1) {
            showSetHelp();
            return false;
        }

        const QList<QString> guids = QString(cmdArgv[0]).split(',', Qt::SkipEmptyParts);

        argumentsMap.insert(CMDArg::SET_WIN_RESET_SCHEMES, {{"guids", guids}});
        nextArg();

        return true;
    }

    bool CMDParser::parseWindowsCopySchemeSettings() {
        if (cmdArgc < 4) {
            showSetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::SET_WIN_SCHEME_COPY_SETTINGS, {
            {"from_scheme", QString(cmdArgv[0])},
            {"from_mode", QString(cmdArgv[1])},
            {"to_scheme", QString(cmdArgv[2])},
            {"to_mode", QString(cmdArgv[3])}
        });

        nextArg(4);
        return true;
    }

    bool CMDParser::parseWindowsDuplicateScheme() {
        if (cmdArgc < 3) {
            showSetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::SET_WIN_DUPLICATE_SCHEME, {
            {"base_scheme", QString(cmdArgv[0])},
            {"name", QString(cmdArgv[1])},
            {"tmp_handle", QString(cmdArgv[2])}
        });

        nextArg(3);
        return true;
    }

    bool CMDParser::parseMakeProfile() {
        if (cmdArgc < 1) {
            showSetHelp();
            return false;
        }

        argumentsMap.insert(CMDArg::SET_MAKE_PROFILE, {{"name", QString(cmdArgv[0])}});

        nextArg();
        return true;
    }

    bool CMDParser::parseDeviceSettings() {
        if (cmdArgc < 2 || !parseDaemon()) {
            showSetHelp();
            return false;
        }

        QHash<QString, QVariant> data;

        while (cmdArgc) {
            if (isArg(cmdArgv[0], winPsResetDefaultsArg)) {
                nextArg();
                argumentsMap.insert(CMDArg::SET_WIN_SCHEMES_RESET_DEFAULTS, {});

            } else if (isArg(cmdArgv[0], winPsReplaceDefaultsArg)) {
                nextArg();
                argumentsMap.insert(CMDArg::SET_WIN_SCHEMES_REPLACE_DEFAULTS, {});

            } else if (isArg(cmdArgv[0], winPsActiveSchemeArg)) {
                nextArg();

                if (!parseWindowsActiveScheme())
                    return false;

            } else if (isArg(cmdArgv[0], winPsDeleteSchemesArg)) {
                nextArg();

                if (!parseWindowsDeleteSchemes())
                    return false;

            } else if (isArg(cmdArgv[0], winPsResetSchemesArg)) {
                nextArg();

                if (!parseWindowsResetSchemes())
                    return false;

            } else if (isArg(cmdArgv[0], winPsCopySchemeSettingsArg)) {
                nextArg();

                if (!parseWindowsCopySchemeSettings())
                    return false;

            } else if (isArg(cmdArgv[0], winPsDuplicateSchemeArg)) {
                nextArg();

                if (!parseWindowsDuplicateScheme())
                    return false;

            } else if (isArg(cmdArgv[0], makeProfileArg)) {
                nextArg();

                if (!parseMakeProfile())
                    return false;

            } else {
                const QList<QString> setting = QString(cmdArgv[0]).split('=', Qt::SkipEmptyParts);

                nextArg();

                if (setting.size() != 2)
                    continue;

                if (setting[0].back() == ']') { // indexed data
                    const QList<QString> settingArg = setting[0].split('[');
                    const QString idx = settingArg[1].sliced(0, settingArg[1].size() - 1);
                    const QString &arg = settingArg[0];
                    const QString &argVal = setting[1];
                    QHash<QString, QVariant> argData {};

                    if (data.contains(arg))
                        argData = data[arg].toHash();

                    if (idx.isEmpty())
                        argData.insert("all", argVal);
                    else
                        argData.insert(idx, argVal);

                    data.insert(arg, argData);

                } else {
                    data.insert(setting[0], setting[1]);
                }
            }
        }

        argumentsMap.insert(CMDArg::SET_DEVICE_SETTINGS, data);
        return true;
    }

    QVariant CMDParser::getCmdValue(const CMDArg arg, const QString &value) const {
        if (!argumentsMap.contains(arg) || !argumentsMap[arg].contains(value))
            return {};

        return argumentsMap[arg].value(value);
    }

    void CMDParser::showHelp() const {
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "PowerTunerCLI v" << CLIENT_VER_MAJOR << "." << CLIENT_VER_MINOR << " - GPLv3 kylon\n\n"
            << "Usage: " << QCoreApplication::applicationName() << " <mode> <options>\n\n"
            << helpIndent(helpIndentLv1) << "help\n"
            << helpIndent(helpIndentLv2) << "Show help.\n\n"
            << helpIndent(helpIndentLv2) << "Options:\n"
            << helpIndent(helpIndentLv3) << helpLinuxSettingsArg << "\n"
            << helpIndent(helpIndentLv4) << "List linux settings for " << deviceSettingsArg << " option.\n\n"
#ifdef WITH_AMD
            << helpIndent(helpIndentLv3) << helpLinuxAMDSettingsArg << "\n"
            << helpIndent(helpIndentLv4) << "List linux AMD settings for " << deviceSettingsArg << " option.\n\n"
#endif
            << helpIndent(helpIndentLv3) << helpWindowsSettingsArg << "\n"
            << helpIndent(helpIndentLv4) << "List windows settings for " << deviceSettingsArg << " option.\n\n"
#ifdef WITH_AMD
            << helpIndent(helpIndentLv3) << helpAMDCPUSettingsArg << "\n"
            << helpIndent(helpIndentLv4) << "List AMD CPU settings for " << deviceSettingsArg << " option.\n\n"
#endif
#ifdef WITH_INTEL
            << helpIndent(helpIndentLv3) << helpIntelCPUSettingsArg << "\n"
            << helpIndent(helpIndentLv4) << "List Intel CPU settings for " << deviceSettingsArg << " option.\n\n"
#endif
            << helpIndent(helpIndentLv3) << helpFanControlSettingsArg << "\n"
            << helpIndent(helpIndentLv4) << "List fan control settings for " << deviceSettingsArg << " option.\n\n"
            << helpIndent(helpIndentLv1) << getArg << "\n"
            << helpIndent(helpIndentLv2) << "Request and print data.\n\n"
            << helpIndent(helpIndentLv1) << setArg << "\n"
            << helpIndent(helpIndentLv2) << "Set options.\n\n"
            << "\n"
            << QCoreApplication::applicationName() << " <mode> help, for more help\n"
            << "\n"
        ;
    }

    void CMDParser::showGetHelp() const {
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "PowerTunerCLI v" << CLIENT_VER_MAJOR << "." << CLIENT_VER_MINOR << " - GPLv3 kylon\n\n"
            << "Usage: " << QCoreApplication::applicationName() << " get <options>\n\n"
            << helpIndent(helpIndentLv1) << "help\n"
            << helpIndent(helpIndentLv2) << "Show help.\n\n"
            << helpIndent(helpIndentLv1) << dataPathArg << "\n"
            << helpIndent(helpIndentLv2) << "Print CLI data path.\n\n"
            << helpIndent(helpIndentLv1) << listDaemonsArg << "\n"
            << helpIndent(helpIndentLv2) << "List saved daemons.\n\n"
            << helpIndent(helpIndentLv1) << daemonSettArg << " " << daemonArg << "\n"
            << helpIndent(helpIndentLv2) << "Request and print daemon settings.\n\n"
            << helpIndent(helpIndentLv1) << deviceInfoArg << " " << daemonArg << "\n"
            << helpIndent(helpIndentLv2) << "Request and print device info.\n\n"
            << helpIndent(helpIndentLv1) << profilesArg << " " << daemonArg << "\n"
            << helpIndent(helpIndentLv2) << "Request and print available profiles.\n\n"
            << helpIndent(helpIndentLv1) << exportProfilesArg << " " << daemonArg << " <output path> <profile|all>\n"
            << helpIndent(helpIndentLv2) << "Download a profile, or \"all\", to <output path>.\n\n"
            << helpIndent(helpIndentLv1) << deviceDataArg << " " << daemonArg << "\n"
            << helpIndent(helpIndentLv2) << "Request and print device data.\n\n"
            << "\n"
        ;
    }

    void CMDParser::showSetHelp() const {
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "PowerTunerCLI v" << CLIENT_VER_MAJOR << "." << CLIENT_VER_MINOR << " - GPLv3 kylon\n\n"
            << "Usage: " << QCoreApplication::applicationName() << " set <options>\n\n"
            << helpIndent(helpIndentLv1) << "help\n"
            << helpIndent(helpIndentLv2) << "Show help.\n\n"
            << helpIndent(helpIndentLv1) << resetCliSettArg << "\n"
            << helpIndent(helpIndentLv2) << "Reset CLI settings.\n\n"
            << helpIndent(helpIndentLv1) << addDaemonsArg << " <name> <address> <port>\n"
            << helpIndent(helpIndentLv2) << "List of daemons (name, address and port) to add to CLI.\n\n"
            << helpIndent(helpIndentLv1) << removeDaemonsArg << " <name|address>\n"
            << helpIndent(helpIndentLv2) << "List of space separated daemons to remove from CLI.\n\n"
            << helpIndent(helpIndentLv1) << daemonSettArg << " " << daemonArg << " <setting=value>\n"
            << helpIndent(helpIndentLv2) << "List of daemon settings, with their value, to set.\n\n"
            << helpIndent(helpIndentLv2) << "Available settings:\n\n"
            << helpIndent(helpIndentLv3) << "start_profile\n"
            << helpIndent(helpIndentLv4) << "Profile to apply on daemon start.\n\n"
            << helpIndent(helpIndentLv3) << "battery_profile\n"
            << helpIndent(helpIndentLv4) << "Profile to apply when running on battery (DC).\n\n"
            << helpIndent(helpIndentLv3) << "psupply_profile\n"
            << helpIndent(helpIndentLv4) << "Profile to apply when connected to a power adapter (AC).\n\n"
            << helpIndent(helpIndentLv3) << "address\n"
            << helpIndent(helpIndentLv4) << "Daemon listening address.\n\n"
            << helpIndent(helpIndentLv3) << "apply_on_wake\n"
            << helpIndent(helpIndentLv4) << "Reapply last applied settings on wake from sleep. [0,1]\n\n"
            << helpIndent(helpIndentLv3) << "ignore_bat_events\n"
            << helpIndent(helpIndentLv4) << "Ignore battery events. [0,1]\n\n"
            << helpIndent(helpIndentLv3) << "apply_interval\n"
            << helpIndent(helpIndentLv4) << "Reapply last applied settings every X seconds. [" << PWTS::DaemonSettings::MinApplyInterval << "," << PWTS::DaemonSettings::MaxApplyInterval << "]\n\n"
            << helpIndent(helpIndentLv3) << "log_level\n"
            << helpIndent(helpIndentLv4) << "Log verbosity. [0=Info, 1=Warning, 2=Error, 3=Service]\n\n"
            << helpIndent(helpIndentLv3) << "max_log_files\n"
            << helpIndent(helpIndentLv4) << "Maximum log files to keep on disk.\n\n"
            << helpIndent(helpIndentLv3) << "tcp_port\n"
            << helpIndent(helpIndentLv4) << "Daemon TCP port.\n\n"
            << helpIndent(helpIndentLv3) << "udp_port\n"
            << helpIndent(helpIndentLv4) << "Daemon UDP port.\n\n"
            << helpIndent(helpIndentLv1) << deleteProfileArg << " " << daemonArg << " <profile>\n"
            << helpIndent(helpIndentLv2) << "Delete a profile from disk.\n\n"
            << helpIndent(helpIndentLv1) << applyProfileArg << " " << daemonArg << " <profile>\n"
            << helpIndent(helpIndentLv2) << "Apply settings from profile.\n\n"
            << helpIndent(helpIndentLv1) << importProfilesArg << " " << daemonArg << " <profile>\n"
            << helpIndent(helpIndentLv2) << "List of space separated paths to profiles to import.\n\n"
            << helpIndent(helpIndentLv1) << deviceSettingsArg << " " << daemonArg << " <setting=value>\n"
            << helpIndent(helpIndentLv2) << "List of device settings, with their value, to set.\n\n"
            << helpIndent(helpIndentLv2) << "To set core/thread level settings, append the CPU index like this: setting[cpu]=value.\n"
            << helpIndent(helpIndentLv2) << "To set the same value to all cores/threads, use an empty index: setting[]=value.\n"
            << helpIndent(helpIndentLv2) << "To set GPU settings, append the GPU index like this: setting[idx]=value.\n"
            << helpIndent(helpIndentLv2) << "To set the same value to all GPUs where the setting applies, use an empty index: setting[]=value.\n"
            << helpIndent(helpIndentLv2) << "To set fan settings, append the fan ID like this: setting[id]=value.\n"
            << helpIndent(helpIndentLv2) << "A range of possible values, where fixed, is provided in the form of [value1, value2..] or [min, max].\n"
            << helpIndent(helpIndentLv2) << "Settings not available in the target device will be ignored.\n\n"
            << helpIndent(helpIndentLv2) << "Print a list of supported settings with help command.\n\n"
            << helpIndent(helpIndentLv2) << "Options:\n"
            << helpIndent(helpIndentLv3) << makeProfileArg << " <profile name>\n"
            << helpIndent(helpIndentLv4) << "Create a profile from current device settings.\n\n"
            << "\n"
        ;
    }

    void CMDParser::showLinuxSettingsListHelp() const {
        const QString sendDataReqHelp = QString("Send %1 request for possible values.\n\n").arg(deviceDataArg);
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "Sysfs:\n"
            << helpIndent(helpIndentLv1) << smtStateArg << "\n"
            << helpIndent(helpIndentLv2) << "Simultaneous Multi-Threading / Hyper-Threading. [on, off, forceoff]\n\n"
            << helpIndent(helpIndentLv1) << cpuIdleGovernorArg << "\n"
            << helpIndent(helpIndentLv2) << "CPU Idle Governor.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << blockDevQueSchedArg << "\n"
            << helpIndent(helpIndentLv2) << "Block device queue scheduler.\n"
            << helpIndent(helpIndentLv2) << "Usage: " << blockDevQueSchedArg << "[device]=scheduler.\n"
            << helpIndent(helpIndentLv2) << "Example: \"" << blockDevQueSchedArg << "[sda]\"=mq-deadline.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << miscPmDevsArg << "\n"
            << helpIndent(helpIndentLv2) << "Devices power management mode. [auto, on]\n"
            << helpIndent(helpIndentLv2) << "Usage: " << miscPmDevsArg << "[control path]=mode.\n"
            << helpIndent(helpIndentLv2) << "Example: \"" << miscPmDevsArg << "[/sys/bus/usb/devices/2-1/power/control]\"=auto.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cpuMinFreqArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPU minimum frequency.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cpuMaxFreqArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPU maximum frequency.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cpuScalingGovernorArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPU scaling governor.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cpuStatusArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPU online status. [0,1]\n\n"
            << "Intel GPUs:\n"
            << helpIndent(helpIndentLv1) << gpuRpsMinFreqArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU minimum frequency.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << gpuRpsMaxFreqArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU maximum frequency.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << gpuRpsMaxBoostArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU maximum boost frequency.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << "AMD GPUs:\n"
            << helpIndent(helpIndentLv1) << powerDpmStateArg << "\n"
            << helpIndent(helpIndentLv2) << "Legacy power DPM state. [battery, balanced, performance]\n\n"
            << helpIndent(helpIndentLv1) << powerDpmForcePerfLvlArg << "\n"
            << helpIndent(helpIndentLv2) << "Power DPM force performance level.\n"
            << helpIndent(helpIndentLv2) << "[auto, low, high, manual, profile_standard, profile_min_sclk, profile_min_mclk, profile_peak]\n\n"
            << helpIndent(helpIndentLv1) << powerDpmForcePerfMinSclkArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU minimum sclk. Requires power_dpm_force_perf_level=manual.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << powerDpmForcePerfMaxSclkArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU maximum sclk. Requires power_dpm_force_perf_level=manual.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << powerDpmForcePerfResetArg << "\n"
            << helpIndent(helpIndentLv2) << "Perform DPM performance level settings reset. [0,1]\n\n";
    }
#ifdef WITH_AMD
    void CMDParser::showLinuxAMDSettingsListHelp() const {
        const QString sendDataReqHelp = QString("Send %1 request for possible values.\n\n").arg(deviceDataArg);
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "AMD CPUs:\n"
            << helpIndent(helpIndentLv1) << pstateStatusArg << "\n"
            << helpIndent(helpIndentLv2) << "AMD PState Scaling Driver status. [disable, active, passive, guided]\n\n"
            << helpIndent(helpIndentLv1) << pstateEppArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "Requires pstate_status=active\n"
            << helpIndent(helpIndentLv2) << "AMD PState Scaling Driver energy performance preference.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp;
    }

    void CMDParser::showAMDCPUSettingsListHelp() const {
        const QString sendDataReqHelp = QString("Send %1 request for possible values.\n\n").arg(deviceDataArg);
        const QString sendInfoReqHelp = QString("Send %1 request for input ranges.\n\n").arg(deviceInfoArg);
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "AMD CPUs:\n"
            << helpIndent(helpIndentLv1) << stapmLimitArg << "\n"
            << helpIndent(helpIndentLv2) << "Skin temperature-aware power management limit (STAPM).\n"
            << helpIndent(helpIndentLv2) << "Sustained power limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << fastLimitArg << "\n"
            << helpIndent(helpIndentLv2) << "Package power tracking fast limit (PPT fast).\n"
            << helpIndent(helpIndentLv2) << "Actual power limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << slowLimitArg << "\n"
            << helpIndent(helpIndentLv2) << "Package power tracking slow limit (PPT slow).\n"
            << helpIndent(helpIndentLv2) << "Average power limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << tctlTempArg << "\n"
            << helpIndent(helpIndentLv2) << "T control temperature (TCTL).\n"
            << helpIndent(helpIndentLv2) << "Thermal limit core (celsius).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << apuSlowArg << "\n"
            << helpIndent(helpIndentLv2) << "Package power tracking APU slow limit (PPT APU).\n"
            << helpIndent(helpIndentLv2) << "APU slow power limit for A+A dGPU platform (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << apuSkinTempArg << "\n"
            << helpIndent(helpIndentLv2) << "Smart temperature tracking APU (STT APU).\n"
            << helpIndent(helpIndentLv2) << "APU skin temperature limit (celsius).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << dgpuSkinTempArg << "\n"
            << helpIndent(helpIndentLv2) << "Smart temperature tracking dGPU (STT dGPU).\n"
            << helpIndent(helpIndentLv2) << "dGPU skin temperature limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << curveOptimizerArg << "\n"
            << helpIndent(helpIndentLv2) << "Curve optimizer offset (All cores).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << curveOptimizerPerArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "Curve optimizer offset.\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << vrmCurrentArg << "\n"
            << helpIndent(helpIndentLv2) << "Thermal Design Current Limit (TDC VDD).\n"
            << helpIndent(helpIndentLv2) << "VRM current limit (amps).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << vrmSocCurrentArg << "\n"
            << helpIndent(helpIndentLv2) << "Thermal Design Current Limit (TDC SoC).\n"
            << helpIndent(helpIndentLv2) << "VRM SoC current limit (amps).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << vrmMaxCurrentArg << "\n"
            << helpIndent(helpIndentLv2) << "Electrical Design Current Limit (EDC VDD).\n"
            << helpIndent(helpIndentLv2) << "VRM maximum current limit (amps).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << vrmSocMaxCurrentArg << "\n"
            << helpIndent(helpIndentLv2) << "Electrical Design Current Limit (EDC SoC).\n"
            << helpIndent(helpIndentLv2) << "VRM SoC maximum current limit (amps).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << staticGfxClkArg << "\n"
            << helpIndent(helpIndentLv2) << "Force static GPU clock (MHz).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << minGfxClkArg << "\n"
            << helpIndent(helpIndentLv2) << "Maximum GPU clock (MHz).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << maxGfxClkArg << "\n"
            << helpIndent(helpIndentLv2) << "Minimum GPU clock (MHz).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << amdPowerProfileArg << "\n"
            << helpIndent(helpIndentLv2) << "Power profile. [0 = powersave, 1 = performance]\n"
            << helpIndent(helpIndentLv2) << "Hidden option to improve power saving or performance.\n"
            << helpIndent(helpIndentLv2) << "Behavior depends on CPU, device and manufacturer.\n\n"
            << helpIndent(helpIndentLv1) << corePerfBoostArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "Disable core performance boost. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pstateCmdArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPU P-State control.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cppcEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable CPPC. [1,1]\n\n"
            << helpIndent(helpIndentLv1) << cppcRequestMinPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPPC request minimum performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cppcRequestMaxPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPPC request maximum performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cppcRequestDesiredPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPPC request desired performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << cppcRequestEppArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "CPPC request energy performance preference. [0, 255]\n\n"
            << helpIndent(helpIndentLv3) << cppcRequestPreferOSArg << "\n"
            << helpIndent(helpIndentLv4) << "Prefer CPPC request set by OS settings, like Windows power scheme or Linux sysfs. [0,1]\n\n";
    }
#endif

    void CMDParser::showWindowsSettingsListHelp() const {
        const QString sendDataReqHelp = QString("Send %1 request for possible values.\n\n").arg(deviceDataArg);
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "Windows:\n"
            << helpIndent(helpIndentLv1) << "Power schemes:\n"
            << helpIndent(helpIndentLv2) << powerSchemeArg << "\n"
            << helpIndent(helpIndentLv3) << "Windows power scheme setting.\n\n"
            << helpIndent(helpIndentLv3) << "Usage: " << powerSchemeArg << "[scheme GUID;setting GUID;mode]\n"
            << helpIndent(helpIndentLv3) << "Example: \"" << powerSchemeArg << "[10401140-123-41f0-iofd-dd5bb2g0dhge;381b4222-f694-41f0-9685-ff5bb260df2e;ac]\"=10\n"
            << helpIndent(helpIndentLv3) << sendDataReqHelp
            << helpIndent(helpIndentLv2) << "Options:\n"
            << helpIndent(helpIndentLv3) << winPsResetDefaultsArg << "\n"
            << helpIndent(helpIndentLv4) << "Reset power schemes to default before applying settings.\n\n"
            << helpIndent(helpIndentLv3) << winPsReplaceDefaultsArg << "\n"
            << helpIndent(helpIndentLv4) << "Replace default power schemes with current schemes after apply.\n\n"
            << helpIndent(helpIndentLv3) << winPsActiveSchemeArg << " <scheme GUID>\n"
            << helpIndent(helpIndentLv4) << "Set the active power scheme.\n\n"
            << helpIndent(helpIndentLv3) << winPsDeleteSchemesArg << " <scheme GUID>\n"
            << helpIndent(helpIndentLv4) << "Comma separated list of power schemes to delete.\n\n"
            << helpIndent(helpIndentLv3) << winPsResetSchemesArg << " <scheme GUID>\n"
            << helpIndent(helpIndentLv4) << "Comma separated list of power schemes to reset to defaults from default user schemes stored in HKEY_USERS\\.Default.\n\n"
            << helpIndent(helpIndentLv3) << winPsCopySchemeSettingsArg << " <from scheme GUID> <from mode> <to scheme GUID> <to mode>\n"
            << helpIndent(helpIndentLv4) << "Copy all settings from one scheme to another.\n"
            << helpIndent(helpIndentLv4) << "<from mode> valid values: ac, dc.\n"
            << helpIndent(helpIndentLv4) << "<to mode> valid values: ac, dc, all.\n"
            << helpIndent(helpIndentLv4) << "Copy command is run before " << powerSchemeArg << " and after " << winPsDuplicateSchemeArg << ".\n\n"
            << helpIndent(helpIndentLv3) << winPsDuplicateSchemeArg << " <base scheme GUID> <scheme name> <temp scheme handle>\n"
            << helpIndent(helpIndentLv4) << "Create a new power scheme based on <base scheme GUID>.\n"
            << helpIndent(helpIndentLv4) << "<temp scheme handle> is a string you can use, in place of its GUID, to set its settings.\n"
            << helpIndent(helpIndentLv4) << "<temp scheme handle> allowed characters: [a-z0-9_-].\n"
            << helpIndent(helpIndentLv4) << "Duplicate command is run before " << powerSchemeArg << ".\n\n";
    }

#ifdef WITH_INTEL
    void CMDParser::showIntelCPUSettingsListHelp() const {
        const QString sendDataReqHelp = QString("Send %1 request for possible values.\n\n").arg(deviceDataArg);
        const QString sendInfoReqHelp = QString("Send %1 request for input ranges.\n\n").arg(deviceInfoArg);
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "Intel CPUs:\n"
            << helpIndent(helpIndentLv1) << pkgLimitPl1Arg << "\n"
            << helpIndent(helpIndentLv2) << "PKG power limit PL1 (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << pkgLimitPl2Arg << "\n"
            << helpIndent(helpIndentLv2) << "PKG power limit PL2 (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << pkgLimitPl1TimeArg << "\n"
            << helpIndent(helpIndentLv2) << "PKG power limit PL1 time window. [0, 56000]\n\n"
            << helpIndent(helpIndentLv1) << pkgLimitPl2TimeArg << "\n"
            << helpIndent(helpIndentLv2) << "PKG power limit PL2 time window. [0, 56000]\n\n"
            << helpIndent(helpIndentLv1) << pkgLimitPl1ClampArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable value clamp for PKG power limit PL1. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgLimitPl2ClampArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable value clamp for PKG power limit PL2. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgLimitPl1EnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable PKG power limit PL1. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgLimitPl2EnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable PKG power limit PL2. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgLimitLockArg << "\n"
            << helpIndent(helpIndentLv2) << "Lock PKG power limit register until device reboot. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << vrCurrentCfgPl4Arg << "\n"
            << helpIndent(helpIndentLv2) << "VR current config limit PL4 (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << vrCurrentCfgLockArg << "\n"
            << helpIndent(helpIndentLv2) << "Lock VR current config register until device reboot. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pp1CurrentLimitArg << "\n"
            << helpIndent(helpIndentLv2) << "Graphics current config (PP1) limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << pp1CurrentLockArg << "\n"
            << helpIndent(helpIndentLv2) << "Lock graphics current config (PP1) register until device reboot. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << turboPwrCurrentTdpArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo power current TDP limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << turboPwrCurrentTdpEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable turbo power current TDP limit. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << turboPwrCurrentTdcArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo power current TDC limit (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << turboPwrCurrentTdcEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable turbo power current TDC limit. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit1cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 1 core. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit2cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 2 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit3cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 3 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit4cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 4 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit5cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 5 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit6cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 6 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit7cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 7 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << turboRatioLimit8cArg << "\n"
            << helpIndent(helpIndentLv2) << "Turbo ratio limit for 8 cores. [1, 255]\n\n"
            << helpIndent(helpIndentLv1) << enhancedSpeedstepEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable CPU enhanced speedstep technology. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << turboModeDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable CPU turbo boost. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << bdProchotEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable Bi-Directional PROCHOT. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << c1eEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable C1 Enhanced (C1E). [0,1]\n\n"
            << helpIndent(helpIndentLv1) << sapmImcC2PolicyArg << "\n"
            << helpIndent(helpIndentLv2) << "Allow self-refresh in package C2 state. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << fastBrkSnpEnArg << "\n"
            << helpIndent(helpIndentLv2) << "Use fast VID swing rate. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << powerPerformancePlatformOverrideArg << "\n"
            << helpIndent(helpIndentLv2) << "Power performance platform override. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << energyEfficiencyOptDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable energy efficiency optimization. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << raceToHaltDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable race to halt optimization. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << prochotOutputDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable PROCHOT output. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << prochotCfgResponseEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable PROCHOT configurable response. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << vrThermAlertDisableLockArg << "\n"
            << helpIndent(helpIndentLv2) << "Lock PROCHOT bits until device reboot. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << vrThermAlertDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable VR_THERMAL_ALERT signaling. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << ringEEDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable ring EE. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << saOptDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable SA optimization. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << ookDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable OOK. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpAutonomousDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable HWP autonomous mode. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << cstatePrewakeDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable C-state pre-wake. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << eistDisableArg << "\n"
            << helpIndent(helpIndentLv2) << "Disable EIST (Enhanced SpeedStep) hardware coordination. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pp0PriorityArg << "\n"
            << helpIndent(helpIndentLv2) << "CPU power balance. [0, 31]\n\n"
            << helpIndent(helpIndentLv1) << pp1PriorityArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU power balance. [0, 31]\n\n"
            << helpIndent(helpIndentLv1) << energyPerfBiasArg << "\n"
            << helpIndent(helpIndentLv2) << "Performance Energy Bias Hint. [0, 15]\n\n"
            << helpIndent(helpIndentLv1) << uvControlCpuArg << "\n"
            << helpIndent(helpIndentLv2) << "CPU undervolt offset.\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << uvControlGpuArg << "\n"
            << helpIndent(helpIndentLv2) << "GPU undervolt offset.\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << uvControlCacheArg << "\n"
            << helpIndent(helpIndentLv2) << "Cache undervolt offset.\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << uvControlUnsliceArg << "\n"
            << helpIndent(helpIndentLv2) << "Unslice undervolt offset.\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << uvControlSaArg << "\n"
            << helpIndent(helpIndentLv2) << "SysAgent undervolt offset.\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << hwpEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable HWP. [1,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpPkgCtlPolarityEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Enable HWP pkg ctl polarity. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestPkgMinPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "HWP request pkg minimum performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << hwpRequestPkgMaxPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "HWP request pkg maximum performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << hwpRequestPkgDesiredPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "HWP request pkg desired performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << hwpRequestPkgEppArg << "\n"
            << helpIndent(helpIndentLv2) << "HWP request pkg energy performance preference. [0, 255]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestPkgAcwArg << "\n"
            << helpIndent(helpIndentLv2) << "HWP request pkg activity window. [0, 127000]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestMinPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request minimum performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << hwpRequestMaxPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request maximum performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << hwpRequestDesiredPerfArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request desired performance.\n"
            << helpIndent(helpIndentLv2) << sendDataReqHelp
            << helpIndent(helpIndentLv1) << hwpRequestEppArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request energy performance preference. [0, 255]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestAcwArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request activity window. [0, 127000]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestPkgControlArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request package control flag. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestMinValidArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request minimum performance validity. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestMaxValidArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request maximum performance validity. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestDesiredValidArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request desired performance validity. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestEppValidArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request energy performance preference validity. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << hwpRequestAcwValidArg << "\n"
            << helpIndent(helpIndentLv2) << "Thread level setting.\n"
            << helpIndent(helpIndentLv2) << "HWP request activity window validity. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlCstateLimArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "C-State configuration control package C-State limit.\n\n"
            << helpIndent(helpIndentLv2) << "sandy/ivy bridge possible values:\n"
            << helpIndent(helpIndentLv3) << "[0] C0/C1 (no package C-sate support)\n"
            << helpIndent(helpIndentLv3) << "[1] C2\n"
            << helpIndent(helpIndentLv3) << "[2] C6 no retention\n"
            << helpIndent(helpIndentLv3) << "[3] C6 retention\n"
            << helpIndent(helpIndentLv3) << "[4] C7\n"
            << helpIndent(helpIndentLv3) << "[5] C7s\n"
            << helpIndent(helpIndentLv3) << "[7] No package C-state limit\n\n"
            << helpIndent(helpIndentLv2) << "core ultra series possible values:\n"
            << helpIndent(helpIndentLv3) << "[0] C0/C1 (no package C-sate support)\n"
            << helpIndent(helpIndentLv3) << "[1] C2\n"
            << helpIndent(helpIndentLv3) << "[2] C3\n"
            << helpIndent(helpIndentLv3) << "[3] C6\n"
            << helpIndent(helpIndentLv3) << "[4] C7\n"
            << helpIndent(helpIndentLv3) << "[5] C7s\n"
            << helpIndent(helpIndentLv3) << "[6] C8\n"
            << helpIndent(helpIndentLv3) << "[7] C9\n"
            << helpIndent(helpIndentLv3) << "[8] C10\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlMaxCoreCstateArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "Max core C-State.\n"
            << helpIndent(helpIndentLv2) << "core ultra series possible values:\n"
            << helpIndent(helpIndentLv3) << "[1] C1\n"
            << helpIndent(helpIndentLv3) << "[2] C3\n"
            << helpIndent(helpIndentLv3) << "[3] C6\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlIOMWaitRedirectEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "I/O MWAIT redirect enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlCfgLockArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "Lock C-State limit and I/O MWAIT redirection settings until device reboot. [1,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlC3StateAutodemotionEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "C3 state auto-demotion enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlC1StateAutodemotionEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "C1 state auto-demotion enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlC3UndemotionEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "C3 un-demotion enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlC1UndemotionEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "C1 un-demotion enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlPkgcAutodemotionEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "Package C-State auto-demotion enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlPkgcUndemotionEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "Package C-State un-demotion enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgCstCfgControlTimedMwaitEnableArg << "\n"
            << helpIndent(helpIndentLv2) << "Core level setting.\n"
            << helpIndent(helpIndentLv2) << "Timed MWAIT feature enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgRaplPl1Arg << "\n"
            << helpIndent(helpIndentLv2) << "MCHBAR package power limit PL1 (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << pkgRaplPl2Arg << "\n"
            << helpIndent(helpIndentLv2) << "MCHBAR package power limit PL2 (watts).\n"
            << helpIndent(helpIndentLv2) << sendInfoReqHelp
            << helpIndent(helpIndentLv1) << pkgRaplPl1EnableArg << "\n"
            << helpIndent(helpIndentLv2) << "MCHBAR package power limit PL1 enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgRaplPl2EnableArg << "\n"
            << helpIndent(helpIndentLv2) << "MCHBAR package power limit PL2 enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgRaplPl1ClampArg << "\n"
            << helpIndent(helpIndentLv2) << "MCHBAR package power limit PL1 clamp enable. [0,1]\n\n"
            << helpIndent(helpIndentLv1) << pkgRaplPl1TimeArg << "\n"
            << helpIndent(helpIndentLv2) << "MCHBAR package power limit PL1 time window. [0, 56000]\n\n"
            << helpIndent(helpIndentLv1) << pkgRaplLockArg << "\n"
            << helpIndent(helpIndentLv2) << "Lock MCHBAR package power limit register until device reboot. [0,1]\n\n"
        ;
    }
#endif

    void CMDParser::showFanControlSettingsListHelp() const {
        QTextStream ts(stdout);

        ts.setFieldAlignment(QTextStream::AlignLeft);

        ts << "Fan control:\n"
            << helpIndent(helpIndentLv1) << fanModeArg << "\n"
            << helpIndent(helpIndentLv2) << "Fan mode. [0=auto, 1=manual]\n\n"
            << helpIndent(helpIndentLv1) << fanTripPointArg << "\n"
            << helpIndent(helpIndentLv2) << "Fan temperature trip point. Requires fan_mode[id]=1.\n"
            << helpIndent(helpIndentLv2) << "Define more than one trip point to create a curve.\n"
            << helpIndent(helpIndentLv2) << "To set a fixed fan speed, provide only one trip point with no temperature.\n\n"
            << helpIndent(helpIndentLv2) << "Usage: \"" << fanTripPointArg << "[id:temperature_celsius]=speed_percent\".\n"
            << helpIndent(helpIndentLv2) << "Usage (fixed speed): \"" << fanTripPointArg << "[id]=speed_percent\".\n"
            << helpIndent(helpIndentLv2) << "Example: \"" << fanTripPointArg << "[3hd907:40]=25\".\n\n"
        ;
    }
}
