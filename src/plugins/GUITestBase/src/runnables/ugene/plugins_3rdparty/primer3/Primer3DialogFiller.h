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

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class Primer3DialogFiller : public Filler {
public:
    class Primer3Settings {
    public:
        Primer3Settings() {}

        // Result primers number (deprecated)
        int resultsCount = -1;
        // Result primers annotation group name
        QString primersGroupName;
        // Result primers name
        QString primersName;
        // Region to search for primers in
        int start = -1;
        int end = -1;
        // Set a checkbox "Pick righ primer"
        bool pickRight = true;
        // Set a checkbox "Pick left primer"
        bool pickLeft = true;
        // Set a checkbox "Pick internal primer"
        bool pickInternal = false;
        // If there are some validation errors expected set true
        bool hasValidationErrors = false;
        // Subsequence of the validateion error text
        QString validationErrorsText;
        // Continue calculation even if errors exists
        bool continueIfError = false;
        // Red color of this widgets will be checked
        QStringList errorWidgetsNames;
        // Set true if you need to enable "RT-PCR" design on the corresponding tab
        bool rtPcrDesign = false;
        // Value to exon range on the "RT-PCR" tab
        QString exonRangeLine;
        // The file to the primer3 settings. All settings from this file will be set with the dialog
        QString filePath;
        // Set settings manually if true, with the "Load settings" button if false
        bool loadManually = true;
        // Set true if you don't need to run, just set parameters and close the dialog
        bool notRun = false;
    };

    Primer3DialogFiller(HI::GUITestOpStatus& os, const Primer3Settings& settings = Primer3Settings());
    void commonScenario();

private:
    struct Widgets {
        QList<QPair<QSpinBox*, QString>> spin;
        QList<QPair<QCheckBox*, QString>> check;
        QList<QPair<QComboBox*, QString>> combo;
        QList<QPair<QDoubleSpinBox*, QString>> doubleSpin;
        QList<QPair<QLineEdit*, QString>> line;
        QPair<QPlainTextEdit*, QString> plainText;
    };

    // Fill the Primer3 dialog with settings, which are in the @settings.filePath manually
    // (the other way is to set they by using the "Load settings" button if @settings.loadManually is false)
    // This method parses the file from @settings.filePath liny-by-line,
    // distributes each parameter to its tab and then clicks each parameter.
    // @parent the parent widget
    void loadFromFileManually(QWidget* parent);
    QWidget* getWidgetTab(QWidget* wt) const;
    void findAllChildrenWithNames(QObject* obj, QMap<QString, QObject*>& children);


    Primer3Settings settings;
};

}  // namespace U2
