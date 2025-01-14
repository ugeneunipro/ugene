/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "Primer3Plugin.h"

#include <QAction>
#include <QMap>
#include <QMessageBox>

#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ToolsMenu.h>

#include <U2Test/GTestFrameworkComponents.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "Primer3Dialog.h"
#include "Primer3Query.h"
#include "task/Primer3TopLevelTask.h"
#include "temperature/Primer3TmCalculatorFactory.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    auto plug = new Primer3Plugin();
    return plug;
}

Primer3Plugin::Primer3Plugin()
    : Plugin(tr("Primer3"), tr("Integrated tool for PCR primers design.")), viewCtx(nullptr) {
    if (AppContext::getMainWindow()) {
        viewCtx = new Primer3ADVContext(this);
        viewCtx->init();

        auto noTargetSequencePrimer3 = new QAction(QIcon(":/primer3/images/primer3.png"), tr("Primer3 (no target sequence)..."), this);
        noTargetSequencePrimer3->setObjectName(ToolsMenu::PRIMER3);
        connect(noTargetSequencePrimer3, &QAction::triggered, viewCtx, &Primer3ADVContext::sl_showDialog);
        ToolsMenu::addAction(ToolsMenu::PRIMER_MENU, noTargetSequencePrimer3);
    }

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDPrimerActorPrototype());

    AppContext::getTmCalculatorRegistry()->registerEntry(new Primer3TmCalculatorFactory);

    //////////////////////////////////////////////////////////////////////////
    // tests
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    SAFE_POINT_NN(xmlTestFormat, );

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = Primer3Tests::createTestFactories();

    for (XMLTestFactory* f : l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        SAFE_POINT(res, "Can't register XMLTestFactory", );
    }
}

Primer3ADVContext::Primer3ADVContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
}

void Primer3ADVContext::initViewContext(GObjectViewController* v) {
    auto av = qobject_cast<AnnotatedDNAView*>(v);
    auto a = new ADVGlobalAction(av, QIcon(":/primer3/images/primer3.png"), tr("Primer3..."), 95);
    a->setObjectName("primer3_action");
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, &ADVGlobalAction::triggered, this, &Primer3ADVContext::sl_showDialog);
}

void Primer3ADVContext::sl_showDialog() {
    QAction* a = (QAction*)sender();
    ADVSequenceObjectContext* seqCtx = nullptr;
    auto viewAction = qobject_cast<GObjectViewAction*>(a);
    if (viewAction != nullptr) {
        auto av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
        SAFE_POINT(av != nullptr, L10N::nullPointerError("AnnotatedDNAView"), );

        seqCtx = av->getActiveSequenceContext();
        SAFE_POINT(seqCtx != nullptr, L10N::nullPointerError("ADVSequenceObjectContext"), );
    }

    Primer3Dialog dialog(seqCtx);
    dialog.exec();
    CHECK(dialog.result() == QDialog::Accepted, );

    const auto& settings = dialog.getSettings();

    Task* primer3Task = nullptr;
    if (seqCtx != nullptr) {
        U2OpStatusImpl os;
        QByteArray seqData = seqCtx->getSequenceObject()->getWholeSequenceData(os);
        CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );

        settings->setSequence(seqData, seqCtx->getSequenceObject()->isCircular());
        QString err = dialog.checkModel();
        CHECK_EXT(err.isEmpty(), QMessageBox::warning(QApplication::activeWindow(), dialog.windowTitle(), err), );
        CHECK_EXT(dialog.prepareAnnotationObject(),
                  QMessageBox::warning(QApplication::activeWindow(),
                                       tr("Error"),
                                       tr("Cannot create an annotation object. Please check settings")), );

        const CreateAnnotationModel& model = dialog.getCreateAnnotationModel();
        auto ato = model.getAnnotationObject();
        seqCtx->getAnnotatedDNAView()->tryAddObject(ato);


        primer3Task = new Primer3TopLevelTask(settings, seqCtx->getSequenceObject(), ato, model.groupName, model.data->name, model.description);
    } else {
        auto resultFilePath = dialog.getResultFileName();
        SAFE_POINT(!resultFilePath.isEmpty(), "Result path is empty", );

        primer3Task = new Primer3TopLevelTask(settings, resultFilePath);
    }

    AppContext::getTaskScheduler()->registerTopLevelTask(primer3Task);
}

QList<XMLTestFactory*> Primer3Tests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_Primer3::createFactory());
    return res;
}

}  // namespace U2
