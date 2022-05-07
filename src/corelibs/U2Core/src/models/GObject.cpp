/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "GObject.h"

#include <QMutexLocker>

#include <U2Core/GObjectTypes.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2ObjectTypeUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DocumentModel.h"
#include "GHints.h"

namespace U2 {

GObject::GObject(QString _type, const QString& _name, const QVariantMap& hintsMap)
    : dataLoaded(false), type(_type), name(_name) {
    SAFE_POINT(!name.isEmpty(), "Got an empty object name, type: " + type, );
    setupHints(hintsMap);
}

GObject::~GObject() {
    delete hints;
    removeAllLocks();
}

QVariantMap GObject::getGHintsMap() const {
    return getGHints()->getMap();
}

Document* GObject::getDocument() const {
    StateLockableTreeItem* sl = getParentStateLockItem();
    Document* doc = qobject_cast<Document*>(sl);
    return doc;
}

void GObject::setGHints(GHints* s) {
    delete hints;
    hints = s;
}

void GObject::setGObjectName(const QString& newName) {
    CHECK(name != newName, );

    if (entityRef.dbiRef.isValid()) {
        U2OpStatus2Log os;
        DbiConnection con(entityRef.dbiRef, os);
        CHECK_OP(os, );
        CHECK(nullptr != con.dbi, );
        U2ObjectDbi* oDbi = con.dbi->getObjectDbi();
        CHECK(nullptr != oDbi, );

        oDbi->renameObject(entityRef.entityId, newName, os);
        CHECK_OP(os, );
    }

    setGObjectNameNotDbi(newName);
}

void GObject::setGObjectNameNotDbi(const QString& newName) {
    CHECK(name != newName, );

    QString oldName = name;
    name = newName;
    hints->set(GObjectHint_LastUsedObjectName, name);

    emit si_nameChanged(oldName);
}

QList<GObjectRelation> GObject::getObjectRelations() const {
    SAFE_POINT(hints != nullptr, "Object hints is NULL", QList<GObjectRelation>());
    return hints->get(GObjectHint_RelatedObjects).value<QList<GObjectRelation>>();
}

void GObject::setObjectRelations(const QList<GObjectRelation>& list) {
    QList<GObjectRelation> internalCopy = list;
    QList<GObjectRelation> oldRelations = getObjectRelations();
    setRelationsInDb(internalCopy);
    hints->set(GObjectHint_RelatedObjects, QVariant::fromValue<QList<GObjectRelation>>(internalCopy));
    emit si_relationChanged(oldRelations);
}

void GObject::setRelationsInDb(QList<GObjectRelation>& list) const {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );  // Database is not available for some reason. It is may be deleted.
    U2ObjectRelationsDbi* rDbi = con.dbi->getObjectRelationsDbi();
    SAFE_POINT(rDbi != nullptr, "Invalid object relations DBI detected!", );
    rDbi->removeReferencesForObject(entityRef.entityId, os);
    SAFE_POINT_OP(os, );
    U2ObjectDbi* oDbi = con.dbi->getObjectDbi();

    QList<U2ObjectRelation> dbRelations;
    for (int i = 0, n = list.size(); i < n; ++i) {
        GObjectRelation& relation = list[i];
        const U2DataType refType = U2ObjectTypeUtils::toDataType(relation.ref.objType);
        const bool relatedObjectDbReferenceValid = relation.ref.entityRef.dbiRef.isValid();

        if (U2Type::Unknown == refType || (relatedObjectDbReferenceValid && !(relation.ref.entityRef.dbiRef == entityRef.dbiRef))) {
            continue;
        }

        if (!relatedObjectDbReferenceValid) {
            QScopedPointer<U2DbiIterator<U2DataId>> idIterator(oDbi->getObjectsByVisualName(relation.ref.objName, refType, os));
            if (os.isCoR() || !idIterator->hasNext()) {
                continue;
            }
            relation.ref.entityRef = U2EntityRef(entityRef.dbiRef, idIterator->next());
        }

        U2ObjectRelation dbRelation;
        dbRelation.id = entityRef.entityId;
        dbRelation.referencedName = relation.ref.objName;
        dbRelation.referencedObject = relation.ref.entityRef.entityId;
        dbRelation.referencedType = refType;
        dbRelation.relationRole = relation.role;

        // after a project has loaded relations can duplicate, but we don't have to create copies in the DBI
        if (!dbRelations.contains(dbRelation)) {
            rDbi->createObjectRelation(dbRelation, os);
            SAFE_POINT_OP(os, );

            dbRelations << dbRelation;
        }
    }
}

void GObject::setupHints(QVariantMap hintsMap) {
    hintsMap.remove(DocumentFormat::DBI_REF_HINT);
    hintsMap.remove(DocumentFormat::DBI_FOLDER_HINT);
    hintsMap.remove(DocumentFormat::DEEP_COPY_OBJECT);

    hintsMap.insert(GObjectHint_LastUsedObjectName, name);

    hints = new GHintsDefaultImpl(hintsMap);
}

QList<GObjectRelation> GObject::findRelatedObjectsByRole(const GObjectRelationRole& role) const {
    QList<GObjectRelation> res;
    QList<GObjectRelation> relations = getObjectRelations();
    foreach (const GObjectRelation& ref, relations) {
        if (ref.role == role) {
            res.append(ref);
        }
    }
    return res;
}

QList<GObjectRelation> GObject::findRelatedObjectsByType(const GObjectType& objType) const {
    QList<GObjectRelation> res;
    foreach (const GObjectRelation& rel, getObjectRelations()) {
        if (rel.ref.objType == objType) {
            res.append(rel);
        }
    }
    return res;
}

void GObject::addObjectRelation(const GObjectRelation& rel) {
    SAFE_POINT(rel.isValid(), "Object relation is not valid!", );
    QList<GObjectRelation> list = getObjectRelations();
    CHECK(!list.contains(rel), );
    list.append(rel);
    setObjectRelations(list);
}

void GObject::removeObjectRelation(const GObjectRelation& ref) {
    QList<GObjectRelation> list = getObjectRelations();
    bool ok = list.removeOne(ref);
    if (ok) {
        setObjectRelations(list);
    }
}

void GObject::addObjectRelation(const GObject* obj, const GObjectRelationRole& role) {
    GObjectRelation rel(obj->getReference(), role);
    addObjectRelation(rel);
}

static bool relationsAreEqualExceptDbId(const GObjectRelation& f, const GObjectRelation& s) {
    return f.role == s.role && f.ref.objName == s.ref.objName && f.getDocURL() == s.getDocURL() && f.ref.objType == s.ref.objType &&
           (!f.ref.entityRef.isValid() || !s.ref.entityRef.isValid() || f.ref.entityRef.dbiRef == s.ref.entityRef.dbiRef);
}

bool GObject::hasObjectRelation(const GObjectRelation& r) const {
    Document* parentDoc = getDocument();
    if (parentDoc != nullptr) {
        foreach (const GObjectRelation& rel, getObjectRelations()) {
            if (relationsAreEqualExceptDbId(rel, r)) {
                return true;
            }
        }
        return false;
    } else {
        return getObjectRelations().contains(r);
    }
}

bool GObject::hasObjectRelation(const GObject* obj, const GObjectRelationRole& role) const {
    GObjectRelation rel(obj->getReference(), role);
    return hasObjectRelation(rel);
}

bool GObject::isUnloaded() const {
    return type == GObjectTypes::UNLOADED;
}

StateLock* GObject::getGObjectModLock(GObjectModLock lock) const {
    return modLocks.value(lock, nullptr);
}

void GObject::relatedObjectRelationChanged() {
    emit si_relatedObjectRelationChanged();
}

bool GObject::objectLessThan(GObject* first, GObject* second) {
    return QString::compare(first->getGObjectName(), second->getGObjectName(), Qt::CaseInsensitive) < 0;
}

void GObject::updateRefInRelations(const GObjectReference& oldRef, const GObjectReference& newRef) {
    QList<GObjectRelation> rels = getObjectRelations();
    bool changed = false;
    for (int i = 0; i < rels.size(); i++) {
        GObjectRelation& rel = rels[i];
        if (rel.ref == oldRef) {
            rel.ref = newRef;
            changed = true;
        }
    }
    if (changed) {
        setObjectRelations(rels);
    }
}
void GObject::removeRelations(const QString& removedDocUrl) {
    QList<GObjectRelation> rels = getObjectRelations();
    bool changed = false;
    for (int i = 0; i < rels.size(); i++) {
        GObjectRelation& rel = rels[i];
        if (rel.ref.docUrl == removedDocUrl) {
            rels.removeAll(rel);
            changed = true;
        }
    }
    if (changed) {
        setObjectRelations(rels);
    }
}

void GObject::updateDocInRelations(const QString& oldDocUrl, const QString& newDocUrl) {
    QList<GObjectRelation> rels = getObjectRelations();
    bool changed = false;
    for (int i = 0; i < rels.size(); i++) {
        GObjectRelation& rel = rels[i];
        if (rel.ref.docUrl == oldDocUrl) {
            rel.ref.docUrl = newDocUrl;
            changed = true;
        }
    }
    if (changed) {
        setObjectRelations(rels);
    }
}

void GObject::ensureDataLoaded() const {
    U2OpStatus2Log os;
    ensureDataLoaded(os);
}

void GObject::ensureDataLoaded(U2OpStatus& os) const {
    QMutexLocker locker(&dataGuard);
    CHECK(!dataLoaded, );
    const_cast<GObject*>(this)->loadDataCore(os);
    CHECK_OP(os, );
    dataLoaded = true;
}

void GObject::loadDataCore(U2OpStatus& /*os*/) {
    FAIL("Not implemented!", );
}

void GObject::setParentStateLockItem(StateLockableTreeItem* p) {
    StateLockableTreeItem::setParentStateLockItem(p);
}

void GObject::removeAllLocks() {
    foreach (StateLock* lock, modLocks.values()) {
        unlockState(lock);
    }
    qDeleteAll(modLocks.values());
    modLocks.clear();
}

int GObject::getObjectVersion() const {
    CHECK(entityRef.dbiRef.isValid(), -1);

    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, -1);
    CHECK(con.dbi != nullptr, -1);

    U2ObjectDbi* objectDbi = con.dbi->getObjectDbi();
    CHECK(objectDbi != nullptr, -1);

    int version = objectDbi->getObjectVersion(entityRef.entityId, os);
    CHECK_OP(os, -1);
    return version;
}

GObjectReference GObject::getReference() const {
    return GObjectReference(this);
}

//////////////////////////////////////////////////////////////////////////
// mime
const QString GObjectMimeData::MIME_TYPE("application/x-ugene-object-mime");

bool GObjectMimeData::hasFormat(const QString& mimeType) const {
    return mimeType == MIME_TYPE;
}

QStringList GObjectMimeData::formats() const {
    return QStringList(MIME_TYPE);
}

}  // namespace U2
