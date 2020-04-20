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
#include <QDir>
#include <QFileInfoList>

#include <U2Gui/ToolsMenu.h>

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>

#include "GTTestsIndexReuse.h"
#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"

namespace U2 {
namespace GUITest_index_reuse {

class WrongRefSetter: public CustomScenario{
public:
    WrongRefSetter(QString _aligner, QString _wrongRef, QString _message, QString _reads = "", QMessageBox::StandardButton _b = QMessageBox::Ok):
    aligner(_aligner), wrongRef(_wrongRef), message(_message), reads(_reads), b(_b){}
    QString aligner;
    QString wrongRef;
    QString message;
    QString reads;
    QMessageBox::StandardButton b;
    void run(HI::GUITestOpStatus &os){
        QWidget* dialog = QApplication::activeModalWidget();
        CHECK_SET_ERR(dialog, "activeModalWidget is NULL");
        GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "methodNamesBox", dialog), aligner);

        //    2. Set wrong file as reference
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, wrongRef));
        GTWidget::click(os, GTWidget::findWidget(os, "addRefButton", dialog));

        if(reads != ""){
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, reads));
            GTWidget::click(os, GTWidget::findWidget(os, "addShortreadsButton", dialog));
        }


        //    Expcted state: warning messagebox appeared
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, b, message));
        GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        GTGlobals::sleep();
        GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
    }
};

//BOWTIE TESTS

GUI_TEST_CLASS_DEFINITION(test_0001){
//    File will be copied to the sandbox
//    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer l("Launching Bowtie build indexer tool");
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                               refName + ".fa",
                                                               testDir + "_common_data/bowtie2/",
                                                               "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);
//    Expected state: index is built
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".1.ebwt"), "index1 not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".2.ebwt"), "index2 not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".3.ebwt"), "index3 not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".4.ebwt"), "index4 not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".rev.1.ebwt"), "index_rev1 not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".rev.2.ebwt"), "index_rev2 not found");
    GTUtilsLog::checkContainsMessage(os, l);

//    2. Repeat step 1.
    GTLogTracer l1("Launching Bowtie build indexer tool");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l1, false);

    QFileInfoList fList = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fList.size() == 15, QString("unexpected files number: %1").arg(fList.size()));

//    3. Rename index files to refName.fa.1.ebwt. Repeat step 1.
    QFile::rename(sandBoxDir + refName + ".1.ebwt", sandBoxDir + refName + ".fa.1.ebwt");
    QFile::rename(sandBoxDir + refName + ".2.ebwt", sandBoxDir + refName + ".fa.2.ebwt");
    QFile::rename(sandBoxDir + refName + ".3.ebwt", sandBoxDir + refName + ".fa.3.ebwt");
    QFile::rename(sandBoxDir + refName + ".4.ebwt", sandBoxDir + refName + ".fa.4.ebwt");
    QFile::rename(sandBoxDir + refName + ".1.ebwt", sandBoxDir + refName + ".fa.1.ebwt");
    QFile::rename(sandBoxDir + refName + ".rev.1.ebwt", sandBoxDir + refName + ".fa.rev.1.ebwt");
    QFile::rename(sandBoxDir + refName + ".rev.2.ebwt", sandBoxDir + refName + ".fa.rev.2.ebwt");

    GTLogTracer l2("Launching Bowtie build indexer tool");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l2, false);

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListRenamed.size() == 16, QString("2: unexpected files number: %1").arg(fListRenamed.size()));

//    4.  An index file is set as a reference sequence
    GTLogTracer l3("Launching Bowtie build indexer tool");
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".fa.1.ebwt",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &ParametersIndex));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l3, false);

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListIndex.size() == 17, QString("3: unexpected files number: %1").arg(fListIndex.size()));

}

GUI_TEST_CLASS_DEFINITION(test_0002){
    //    1. {Tools -> Align to reference -> Align short reads}
    //    2. Set wrong file as reference, align with Bowtie
    //    Exected state: error message appears
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new WrongRefSetter("Bowtie", dataDir + "samples/CLUSTALW/COI.aln",
                                                                                      "These files have the incompatible format:",
                                                                                  testDir + "_common_data/bowtie2/reads_1.fq",  QMessageBox::No)));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0003){
//BROKEN INDEX TEST
//    1. {Tools -> Align to reference -> Align short reads}
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                               refName + ".fa",
                                                               testDir + "_common_data/bowtie2/",
                                                               "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);
//    2. Delete one of the indes files
    QFile f(sandBoxDir + refName + ".3.ebwt");
    CHECK_SET_ERR(f.exists(), "index not built");
    f.remove();

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new WrongRefSetter("Bowtie", sandBoxDir + refName + ".1.ebwt",
                                                                                      "You set the index as a reference and the index files are corrupted")));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTGlobals::sleep();
}

//BOWTIE2 TESTS

GUI_TEST_CLASS_DEFINITION(test_0004){
//    File will be copied to the sandbox
//    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer l("Launching Bowtie 2 build indexer tool");
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                               refName + ".fa",
                                                               testDir + "_common_data/bowtie2/",
                                                               "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie2);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is built
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".1.bt2"), "index1 not created");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".2.bt2"), "index2 not created");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".3.bt2"), "index3 not created");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".4.bt2"), "index4 not created");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".rev.1.bt2"), "index_rev1 not created");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".rev.1.bt2"), "index_rev1 not created");
    GTUtilsLog::checkContainsMessage(os, l);

//    2. Repeat step 1.
    GTLogTracer l1("Launching Bowtie 2 build indexer tool");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l1, false);

    QFileInfoList fList = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fList.size() == 15, QString("unexpected files number: %1").arg(fList.size()));

//    3. Rename index files to refName.fa.1.bt2. Repeat step 1.
    QFile::rename(sandBoxDir + refName + ".1.bt2", sandBoxDir + refName + ".fa.1.bt2");
    QFile::rename(sandBoxDir + refName + ".2.bt2", sandBoxDir + refName + ".fa.2.bt2");
    QFile::rename(sandBoxDir + refName + ".3.bt2", sandBoxDir + refName + ".fa.3.bt2");
    QFile::rename(sandBoxDir + refName + ".4.bt2", sandBoxDir + refName + ".fa.4.bt2");
    QFile::rename(sandBoxDir + refName + ".1.bt2", sandBoxDir + refName + ".fa.1.bt2");
    QFile::rename(sandBoxDir + refName + ".rev.1.bt2", sandBoxDir + refName + ".fa.rev.1.bt2");
    QFile::rename(sandBoxDir + refName + ".rev.2.bt2", sandBoxDir + refName + ".fa.rev.2.bt2");

    GTLogTracer l2("Launching Bowtie 2 build indexer tool");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l2, false);

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListRenamed.size() == 16, QString("2: unexpected files number: %1").arg(fListRenamed.size()));

//    4.  An index file is set as a reference sequence
    GTLogTracer l3("Launching Bowtie build indexer tool");
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".fa.1.bt2",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bowtie2);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &ParametersIndex));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l3, false);

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListIndex.size() == 17, QString("3: unexpected files number: %1").arg(fListIndex.size()));

}

GUI_TEST_CLASS_DEFINITION(test_0005){
    //    1. {Tools -> Align to reference -> Align short reads}
    //    2. Set wrong file as reference, align with Bowtie2
    //    Exected state: error message appears
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new WrongRefSetter("Bowtie2", dataDir + "samples/CLUSTALW/COI.aln",
                                                                                      "These files have the incompatible format:",
                                                                                      testDir + "_common_data/bowtie2/reads_1.fq",  QMessageBox::No)));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0006){
//BROKEN INDEX TEST
//    1. {Tools -> Align to reference -> Align short reads}
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie2);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    2. Delete one of the indes files
    QFile f(sandBoxDir + refName + ".3.bt2");
    CHECK_SET_ERR(f.exists(), "index not built");
    f.remove();

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new WrongRefSetter("Bowtie2", sandBoxDir + refName + ".1.bt2",
                                                                                      "You set the index as a reference and the index files are corrupted")));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTGlobals::sleep();
}

//BWA TESTS
GUI_TEST_CLASS_DEFINITION(test_0007){
//    File will be copied to the sandbox
//    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer l("bwa index -p");
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is built
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.amb"), "amb not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.ann"), "ann not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.bwt"), "bwt not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.pac"), "pac not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.sa"), "sa not found");
    GTUtilsLog::checkContainsMessage(os, l);

//    2. Repeat step 1.
    GTLogTracer l1("bwa index -p");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l1, false);

    QFileInfoList fList = QDir(sandBoxDir).entryInfoList();
    QString s;
    foreach (QFileInfo info, fList) {
        s.append(info.absoluteFilePath() + "\n");
    }
    CHECK_SET_ERR(fList.size() == 15, s + QString("unexpected files number: %1").arg(fList.size()));

//    3. Rename index files to refName.fa.1.ebwt. Repeat step 1.
    QFile::rename(sandBoxDir + refName + ".fa.amb", sandBoxDir + refName + ".amb");
    QFile::rename(sandBoxDir + refName + ".fa.ann", sandBoxDir + refName + ".ann");
    QFile::rename(sandBoxDir + refName + ".fa.bwt", sandBoxDir + refName + ".bwt");
    QFile::rename(sandBoxDir + refName + ".fa.pac", sandBoxDir + refName + ".pac");
    QFile::rename(sandBoxDir + refName + ".fa.sa", sandBoxDir + refName + ".sa");

    GTLogTracer l2("bwa index -p");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l2, false);

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    QString s1;
    foreach (QFileInfo info, fListRenamed) {
        s1.append(info.absoluteFilePath() + "\n");
    }
    CHECK_SET_ERR(fListRenamed.size() == 16, s1 + QString("2: unexpected files number: %1").arg(fListRenamed.size()));

//    4.  An index file is set as a reference sequence
    GTLogTracer l3("bwa index -p");
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".ann",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &ParametersIndex));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l3, false);

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    QString s2;
    foreach (QFileInfo info, fListIndex) {
        s2.append(info.absoluteFilePath() + "\n");
    }
    CHECK_SET_ERR(fListIndex.size() == 19, s2 + QString("3: unexpected files number: %1").arg(fListIndex.size()));

}

GUI_TEST_CLASS_DEFINITION(test_0008){
    //    1. {Tools -> Align to reference -> Align short reads}
    //    2. Set wrong file as reference, align with BWA
    //    Exected state: error message appears
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new WrongRefSetter("BWA", dataDir + "samples/CLUSTALW/COI.aln",
                                                                                      "These files have the incompatible format:",
                                                                                      testDir + "_common_data/bowtie2/reads_1.fq",  QMessageBox::No)));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0009){
//BROKEN INDEX TEST
//    1. {Tools -> Align to reference -> Align short reads}
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    2. Delete one of the indes files
    QFile f(sandBoxDir + refName + ".fa.bwt");
    CHECK_SET_ERR(f.exists(), "index not built");
    f.remove();

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new WrongRefSetter("BWA", sandBoxDir + refName + ".fa.ann",
                                                                                      "You set the index as a reference and the index files are corrupted")));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0010){
//    File will be copied to the sandbox
//    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer l("bwa index -p");
    const QString refName = "lambda_virus";
    GTFile::copy(os, testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::BwaMem);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &Parameters));

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is built
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.amb"), "amb not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.ann"), "ann not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.bwt"), "bwt not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.pac"), "pac not found");
    CHECK_SET_ERR(GTFile::check(os, sandBoxDir + refName + ".fa.sa"), "sa not found");
    GTUtilsLog::checkContainsMessage(os, l);

//    2. Repeat step 1.
    GTLogTracer l1("bwa index -p");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l1, false);

    QFileInfoList fList = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fList.size() == 14, QString("unexpected files number: %1").arg(fList.size()));

//    3. Rename index files to refName.fa.1.ebwt. Repeat step 1.
    QFile::rename(sandBoxDir + refName + ".fa.amb", sandBoxDir + refName + ".amb");
    QFile::rename(sandBoxDir + refName + ".fa.ann", sandBoxDir + refName + ".ann");
    QFile::rename(sandBoxDir + refName + ".fa.bwt", sandBoxDir + refName + ".bwt");
    QFile::rename(sandBoxDir + refName + ".fa.pac", sandBoxDir + refName + ".pac");
    QFile::rename(sandBoxDir + refName + ".fa.sa", sandBoxDir + refName + ".sa");

    GTLogTracer l2("bwa index -p");
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "AssemblyToRefDialog"));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l2, false);

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListRenamed.size() == 15, QString("2: unexpected files number: %1").arg(fListRenamed.size()));

//    4.  An index file is set as a reference sequence
    GTLogTracer l3("bwa index -p");
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".ann",
                                                               testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, &ParametersIndex));

    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Map reads to reference...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Expected state: index is reused
    GTUtilsLog::checkContainsMessage(os, l3, false);

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListIndex.size() == 19, QString("3: unexpected files number: %1").arg(fListIndex.size()));

}

}
}
