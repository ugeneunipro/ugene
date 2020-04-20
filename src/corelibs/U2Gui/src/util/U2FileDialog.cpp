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
#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "U2FileDialog.h"

namespace U2 {

QString U2FileDialog::getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options) {
    QString name;
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        name = QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options | QFileDialog::DontUseNativeDialog);
    } else {
        name = QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
    }
    const QString result = name;
    activateWindow();
    return result;
}

QStringList U2FileDialog::getOpenFileNames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options) {
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        options = options | QFileDialog::DontUseNativeDialog;
    }
    const QStringList result = QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
    activateWindow();
    return result;
}

QString U2FileDialog::getExistingDirectory(QWidget *parent, const QString &caption, const QString &dir, QFileDialog::Options options) {
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        options = options | QFileDialog::DontUseNativeDialog;
    }
    const QString result = QFileDialog::getExistingDirectory(parent, caption, dir, options);
    activateWindow();
    return result;
}

QString U2FileDialog::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options) {
    QString name;
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        name = QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options | QFileDialog::DontUseNativeDialog);
    } else {
        name = QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
    }
    const QString result = name;
    activateWindow();
    return result;
}

void U2FileDialog::activateWindow() {
#ifdef Q_OS_MAC
    QWidget *target = QApplication::activeModalWidget();
    if (NULL == target) {
        MainWindow *mainWindow = AppContext::getMainWindow();
        CHECK(NULL != mainWindow, );
        QMainWindow *qMainWindow = mainWindow->getQMainWindow();
        target = qobject_cast<QWidget*>(qMainWindow);
    }
    CHECK(NULL != target, );
    target->activateWindow();
#endif
}


}   // namespace U2
