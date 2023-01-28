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

namespace U2 {
class GTUtilsOptionPanelMca {
public:
    enum Tabs {
        General,
        Consensus,
        Reads
    };

    enum FileFormat {
        FASTA,
        GenBank,
        PlainText
    };

    static const QMap<Tabs, QString> tabsNames;
    static const QMap<Tabs, QString> innerWidgetNames;

    static void toggleTab(HI::GUITestOpStatus& os, Tabs tab, QWidget* parent = nullptr);
    static void openTab(HI::GUITestOpStatus& os, Tabs tab, QWidget* parent = nullptr);
    static void closeTab(HI::GUITestOpStatus& os, Tabs tab);
    static bool isTabOpened(HI::GUITestOpStatus& os, Tabs tab, QWidget* parent = nullptr);

    static void setConsensusType(HI::GUITestOpStatus& os, const QString& consensusTypeName);
    static QString getConsensusType(HI::GUITestOpStatus& os);
    static QStringList getConsensusTypes(HI::GUITestOpStatus& os);

    static int getHeight(HI::GUITestOpStatus& os);
    static int getLength(HI::GUITestOpStatus& os);

    static void setThreshold(HI::GUITestOpStatus& os, int threshold);
    static int getThreshold(HI::GUITestOpStatus& os);

    static void setExportFileName(HI::GUITestOpStatus& os, QString exportFileName);
    static QString getExportFileName(HI::GUITestOpStatus& os);

    static void setFileFormat(HI::GUITestOpStatus& os, FileFormat fileFormat);

    static void pushResetButton(HI::GUITestOpStatus& os);
    static void pushExportButton(HI::GUITestOpStatus& os);

    /*
     * Open the "reads" tab and set up the "Alternative mutations" widget
     * @os operation status
     * @show check the "Show alternative mutations" checkbox if true, otherwise uncheck
     * @value persentage value, default - 75
     * @withSpinbox set value with the spin box if true, otherwise with slider
     **/
    static void showAlternativeMutations(HI::GUITestOpStatus& os, bool show, int value = 75, bool withSpinbox = true, QWidget* parent = nullptr);
};
}  // namespace U2
