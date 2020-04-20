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

#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>

#include "AddPrimerDialogFiller.h"

namespace U2 {

AddPrimerDialogFiller::Parameters::Parameters()
: primer(""), name(""), scenario(NULL)
{

}

AddPrimerDialogFiller::AddPrimerDialogFiller(HI::GUITestOpStatus &os, const Parameters &parameters)
: Filler(os, "EditPrimerDialog", parameters.scenario), parameters(parameters)
{

}

#define GT_CLASS_NAME "GTUtilsDialog::AddPrimerDialogFiller"
#define GT_METHOD_NAME "run"
void AddPrimerDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *primerEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "primerEdit", dialog));
    GT_CHECK(primerEdit, "primerEdit is NULL");
    GTLineEdit::setText(os, primerEdit, parameters.primer);

    if (!parameters.name.isEmpty()) {
        QLineEdit *nameEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "nameEdit", dialog));
        GT_CHECK(nameEdit, "nameEdit is NULL");
        GTLineEdit::setText(os, nameEdit, parameters.name);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}

