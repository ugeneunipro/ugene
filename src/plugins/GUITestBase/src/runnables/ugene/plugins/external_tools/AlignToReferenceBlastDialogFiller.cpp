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

#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>

#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTWidget.h>

#include <U2Core/U2SafePoints.h>

#include <U2Test/UGUITest.h>

#include "AlignToReferenceBlastDialogFiller.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::AlignToReferenceBlastDialogFiller"

AlignToReferenceBlastDialogFiller::AlignToReferenceBlastDialogFiller(const Settings &settings, HI::GUITestOpStatus &os)
: Filler(os, "AlignToReferenceBlastDialog"), settings(settings)
{

}

AlignToReferenceBlastDialogFiller::AlignToReferenceBlastDialogFiller(HI::GUITestOpStatus &os, CustomScenario* c)
: Filler(os, "AlignToReferenceBlastDialog", c)
{

}

#define GT_METHOD_NAME "commonScenario"
void AlignToReferenceBlastDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    setReference(os, settings.referenceUrl, dialog);
    CHECK_OP(os, );

    setReads(os, settings.readUrls, dialog);
    CHECK_OP(os, );

    QSpinBox* settingSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "minIdentitySpinBox", dialog));
    GT_CHECK(settingSpinBox, "minIdentitySpinBox is NULL");
    GTSpinBox::setValue(os, settingSpinBox, settings.minIdentity);

    settingSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "qualitySpinBox", dialog));
    GT_CHECK(settingSpinBox, "qualitySpinBox is NULL");
    GTSpinBox::setValue(os, settingSpinBox, settings.qualityThreshold);

    QCheckBox* checkBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "addToProjectCheckbox", dialog));
    GT_CHECK(checkBox, "addToProjectCheckbox is NULL");
    GTCheckBox::setChecked(os, checkBox, settings.addResultToProject);

    setDestination(os, settings.outAlignment, dialog);
    CHECK_OP(os, );

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReference"
void AlignToReferenceBlastDialogFiller::setReference(GUITestOpStatus &os, const QString &referenceUrl, QWidget *dialog) {
    QLineEdit* reference = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "referenceLineEdit", dialog));
    GT_CHECK(reference, "referenceLineEdit is NULL");
    GTLineEdit::setText(os, reference, referenceUrl);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setReads"
void AlignToReferenceBlastDialogFiller::setReads(GUITestOpStatus &os, const QStringList &readUrls, QWidget *dialog) {
    QWidget* addReadButton = GTWidget::findWidget(os, "addReadButton");
    GT_CHECK(addReadButton, "addReadButton is NULL");

    QListWidget* readsListWidget = qobject_cast<QListWidget*>(GTWidget::findWidget(os, "readsListWidget", dialog));
    GT_CHECK(readsListWidget, "readsListWidget is NULL");

    QStringList uniqueReads;
    foreach (const QString& read, readUrls) {
        if (readsListWidget->findItems(read, Qt::MatchExactly).isEmpty()) {
            uniqueReads << read;
        }
    }
    CHECK(!uniqueReads.isEmpty(), );

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils_list(os, uniqueReads));
    GTWidget::click(os, addReadButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDestination"
void AlignToReferenceBlastDialogFiller::setDestination(GUITestOpStatus &os, const QString &destinationUrl, QWidget *dialog) {
    QLineEdit* out = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "outputLineEdit", dialog));
    GT_CHECK(out, "outputLineEdit is NULL");
    GTLineEdit::setText(os, out, destinationUrl);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
