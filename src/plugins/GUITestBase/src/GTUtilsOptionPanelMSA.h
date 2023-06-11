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
        AddTree,
        TreeOptions,
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

    static void toggleTab(Tabs tab);
    static QWidget* openTab(Tabs tab);
    static void closeTab(Tabs tab);
    static bool isTabOpened(Tabs tab);

    /** Checks that the tab is opened and returns the inner tab widget .*/
    static QWidget* checkTabIsOpened(Tabs tab);

    static void addReference(const QString& seqName, AddRefMethod method = Button);
    static void addFirstSeqToPA(const QString& seqName, AddRefMethod method = Button);
    static void addSecondSeqToPA(const QString& seqName, AddRefMethod method = Button);
    static QString getSeqFromPAlineEdit(int num);
    static void removeReference();
    static QString getReference();
    static int getLength();
    static int getHeight();
    static void copySelection(const CopyFormat& format = CopyFormat::CLUSTALW);

    static void setColorScheme(const QString& colorSchemeName, GTGlobals::UseMethod method = GTGlobals::UseKeyBoard);
    static QString getColorScheme();

    static void setHighlightingScheme(const QString& highlightingSchemeName);

    // functions for accessing PA gui elements
    static QToolButton* getAddButton(int number);
    static QLineEdit* getSeqLineEdit(int number);
    static QToolButton* getDeleteButton(int number);
    static QPushButton* getAlignButton();
    static void setPairwiseAlignmentAlgorithm(const QString& algorithm);

    // functions for accessing Highlighting schemes options elements
    static void setThreshold(int threshold);
    static int getThreshold();

    static void setThresholdComparison(ThresholdComparison comparison);
    static ThresholdComparison getThresholdComparison();

    static void setUseDotsOption(bool useDots);
    static bool isUseDotsOptionSet();

    // functions for accessing "Export consensus" options elements
    static void setExportConsensusOutputPath(const QString& filePath);
    static QString getExportConsensusOutputPath();

    static QString getExportConsensusOutputFormat();

    // functions for accessing "Find pattern" options elements
    static void enterPattern(const QString& pattern, bool useCopyPaste = false);
    static QString getPattern();
    static void setAlgorithm(const QString& algorithm);
    static void setMatchPercentage(int percentage);
    static void setCheckedRemoveOverlappedResults(bool checkedState = true);
    static void checkResultsText(const QString& expectedText);
    static void setRegionType(const QString& regionType);
    static void setRegion(int from, int to);
    static void setSearchContext(const QString& context);

    static void clickNext();
    static void clickPrev();

    static bool isSearchInShowHideWidgetOpened();
    static void openSearchInShowHideWidget(bool open = true);

    /** Returns text of the label that shows alignment alphabet. Requires "General" options panel to be opened. */
    static QString getAlphabetLabelText();

    /** Sets output file path. */
    static void setOutputFile(const QString& outputFilePath);

private:
    static QWidget* getWidget(const QString& widgetName, int number);

    static void addSeqToPA(const QString& seqName, AddRefMethod method, int number);

    static QMap<Tabs, QString> initNames();
    static QMap<Tabs, QString> initInnerWidgetNames();
};

}  // namespace U2
