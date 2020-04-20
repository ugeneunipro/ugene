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

#ifndef _U2_EXPORT_COVERAGE_DIALOG_FILLER_H_
#define _U2_EXPORT_COVERAGE_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class ExportCoverageDialogFiller : public Filler {
public:
    enum ActionType {                   // An appropriate action data
        EnterFilePath,                  // QString with a path (do not forget about native separators)
        SelectFile,                     // QString with a path
        SetFormat,                      // QString with a format name
        SetCompress,                    // bool with a new checkbox state
        SetExportCoverage,              // bool with a new checkbox state
        SetExportBasesQuantity,         // bool with a new checkbox state
        SetThreshold,                   // int with a new threshold value
        CheckFilePath,                  // QString with an expected path (do not forget about native separators)
        CheckFormat,                    // QString with an expected format name
        CheckCompress,                  // bool with an expected checkbox state
        CheckOptionsVisibility,         // bool with an expected groupbox visibility state
        CheckExportCoverage,            // bool with an expected checkbox state
        CheckExportBasesQuantity,       // bool with an expected checkbox state
        CheckThreshold,                 // int with an expected spinbox value
        CheckThresholdBounds,           // QPoint where x is a minimum value and y is a maximum value
        ExpectMessageBox,               // ignored
        ClickOk,                        // ignored
        ClickCancel                     // ignored
    };

    typedef QPair<ActionType, QVariant> Action;

    ExportCoverageDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions);

    void commonScenario();

private:
    void enterFilePath(const QVariant &actionData);
    void selectFile(const QVariant &actionData);
    void setFormat(const QVariant &actionData);
    void setCompress(const QVariant &actionData);
    void setExportCoverage(const QVariant &actionData);
    void setExportBasesQuantity(const QVariant &actionData);
    void setThreshold(const QVariant &actionData);
    void checkFilePath(const QVariant &actionData);
    void checkFormat(const QVariant &actionData);
    void checkCompress(const QVariant &actionData);
    void checkOptionsVisibility(const QVariant &actionData);
    void checkExportCoverage(const QVariant &actionData);
    void checkExportBasesQuantity(const QVariant &actionData);
    void checkThreshold(const QVariant &actionData);
    void checkThresholdBounds(const QVariant &actionData);
    void expectMessageBox();
    void clickOk();
    void clickCancel();

    QWidget *dialog;
    const QList<Action> actions;
};

}   // namespace U2

#endif // _U2_EXPORT_COVERAGE_DIALOG_FILLER_H_
