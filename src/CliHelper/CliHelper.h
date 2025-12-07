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

#include "../CMDParser/CMDParser.h"
#include "../Classes/FileLogger.h"
#include "pwtShared/Include/Features.h"

namespace PWT::CLI {
    class CliHelper {
    protected:
        QSharedPointer<CMDParser> cmdParser;
        QSharedPointer<FileLogger> logger;
        PWTS::Features features;

        void logInvalidValue(const QString &arg, const QString &val) const { logger->write(QString("%1: invalid value: %2").arg(arg, val)); }

    public:
        CliHelper(const QSharedPointer<CMDParser> &cmd, const PWTS::Features &daemonFeatures) {
            cmdParser = cmd;
            features = daemonFeatures;
            logger = FileLogger::getInstance();
        }

        virtual ~CliHelper() = default;

        virtual void setClientPacketData() = 0;
    };
}
