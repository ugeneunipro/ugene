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

#include <base_dialogs/GTFileDialog.h>
#include <harness/UGUITestBase.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>

#include <U2Core/U2SafePoints.h>

#include "AlignToReferenceBlastDialogFiller.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::AlignToReferenceBlastDialogFiller"

AlignToReferenceBlastDialogFiller::AlignToReferenceBlastDialogFiller(const Settings& settings, HI::GUITestOpStatus& os)
    : Filler(os, "AlignToReferenceBlastDialog"), settings(settings) {
}

AlignToReferenceBlastDialogFiller::AlignToReferenceBlastDialogFiller(HI::GUITestOpStatus& os, CustomScenario* c)
    : Filler(os, "AlignToReferenceBlastDialog", c) {
}

#define GT_METHOD_NAME "commonScenario"
void AlignToReferenceBlastDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    setReference(os, settings.referenceUrl, dialog);
    CHECK_OP(os, );

    setReads(os, settings.readUrls, dialog);
    CHECK_OP(os, );

    auto settingSpinBox = GTWidget::findSpinBox(os, "minIdentitySpinBox", dialog);
    GTSpinBox::setValue(os, settingSpinBox, settings.minIdentity);

    settingSpinBox = GTWidget::findSpinBox(os, "qualitySpinBox", dialog);
    GTSpinBox::setValue(os, settingSpinBox, settings.qualityThreshold);

    auto checkBox = GTWidget::findCheckBox(os, "addToProjectCheckbox", dialog);
    GTCheckBox::setChecked(os, checkBox, settings.addResultToProject);

    setDestination(os, settings.outAlignment, dialog);
    CHECK_OP(os, );

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReference"
void AlignToReferenceBlastDialogFiller::setReference(GUITestOpStatus& os, const QString& referenceUrl, QWidget* dialog) {
    GTLineEdit::setText(os, "referenceLineEdit", referenceUrl, dialog);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReads"
void AlignToReferenceBlastDialogFiller::setReads(GUITestOpStatus& os, const QStringList& readUrls, QWidget* dialog) {
    auto addReadButton = GTWidget::findWidget(os, "addReadButton");
    auto readsListWidget = GTWidget::findListWidget(os, "readsListWidget", dialog);

    QStringList uniqueReads;
    for (const QString& read : qAsConst(readUrls)) {
        if (readsListWidget->findItems(read, Qt::MatchExactly).isEmpty()) {
            uniqueReads << read;
        }
    }
    GT_CHECK(!uniqueReads.isEmpty(), "List of unique reads is empty!");

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, uniqueReads));
    GTWidget::click(os, addReadButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDestination"
void AlignToReferenceBlastDialogFiller::setDestination(GUITestOpStatus& os, const QString& destinationUrl, QWidget* dialog) {
    GTLineEdit::setText(os, "outputLineEdit", destinationUrl, dialog);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
