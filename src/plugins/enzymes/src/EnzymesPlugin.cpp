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

#include "EnzymesPlugin.h"

#include <QMenu>
#include <QMessageBox>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AppContext.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ToolsMenu.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSequenceWidget.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "ConstructMoleculeDialog.h"
#include "CreateFragmentDialog.h"
#include "DigestSequenceDialog.h"
#include "EnzymesQuery.h"
#include "EnzymesTests.h"
#include "FindEnzymesDialog.h"
#include "FindEnzymesTask.h"

const QString CREATE_PCR_PRODUCT_ACTION_NAME = "Create PCR product";

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    EnzymesPlugin* plug = new EnzymesPlugin();
    return plug;
}

EnzymesPlugin::EnzymesPlugin()
    : Plugin(tr("Restriction analysis"), tr("Finds and annotates restriction sites on a DNA sequence.")), ctxADV(nullptr) {
    if (AppContext::getMainWindow()) {
        createToolsMenu();

        QList<QAction*> actions;
        actions.append(openDigestSequenceDialog);
        actions.append(openConstructMoleculeDialog);
        actions.append(openCreateFragmentDialog);

        ctxADV = new EnzymesADVContext(this, actions);
        ctxADV->init();

        AppContext::getAutoAnnotationsSupport()->registerAutoAnnotationsUpdater(new FindEnzymesAutoAnnotationUpdater());
    }

    EnzymesSelectorWidget::setupSettings();

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat != nullptr);

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDEnzymesActorPrototype());

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = EnzymeTests::createTestFactories();

    foreach (XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        assert(res);
        Q_UNUSED(res);
    }
}

void EnzymesPlugin::createToolsMenu() {
    openDigestSequenceDialog = new QAction(tr("Digest into fragments..."), this);
    openDigestSequenceDialog->setObjectName(ToolsMenu::CLONING_FRAGMENTS);
    openConstructMoleculeDialog = new QAction(tr("Construct molecule..."), this);
    openConstructMoleculeDialog->setObjectName(ToolsMenu::CLONING_CONSTRUCT);
    openCreateFragmentDialog = new QAction(tr("Create fragment..."), this);
    openCreateFragmentDialog->setObjectName("Create Fragment");

    connect(openDigestSequenceDialog, SIGNAL(triggered()), SLOT(sl_onOpenDigestSequenceDialog()));
    connect(openConstructMoleculeDialog, SIGNAL(triggered()), SLOT(sl_onOpenConstructMoleculeDialog()));
    connect(openCreateFragmentDialog, SIGNAL(triggered()), SLOT(sl_onOpenCreateFragmentDialog()));

    ToolsMenu::addAction(ToolsMenu::CLONING_MENU, openDigestSequenceDialog);
    ToolsMenu::addAction(ToolsMenu::CLONING_MENU, openConstructMoleculeDialog);
}

void EnzymesPlugin::sl_onOpenDigestSequenceDialog() {
    GObjectViewWindow* w = GObjectViewUtils::getActiveObjectViewWindow();

    if (w == nullptr) {
        QMessageBox::information(QApplication::activeWindow(), openDigestSequenceDialog->text(), tr("There is no active sequence object.\nTo start partition open sequence document."));
        return;
    }

    auto view = qobject_cast<AnnotatedDNAView*>(w->getObjectView());
    if (view == nullptr) {
        QMessageBox::information(QApplication::activeWindow(), openDigestSequenceDialog->text(), tr("There is no active sequence object.\nTo start partition open sequence document."));
        return;
    }

    if (!view->getActiveSequenceContext()->getSequenceObject()->getAlphabet()->isNucleic()) {
        QMessageBox::information(QApplication::activeWindow(), openDigestSequenceDialog->text(), tr("Can not digest into fragments non-nucleic sequence."));
        return;
    }

    QObjectScopedPointer<DigestSequenceDialog> dlg = new DigestSequenceDialog(view->getActiveSequenceContext(), QApplication::activeWindow());
    dlg->exec();
}

void EnzymesPlugin::sl_onOpenCreateFragmentDialog() {
    GObjectViewWindow* w = GObjectViewUtils::getActiveObjectViewWindow();

    if (w == nullptr) {
        QMessageBox::information(QApplication::activeWindow(), openCreateFragmentDialog->text(), tr("There is no active sequence object.\nTo create fragment open sequence document."));
        return;
    }

    auto view = qobject_cast<AnnotatedDNAView*>(w->getObjectView());
    if (view == nullptr) {
        QMessageBox::information(QApplication::activeWindow(), openCreateFragmentDialog->text(), tr("There is no active sequence object.\nTo create fragment open sequence document."));
        return;
    }

    U2SequenceObject* dnaObj = view->getActiveSequenceContext()->getSequenceObject();
    assert(dnaObj != nullptr);
    if (!dnaObj->getAlphabet()->isNucleic()) {
        QMessageBox::information(QApplication::activeWindow(), openCreateFragmentDialog->text(), tr("The sequence doesn't have nucleic alphabet, it can not be used in cloning."));
        return;
    }

    QObjectScopedPointer<CreateFragmentDialog> dlg = new CreateFragmentDialog(view->getActiveSequenceContext(), QApplication::activeWindow());
    dlg->exec();
}

void EnzymesPlugin::sl_onOpenConstructMoleculeDialog() {
    Project* p = AppContext::getProject();
    if (p == nullptr) {
        QMessageBox::information(QApplication::activeWindow(), openConstructMoleculeDialog->text(), tr("There is no active project.\nTo start ligation create a project or open an existing."));
        return;
    }

    QList<DNAFragment> fragments = DNAFragment::findAvailableFragments();

    QObjectScopedPointer<ConstructMoleculeDialog> dlg = new ConstructMoleculeDialog(fragments, QApplication::activeWindow());
    dlg->exec();
}

//////////////////////////////////////////////////////////////////////////

EnzymesADVContext::EnzymesADVContext(QObject* p, const QList<QAction*>& actions)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID), cloningActions(actions) {
}

void EnzymesADVContext::initViewContext(GObjectViewController* view) {
    auto av = qobject_cast<AnnotatedDNAView*>(view);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":enzymes/images/enzymes.png"), tr("Find restriction sites..."), 50);
    a->setObjectName("Find restriction sites");
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, SIGNAL(triggered()), SLOT(sl_search()));

    GObjectViewAction* createPCRProductAction = new GObjectViewAction(av, av, tr("Create PCR product..."));
    createPCRProductAction->setObjectName(CREATE_PCR_PRODUCT_ACTION_NAME);
    connect(createPCRProductAction, SIGNAL(triggered()), SLOT(sl_createPCRProduct()));
    addViewAction(createPCRProductAction);
}

void EnzymesADVContext::sl_search() {
    auto action = qobject_cast<GObjectViewAction*>(sender());
    assert(action != nullptr);
    auto av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    assert(av != nullptr);

    ADVSequenceObjectContext* seqCtx = av->getActiveSequenceContext();
    assert(seqCtx->getAlphabet()->isNucleic());
    QObjectScopedPointer<FindEnzymesDialog> d = new FindEnzymesDialog(seqCtx);
    d->exec();
}

// TODO: move definitions to core
#define PRIMER_ANNOTATION_GROUP_NAME "pair"

void EnzymesADVContext::buildStaticOrContextMenu(GObjectViewController* v, QMenu* m) {
    auto av = qobject_cast<AnnotatedDNAView*>(v);
    SAFE_POINT(av != nullptr, "Invalid sequence view", );
    CHECK(av->getActiveSequenceContext()->getAlphabet()->isNucleic(), );

    QMenu* cloningMenu = new QMenu(tr("Cloning"), m);
    cloningMenu->menuAction()->setObjectName("Cloning");
    cloningMenu->addActions(cloningActions);

    QAction* exportMenuAction = GUIUtils::findAction(m->actions(), ADV_MENU_EXPORT);
    m->insertMenu(exportMenuAction, cloningMenu);

    if (!av->getAnnotationsSelection()->getAnnotations().isEmpty()) {
        Annotation* firstAnnotation = av->getAnnotationsSelection()->getAnnotations().first();
        U2FeatureType featureType = firstAnnotation->getType();
        QString groupName = firstAnnotation->getGroup()->getName();
        int countOfAnnotationsInGroup = firstAnnotation->getGroup()->getAnnotations().size();

        if (featureType == U2FeatureTypes::Primer && groupName.startsWith(PRIMER_ANNOTATION_GROUP_NAME) && countOfAnnotationsInGroup == 2) {
            QAction* createPcrAction = findViewAction(v, CREATE_PCR_PRODUCT_ACTION_NAME);
            SAFE_POINT(createPcrAction != nullptr, "CREATE_PCR_PRODUCT_ACTION_NAME not found!", );
            cloningMenu->addAction(createPcrAction);
        }
    }
}

void EnzymesADVContext::sl_createPCRProduct() {
    auto action = qobject_cast<GObjectViewAction*>(sender());
    SAFE_POINT(action != nullptr, "Invalid action object!", );
    auto av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    SAFE_POINT(av != nullptr, "Invalid DNA view!", );

    const QList<Annotation*>& annotations = av->getAnnotationsSelection()->getAnnotations();
    CHECK(!annotations.isEmpty(), )

    Annotation* a = annotations.first();
    AnnotationGroup* group = a->getGroup();
    if (group->getName().startsWith(PRIMER_ANNOTATION_GROUP_NAME)) {
        SAFE_POINT(group->getAnnotations().size() == 2, "Invalid selected annotation count!", );

        Annotation* a1 = group->getAnnotations().at(0);
        Annotation* a2 = group->getAnnotations().at(1);
        int startPos = a1->getLocation()->regions.at(0).startPos;
        SAFE_POINT(a2->getLocation()->strand == U2Strand::Complementary, "Invalid annotation's strand!", );
        int endPos = a2->getLocation()->regions.at(0).endPos();

        U2SequenceObject* seqObj = av->getActiveSequenceContext()->getSequenceObject();
        U2Region region(startPos, endPos - startPos);
        QObjectScopedPointer<CreateFragmentDialog> dlg = new CreateFragmentDialog(seqObj, region, av->getActiveSequenceWidget());
        dlg->setWindowTitle("Create PCR product");
        dlg->exec();
    }
}

}  // namespace U2
