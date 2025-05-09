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

#include "AnnotatorTests.h"

#include <QDir>
#include <QStringList>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/CollectionUtils.h>

/* TRANSLATOR U2::GTest */

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// GTest_AnnotatorSearch

#define GROUPS_ATTR "groups"
#define REGION_SIZE_ATTR "region_size"
#define FIT_TO_REGION_ATTR "is_fit_to_region"
#define DOC_ATTR "doc"
#define SEQ_ATTR "seq"
#define EXPECTED_RESULTS_ATTR "expected_results"
#define RES_ATTR "result"
#define CIRCULAR_ATTR "circular"

void GTest_AnnotatorSearch::init(XMLTestFormat*, const QDomElement& el) {
    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    seqName = el.attribute(SEQ_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQ_ATTR);
        return;
    }

    QString groups = el.attribute(GROUPS_ATTR);
    if (groups.isEmpty()) {
        failMissingValue(GROUPS_ATTR);
        return;
    }
    groupsToSearch = toSet(groups.split(QRegExp("\\,")));  // may be QRegExp("\\,")

    QString expected = el.attribute(EXPECTED_RESULTS_ATTR);
    if (!expected.isEmpty()) {
        QStringList expectedList = expected.split(QRegExp("\\,"));  // may be QRegExp("\\,")
        foreach (QString region, expectedList) {
            QStringList bounds = region.split(QRegExp("\\.."));
            if (bounds.size() != 2) {
                stateInfo.setError(QString("wrong value for %1").arg(EXPECTED_RESULTS_ATTR));
                return;
            }
            bool startOk, finishOk;
            int start = bounds.first().toInt(&startOk), finish = bounds.last().toInt(&finishOk);
            if (!startOk || !finishOk) {
                stateInfo.setError(QString("wrong value for %1").arg(EXPECTED_RESULTS_ATTR));
                return;
            }
            start--;
            expectedResults.append(U2Region(start, finish - start));
        }
    }

    QString strRegionSize = el.attribute(REGION_SIZE_ATTR);
    if (strRegionSize.isEmpty()) {
        failMissingValue(REGION_SIZE_ATTR);
        return;
    }
    bool isOk = false;
    regionSize = strRegionSize.toInt(&isOk);
    if (!isOk) {
        stateInfo.setError(QString("Unable to convert. Value wrong %1").arg(REGION_SIZE_ATTR));
        return;
    }

    QString strFitToRegion = el.attribute(FIT_TO_REGION_ATTR);
    if (strRegionSize.isEmpty()) {
        failMissingValue(FIT_TO_REGION_ATTR);
        return;
    }
    if (strFitToRegion == "true") {
        st = CollocationsAlgorithm::NormalSearch;
    } else if (strFitToRegion == "false") {
        st = CollocationsAlgorithm::PartialSearch;
    } else {
        stateInfo.setError(QString("Unable to convert. Value wrong %1").arg(FIT_TO_REGION_ATTR));
        return;
    }
}

void GTest_AnnotatorSearch::prepare() {
    searchTask = nullptr;
    Document* doc = getContext<Document>(this, docName);
    if (doc == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(docName));
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (list.size() == 0) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE));
        return;
    }
    GObject* obj = list.first();
    if (obj == nullptr) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::SEQUENCE));
        return;
    }
    assert(obj != nullptr);
    auto mySequence = qobject_cast<U2SequenceObject*>(obj);
    if (mySequence == nullptr) {
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }
    AnnotationTableObject* ao = getContext<AnnotationTableObject>(this, seqName);
    if (ao == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(seqName));
        return;
    }
    QList<AnnotationTableObject*> aoList;
    aoList.append(ao);
    CollocationsAlgorithmSettings cfg;
    cfg.distance = regionSize;
    cfg.searchRegion = U2Region(0, mySequence->getSequenceLength());
    cfg.st = st;
    searchTask = new CollocationSearchTask(aoList, groupsToSearch, cfg);
    addSubTask(searchTask);
}

Task::ReportResult GTest_AnnotatorSearch::report() {
    if (searchTask != nullptr) {
        if (!searchTask->hasError()) {
            QVector<U2Region> actualResults = searchTask->popResults();
            int actualSize = actualResults.size(), expectedSize = expectedResults.size();
            if (actualSize != expectedSize) {
                stateInfo.setError(QString("Expected and Actual lists of regions are different: %1 %2").arg(expectedSize).arg(actualSize));
                return ReportResult_Finished;
            }
            std::sort(actualResults.begin(), actualResults.end());
            std::sort(expectedResults.begin(), expectedResults.end());
            if (actualResults != expectedResults) {
                stateInfo.setError(QString("One of the expected regions not found in results").arg(expectedSize).arg(actualSize));
            }
        }
    }
    return ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
// GTest_PlasmidAutoAnnotation

void GTest_CustomAutoAnnotation::init(XMLTestFormat*, const QDomElement& el) {
    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    seqName = el.attribute(SEQ_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQ_ATTR);
        return;
    }

    resultDocContextName = el.attribute(RES_ATTR);
    if (resultDocContextName.isEmpty()) {
        failMissingValue(RES_ATTR);
        return;
    }

    isCircular = el.attribute(CIRCULAR_ATTR) == "true";
    QString expected = el.attribute(EXPECTED_RESULTS_ATTR);
    if (!expected.isEmpty()) {
        bool ok = false;
        int result = expected.toInt(&ok);
        if (ok) {
            expectedUniqueFeaturesFound = result;
        } else {
            expectedAnnotationGroupNames = expected.split(",");
            if (expectedAnnotationGroupNames.isEmpty()) {
                wrongValue(EXPECTED_RESULTS_ATTR);
                return;
            }
        }
    }
}

void GTest_CustomAutoAnnotation::prepare() {
    searchTask = nullptr;
    Document* doc = getContext<Document>(this, docName);
    if (doc == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(docName));
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (list.size() == 0) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE));
        return;
    }

    GObject* obj = list.first();
    if (obj == nullptr) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::SEQUENCE));
        return;
    }
    assert(obj != nullptr);
    auto dnaObj = qobject_cast<U2SequenceObject*>(obj);
    if (dnaObj == nullptr) {
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }

    if (isCircular) {
        dnaObj->setCircular(true);
    }

    QString customAnnotationDir = QDir::searchPaths(PATH_PREFIX_DATA).first() + "/custom_annotations";
    QString plasmidFeaturesPath = customAnnotationDir + "/plasmid_features.txt";
    SharedFeatureStore store(new FeatureStore("plasmids", plasmidFeaturesPath));
    store->load();
    if (!store->isLoaded()) {
        stateInfo.setError(QString("Failed to load plasmid feature database %1").arg(plasmidFeaturesPath));
        return;
    }

    ao = new AnnotationTableObject(resultDocContextName, doc->getDbiRef());
    addContext(resultDocContextName, ao);

    searchTask = new CustomPatternAnnotationTask(ao, dnaObj->getEntityRef(), store);
    addSubTask(searchTask);
}

Task::ReportResult GTest_CustomAutoAnnotation::report() {
    if (expectedUniqueFeaturesFound != -1) {
        auto annotations = ao->getAnnotations();
        QSet<QString> uniqueFeatures;
        for (auto ann : qAsConst(annotations)) {
            uniqueFeatures << ann->getName();
        }
        int uniqueFeaturesSize = uniqueFeatures.size();
        if (expectedUniqueFeaturesFound != uniqueFeaturesSize) {
            setError(QString("Expected features number: %1, actual: %2").arg(expectedUniqueFeaturesFound).arg(uniqueFeaturesSize));
            return ReportResult_Finished;
        }

    } else if (!expectedAnnotationGroupNames.isEmpty()) {
        auto root = ao->getRootGroup();
        CHECK_EXT(root != nullptr, setError("Root group is null"), ReportResult_Finished);

        auto mainSubgroups = root->getSubgroups();
        CHECK_EXT(mainSubgroups.size() == 1, setError("Main subgroup is not found"), ReportResult_Finished);

        auto subgroups = mainSubgroups.first()->getSubgroups();
        QStringList subgroupsNames;
        for (auto subgroup : qAsConst(subgroups)) {
            subgroupsNames << subgroup->getName();
        }
        std::sort(expectedAnnotationGroupNames.begin(), expectedAnnotationGroupNames.end());
        std::sort(subgroupsNames.begin(), subgroupsNames.end());
        if (expectedAnnotationGroupNames != subgroupsNames) {
            setError(QString("Unexpected annotation group names, expected: \"%1\", current: \"%2\"")
                .arg(expectedAnnotationGroupNames.join(",")).arg(subgroupsNames.join(",")));
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
// GTest_GeneByGeneApproach

#define IDENTITY_ATTR "identity"
#define ANN_NAME_ATTR "ann_name"
#define EXPECTED_RESULT "exp_result"

void GTest_GeneByGeneApproach::init(XMLTestFormat*, const QDomElement& el) {
    docName = el.attribute(DOC_ATTR);
    if (docName.isEmpty()) {
        failMissingValue(DOC_ATTR);
        return;
    }

    seqName = el.attribute(SEQ_ATTR);
    if (seqName.isEmpty()) {
        failMissingValue(SEQ_ATTR);
        return;
    }

    annName = el.attribute(ANN_NAME_ATTR);
    if (annName.isEmpty()) {
        failMissingValue(ANN_NAME_ATTR);
        return;
    }

    QString expectedStr = el.attribute(EXPECTED_RESULT);
    if (expectedStr == "true") {
        expected = true;
    } else if (expectedStr == "false") {
        expected = false;
    } else {
        stateInfo.setError(QString("Unable to convert. Value wrong %1").arg(EXPECTED_RESULT));
        return;
    }

    QString identityStr = el.attribute(IDENTITY_ATTR);
    bool ok = false;
    identity = identityStr.toFloat(&ok);
    if (identityStr.isEmpty() || !ok) {
        failMissingValue(IDENTITY_ATTR);
        return;
    }
}

void GTest_GeneByGeneApproach::prepare() {
    Document* doc = getContext<Document>(this, docName);
    if (doc == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(docName));
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (list.size() == 0) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE));
        return;
    }
    GObject* obj = nullptr;
    foreach (GObject* o, list) {
        if (o->getGObjectName() == seqName) {
            obj = o;
            break;
        }
    }
    if (obj == nullptr) {
        stateInfo.setError(QString("object with name \"%1\" not found").arg(seqName));
        return;
    }
    assert(obj != nullptr);
    auto mySequence = qobject_cast<U2SequenceObject*>(obj);
    if (mySequence == nullptr) {
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }
    AnnotationTableObject* ao = getContext<AnnotationTableObject>(this, seqName);
    if (ao == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(seqName));
        return;
    }
    const QList<Annotation*> annotations = ao->getAnnotations();

    QList<SharedAnnotationData> annData;

    foreach (Annotation* a, annotations) {
        annData << a->getData();
    }

    result = GeneByGeneComparator::compareGeneAnnotation(mySequence->getWholeSequence(stateInfo), annData, annName, identity);
}

Task::ReportResult GTest_GeneByGeneApproach::report() {
    if (expected != result.identical) {
        stateInfo.setError(QString("Expected gene is not found"));
    }

    return ReportResult_Finished;
}

}  // namespace U2
