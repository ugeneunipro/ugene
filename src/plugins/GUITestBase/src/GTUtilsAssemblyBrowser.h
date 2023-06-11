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

#pragma once

#include <GTGlobals.h>

#include <QModelIndex>
#include <QScrollBar>

namespace U2 {

class AssemblyBrowserUi;
class AssemblyModel;

class GTUtilsAssemblyBrowser {
public:
    enum Area {
        Consensus,
        Overview,
        Reads
    };

    enum Method {
        Button,
        Hotkey
    };

    /** Returns opened assembly browser window. Fails if not found. */
    static QWidget* getActiveAssemblyBrowserWindow();

    /** Checks that assembly browser view is opened and is active and fails if not. */
    static void checkAssemblyBrowserWindowIsActive();

    static AssemblyBrowserUi* getView(const QString& viewTitle = "");

    static void addRefFromProject(QString docName, QModelIndex parent = QModelIndex());

    static bool hasReference(const QString& viewTitle);
    static bool hasReference(QWidget* view = nullptr);
    static bool hasReference(AssemblyBrowserUi* assemblyBrowser);

    static qint64 getLength();
    static qint64 getReadsCount();

    static bool isWelcomeScreenVisible();

    static void zoomIn(Method method = Button);
    static void zoomToMax();
    static void zoomToMin();
    static void zoomToReads();

    static void goToPosition(qint64 position, Method method = Hotkey);

    static void callContextMenu(Area area = Consensus);
    static void callExportCoverageDialog(Area area = Consensus);

    static QScrollBar* getScrollBar(Qt::Orientation orientation);

    static void scrollToStart(Qt::Orientation orientation);
};

}  // namespace U2
