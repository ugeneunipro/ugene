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

#include <QDir>
#include <QMenu>
#include <QMessageBox>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Gui/MainWindow.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/ToolsMenu.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSequenceWidget.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include <U2Test/GTestFrameworkComponents.h>

#include "ConstructMoleculeDialog.h"
#include "CreateFragmentDialog.h"
#include "DigestSequenceDialog.h"
#include "EnzymesPlugin.h"
#include "EnzymesQuery.h"
#include "EnzymesTests.h"
#include "FindEnzymesDialog.h"
#include "FindEnzymesTask.h"

const QString CREATE_PCR_PRODUCT_ACTION_NAME = "Create PCR product";

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin * U2_PLUGIN_INIT_FUNC() {
    EnzymesPlugin * plug = new EnzymesPlugin();
    return plug;
}

EnzymesPlugin::EnzymesPlugin()
    : Plugin(tr("Restriction analysis"), tr("Finds and annotates restriction sites on a DNA sequence.")), ctxADV(NULL)
{
    if (AppContext::getMainWindow()) {
        createToolsMenu();

        QList<QAction*> actions;
        actions.append(openDigestSequenceDialog);
        actions.append(openConstructMoleculeDialog);
        actions.append(openCreateFragmentDialog);

        ctxADV = new EnzymesADVContext(this,actions);
        ctxADV->init();

        AppContext::getAutoAnnotationsSupport()->registerAutoAnnotationsUpdater(new FindEnzymesAutoAnnotationUpdater());
    }

    EnzymesSelectorWidget::setupSettings();
    FindEnzymesDialog::initDefaultSettings();

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDEnzymesActorPrototype());

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = EnzymeTests::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) {
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

    if (w == NULL) {
        QMessageBox::information(QApplication::activeWindow(), openDigestSequenceDialog->text(),
            tr("There is no active sequence object.\nTo start partition open sequence document."));
        return;
    }

    AnnotatedDNAView* view = qobject_cast<AnnotatedDNAView*>(w->getObjectView());
    if (view == NULL) {
        QMessageBox::information(QApplication::activeWindow(), openDigestSequenceDialog->text(),
            tr("There is no active sequence object.\nTo start partition open sequence document."));
        return;

    }

    if (!view->getSequenceInFocus()->getSequenceObject()->getAlphabet()->isNucleic()) {
        QMessageBox::information(QApplication::activeWindow(), openDigestSequenceDialog->text(),
            tr("Can not digest into fragments non-nucleic sequence."));
        return;

    }

    QObjectScopedPointer<DigestSequenceDialog> dlg = new DigestSequenceDialog(view->getSequenceInFocus(), QApplication::activeWindow());
    dlg->exec();
}

void EnzymesPlugin::sl_onOpenCreateFragmentDialog() {
    GObjectViewWindow* w = GObjectViewUtils::getActiveObjectViewWindow();

    if (w == NULL) {
        QMessageBox::information(QApplication::activeWindow(), openCreateFragmentDialog->text(),
            tr("There is no active sequence object.\nTo create fragment open sequence document."));
        return;
    }

    AnnotatedDNAView* view = qobject_cast<AnnotatedDNAView*>(w->getObjectView());
    if (view == NULL) {
        QMessageBox::information(QApplication::activeWindow(), openCreateFragmentDialog->text(),
            tr("There is no active sequence object.\nTo create fragment open sequence document."));
        return;

    }

    U2SequenceObject* dnaObj = view->getSequenceInFocus()->getSequenceObject();
    assert(dnaObj != NULL);
    if (!dnaObj->getAlphabet()->isNucleic()) {
        QMessageBox::information(QApplication::activeWindow(), openCreateFragmentDialog->text(),
            tr("The sequence doesn't have nucleic alphabet, it can not be used in cloning."));
        return;
    }

    QObjectScopedPointer<CreateFragmentDialog> dlg = new CreateFragmentDialog(view->getSequenceInFocus(), QApplication::activeWindow());
    dlg->exec();
}

void EnzymesPlugin::sl_onOpenConstructMoleculeDialog() {
    Project* p = AppContext::getProject();
    if (p == NULL) {
        QMessageBox::information(QApplication::activeWindow(), openConstructMoleculeDialog->text(),
            tr("There is no active project.\nTo start ligation create a project or open an existing."));
        return;
    }

    QList<DNAFragment> fragments = DNAFragment::findAvailableFragments();

    QObjectScopedPointer<ConstructMoleculeDialog> dlg = new ConstructMoleculeDialog(fragments, QApplication::activeWindow());
    dlg->exec();
}

//////////////////////////////////////////////////////////////////////////

EnzymesADVContext::EnzymesADVContext(QObject* p,const QList<QAction*>& actions)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID), cloningActions(actions)
{

}

void EnzymesADVContext::initViewContext(GObjectView* view) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(view);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":enzymes/images/enzymes.png"), tr("Find restriction sites..."), 50);
    a->setObjectName("Find restriction sites");
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, SIGNAL(triggered()), SLOT(sl_search()));

    GObjectViewAction *createPCRProductAction = new GObjectViewAction(av, av, tr("Create PCR product..."));
    createPCRProductAction->setObjectName(CREATE_PCR_PRODUCT_ACTION_NAME);
    connect(createPCRProductAction, SIGNAL(triggered()), SLOT(sl_createPCRProduct()));
    addViewAction(createPCRProductAction);
}

void EnzymesADVContext::sl_search() {
    GObjectViewAction* action = qobject_cast<GObjectViewAction*>(sender());
    assert(action!=NULL);
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    assert(av!=NULL);

    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    assert(seqCtx->getAlphabet()->isNucleic());
    QObjectScopedPointer<FindEnzymesDialog> d = new FindEnzymesDialog(seqCtx);
    d->exec();
}

// TODO: move definitions to core
#define PRIMER_ANNOTATION_GROUP_NAME    "pair"
#define PRIMER_ANNOTATION_NAME          "primer"


void EnzymesADVContext::buildMenu(GObjectView *v, QMenu *m) {
    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(v);
    SAFE_POINT(NULL != av, "Invalid sequence view",);
    CHECK(av->getSequenceInFocus()->getAlphabet()->isNucleic(),);

    QMenu *cloningMenu = new QMenu(tr("Cloning"), m);
    cloningMenu->menuAction()->setObjectName("Cloning");
    cloningMenu->addActions(cloningActions);

    QAction *exportMenuAction = GUIUtils::findAction(m->actions(), ADV_MENU_EXPORT);
    m->insertMenu(exportMenuAction, cloningMenu);

    if (!av->getAnnotationsSelection()->getAnnotations().isEmpty()) {
        Annotation *a = av->getAnnotationsSelection()->getAnnotations().first();
        const QString annName = a->getName();
        const QString groupName = a->getGroup()->getName();
        const int annCount = a->getGroup()->getAnnotations().size();

        if (annName == PRIMER_ANNOTATION_NAME && groupName.startsWith(PRIMER_ANNOTATION_GROUP_NAME) && 2 == annCount) {
            QAction *a = findViewAction(v, CREATE_PCR_PRODUCT_ACTION_NAME);
            SAFE_POINT(NULL != a, "Invalid menu action",);
            cloningMenu->addAction(a);
        }
    }
}

void EnzymesADVContext::sl_createPCRProduct() {
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender());
    SAFE_POINT(action != NULL, "Invalid action object!",);
    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(action->getObjectView());
    SAFE_POINT(av != NULL, "Invalid DNA view!",);

    const QList<Annotation*>& annotations = av->getAnnotationsSelection()->getAnnotations();
    CHECK(!annotations.isEmpty(),)

    Annotation *a = annotations.first();
    AnnotationGroup *group = a->getGroup();
    if (group->getName().startsWith(PRIMER_ANNOTATION_GROUP_NAME)) {
        SAFE_POINT(group->getAnnotations().size() == 2, "Invalid selected annotation count!",);

        Annotation *a1 = group->getAnnotations().at(0);
        Annotation *a2 = group->getAnnotations().at(1);
        int startPos = a1->getLocation()->regions.at(0).startPos;
        SAFE_POINT(a2->getLocation()->strand == U2Strand::Complementary, "Invalid annotation's strand!",);
        int endPos = a2->getLocation()->regions.at(0).endPos();

        U2SequenceObject *seqObj = av->getSequenceInFocus()->getSequenceObject();
        U2Region region(startPos, endPos - startPos);
        QObjectScopedPointer<CreateFragmentDialog> dlg = new CreateFragmentDialog(seqObj, region, av->getSequenceWidgetInFocus());
        dlg->setWindowTitle("Create PCR product");
        dlg->exec();
    }
}

} //namespace
