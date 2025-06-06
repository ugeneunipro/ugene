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

#include <GTGlobals.h>

#include <QAbstractItemModel>

#include <U2Gui/ProjectTreeController.h>

namespace U2 {
using namespace HI;

class GTUtilsCloudStorageView {
public:
    static void toggleDockView();

    static void clickLogin();

    static void clickLogout();

    static QModelIndex checkItemIsPresent(const QList<QString>& path);

    static QModelIndex checkItemIsShared(const QList<QString>& path, const QString& email);

    static QModelIndex checkItemIsNotShared(const QList<QString>& path, const QString& email);

    static void checkItemIsNotPresent(const QList<QString>& path);

    static void renameItem(const QList<QString>& path, const QString& newName);

    static void createDir(const QList<QString>& path);

    static void deleteEntry(const QList<QString>& path);

    static void uploadFile(const QList<QString>& dirPath, const QString& localFileUrl);

    static void downloadFileWithDoubleClick(const QList<QString>& dirPath, int expectedFileSize);

    static void shareItem(const QList<QString>& path, const QString& email);

    static void unshareItem(const QList<QString>& path, const QString& email);

    static QTreeView* getStorageTreeView();
};

}  // namespace U2
