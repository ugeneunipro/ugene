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

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/ProjectView.h>
#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "ExternalToolSupportSettingsController.h"
#include "HmmerBuildDialog.h"
#include "HmmerSearchDialog.h"
#include "HmmerSupport.h"
#include "PhmmerSearchDialog.h"

namespace U2 {

const QString HmmerSupport::BUILD_TOOL = "HMMER build";
const QString HmmerSupport::BUILD_TOOL_ID = "USUPP_HMMBUILD";
const QString HmmerSupport::SEARCH_TOOL = "HMMER search";
const QString HmmerSupport::SEARCH_TOOL_ID = "USUPP_HMMSEARCH";
const QString HmmerSupport::PHMMER_TOOL = "PHMMER search";
const QString HmmerSupport::PHMMER_TOOL_ID = "USUPP_PHMMER";

HmmerSupport::HmmerSupport(const QString& id, const QString &name)
    : ExternalTool(id, name, "")
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "HMMER";
    versionRegExp = QRegExp("HMMER (\\d+.\\d+.\\d+\\w?)");

    if (id == BUILD_TOOL_ID) {
        initBuild();
    }

    if (id == SEARCH_TOOL_ID) {
        initSearch();
    }

    if (id == PHMMER_TOOL_ID) {
        initPhmmer();
    }
}

void HmmerSupport::sl_buildProfile() {
    if (!isToolSet(BUILD_TOOL)) {
        return;
    }

    MultipleSequenceAlignment ma;
    MWMDIWindow *activeWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    if (NULL != activeWindow) {
        GObjectViewWindow *objectViewWindow = qobject_cast<GObjectViewWindow *>(activeWindow);
        if (NULL != objectViewWindow) {
            MSAEditor *msaEditor = qobject_cast<MSAEditor *>(objectViewWindow->getObjectView());
            if (NULL != msaEditor) {
                MultipleSequenceAlignmentObject *maObj = msaEditor->getMaObject();
                if (maObj != NULL) {
                    ma = maObj->getMultipleAlignment();
                }
            }
        }
    }
    QWidget *parent = AppContext::getMainWindow()->getQMainWindow();

    QObjectScopedPointer<HmmerBuildDialog> buildDialog = new HmmerBuildDialog(ma, parent);
    buildDialog->exec();
}

namespace {

U2SequenceObject * getDnaSequenceObject() {
    U2SequenceObject *seqObj = NULL;
    GObjectViewWindow *activeWindow = qobject_cast<GObjectViewWindow *>(AppContext::getMainWindow()->getMDIManager()->getActiveWindow());
    if (NULL != activeWindow) {
        AnnotatedDNAView *dnaView = qobject_cast<AnnotatedDNAView *>(activeWindow->getObjectView());
        seqObj = (NULL != dnaView ? dnaView->getSequenceInFocus()->getSequenceObject() : NULL);
    }

    if (NULL == seqObj) {
        ProjectView *projectView = AppContext::getProjectView();
        if (NULL != projectView) {
            const GObjectSelection *objSelection = projectView->getGObjectSelection();
            GObject *obj = (objSelection->getSelectedObjects().size() == 1 ? objSelection->getSelectedObjects().first() : NULL);
            seqObj = qobject_cast<U2SequenceObject *>(obj);
        }
    }

    return seqObj;
}

}

void HmmerSupport::sl_search() {
    if (!isToolSet(SEARCH_TOOL)) {
        return;
    }

    U2SequenceObject *seqObj = getDnaSequenceObject();
    if (NULL == seqObj) {
        QMessageBox::critical(NULL, tr("Error!"), tr("Target sequence not selected: no opened annotated dna view"));
        return;
    }
    ADVSequenceObjectContext *seqCtx = NULL;
    GObjectViewWindow *activeWindow = qobject_cast<GObjectViewWindow *>(AppContext::getMainWindow()->getMDIManager()->getActiveWindow());
    if (NULL != activeWindow) {
        AnnotatedDNAView* dnaView = qobject_cast<AnnotatedDNAView *>(activeWindow->getObjectView());
        seqCtx = (dnaView != NULL) ? dnaView->getSequenceInFocus() : NULL;
    }

    QWidget *parent = AppContext::getMainWindow()->getQMainWindow();
    if(seqCtx != NULL){
        QObjectScopedPointer<HmmerSearchDialog> searchDlg = new HmmerSearchDialog(seqCtx, parent);
        searchDlg->exec();
    }else{
        QObjectScopedPointer<HmmerSearchDialog> searchDlg = new HmmerSearchDialog(seqObj, parent);
        searchDlg->exec();
    }
}

void HmmerSupport::sl_phmmerSearch() {
    if (!isToolSet(PHMMER_TOOL)) {
        return;
    }

    U2SequenceObject *seqObj = getDnaSequenceObject();
    if (NULL == seqObj) {
        QMessageBox::critical(NULL, tr("Error!"), tr("Target sequence not selected: no opened annotated dna view"));
        return;
    }
    ADVSequenceObjectContext *seqCtx = NULL;
    GObjectViewWindow *activeWindow = qobject_cast<GObjectViewWindow *>(AppContext::getMainWindow()->getMDIManager()->getActiveWindow());
    if (NULL != activeWindow) {
        AnnotatedDNAView* dnaView = qobject_cast<AnnotatedDNAView *>(activeWindow->getObjectView());
        seqCtx = (dnaView != NULL) ? dnaView->getSequenceInFocus() : NULL;
    }

    QWidget *parent = AppContext::getMainWindow()->getQMainWindow();
    if(seqCtx != NULL){
        QObjectScopedPointer<PhmmerSearchDialog> phmmerDialog = new PhmmerSearchDialog(seqCtx, parent);
        phmmerDialog->exec();
    }else{
        QObjectScopedPointer<PhmmerSearchDialog> phmmerDialog = new PhmmerSearchDialog(seqObj, parent);
        phmmerDialog->exec();
    }
}

void HmmerSupport::initBuild() {
#ifdef Q_OS_WIN
    executableFileName = "hmmbuild.exe";
#elif defined(Q_OS_UNIX)
    executableFileName = "hmmbuild";
#endif

    validationArguments << "-h";
    validMessage = "hmmbuild";
    description = tr("<i>HMMER build</i> constructs HMM profiles from multiple sequence alignments.");

    MainWindow *mainWindow = AppContext::getMainWindow();
    if (NULL != mainWindow) {
        QAction *buildAction = new QAction(tr("Build HMM3 profile..."), this);
        buildAction->setObjectName(ToolsMenu::HMMER_BUILD3);
        connect(buildAction, SIGNAL(triggered()), SLOT(sl_buildProfile()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, buildAction);
    }
}

void HmmerSupport::initSearch() {
#ifdef Q_OS_WIN
    executableFileName = "hmmsearch.exe";
#elif defined(Q_OS_UNIX)
    executableFileName = "hmmsearch";
#endif

    validationArguments << "-h";
    validMessage = "hmmsearch";
    description = tr("<i>HMMER search</i> searches profile(s) against a sequence database.");

    MainWindow *mainWindow = AppContext::getMainWindow();
    if (NULL != mainWindow) {
        QAction *searchAction = new QAction(tr("Search with HMMER3..."), this);
        searchAction->setObjectName(ToolsMenu::HMMER_SEARCH3);
        connect(searchAction, SIGNAL(triggered()), SLOT(sl_search()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, searchAction);
    }
}

void HmmerSupport::initPhmmer() {
#ifdef Q_OS_WIN
    executableFileName = "phmmer.exe";
#elif defined(Q_OS_UNIX)
    executableFileName = "phmmer";
#endif

    validationArguments << "-h";
    validMessage = "phmmer";
    description = tr("<i>PHMMER search</i> searches a protein sequence against a protein database.");

    MainWindow *mainWindow = AppContext::getMainWindow();
    if (NULL != mainWindow) {
        QAction *searchAction = new QAction(tr("Search with phmmer..."), this);
        searchAction->setObjectName(ToolsMenu::HMMER_SEARCH3P);
        connect(searchAction, SIGNAL(triggered()), SLOT(sl_phmmerSearch()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, searchAction);
    }
}

bool HmmerSupport::isToolSet(const QString &name) const {
    if (path.isEmpty()){
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle(name);
        msgBox->setText(tr("Path for %1 tool not selected.").arg(name));
        msgBox->setInformativeText(tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        switch (ret) {
           case QMessageBox::Yes:
               AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
               break;
           case QMessageBox::No:
               return false;
               break;
           default:
               assert(false);
               break;
         }
    }

    if (path.isEmpty()) {
        return false;
    }

    return true;
}

HmmerMsaEditorContext::HmmerMsaEditorContext(QObject *parent)
    : GObjectViewWindowContext(parent, MsaEditorFactory::ID)
{

}

void HmmerMsaEditorContext::initViewContext(GObjectView *view) {
    MSAEditor *msaEditor = qobject_cast<MSAEditor *>(view);
    SAFE_POINT(NULL != msaEditor, "Msa Editor is NULL", );
    CHECK(NULL != msaEditor->getMaObject(), );

    GObjectViewAction *action = new GObjectViewAction(this, view, tr("Build HMMER3 profile"));
    action->setObjectName("Build HMMER3 profile");
    action->setIcon(QIcon(":/external_tool_support/images/hmmer.png"));
    connect(action, SIGNAL(triggered()), SLOT(sl_build()));
    addViewAction(action);
}

void HmmerMsaEditorContext::buildMenu(GObjectView *view, QMenu *menu) {
    MSAEditor *msaEditor = qobject_cast<MSAEditor *>(view);
    SAFE_POINT(NULL != msaEditor, "Msa Editor is NULL", );
    SAFE_POINT(NULL != menu, "Menu is NULL", );
    CHECK(NULL != msaEditor->getMaObject(), );

    QList<GObjectViewAction *> list = getViewActions(view);
    SAFE_POINT(1 == list.size(), "List size is incorrect", );
    QMenu *advancedMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_ADVANCED);
    SAFE_POINT(advancedMenu != NULL, "menu 'Advanced' is NULL", );
    advancedMenu->addAction(list.first());
}

void HmmerMsaEditorContext::sl_build() {
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender());
    SAFE_POINT(NULL != action, "action is NULL", );
    MSAEditor *msaEditor = qobject_cast<MSAEditor *>(action->getObjectView());
    SAFE_POINT(NULL != msaEditor, "Msa Editor is NULL", );

    MultipleSequenceAlignmentObject *obj = msaEditor->getMaObject();
    if (obj != NULL) {
        QObjectScopedPointer<HmmerBuildDialog> buildDlg = new HmmerBuildDialog(obj->getMultipleAlignment  ());
        buildDlg->exec();
        CHECK(!buildDlg.isNull(), );
    }
}

HmmerAdvContext::HmmerAdvContext(QObject *parent) :
    GObjectViewWindowContext(parent, AnnotatedDNAViewFactory::ID) {

}

void HmmerAdvContext::initViewContext(GObjectView *view) {
    AnnotatedDNAView *adv = qobject_cast<AnnotatedDNAView *>(view);
    SAFE_POINT(NULL != adv, "AnnotatedDNAView is NULL", );

    ADVGlobalAction *searchAction = new ADVGlobalAction(adv, QIcon(":/external_tool_support/images/hmmer.png"), tr("Find HMM signals with HMMER3..."), 70);
    searchAction->setObjectName("Find HMM signals with HMMER3");
    connect(searchAction, SIGNAL(triggered()), SLOT(sl_search()));
}

#define MAX_HMMSEARCH_SEQUENCE_LENGTH_X86 (2*1024*1024)

void HmmerAdvContext::sl_search() {
    QWidget *parent = getParentWidget(sender());
    assert(NULL != parent);
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender());
    SAFE_POINT(NULL != action, "action is NULL", );
    AnnotatedDNAView *adv = qobject_cast<AnnotatedDNAView *>(action->getObjectView());
    SAFE_POINT(NULL != adv, "AnnotatedDNAView is NULL", );
    ADVSequenceObjectContext *seqCtx = adv->getSequenceInFocus();
    if (NULL == seqCtx) {
        QMessageBox::critical(parent, tr("Error"), tr("No sequence in focus found"));
        return;
    }

#ifdef Q_PROCESSOR_X86_32
    // do not show action on 32 bit platforms for large services
    quint64 seqLen = seqCtx->getSequenceLength();
    if (seqLen > MAX_HMMSEARCH_SEQUENCE_LENGTH_X86) {
        QMessageBox::critical(parent, tr("Error"), tr("Sequences larger 2Gb are not supported on 32-bit architecture."));
        return;
    }
#endif


    QObjectScopedPointer<HmmerSearchDialog> searchDlg = new HmmerSearchDialog(seqCtx, parent);
    searchDlg->exec();
}

QWidget * HmmerAdvContext::getParentWidget(QObject *sender) {
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender);
    SAFE_POINT(NULL != action, "action is NULL", NULL);
    AnnotatedDNAView *adv = qobject_cast<AnnotatedDNAView *>(action->getObjectView());
    SAFE_POINT(NULL != adv, "AnnotatedDNAView is NULL", NULL);

    if (adv->getWidget()) {
        return adv->getWidget();
    } else {
        return AppContext::getMainWindow()->getQMainWindow();
    }
}

U2SequenceObject * HmmerAdvContext::getSequenceInFocus(QObject *sender) {
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender);
    SAFE_POINT(NULL != action, "action is NULL", NULL);
    AnnotatedDNAView *adv = qobject_cast<AnnotatedDNAView *>(action->getObjectView());
    SAFE_POINT(NULL != adv, "AnnotatedDNAView is NULL", NULL);
    ADVSequenceObjectContext *seqCtx = adv->getSequenceInFocus();
    if (NULL == seqCtx) {
        return NULL;
    }
    return seqCtx->getSequenceObject();
}

HmmerContext::HmmerContext(QObject *parent) :
    QObject(parent),
    msaEditorContext(NULL),
    advContext(NULL)
{

}

void HmmerContext::init() {
    msaEditorContext = new HmmerMsaEditorContext(this);
    advContext = new HmmerAdvContext(this);

    msaEditorContext->init();
    advContext->init();
}

Hmmer3LogParser::Hmmer3LogParser() {

}

void Hmmer3LogParser::parseErrOutput(const QString& partOfLog) {
    lastPartOfLog = partOfLog.split(QRegExp("(\n|\r)"));
    lastPartOfLog.first() = lastErrLine + lastPartOfLog.first();
    lastErrLine = lastPartOfLog.takeLast();

    foreach(const QString &buf, lastPartOfLog) {
        if (!buf.isEmpty()) {
            algoLog.error("Hmmer3: " + buf);
            setLastError(buf);
        }
    }
}

}   // namespace U2
