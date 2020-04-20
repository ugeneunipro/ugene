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

#ifndef _U2_GT_UTILS_ASSEMBLY_BROWSER_H_
#define _U2_GT_UTILS_ASSEMBLY_BROWSER_H_

#include <QModelIndex>
#include <GTGlobals.h>

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

    static AssemblyBrowserUi *getView(HI::GUITestOpStatus &os, const QString& viewTitle = "");

    static void addRefFromProject(HI::GUITestOpStatus &os, QString docName, QModelIndex parent = QModelIndex());

    static bool hasReference(HI::GUITestOpStatus &os, const QString &viewTitle);
    static bool hasReference(HI::GUITestOpStatus &os, QWidget *view = NULL);
    static bool hasReference(HI::GUITestOpStatus &os, AssemblyBrowserUi *assemblyBrowser);

    static qint64 getLength(HI::GUITestOpStatus &os);
    static qint64 getReadsCount(HI::GUITestOpStatus &os);

    static bool isWelcomeScreenVisible(HI::GUITestOpStatus &os);

    static void zoomIn(HI::GUITestOpStatus& os, Method method = Button);
    static void zoomToMax(HI::GUITestOpStatus& os);
    static void zoomToMin(HI::GUITestOpStatus& os);
    static void zoomToReads(HI::GUITestOpStatus& os);

    static void goToPosition(HI::GUITestOpStatus &os, qint64 position, Method method = Hotkey);

    static void callContextMenu(HI::GUITestOpStatus &os, Area area = Consensus);
    static void callExportCoverageDialog(HI::GUITestOpStatus &os, Area area = Consensus);

    static QScrollBar* getScrollBar(HI::GUITestOpStatus &os, Qt::Orientation orientation);
};

}   // namespace U2

#endif // _U2_GT_UTILS_ASSEMBLY_BROWSER_H_
