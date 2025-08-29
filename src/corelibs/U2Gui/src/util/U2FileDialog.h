/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

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
                                   const QFileDialog::Options& options = {0});

    static QStringList getOpenFileNames(QWidget* parent = nullptr,
                                        const QString& caption = QString(),
                                        const QString& dir = QString(),
                                        const QString& filter = QString(),
                                        const QString& selectedFilter = QString(),
                                        const QFileDialog::Options& options = {0});

    static QString getExistingDirectory(QWidget* parent = nullptr,
                                        const QString& caption = QString(),
                                        const QString& dir = QString(),
                                        const QFileDialog::Options& options = QFileDialog::ShowDirsOnly);

    static QString getSaveFileName(QWidget* parent = nullptr,
                                   const QString& caption = QString(),
                                   const QString& dir = QString(),
                                   const QString& filter = QString(),
                                   const QString& selectedFilter = QString(),
                                   const QFileDialog::Options& options = {0});
    /* Returns pair of filename and selected file filter in file dialog */
    static QPair<QString, QString> getFileNameAndSelectedFilter(QWidget* parent,
                                                         const QString& caption,
                                                         const QString& dir,
                                                         const QString& filter,
                                                         const QString& selectedFilter,
                                                         QFileDialog::Options options,
                                                         QFileDialog::AcceptMode acceptMode,
                                                         QFileDialog::FileMode fileMode);
};

}  // namespace U2
