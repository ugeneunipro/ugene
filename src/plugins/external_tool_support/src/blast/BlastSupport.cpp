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

#include "BlastSupport.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "AlignToReferenceBlastDialog.h"
#include "BlastCommonTask.h"
#include "BlastDBCmdDialog.h"
#include "BlastDBCmdTask.h"
#include "BlastNTask.h"
#include "BlastPTask.h"
#include "BlastRunDialog.h"
#include "BlastXTask.h"
#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "MakeBlastDbDialog.h"
#include "MakeBlastDbTask.h"
#include "RPSBlastTask.h"
#include "TBlastNTask.h"
#include "TBlastXTask.h"
#include "utils/ExternalToolSupportAction.h"
#include "utils/ExternalToolUtils.h"

namespace U2 {

const QString BlastSupport::ET_BLASTN_ID = "USUPP_BLASTN";
const QString BlastSupport::ET_BLASTP_ID = "USUPP_BLASTP";
const QString BlastSupport::ET_BLASTX_ID = "USUPP_BLASTX";
const QString BlastSupport::ET_TBLASTN_ID = "USUPP_TBLASTN";
const QString BlastSupport::ET_TBLASTX_ID = "USUPP_TBLASTX";
const QString BlastSupport::ET_RPSBLAST_ID = "USUPP_RPS_BLAST";
const QString BlastSupport::BLAST_TMP_DIR = "blast";
const QString BlastSupport::ET_BLASTDBCMD_ID = "USUPP_BLAST_DB_CMD";
const QString BlastSupport::ET_MAKEBLASTDB_ID = "USUPP_MAKE_BLAST_DB";

BlastSupport::BlastSupport(const QString& id)
    : ExternalTool(id, "blast", getProgramNameByToolId(id)) {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/ncbi.png");
        grayIcon = QIcon(":external_tool_support/images/ncbi_gray.png");
        warnIcon = QIcon(":external_tool_support/images/ncbi_warn.png");
    }
    validationArguments << "-h";

    if (id == ET_BLASTN_ID) {
        executableFileName = isOsWindows() ? "blastn.exe" : "blastn";
        validMessage = "Nucleotide-Nucleotide BLAST";
        description = tr("The <i>blastn</i> tool searches a nucleotide database using a nucleotide query.");
        versionRegExp = QRegExp("Nucleotide-Nucleotide BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_BLASTP_ID) {
        executableFileName = isOsWindows() ? "blastp.exe" : "blastp";
        validMessage = "Protein-Protein BLAST";
        description = tr("The <i>blastp</i> tool searches a protein database using a protein query.");
        versionRegExp = QRegExp("Protein-Protein BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_BLASTX_ID) {
        executableFileName = isOsWindows() ? "blastx.exe" : "blastx";
        validMessage = "Translated Query-Protein Subject";
        description = tr("The <i>blastx</i> tool searches a protein database using a translated nucleotide query.");
        versionRegExp = QRegExp("Translated Query-Protein Subject BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_TBLASTN_ID) {
        executableFileName = isOsWindows() ? "tblastn.exe" : "tblastn";
        validMessage = "Protein Query-Translated Subject";
        description = tr("The <i>tblastn</i> compares a protein query against a translated nucleotide database");
        versionRegExp = QRegExp("Protein Query-Translated Subject BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_TBLASTX_ID) {
        executableFileName = isOsWindows() ? "tblastx.exe" : "tblastx";
        validMessage = "Translated Query-Translated Subject";
        description = tr("The <i>tblastx</i> translates the query nucleotide sequence in all six possible frames and compares it against the six-frame translations of a nucleotide sequence database.");
        versionRegExp = QRegExp("Translated Query-Translated Subject BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_RPSBLAST_ID) {
        executableFileName = isOsWindows() ? "rpsblast.exe" : "rpsblast";
        validMessage = "Reverse Position Specific BLAST";
        description = "";
        versionRegExp = QRegExp("Reverse Position Specific BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_BLASTDBCMD_ID) {
        executableFileName = isOsWindows() ? "blastdbcmd.exe" : "blastdbcmd";
        validationArguments << "--help";
        validMessage = "blastdbcmd";
        description = tr("The <i>BlastDBCmd</i> fetches protein or nucleotide sequences from BLAST database based on a query.");
        versionRegExp = QRegExp("BLAST database client, version (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_MAKEBLASTDB_ID) {
        executableFileName = isOsWindows() ? "makeblastdb.exe" : "makeblastdb";
        validationArguments << "-help";
        validMessage = "makeblastdb";
        description = tr("The <i>makeblastdb</i> formats protein or nucleotide source databases before these databases can be searched by other BLAST tools.");
        versionRegExp = QRegExp("Application to create BLAST databases, version (\\d+\\.\\d+\\.\\d+\\+?)");
    } else {
        FAIL("Unsupported blast tool: " + id, );
    }
    toolKitName = "BLAST";
    pathChecks << ExternalTool::PathChecks::NonLatinArguments 
               << ExternalTool::PathChecks::SpacesArguments;
    if (isOsLinux() || isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinTemporaryDirPath
                   << ExternalTool::PathChecks::SpacesTemporaryDirPath;
    }
    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinToolPath;
    }
}

bool BlastSupport::checkBlastTool(const QString& toolId) {
    ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(toolId);
    SAFE_POINT(tool != nullptr, "Blast tool not found: " + toolId, false);
    CHECK(tool->getPath().isEmpty(), true);

    QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox();
    msgBox->setWindowTitle("BLAST toolbox");
    msgBox->setText(tr("Path for BLAST tools is not selected."));
    msgBox->setInformativeText(tr("Do you want to select it now?"));
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);
    int ret = msgBox->exec();
    CHECK(!msgBox.isNull() && ret == QMessageBox::Yes, false);
    AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);

    return !tool->getPath().isEmpty();
}

void BlastSupport::sl_runBlastSearch() {
    // Call select input file and setup settings dialog
    QMainWindow* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<BlastWithExtFileRunDialog> blastRunDialog = new BlastWithExtFileRunDialog(mainWindow);
    blastRunDialog->exec();
    CHECK(!blastRunDialog.isNull() && blastRunDialog->result() == QDialog::Accepted, );

    QList<BlastTaskSettings> settingsList = blastRunDialog->getSettingsList();
    auto blastMultiTask = new BlastMultiTask(settingsList, settingsList[0].outputResFile);
    AppContext::getTaskScheduler()->registerTopLevelTask(blastMultiTask);
}

void BlastSupport::sl_runBlastDbCmd() {
    // Call select input file and setup settings dialog.
    QObjectScopedPointer<BlastDBCmdDialog> blastDBCmdDialog = new BlastDBCmdDialog(AppContext::getMainWindow()->getQMainWindow());
    blastDBCmdDialog->exec();
    CHECK(!blastDBCmdDialog.isNull() && blastDBCmdDialog->result() == QDialog::Accepted, );

    checkBlastTool(id);
    AppContext::getTaskScheduler()->registerTopLevelTask(new BlastDBCmdTask(blastDBCmdDialog->getTaskSettings()));
}

void BlastSupport::sl_runAlignToReference() {
    QObjectScopedPointer<AlignToReferenceBlastDialog> dlg = new AlignToReferenceBlastDialog(AppContext::getMainWindow()->getQMainWindow());
    dlg->exec();
    CHECK(!dlg.isNull() && dlg->result() == QDialog::Accepted, );

    // Check that tool is valid before run.
    CHECK(checkBlastTool(ET_BLASTN_ID) && checkBlastTool(ET_MAKEBLASTDB_ID), );
    CHECK(ExternalToolSupportSettings::checkTemporaryDir(), );

    AlignToReferenceBlastCmdlineTask::Settings settings = dlg->getSettings();
    AppContext::getTaskScheduler()->registerTopLevelTask(new AlignToReferenceBlastCmdlineTask(settings));
}

void BlastSupport::sl_runMakeBlastDb() {
    // Call select input file and setup settings dialog
    QObjectScopedPointer<MakeBlastDbDialog> makeBlastDbDialog = new MakeBlastDbDialog(AppContext::getMainWindow()->getQMainWindow());
    makeBlastDbDialog->exec();
    CHECK(!makeBlastDbDialog.isNull() && makeBlastDbDialog->result() == QDialog::Accepted, );

    CHECK(checkBlastTool(ET_MAKEBLASTDB_ID), );
    CHECK(ExternalToolSupportSettings::checkTemporaryDir(), );

    AppContext::getTaskScheduler()->registerTopLevelTask(new MakeBlastDbTask(makeBlastDbDialog->getTaskSettings()));
}

QString BlastSupport::getToolIdByProgramName(const QString& programName) {
    QString toolId = programName == "blastn"        ? ET_BLASTN_ID
                     : programName == "blastp"      ? ET_BLASTP_ID
                     : programName == "blastx"      ? ET_BLASTX_ID
                     : programName == "tblastn"     ? ET_TBLASTN_ID
                     : programName == "tblastx"     ? ET_TBLASTX_ID
                     : programName == "rpsblast"    ? ET_RPSBLAST_ID
                     : programName == "blastdbcmd"  ? ET_BLASTDBCMD_ID
                     : programName == "makeblastdb" ? ET_MAKEBLASTDB_ID
                                                    : "";
    SAFE_POINT(!toolId.isEmpty(), "Unsupported blast program name: " + programName, "");
    return toolId;
}

QString BlastSupport::getProgramNameByToolId(const QString& toolId) {
    QString programName = toolId == ET_BLASTN_ID        ? "blastn"
                          : toolId == ET_BLASTP_ID      ? "blastp"
                          : toolId == ET_BLASTX_ID      ? "blastx"
                          : toolId == ET_TBLASTN_ID     ? "tblastn"
                          : toolId == ET_TBLASTX_ID     ? "tblastx"
                          : toolId == ET_RPSBLAST_ID    ? "rpsblast"
                          : toolId == ET_BLASTDBCMD_ID  ? "blastdbcmd"
                          : toolId == ET_MAKEBLASTDB_ID ? "makeblastdb"
                                                        : "";
    SAFE_POINT(!programName.isEmpty(), "Unsupported blast tool: " + toolId, "");
    return programName;
}

////////////////////////////////////////
// BlastSupportContext

#define BLAST_ANNOTATION_NAME "blast result"

BlastSupportContext::BlastSupportContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
    searchToolIds = QStringList({BlastSupport::ET_BLASTN_ID,
                                 BlastSupport::ET_BLASTP_ID,
                                 BlastSupport::ET_BLASTX_ID,
                                 BlastSupport::ET_TBLASTN_ID,
                                 BlastSupport::ET_TBLASTX_ID,
                                 BlastSupport::ET_RPSBLAST_ID});

    fetchSequenceByIdAction = new QAction(tr("Fetch sequences by 'id'"), this);
    fetchSequenceByIdAction->setObjectName("fetchSequenceById");
    connect(fetchSequenceByIdAction, SIGNAL(triggered()), SLOT(sl_fetchSequenceById()));
}

void BlastSupportContext::initViewContext(GObjectViewController* view) {
    SAFE_POINT(qobject_cast<AnnotatedDNAView*>(view) != nullptr, "Object view is not an AnnotatedDNAView", );

    auto queryAction = new ExternalToolSupportAction(this, view, tr("Query with local BLAST..."), 2000, searchToolIds);
    queryAction->setObjectName("query_with_blast");
    addViewAction(queryAction);
    connect(queryAction, SIGNAL(triggered()), SLOT(sl_showDialog()));
}

/**  Returns comma-separated sequence ids from the list of annotations.  */
static QString getCommaSeparatedIdsFromAnnotations(const QList<Annotation*>& annotations) {
    QStringList idList;
    for (const Annotation* ann : qAsConst(annotations)) {
        QString value = ann->findFirstQualifierValue("id");
        if (!value.isEmpty() && !idList.contains(value)) {
            idList.append(value);
        }
    }
    return idList.join(",");
}

void BlastSupportContext::buildStaticOrContextMenu(GObjectViewController* view, QMenu* m) {
    auto dnaView = qobject_cast<AnnotatedDNAView*>(view);
    CHECK(dnaView != nullptr, );

    QList<GObjectViewAction*> actions = getViewActions(view);
    QMenu* analyseMenu = GUIUtils::findSubMenu(m, ADV_MENU_ANALYSE);
    SAFE_POINT(analyseMenu != nullptr, "analyseMenu", );
    for (GObjectViewAction* a : qAsConst(actions)) {
        a->addToMenuWithOrder(analyseMenu);
    }

    QList<Annotation*> selectedAnnotations = dnaView->getAnnotationsSelection()->getAnnotations();
    CHECK(!selectedAnnotations.isEmpty(), );

    bool isBlastResultSelected = std::all_of(selectedAnnotations.begin(), selectedAnnotations.end(), [](auto& a) { return a->getName() == BLAST_ANNOTATION_NAME; });
    CHECK(isBlastResultSelected, );

    commaSeparatedSelectedSequenceIds = getCommaSeparatedIdsFromAnnotations(selectedAnnotations);
    CHECK(!commaSeparatedSelectedSequenceIds.isEmpty(), );

    fetchSequenceByIdAction->setText(tr("Fetch sequences by 'id' %1").arg(BLAST_ANNOTATION_NAME));

    auto fetchMenu = new QMenu(tr("Fetch sequences from local BLAST database"));
    fetchMenu->menuAction()->setObjectName("fetchMenu");
    fetchMenu->addAction(fetchSequenceByIdAction);

    auto exportMenu = GUIUtils::findSubMenu(m, ADV_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "exportMenu", );
    m->insertMenu(exportMenu->menuAction(), fetchMenu);
}

void BlastSupportContext::sl_showDialog() {
    auto viewAction = qobject_cast<GObjectViewAction*>(sender());
    SAFE_POINT(viewAction != nullptr, "Not a GObjectViewAction!", );

    auto view = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    SAFE_POINT(view != nullptr, "Not an AnnotatedDNAView!", );

    ADVSequenceObjectContext* seqCtx = view->getActiveSequenceContext();
    QObjectScopedPointer<BlastRunDialog> dlg = new BlastRunDialog(seqCtx, view->getWidget());
    dlg->exec();
    CHECK(!dlg.isNull() && dlg->result() == QDialog::Accepted, );

    BlastTaskSettings settings = dlg->getSettings();
    U2Region region = dlg->getSelectedRegion();

    U2OpStatus2Log os(LogLevel_DETAILS);
    settings.querySequences = {seqCtx->getSequenceData(region, os)};
    CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );

    settings.resultRegionOffset = region.startPos;
    SAFE_POINT(seqCtx->getSequenceObject() != nullptr, tr("Sequence object is NULL"), );

    settings.isSequenceCircular = seqCtx->getSequenceObject()->isCircular();
    settings.querySequenceObject = seqCtx->getSequenceObject();

    QString toolId = BlastSupport::getToolIdByProgramName(settings.programName);
    CHECK(BlastSupport::checkBlastTool(toolId), );
    CHECK(ExternalToolSupportSettings::checkTemporaryDir(), );

    auto task = settings.programName == "blastn"     ? new BlastNTask(settings)
                : settings.programName == "blastp"   ? new BlastPTask(settings)
                : settings.programName == "blastx"   ? new BlastXTask(settings)
                : settings.programName == "tblastn"  ? new TBlastNTask(settings)
                : settings.programName == "tblastx"  ? new TBlastXTask(settings)
                : settings.programName == "rpsblast" ? new RPSBlastTask(settings)
                                                     : (Task*)nullptr;
    SAFE_POINT(task != nullptr, "Unsupported blast program name: " + settings.programName, );
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void BlastSupportContext::sl_fetchSequenceById() {
    CHECK(BlastSupport::checkBlastTool({BlastSupport::ET_BLASTDBCMD_ID}), );

    QObjectScopedPointer<BlastDBCmdDialog> blastDBCmdDialog = new BlastDBCmdDialog(AppContext::getMainWindow()->getQMainWindow());
    blastDBCmdDialog->setQueryId(commaSeparatedSelectedSequenceIds);
    blastDBCmdDialog->exec();

    CHECK(!blastDBCmdDialog.isNull() && blastDBCmdDialog->result() == QDialog::Accepted, );
    AppContext::getTaskScheduler()->registerTopLevelTask(new BlastDBCmdTask(blastDBCmdDialog->getTaskSettings()));
}

}  // namespace U2
