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

AlignToReferenceBlastDialogFiller::AlignToReferenceBlastDialogFiller(const Settings& settings)
    : Filler("AlignToReferenceBlastDialog"), settings(settings) {
}

AlignToReferenceBlastDialogFiller::AlignToReferenceBlastDialogFiller(CustomScenario* c)
    : Filler("AlignToReferenceBlastDialog", c) {
}

#define GT_METHOD_NAME "commonScenario"
void AlignToReferenceBlastDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    setReference(settings.referenceUrl, dialog);

    setReads(settings.readUrls, dialog);

    auto settingSpinBox = GTWidget::findSpinBox("minIdentitySpinBox", dialog);
    GTSpinBox::setValue(settingSpinBox, settings.minIdentity);

    settingSpinBox = GTWidget::findSpinBox("qualitySpinBox", dialog);
    GTSpinBox::setValue(settingSpinBox, settings.qualityThreshold);

    auto checkBox = GTWidget::findCheckBox("addToProjectCheckbox", dialog);
    GTCheckBox::setChecked(checkBox, settings.addResultToProject);

    setDestination(settings.outAlignment, dialog);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReference"
void AlignToReferenceBlastDialogFiller::setReference(const QString& referenceUrl, QWidget* dialog) {
    GTLineEdit::setText("referenceLineEdit", referenceUrl, dialog);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReads"
void AlignToReferenceBlastDialogFiller::setReads(const QStringList& readUrls, QWidget* dialog) {
    auto addReadButton = GTWidget::findWidget("addReadButton");
    auto readsListWidget = GTWidget::findListWidget("readsListWidget", dialog);

    QStringList uniqueReads;
    for (const QString& read : qAsConst(readUrls)) {
        if (readsListWidget->findItems(read, Qt::MatchExactly).isEmpty()) {
            uniqueReads << read;
        }
    }
    GT_CHECK(!uniqueReads.isEmpty(), "List of unique reads is empty!");

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list(uniqueReads));
    GTWidget::click(addReadButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDestination"
void AlignToReferenceBlastDialogFiller::setDestination(const QString& destinationUrl, QWidget* dialog) {
    GTLineEdit::setText("outputLineEdit", destinationUrl, dialog);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
