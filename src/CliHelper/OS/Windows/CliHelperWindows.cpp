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
#include <QRegularExpression>

#include "CliHelperWindows.h"
#include "../../../CMDParser/SettingsArguments.h"

namespace PWT::CLI {
    CliHelperWindows::CliHelperWindows(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures, const QSharedPointer<PWTS::WIN::WindowsData> &data): CliHelper(cmd, daemonFeatures) {
        packetData = data;
    }

    void CliHelperWindows::duplicatePowerScheme() const {
        if (!cmdParser->isSet(CMDArg::SET_WIN_DUPLICATE_SCHEME))
            return;

        const QString base = cmdParser->getCmdValue(CMDArg::SET_WIN_DUPLICATE_SCHEME, "base_scheme").toString().prepend('{').append('}');
        const QString name = cmdParser->getCmdValue(CMDArg::SET_WIN_DUPLICATE_SCHEME, "name").toString();
        const QString tmpHandle = cmdParser->getCmdValue(CMDArg::SET_WIN_DUPLICATE_SCHEME, "tmp_handle").toString();
        const QString tmpGuid = QString("{guid-tbd-%1}").arg(tmpHandle);
        const QRegularExpression tbdGuidRex {R"(^[\w\d\-\_]+$)"};

        if (!tbdGuidRex.match(tmpHandle).hasMatch()) {
            logger->write(QString("invalid handle string '%1': allowed characters: [a-z0-9_-]").arg(tmpHandle));
            return;

        } else if (!packetData->schemes.contains(base)) {
            logger->write(QString("base scheme %1 not found").arg(base));
            return;
        }

        packetData->schemes.insert(tmpGuid, packetData->schemes[base]);
        packetData->schemes[tmpGuid].friendlyName = name;
    }

    void CliHelperWindows::copyPowerSchemeSettings() const {
        if (!cmdParser->isSet(CMDArg::SET_WIN_SCHEME_COPY_SETTINGS))
            return;

        const QString fromScheme = cmdParser->getCmdValue(CMDArg::SET_WIN_SCHEME_COPY_SETTINGS, "from_scheme").toString().prepend('{').append('}');
        const QString fromMode = cmdParser->getCmdValue(CMDArg::SET_WIN_SCHEME_COPY_SETTINGS, "from_mode").toString();
        const QString toScheme = cmdParser->getCmdValue(CMDArg::SET_WIN_SCHEME_COPY_SETTINGS, "to_scheme").toString().prepend('{').append('}');
        const QString toMode = cmdParser->getCmdValue(CMDArg::SET_WIN_SCHEME_COPY_SETTINGS, "to_mode").toString();
        const QList<QString> validFromModes = {"ac", "dc"};
        const QList<QString> validToModes = {"ac", "dc", "all"};
        const bool toAc = toMode == "ac";
        const bool toDc = toMode == "dc";
        const bool toAll = toMode == "all";
        const bool fromAc = fromMode == "ac";

        if (!packetData->schemes.contains(fromScheme)) {
            logger->write(QString("cannot copy settings from '%1': not found").arg(fromScheme));
            return;

        } else if (!packetData->schemes.contains(toScheme)) {
            logger->write(QString("cannot copy settings to '%1': not found").arg(toScheme));
            return;

        } else if (!validFromModes.contains(fromMode) || !validToModes.contains(toMode)) {
            logger->write(QString("invalid mode argument: from %1 to %2").arg(fromMode, toMode));
            return;
        }

        for (const auto &[settingGuid, setting]: packetData->schemes[fromScheme].settings.asKeyValueRange()) {
            if (!packetData->schemes[toScheme].settings.contains(settingGuid)) [[unlikely]] {
                logger->write(QString("cannot copy setting '%1' to scheme '%2': setting not found").arg(settingGuid, toScheme));
                continue;
            }

            PWTS::WIN::PowerSettingValue val = packetData->schemes[toScheme].settings[settingGuid].value;

            if (toAc || toAll) {
                if (fromAc)
                    val.ac = setting.value.ac;
                else
                    val.ac = setting.value.dc;
            }

            if (toDc || toAll) {
                if (fromAc)
                    val.dc = setting.value.ac;
                else
                    val.dc = setting.value.dc;
            }

            packetData->schemes[toScheme].settings[settingGuid].value = val;
        }
    }

    void CliHelperWindows::setPowerSchemes() const {
        if (!cmdParser->hasCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerSchemeArg) || packetData->schemes.isEmpty() || packetData->schemeOptionsData.isEmpty())
            return;

        const QHash<QString, QVariant> args = cmdParser->getCmdValue(CMDArg::SET_DEVICE_SETTINGS, powerSchemeArg).toHash();

        for (const auto &[arg, value]: args.asKeyValueRange()) {
            const QList<QString> settingData = arg.split(';');

            if (settingData.size() < 3) {
                logger->write(QString("%1: invalid argument: %2").arg(powerSchemeArg, arg));
                continue;
            }

            const QString setting = QString("{%1}").arg(settingData[1]);
            QString scheme = QString("{%1}").arg(settingData[0]);
            PWTS::WIN::PowerSettingValue settingVal;

            if (!packetData->schemes.contains(scheme)) { // maybe temp user handle
                scheme = QString("{guid-tbd-%1}").arg(settingData[0]);

                if (!packetData->schemes.contains(scheme)) {
                    logger->write(QString("%1: scheme not found: %2").arg(powerSchemeArg, scheme));
                    continue;
                }
            }

            if (!packetData->schemes[scheme].settings.contains(setting)) {
                logger->write(QString("%1: setting '%2' not found in scheme '%3'").arg(powerSchemeArg, setting, scheme));
                continue;

            } else if (settingData[2] != "ac" && settingData[2] != "dc") {
                logger->write(QString("%1: invalid mode for setting '%2' of scheme '%3'").arg(powerSchemeArg, settingData[2], setting, scheme));
                continue;
            }

            settingVal = packetData->schemes[scheme].settings[setting].value;

            if (settingData[2] == "ac")
                settingVal.ac = value.toInt();
            else
                settingVal.dc = value.toInt();

            packetData->schemes[scheme].settings[setting].value = settingVal;
        }
    }

    void CliHelperWindows::setPowerSchemeFlags() const {
        packetData->replaceDefaultSchemes = cmdParser->isSet(CMDArg::SET_WIN_SCHEMES_REPLACE_DEFAULTS);
        packetData->resetSchemesDefault = cmdParser->isSet(CMDArg::SET_WIN_SCHEMES_RESET_DEFAULTS);
    }

    void CliHelperWindows::setPowerSchemesDeleteFlag() const {
        if (!cmdParser->isSet(CMDArg::SET_WIN_DELETE_SCHEMES))
            return;

        const QList<QString> guidList = cmdParser->getCmdValue(CMDArg::SET_WIN_DELETE_SCHEMES, "guids").toStringList();

        for (const QString &guid: guidList) {
            const QString toFlag = QString("{%1}").arg(guid);

            if (packetData->schemes.contains(toFlag))
                packetData->schemes[toFlag].deleteFlag = true;
        }
    }

    void CliHelperWindows::setPowerSchemesResetFlag() const {
        if (!cmdParser->isSet(CMDArg::SET_WIN_RESET_SCHEMES))
            return;

        const QList<QString> guidList = cmdParser->getCmdValue(CMDArg::SET_WIN_RESET_SCHEMES, "guids").toStringList();

        for (const QString &guid: guidList) {
            const QString toFlag = QString("{%1}").arg(guid);

            if (packetData->schemes.contains(toFlag))
                packetData->schemes[toFlag].resetFlag = true;
        }
    }

    void CliHelperWindows::setActivePowerScheme() const {
        if (!cmdParser->isSet(CMDArg::SET_WIN_ACTIVE_SCHEME))
            return;

        const QString active = cmdParser->getCmdValue(CMDArg::SET_WIN_ACTIVE_SCHEME, "guid").toString();
        QString activeGuid = QString("{%1}").arg(active);

        if (!packetData->schemes.contains(activeGuid)) // maybe temp user handle
            activeGuid = QString("{guid-tbd-%1}").arg(active);

        if (packetData->schemes.contains(activeGuid) && !packetData->schemes[activeGuid].deleteFlag)
            packetData->activeScheme = activeGuid;
        else
            logger->write(QString("cannot set active scheme '%1': not found or flagged for deletion").arg(activeGuid));
    }

    void CliHelperWindows::setClientPacketData() {
    	if (features.cpu.contains(PWTS::Feature::PWR_SCHEME_GROUP)) {
    	    duplicatePowerScheme();
    	    copyPowerSchemeSettings();
    	    setPowerSchemes();
    	    setPowerSchemeFlags();
    	    setPowerSchemesDeleteFlag();
    	    setPowerSchemesResetFlag();
    	    setActivePowerScheme();
    	}

        // delete ro package data
        packetData->schemeOptionsData = {};
    }
}
