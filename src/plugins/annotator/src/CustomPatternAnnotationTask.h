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

#pragma once

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
    static const QString PROMOTER;
    static const QString TERMINATOR;
    static const QString REGULATORY_SEQUENCE;
    static const QString REPLICATION_ORIGIN;
    static const QString SELECTABLE_MARKER;
    static const QString REPORTER_GENE;
    static const QString TWO_HYBRID_GENE;
    static const QString LOCALIZATION_SEQUENCE;
    static const QString AFFINITY_TAG;
    static const QString GENE;
    static const QString PRIMER;
    static const QString MISCELLANEOUS;
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
    FeatureStore(const QString& storeName, const QString& filePath)
        : name(storeName), path(filePath), minFeatureSize(0) {
    }
    void load();

    bool isLoaded() const;

    int getMinFeatureSize() const;

    const QString& getName() const;

    const QList<FeaturePattern>& getFeatures() const;
};

typedef QSharedPointer<FeatureStore> SharedFeatureStore;

class CustomPatternAnnotationTask : public Task {
    Q_OBJECT
public:
    CustomPatternAnnotationTask(AnnotationTableObject* aobj, const U2EntityRef& entityRef, const SharedFeatureStore& store, const QStringList& filteredFeatures = QStringList());

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    struct PatternInfo {
        QString name;
        QString type;
        bool forwardStrand = true;

        PatternInfo() = default;
        PatternInfo(const QString& _nm, const QString& _type, bool isForward)
            : name(_nm), type(_type), forwardStrand(isForward) {
        }
    };

private:
    QSharedPointer<SArrayIndex> index;
    QMap<QString, QList<SharedAnnotationData>> groupAnnotationsMap;
    QMap<Task*, PatternInfo> taskFeatureNames;
    U2SequenceObject dnaObj;
    QPointer<AnnotationTableObject> annotationTableObject;
    QByteArray sequence;
    SharedFeatureStore featureStore;
    QStringList filteredFeatures;
};

class CustomPatternAutoAnnotationUpdater : public AutoAnnotationsUpdater {
    Q_OBJECT
    SharedFeatureStore featureStore;

public:
    CustomPatternAutoAnnotationUpdater(const SharedFeatureStore& store);
    Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* aa) override;
    bool checkConstraints(const AutoAnnotationConstraints& constraints) override;
};

}  // namespace U2
