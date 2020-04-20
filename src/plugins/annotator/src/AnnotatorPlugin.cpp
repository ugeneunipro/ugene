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

#include <QMenu>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Gui/GUIUtils.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "AnnotatorPlugin.h"
#include "AnnotatorTests.h"
#include "CollocationWorker.h"
#include "CollocationsDialogController.h"
#include "CustomAutoAnnotationDialog.h"
#include "CustomPatternAnnotationTask.h"
#include "GeneByGeneReportWorker.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    AnnotatorPlugin * plug = new AnnotatorPlugin();
    return plug;
}

AnnotatorPlugin::AnnotatorPlugin() : Plugin(tr("DNA Annotator"), tr("This plugin contains routines to manipulate and search DNA sequence annotations")), viewCtx(NULL)
{
    if (AppContext::getMainWindow()) {
        QString customAnnotationDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/custom_annotations";
        QString plasmidFeaturesPath = customAnnotationDir + "/plasmid_features.txt";
        SharedFeatureStore store( new FeatureStore(PLASMID_FEATURES_GROUP_NAME, plasmidFeaturesPath));
        store->load();
        if (store->isLoaded()) {
            CustomPatternAutoAnnotationUpdater* aaUpdater = new CustomPatternAutoAnnotationUpdater(store);
            AppContext::getAutoAnnotationsSupport()->registerAutoAnnotationsUpdater(aaUpdater);
        }

        viewCtx = new AnnotatorViewContext(this, store->isLoaded());
        viewCtx->init();
    }
    LocalWorkflow::CollocationWorkerFactory::init();
    LocalWorkflow::GeneByGeneReportWorkerFactory::init();

    //Annotator test
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = AnnotatorTests::createTestFactories();


    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }
}

AnnotatorViewContext::AnnotatorViewContext(QObject* p, bool customAutoAnnotations)
: GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID), customFeaturesAvailable(customAutoAnnotations) {

}

void AnnotatorViewContext::initViewContext(GObjectView* v) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(v);
    ADVGlobalAction* findRegionsAction = new ADVGlobalAction(av, QIcon(":annotator/images/regions.png"), tr("Find annotated regions..."), 30);
    connect(findRegionsAction, SIGNAL(triggered()), SLOT(sl_showCollocationDialog()));

    if (customFeaturesAvailable) {
        ADVGlobalAction* annotatePlasmidAction = new ADVGlobalAction(av, QIcon(":annotator/images/plasmid_features.png"), tr("Annotate plasmid..."), 31);
        annotatePlasmidAction->addAlphabetFilter(DNAAlphabet_NUCL);
        connect(annotatePlasmidAction, SIGNAL(triggered()), SLOT(sl_showCustomAutoAnnotationDialog()));
    }
}

void AnnotatorViewContext::sl_showCollocationDialog() {
    QAction* a = (QAction*)sender();
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(a);
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    assert(av);

    QSet<QString> allNames;

    foreach (AnnotationTableObject *ao, av->getAnnotationObjects()) {
        foreach (Annotation *annotation, ao->getAnnotations()) {
            allNames.insert(annotation->getName());
        }
    }
    if (allNames.isEmpty()) {
        QMessageBox::warning(av->getWidget(), tr("Warning"),tr("No annotations found"));
        return;
    }

    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    if (seqCtx == NULL) {
        return;
    }

    QObjectScopedPointer<CollocationsDialogController> d = new CollocationsDialogController(allNames.toList(), seqCtx);
    d->exec();
}


void AnnotatorViewContext::sl_showCustomAutoAnnotationDialog() {
    QAction* a = (QAction*)sender();
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(a);
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    assert(av);

    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    if (seqCtx == NULL) {
        return;
    }

    QObjectScopedPointer<CustomAutoAnnotationDialog> dlg = new CustomAutoAnnotationDialog(seqCtx);
    dlg->exec();
}

QList<XMLTestFactory*> AnnotatorTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_AnnotatorSearch::createFactory());
    res.append(GTest_GeneByGeneApproach::createFactory());
    res.append(GTest_CustomAutoAnnotation::createFactory());
    return res;
}

}//namespace
