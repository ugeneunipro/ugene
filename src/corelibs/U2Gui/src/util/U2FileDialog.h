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

#ifndef _U2_FILE_DIALOG_H_
#define _U2_FILE_DIALOG_H_

#include <QFileDialog>

#include <U2Core/global.h>

namespace U2 {

class U2GUI_EXPORT U2FileDialog {
public:
    static QString getOpenFileName(QWidget* parent = nullptr,
                                   const QString& caption = QString(),
                                   const QString& dir = QString(),
                                   const QString& filter = QString(),
                                   const QString& selectedFilter = QString(),
                                   const QFileDialog::Options& options = nullptr);

    static QStringList getOpenFileNames(QWidget* parent = nullptr,
                                        const QString& caption = QString(),
                                        const QString& dir = QString(),
                                        const QString& filter = QString(),
                                        const QString& selectedFilter = QString(),
                                        const QFileDialog::Options& options = nullptr);

    static QString getExistingDirectory(QWidget* parent = nullptr,
                                        const QString& caption = QString(),
                                        const QString& dir = QString(),
                                        const QFileDialog::Options& options = QFileDialog::ShowDirsOnly);

    static QString getSaveFileName(QWidget* parent = nullptr,
                                   const QString& caption = QString(),
                                   const QString& dir = QString(),
                                   const QString& filter = QString(),
                                   const QString& selectedFilter = QString(),
                                   const QFileDialog::Options& options = nullptr);
};

}  // namespace U2

#endif  // _U2_FILE_DIALOG_H_
