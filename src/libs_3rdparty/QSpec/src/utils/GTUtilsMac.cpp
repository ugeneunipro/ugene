/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "GTUtilsMac.h"

#include <QProcess>

namespace HI {

#define GT_CLASS_NAME "GTUtilsMac"

#define GT_METHOD_NAME "workaroundForMacCGEvents"

GTUtilsMac::GTUtilsMac() {
}

GTUtilsMac::~GTUtilsMac() {
#ifdef Q_OS_DARWIN
    if (process != nullptr) {
        process->kill();
        delete process;
    }
#endif
}

void GTUtilsMac::startWorkaroundForMacCGEvents(int delay, bool waitFinished) {
#ifdef Q_OS_DARWIN
    QString prog = qgetenv("UGENE_GUI_TEST_MACOS_WORKAROUND_FOR_CGEVENTS");

    if (!prog.isNull()) {
        if (process != nullptr) {
            process->kill();
            delete process;
        }
        process = new QProcess();
        process->start(prog, {"-x", "1000", "-y", "0", "-w", "80", "-h", "40", "-d", QString::number(delay), "-t", "40"});
        if (waitFinished) {
            bool finished = process->waitForFinished();
            if (!finished) {
                process->kill();
            }
            delete process;
            process = nullptr;
        }
    }
#else
    Q_UNUSED(delay);
    Q_UNUSED(waitFinished);
#endif
}

#undef GT_METHOD_NAME

}  // namespace HI
