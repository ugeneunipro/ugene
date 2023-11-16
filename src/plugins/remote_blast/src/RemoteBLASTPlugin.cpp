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

#include "RemoteBLASTPlugin.h"

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QMenu>
#include <QMessageBox>

#include <U2Algorithm/CDSearchTaskFactoryRegistry.h>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DataBaseRegistry.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/AnnotationsTreeView.h>

#include "BlastQuery.h"
#include "RemoteBLASTPrimerPairsToAnnotationsTask.h"
#include "RemoteBLASTTask.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    RemoteBLASTPlugin* plug = new RemoteBLASTPlugin();
    return plug;
}

RemoteBLASTPlugin::RemoteBLASTPlugin()
    : Plugin(tr("Remote BLAST"), tr("Performs remote database queries: BLAST, CDD, etc...")), ctx(nullptr) {
    if (AppContext::getMainWindow()) {
        ctx = new RemoteBLASTViewContext(this);
        ctx->init();
    }

    DataBaseRegistry* reg = AppContext::getDataBaseRegistry();
    reg->registerDataBase(new BLASTFactory(), "blastn");
    reg->registerDataBase(new BLASTFactory(), "blastp");
    reg->registerDataBase(new BLASTFactory(), "cdd");

    LocalWorkflow::RemoteBLASTWorkerFactory::init();

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDCDDActorPrototype());

    AppContext::getCDSFactoryRegistry()->registerFactory(new RemoteCDSearchFactory(), CDSearchFactoryRegistry::RemoteSearch);

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat != nullptr);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = RemoteBLASTPluginTests::createTestFactories();

    foreach (XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }
}

RemoteBLASTViewContext::RemoteBLASTViewContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
}

void RemoteBLASTViewContext::initViewContext(GObjectViewController* view) {
    auto av = qobject_cast<AnnotatedDNAView*>(view);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":/remote_blast/images/remote_db_request.png"), tr("Query NCBI BLAST database..."), 60);
    a->setObjectName("Query NCBI BLAST database");
    a->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_B));
    connect(a, SIGNAL(triggered()), SLOT(sl_showDialog()));
}

/*
 * NCBI blast service provides no details about request size limits.
 * See: https://blast.ncbi.nlm.nih.gov/Blast.cgi?CMD=Web&PAGE_TYPE=BlastDocs&DOC_TYPE=DeveloperInfo
 *
 * But it is safe to expect that any single request should not exceed some meaningful size: normally it is below ~10k.
 * We use 1000x bigger size (10M) to 1) keep safety 2) avoid unnecessary limitations for users.
 */
#define MAX_REGION_SIZE_TO_SEARCH_WITH_REMOTE_BLAST (10 * 1000 * 1000)

void RemoteBLASTViewContext::sl_showDialog() {
    QAction* a = (QAction*)sender();
    auto viewAction = qobject_cast<GObjectViewAction*>(a);
    auto av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    SAFE_POINT(av != nullptr, L10N::nullPointerError("AnnotatedDNAView"), );

    auto seqCtx = av->getActiveSequenceContext();
    // If we have selected primer pairs we should BLAST their regions
    auto selectedPrimerPairs = getSelectedPrimerPairs(av->getAnnotationsGroupSelection());
    QStringList selectedPrimerPairGroupNames;
    for (const auto& primerPair : qAsConst(selectedPrimerPairs)) {
        selectedPrimerPairGroupNames << primerPair.first->getGroup()->getName();
    }

    bool isAminoSeq = seqCtx->getAlphabet()->isAmino();

    QObjectScopedPointer<SendSelectionDialog> dlg = new SendSelectionDialog(seqCtx, isAminoSeq, selectedPrimerPairGroupNames, av->getWidget());
    dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(QDialog::Accepted == dlg->result(), );

    RemoteBLASTTaskSettings cfg = dlg->cfg;
    auto seqObj = seqCtx->getSequenceObject();
    SAFE_POINT(seqObj != nullptr, L10N::nullPointerError("U2SequenceObject"), );

    cfg.isCircular = seqObj->isCircular();
    DNATranslation* aminoT = (dlg->translateToAmino ? seqCtx->getAminoTT() : 0);
    cfg.aminoT = aminoT;
    DNATranslation* complT = (dlg->translateToAmino ? seqCtx->getComplementTT() : 0);
    cfg.complT = complT;


    if (selectedPrimerPairs.isEmpty()) {
        DNASequenceSelection* s = seqCtx->getSequenceSelection();
        QVector<U2Region> regions;
        if (s->isEmpty()) {
            regions.append(U2Region(0, seqCtx->getSequenceLength()));
        } else {
            regions = s->getSelectedRegions();
        }

        // First check that the regions are not too large: remote service will not accept gigs of data.
        for (const U2Region& region : qAsConst(regions)) {
            if (region.length > MAX_REGION_SIZE_TO_SEARCH_WITH_REMOTE_BLAST) {
                QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), tr("Selected region is too large!"));
                return;
            }
        }
        U2OpStatusImpl os;
        for (const U2Region& r : qAsConst(regions)) {
            QByteArray query = seqCtx->getSequenceData(r, os);
            CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );

            cfg.query = query;
            auto annotationTableObject = dlg->getAnnotationObject();
            SAFE_POINT(annotationTableObject != nullptr, L10N::nullPointerError("AnnotationTableObject"), );

            Task* t = new RemoteBLASTToAnnotationsTask(cfg, r.startPos, annotationTableObject, dlg->getUrl(), dlg->getGroupName(), dlg->getAnnotationDescription());
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
        }
    } else {
        auto sequenceObject = seqCtx->getSequenceObject();
        auto t = new RemoteBLASTPrimerPairsToAnnotationsTask(sequenceObject, selectedPrimerPairs, cfg);
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    }
}

const QList<QPair<Annotation*, Annotation*>> RemoteBLASTViewContext::getSelectedPrimerPairs(AnnotationGroupSelection* ags) {
    auto annGroups = ags->getSelection();
    QList<QPair<Annotation*, Annotation*>> primerAnnotationPairs;
    for (auto primerGroup : qAsConst(annGroups)) {
        auto primerAnnotations = primerGroup->getAnnotations();
        auto primerAnnSize = primerAnnotations.size();
        // We should have whether left and right primer (2) or left, right and internal (3)
        CHECK_CONTINUE(primerAnnSize == 2 || primerAnnSize == 3);

        Annotation* forward = nullptr;
        Annotation* reverse = nullptr;
        for (auto ann : qAsConst(primerAnnotations)) {
            CHECK_BREAK(ann->getType() == U2FeatureTypes::Primer);
            CHECK_CONTINUE(ann->getName() == "top_primers");

            switch (ann->getLocation()->strand.getDirection()) {
                case U2Strand::Direction::Direct:
                    forward = ann;
                    break;
                case U2Strand::Direction::Complementary:
                    reverse = ann;
                    break;
            }
        }
        if (forward != nullptr && reverse != nullptr) {
            primerAnnotationPairs << QPair<Annotation*, Annotation*> {forward, reverse};
        }
    }

    return primerAnnotationPairs;
}


QList<XMLTestFactory*> RemoteBLASTPluginTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_RemoteBLAST::createFactory());
    return res;
}

}  // namespace U2
