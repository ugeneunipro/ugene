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

#ifndef _U2_GT_UTILS_OPTION_PANEL_MCA_H_
#define _U2_GT_UTILS_OPTION_PANEL_MCA_H_

#include <GTGlobals.h>

namespace U2 {
class GTUtilsOptionPanelMca {
public:
    enum Tabs {
        General,
        Consensus,
    };

    enum FileFormat {
        FASTA,
        GenBank,
        PlainText
    };

    static const QMap<Tabs, QString> tabsNames;
    static const QMap<Tabs, QString> innerWidgetNames;

    static void toggleTab(HI::GUITestOpStatus &os, Tabs tab);
    static void openTab(HI::GUITestOpStatus &os, Tabs tab);
    static void closeTab(HI::GUITestOpStatus &os, Tabs tab);
    static bool isTabOpened(HI::GUITestOpStatus &os, Tabs tab);

    static void setConsensusType(HI::GUITestOpStatus &os, const QString &consensusTypeName);
    static QString getConsensusType(HI::GUITestOpStatus &os);
    static QStringList getConsensusTypes(HI::GUITestOpStatus &os);

    static int getHeight(HI::GUITestOpStatus &os);
    static int getLength(HI::GUITestOpStatus &os);

    static void setThreshold(HI::GUITestOpStatus &os, int threshold);
    static int getThreshold(HI::GUITestOpStatus &os);

    static void setExportFileName(HI::GUITestOpStatus &os, QString exportFileName);
    static QString getExportFileName(HI::GUITestOpStatus &os);

    static void setFileFormat(HI::GUITestOpStatus &os, FileFormat fileFormat);

    static void pushResetButton(HI::GUITestOpStatus &os);
    static void pushExportButton(HI::GUITestOpStatus &os);

private:
    static QMap<Tabs, QString> initNames();
    static QMap<Tabs, QString> initInnerWidgetNames();
};
}

#endif // _U2_GT_UTILS_OPTION_PANEL_MCA_H_
