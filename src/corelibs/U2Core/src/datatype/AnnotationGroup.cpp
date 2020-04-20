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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AnnotationGroup.h"

namespace U2 {

const QString AnnotationGroup::ROOT_GROUP_NAME("/");
const QChar AnnotationGroup::GROUP_PATH_SEPARATOR('/');

AnnotationGroup::AnnotationGroup()
    : U2Entity(), parentObject(NULL), parentGroup(NULL) {

}

AnnotationGroup::AnnotationGroup(const U2DataId &featureId, const QString &name, AnnotationGroup *parentGroup, AnnotationTableObject *parentObject)
    : U2Entity(featureId), parentObject(parentObject), name(name), parentGroup(parentGroup) {
    SAFE_POINT(NULL != parentObject && hasValidId(), "Invalid feature table detected", );
}

AnnotationGroup::~AnnotationGroup() {
    qDeleteAll(annotations);
    qDeleteAll(subgroups);
}

bool AnnotationGroup::isValidGroupName(const QString &name, bool pathMode) {
    if (name.isEmpty()) {
        return false;
    }
    //todo: optimize
    QBitArray validChars = TextUtils::ALPHA_NUMS;
    validChars['_'] = true;
    validChars['-'] = true;
    validChars[' '] = true;
    validChars['\''] = true;

    if (pathMode) {
        validChars[GROUP_PATH_SEPARATOR.toLatin1()] = true;
    }
    QByteArray groupName = name.toLocal8Bit();
    if (!TextUtils::fits(validChars, groupName.constData(), groupName.size())) {
        return false;
    }
    if (' ' == groupName[0] || ' ' == groupName[groupName.size() - 1]) {
        return false;
    }
    return true;
}

void AnnotationGroup::findAllAnnotationsInGroupSubTree(QList<Annotation *> &set) const {
    foreach(Annotation *a, annotations) {
        if (!set.contains(a)) {
            set.append(a);
        }
    }
    foreach(AnnotationGroup *subgroup, subgroups) {
        subgroup->findAllAnnotationsInGroupSubTree(set);
    }
}

QList<Annotation *> AnnotationGroup::getAnnotations(bool recurcively) const {
    QList<Annotation *> result = annotations;
    if (recurcively) {
        foreach(AnnotationGroup *subgroup, subgroups) {
            result.append(subgroup->getAnnotations(true));
        }
    }
    return result;
}

bool AnnotationGroup::hasAnnotations() const {
    if (!annotations.isEmpty()) {
        return true;
    }
    foreach(AnnotationGroup *subgroup, subgroups) {
        if (subgroup->hasAnnotations()) {
            return true;
        }
    }
    return false;
}

QList<Annotation *> AnnotationGroup::addAnnotations(const QList<SharedAnnotationData> &anns) {
    QList<Annotation *> result;
    CHECK(!anns.isEmpty(), result);

    U2OpStatusImpl os;
    DbiOperationsBlock opBlock(parentObject->getEntityRef().dbiRef, os);
    Q_UNUSED(opBlock);
    CHECK_OP(os, result);

    foreach(const SharedAnnotationData &d, anns) {
        const U2Feature feature = U2FeatureUtils::exportAnnotationDataToFeatures(d, parentObject->getRootFeatureId(), id,
            parentObject->getEntityRef().dbiRef, os);

        result.append(new Annotation(feature.id, d, this, parentObject));
        SAFE_POINT_OP(os, result);
    }

    foreach(Annotation *a, result) {
        annotationById[a->id] = a;
    }
    annotations.append(result);

    parentObject->setModified(true);
    parentObject->emit_onAnnotationsAdded(result);

    return result;
}

void AnnotationGroup::addShallowAnnotations(const QList<Annotation *> &anns, bool newAnnotations) {
#ifdef _DEBUG
    foreach(Annotation *a, anns) {
        SAFE_POINT(a->getGroup() == this, "Unexpected annotation group", );
    }
#endif

    foreach(Annotation *a, anns) {
        annotationById[a->id] = a;
    }
    annotations.append(anns);

    if (newAnnotations) {
        parentObject->setModified(true);
        parentObject->emit_onAnnotationsAdded(anns);
    }
}

void AnnotationGroup::removeAnnotations(const QList<Annotation *> &anns) {
    parentObject->emit_onAnnotationsRemoved(anns);
    U2OpStatusImpl os;

    QList<U2DataId> annotationsIds;
    foreach(Annotation *a, anns) {
        SAFE_POINT(NULL != a && a->getGroup() == this, "Unexpected annotation group", );
        annotationsIds.append(a->id);
    }
    U2FeatureUtils::removeFeatures(annotationsIds, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    foreach(Annotation *a, anns) {
        annotationById.remove(a->id);
        annotations.removeOne(a);
        delete a;
    }
    parentObject->setModified(true);
}

QList<AnnotationGroup *> AnnotationGroup::getSubgroups() const {
    return subgroups;
}

void AnnotationGroup::removeSubgroup(AnnotationGroup *g) {
    SAFE_POINT(g != NULL, L10N::nullPointerError("annotation group"), );
    SAFE_POINT(g->getParentGroup() == this, "Attempting to remove group belonging to different group", );

    parentObject->emit_onGroupRemoved(this, g);

    g->clear();

    U2OpStatusImpl os;
    U2FeatureUtils::removeFeature(g->id, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    subgroups.removeOne(g);
    delete g;

    parentObject->setModified(true);
}

QString AnnotationGroup::getName() const {
    return name;
}

void AnnotationGroup::setName(const QString &newName) {
    SAFE_POINT(!newName.isEmpty(), "Attempting to set an empty name for a group!", );
    CHECK(name != newName, );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureName(id, newName, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    name = newName;

    parentObject->setModified(true);
    parentObject->emit_onGroupRenamed(this);
}

QString AnnotationGroup::getGroupPath() const {
    if (NULL == parentGroup) {
        return QString();
    } else if (NULL == parentGroup->getParentGroup()) {
        return name;
    } else {
        return parentGroup->getGroupPath() + GROUP_PATH_SEPARATOR + name;
    }
}

AnnotationTableObject * AnnotationGroup::getGObject() const {
    return parentObject;
}

AnnotationGroup * AnnotationGroup::getParentGroup() {
    return parentGroup;
}

AnnotationGroup * AnnotationGroup::getSubgroup(const QString &path, bool create) {
    if (path.isEmpty()) {
        return this;
    }
    const int separatorFirstPosition = path.indexOf(GROUP_PATH_SEPARATOR);
    const QString subgroupName = (0 > separatorFirstPosition) ? path
        : ((0 == separatorFirstPosition) ? path.mid(1) : path.left(separatorFirstPosition));

    AnnotationGroup *subgroup = NULL;
    foreach(AnnotationGroup *g, subgroups) {
        if (g->getName() == subgroupName) {
            subgroup = g;
            break;
        }
    }

    if (NULL == subgroup && create) {
        U2OpStatusImpl os;
        const U2DbiRef dbiRef = parentObject->getEntityRef().dbiRef;
        const U2Feature subgroupFeature = U2FeatureUtils::exportAnnotationGroupToFeature(subgroupName,
            parentObject->getRootFeatureId(), id, dbiRef, os);
        SAFE_POINT_OP(os, subgroup);

        subgroup = new AnnotationGroup(subgroupFeature.id, subgroupName, this, parentObject);
        subgroups.append(subgroup);

        parentObject->emit_onGroupCreated(subgroup);
    }
    if (separatorFirstPosition <= 0 || NULL == subgroup) {
        return subgroup;
    }
    return subgroup->getSubgroup(path.mid(separatorFirstPosition + 1), create);
}

AnnotationGroup * AnnotationGroup::addSubgroup(const U2Feature &feature) {
    SAFE_POINT(feature.hasValidId() && feature.featureClass == U2Feature::Group, "Unexpected feature provided", NULL);

    if (feature.parentFeatureId == id) {
        AnnotationGroup *result = new AnnotationGroup(feature.id, feature.name, this, parentObject);
        subgroups.append(result);
        parentObject->emit_onGroupCreated(result);
        return result;
    } else {
        AnnotationGroup *parentGroup = findSubgroupById(feature.parentFeatureId);
        SAFE_POINT(NULL != parentGroup, L10N::nullPointerError("annotation group"), NULL);
        return parentGroup->addSubgroup(feature);
    }
}

Annotation * AnnotationGroup::findAnnotationById(const U2DataId &featureId) const {
    SAFE_POINT(!featureId.isEmpty(), "Unexpected feature provided", NULL);

    if (annotationById.contains(featureId)) {
        return annotationById[featureId];
    } else {
        foreach(AnnotationGroup *g, subgroups) {
            Annotation *result = g->findAnnotationById(featureId);
            if (NULL != result) {
                return result;
            }
        }
    }
    return NULL;
}

AnnotationGroup * AnnotationGroup::findSubgroupById(const U2DataId &featureId) const {
    SAFE_POINT(!featureId.isEmpty(), "Unexpected feature provided", NULL);

    foreach(AnnotationGroup *g, subgroups) {
        if (g->id == featureId) {
            return g;
        }

        AnnotationGroup *result = g->findSubgroupById(featureId);
        if (NULL != result) {
            return result;
        }
    }
    return NULL;
}

void AnnotationGroup::getSubgroupPaths(QStringList &res) const {
    if (!isRootGroup()) {
        res.append(getGroupPath());
    }
    foreach(const AnnotationGroup *g, subgroups) {
        g->getSubgroupPaths(res);
    }
}

void AnnotationGroup::clear() {
    if (!annotations.isEmpty()) {
        removeAnnotations(annotations);
    }
    while (!subgroups.isEmpty()) {
        removeSubgroup(subgroups.first());
    }
}

int AnnotationGroup::getGroupDepth() const {
    return 1 + (isRootGroup() ? 0 : parentGroup->getGroupDepth());
}

bool AnnotationGroup::isParentOf(AnnotationGroup *g) const {
    if (g->getGObject() != parentObject || g == this) {
        return false;
    }
    for (AnnotationGroup *pg = g->getParentGroup(); pg != NULL; pg = pg->getParentGroup()) {
        if (pg == this) {
            return true;
        }
    }
    return false;
}

bool AnnotationGroup::isRootGroup() const {
    return NULL == parentGroup;
}

bool AnnotationGroup::isTopLevelGroup() const {
    return parentGroup != NULL && parentGroup->isRootGroup();
}

bool AnnotationGroup::operator ==(const AnnotationGroup &other) const {
    return id == other.id && parentObject == other.getGObject();
}

namespace {

bool registerAnnotationGroupMeta() {
    qRegisterMetaType<AnnotationGroup>("AnnotationGroup");
    return true;
}

}

const bool AnnotationGroup::annotationGroupMetaRegistered = registerAnnotationGroupMeta();

} // namespace U2
