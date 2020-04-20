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
#include <QMap>
#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/GAutoDeleteList.h>

#include <U2Gui/GUIUtils.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Lang/QueryDesignerRegistry.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "ORFDialog.h"
#include "ORFMarkerPlugin.h"
#include "ORFMarkerTask.h"
#include "ORFMarkerTests.h"
#include "ORFQuery.h"
#include "ORFWorker.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    ORFMarkerPlugin* plug = new ORFMarkerPlugin();
    return plug;
}

ORFMarkerPlugin::ORFMarkerPlugin() :
    Plugin(tr("ORF Marker"), tr("Searches for open reading frames (ORF) in a DNA sequence.")), viewCtx(NULL) {
    if (AppContext::getMainWindow()) {
        viewCtx = new ORFViewContext(this);
        viewCtx->init();
        AppContext::getAutoAnnotationsSupport()->registerAutoAnnotationsUpdater(new ORFAutoAnnotationsUpdater );

    }

    LocalWorkflow::ORFWorkerFactory::init();

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDORFActorPrototype());

    //ORFMarker test
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = ORFMarkerTests::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }
}

ORFMarkerPlugin::~ORFMarkerPlugin() {
    //printf("ORF deallocated!\n");
}

ORFViewContext::ORFViewContext(QObject* p) :
GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {}

void ORFViewContext::initViewContext(GObjectView* v) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(v);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":orf_marker/images/orf_marker.png"), tr("Find ORFs..."), 20);
    a->setObjectName("Find ORFs");
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, SIGNAL(triggered()), SLOT(sl_showDialog()));
}

void ORFViewContext::sl_showDialog() {
    QAction* a = (QAction*)sender();
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(a);
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    assert(av);

    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    assert(seqCtx->getAlphabet()->isNucleic());
    QObjectScopedPointer<ORFDialog> d = new ORFDialog(seqCtx);
    d->exec();
}

QList<XMLTestFactory*> ORFMarkerTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_ORFMarkerTask::createFactory());
    return res;
}


}//namespace

