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

#include "HmmerSupport.h"

#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

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
#include "PhmmerSearchDialog.h"

namespace U2 {

const QString HmmerSupport::BUILD_TOOL = "HMMER build";
const QString HmmerSupport::BUILD_TOOL_ID = "USUPP_HMMBUILD";
const QString HmmerSupport::SEARCH_TOOL = "HMMER search";
const QString HmmerSupport::SEARCH_TOOL_ID = "USUPP_HMMSEARCH";
const QString HmmerSupport::PHMMER_TOOL = "PHMMER search";
const QString HmmerSupport::PHMMER_TOOL_ID = "USUPP_PHMMER";

HmmerSupport::HmmerSupport(const QString& id, const QString& name)
    : ExternalTool(id, "hmmer3", name) {
    if (AppContext::getMainWindow() != nullptr) {
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
    MWMDIWindow* activeWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    if (activeWindow != nullptr) {
        auto objectViewWindow = qobject_cast<GObjectViewWindow*>(activeWindow);
        if (objectViewWindow != nullptr) {
            auto msaEditor = qobject_cast<MSAEditor*>(objectViewWindow->getObjectView());
            if (msaEditor != nullptr) {
                MultipleSequenceAlignmentObject* maObj = msaEditor->getMaObject();
                if (maObj != nullptr) {
                    ma = maObj->getMultipleAlignment();
                }
            }
        }
    }
    QWidget* parent = AppContext::getMainWindow()->getQMainWindow();

    QObjectScopedPointer<HmmerBuildDialog> buildDialog = new HmmerBuildDialog(ma, parent);
    buildDialog->exec();
}

namespace {

U2SequenceObject* getDnaSequenceObject() {
    U2SequenceObject* seqObj = nullptr;
    auto activeWindow = qobject_cast<GObjectViewWindow*>(AppContext::getMainWindow()->getMDIManager()->getActiveWindow());
    if (activeWindow != nullptr) {
        auto dnaView = qobject_cast<AnnotatedDNAView*>(activeWindow->getObjectView());
        seqObj = (dnaView != nullptr ? dnaView->getActiveSequenceContext()->getSequenceObject() : nullptr);
    }

    if (seqObj == nullptr) {
        ProjectView* projectView = AppContext::getProjectView();
        if (projectView != nullptr) {
            const GObjectSelection* objSelection = projectView->getGObjectSelection();
            GObject* obj = (objSelection->getSelectedObjects().size() == 1 ? objSelection->getSelectedObjects().first() : nullptr);
            seqObj = qobject_cast<U2SequenceObject*>(obj);
        }
    }

    return seqObj;
}

}  // namespace

void HmmerSupport::sl_search() {
    if (!isToolSet(SEARCH_TOOL)) {
        return;
    }

    U2SequenceObject* seqObj = getDnaSequenceObject();
    if (seqObj == nullptr) {
        QMessageBox::critical(nullptr, tr("Error!"), tr("Target sequence not selected: no opened annotated dna view"));
        return;
    }
    ADVSequenceObjectContext* seqCtx = nullptr;
    auto activeWindow = qobject_cast<GObjectViewWindow*>(AppContext::getMainWindow()->getMDIManager()->getActiveWindow());
    if (activeWindow != nullptr) {
        auto dnaView = qobject_cast<AnnotatedDNAView*>(activeWindow->getObjectView());
        seqCtx = (dnaView != nullptr) ? dnaView->getActiveSequenceContext() : nullptr;
    }

    QWidget* parent = AppContext::getMainWindow()->getQMainWindow();
    if (seqCtx != nullptr) {
        QObjectScopedPointer<HmmerSearchDialog> searchDlg = new HmmerSearchDialog(seqCtx, parent);
        searchDlg->exec();
    } else {
        QObjectScopedPointer<HmmerSearchDialog> searchDlg = new HmmerSearchDialog(seqObj, parent);
        searchDlg->exec();
    }
}

void HmmerSupport::sl_phmmerSearch() {
    if (!isToolSet(PHMMER_TOOL)) {
        return;
    }

    U2SequenceObject* seqObj = getDnaSequenceObject();
    if (seqObj == nullptr) {
        QMessageBox::critical(nullptr, tr("Error!"), tr("Target sequence not selected: no opened annotated dna view"));
        return;
    }
    ADVSequenceObjectContext* seqCtx = nullptr;
    auto activeWindow = qobject_cast<GObjectViewWindow*>(AppContext::getMainWindow()->getMDIManager()->getActiveWindow());
    if (activeWindow != nullptr) {
        auto dnaView = qobject_cast<AnnotatedDNAView*>(activeWindow->getObjectView());
        seqCtx = (dnaView != nullptr) ? dnaView->getActiveSequenceContext() : nullptr;
    }

    QWidget* parent = AppContext::getMainWindow()->getQMainWindow();
    if (seqCtx != nullptr) {
        QObjectScopedPointer<PhmmerSearchDialog> phmmerDialog = new PhmmerSearchDialog(seqCtx, parent);
        phmmerDialog->exec();
    } else {
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

    MainWindow* mainWindow = AppContext::getMainWindow();
    if (mainWindow != nullptr) {
        QAction* buildAction = new QAction(tr("Build HMM3 profile..."), this);
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

    MainWindow* mainWindow = AppContext::getMainWindow();
    if (mainWindow != nullptr) {
        QAction* searchAction = new QAction(tr("Search with HMMER3..."), this);
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

    MainWindow* mainWindow = AppContext::getMainWindow();
    if (mainWindow != nullptr) {
        QAction* searchAction = new QAction(tr("Search with phmmer..."), this);
        searchAction->setObjectName(ToolsMenu::HMMER_SEARCH3P);
        connect(searchAction, SIGNAL(triggered()), SLOT(sl_phmmerSearch()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, searchAction);
    }
}

bool HmmerSupport::isToolSet(const QString& name) const {
    if (path.isEmpty()) {
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
            default:
                assert(false);
        }
    }

    if (path.isEmpty()) {
        return false;
    }

    return true;
}

HmmerMsaEditorContext::HmmerMsaEditorContext(QObject* parent)
    : GObjectViewWindowContext(parent, MsaEditorFactory::ID) {
}

void HmmerMsaEditorContext::initViewContext(GObjectViewController* view) {
    auto msaEditor = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "Msa Editor is NULL", );
    CHECK(msaEditor->getMaObject() != nullptr, );

    GObjectViewAction* action = new GObjectViewAction(this, view, tr("Build HMMER3 profile"));
    action->setObjectName("Build HMMER3 profile");
    action->setIcon(QIcon(":/external_tool_support/images/hmmer.png"));
    connect(action, SIGNAL(triggered()), SLOT(sl_build()));
    addViewAction(action);
}

void HmmerMsaEditorContext::buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu) {
    auto msaEditor = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaEditor != nullptr, "Msa Editor is NULL", );
    SAFE_POINT(menu != nullptr, "Menu is NULL", );
    CHECK(nullptr != msaEditor->getMaObject(), );

    QList<GObjectViewAction*> list = getViewActions(view);
    SAFE_POINT(list.size() == 1, "List size is incorrect", );
    QMenu* advancedMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_ADVANCED);
    SAFE_POINT(advancedMenu != nullptr, "menu 'Advanced' is NULL", );
    advancedMenu->addAction(list.first());
}

void HmmerMsaEditorContext::sl_build() {
    auto action = qobject_cast<GObjectViewAction*>(sender());
    SAFE_POINT(action != nullptr, "action is NULL", );
    auto msaEditor = qobject_cast<MSAEditor*>(action->getObjectView());
    SAFE_POINT(msaEditor != nullptr, "Msa Editor is NULL", );

    MultipleSequenceAlignmentObject* obj = msaEditor->getMaObject();
    if (obj != nullptr) {
        QObjectScopedPointer<HmmerBuildDialog> buildDlg = new HmmerBuildDialog(obj->getMultipleAlignment());
        buildDlg->exec();
        CHECK(!buildDlg.isNull(), );
    }
}

HmmerAdvContext::HmmerAdvContext(QObject* parent)
    : GObjectViewWindowContext(parent, AnnotatedDNAViewFactory::ID) {
}

void HmmerAdvContext::initViewContext(GObjectViewController* view) {
    auto adv = qobject_cast<AnnotatedDNAView*>(view);
    SAFE_POINT(adv != nullptr, "AnnotatedDNAView is NULL", );

    ADVGlobalAction* searchAction = new ADVGlobalAction(adv, QIcon(":/external_tool_support/images/hmmer.png"), tr("Find HMM signals with HMMER3..."), 70);
    searchAction->setObjectName("Find HMM signals with HMMER3");
    connect(searchAction, SIGNAL(triggered()), SLOT(sl_search()));
}

void HmmerAdvContext::sl_search() {
    QWidget* parent = getParentWidget(sender());
    assert(parent != nullptr);
    auto action = qobject_cast<GObjectViewAction*>(sender());
    SAFE_POINT(action != nullptr, "action is NULL", );
    auto adv = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    SAFE_POINT(adv != nullptr, "AnnotatedDNAView is NULL", );
    ADVSequenceObjectContext* seqCtx = adv->getActiveSequenceContext();
    if (seqCtx == nullptr) {
        QMessageBox::critical(parent, tr("Error"), tr("No sequence in focus found"));
        return;
    }

    QObjectScopedPointer<HmmerSearchDialog> searchDlg = new HmmerSearchDialog(seqCtx, parent);
    searchDlg->exec();
}

QWidget* HmmerAdvContext::getParentWidget(QObject* sender) {
    auto action = qobject_cast<GObjectViewAction*>(sender);
    SAFE_POINT(action != nullptr, "action is NULL", nullptr);
    auto adv = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    SAFE_POINT(adv != nullptr, "AnnotatedDNAView is NULL", nullptr);

    return adv->getWidget() ? adv->getWidget() : AppContext::getMainWindow()->getQMainWindow();
}

U2SequenceObject* HmmerAdvContext::getSequenceInFocus(QObject* sender) {
    auto action = qobject_cast<GObjectViewAction*>(sender);
    SAFE_POINT(action != nullptr, "action is NULL", nullptr);
    auto adv = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    SAFE_POINT(adv != nullptr, "AnnotatedDNAView is NULL", nullptr);
    ADVSequenceObjectContext* seqCtx = adv->getActiveSequenceContext();
    if (seqCtx == nullptr) {
        return nullptr;
    }
    return seqCtx->getSequenceObject();
}

HmmerContext::HmmerContext(QObject* parent)
    : QObject(parent),
      msaEditorContext(nullptr),
      advContext(nullptr) {
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

    for (const QString& buf : qAsConst(lastPartOfLog)) {
        if (!buf.isEmpty()) {
            algoLog.error("Hmmer3: " + buf);
            setLastError(buf);
        }
    }
}

}  // namespace U2
