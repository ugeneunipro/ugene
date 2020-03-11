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

#ifndef _U2_GT_UTILS_OPTION_PANEL_MSA_H_
#define _U2_GT_UTILS_OPTION_PANEL_MSA_H_

#include <GTGlobals.h>

class QLineEdit;
class QPushButton;
class QToolButton;

namespace U2 {

class GTUtilsOptionPanelMsa {
public:
    enum Tabs {
        General,
        Highlighting,
        PairwiseAlignment,
        TreeSettings,
        ExportConsensus,
        Statistics,
        Search
    };

    enum AddRefMethod {
        Button,
        Completer
    };

    enum ThresholdComparison {
        LessOrEqual,
        GreaterOrEqual
    };

    enum class CopyFormat {
        Fasta,
        CLUSTALW,
        Stocholm,
        MSF,
        NEXUS,
        Mega,
        PHYLIP_Interleaved,
        PHYLIP_Sequential,
        Rich_text
    };

    static const QMap<Tabs, QString> tabsNames;
    static const QMap<Tabs, QString> innerWidgetNames;

    static void toggleTab(HI::GUITestOpStatus &os, Tabs tab);
    static void openTab(HI::GUITestOpStatus &os, Tabs tab);
    static void closeTab(HI::GUITestOpStatus &os, Tabs tab);
    static bool isTabOpened(HI::GUITestOpStatus &os, Tabs tab);

    static void addReference(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method = Button);
    static void addFirstSeqToPA(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method = Button);
    static void addSecondSeqToPA(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method = Button);
    static QString getSeqFromPAlineEdit(HI::GUITestOpStatus &os, int num);
    static void removeReference(HI::GUITestOpStatus &os);
    static QString getReference(HI::GUITestOpStatus &os);
    static int getLength(HI::GUITestOpStatus &os);
    static int getHeight(HI::GUITestOpStatus &os);
    static void copySelection(HI::GUITestOpStatus &os, const CopyFormat format = CopyFormat::CLUSTALW);

    static void setColorScheme(HI::GUITestOpStatus &os, const QString &colorSchemeName, GTGlobals::UseMethod method = GTGlobals::UseKeyBoard);
    static QString getColorScheme(HI::GUITestOpStatus &os);

    static void setHighlightingScheme(HI::GUITestOpStatus &os, const QString &highlightingSchemeName);

    // functions for accessing PA gui elements
    static QToolButton* getAddButton(HI::GUITestOpStatus &os, int number);
    static QLineEdit* getSeqLineEdit(HI::GUITestOpStatus &os, int number);
    static QToolButton* getDeleteButton(HI::GUITestOpStatus &os, int number);
    static QPushButton *getAlignButton(HI::GUITestOpStatus &os);
    static void setPairwiseAlignmentAlgorithm(HI::GUITestOpStatus &os, const QString &algorithm);

    // functions for accessing Highlighting schemes options elements
    static void setThreshold(HI::GUITestOpStatus &os, int threshold);
    static int getThreshold(HI::GUITestOpStatus &os);

    static void setThresholdComparison(HI::GUITestOpStatus &os, ThresholdComparison comparison);
    static ThresholdComparison getThresholdComparison(HI::GUITestOpStatus &os);

    static void setUseDotsOption(HI::GUITestOpStatus &os, bool useDots);
    static bool isUseDotsOptionSet(HI::GUITestOpStatus &os);

    // functions for accessing "Export consensus" options elements
    static void setExportConsensusOutputPath(HI::GUITestOpStatus &os, const QString &filePath);
    static QString getExportConsensusOutputPath(HI::GUITestOpStatus &os);

    static void setExportConsensusOutputFormat(HI::GUITestOpStatus &os, const QString &format);
    static QString getExportConsensusOutputFormat(HI::GUITestOpStatus &os);

    // functions for accessing "Find pattern" options elements
    static void enterPattern(HI::GUITestOpStatus &os, QString pattern, bool useCopyPaste = false);
    static QString getPattern(HI::GUITestOpStatus &os);
    static void setAlgorithm(HI::GUITestOpStatus &os, QString algorithm);
    static void setMatchPercentage(HI::GUITestOpStatus &os, int percentage);
    static void setCheckedRemoveOverlappedResults(HI::GUITestOpStatus &os, bool checkedState = true);
    static bool checkResultsText(HI::GUITestOpStatus &os, QString expectedText);

    static void clickNext(HI::GUITestOpStatus &os);
    static void clickPrev(HI::GUITestOpStatus &os);

private:
    static QWidget* getWidget(HI::GUITestOpStatus &os, const QString& widgetName, int number);

    static void addSeqToPA(HI::GUITestOpStatus &os, QString seqName, AddRefMethod method, int number);

    static QMap<Tabs, QString> initNames();
    static QMap<Tabs, QString> initInnerWidgetNames();
};

}

#endif // _U2_GT_UTILS_OPTION_PANEL_MSA_H_
