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

#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Task.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/ProjectView.h>
#include <U2Gui/ToolsMenu.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "HMMIO.h"
#include "HMMIOWorker.h"
#include "TaskLocalStorage.h"
#include "hmmer2/funcs.h"
#include "uHMMPlugin.h"
#include "u_build/HMMBuildDialogController.h"
#include "u_calibrate/HMMCalibrateDialogController.h"
#include "u_search/HMMSearchDialogController.h"
#include "u_search/HMMSearchQDActor.h"
#include "u_tests/uhmmerTests.h"

Q_DECLARE_METATYPE(QMenu *);

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
#ifdef HMMER_BUILD_WITH_SSE2
    bool runningOnSSEProc = AppResourcePool::isSSE2Enabled();
    if( !runningOnSSEProc ) {
        return 0;
    }
#endif
    uHMMPlugin * plug = new uHMMPlugin();
    return plug;
}

uHMMPlugin::uHMMPlugin() : Plugin(tr("HMM2"), tr("Based on HMMER 2.3.2 package. Biological sequence analysis using profile hidden Markov models")), ctxMSA(NULL), ctxADV(NULL)
{
    if (AppContext::getMainWindow()) {
        QAction* buildAction = new QAction(tr("Build HMM2 profile..."), this);
        buildAction->setObjectName(ToolsMenu::HMMER_BUILD2);
        connect(buildAction, SIGNAL(triggered()), SLOT(sl_build()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, buildAction);

        QAction* calibrateAction = new QAction(tr("Calibrate profile with HMMER2..."), this);
        calibrateAction->setObjectName(ToolsMenu::HMMER_CALIBRATE2);
        connect(calibrateAction, SIGNAL(triggered()), SLOT(sl_calibrate()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, calibrateAction);

        QAction* searchAction = new QAction(tr("Search with HMMER2..."), this);
        searchAction->setObjectName(ToolsMenu::HMMER_SEARCH2);
        connect(searchAction, SIGNAL(triggered()), SLOT(sl_search()));
        ToolsMenu::addAction(ToolsMenu::HMMER_MENU, searchAction);

        ctxMSA = new HMMMSAEditorContext(this);
        ctxMSA->init();

        ctxADV = new HMMADVContext(this);
        ctxADV->init();
    }
    LocalWorkflow::HMMLib::init();

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new HMM2QDActorPrototype());

    //uHMMER Tests
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = UHMMERTests::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        assert(res);
        Q_UNUSED(res);
    }
}

uHMMPlugin::~uHMMPlugin() {
    LocalWorkflow::HMMLib::cleanup();
}


void uHMMPlugin::sl_calibrate() {
    QWidget *p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<HMMCalibrateDialogController> d = new HMMCalibrateDialogController(p);
    d->exec();
}

void uHMMPlugin::sl_build() {
    MultipleSequenceAlignment ma = MultipleSequenceAlignment();

    //try to find alignment check that MSA Editor is active
    QString profileName;
    MWMDIWindow* w = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    if (w!=NULL) {
        GObjectViewWindow* ow = qobject_cast<GObjectViewWindow*>(w);
        if (ow!=NULL) {
            GObjectView* ov = ow->getObjectView();
            MSAEditor* av = qobject_cast<MSAEditor*>(ov);
            if (av!=NULL) {
                MultipleSequenceAlignmentObject* maObj = av->getMaObject();
                if (maObj!=NULL) {
                    ma = maObj->getMsaCopy();
                    profileName = maObj->getGObjectName() == MA_OBJECT_NAME ? maObj->getDocument()->getName() : maObj->getGObjectName();
                }
            }
        }
    }
    QWidget *p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<HMMBuildDialogController> d = new HMMBuildDialogController(profileName, ma, p);
    d->exec();
}

void uHMMPlugin::sl_search() {
    //to select a sequence
    //1. check that annotated DNA view is active
    //2. if not -> check that DNASequence object is selected in project view

    U2SequenceObject* obj = NULL;
    ADVSequenceObjectContext* seqCtx = NULL;

    MWMDIWindow* w = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    if (w!=NULL) {
        GObjectViewWindow* ow = qobject_cast<GObjectViewWindow*>(w);
        if (ow!=NULL) {
            GObjectView* ov = ow->getObjectView();
            AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(ov);
            if (av!=NULL) {
                seqCtx = av->getSequenceInFocus();
                obj = seqCtx->getSequenceObject();
            }
        }
    }

    if (obj == NULL) {
        ProjectView* pv = AppContext::getProjectView();
        if (pv!=NULL) {
            const GObjectSelection* sel = pv->getGObjectSelection();
            GObject* o = sel->getSelectedObjects().size() == 1 ? sel->getSelectedObjects().first() : NULL;
            obj = qobject_cast<U2SequenceObject*>(o);
        }
    }
    QWidget *p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    if (obj == NULL) {
        QMessageBox::critical(p, tr("Error"), tr("Error! Select sequence in Project view or open sequence view."));
        return;
    }
    if(seqCtx != NULL){
        QObjectScopedPointer<HMMSearchDialogController> d = new HMMSearchDialogController(seqCtx, p);
        d->exec();
    }else{
        QObjectScopedPointer<HMMSearchDialogController> d = new HMMSearchDialogController(obj, p);
        d->exec();
    }
}


//////////////////////////////////////////////////////////////////////////
// contexts

HMMMSAEditorContext::HMMMSAEditorContext(QObject* p) : GObjectViewWindowContext(p, MsaEditorFactory::ID) {
}


void HMMMSAEditorContext::initViewContext(GObjectView* view) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(view);
    SAFE_POINT(msaed != NULL, "Invalid GObjectView", );
    CHECK(msaed->getMaObject() != NULL, );

    GObjectViewAction* a = new GObjectViewAction(this, view, tr("Build HMMER2 profile"));
    a->setObjectName("Build HMMER2 profile");
    a->setIcon(QIcon(":/hmm2/images/hmmer_16.png"));
    connect(a, SIGNAL(triggered()), SLOT(sl_build()));
    addViewAction(a);
}

void HMMMSAEditorContext::buildMenu(GObjectView* v, QMenu* m) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    SAFE_POINT( NULL != msaed && NULL != m, "Invalid GObjectVeiw or QMenu", );
    CHECK(msaed->getMaObject() != NULL, );

    QList<GObjectViewAction*> list = getViewActions(v);
    assert(list.size()==1);
    GObjectViewAction* a = list.first();
    QMenu* aMenu = GUIUtils::findSubMenu(m, MSAE_MENU_ADVANCED);
    SAFE_POINT(aMenu != NULL, "aMenu", );
    aMenu->addAction(a);
}


void HMMMSAEditorContext::sl_build() {
    GObjectViewAction* action = qobject_cast<GObjectViewAction*>(sender());
    assert(action!=NULL);
    MSAEditor* ed = qobject_cast<MSAEditor*>(action->getObjectView());
    assert(ed!=NULL);
    MultipleSequenceAlignmentObject* obj = ed->getMaObject();
    if (obj) {
        QString profileName = obj->getGObjectName() == MA_OBJECT_NAME ? obj->getDocument()->getName() : obj->getGObjectName();
        QObjectScopedPointer<HMMBuildDialogController> d = new HMMBuildDialogController(profileName, obj->getMultipleAlignment());
        d->exec();
        CHECK(!d.isNull(), );
    }
}

HMMADVContext::HMMADVContext(QObject* p) : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {

}

void HMMADVContext::initViewContext(GObjectView* view) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(view);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":/hmm2/images/hmmer_16.png"), tr("Find HMM signals with HMMER2..."), 70);
    connect(a, SIGNAL(triggered()), SLOT(sl_search()));
}

void HMMADVContext::sl_search() {
    GObjectViewAction* action = qobject_cast<GObjectViewAction*>(sender());
    assert(action!=NULL);
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    assert(av!=NULL);
    QWidget *p;
    if (av->getWidget()){
        p = av->getWidget();
    }else{
        p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    }
    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    if(seqCtx == NULL) {
        QMessageBox::critical(p, tr("Error"), tr("No sequences found"));
        return;
    }
    QObjectScopedPointer<HMMSearchDialogController> d = new HMMSearchDialogController(seqCtx, p);
    d->exec();
}

}   // namespace U2
