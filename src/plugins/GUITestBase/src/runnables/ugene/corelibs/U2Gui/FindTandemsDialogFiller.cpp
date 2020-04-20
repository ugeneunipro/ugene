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

#include "FindTandemsDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTLineEdit.h>

#include <QApplication>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

FindTandemsDialogFiller::FindTandemsDialogFiller( HI::GUITestOpStatus &_os, const QString & _resultFilesPath)
    : Filler(_os, "FindTandemsDialog"), button(Start), resultAnnotationFilesPath(_resultFilesPath){
}

#define GT_CLASS_NAME "GTUtilsDialog::FindTandemsDialogFiller"
#define GT_METHOD_NAME "run"

FindTandemsDialogFiller::FindTandemsDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "FindTandemsDialog", scenario),
    button(Start)
{

}

void FindTandemsDialogFiller::commonScenario(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (button == Cancel) {
        QAbstractButton *cancelButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "cancelButton", dialog));
        GTWidget::click(os, cancelButton);
        return;
    }

    GTTabWidget::clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, "tabWidget"), 1);
    GTGlobals::sleep(100);

    QLineEdit *resultLocationEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "leNewTablePath", dialog));
    GT_CHECK(resultLocationEdit, "resultLocation is NULL");
    GTLineEdit::setText(os, resultLocationEdit, resultAnnotationFilesPath);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
