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

#include "FindEnzymesTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2SafePoints.h>

#include "EnzymesIO.h"

namespace U2 {

/* TRANSLATOR U2::FindEnzymesTask */

//////////////////////////////////////////////////////////////////////////
// enzymes -> annotations

FindEnzymesToAnnotationsTask::FindEnzymesToAnnotationsTask(AnnotationTableObject *aobj, const U2EntityRef &seqRef, const QList<SEnzymeData> &enzymes, const FindEnzymesTaskConfig &config)
    : Task(tr("Find and store enzymes"), TaskFlags_NR_FOSCOE), dnaSeqRef(seqRef), enzymes(enzymes), aObj(aobj), cfg(config), fTask(NULL) {
    GCOUNTER(cvar, tvar, "FindEnzymesToAnnotationsTask");
}

void FindEnzymesToAnnotationsTask::prepare() {
    CHECK_EXT(!enzymes.isEmpty(), stateInfo.setError(tr("No enzymes selected.")), );
    U2SequenceObject dnaSeq("sequence", dnaSeqRef);
    fTask = new FindEnzymesTask(dnaSeqRef, cfg.searchRegion.isEmpty() ? U2Region(0, dnaSeq.getSequenceLength()) : cfg.searchRegion, enzymes, cfg.maxResults, cfg.circular, cfg.excludedRegions);
    addSubTask(fTask);
}

QList<Task *> FindEnzymesToAnnotationsTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> result;

    CHECK(subTask == fTask, result);
    CHECK_OP(stateInfo, result);
    CHECK_EXT(!aObj.isNull(), stateInfo.setError(tr("Annotation table does not exist")), result);
    CHECK_EXT(!aObj->isStateLocked(), stateInfo.setError(tr("Annotation table is read-only")), result);

    bool useSubgroups = enzymes.size() > 1 || cfg.groupName.isEmpty();
    QMap<QString, QList<SharedAnnotationData>> resultMap;
    foreach (const SEnzymeData &ed, enzymes) {
        QList<SharedAnnotationData> anns = fTask->getResultsAsAnnotations(ed->id);
        if (anns.size() >= cfg.minHitCount && anns.size() <= cfg.maxHitCount) {
            QString group = useSubgroups ? cfg.groupName + "/" + ed->id : cfg.groupName;
            resultMap[group].append(anns);
        }
    }

    result << new CreateAnnotationsTask(aObj, resultMap);
    return result;
}

Task::ReportResult FindEnzymesToAnnotationsTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    if (!aObj.isNull() && aObj->getAnnotations().isEmpty() && !cfg.isAutoAnnotationUpdateTask) {
        // no results found -> delete empty annotation document
        Project *proj = AppContext::getProject();
        if (NULL != proj) {
            Document *toDelete = NULL;
            QList<Document *> docs = proj->getDocuments();
            foreach (Document *doc, docs) {
                if (doc->getObjects().contains(aObj)) {
                    toDelete = doc;
                    break;
                }
            }
            if (toDelete != NULL) {
                proj->removeDocument(toDelete);
            }
        }
        aObj = NULL;
        stateInfo.setError("Enzymes selection is not found");
    }

    return ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
// find multiple enzymes task
FindEnzymesTask::FindEnzymesTask(const U2EntityRef &seqRef, const U2Region &region, const QList<SEnzymeData> &enzymes, int mr, bool _circular, QVector<U2Region> excludedRegions)
    : Task(tr("Find Enzymes"), TaskFlags_NR_FOSCOE),
      maxResults(mr),
      excludedRegions(excludedRegions),
      circular(_circular),
      seqlen(0) {
    U2SequenceObject seq("sequence", seqRef);

    SAFE_POINT(seq.getAlphabet()->isNucleic(), tr("Alphabet is not nucleic."), );
    seqlen = seq.getSequenceLength();
    //for every enzymes in selection create FindSingleEnzymeTask
    foreach (const SEnzymeData &e, enzymes) {
        addSubTask(new FindSingleEnzymeTask(seqRef, region, e, this, circular));
    }
}

void FindEnzymesTask::onResult(int pos, const SEnzymeData &enzyme, const U2Strand &strand) {
    if (pos > seqlen) {
        pos %= seqlen;
    }
    foreach (const U2Region &r, excludedRegions) {
        if (U2Region(pos, enzyme->seq.length()).intersects(r)) {
            return;
        }
    }

    QMutexLocker l(&resultsLock);
    if (results.size() > maxResults) {
        if (!isCanceled()) {
            stateInfo.setError(tr("Number of results exceed %1, stopping").arg(maxResults));
            cancel();
        }
        return;
    }
    results.append(FindEnzymesAlgResult(enzyme, pos, strand));
}

QList<SharedAnnotationData> FindEnzymesTask::getResultsAsAnnotations(const QString &enzymeId) const {
    QList<SharedAnnotationData> res;
    if (hasError() || isCanceled()) {
        return res;
    }
    QString cutStr;
    QString dbxrefStr;
    foreach (const FindEnzymesAlgResult &r, results) {
        if (r.enzyme->id != enzymeId) {
            continue;
        }
        if (!r.enzyme->accession.isEmpty()) {
            QString accession = r.enzyme->accession;
            if (accession.startsWith("RB")) {
                accession = accession.mid(2);
            }
            dbxrefStr = "REBASE:" + accession;
        } else if (!r.enzyme->id.isEmpty()) {
            dbxrefStr = "REBASE:" + r.enzyme->id;
        }
        if (r.enzyme->cutDirect != ENZYME_CUT_UNKNOWN) {
            cutStr = QString::number(r.enzyme->cutDirect);
            if (r.enzyme->cutComplement != ENZYME_CUT_UNKNOWN && r.enzyme->cutComplement != r.enzyme->cutDirect) {
                cutStr += "/" + QString::number(r.enzyme->cutComplement);
            }
        }
        break;
    }

    foreach (const FindEnzymesAlgResult &r, results) {
        if (r.enzyme->id == enzymeId) {
            if (circular && r.pos + r.enzyme->seq.size() > seqlen) {
                if (seqlen < r.pos) {
                    continue;
                }
                SharedAnnotationData ad(new AnnotationData);
                ad->type = U2FeatureTypes::RestrictionSite;
                ad->name = r.enzyme->id;
                qint64 firstRegionLength = seqlen - r.pos;
                if (firstRegionLength != 0) {
                    ad->location->regions << U2Region(r.pos, firstRegionLength);
                }
                ad->location->regions << U2Region(0, r.enzyme->seq.size() - firstRegionLength);
                ad->setStrand(r.strand);
                if (!dbxrefStr.isEmpty()) {
                    ad->qualifiers.append(U2Qualifier("db_xref", dbxrefStr));
                }
                if (!cutStr.isEmpty()) {
                    ad->qualifiers.append(U2Qualifier(GBFeatureUtils::QUALIFIER_CUT, cutStr));
                }
                res.append(ad);
            } else {
                SharedAnnotationData ad(new AnnotationData);
                ad->type = U2FeatureTypes::RestrictionSite;
                ad->name = r.enzyme->id;
                ad->location->regions << U2Region(r.pos, r.enzyme->seq.size());
                ad->setStrand(r.strand);
                if (!dbxrefStr.isEmpty()) {
                    ad->qualifiers.append(U2Qualifier("db_xref", dbxrefStr));
                }
                if (!cutStr.isEmpty()) {
                    ad->qualifiers.append(U2Qualifier(GBFeatureUtils::QUALIFIER_CUT, cutStr));
                }
                res.append(ad);
            }
        }
    }
    return res;
}

Task::ReportResult FindEnzymesTask::report() {
    if (!hasError() && !isCanceled()) {
        algoLog.info(tr("Found %1 restriction sites").arg(results.count()));
    }
    return ReportResult_Finished;
}

void FindEnzymesTask::cleanup() {
    if (hasError()) {
        results.clear();
    }
}

//////////////////////////////////////////////////////////////////////////
// find single enzyme task
FindSingleEnzymeTask::FindSingleEnzymeTask(const U2EntityRef &_seqRef, const U2Region &region, const SEnzymeData &_enzyme, FindEnzymesAlgListener *l, bool _circular, int mr)
    : Task(tr("Find enzyme '%1'").arg(_enzyme->id), TaskFlag_NoRun),
      dnaSeqRef(_seqRef),
      region(region),
      enzyme(_enzyme),
      maxResults(mr),
      resultListener(l),
      circular(_circular) {
    U2SequenceObject dnaSeq("sequence", dnaSeqRef);

    SAFE_POINT(dnaSeq.getAlphabet()->isNucleic(), tr("Alphabet is not nucleic."), );
    if (resultListener == NULL) {
        resultListener = this;
    }

    const int BLOCK_READ_FROM_DB = 128000;
    static const int chunkSize = BLOCK_READ_FROM_DB;

    SequenceDbiWalkerConfig swc;
    swc.seqRef = dnaSeqRef;
    swc.range = region;
    swc.chunkSize = qMax(enzyme->seq.size(), chunkSize);
    swc.lastChunkExtraLen = swc.chunkSize / 2;
    swc.overlapSize = enzyme->seq.size() - 1;
    swc.walkCircular = circular;
    swc.walkCircularDistance = swc.overlapSize;

    addSubTask(new SequenceDbiWalkerTask(swc, this, tr("Find enzyme '%1' parallel").arg(enzyme->id)));
}

void FindSingleEnzymeTask::onResult(int pos, const SEnzymeData &enzyme, const U2Strand &strand) {
    if (circular && pos >= region.length) {
        return;
    }
    QMutexLocker l(&resultsLock);
    if (results.size() > maxResults) {
        if (!isCanceled()) {
            stateInfo.setError(FindEnzymesTask::tr("Number of results exceed %1, stopping").arg(maxResults));
            cancel();
        }
        return;
    }
    results.append(FindEnzymesAlgResult(enzyme, pos, strand));
}

void FindSingleEnzymeTask::onRegion(SequenceDbiWalkerSubtask *t, TaskStateInfo &ti) {
    if (enzyme->seq.isEmpty()) {
        return;
    }
    U2SequenceObject dnaSequenceObject("sequence", dnaSeqRef);
    qint64 sequenceLen = dnaSequenceObject.getSequenceLength();
    if (sequenceLen < enzyme->seq.length()) {
        return;
    }
    SAFE_POINT(enzyme->alphabet != NULL, tr("No enzyme alphabet"), );
    if (!enzyme->alphabet->isNucleic()) {
        algoLog.info(tr("Non-nucleic enzyme alphabet: %1, enzyme: %2, skipping..").arg(enzyme->alphabet->getId()).arg(enzyme->id));
        return;
    }

    const DNAAlphabet *seqAlphabet = dnaSequenceObject.getAlphabet();
    SAFE_POINT(seqAlphabet != NULL, tr("Failed to get sequence alphabet"), );

    bool useExtendedComparator = enzyme->alphabet->getId() == BaseDNAAlphabetIds::NUCL_DNA_EXTENDED() || seqAlphabet->getId() == BaseDNAAlphabetIds::NUCL_DNA_EXTENDED() || seqAlphabet->getId() == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT() || seqAlphabet->getId() == BaseDNAAlphabetIds::NUCL_RNA_EXTENDED();

    U2Region chunkRegion = t->getGlobalRegion();
    DNASequence dnaSeq;
    if (U2Region(0, sequenceLen).contains(chunkRegion)) {
        dnaSeq = dnaSequenceObject.getSequence(chunkRegion, ti);
    } else {
        U2Region partOne = U2Region(0, sequenceLen).intersect(chunkRegion);
        dnaSeq = dnaSequenceObject.getSequence(partOne, ti);
        CHECK_OP(ti, );
        U2Region partTwo = U2Region(0, chunkRegion.endPos() % sequenceLen);
        dnaSeq.seq.append(dnaSequenceObject.getSequence(partTwo, ti).seq);
    }
    CHECK_OP(ti, );

    // Note that enzymes algorithm filters N symbols in sequence by itself
    if (useExtendedComparator) {
        FindEnzymesAlgorithm<ExtendedDNAlphabetComparator> algo;
        algo.run(dnaSeq, U2Region(0, chunkRegion.length), enzyme, resultListener, ti, chunkRegion.startPos);
    } else {
        FindEnzymesAlgorithm<ExactDNAAlphabetComparatorN1M_N2M> algo;
        algo.run(dnaSeq, U2Region(0, chunkRegion.length), enzyme, resultListener, ti, chunkRegion.startPos);
    }
}

void FindSingleEnzymeTask::cleanup() {
    results.clear();
}

//////////////////////////////////////////////////////////////////////////
// find enzymes auto annotation updater

FindEnzymesAutoAnnotationUpdater::FindEnzymesAutoAnnotationUpdater()
    : AutoAnnotationsUpdater(tr("Restriction Sites"), ANNOTATION_GROUP_ENZYME) {
}

Task *FindEnzymesAutoAnnotationUpdater::createAutoAnnotationsUpdateTask(const AutoAnnotationObject *aa) {
    const QList<SEnzymeData> enzymeList = EnzymesIO::getDefaultEnzymesList();
    QString selStr = AppContext::getSettings()->getValue(EnzymeSettings::LAST_SELECTION).toString();
    if (selStr.isEmpty()) {
        selStr = EnzymeSettings::COMMON_ENZYMES;
    }

    QStringList lastSelection = selStr.split(ENZYME_LIST_SEPARATOR);
    QList<SEnzymeData> selectedEnzymes;
    foreach (const QString id, lastSelection) {
        foreach (const SEnzymeData &enzyme, enzymeList) {
            if (id == enzyme->id) {
                selectedEnzymes.append(enzyme);
            }
        }
    }

    FindEnzymesTaskConfig cfg;
    cfg.circular = aa->getSeqObject()->isCircular();
    cfg.groupName = getGroupName();
    cfg.isAutoAnnotationUpdateTask = true;
    cfg.minHitCount = AppContext::getSettings()->getValue(EnzymeSettings::MIN_HIT_VALUE, 1).toInt();
    cfg.maxHitCount = AppContext::getSettings()->getValue(EnzymeSettings::MAX_HIT_VALUE, INT_MAX).toInt();
    cfg.maxResults = AppContext::getSettings()->getValue(EnzymeSettings::MAX_RESULTS, 500000).toInt();

    U2Region savedSearchRegion = AppContext::getSettings()->getValue(EnzymeSettings::SEARCH_REGION, QVariant::fromValue(U2Region())).value<U2Region>();

    U2SequenceObject *dnaObj = aa->getSeqObject();
    const U2Region wholeSequenceRegion = U2Region(0, dnaObj->getSequenceLength());
    if (cfg.circular) {
        //In circular mode the region can have an overflow to handle end/start positions correctly
        cfg.searchRegion = U2Region(savedSearchRegion.startPos, qMin(savedSearchRegion.length, wholeSequenceRegion.length));
    } else {
        cfg.searchRegion = savedSearchRegion.intersect(wholeSequenceRegion);
    }
    if (cfg.searchRegion.isEmpty()) {
        cfg.searchRegion = wholeSequenceRegion;
    }

    QVector<U2Region> excludedRegions =
        AppContext::getSettings()->getValue(EnzymeSettings::EXCLUDED_REGION, QVariant::fromValue(QVector<U2Region>())).value<QVector<U2Region>>();

    if (!excludedRegions.isEmpty()) {
        cfg.excludedRegions = excludedRegions;
    }

    AnnotationTableObject *aObj = aa->getAnnotationObject();
    const U2EntityRef &dnaRef = aa->getSeqObject()->getEntityRef();
    Task *task = new FindEnzymesToAnnotationsTask(aObj, dnaRef, selectedEnzymes, cfg);

    return task;
}

bool FindEnzymesAutoAnnotationUpdater::checkConstraints(const AutoAnnotationConstraints &constraints) {
    if (constraints.alphabet == NULL) {
        return false;
    }

    return constraints.alphabet->isNucleic();
}

}    // namespace U2
