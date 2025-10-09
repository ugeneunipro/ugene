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

#include "RemoteBLASTPlugin.h"

#include <QDir>
#include <QMenu>
#include <QMessageBox>

#include <U2Algorithm/CDSearchTaskFactoryRegistry.h>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AppContext.h>
#include <U2Core/CreateAnnotationTask.h>
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
    auto plug = new RemoteBLASTPlugin();
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

const QString TRANSFORM_INTO_A_PRIMER_PAIR_NAME = "transform_into_a_primer_pair";

void RemoteBLASTViewContext::initViewContext(GObjectViewController* view) {
    auto av = qobject_cast<AnnotatedDNAView*>(view);
    auto a = new ADVGlobalAction(av, ":/remote_blast/images/remote_db_request.png", tr("Query NCBI BLAST database..."), 60);
    a->setObjectName("Query NCBI BLAST database");
    a->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_B));
    connect(a, SIGNAL(triggered()), SLOT(sl_showDialog()));

    auto transformIntoPrimerPair = new ADVGlobalAction(av, "", tr("Transform into a primer pair"), 60);
    transformIntoPrimerPair->setObjectName(TRANSFORM_INTO_A_PRIMER_PAIR_NAME);
    transformIntoPrimerPair->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_T));
    transformIntoPrimerPair->setShortcutContext(Qt::WindowShortcut);
    connect(transformIntoPrimerPair, &QAction::triggered, this, &RemoteBLASTViewContext::sl_transformIntoPrimerPair);
    addViewAction(transformIntoPrimerPair);
}

void RemoteBLASTViewContext::buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu) {
    auto av = qobject_cast<AnnotatedDNAView*>(view);
    SAFE_POINT_NN(av, );
    CHECK(av->getActiveSequenceContext()->getAlphabet()->isNucleic(), );

    auto atv = av->getAnnotationsView();
    SAFE_POINT_NN(atv, );

    auto atvw = atv->getTreeWidget();
    SAFE_POINT_NN(atvw, );

    auto items = atvw->selectedItems();
    CHECK(isTransformIntoPrimerPairEnabled(items), );

    auto transformIntoPrimerPair = findViewAction(view, TRANSFORM_INTO_A_PRIMER_PAIR_NAME);
    SAFE_POINT_NN(transformIntoPrimerPair, );

    auto editMenu = GUIUtils::findSubMenu(menu, ADV_MENU_EDIT);
    SAFE_POINT_NN(editMenu, );

    editMenu->addAction(transformIntoPrimerPair);
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
    SAFE_POINT_NN(av, );

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
    CHECK(dlg->result() == QDialog::Accepted, );

    RemoteBLASTTaskSettings cfg = dlg->cfg;
    auto seqObj = seqCtx->getSequenceObject();
    SAFE_POINT_NN(seqObj, );

    cfg.isCircular = seqObj->isCircular();
    DNATranslation* aminoT = dlg->translateToAmino ? seqCtx->getAminoTT() : 0;
    cfg.aminoT = aminoT;
    DNATranslation* complT = dlg->translateToAmino ? seqCtx->getComplementTT() : 0;
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

static const QString TOP_PRIMERS_ANNOTATIONS_GROUP_NAME = "top_primers";
static const QString PAIR_NAME_BEGINNING = "pair ";

void RemoteBLASTViewContext::sl_transformIntoPrimerPair() {
    auto action = qobject_cast<ADVGlobalAction*>(sender());
    SAFE_POINT_NN(action, );

    auto av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());
    SAFE_POINT_NN(av, );

    auto atv = av->getAnnotationsView();
    SAFE_POINT_NN(atv, );

    auto atvw = atv->getTreeWidget();
    SAFE_POINT_NN(atvw, );

    auto items = atvw->selectedItems();
    SAFE_POINT(items.size() == 2, "Should be two selected items", );

    AVAnnotationItem* firstAvItem = static_cast<AVAnnotationItem*>(items.first());
    SAFE_POINT_NN(firstAvItem, );

    AVAnnotationItem* secondAvItem = static_cast<AVAnnotationItem*>(items.last());
    SAFE_POINT_NN(secondAvItem, );

    // To simplify the case, primers should be in the same Annotation Table
    auto ato = firstAvItem->getAnnotationTableObject();
    SAFE_POINT_NN(ato, );

    auto secondAto = secondAvItem->getAnnotationTableObject();
    SAFE_POINT_NN(secondAto, );

    if (ato != secondAto) {
        coreLog.error(tr("Selected annotations belongs to different tables"));
        return;
    }

    auto createPrimerAnnotation = [](const SharedAnnotationData& originalAnnotation) -> SharedAnnotationData {
        SharedAnnotationData result(originalAnnotation);
        result->name = TOP_PRIMERS_ANNOTATIONS_GROUP_NAME;
        result->type = U2FeatureTypes::Primer;

        return result;
    };

    auto forwardAnnData = createPrimerAnnotation(firstAvItem->annotation->getData());
    auto reverseAnnData = createPrimerAnnotation(secondAvItem->annotation->getData());
    // Make primers "looks to each other"
    if (forwardAnnData->getRegions().first().endPos() < reverseAnnData->getRegions().first().startPos) {
        forwardAnnData->setStrand(U2Strand::Direct);
        reverseAnnData->setStrand(U2Strand::Complementary);
    } else {
        forwardAnnData->setStrand(U2Strand::Complementary);
        reverseAnnData->setStrand(U2Strand::Direct);
    }
    // Calculate the correct pair number
    int existedPairNumber = calculateExistedPrimerPairsNumber(ato);
    QMap<QString, QList<SharedAnnotationData>> resultAnnotations;
    QString movedPrimersGroupName = TOP_PRIMERS_ANNOTATIONS_GROUP_NAME + "/" + PAIR_NAME_BEGINNING + QString::number(existedPairNumber + 1);
    resultAnnotations.insert(movedPrimersGroupName, {forwardAnnData, reverseAnnData});
    auto cat = new CreateAnnotationsTask(ato, resultAnnotations);
    AppContext::getTaskScheduler()->registerTopLevelTask(cat);
}

const QList<QPair<Annotation*, Annotation*>> RemoteBLASTViewContext::getSelectedPrimerPairs(AnnotationGroupSelection* ags) {
    auto annGroups = ags->getSelection();
    QList<QPair<Annotation*, Annotation*>> primerAnnotationPairs;
    for (auto primerGroup : qAsConst(annGroups)) {
        auto primerAnnotations = primerGroup->getAnnotations();
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

bool RemoteBLASTViewContext::isTransformIntoPrimerPairEnabled(const QList<QTreeWidgetItem*>& items) {
    // There are two primers - the left one and the right one.
    CHECK(items.size() == 2, false);

    // The selected item should be an annotations (not a group or a qualifier)
    // and shouldn't already be a primer.
    // Also, to simplify everything, only writable objects are supported.
    auto getAvItem = [](QTreeWidgetItem* item) -> AVAnnotationItem* {
        auto avAnnItem = static_cast<AVAnnotationItem*>(item);
        CHECK(!avAnnItem->isReadonly(), nullptr);
        CHECK(avAnnItem->type == AVItemType::AVItemType_Annotation, nullptr);
        CHECK(avAnnItem->annotation->getName() != "top_primers", nullptr);

        return avAnnItem;
    };

    auto firstItem = getAvItem(items.first());
    CHECK(firstItem != nullptr, false);

    auto secondItem = getAvItem(items.last());
    CHECK(secondItem != nullptr, false);

    // Primers have only one region and never intersects each other
    auto regionsAreOk = [](const QVector<U2Region>& firstRegions, const QVector<U2Region>& secondRegions) -> bool {
        CHECK(firstRegions.size() == 1 && secondRegions.size() == 1, false);

        return !firstRegions.first().intersects(secondRegions.first());
    };
    auto firstRegions = firstItem->annotation->getRegions();
    auto secondRegions = secondItem->annotation->getRegions();
    CHECK(regionsAreOk(firstRegions, secondRegions), false);

    return true;
}

int RemoteBLASTViewContext::calculateExistedPrimerPairsNumber(AnnotationTableObject* ato) {
    int movedPairsNumber = 0;
    auto rootGroup = ato->getRootGroup();
    SAFE_POINT_NN(rootGroup, 0);

    auto primer3ResultsGroup = rootGroup->getSubgroup(TOP_PRIMERS_ANNOTATIONS_GROUP_NAME, false);
    if (primer3ResultsGroup != nullptr) {
        auto pairGroups = primer3ResultsGroup->getSubgroups();
        for (auto pairGroup : qAsConst(pairGroups)) {
            auto name = pairGroup->getName();
            CHECK_CONTINUE(name.startsWith(PAIR_NAME_BEGINNING));

            auto orderNumberString = name.mid(PAIR_NAME_BEGINNING.size());
            bool ok = false;
            int orderNumber = orderNumberString.toInt(&ok);
            CHECK_CONTINUE(ok);

            movedPairsNumber = qMax(movedPairsNumber, orderNumber);
        }
    }

    return movedPairsNumber;
}

QList<XMLTestFactory*> RemoteBLASTPluginTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_RemoteBLAST::createFactory());
    return res;
}

}  // namespace U2
