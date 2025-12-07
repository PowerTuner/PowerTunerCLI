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
#include <QDateTime>
#include <QFileInfo>

#include "FileLogger.h"
#include "../Utils.h"

namespace PWT::CLI {
    FileLogger::~FileLogger() {
        logFile.close();
    }

    QSharedPointer<FileLogger> FileLogger::getInstance() {
        if (instance.isNull())
            instance.reset(new FileLogger);

        return instance;
    }

    void FileLogger::init(const QString &logPath) {
        if (logPath.isEmpty() || logFile.isOpen())
            return;

        logFile.setFileName(QString("%1/pwtcli.log").arg(logPath));
        ts.setDevice(&logFile);

        if (!logFile.open(QFile::WriteOnly | QFile::Text))
            printError(QString("failed to init log file %1: %2").arg(logFile.fileName(), logFile.errorString()));
    }

    void FileLogger::write(const QString &msg, const std::source_location source) {
        if (!logFile.isOpen())
            return;

        if (logFile.size() >= limit)
            logFile.resize(0);

        const QFileInfo srcFInfo {source.file_name()};

        ts << QDateTime::currentDateTime().toString("[ddd MMMM d yyyy hh:mm:ss]") << "\n" <<
            srcFInfo.fileName() << "[" << source.line() << ":" << source.column() << "]: " << source.function_name() << "\n" <<
            msg << "\n\n";

        ts.flush();

        if (ts.status() != QTextStream::Ok) {
            printError("failed to write log, log will be disabled");
            logFile.close();
        }
    }
}
