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

#ifndef _U2_GT_UTILS_OPTION_PANEL_SEQUENCE_VIEW_H_
#define _U2_GT_UTILS_OPTION_PANEL_SEQUENCE_VIEW_H_

#include <QToolButton>
#include <QLineEdit>

#include "GTGlobals.h"

namespace U2 {

class GTUtilsOptionPanelSequenceView{
public:
    enum Tabs {
        Search,
        AnnotationsHighlighting,
        Statistics,
        InSilicoPcr,
        CircularView
    };

    enum AddRefMethod {
        Button,
        Completer
    };

    static const QMap<Tabs,QString> tabsNames;
    static const QMap<Tabs, QString> innerWidgetNames;

    static void toggleTab(HI::GUITestOpStatus &os, Tabs tab);
    static void openTab(HI::GUITestOpStatus &os, Tabs tab);
    static void closeTab(HI::GUITestOpStatus &os, Tabs tab);
    static bool isTabOpened(HI::GUITestOpStatus &os, Tabs tab);

    // Find pattern options panel tab
    static void enterPattern(HI::GUITestOpStatus &os, QString pattern, bool useCopyPaste = false);
    static void enterPatternFromFile(HI::GUITestOpStatus &os, QString filepath, QString filename);

    static bool checkResultsText(HI::GUITestOpStatus &os, QString expectedText);

    static void setSearchWithAmbiguousBases(HI::GUITestOpStatus &os, bool searchWithAmbiguousBases = true);
    static void setStrand(HI::GUITestOpStatus &os, QString strandStr);
    static void setRegionType(HI::GUITestOpStatus &os, const QString &regionType);
    static void setRegion(HI::GUITestOpStatus &os, int from, int to);
    static void setSearchInTranslation(HI::GUITestOpStatus &os, bool inTranslation = true);
    static void setSearchInLocation(HI::GUITestOpStatus &os, QString strandStr);
    static void setSetMaxResults(HI::GUITestOpStatus &os, int maxResults);
    static void setAlgorithm(HI::GUITestOpStatus &os, QString algorithm);
    static void setMatchPercentage(HI::GUITestOpStatus &os, int percentage);
    static void setUsePatternName(HI::GUITestOpStatus &os, bool setChecked = true);

    static int getMatchPercentage(HI::GUITestOpStatus &os);
    static QString getRegionType(HI::GUITestOpStatus &os);
    static QPair<int, int> getRegion(HI::GUITestOpStatus &os);
    static const QString getHintText(HI::GUITestOpStatus &os);

    static void enterFilepathForSavingAnnotations(HI::GUITestOpStatus &os, QString filepath);

    static void toggleInputFromFilePattern(HI::GUITestOpStatus &os);

    static void toggleSaveAnnotationsTo(HI::GUITestOpStatus &os);

    static void clickNext(HI::GUITestOpStatus &os);
    static void clickPrev(HI::GUITestOpStatus &os);
    static void clickGetAnnotation(HI::GUITestOpStatus &os);
    static bool isPrevNextEnabled(HI::GUITestOpStatus &os);
    static bool isGetAnnotationsEnabled(HI::GUITestOpStatus &os);

    // Circular view options panel tab
    static void toggleCircularView(HI::GUITestOpStatus &os);
    static void setTitleFontSize(HI::GUITestOpStatus &os, int fontSize);
    static int getTitleFontSize(HI::GUITestOpStatus &os);

    // In Silico PCR tab
    static void setForwardPrimer(HI::GUITestOpStatus &os, const QString &primer);
    static void setForwardPrimerMismatches(HI::GUITestOpStatus &os, const int mismatches);
    static void setReversePrimer(HI::GUITestOpStatus &os, const QString &primer);
    static void setReversePrimerMismatches(HI::GUITestOpStatus &os, const int mismatches);
    static void showPrimersDetails(HI::GUITestOpStatus &os);
    static int productsCount(HI::GUITestOpStatus &os);
    static void pressFindProducts(HI::GUITestOpStatus &os);
    static void pressExtractProduct(HI::GUITestOpStatus &os);

    // Utility
    static bool isSearchAlgorithmShowHideWidgetOpened(HI::GUITestOpStatus &os);
    static bool isSearchInShowHideWidgetOpened(HI::GUITestOpStatus &os);
    static bool isOtherSettingsShowHideWidgetOpened(HI::GUITestOpStatus &os);
    static bool isSaveAnnotationToShowHideWidgetOpened(HI::GUITestOpStatus &os);
    static bool isAnnotationParametersShowHideWidgetOpened(HI::GUITestOpStatus &os);

    static void openSearchAlgorithmShowHideWidget(HI::GUITestOpStatus &os, bool open = true);
    static void openSearchInShowHideWidget(HI::GUITestOpStatus &os, bool open = true);
    static void openOtherSettingsShowHideWidget(HI::GUITestOpStatus &os, bool open = true);
    static void openSaveAnnotationToShowHideWidget(HI::GUITestOpStatus &os, bool open = true);
    static void openAnnotationParametersShowHideWidget(HI::GUITestOpStatus &os, bool open = true);

private:
    static QMap<Tabs, QString> initNames();
    static QMap<Tabs, QString> initInnerWidgetNames();
};

}   // namespace U2

#endif // _U2_GT_UTILS_OPTION_PANEL_SEQUENCE_VIEW_H_
