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

namespace PWT::CLI {
    enum struct CMDArg: int {
        DAEMON,

        GET_MODE,
        GET_DAEMON_LIST,
        GET_DATA_PATH,
        GET_DAEMON_SETTINGS,
        GET_DEVICE_INFO,
        GET_PROFILE_LIST,
        GET_EXPORT_PROFILES,
        GET_DEVICE_DATA,

        SET_MODE,
        SET_RESET_CLI_SETTINGS,
        SET_ADD_DAEMONS,
        SET_REMOVE_DAEMONS,
        SET_DAEMON_SETTINGS,
        SET_DELETE_PROFILE,
        SET_APPLY_PROFILE,
        SET_IMPORT_PROFILES,
        SET_MAKE_PROFILE,
        SET_DEVICE_SETTINGS,
        SET_WIN_SCHEMES_RESET_DEFAULTS,
        SET_WIN_SCHEMES_REPLACE_DEFAULTS,
        SET_WIN_ACTIVE_SCHEME,
        SET_WIN_DELETE_SCHEMES,
        SET_WIN_RESET_SCHEMES,
        SET_WIN_SCHEME_COPY_SETTINGS,
        SET_WIN_DUPLICATE_SCHEME
    };
}
