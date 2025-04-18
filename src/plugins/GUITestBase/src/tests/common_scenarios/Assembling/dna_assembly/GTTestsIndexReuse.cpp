/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>

#include <QApplication>
#include <QDir>
#include <QFileInfoList>

#include <U2Gui/ToolsMenu.h>

#include "GTTestsIndexReuse.h"
#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"

namespace U2 {
namespace GUITest_index_reuse {

class WrongRefSetter : public CustomScenario {
public:
    WrongRefSetter(QString _aligner, QString _wrongRef, QString _message, QString _reads = "", QMessageBox::StandardButton _b = QMessageBox::Ok)
        : aligner(_aligner), wrongRef(_wrongRef), message(_message), reads(_reads), b(_b) {
    }
    QString aligner;
    QString wrongRef;
    QString message;
    QString reads;
    QMessageBox::StandardButton b;
    void run() {
        QWidget* dialog = GTWidget::getActiveModalWidget();
        GTComboBox::selectItemByText(GTWidget::findComboBox("methodNamesBox", dialog), aligner);

        //    2. Set wrong file as reference
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(wrongRef));
        GTWidget::click(GTWidget::findWidget("addRefButton", dialog));

        if (reads != "") {
            GTUtilsDialog::waitForDialog(new GTFileDialogUtils(reads));
            GTWidget::click(GTWidget::findWidget("addShortreadsButton", dialog));
        }

        //    Expected state: warning messagebox appeared
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(b, message));
        GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
    }
};

// BOWTIE TESTS

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    File will be copied to the sandbox
    //    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer lt;
    QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                 refName + ".fa",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::add(new AlignShortReadsFiller(&Parameters));

    GTUtilsDialog::add(new ImportBAMFileFiller());

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is built
    GTFile::checkFileExists(sandBoxDir + refName + ".1.ebwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".2.ebwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".3.ebwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".4.ebwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".rev.1.ebwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".rev.2.ebwt");
    CHECK_SET_ERR(lt.hasMessage("Launching Bowtie build indexer tool"), "Expected message not found");

    //    2. Repeat step 1.
    lt.clear();
    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));

    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("Launching Bowtie build indexer tool"), "Found unexpected message");

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

    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));
    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("Launching Bowtie build indexer tool"), "Found unexpected message");

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListRenamed.size() == 16, QString("2: unexpected files number: %1").arg(fListRenamed.size()));

    //    4.  An index file is set as a reference sequence
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".fa.1.ebwt", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::add(new AlignShortReadsFiller(&ParametersIndex));

    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("Launching Bowtie build indexer tool"), "Found unexpected message");

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListIndex.size() == 17, QString("3: unexpected files number: %1").arg(fListIndex.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    1. {Tools -> Align to reference -> Align short reads}
    //    2. Set wrong file as reference, align with Bowtie
    //    Exected state: error message appears
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new WrongRefSetter("Bowtie", dataDir + "samples/CLUSTALW/COI.aln", "These files have the incompatible format:", testDir + "_common_data/bowtie2/reads_1.fq", QMessageBox::No)));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // BROKEN INDEX TEST
    //     1. {Tools -> Align to reference -> Align short reads}
    const QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/",
                                                 refName + ".fa",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie);
    GTUtilsDialog::add(new AlignShortReadsFiller(&Parameters));

    GTUtilsDialog::add(new ImportBAMFileFiller());

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Delete one of the indes files
    QFile f(sandBoxDir + refName + ".3.ebwt");
    CHECK_SET_ERR(f.exists(), "index not built");
    f.remove();

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new WrongRefSetter("Bowtie", sandBoxDir + refName + ".1.ebwt", "You set the index as a reference and the index files are corrupted")));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

// BOWTIE2 TESTS

GUI_TEST_CLASS_DEFINITION(test_0004) {
    //    File will be copied to the sandbox
    //    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer lt;
    QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", sandBoxDir + "lambda_virus.fa");

    AlignShortReadsFiller::Parameters parameters(sandBoxDir,
                                                 refName + ".fa",
                                                 testDir + "_common_data/bowtie2/",
                                                 "reads_1.fq",
                                                 AlignShortReadsFiller::Parameters::Bowtie2);
    GTUtilsDialog::add(new AlignShortReadsFiller(&parameters));
    GTUtilsDialog::add(new ImportBAMFileFiller());
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is built
    GTFile::checkFileExists(sandBoxDir + refName + ".1.bt2");
    GTFile::checkFileExists(sandBoxDir + refName + ".2.bt2");
    GTFile::checkFileExists(sandBoxDir + refName + ".3.bt2");
    GTFile::checkFileExists(sandBoxDir + refName + ".4.bt2");
    GTFile::checkFileExists(sandBoxDir + refName + ".rev.1.bt2");
    GTFile::checkFileExists(sandBoxDir + refName + ".rev.1.bt2");
    CHECK_SET_ERR(lt.hasMessage("Launching Bowtie 2 build indexer tool"), "Expected message not found");

    //    2. Repeat step 1.
    lt.clear();
    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));
    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("Launching Bowtie build indexer tool"), "Found unexpected message");

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

    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));
    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("Launching Bowtie build indexer tool"), "Found unexpected message");

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListRenamed.size() == 16, QString("2: unexpected files number: %1").arg(fListRenamed.size()));

    //    4.  An index file is set as a reference sequence
    AlignShortReadsFiller::Parameters parametersIndex(sandBoxDir, refName + ".fa.1.bt2", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bowtie2);
    GTUtilsDialog::add(new AlignShortReadsFiller(&parametersIndex));
    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("Launching Bowtie build indexer tool"), "Found unexpected message");

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListIndex.size() == 17, QString("3: unexpected files number: %1").arg(fListIndex.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //    1. {Tools -> Align to reference -> Align short reads}
    //    2. Set wrong file as reference, align with Bowtie2
    //    Exected state: error message appears
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new WrongRefSetter("Bowtie2", dataDir + "samples/CLUSTALW/COI.aln", "These files have the incompatible format:", testDir + "_common_data/bowtie2/reads_1.fq", QMessageBox::No)));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // BROKEN INDEX TEST
    //     1. {Tools -> Align to reference -> Align short reads}
    const QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bowtie2);
    GTUtilsDialog::add(new AlignShortReadsFiller(&Parameters));
    GTUtilsDialog::add(new ImportBAMFileFiller());

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Delete one of the indes files
    QFile f(sandBoxDir + refName + ".3.bt2");
    CHECK_SET_ERR(f.exists(), "index not built");
    f.remove();

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new WrongRefSetter("Bowtie2", sandBoxDir + refName + ".1.bt2", "You set the index as a reference and the index files are corrupted")));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

// BWA TESTS
GUI_TEST_CLASS_DEFINITION(test_0007) {
    //    File will be copied to the sandbox
    //    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer lt;
    const QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::add(new AlignShortReadsFiller(&Parameters));

    GTUtilsDialog::add(new ImportBAMFileFiller());

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is built
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.amb");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.ann");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.bwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.pac");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.sa");
    CHECK_SET_ERR(lt.hasMessage("bwa index -p"), "Expected message not found");

    //    2. Repeat step 1.
    lt.clear();
    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));

    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");

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

    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));
    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    QString s1;
    foreach (QFileInfo info, fListRenamed) {
        s1.append(info.absoluteFilePath() + "\n");
    }
    CHECK_SET_ERR(fListRenamed.size() == 16, s1 + QString("2: unexpected files number: %1").arg(fListRenamed.size()));

    //    4.  An index file is set as a reference sequence
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".ann", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::add(new AlignShortReadsFiller(&ParametersIndex));

    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    QString s2;
    foreach (QFileInfo info, fListIndex) {
        s2.append(info.absoluteFilePath() + "\n");
    }
    CHECK_SET_ERR(fListIndex.size() == 19, s2 + QString("3: unexpected files number: %1").arg(fListIndex.size()));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    //    1. {Tools -> Align to reference -> Align short reads}
    //    2. Set wrong file as reference, align with BWA
    //    Exected state: error message appears
    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new WrongRefSetter("BWA", dataDir + "samples/CLUSTALW/COI.aln", "These files have the incompatible format:", testDir + "_common_data/bowtie2/reads_1.fq", QMessageBox::No)));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // BROKEN INDEX TEST
    //     1. {Tools -> Align to reference -> Align short reads}
    const QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::add(new AlignShortReadsFiller(&Parameters));

    GTUtilsDialog::add(new ImportBAMFileFiller());

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    2. Delete one of the indes files
    QFile f(sandBoxDir + refName + ".fa.bwt");
    CHECK_SET_ERR(f.exists(), "index not built");
    f.remove();

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new WrongRefSetter("BWA", sandBoxDir + refName + ".fa.ann", "You set the index as a reference and the index files are corrupted")));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //    File will be copied to the sandbox
    //    1. {Tools -> Align to reference -> Align short reads}
    GTLogTracer lt;
    const QString refName = "lambda_virus";
    GTFile::copy(testDir + "_common_data/bowtie2/lambda_virus.fa", testDir + "_common_data/scenarios/sandbox/lambda_virus.fa");

    AlignShortReadsFiller::Parameters Parameters(testDir + "_common_data/scenarios/sandbox/", refName + ".fa", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::BwaMem);
    GTUtilsDialog::add(new AlignShortReadsFiller(&Parameters));

    GTUtilsDialog::add(new ImportBAMFileFiller());

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is built
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.amb");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.ann");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.bwt");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.pac");
    GTFile::checkFileExists(sandBoxDir + refName + ".fa.sa");
    CHECK_SET_ERR(lt.hasMessage("bwa index -p"), "Expected message not found");

    //    2. Repeat step 1.
    lt.clear();
    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));
    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");

    QFileInfoList fList = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fList.size() == 14, QString("unexpected files number: %1").arg(fList.size()));

    //    3. Rename index files to refName.fa.1.ebwt. Repeat step 1.
    QFile::rename(sandBoxDir + refName + ".fa.amb", sandBoxDir + refName + ".amb");
    QFile::rename(sandBoxDir + refName + ".fa.ann", sandBoxDir + refName + ".ann");
    QFile::rename(sandBoxDir + refName + ".fa.bwt", sandBoxDir + refName + ".bwt");
    QFile::rename(sandBoxDir + refName + ".fa.pac", sandBoxDir + refName + ".pac");
    QFile::rename(sandBoxDir + refName + ".fa.sa", sandBoxDir + refName + ".sa");

    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");
    GTUtilsDialog::add(new DefaultDialogFiller("AssemblyToRefDialog"));

    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");

    QFileInfoList fListRenamed = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListRenamed.size() == 15, QString("2: unexpected files number: %1").arg(fListRenamed.size()));

    //    4.  An index file is set as a reference sequence
    AlignShortReadsFiller::Parameters ParametersIndex(testDir + "_common_data/scenarios/sandbox/", refName + ".ann", testDir + "_common_data/bowtie2/", "reads_1.fq", AlignShortReadsFiller::Parameters::Bwa);
    GTUtilsDialog::add(new AlignShortReadsFiller(&ParametersIndex));

    GTUtilsDialog::add(new DefaultDialogFiller("Import BAM File", QDialogButtonBox::Cancel));

    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Expected state: index is reused
    CHECK_SET_ERR(!lt.hasMessage("bwa index -p"), "Found unexpected message");

    QFileInfoList fListIndex = QDir(sandBoxDir).entryInfoList();
    CHECK_SET_ERR(fListIndex.size() == 19, QString("3: unexpected files number: %1").arg(fListIndex.size()));
}

}  // namespace GUITest_index_reuse
}  // namespace U2
