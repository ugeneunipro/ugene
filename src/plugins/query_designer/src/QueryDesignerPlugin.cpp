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

#include <U2Core/AppContext.h>
#include <U2Core/GAutoDeleteList.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/ToolsMenu.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "QDDocumentFormat.h"
#include "QDRunDialog.h"
#include "QDSamples.h"
#include "QDSceneIOTasks.h"
#include "QDTests.h"
#include "QDWorker.h"
#include "QueryDesignerPlugin.h"
#include "QueryViewController.h"
#include "library/QDFindActor.h"
#include "library/QDFindPolyRegionsActor.h"
#include "library/QDGcContentActor.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    QueryDesignerPlugin * plug = new QueryDesignerPlugin();
    return plug;
}

QueryDesignerPlugin::QueryDesignerPlugin()
    : Plugin(tr("Query Designer"),
             tr("Analyzes a nucleotide sequence using different algorithms (Repeat finder,"
                " ORF finder, etc.) imposing constraints on the positional relationship"
                " of the results.")) {
    if (AppContext::getMainWindow()) {
        services << new QueryDesignerService();
        viewCtx = new QueryDesignerViewContext(this);
        viewCtx->init();
        AppContext::getObjectViewFactoryRegistry()
            ->registerGObjectViewFactory(new QDViewFactory(this));
    }
    registerLibFactories();
    AppContext::getDocumentFormatRegistry()->registerFormat(new QDDocFormat(this));

    QString defaultDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/query_samples";

    if (LastUsedDirHelper::getLastUsedDir(QUERY_DESIGNER_ID).isEmpty()) {
        LastUsedDirHelper::setLastUsedDir(defaultDir, QUERY_DESIGNER_ID);
    }

    //tests
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = QDTests::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        assert(res); Q_UNUSED(res);
    }

    LocalWorkflow::QDWorkerFactory::init();
}

void QueryDesignerPlugin::registerLibFactories() {
    AppContext::getQDActorProtoRegistry()->registerProto(new QDFindActorPrototype());
    AppContext::getQDActorProtoRegistry()->registerProto(new QDFindPolyActorPrototype());
    AppContext::getQDActorProtoRegistry()->registerProto(new QDFindGcActorPrototype());
}

QueryDesignerViewContext::QueryDesignerViewContext(QObject *p)
: GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {}

void QueryDesignerViewContext::initViewContext(GObjectView* view) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(view);
    ADVGlobalAction* a = new ADVGlobalAction(av,
        QIcon(":query_designer/images/query_designer.png"),
        tr("Analyze with query schema..."), 50, ADVGlobalActionFlag_AddToAnalyseMenu);
    connect(a, SIGNAL(triggered()), SLOT(sl_showDialog()));
}

void QueryDesignerViewContext::sl_showDialog() {
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(sender());
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    assert(av);
    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    QObjectScopedPointer<QDDialog> d = new QDDialog(seqCtx);
    d->exec();
}

class CloseDesignerTask : public Task {
public:
    CloseDesignerTask(QueryDesignerService* s) :
      Task(U2::QueryDesignerPlugin::tr("Close Designer"), TaskFlag_NoRun),
          service(s) {}
      virtual void prepare();
private:
    QueryDesignerService* service;
};

void CloseDesignerTask::prepare() {
    if (!service->closeViews()) {
        stateInfo.setError(  U2::QueryDesignerPlugin::tr("Close Designer canceled") );
    }
}

bool QueryDesignerService::closeViews() {
    MWMDIManager* wm = AppContext::getMainWindow()->getMDIManager();
    assert(wm);
    foreach(MWMDIWindow* w, wm->getWindows()) {
        QueryViewController* view = qobject_cast<QueryViewController*>(w);
        if (view) {
            if (!AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(view)) {
                return false;
            }
        }
    }
    return true;
}

void QueryDesignerService::sl_startQDPlugin() {
    QAction* action = new QAction(QIcon(":query_designer/images/query_designer.png"), tr("Query Designer..."), this);
    //action->setObjectName("Query Designer");
    connect(action, SIGNAL(triggered()), SLOT(sl_showDesignerWindow()));

    action->setObjectName(ToolsMenu::QUERY_DESIGNER);
    ToolsMenu::addAction(ToolsMenu::TOOLS, action);
}

void QueryDesignerService::sl_showDesignerWindow() {
    assert(isEnabled());
    QueryViewController* view = new QueryViewController;
    view->setWindowIcon(QIcon(":query_designer/images/query_designer.png"));
    AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
    AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
}

Task* QueryDesignerService::createServiceEnablingTask() {
    QString defaultDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + QUERY_SAMPLES_PATH;
    return new QDLoadSamplesTask(QStringList(defaultDir));
}

Task* QueryDesignerService::createServiceDisablingTask() {
    return new CloseDesignerTask(this);
}

void QueryDesignerService::serviceStateChangedCallback(ServiceState , bool enabledStateChanged) {
    if (!enabledStateChanged) {
        return;
    }
    if (isEnabled()) {
        if(!AppContext::getPluginSupport()->isAllPluginsLoaded()) {
            connect(AppContext::getPluginSupport(),SIGNAL(si_allStartUpPluginsLoaded()),SLOT(sl_startQDPlugin()));
        } else {
            sl_startQDPlugin();
        }
    }
}

}//namespace
