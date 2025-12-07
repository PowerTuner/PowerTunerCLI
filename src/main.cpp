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

#include "PowerTunerCLI.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName("PowerTunerCLI");
    qRegisterMetaType<PWTS::ClientPacket>();
    qRegisterMetaType<PWTS::DaemonPacket>();

    QCoreApplication a(argc, argv);
    QScopedPointer<PWT::CLI::PowerTunerCLI> cli;

    cli.reset(new PWT::CLI::PowerTunerCLI());

    QObject::connect(cli.get(), &PWT::CLI::PowerTunerCLI::quit, &a, &QCoreApplication::exit, Qt::QueuedConnection);

    cli->run(argc, argv);
    return a.exec();
}
