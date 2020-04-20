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
#include <QComboBox>

#include <U2Test/UGUITest.h>

#include <primitives/GTComboBox.h>
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTWidget.h>

#include "GTUtilsTaskTreeView.h"

#include "BlastAllSupportDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::BlastAllSupportDialogFiller"

BlastAllSupportDialogFiller::BlastAllSupportDialogFiller(const Parameters &parameters, HI::GUITestOpStatus &os)
: Filler(os, "BlastAllSupportDialog"), parameters(parameters), dialog(NULL)
{

}

BlastAllSupportDialogFiller::BlastAllSupportDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario)
    : Filler(os, "BlastAllSupportDialog", scenario), dialog(NULL)
{

}

#define GT_METHOD_NAME "commonScenario"
void BlastAllSupportDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (parameters.test_3211) {
        test_3211();
        return;
    }

    if (!parameters.runBlast) {
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    QComboBox *programName = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "programName", dialog));
    GT_CHECK(programName, "programName is NULL");
    GTComboBox::setIndexWithText(os, programName, parameters.programNameText);

    if (!parameters.dbPath.isEmpty()) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters.dbPath));
        GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton"));
    }

    if (parameters.withInputFile) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters.inputPath));
        GTWidget::click(os, GTWidget::findWidget(os, "browseInput"));
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

void BlastAllSupportDialogFiller::test_3211() {
    //Expected state: there is a "Request to Local BLAST Database" dialog without an annotation widget.
    QWidget *widget = GTWidget::findWidget(os, "rbCreateNewTable", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == widget, "Annotations widget exists");

    //2. Set any input sequence.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, UGUITest::dataDir + "samples/FASTA/human_T1.fa"));
    GTWidget::click(os, GTWidget::findWidget(os, "browseInput"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //Expected state: an annotation widget was added.
    GTWidget::findWidget(os, "rbCreateNewTable");

    //3. Set any another input sequence.
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, UGUITest::testDir + "_common_data/fasta/human_T1_cutted.fa"));
    GTWidget::click(os, GTWidget::findWidget(os, "browseInput"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state: there is a single annotation widget.
    GTWidget::findWidget(os, "rbCreateNewTable");

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}

#undef GT_CLASS_NAME

}   // namespace U2
