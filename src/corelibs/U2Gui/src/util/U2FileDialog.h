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

#ifndef _U2_FILE_DIALOG_H_
#define _U2_FILE_DIALOG_H_

#include <QFileDialog>

#include <U2Core/global.h>

namespace U2 {

/** A workaround for the issue: UGENE-3443 (part 1)
  * https://bugreports.qt-project.org/browse/QTBUG-25295
  * This class should be removed after switching to Qt 5.1 or newer
  */
class U2GUI_EXPORT U2FileDialog {
public:
    static QString getOpenFileName(QWidget * parent = 0,
                                   const QString & caption = QString(),
                                   const QString & dir = QString(),
                                   const QString & filter = QString(),
                                   QString * selectedFilter = 0,
                                   QFileDialog::Options options = 0);

    static QStringList getOpenFileNames(QWidget * parent = 0,
                                        const QString & caption = QString(),
                                        const QString & dir = QString(),
                                        const QString & filter = QString(),
                                        QString * selectedFilter = 0,
                                        QFileDialog::Options options = 0);

    static QString getExistingDirectory(QWidget * parent = 0,
                                        const QString & caption = QString(),
                                        const QString & dir = QString(),
                                        QFileDialog::Options options = QFileDialog::ShowDirsOnly);

    static QString getSaveFileName(QWidget * parent = 0,
                                   const QString & caption = QString(),
                                   const QString & dir = QString(),
                                   const QString & filter = QString(),
                                   QString * selectedFilter = 0,
                                   QFileDialog::Options options = 0);

private:
    static void activateWindow();
};

}   // namespace U2

#endif // _U2_FILE_DIALOG_H_
