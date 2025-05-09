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

#include "FindEnzymesTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GHints.h>
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

FindEnzymesToAnnotationsTask::FindEnzymesToAnnotationsTask(AnnotationTableObject* aobj, const U2EntityRef& seqRef, const QList<SEnzymeData>& enzymes, const FindEnzymesTaskConfig& config)
    : Task(tr("Find and store enzymes"), TaskFlags_NR_FOSCOE), dnaSeqRef(seqRef), enzymes(enzymes), annotationObject(aobj), cfg(config) {
    GCOUNTER(cvar, "FindEnzymesToAnnotationsTask");
}

void FindEnzymesToAnnotationsTask::prepare() {
    CHECK_EXT(!enzymes.isEmpty(), stateInfo.setError(tr("No enzymes selected.")), );
    QVector<U2Region> searchRegions = cfg.searchRegions;
    if (cfg.searchRegions.isEmpty()) {
        U2SequenceObject sequenceObject("sequence", dnaSeqRef);
        searchRegions = {U2Region(0, sequenceObject.getSequenceLength())};
    }
    QVector<U2Region> excludedRegions = cfg.excludeMode ? cfg.excludedRegions : QVector<U2Region>();
    for (const U2Region searchRegion : qAsConst(searchRegions)) {
        FindEnzymesTask* findTask = new FindEnzymesTask(dnaSeqRef, searchRegion, excludedRegions, enzymes, cfg.maxResults, cfg.circular);
        searchEnzymesTasks.append(findTask);
        addSubTask(findTask);
    }
}

QList<Task*> FindEnzymesToAnnotationsTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;

    CHECK(searchEnzymesTasks.contains(subTask), result);
    CHECK_OP(stateInfo, result);
    CHECK_EXT(!annotationObject.isNull(), stateInfo.setError(tr("Annotation table does not exist")), result);
    CHECK_EXT(!annotationObject->isStateLocked(), stateInfo.setError(tr("Annotation table is read-only")), result);
    FindEnzymesTask* findTask = qobject_cast<FindEnzymesTask*>(subTask);
    CHECK_EXT(findTask != nullptr, stateInfo.setError(L10N::nullPointerError("FindEnzymesTask")), result);

    bool useSubgroups = enzymes.size() > 1 || cfg.groupName.isEmpty();
    QMap<QString, QList<SharedAnnotationData>> annotationsByGroupMap;
    for (const SEnzymeData& enzyme : qAsConst(enzymes)) {
        QList<SharedAnnotationData> resultAnnotationList = findTask->getResultsAsAnnotations(enzyme->id);
        if (resultAnnotationList.size() >= cfg.minHitCount && resultAnnotationList.size() <= cfg.maxHitCount) {
            QString group = useSubgroups ? cfg.groupName + "/" + enzyme->id : cfg.groupName;
            annotationsByGroupMap[group].append(resultAnnotationList);
        }
    }

    result << new CreateAnnotationsTask(annotationObject, annotationsByGroupMap);
    return result;
}

Task::ReportResult FindEnzymesToAnnotationsTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    if (!annotationObject.isNull() && annotationObject->getAnnotations().isEmpty() && !cfg.isAutoAnnotationUpdateTask) {
        // no results found -> delete empty annotation document
        Project* proj = AppContext::getProject();
        if (proj != nullptr) {
            Document* toDelete = nullptr;
            QList<Document*> docs = proj->getDocuments();
            for (Document* doc : qAsConst(docs)) {
                if (doc->getObjects().contains(annotationObject)) {
                    toDelete = doc;
                    break;
                }
            }
            if (toDelete != nullptr) {
                proj->removeDocument(toDelete);
            }
        }
        annotationObject = nullptr;
        stateInfo.setError("Enzymes selection is not found");
    }

    return ReportResult_Finished;
}

static int getDirectCutOffset(const SEnzymeData& enzyme) {
    int result = 0;
    if (enzyme->cutDirect != ENZYME_CUT_UNKNOWN) {
        result = qMax(result, enzyme->cutDirect);
    }
    if (enzyme->secondCutDirect != ENZYME_CUT_UNKNOWN) {
        result = qMax(result, enzyme->cutDirect);
    }
    if (result == 0) {
        result = enzyme->seq.size();
    }
    return result;
}

static int getComplementCutOffset(const SEnzymeData& enzyme) {
    int result = 0;
    if (enzyme->cutComplement != ENZYME_CUT_UNKNOWN) {
        result = qMax(result, enzyme->cutComplement);
    }
    if (enzyme->secondCutComplement != ENZYME_CUT_UNKNOWN) {
        result = qMax(result, enzyme->secondCutComplement);
    }
    if (result == 0) {
        result = enzyme->seq.size();
    }
    if (enzyme->cutComplement < 0 || enzyme->secondCutComplement < 0) {
        return result;
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// find multiple enzymes task
FindEnzymesTask::FindEnzymesTask(const U2EntityRef& seqRef_, const U2Region& region_, const QVector<U2Region>& excludeRegions_, 
                                 const QList<SEnzymeData>& enzymes_, int mr, bool circular)
    : Task(tr("Find Enzymes"), TaskFlags_NR_FOSCOE),
      maxResults(mr),
      isCircular(circular),
      seqlen(0),
      countOfResultsInMap(0),
      enzymes(enzymes_),
      seqRef(seqRef_),
      region(region_),
      excludeRegions(excludeRegions_) {
    U2SequenceObject seq("sequence", seqRef);
    SAFE_POINT(seq.getAlphabet()->isNucleic(), "Alphabet is not nucleic.", );
    seqlen = seq.getSequenceLength();
    
    if (excludeRegions.isEmpty()) {
        QList<Task*> result;
        for (const SEnzymeData& enzyme : qAsConst(enzymes)) {
            addSubTask(new FindSingleEnzymeTask(seqRef, region, enzyme, this, isCircular));
        }
    } else {
        for (U2Region excludeRegion : qAsConst(excludeRegions)) {
            for (const SEnzymeData enzyme : qAsConst(enzymes)) {
                //maximum offset for direct 'cutter' of enzyme
                const int leftExtension = getDirectCutOffset(enzyme);
                //maximum offset for complement 'cutter' of enzyme
                const int rightExtension = getComplementCutOffset(enzyme);
                const int seqLength = seq.getSequenceLength();
                if (excludeRegion.startPos - leftExtension < 0) {                        
                    excludeRegion.startPos = isCircular ? seqLength - (leftExtension - excludeRegion.startPos) : 0;
                } else {
                    excludeRegion.startPos -= leftExtension;
                }
                excludeRegion.length += leftExtension + rightExtension;
                if (excludeRegion.endPos() > seqLength && !isCircular) {
                    excludeRegion.length = seqLength - excludeRegion.startPos;
                }
                if (excludeRegion.length >= seqLength) {
                    algoLog.info(
                    tr("Excluded search region with enzyme offsets equal or larger than whole sequence. %1 enzyme search skipped.")
                    .arg(enzyme->id));
                    continue;
                }
                excludeSearchTasksRunningCounter[enzyme->id] += 1;
                addSubTask(new FindSingleEnzymeTask(seqRef, excludeRegion, enzyme, nullptr, isCircular, 1, false));
            }
        }
    }
}

QList<Task*> FindEnzymesTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(!excludeSearchTasksRunningCounter.isEmpty(), result);
    FindSingleEnzymeTask* findSingleEnzymeTask = qobject_cast<FindSingleEnzymeTask*>(subTask);
    SAFE_POINT(findSingleEnzymeTask != nullptr, L10N::nullPointerError("FindSingleEnzymeTask"), result);
    const SEnzymeData& enzyme = findSingleEnzymeTask->getEnzyme();
    excludeSearchTasksRunningCounter[enzyme->id] -= 1;
    CHECK_EXT(findSingleEnzymeTask->getResults().isEmpty(), enzymesFoundInExcludedRegion << enzyme->id, result);
    CHECK(excludeSearchTasksRunningCounter[enzyme->id] == 0 && !enzymesFoundInExcludedRegion.contains(enzyme->id), result);
    result << new FindSingleEnzymeTask(seqRef, region, enzyme, this, isCircular);
    return result;
}

void FindEnzymesTask::onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand, bool&) {
    CHECK_OP(stateInfo, );
    QMutexLocker locker(&resultsLock);
    if (pos > seqlen) {
        pos %= seqlen;
    }
    if (countOfResultsInMap > maxResults) {
        if (!isCanceled()) {
            stateInfo.setError(tr("Number of results exceed %1, stopping").arg(maxResults));
            cancel();
        }
        return;
    }
    searchResultMap[enzyme->id] << FindEnzymesAlgResult(enzyme, pos, strand);
    countOfResultsInMap++;
}

QList<SharedAnnotationData> FindEnzymesTask::getResultsAsAnnotations(const QString& enzymeId) const {
    CHECK_OP(stateInfo, {});
    QList<SharedAnnotationData> res;
    QString cutStr;
    QString secondCutStr;
    QString dbxrefStr;
    QList<FindEnzymesAlgResult> searchResultList = searchResultMap.value(enzymeId);
    for (const FindEnzymesAlgResult& searchResult : qAsConst(searchResultList)) {
        const SEnzymeData& enzyme = searchResult.enzyme;
        if (!enzyme->accession.isEmpty()) {
            QString accession = enzyme->accession;
            if (accession.startsWith("RB")) {
                accession = accession.mid(2);
            }
            while (accession.startsWith("0")) {
                accession.remove(0, 1);
            }
            dbxrefStr = "REBASE:" + accession;
        } else if (!enzyme->id.isEmpty()) {
            dbxrefStr = "REBASE:" + enzyme->id;
        }
        auto generateCutQualifier = [](int cutDirect, int cutComplement) -> QString {
            CHECK(cutDirect != ENZYME_CUT_UNKNOWN, QString());

            QString result;
            result = QString::number(cutDirect);
            if (cutComplement != ENZYME_CUT_UNKNOWN && cutComplement != cutDirect) {
                result += "/" + QString::number(cutComplement);
            }

            return result;
        };
        cutStr = generateCutQualifier(enzyme->cutDirect, enzyme->cutComplement);
        secondCutStr = generateCutQualifier(enzyme->secondCutDirect, enzyme->secondCutComplement);
        break;
    }

    for (const FindEnzymesAlgResult& searchResult : qAsConst(searchResultList)) {
        const SEnzymeData& enzyme = searchResult.enzyme;
        SharedAnnotationData ad(new AnnotationData);
        if (isCircular && searchResult.pos + enzyme->seq.size() > seqlen) {
            CHECK_CONTINUE(seqlen >= searchResult.pos);
            qint64 firstRegionLength = seqlen - searchResult.pos;
            if (firstRegionLength != 0) {
                ad->location->regions << U2Region(searchResult.pos, firstRegionLength);
            }
            ad->location->regions << U2Region(0, enzyme->seq.size() - firstRegionLength);
        } else {
            ad->location->regions << U2Region(searchResult.pos, enzyme->seq.size());
        }
        ad->type = U2FeatureTypes::RestrictionSite;
        ad->name = enzyme->id;
        ad->setStrand(searchResult.strand);
        if (!dbxrefStr.isEmpty()) {
            ad->qualifiers.append(U2Qualifier("db_xref", dbxrefStr));
        }
        if (!cutStr.isEmpty()) {
            ad->qualifiers.append(U2Qualifier(GBFeatureUtils::QUALIFIER_CUT, cutStr));
        }
        if (!secondCutStr.isEmpty()) {
            ad->qualifiers.append(U2Qualifier(GBFeatureUtils::QUALIFIER_SECOND_CUT, secondCutStr));
        }

        res.append(ad);
    }
    return res;
}

Task::ReportResult FindEnzymesTask::report() {
    if (!hasError() && !isCanceled()) {
        if (!enzymesFoundInExcludedRegion.isEmpty()) {
            algoLog.info(tr("The following enzymes were found, but skipped because they were found inside of the \"Uncut area\": %1.")
                         .arg(enzymesFoundInExcludedRegion.values().join(",")));
        }
        algoLog.info(tr("Found %1 restriction sites").arg(countOfResultsInMap));
    }
    return ReportResult_Finished;
}

void FindEnzymesTask::cleanup() {
    if (hasError()) {
        searchResultMap.clear();
    }
}

//////////////////////////////////////////////////////////////////////////
// find single enzyme task
FindSingleEnzymeTask::FindSingleEnzymeTask(const U2EntityRef& sequenceObjectRef, const U2Region& region, const SEnzymeData& enzyme, FindEnzymesAlgListener* l, bool isCircular, int maxResults, bool _cancelOnMaxResults)
    : Task(tr("Find enzyme '%1'").arg(enzyme->id), TaskFlag_NoRun),
      sequenceObjectRef(sequenceObjectRef),
      region(region),
      enzyme(enzyme),
      maxResults(maxResults),
      resultListener(l),
      isCircular(isCircular),
      cancelOnMaxResults(_cancelOnMaxResults) {
}

void FindSingleEnzymeTask::prepare() {
    U2SequenceObject dnaSeq("sequence", sequenceObjectRef);

    SAFE_POINT(dnaSeq.getAlphabet()->isNucleic(), "Alphabet is not nucleic.", );
    if (resultListener == nullptr) {
        resultListener = this;
    }

    const int BLOCK_READ_FROM_DB = 128000;
    static const int chunkSize = BLOCK_READ_FROM_DB;

    SequenceDbiWalkerConfig sequenceWalkerConfig;
    sequenceWalkerConfig.seqRef = sequenceObjectRef;
    sequenceWalkerConfig.range = region;
    int enzymeFullLength = enzyme->getFullLength();
    sequenceWalkerConfig.chunkSize = qMax(enzymeFullLength, chunkSize);
    sequenceWalkerConfig.lastChunkExtraLen = sequenceWalkerConfig.chunkSize / 2;
    sequenceWalkerConfig.overlapSize = enzymeFullLength - 1;
    sequenceWalkerConfig.walkCircular = isCircular;
    sequenceWalkerConfig.walkCircularDistance = sequenceWalkerConfig.overlapSize;

    addSubTask(new SequenceDbiWalkerTask(sequenceWalkerConfig, this, tr("Find enzyme '%1' parallel").arg(enzyme->id)));
}

void FindSingleEnzymeTask::onResult(int pos, const SEnzymeData& enzyme, const U2Strand& strand, bool& stop) {
    CHECK_OP(stateInfo, );
    QMutexLocker locker(&resultsLock);
    CHECK_EXT(resultList.size() > maxResults, resultList.append(FindEnzymesAlgResult(enzyme, pos, strand)), );    
    if (cancelOnMaxResults) {
        if (!isCanceled()) {
            stateInfo.setError(FindEnzymesTask::tr("Number of results exceed %1, stopping").arg(maxResults));
            cancel();
        }
    } else {
        stop = true;
    }
    stoppedOnMaxResults = true;
}

void FindSingleEnzymeTask::onRegion(SequenceDbiWalkerSubtask* t, TaskStateInfo& ti) {
    CHECK_OP(ti, );
    if (enzyme->seq.isEmpty()) {
        return;
    }
    U2SequenceObject dnaSequenceObject("sequence", sequenceObjectRef);
    qint64 sequenceLen = dnaSequenceObject.getSequenceLength();
    if (sequenceLen < enzyme->seq.length()) {
        return;
    }
    SAFE_POINT(enzyme->alphabet != nullptr, "No enzyme alphabet", );
    if (!enzyme->alphabet->isNucleic()) {
        algoLog.info(tr("Non-nucleic enzyme alphabet: %1, enzyme: %2, skipping..").arg(enzyme->alphabet->getId()).arg(enzyme->id));
        return;
    }

    const DNAAlphabet* seqAlphabet = dnaSequenceObject.getAlphabet();
    SAFE_POINT(seqAlphabet != nullptr, "Failed to get sequence alphabet", );

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
    resultList.clear();
}

SEnzymeData FindSingleEnzymeTask::getEnzyme() const {
    return enzyme;
}

bool FindSingleEnzymeTask::wasStoppedOnMaxResults() const {
    return stoppedOnMaxResults;
}

qint64 FindSingleEnzymeTask::estimateNumberOfEnzymesInSequence(qint64 sequenceLength, int variants) {
    // The rough estimation experimentally received from test on large DNA files : 1 enzymes can be found 5 times per 1000bp in both direct and complement strands.
    return qRound(5 * (sequenceLength / 1000.0) * variants);
}

//////////////////////////////////////////////////////////////////////////
// find enzymes auto annotation updater

FindEnzymesAutoAnnotationUpdater::FindEnzymesAutoAnnotationUpdater()
    : AutoAnnotationsUpdater(tr("Restriction Sites"), ANNOTATION_GROUP_ENZYME) {
}

Task* FindEnzymesAutoAnnotationUpdater::createAutoAnnotationsUpdateTask(const AutoAnnotationObject* annotationObject) {
    Settings* appSettings = AppContext::getSettings();
    QString selectedEnzymesString = appSettings->getValue(EnzymeSettings::LAST_SELECTION).toString();
    if (selectedEnzymesString.isEmpty()) {
        selectedEnzymesString = EnzymeSettings::COMMON_ENZYMES;
    }

    QList<SEnzymeData> allEnzymesList = EnzymesIO::getDefaultEnzymesList();
    QStringList selectedEnzymeIdList = selectedEnzymesString.split(ENZYME_LIST_SEPARATOR);
    QList<SEnzymeData> selectedEnzymes;
    for (const QString& id : qAsConst(selectedEnzymeIdList)) {
        for (const SEnzymeData& enzyme : allEnzymesList) {
            if (id == enzyme->id) {
                selectedEnzymes.append(enzyme);
            }
        }
    }

    U2SequenceObject* sequenceObject = annotationObject->getSequenceObject();
    qint64 sequenceLength = sequenceObject->getSequenceLength();
    if (selectedEnzymes.isEmpty()) {
        return nullptr;
    }
    if (isTooManyAnnotationsInTheResult(sequenceLength, selectedEnzymes.size())) {
        uiLog.trace("Find-enzymes task won't start, too many estimated results");
        return nullptr;
    }

    FindEnzymesTaskConfig cfg;
    cfg.circular = sequenceObject->isCircular();
    cfg.groupName = getGroupName();
    cfg.isAutoAnnotationUpdateTask = true;
    cfg.minHitCount = appSettings->getValue(EnzymeSettings::MIN_HIT_VALUE, 1).toInt();
    int maxAnnotations = AUTO_ANNOTATION_MAX_ANNOTATIONS_ADV_CAN_HANDLE;
    if (qgetenv("UGENE_DISABLE_ENZYMES_OVERFLOW_CHECK") == "1") {
        maxAnnotations = INT_MAX;
    }
    cfg.maxHitCount = qMin(appSettings->getValue(EnzymeSettings::MAX_HIT_VALUE, maxAnnotations).toInt(), maxAnnotations);
    cfg.maxResults = qMin(appSettings->getValue(EnzymeSettings::MAX_RESULTS, maxAnnotations).toInt(), maxAnnotations);

    const QVector<U2Region> savedSearchRegions = getLastSearchLocationForObject(sequenceObject).data()->regions;
    U2Region wholeSequenceRegion(0, sequenceLength);
    for (const U2Region& region : qAsConst(savedSearchRegions)) {
        if (cfg.circular) {
            // In circular mode the region can have an overflow to handle end/start positions correctly
            cfg.searchRegions << U2Region(region.startPos, qMin(region.length, wholeSequenceRegion.length));
        } else {
            cfg.searchRegions << region.intersect(wholeSequenceRegion);
        }
    }
    if (cfg.searchRegions.isEmpty()) {
        cfg.searchRegions = {wholeSequenceRegion};
    }

    const QVector<U2Region> savedExcludedRegions = getLastExcludeLocationForObject(sequenceObject).data()->regions;
    if (!savedExcludedRegions.isEmpty()) {
        cfg.excludedRegions = savedExcludedRegions;
    }
    cfg.excludeMode = getExcludeModeEnabledForObject(sequenceObject);

    AnnotationTableObject* annotationTableObject = annotationObject->getAnnotationObject();
    const U2EntityRef& sequenceObjectRef = sequenceObject->getEntityRef();
    return new FindEnzymesToAnnotationsTask(annotationTableObject, sequenceObjectRef, selectedEnzymes, cfg);
}

bool FindEnzymesAutoAnnotationUpdater::checkConstraints(const AutoAnnotationConstraints& constraints) {
    return constraints.alphabet != nullptr && constraints.alphabet->isNucleic();
}

/** Search (include) and exclude regions are saved per-sequence object in the persistent object hints which are saved in the project file. */
#define ENZYMES_SEARCH_LOCATION "FindEnzymes_searchLocation"
#define ENZYMES_EXCLUDE_LOCATION "FindEnzymes_excludeLocation"
#define ENZYMES_EXCLUDE_MODE "FindEnzymes_excludeMode"

static void setLocationToHints(GObject* object, const QString& hintName, const U2Location& location) {
    GHints* objectHints = object->getGHints();
    if (location.data()->regions.isEmpty()) {
        objectHints->remove(hintName);  // same as default, no need to save.
        return;
    }
    objectHints->set(hintName, QVariant::fromValue<U2Location>(location));
}

static U2Location getLocationFromHints(const GObject* object, const QString& hintName) {
    GHints* objectHints = object->getGHints();
    return objectHints->get(hintName, QVariant::fromValue<U2Location>(U2Location())).value<U2Location>();
}

U2Location FindEnzymesAutoAnnotationUpdater::getLastSearchLocationForObject(const U2SequenceObject* sequenceObject) {
    return getLocationFromHints(sequenceObject, ENZYMES_SEARCH_LOCATION);
}

void FindEnzymesAutoAnnotationUpdater::setLastSearchLocationForObject(U2SequenceObject* sequenceObject, const U2Location& location) {
    setLocationToHints(sequenceObject, ENZYMES_SEARCH_LOCATION, location);
}

U2::U2Location FindEnzymesAutoAnnotationUpdater::getLastExcludeLocationForObject(const U2SequenceObject* sequenceObject) {
    return getLocationFromHints(sequenceObject, ENZYMES_EXCLUDE_LOCATION);
}

void FindEnzymesAutoAnnotationUpdater::setLastExcludeLocationForObject(U2SequenceObject* sequenceObject, const U2Location& location) {
    setLocationToHints(sequenceObject, ENZYMES_EXCLUDE_LOCATION, location);
}

bool FindEnzymesAutoAnnotationUpdater::isTooManyAnnotationsInTheResult(qint64 sequenceLength, int countOfEnzymeVariants) {
    if (qgetenv("UGENE_DISABLE_ENZYMES_OVERFLOW_CHECK") == "1") {
        return false;
    }
    qint64 maxResultsEstimation = FindSingleEnzymeTask::estimateNumberOfEnzymesInSequence(sequenceLength, countOfEnzymeVariants);
    return maxResultsEstimation > AUTO_ANNOTATION_MAX_ANNOTATIONS_ADV_CAN_HANDLE;
}

void FindEnzymesAutoAnnotationUpdater::setExcludeModeEnabledForObject(U2SequenceObject* sequenceObject, bool enabled) {
    GHints* objectHints = sequenceObject->getGHints();
    objectHints->set(ENZYMES_EXCLUDE_MODE, QVariant::fromValue<bool>(enabled));
}

bool FindEnzymesAutoAnnotationUpdater::getExcludeModeEnabledForObject(U2SequenceObject* sequenceObject) {
    GHints* objectHints = sequenceObject->getGHints();
    return objectHints->get(ENZYMES_EXCLUDE_MODE, false).value<bool>();
}

}  // namespace U2
