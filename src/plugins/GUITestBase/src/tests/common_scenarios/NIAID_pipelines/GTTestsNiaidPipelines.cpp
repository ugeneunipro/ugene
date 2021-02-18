/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GTTestsNiaidPipelines.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QLineEdit>
#include <QProcess>
#include <QWizard>

#include <U2Core/AppContext.h>

#include <U2Gui/ToolsMenu.h>

#include "GTGlobals.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTAction.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTMenu.h"
#include "primitives/GTToolbar.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/external_tools/TrimmomaticDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/ConfigurationWizardFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTFile.h"
#include "utils/GTKeyboardUtils.h"
#include "utils/GTUtilsApp.h"

namespace U2 {

namespace GUITest_common_scenarios_NIAID_pipelines {

static bool hasDashboardNotification(HI::GUITestOpStatus &os, const QString &errMsg) {
    QWidget *const dashboard = GTWidget::findWidget(os, "NotificationsDashboardWidget",
        GTUtilsDashboard::getDashboard(os));
    return !GTWidget::findLabelByText(os, errMsg, dashboard).isEmpty();
};

GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "call variants");
    //GTUtilsDialog::waitForDialog(os, new WizardFiller0001(os,"BED or position list file"));
    QAbstractButton *wiz = GTAction::button(os, "Show wizard");
    GTWidget::click(os, wiz);

    TaskScheduler *scheduller = AppContext::getTaskScheduler();

    GTGlobals::sleep(5000);
    while (!scheduller->getTopLevelTasks().isEmpty()) {
        GTGlobals::sleep();
    }
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
}
#define GT_CLASS_NAME "GTUtilsDialog::WizardFiller0002"
#define GT_METHOD_NAME "run"
class WizardFiller0002 : public WizardFiller {
public:
    WizardFiller0002(HI::GUITestOpStatus &_os)
        : WizardFiller(_os, "Tuxedo Wizard") {
    }
    void run() {
        QWidget *dialog = GTWidget::getActiveModalWidget(os);

        QList<QWidget *> list = dialog->findChildren<QWidget *>();

        QList<QWidget *> datasetList;
        foreach (QWidget *act, list) {
            if (act->objectName() == "DatasetWidget")
                datasetList.append(act);
        }
        QWidget *dataset = datasetList.takeLast();

        QPushButton *cancel = qobject_cast<QPushButton *>(GTWidget::findButtonByText(os, "Cancel", dialog));

        GT_CHECK(dataset, "dataset widget not found");
        GT_CHECK(cancel, "cancel button not found");

        QPoint i = dataset->mapToGlobal(dataset->rect().bottomLeft());
        QPoint j = cancel->mapToGlobal(cancel->rect().topLeft());

        CHECK_SET_ERR(qAbs(i.y() - j.y()) < 100, QString("%1   %2").arg(i.y()).arg(j.y()));
        GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
    }
};
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //    2. Open tuxedo pipeline from samples
    GTUtilsDialog::waitForDialog(os, new WizardFiller0002(os));
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Tuxedo Workflow", QStringList() << "Full"
                                                                                                                  << "Single-end"));
    GTUtilsWorkflowDesigner::addSample(os, "Tuxedo tools");
    GTGlobals::sleep();
    //    3. Open wizard

    //    Expected state: dataset widget fits full height
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    class ChIPSeqAnalysisWizardFiller : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);

            QLineEdit *lineEdit = GTWidget::findWidgetByType<QLineEdit *>(os, dialog, "lineEdit not found");
            GTLineEdit::setText(os, lineEdit, QFileInfo(dataDir + "cistrome_input/macs_input_chr4/chr4.bed").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> ChIP-Seq data analysis.... Choose Only treatment tags
    //2. Set "cistrome_input/macs_input_chr4/chr4.bed" as input
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure Cistrome Workflow",
                                                                    QStringList() << "Only treatment tags"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-seq Analysis Wizard", new ChIPSeqAnalysisWizardFiller()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "ChIP-Seq data analysis...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 60 * 1000 * 20);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    class ChIPSeqAnalysisWizardFiller : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "Treatment FASTQ widget");
            QLineEdit *lineEdit2 = GTWidget::findExactWidget<QLineEdit *>(os, "Control FASTQ widget");
            GTLineEdit::setText(os, lineEdit1, QFileInfo(dataDir + "cistrome_input/macs_input_chr4/chr4.bed").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit2, QFileInfo(dataDir + "cistrome_input/macs_input_chr4/control_tags/chr4.bed").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };


    //1. Click Tools -> NGS data analysis -> ChIP-Seq data analysis.... Choose Treatment and control
    //2. Set "cistrome_input/macs_input_chr4/chr4.bed" as "Treatment" and "cistrome_input/macs_input_chr4/control_tags/chr4.bed" as "Control"
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os,
                                                                    "Configure Cistrome Workflow",
                                                                    QStringList() << "Treatment and control"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "ChIP-Seq Analysis Wizard", new ChIPSeqAnalysisWizardFiller()));

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "ChIP-Seq data analysis...");
    GTUtilsTaskTreeView::waitTaskFinished(os, 60 * 1000 * 20);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    //1. Click Tools -> NGS data analysis -> Raw ChIP-Seq data processing... Choose Single-end
    //2. Set "_common_data/fastq/lymph.fastq" and _common_data/fasta/DNA.fa as reads and reference in wizard
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors

    class RawChIPSeqDataProcessingWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files widget");
            
            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/fastq/lymph.fastq").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QLineEdit *lineEdit2 = GTWidget::findExactWidget<QLineEdit *>(os, "Reference genome widget");
            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/fasta/DNA.fa").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw ChIP-Seq Data Processing", QStringList() << "Single-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw ChIP-Seq Data Processing Wizard", new RawChIPSeqDataProcessingWizard()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Raw ChIP-Seq data processing...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //1. Click Tools -> NGS data analysis -> Raw ChIP-Seq data processing... Choose Paired-end
    //2. Set "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq" "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq" "_common_data/fasta/DNA.fa" as reads and reference in wizard
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors

    class RawChIPSeqDataProcessingWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files widget");
            QLineEdit *lineEdit11 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files with pairs widget");

            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit11, QFileInfo(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QLineEdit *lineEdit2 = GTWidget::findExactWidget<QLineEdit *>(os, "Reference genome widget");
            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/fasta/DNA.fa").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw ChIP-Seq Data Processing", QStringList() << "Paired-end"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw ChIP-Seq Data Processing Wizard", new RawChIPSeqDataProcessingWizard()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Raw ChIP-Seq data processing...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    //1. Click Tools -> NGS data analysis -> Variant calling...
    //2. Set "data\samples\Assembly\chrM.fa" as reference sequence file in wizard
    //3. Add "data\samples\Assembly\chrM.sorted.bam"
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: No errors in the log, no notifications in the dashboard. One output file "variations.vcf"

    class VariantCallingWizard : public CustomScenario {
        QString assemblyFilePath;

    public:
        VariantCallingWizard(const QString& assemblyFilePath)
            : assemblyFilePath(assemblyFilePath) {
        }

        void run(HI::GUITestOpStatus &os) override {
            QWidget *const wizard = GTWidget::getActiveModalWidget(os);
            GTWidget::click(os, GTWidget::findButtonByText(os, "Defaults", wizard));

            // Dialog filling
            QString refSeqFilePath = QFileInfo(dataDir + "samples/Assembly/chrM.fa").absoluteFilePath();
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "mainWidget", wizard), refSeqFilePath);

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, assemblyFilePath));
            GTWidget::click(os, GTWidget::findWidget(os, "addFileButton", wizard));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    QString assemblyFilePath = QFileInfo(dataDir + "samples/Assembly/chrM.sorted.bam").absoluteFilePath();
    const auto bamFiller = new WizardFiller(os, "Call Variants Wizard", new VariantCallingWizard(assemblyFilePath));
    GTUtilsDialog::waitForDialog(os, bamFiller);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Variant calling...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os),
        "Notifications in dashboard: " /*+ GTUtilsDashboard::getJoinedNotificationsString(os)*/);
    CHECK_SET_ERR(GTUtilsDashboard::getOutputFiles(os) == QStringList("variations.vcf"),
        "Expected output file variations.vcf")

    //6. Return to workflow and call the Variant calling wizard
    //7. Delete "chrM.sorted.bam" and add "data\samples\Assembly\chrM.sam"
    //4. Click "Next" several times and "Run"
    //5. Wait for workflow finished
    //Expected state: One error in log and one warning on the dashboard about header in the SAM file, two output files:
    //"chrM.sam.bam.sorted.bam", "variations.vcf"
    GTUtilsWorkflowDesigner::returnToWorkflow(os);

    assemblyFilePath = QFileInfo(dataDir + "samples/Assembly/chrM.sam").absoluteFilePath();
    const QString errMsg = QString("There is no header in the SAM file \"%1\". "
                                   "The header information will be generated automatically.")
                                   .arg(assemblyFilePath);

    const auto samFiller = new WizardFiller(os, "Call Variants Wizard", new VariantCallingWizard(assemblyFilePath));
    GTUtilsDialog::waitForDialog(os, samFiller);

    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Show wizard");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, lt, errMsg);
    CHECK_SET_ERR(hasDashboardNotification(os, errMsg), "Expected dashboard notification \"" + errMsg + "\"")

    QStringList out = GTUtilsDashboard::getOutputFiles(os);
    bool checkOutputFiles = out.contains("chrM.sam.bam.sorted.bam") && out.contains("variations.vcf") &&
                            out.size() == 2;
    CHECK_SET_ERR(checkOutputFiles,
        "Expected two output files, but one or both are missing or there is an unexpected file")
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    //1. Click Tools -> NGS data analysis -> Raw RNA-Seq data processing... Choose Single-end, Skip mapping
    //2. Set "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq" "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq" "_common_data/fasta/DNA.fa" as reads and reference in wizard
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors
    class RawRNASeqDataProcessingWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files widget");

            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/fastq/lymph.fastq").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw RNA-Seq Data Processing", QStringList() << "Single-end" << "Skip mapping"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw RNA-Seq Data Processing Wizard", new RawRNASeqDataProcessingWizard()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Raw RNA-Seq data processing...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    //1. Click Tools -> NGS data analysis -> Raw RNA-Seq data processing... Choose Paired-end, Skip mapping
    //2. Set "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq" "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq" "_common_data/fasta/DNA.fa" as reads and reference in wizard
    //3. Click "Next" several times and "Run"
    //4. Wait for workflow finished
    //Expected state: no errors
    class RawRNASeqDataProcessingWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files widget");
            QLineEdit *lineEdit11 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files with pairs widget");

            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit11, QFileInfo(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw RNA-Seq Data Processing", QStringList() << "Paired-end"
                                                                                                                              << "Skip mapping"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw RNA-Seq Data Processing Wizard", new RawRNASeqDataProcessingWizard()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Raw RNA-Seq data processing...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    //1. Click Tools -> NGS data analysis -> Raw RNA-Seq data processing... Choose Single-end, Include mapping with tophat
    //2. Set "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq" "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq" "_common_data/fasta/DNA.fa" as reads and reference in wizard
    //3. Click "Next" two times
    //4. Set "_common_data/bowtie/index" as bowtie index folder and "e_coli" as "Bowtie index basename"
    //5. Click "Next" several times and "Run"
    //6. Wait for workflow finished
    //Expected state: no errors
    class RawRNASeqDataProcessingWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override  {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files widget");

            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/fastq/lymph.fastq").absoluteFilePath());
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QLineEdit *lineEdit2 = GTWidget::findExactWidget<QLineEdit *>(os, "Bowtie index folder widget");
            QLineEdit *lineEdit3 = GTWidget::findExactWidget<QLineEdit *>(os, "Bowtie index basename widget");

            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/bowtie/index").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit3, "e_coli");
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw RNA-Seq Data Processing", QStringList() << "Single-end" << "Include mapping with TopHat"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw RNA-Seq Data Processing Wizard", new RawRNASeqDataProcessingWizard()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Raw RNA-Seq data processing...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    //1. Click Tools -> NGS data analysis -> Raw RNA-Seq data processing... Choose Paired-end, Include mapping with tophat
    //2. Set "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq" "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq" "_common_data/fasta/DNA.fa" as reads and reference in wizard
    //3. Click "Next" two times
    //4. Set "_common_data/bowtie/index" as bowtie index folder and "e_coli" as "Bowtie index basename"
    //5. Click "Next" several times and "Run"
    //6. Wait for workflow finished
    //Expected state: no errors
    class RawRNASeqDataProcessingWizard : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QLineEdit *lineEdit1 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files widget");
            QLineEdit *lineEdit11 = GTWidget::findExactWidget<QLineEdit *>(os, "FASTQ files with pairs widget");

            GTLineEdit::setText(os, lineEdit1, QFileInfo(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_1.fastq").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit11, QFileInfo(testDir + "_common_data/e_coli/e_coli_reads/e_coli_1_2.fastq").absoluteFilePath());

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            QLineEdit *lineEdit2 = GTWidget::findExactWidget<QLineEdit *>(os, "Bowtie index folder widget");
            QLineEdit *lineEdit3 = GTWidget::findExactWidget<QLineEdit *>(os, "Bowtie index basename widget");

            GTLineEdit::setText(os, lineEdit2, QFileInfo(testDir + "_common_data/bowtie/index").absoluteFilePath());
            GTLineEdit::setText(os, lineEdit3, "e_coli");
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new ConfigurationWizardFiller(os, "Configure Raw RNA-Seq Data Processing", QStringList() << "Paired-end"
                                                                                                                              << "Include mapping with TopHat"));
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Raw RNA-Seq Data Processing Wizard", new RawRNASeqDataProcessingWizard()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Raw RNA-Seq data processing...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    QStringList errors = GTUtilsWorkflowDesigner::getErrors(os);
    CHECK_SET_ERR(errors.size() == 0, "Unexpected errors");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    //1. Click Tools -> NGS data analysis -> RNA-Seq data analysis...
    //2. Set "_common_data/cmdline/tuxedo_pipeline/data/test_0004/fastq1/exp_1_1.fastq" as FASTQ file 1
    //3. Message box about paired reads appears. Click Yes in it
    //4. Click "Next". Select ILLUMINACLIP as Trimming steps
    //5. Click "Next". Set "_common_data/cmdline/tuxedo_pipeline/data/test_0004/NC_010473.fa" as reference genome
    //6. Click "Next" two times and "Run"
    //7. Wait for workflow finished
    //Expected state: no errors in log and dashboard
    class RnaSeqAnalysisWizardScenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) override {
            QWidget *const dialog = GTWidget::getActiveModalWidget(os);

            // Input fastq
            QList<QToolButton *> addFastq = dialog->findChildren<QToolButton *>("addFileButton");
            CHECK_SET_ERR(addFastq.size() == 2, "Add file button not found")
            const QString fastq = QFileInfo(testDir + "_common_data/cmdline/tuxedo_pipeline/data/test_0004/fastq1/exp_1_1.fastq").absoluteFilePath();

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, fastq));
            GTWidget::click(os, addFastq.at(1));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            // Trimming steps wizard filling
            using TrimmomaticAddSettings = QPair<TrimmomaticDialogFiller::TrimmomaticSteps, QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant>>;
            QList<TrimmomaticAddSettings> steps;
            QMap<TrimmomaticDialogFiller::TrimmomaticValues, QVariant> illuminaclip = {{TrimmomaticDialogFiller::TrimmomaticValues::AdapterSequence, dataDir + "adapters/illumina/TruSeq3-SE.fa"},
                                                                                       {TrimmomaticDialogFiller::TrimmomaticValues::SeedMismatches, "2"},
                                                                                       {TrimmomaticDialogFiller::TrimmomaticValues::PalindromeClipThreshold, "30"},
                                                                                       {TrimmomaticDialogFiller::TrimmomaticValues::SimpleClipThreshold, "10"}};
            steps.append(TrimmomaticAddSettings(TrimmomaticDialogFiller::TrimmomaticSteps::ILLUMINACLIP, illuminaclip));
            TrimmomaticDialogFiller *trimmomaticFiller = new TrimmomaticDialogFiller(os, steps);
            GTUtilsDialog::waitForDialog(os, trimmomaticFiller);

            const auto trimmProperty = GTWidget::findExactWidget<QToolButton *>(os, "trimmomaticPropertyToolButton");
            GTWidget::click(os, trimmProperty);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            // Reference fasta
            QLineEdit *leReference = GTWidget::findExactWidget<QLineEdit *>(os, "Reference genome widget");
            const QString ref = QFileInfo(testDir + "_common_data/cmdline/tuxedo_pipeline/data/test_0004/NC_010473.fa").absoluteFilePath();
            GTLineEdit::setText(os, leReference, ref);

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };
    const GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "RNA-Seq Analysis with TopHat and StringTie", new RnaSeqAnalysisWizardScenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "RNA-Seq data analysis...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os),
                  "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString(os));
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //1. Click Tools -> NGS data analysis -> Extract transcript sequences...
    //2. Click "Read Sequence(s)" element
    //3. Add "_common_data/cmdline/tuxedo_pipeline/data/index/chr6.fa" file to "Dataset 1" dataset
    //4. Click "Read Transcripts" element
    //5. Add "_common_data/cmdline/tuxedo_pipeline/data_to_compare_with/transcripts.gtf" file to "Dataset 1" dataset
    //6. Run workflow. Wait for workflow finished
    //Expected state: no errors
    const GTLogTracer lt;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools"
                                                << "NGS data analysis"
                                                << "Extract transcript sequences...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read Sequence(s)");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/tuxedo_pipeline/data/index/chr6.fa", true);

    GTUtilsWorkflowDesigner::click(os, "Read Transcripts");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/tuxedo_pipeline/data_to_compare_with/transcripts.gtf", true);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os),
                  "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString(os));
}

}    // namespace GUITest_common_scenarios_NIAID_pipelines
}    // namespace U2
