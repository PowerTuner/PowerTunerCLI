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

#include <QVariant>

#include "CMDArg.h"

namespace PWT::CLI {
    class CMDParser final {
    private:
        // help text indent level
        static constexpr int helpIndentLv1 = 1;
        static constexpr int helpIndentLv2 = helpIndentLv1 + 2;
        static constexpr int helpIndentLv3 = helpIndentLv2 + 1;
        static constexpr int helpIndentLv4 = helpIndentLv3 + 2;
        static constexpr int helpIndentLv5 = helpIndentLv4 + 2;
        static constexpr int helpIndentLv6 = helpIndentLv5 + 2;

        // get
        static constexpr char getArg[] = "get";
        static constexpr char listDaemonsArg[] = "daemons";
        static constexpr char dataPathArg[] = "data-path";
        static constexpr char deviceInfoArg[] = "device-info";
        static constexpr char profilesArg[] = "profiles";
        static constexpr char exportProfilesArg[] = "export-profiles";
        static constexpr char deviceDataArg[] = "device-data";

        // set
        static constexpr char setArg[] = "set";
        static constexpr char resetCliSettArg[] = "reset-settings";
        static constexpr char addDaemonsArg[] = "add-daemons";
        static constexpr char removeDaemonsArg[] = "remove-daemons";
        static constexpr char deleteProfileArg[] = "delete-profile";
        static constexpr char applyProfileArg[] = "apply-profile";
        static constexpr char importProfilesArg[] = "import-profiles";
        static constexpr char deviceSettingsArg[] = "device-settings";
        static constexpr char makeProfileArg[] = "make-profile";

        // help
        static constexpr char helpLinuxSettingsArg[] = "linux_settings";
        static constexpr char helpWindowsSettingsArg[] = "windows_settings";
#ifdef WITH_AMD
        static constexpr char helpLinuxAMDSettingsArg[] = "linux_amd_settings";
        static constexpr char helpAMDCPUSettingsArg[] = "amd_cpu_settings";
#endif
#ifdef WITH_INTEL
        static constexpr char helpIntelCPUSettingsArg[] = "intel_cpu_settings";
#endif
        static constexpr char helpFanControlSettingsArg[] = "fan_control_settings";

        // set windows schemes options
        static constexpr char winPsResetDefaultsArg[] = "ps-reset-defaults";
        static constexpr char winPsReplaceDefaultsArg[] = "ps-replace-defaults";
        static constexpr char winPsActiveSchemeArg[] = "ps-active-scheme";
        static constexpr char winPsDeleteSchemesArg[] = "ps-delete-schemes";
        static constexpr char winPsResetSchemesArg[] = "ps-reset-schemes";
        static constexpr char winPsCopySchemeSettingsArg[] = "ps-copy-settings";
        static constexpr char winPsDuplicateSchemeArg[] = "ps-duplicate-scheme";

        // common
        static constexpr char daemonSettArg[] = "daemon-settings";

        // sub args
        static constexpr char daemonArg[] = "\"<daemon|address;port>\"";

        QHash<CMDArg, QHash<QString, QVariant>> argumentsMap;
        char **cmdArgv = nullptr;
        int cmdArgc = 0;

        void nextArg(int inc = 1);
        [[nodiscard]] QString helpIndent(int level) const;
        [[nodiscard]] bool isArg(const char *arg, const char *expected) const;
        [[nodiscard]] bool parseMode();
        [[nodiscard]] bool parseGetCommand();
        [[nodiscard]] bool parseSetCommand();
        [[nodiscard]] bool parseAdvHelpCommand() const;
        [[nodiscard]] bool parseDaemon();
        [[nodiscard]] bool parseAddDaemons();
        [[nodiscard]] bool parseRemoveDaemons();
        [[nodiscard]] bool parseSetDaemonSettings();
        [[nodiscard]] bool parseDeleteProfile();
        [[nodiscard]] bool parseApplyProfile();
        [[nodiscard]] bool parseExportProfiles();
        [[nodiscard]] bool parseImportProfiles();
        [[nodiscard]] bool parseMakeProfile();
        [[nodiscard]] bool parseWindowsActiveScheme();
        [[nodiscard]] bool parseWindowsDeleteSchemes();
        [[nodiscard]] bool parseWindowsResetSchemes();
        [[nodiscard]] bool parseWindowsCopySchemeSettings();
        [[nodiscard]] bool parseWindowsDuplicateScheme();
        [[nodiscard]] bool parseDeviceSettings();
        void showHelp() const;
        void showGetHelp() const;
        void showSetHelp() const;
        void showLinuxSettingsListHelp() const;
        void showWindowsSettingsListHelp() const;
#ifdef WITH_AMD
        void showLinuxAMDSettingsListHelp() const;
        void showAMDCPUSettingsListHelp() const;
#endif
#ifdef WITH_INTEL
        void showIntelCPUSettingsListHelp() const;
#endif
        void showFanControlSettingsListHelp() const;

    public:
        [[nodiscard]] bool isSet(const CMDArg arg) const { return argumentsMap.contains(arg); }
        [[nodiscard]] bool hasCmdValue(const CMDArg arg, const QString &value) const { return argumentsMap[arg].contains(value); }

        [[nodiscard]] bool parse(int argc, char *argv[]);
        [[nodiscard]] QVariant getCmdValue(CMDArg arg, const QString &value) const;
    };
}
