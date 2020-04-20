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

#include <QCoreApplication>

#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationTableObjectConstraints.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/L10n.h>
#include <U2Core/Timer.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AnnotationTableObject.h"
#include "GObjectTypes.h"

namespace U2 {

AnnotationTableObject::AnnotationTableObject(const QString &objectName, const U2DbiRef &dbiRef, const QVariantMap &hintsMap)
    : GObject(GObjectTypes::ANNOTATION_TABLE, objectName, hintsMap)
{
    U2OpStatusImpl os;
    const QString folder = hintsMap.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    U2AnnotationTable table = U2FeatureUtils::createAnnotationTable(objectName, dbiRef, folder, os);
    SAFE_POINT_OP(os, );

    entityRef = U2EntityRef(dbiRef, table.id);
    rootGroup = new AnnotationGroup(table.rootFeature, AnnotationGroup::ROOT_GROUP_NAME, NULL, this);
    dataLoaded = true;
}

AnnotationTableObject::AnnotationTableObject(const QString &objectName, const U2EntityRef &tableRef, const QVariantMap &hintsMap)
    : GObject(GObjectTypes::ANNOTATION_TABLE, objectName, hintsMap), rootGroup(NULL)
{
    entityRef = tableRef;
}

AnnotationTableObject::~AnnotationTableObject() {
    delete rootGroup;
}

QList<Annotation *> AnnotationTableObject::getAnnotations() const {
    ensureDataLoaded();
    return rootGroup->getAnnotations(true);
}

bool AnnotationTableObject::hasAnnotations() const {
    ensureDataLoaded();
    return rootGroup->hasAnnotations();
}

AnnotationGroup * AnnotationTableObject::getRootGroup() {
    ensureDataLoaded();
    return rootGroup;
}

typedef QPair<AnnotationGroup *, QList<SharedAnnotationData> > AnnotationGroupData;

QList<Annotation *> AnnotationTableObject::addAnnotations(const QList<SharedAnnotationData> &annotations, const QString &groupName) {
    QList<Annotation *> result;
    CHECK(!annotations.isEmpty(), result);

    ensureDataLoaded();

    if (groupName.isEmpty()) {
        QString previousGroupName;
        QMap<QString, AnnotationGroupData> group2Annotations;
        foreach (const SharedAnnotationData &a, annotations) {
            const QString groupName = a->name;
            if (!group2Annotations.contains(groupName)) {
                AnnotationGroup *group = rootGroup->getSubgroup(groupName, true);
                group2Annotations[groupName].first = group;
            }
            group2Annotations[groupName].second.append(a);
        }
        foreach (const AnnotationGroupData &groupData, group2Annotations) {
            result.append(groupData.first->addAnnotations(groupData.second));
        }
    } else {
        AnnotationGroup *group = rootGroup->getSubgroup(groupName, true);
        result.append(group->addAnnotations(annotations));
    }
    return result;
}

void AnnotationTableObject::removeAnnotations(const QList<Annotation *> &annotations) {
    CHECK(!annotations.isEmpty(), );

    QMap<AnnotationGroup *, QList<Annotation *> > group2Annotations;
    foreach (Annotation *ann, annotations) {
        SAFE_POINT(ann->getGObject() == this, "Unexpected annotation detected", );
        group2Annotations[ann->getGroup()].append(ann);
    }

    foreach (AnnotationGroup *group, group2Annotations.keys()) {
        group->removeAnnotations(group2Annotations[group]);
    }
}

GObject * AnnotationTableObject::clone(const U2DbiRef &ref, U2OpStatus &os, const QVariantMap &hints) const {
    ensureDataLoaded();

    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    DbiOperationsBlock opBlock(ref, os);
    Q_UNUSED(opBlock);
    CHECK_OP(os, NULL);

    AnnotationTableObject *cln = new AnnotationTableObject(getGObjectName(), ref, gHints.getMap());
    cln->setIndexInfo(getIndexInfo());

    QStringList subgroupPaths;
    rootGroup->getSubgroupPaths(subgroupPaths);
    AnnotationGroup *clonedRootGroup = cln->getRootGroup();
    foreach (const QString &groupPath, subgroupPaths) {
        AnnotationGroup *originalGroup = rootGroup->getSubgroup(groupPath, false);
        SAFE_POINT(originalGroup != NULL, L10N::nullPointerError("annotation group"), NULL);

        AnnotationGroup *clonedGroup = clonedRootGroup->getSubgroup(groupPath, true);
        QList<SharedAnnotationData> groupData;
        foreach (const Annotation *a, originalGroup->getAnnotations()) {
            groupData.append(a->getData());
        }
        clonedGroup->addAnnotations(groupData);
    }

    return cln;
}

QList<Annotation *> AnnotationTableObject::getAnnotationsByName(const QString &name) const {
    QList<Annotation *> result;

    ensureDataLoaded();

    foreach (Annotation *a, getAnnotations()) {
        if (a->getName() == name) {
            result.append(a);
        }
    }

    return result;
}

namespace {

bool annotationIntersectsRange(const Annotation *a, const U2Region &range, bool contains) {
    SAFE_POINT(NULL != a, L10N::nullPointerError("annotation"), false);
    if (!contains) {
        foreach (const U2Region &r, a->getRegions()) {
            if (r.intersects(range)) {
                return true;
            }
        }
        return false;
    } else {
        foreach (const U2Region &r, a->getRegions()) {
            if (!range.contains(r)) {
                return false;
            }
        }
        return true;
    }
}

}

QList<Annotation *> AnnotationTableObject::getAnnotationsByRegion(const U2Region &region, bool contains) const {
    QList<Annotation *> result;

    ensureDataLoaded();

    foreach (Annotation *a, getAnnotations()) {
        if (annotationIntersectsRange(a, region, contains)) {
            result.append(a);
        }
    }

    return result;
}

QList<Annotation *> AnnotationTableObject::getAnnotationsByType(const U2FeatureType featureType) const {
    QList<Annotation *> result;

    ensureDataLoaded();

    foreach(Annotation *a, getAnnotations()) {
        if (a->getType() == featureType) {
            result.append(a);
        }
    }

    return result;
}

bool AnnotationTableObject::checkConstraints(const GObjectConstraints *c) const {
    const AnnotationTableObjectConstraints *ac = qobject_cast<const AnnotationTableObjectConstraints *>(c);
    SAFE_POINT(NULL != ac, "Invalid feature constraints", false);

    ensureDataLoaded();

    const int fitSize = ac->sequenceSizeToFit;
    SAFE_POINT(0 < fitSize, "Invalid sequence length provided!", false);
    foreach (const Annotation *a, getAnnotations()) {
        foreach (const U2Region &region, a->getRegions()) {
            SAFE_POINT(0 <= region.startPos, "Invalid annotation region", false);
            if (region.endPos() > fitSize) {
                return false;
            }
        }
    }
    return true;
}

void AnnotationTableObject::setGObjectName(const QString &newName) {
    CHECK(name != newName, );

    ensureDataLoaded();
    GObject::setGObjectName(newName);
}

U2DataId AnnotationTableObject::getRootFeatureId() const {
    ensureDataLoaded();

    return rootGroup->id;
}

void AnnotationTableObject::emit_onAnnotationsAdded(const QList<Annotation *> &l) {
    emit si_onAnnotationsAdded(l);
}

void AnnotationTableObject::emit_onAnnotationsModified(const AnnotationModification &annotationModification) {
    emit_onAnnotationsModified(QList<AnnotationModification>() << annotationModification);
}

void AnnotationTableObject::emit_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications) {
    emit si_onAnnotationsModified(annotationModifications);
}

void AnnotationTableObject::emit_onAnnotationsRemoved(const QList<Annotation *> &a) {
    emit si_onAnnotationsRemoved(a);
}

void AnnotationTableObject::emit_onGroupCreated(AnnotationGroup *g) {
    emit si_onGroupCreated(g);
}

void AnnotationTableObject::emit_onGroupRemoved(AnnotationGroup *p, AnnotationGroup *g) {
    emit si_onGroupRemoved(p, g);
}

void AnnotationTableObject::emit_onGroupRenamed(AnnotationGroup *g) {
    emit si_onGroupRenamed(g);
}

void AnnotationTableObject::emit_onAnnotationsInGroupRemoved(const QList<Annotation *> &l, AnnotationGroup *gr) {
    emit si_onAnnotationsInGroupRemoved(l, gr);
}

void AnnotationTableObject::loadDataCore(U2OpStatus &os) {
    SAFE_POINT(NULL == rootGroup, "Annotation table is initialized unexpectedly", );

    U2AnnotationTable table = U2FeatureUtils::getAnnotationTable(entityRef, os);
    CHECK_OP(os, );

    rootGroup = U2FeatureUtils::loadAnnotationTable(table.rootFeature, entityRef.dbiRef, this, os);
}

} // namespace U2
