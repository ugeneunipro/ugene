/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTextStream>

#include "SendReportDialog.h"
#include "Utils.h"

#if defined(Q_OS_UNIX) && defined(Q_WS_X11)
#    include <X11/Xlib.h>
#endif

namespace {
QString loadReport(int argc, char *argv[]) {
    if (Utils::hasReportUrl()) {
        return Utils::loadReportFromUrl(Utils::getReportUrl());
    } else if (argc > 1) {
        return QString::fromUtf8(QByteArray::fromBase64(argv[argc - 1]));
    }

    return "";
}
}    // namespace

int main(int argc, char *argv[]) {
    bool useGui = true;
#if defined(Q_OS_UNIX) && defined(Q_WS_X11)
    useGui = (XOpenDisplay(NULL) != NULL);
#endif

    QApplication a(argc, argv, useGui);
    Q_UNUSED(a);

    // User lauches the program manually
    if (argc == 1) {
        if (useGui) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Information");
            msgBox.setText("Use \"ugeneui\" to start Unipro UGENE graphical interface \nor \"ugenecl\" to use the command-line interface.");
            msgBox.exec();
        } else {
            printf("Use \"ugeneui\" to start Unipro UGENE graphical interface or \"ugenecl\" to use the command-line interface.");
        }
        return 1;
    }

#ifdef Q_OS_MAC
    // A workaround to avoid using non-bundled plugins
    QCoreApplication::removeLibraryPath(QLibraryInfo::location(QLibraryInfo::PluginsPath));
    QCoreApplication::addLibraryPath("../PlugIns");
#endif

    const QString message = loadReport(argc, argv);
    const QString dumpUrl = Utils::getDumpUrl();
    bool silentSending = Utils::hasSilentModeFlag();

    if (silentSending) {
        ReportSender sender(true);
        sender.setFailedTest(Utils::getFailedTestName());
        sender.parse(message, dumpUrl);
        sender.send("", dumpUrl);
        return 0;
    }

    // The program is lanched by UGENE
    if (useGui) {
        SendReportDialog dlg(message, dumpUrl);
        dlg.setWindowIcon(QIcon(":ugenem/images/crash_icon.png"));
        dlg.exec();
    } else {
        QTextStream stream(stdin);
        printf("UGENE crashed. Would you like to send crash report to developer team? (y/n)\n");
        const QString str = stream.readLine();
        printf("\n%s", str.toUtf8().data());

        if (str == "y" || str == "Y") {
            ReportSender sender;
            sender.parse(message, dumpUrl);
            sender.send("", dumpUrl);
        }
    }

    return 0;
}
