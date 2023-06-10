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

#include <QLineEdit>
#include <QToolButton>

#include "GTGlobals.h"

namespace U2 {

class GTUtilsOptionPanelSequenceView {
public:
    enum Tabs {
        Search,
        AnnotationsHighlighting,
        Statistics,
        InSilicoPcr,
        CircularView,
        GeneCut
    };

    enum AddRefMethod {
        Button,
        Completer
    };

    static const QMap<Tabs, QString> tabsNames;
    static const QMap<Tabs, QString> innerWidgetNames;

    static void toggleTab(Tabs tab);
    static void openTab(Tabs tab);
    static void closeTab(Tabs tab);
    static bool isTabOpened(Tabs tab);
    static void checkTabIsOpened(Tabs tab);

    // Find pattern options panel tab
    static void enterPattern(const QString& pattern, bool useCopyPaste = false);
    static void enterPatternFromFile(const QString& filepath, const QString& filename);

    static bool checkResultsText(const QString& expectedText);

    static void setSearchWithAmbiguousBases(bool searchWithAmbiguousBases = true);
    static void setStrand(const QString& strandStr);
    static void setRegionType(const QString& regionType);
    static void setRegion(int from, int to);
    static void setSearchInTranslation(bool inTranslation = true);
    static void setSearchInLocation(const QString& strandStr);
    static void setSetMaxResults(int maxResults);
    static void setAlgorithm(const QString& algorithm);
    static void setMatchPercentage(int percentage);
    static void setUsePatternName(bool setChecked = true);

    static int getMatchPercentage();
    static QString getRegionType();
    static QPair<int, int> getRegion();
    static QString getHintText();

    static void enterFilepathForSavingAnnotations(const QString& filepath);

    static void toggleInputFromFilePattern();

    static void toggleSaveAnnotationsTo();

    static void clickNext();
    static void clickPrev();
    static void clickGetAnnotation();
    static bool isPrevNextEnabled();
    static bool isGetAnnotationsEnabled();

    // Circular view options panel tab
    static void toggleCircularView();
    static void setTitleFontSize(int fontSize);
    static int getTitleFontSize();

    // In Silico PCR tab
    static void setForwardPrimer(const QString& primer);
    static void setForwardPrimerMismatches(int mismatches);
    static void setReversePrimer(const QString& primer);
    static void setReversePrimerMismatches(int mismatches);
    static void showPrimersDetails();
    static int productsCount();
    static void pressFindProducts();
    static void pressExtractProduct();

    // Utility
    static bool isSearchAlgorithmShowHideWidgetOpened();
    static bool isSearchInShowHideWidgetOpened();
    static bool isOtherSettingsShowHideWidgetOpened();
    static bool isSaveAnnotationToShowHideWidgetOpened();
    static bool isAnnotationParametersShowHideWidgetOpened();

    static void openSearchInShowHideWidget(bool open = true);
    static void openSaveAnnotationToShowHideWidget(bool open = true);
    static void openAnnotationParametersShowHideWidget(bool open = true);
    static void openInSilicoPcrMeltingTemperatureShowHideWidget();

    // Statistics
    static void showMeltingTemperatureDialog();

    /** Template (with %1) for melting temperature report string when the TM is available. */
    static const QString meltingTmReportString;

private:
    static QMap<Tabs, QString> initNames();
    static QMap<Tabs, QString> initInnerWidgetNames();
};

}  // namespace U2
