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

#ifndef _U2_CUSTOM_PATTERN_ANNOTATION_TASK_H_
#define _U2_CUSTOM_PATTERN_ANNOTATION_TASK_H_

#include <QSharedPointer>

#include <U2Algorithm/SArrayIndex.h>

#include <U2Core/AnnotationData.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Task.h>

namespace U2 {

class AnnotationTableObject;

#define PLASMID_FEATURES_GROUP_NAME "plasmid_features"
#define FILTERED_FEATURE_LIST "filter_feature_list"

class PlasmidFeatureTypes {
public:
    static const QString GENE;
    static const QString FEATURE;
    static const QString ORIGIN;
    static const QString PRIMER;
    static const QString PROMOTER;
    static const QString REGULATORY;
    static const QString TERMINATOR;
};

struct FeaturePattern {
    QString name;
    QString type;
    QByteArray sequence;
};

class FeatureStore {
    QList<FeaturePattern> features;
    QString name, path;
    int minFeatureSize;
public:
    FeatureStore(const QString& storeName, const QString& filePath) : name(storeName), path(filePath), minFeatureSize(0) {}
    void load();
    bool isLoaded() const { return !features.isEmpty(); }
    int getMinFeatureSize() const { return minFeatureSize; }
    const QString& getName() const { return name; }
    const QList<FeaturePattern>& getFeatures() const { return features; }
};

typedef QSharedPointer<FeatureStore> SharedFeatureStore;

class CustomPatternAnnotationTask :  public Task
{
    Q_OBJECT
public:
    CustomPatternAnnotationTask(AnnotationTableObject* aobj, const U2EntityRef& entityRef, const SharedFeatureStore& store,
        const QStringList& filteredFeatures = QStringList());

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    struct PatternInfo {
        QString name;
        bool forwardStrand;
        PatternInfo() : forwardStrand(true) {}
        PatternInfo(const QString& nm, bool isForward) : name(nm), forwardStrand(isForward) {}
    };

private:
    QSharedPointer<SArrayIndex> index;
    QMap<Task*, PatternInfo> taskFeatureNames;
    QList<SharedAnnotationData> annotations;
    U2SequenceObject dnaObj;
    AnnotationTableObject* aTableObj;
    QByteArray sequence;
    SharedFeatureStore featureStore;
    QStringList filteredFeatures;
};

class CustomPatternAutoAnnotationUpdater : public AutoAnnotationsUpdater {
    Q_OBJECT
    SharedFeatureStore featureStore;
public:
    CustomPatternAutoAnnotationUpdater(const SharedFeatureStore& store);
    Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* aa);
    bool checkConstraints(const AutoAnnotationConstraints& constraints);
};


} // namespace

#endif // _U2_CUSTOM_PATTERN_ANNOTATION_TASK_H_
