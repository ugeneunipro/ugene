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

#include "SWAlgorithmPlugin.h"

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/OptionsPanel.h>

#include <U2Lang/QueryDesignerRegistry.h>

#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "PairwiseAlignmentSmithWatermanGUIExtension.h"
#include "SWAlgorithmTask.h"
#include "SWQuery.h"
#include "SWTaskFactory.h"
#include "SWWorker.h"
#include "SmithWatermanTests.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    return new SWAlgorithmPlugin();
}

SWAlgorithmPlugin::SWAlgorithmPlugin()
    : Plugin(tr("Optimized Smith-Waterman "), tr("Various implementations of Smith-Waterman algorithm")) {
    // initializing ADV context
    if (AppContext::getMainWindow()) {
        ctxADV = new SWAlgorithmADVContext(this);
        ctxADV->init();
    }

    LocalWorkflow::SWWorkerFactory::init();

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new SWQDActorFactory());

    // Smith-Waterman algorithm tests
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat != nullptr);

    auto l = new U2::GAutoDeleteList<U2::XMLTestFactory>(this);
    l->qlist = SWAlgorithmTests::createTestFactories();

    foreach (XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }

    AlignmentAlgorithmsRegistry* par = AppContext::getAlignmentAlgorithmsRegistry();
    SmithWatermanTaskFactoryRegistry* swar = AppContext::getSmithWatermanTaskFactoryRegistry();

    coreLog.trace("Registering classic SW implementation");
    swar->registerFactory(new SWTaskFactory(SW_classic), QString("Classic 2"));  // ADV search register
    par->registerAlgorithm(new SWPairwiseAlignmentAlgorithm());

    coreLog.trace("Registering SSE2 SW implementation");
    swar->registerFactory(new SWTaskFactory(SW_sse2), QString("SSE2"));
    par->getAlgorithm("Smith-Waterman")
        ->addAlgorithmRealization(
            new PairwiseAlignmentSmithWatermanTaskFactory(SW_sse2),
            new PairwiseAlignmentSmithWatermanGUIExtensionFactory(SW_sse2),
            "SSE2");
}

QList<XMLTestFactory*> SWAlgorithmTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_SmithWatermnan::createFactory());
    res.append(GTest_SmithWatermnanPerf::createFactory());
    return res;
}

SWAlgorithmADVContext::SWAlgorithmADVContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID), dialogConfig() {
}

void SWAlgorithmADVContext::initViewContext(GObjectView* view) {
    auto av = qobject_cast<AnnotatedDNAView*>(view);
    assert(av != nullptr);
    auto a = new ADVGlobalAction(av, QIcon(":core/images/sw.png"), tr("Find pattern [Smith-Waterman]..."), 15);
    a->setObjectName("find_pattern_smith_waterman_action");

    a->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F));
    a->setShortcutContext(Qt::WindowShortcut);
    av->getWidget()->addAction(a);

    connect(a, SIGNAL(triggered()), SLOT(sl_search()));
}

void SWAlgorithmADVContext::sl_search() {
    auto action = qobject_cast<GObjectViewAction*>(sender());
    assert(0 != action);

    auto av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    assert(av != nullptr);

    ADVSequenceObjectContext* seqCtx = av->getActiveSequenceContext();
    SmithWatermanDialogController::run(av->getWidget(), seqCtx, &dialogConfig);
}

SWPairwiseAlignmentAlgorithm::SWPairwiseAlignmentAlgorithm()
    : AlignmentAlgorithm(PairwiseAlignment,
                         "Smith-Waterman",
                         AlignmentAlgorithmsRegistry::tr("Smith-Waterman"),
                         new PairwiseAlignmentSmithWatermanTaskFactory(SW_classic),
                         new PairwiseAlignmentSmithWatermanGUIExtensionFactory(SW_classic),
                         "SW_classic") {
}

bool SWPairwiseAlignmentAlgorithm::checkAlphabet(const DNAAlphabet* alphabet) const {
    SAFE_POINT(nullptr != alphabet, "Alphabet is NULL.", false);
    SubstMatrixRegistry* matrixReg = AppContext::getSubstMatrixRegistry();
    SAFE_POINT(matrixReg, "SubstMatrixRegistry is NULL.", false);
    QStringList matrixList = matrixReg->selectMatrixNamesByAlphabet(alphabet);
    return !matrixList.isEmpty();
}

}  // namespace U2
