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

#include "CustomPatternAnnotationTask.h"

#include <QFile>

#include <U2Algorithm/SArrayBasedFindTask.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Settings.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// CustomPatternAnnotationTask

const QString PlasmidFeatureTypes::FEATURE("Feature");
const QString PlasmidFeatureTypes::GENE("Gene");
const QString PlasmidFeatureTypes::ORIGIN("Origin");
const QString PlasmidFeatureTypes::PRIMER("Primer");
const QString PlasmidFeatureTypes::PROMOTER("Promoter");
const QString PlasmidFeatureTypes::REGULATORY("Regulatory");
const QString PlasmidFeatureTypes::TERMINATOR("Terminator");

CustomPatternAnnotationTask::CustomPatternAnnotationTask(AnnotationTableObject* aObj, const U2::U2EntityRef& entityRef, const SharedFeatureStore& store, const QStringList& filteredFeatureTypes)
    : Task(tr("Custom pattern annotation"), TaskFlags_NR_FOSCOE), dnaObj("ref", entityRef), annotationTableObject(aObj),
      featureStore(store), filteredFeatures(filteredFeatureTypes) {
    GCOUNTER(cvar, "CustomPatternAnnotationTask");
}

void CustomPatternAnnotationTask::prepare() {
    sequence = dnaObj.getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    if (dnaObj.isCircular()) {
        sequence += sequence;
    }

    const QList<FeaturePattern>& patterns = featureStore->getFeatures();

    if (patterns.length() == 0) {
        return;
    }
    char unknownChar = 'N';

    if (sequence.length() < featureStore->getMinFeatureSize()) {
        return;
    }

    index = QSharedPointer<SArrayIndex>(new SArrayIndex(sequence.constData(), quint32(sequence.length()), quint32(featureStore->getMinFeatureSize()), stateInfo, unknownChar));

    if (hasError()) {
        return;
    }

    DNATranslation* complTT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dnaObj.getAlphabet());
    assert(complTT);

    foreach (const FeaturePattern& pattern, patterns) {
        if (filteredFeatures.contains(pattern.type)) {
            continue;
        }

        if (pattern.sequence.length() > sequence.length()) {
            continue;
        }

        SArrayBasedSearchSettings settings;
        settings.unknownChar = unknownChar;
        settings.query = pattern.sequence;

        SArrayBasedFindTask* task = new SArrayBasedFindTask(index.data(), settings);
        taskFeatureNames.insert(task, PatternInfo(pattern.name, true));
        addSubTask(task);

        complTT->translate(settings.query.data(), settings.query.size());
        TextUtils::reverse(settings.query.data(), settings.query.size());

        SArrayBasedFindTask* revComplTask = new SArrayBasedFindTask(index.data(), settings);
        taskFeatureNames.insert(revComplTask, PatternInfo(pattern.name, false));
        addSubTask(revComplTask);
    }
}

QList<Task*> CustomPatternAnnotationTask::onSubTaskFinished(Task* subTask) {
    CHECK_EXT(!annotationTableObject.isNull(), setError(tr("Object with annotations was removed")), {});

    QList<Task*> subTasks;

    if (!taskFeatureNames.contains(subTask)) {
        return subTasks;
    }

    SArrayBasedFindTask* task = static_cast<SArrayBasedFindTask*>(subTask);
    const QList<int>& results = task->getResults();
    PatternInfo info = taskFeatureNames.take(task);

    qint64 seqLen = dnaObj.getSequenceLength();

    foreach (int pos, results) {
        if (pos > dnaObj.getSequenceLength()) {
            continue;
        }

        int endPos = pos + task->getQuery().length() - 1;

        SharedAnnotationData data(new AnnotationData);
        data->name = info.name;
        U2Strand strand = info.forwardStrand ? U2Strand::Direct : U2Strand::Complementary;
        data->setStrand(strand);

        if (dnaObj.isCircular() && endPos > seqLen) {
            qint64 outerLen = endPos - seqLen;
            qint64 innerLen = task->getQuery().length() - outerLen;
            U2Region region1(pos - 1, innerLen);
            U2Region region2(0, outerLen);
            data->location->regions << region1 << region2;
            data->setLocationOperator(U2LocationOperator_Join);
        } else {
            U2Region region(pos - 1, task->getQuery().length());
            data->location->regions << region;
        }

        annotations.append(data);
    }

    if (taskFeatureNames.isEmpty() && annotations.size() > 0) {
        const QString& groupName = featureStore->getName();
        subTasks.append(new CreateAnnotationsTask(annotationTableObject, {{groupName, annotations}}));
    }

    return subTasks;
}

//////////////////////////////////////////////////////////////////////////
// FeatureStore
bool FeatureStore::isLoaded() const {
    return !features.isEmpty();
}

int FeatureStore::getMinFeatureSize() const {
    return minFeatureSize;
}

const QString& FeatureStore::getName() const {
    return name;
}

const QList<FeaturePattern>& FeatureStore::getFeatures() const {
    return features;
}

void FeatureStore::load() {
    QFile inputFile(path);

    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    int minPatternSize = INT_MAX;

    while (!inputFile.atEnd()) {
        QByteArray line = inputFile.readLine().trimmed();
        QList<QByteArray> lineItems = line.split('\t');

        if (line.startsWith("#")) {
            continue;
        }

        assert(lineItems.size() == 3);
        if (lineItems.size() != 3) {
            break;
        }

        FeaturePattern pattern;

        pattern.name = lineItems[0].trimmed();
        pattern.type = lineItems[1].trimmed();
        pattern.sequence = lineItems[2].toUpper();
        if (pattern.sequence.length() < minPatternSize) {
            minPatternSize = pattern.sequence.length();
        }

        features.append(pattern);
    }

    if (minPatternSize != INT_MAX) {
        minFeatureSize = minPatternSize;
    }
}

//////////////////////////////////////////////////////////////////////////
// CustomPatternAutoAnnotationUpdater

CustomPatternAutoAnnotationUpdater::CustomPatternAutoAnnotationUpdater(const SharedFeatureStore& store)
    : AutoAnnotationsUpdater(tr("Plasmid features"), store->getName(), true), featureStore(store) {
    assert(featureStore);
}

Task* CustomPatternAutoAnnotationUpdater::createAutoAnnotationsUpdateTask(const AutoAnnotationObject* aa) {
    QStringList filteredFeatureTypes = AppContext::getSettings()->getValue(FILTERED_FEATURE_LIST, QStringList()).toStringList();

    AnnotationTableObject* aObj = aa->getAnnotationObject();
    const U2EntityRef& dnaRef = aa->getSequenceObject()->getEntityRef();
    Task* task = new CustomPatternAnnotationTask(aObj, dnaRef, featureStore, filteredFeatureTypes);

    return task;
}

bool CustomPatternAutoAnnotationUpdater::checkConstraints(const AutoAnnotationConstraints& constraints) {
    if (constraints.alphabet == nullptr) {
        return false;
    }

    return constraints.alphabet->isNucleic();
}

}  // namespace U2
