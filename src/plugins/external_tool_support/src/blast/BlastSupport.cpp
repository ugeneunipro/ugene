/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include "BlastDBCmdDialog.h"
#include "BlastDBCmdSupport.h"
#include "BlastDBCmdTask.h"
#include "BlastNTask.h"
#include "BlastPTask.h"
#include "BlastCommonTask.h"
#include "BlastRunDialog.h"
#include "BlastXTask.h"
#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "RPSBlastTask.h"
#include "TBlastNTask.h"
#include "TBlastXTask.h"
#include "blast/FormatDBSupport.h"
#include "utils/ExternalToolSupportAction.h"
#include "utils/ExternalToolUtils.h"

namespace U2 {

const QString BlastSupport::ET_BLASTN = "BlastN";
const QString BlastSupport::ET_BLASTN_ID = "USUPP_BLASTN";
const QString BlastSupport::ET_BLASTP = "BlastP";
const QString BlastSupport::ET_BLASTP_ID = "USUPP_BLASTP";
const QString BlastSupport::ET_GPU_BLASTP_ID = "UGENE_GPU_BLASTP";
const QString BlastSupport::ET_BLASTX = "BlastX";
const QString BlastSupport::ET_BLASTX_ID = "USUPP_BLASTX";
const QString BlastSupport::ET_TBLASTN = "TBlastN";
const QString BlastSupport::ET_TBLASTN_ID = "USUPP_TBLASTN";
const QString BlastSupport::ET_TBLASTX = "TBlastX";
const QString BlastSupport::ET_TBLASTX_ID = "USUPP_TBLASTX";
const QString BlastSupport::ET_RPSBLAST = "RPSBlast";
const QString BlastSupport::ET_RPSBLAST_ID = "USUPP_RPS_BLAST";
const QString BlastSupport::BLAST_TMP_DIR = "blast";

BlastSupport::BlastSupport(const QString &id, const QString &name, const QString &path)
    : ExternalTool(id, "blast", name, path) {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/ncbi.png");
        grayIcon = QIcon(":external_tool_support/images/ncbi_gray.png");
        warnIcon = QIcon(":external_tool_support/images/ncbi_warn.png");
    }
    validationArguments << "-h";

    if (id == ET_BLASTN_ID) {
#ifdef Q_OS_WIN
        executableFileName = "blastn.exe";
#else
#    if defined(Q_OS_UNIX)
        executableFileName = "blastn";
#    endif
#endif
        validMessage = "Nucleotide-Nucleotide BLAST";
        description = "The <i>blastn</i> tool searches a nucleotide database \
                using a nucleotide query.";
        versionRegExp = QRegExp("Nucleotide-Nucleotide BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_BLASTP_ID) {
#ifdef Q_OS_WIN
        executableFileName = "blastp.exe";
#else
#    if defined(Q_OS_UNIX)
        executableFileName = "blastp";
#    endif
#endif
        validMessage = "Protein-Protein BLAST";
        description = "The <i>blastp</i> tool searches a protein database \
                using a protein query.";
        versionRegExp = QRegExp("Protein-Protein BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
        // https://ugene.net/tracker/browse/UGENE-945
        //     }else if(name == GPU_BLASTP_TOOL_NAME) {
        // #ifdef Q_OS_WIN
        //     executableFileName="blastp.exe";
        // #else
        //     #ifdef Q_OS_UNIX
        //     executableFileName="blastp";
        //     #endif
        // #endif
        //     validMessage="[-gpu boolean]";
        //     description="The <i>blastp</i> tool searches a protein database using a protein query.";
        //     versionRegExp=QRegExp("Protein-Protein BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_BLASTX_ID) {
#ifdef Q_OS_WIN
        executableFileName = "blastx.exe";
#else
#    if defined(Q_OS_UNIX)
        executableFileName = "blastx";
#    endif
#endif
        validMessage = "Translated Query-Protein Subject";
        description = "The <i>blastx</i> tool searches a protein database \
                using a translated nucleotide query.";
        versionRegExp = QRegExp("Translated Query-Protein Subject BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_TBLASTN_ID) {
#ifdef Q_OS_WIN
        executableFileName = "tblastn.exe";
#else
#    if defined(Q_OS_UNIX)
        executableFileName = "tblastn";
#    endif
#endif
        validMessage = "Protein Query-Translated Subject";
        description = "The <i>tblastn</i> compares a protein query against \
                a translated nucleotide database";
        versionRegExp = QRegExp("Protein Query-Translated Subject BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_TBLASTX_ID) {
#ifdef Q_OS_WIN
        executableFileName = "tblastx.exe";
#else
#    if defined(Q_OS_UNIX)
        executableFileName = "tblastx";
#    endif
#endif
        validMessage = "Translated Query-Translated Subject";
        description = "The <i>tblastx</i> translates the query nucleotide \
                sequence in all six possible frames and compares it \
                against the six-frame translations of a nucleotide \
                sequence database.";
        versionRegExp = QRegExp("Translated Query-Translated Subject BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    } else if (id == ET_RPSBLAST_ID) {
#ifdef Q_OS_WIN
        executableFileName = "rpsblast.exe";
#else
#    if defined(Q_OS_UNIX)
        executableFileName = "rpsblast";
#    endif
#endif
        validMessage = "Reverse Position Specific BLAST";
        description = "";
        versionRegExp = QRegExp("Reverse Position Specific BLAST (\\d+\\.\\d+\\.\\d+\\+?)");
    }
    toolKitName = "BLAST";
    lastDBName = "";
    lastDBPath = "";
}

void BlastSupport::sl_runWithExtFileSpecify() {
    //Check that blastal and tempory folder path defined
    QStringList toolList;
    toolList << ET_BLASTN_ID << ET_BLASTP_ID << ET_BLASTX_ID << ET_TBLASTN_ID << ET_TBLASTX_ID << ET_RPSBLAST_ID;
    bool isOneOfToolConfigured = false;
    foreach (QString id, toolList) {
        if (!AppContext::getExternalToolRegistry()->getById(id)->getPath().isEmpty()) {
            isOneOfToolConfigured = true;
        }
    }
    if (!isOneOfToolConfigured) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("BLAST Search");
        msgBox->setText(tr("Path for BLAST tools not selected."));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), );

        switch (ret) {
            case QMessageBox::Yes:
                AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
                break;
            case QMessageBox::No:
                return;
            default:
                assert(false);
        }
        for (const QString &id : qAsConst(toolList)) {
            if (!AppContext::getExternalToolRegistry()->getById(id)->getPath().isEmpty()) {
                isOneOfToolConfigured = true;
                break;
            }
        }
        if (!isOneOfToolConfigured) {
            return;
        }
    }
    U2OpStatus2Log os;
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    //Call select input file and setup settings dialog
    QObjectScopedPointer<BlastWithExtFileSpecifySupportRunDialog> blastRunDialog = new BlastWithExtFileSpecifySupportRunDialog(lastDBPath, lastDBName, AppContext::getMainWindow()->getQMainWindow());
    blastRunDialog->exec();
    CHECK(!blastRunDialog.isNull(), );

    if (blastRunDialog->result() != QDialog::Accepted) {
        return;
    }
    QList<BlastTaskSettings> settingsList = blastRunDialog->getSettingsList();
    auto blastMultiTask = new BlastMultiTask(settingsList, settingsList[0].outputResFile);
    AppContext::getTaskScheduler()->registerTopLevelTask(blastMultiTask);
}

void BlastSupport::sl_runAlign() {
    ExternalToolUtils::checkExtToolsPath(QStringList() << ET_BLASTN_ID << FormatDBSupport::ET_MAKEBLASTDB_ID);

    if (AppContext::getExternalToolRegistry()->getById(ET_BLASTN_ID)->getPath().isEmpty() || AppContext::getExternalToolRegistry()->getById(FormatDBSupport::ET_MAKEBLASTDB_ID)->getPath().isEmpty()) {
        return;
    }

    QObjectScopedPointer<AlignToReferenceBlastDialog> dlg = new AlignToReferenceBlastDialog(AppContext::getMainWindow()->getQMainWindow());
    dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(dlg->result() == QDialog::Accepted, );

    AlignToReferenceBlastCmdlineTask::Settings settings = dlg->getSettings();
    AlignToReferenceBlastCmdlineTask *task = new AlignToReferenceBlastCmdlineTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

////////////////////////////////////////
//BlastSupportContext

#define BLAST_ANNOTATION_NAME "blast result"

BlastSupportContext::BlastSupportContext(QObject *p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
    toolIdList << BlastSupport::ET_BLASTN_ID << BlastSupport::ET_BLASTP_ID << BlastSupport::ET_BLASTX_ID << BlastSupport::ET_TBLASTN_ID << BlastSupport::ET_TBLASTX_ID << BlastSupport::ET_RPSBLAST_ID;
    lastDBName = "";
    lastDBPath = "";

    fetchSequenceByIdAction = new QAction(tr("Fetch sequences by 'id'"), this);
    fetchSequenceByIdAction->setObjectName("fetchSequenceById");
    connect(fetchSequenceByIdAction, SIGNAL(triggered()), SLOT(sl_fetchSequenceById()));
}

void BlastSupportContext::initViewContext(GObjectView *view) {
    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(view);
    assert(av != nullptr);
    Q_UNUSED(av);

    ExternalToolSupportAction *queryAction = new ExternalToolSupportAction(this, view, tr("Query with local BLAST..."), 2000, toolIdList);
    queryAction->setObjectName("query_with_blast");

    addViewAction(queryAction);
    connect(queryAction, SIGNAL(triggered()), SLOT(sl_showDialog()));
}

static void setActionFontItalic(QAction *action, bool italic) {
    QFont font = action->font();
    font.setItalic(italic);
    action->setFont(font);
}

void BlastSupportContext::buildStaticOrContextMenu(GObjectView *view, QMenu *m) {
    QList<GObjectViewAction *> actions = getViewActions(view);
    QMenu *analyseMenu = GUIUtils::findSubMenu(m, ADV_MENU_ANALYSE);
    SAFE_POINT(analyseMenu != nullptr, "analyseMenu", );
    foreach (GObjectViewAction *a, actions) {
        a->addToMenuWithOrder(analyseMenu);
    }

    AnnotatedDNAView *dnaView = qobject_cast<AnnotatedDNAView *>(view);
    if (!dnaView) {
        return;
    }

    QString name;
    if (!dnaView->getAnnotationsSelection()->getAnnotations().isEmpty()) {
        name = dnaView->getAnnotationsSelection()->getAnnotations().first()->getName();
    }
    selectedId = ADVSelectionUtils::getSequenceIdsFromSelection(dnaView->getAnnotationsSelection()->getAnnotations(), true);
    bool isShowId = !selectedId.isEmpty();

    bool isBlastResult = false;
    for (const Annotation *annotation : qAsConst(dnaView->getAnnotationsSelection()->getAnnotations())) {
        if (name != annotation->getName()) {
            name = "";
        }
        isBlastResult = name == BLAST_ANNOTATION_NAME;
    }

    if (isShowId && isBlastResult) {
        name = name.isEmpty() ? "" : "from '" + name + "'";
        QMenu *fetchMenu = new QMenu(tr("Fetch sequences from local BLAST database"));
        fetchMenu->menuAction()->setObjectName("fetchMenu");
        QMenu *exportMenu = GUIUtils::findSubMenu(m, ADV_MENU_EXPORT);
        SAFE_POINT(exportMenu != nullptr, "exportMenu", );
        m->insertMenu(exportMenu->menuAction(), fetchMenu);
        fetchSequenceByIdAction->setText(tr("Fetch sequences by 'id' %1").arg(name));
        bool emptyToolPath = AppContext::getExternalToolRegistry()->getById(BlastDbCmdSupport::ET_BLASTDBCMD_ID)->getPath().isEmpty();
        setActionFontItalic(fetchSequenceByIdAction, emptyToolPath);
        fetchMenu->addAction(fetchSequenceByIdAction);
    }
}

void BlastSupportContext::sl_showDialog() {
    //Check that any of BLAST tools and temporary folder path defined
    bool isOneOfToolConfigured = false;
    for (const QString &id : qAsConst(toolIdList)) {
        if (!AppContext::getExternalToolRegistry()->getById(id)->getPath().isEmpty()) {
            isOneOfToolConfigured = true;
            break;
        }
    }
    if (!isOneOfToolConfigured) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("BLAST Search");
        msgBox->setText(tr("Path for BLAST tools not selected."));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), );

        switch (ret) {
            case QMessageBox::Yes:
                AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
                break;
            case QMessageBox::No:
                return;
            default:
                assert(false);
        }
        for (const QString &id : qAsConst(toolIdList)) {
            if (!AppContext::getExternalToolRegistry()->getById(id)->getPath().isEmpty()) {
                isOneOfToolConfigured = true;
                break;
            }
        }
        if (!isOneOfToolConfigured) {
            return;
        }
    }

    U2OpStatus2Log os(LogLevel_DETAILS);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    CHECK_OP(os, );

    QAction *a = (QAction *)sender();
    GObjectViewAction *viewAction = qobject_cast<GObjectViewAction *>(a);
    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(viewAction->getObjectView());
    assert(av);

    ADVSequenceObjectContext *seqCtx = av->getActiveSequenceContext();
    QObjectScopedPointer<BlastRunDialog> dlg = new BlastRunDialog(seqCtx, lastDBPath, lastDBName, av->getWidget());
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        BlastTaskSettings settings = dlg->getSettings();
        U2Region region = dlg->getSelectedRegion();
        settings.querySequence = seqCtx->getSequenceData(region, os);
        CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );
        settings.offsInGlobalSeq = region.startPos;
        SAFE_POINT(seqCtx->getSequenceObject() != nullptr, tr("Sequence object is NULL"), );
        settings.isSequenceCircular = seqCtx->getSequenceObject()->isCircular();
        settings.querySequenceObject = seqCtx->getSequenceObject();
        Task *t = nullptr;
        if (settings.programName == "blastn") {
            t = new BlastNTask(settings);
        } else if (settings.programName == "blastp") {
            t = new BlastPTask(settings);
        } else if (settings.programName == "blastx") {
            t = new BlastXTask(settings);
        } else if (settings.programName == "tblastn") {
            t = new TBlastNTask(settings);
        } else if (settings.programName == "tblastx") {
            t = new TBlastXTask(settings);
        } else if (settings.programName == "rpsblast") {
            t = new RPSBlastTask(settings);
        }
        assert(t);
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    }
}

void BlastSupportContext::sl_fetchSequenceById() {
    if (AppContext::getExternalToolRegistry()->getById(BlastDbCmdSupport::ET_BLASTDBCMD_ID)->getPath().isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle("BLAST " + QString(BlastDbCmdSupport::ET_BLASTDBCMD));
        msgBox->setText(tr("Path for BLAST %1 tool not selected.").arg(BlastDbCmdSupport::ET_BLASTDBCMD));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), );

        switch (ret) {
            case QMessageBox::Yes:
                AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
                break;
            case QMessageBox::No:
                return;
            default:
                assert(false);
        }
    }

    BlastDBCmdSupportTaskSettings settings;
    QObjectScopedPointer<BlastDBCmdDialog> blastDBCmdDialog = new BlastDBCmdDialog(settings, AppContext::getMainWindow()->getQMainWindow());
    blastDBCmdDialog->setQueryId(selectedId);
    blastDBCmdDialog->exec();
    CHECK(!blastDBCmdDialog.isNull(), );

    if (blastDBCmdDialog->result() != QDialog::Accepted) {
        return;
    }

    BlastDBCmdTask *blastDBCmdSupportTask = new BlastDBCmdTask(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(blastDBCmdSupportTask);
}

}    // namespace U2
