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

#include "GObjectUtils.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrl.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PFMatrixObject.h>
#include <U2Core/PWMatrixObject.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UnloadedObject.h>
#include <U2Core/VariantTrackObject.h>

namespace U2 {

QList<GObject*> GObjectUtils::select(const QList<GObject*>& objs, GObjectType t, UnloadedObjectFilter f) {
    QList<GObject*> res;
    foreach (GObject* o, objs) {
        bool isUnloaded = o->getGObjectType() == GObjectTypes::UNLOADED;
        if ((t.isEmpty() && (f == UOF_LoadedAndUnloaded || !isUnloaded)) || o->getGObjectType() == t) {
            res.append(o);
        } else if (f == UOF_LoadedAndUnloaded && isUnloaded) {
            auto uo = qobject_cast<UnloadedObject*>(o);
            if (uo->getLoadedObjectType() == t) {
                res.append(o);
            }
        }
    }
    return res;
}

GObject* GObjectUtils::selectOne(const QList<GObject*>& objects, GObjectType type, UnloadedObjectFilter f) {
    QList<GObject*> res = select(objects, type, f);
    return res.isEmpty() ? nullptr : res.first();
}

QList<GObject*> GObjectUtils::findAllObjects(const UnloadedObjectFilter& unloadedObjectFilter, const GObjectType& objectType, bool writableOnly) {
    QList<GObject*> res;
    SAFE_POINT(AppContext::getProject() != nullptr, "No active project found", res);

    const QList<Document*>& documents = AppContext::getProject()->getDocuments();
    for (const Document* doc : qAsConst(documents)) {
        if (writableOnly && doc->isStateLocked()) {
            continue;
        }
        if (objectType.isEmpty()) {
            if (doc->isLoaded() || unloadedObjectFilter == UOF_LoadedAndUnloaded) {
                res += doc->getObjects();
            }
        } else {
            res += doc->findGObjectByType(objectType, unloadedObjectFilter);
        }
    }
    return res;
}

QList<GObject*> GObjectUtils::selectRelations(GObject* obj, GObjectType type, GObjectRelationRole relationRole, const QList<GObject*>& fromObjects, UnloadedObjectFilter f) {
    QList<GObject*> res;
    QList<GObjectRelation> relations = obj->getObjectRelations();
    foreach (const GObjectRelation& r, relations) {
        if (r.role != relationRole || (!type.isEmpty() && r.ref.objType != type)) {
            continue;
        }
        GObject* referencedObject = selectObjectByReference(r.ref, fromObjects, f);
        if (referencedObject != nullptr) {
            res.append(referencedObject);
        }
    }
    return res;
}

QList<GObject*> GObjectUtils::selectRelationsFromParentDoc(const GObject* obj, const GObjectType& type, GObjectRelationRole relationRole) {
    QList<GObject*> result;

    Document* parentDoc = obj->getDocument();
    SAFE_POINT(parentDoc != nullptr, "Invalid parent document detected", result);

    U2OpStatus2Log os;
    DbiConnection con(parentDoc->getDbiRef(), os);
    U2ObjectRelationsDbi* relationsDbi = con.dbi->getObjectRelationsDbi();
    SAFE_POINT(relationsDbi != nullptr, "Invalid object relations DBI", result);

    const QList<U2ObjectRelation> relations = relationsDbi->getObjectRelations(obj->getEntityRef().entityId, os);
    CHECK_OP(os, result);
    foreach (const U2ObjectRelation& relation, relations) {
        if (type == relation.referencedType && relationRole == relation.relationRole) {
            GObject* referenceObj = parentDoc->getObjectById(relation.referencedObject);
            if (referenceObj != nullptr) {
                result.append(referenceObj);
            } else {
                os.setError(QString("Reference object with ID '%1' and name '%2' not found in the document")
                                .arg(QString(relation.referencedObject))
                                .arg(relation.referencedName));
            }
        }
    }
    return result;
}

namespace {

QList<GObject*> findRelatedObjectsForUnloadedObjects(const GObjectReference& obj, GObjectRelationRole role, const QSet<GObject*>& fromObjects) {
    QList<GObject*> res;
    GObjectRelation objRelation(obj, role);
    foreach (GObject* o, fromObjects) {
        if (!o->isUnloaded()) {
            coreLog.error("Invalid object loaded state detected");
            continue;
        }
        if (o->hasObjectRelation(objRelation)) {
            res.append(o);
        }
    }
    return res;
}

QList<GObject*> findRelatedObjectsForLoadedObjects(const GObjectReference& obj, GObjectRelationRole role, const QSet<GObject*>& fromObjects) {
    QList<GObject*> res;

    const GObjectRelation objRelation(obj, role);
    QHash<Document*, U2DbiRef> doc2DbiRef;
    foreach (GObject* object, fromObjects) {
        Document* doc = object->getDocument();
        SAFE_POINT(doc != nullptr, "Invalid parent document detected", res);
        if (object->hasObjectRelation(objRelation)) {  // this 'if' branch has to be distinctive from the enclosing one
            res.append(object);
        }
    }

    U2OpStatusImpl os;
    foreach (Document* doc, doc2DbiRef.keys()) {
        const U2DbiRef dbiRef = doc2DbiRef.value(doc);
        if (!dbiRef.isValid()) {
            coreLog.error("Invalid DBI reference detected");
            continue;
        }
        DbiConnection con(dbiRef, os);
        U2ObjectRelationsDbi* relationsDbi = con.dbi->getObjectRelationsDbi();
        SAFE_POINT(relationsDbi != nullptr, "Invalid object relations DBI", res);

        const QList<U2DataId> relatedIds = relationsDbi->getReferenceRelatedObjects(obj.entityRef.entityId, role, os);
        SAFE_POINT_OP(os, res);

        for (const U2DataId& objId : qAsConst(relatedIds)) {
            GObject* object = doc->getObjectById(objId);
            if (fromObjects.contains(object)) {
                res.append(object);
            }
        }
    }

    return res;
}

}  // namespace

QList<GObject*> GObjectUtils::findObjectsRelatedToObjectByRole(const GObject* obj, GObjectType resultObjType, GObjectRelationRole role, const QList<GObject*>& fromObjects, UnloadedObjectFilter f) {
    return findObjectsRelatedToObjectByRole(GObjectReference(obj), resultObjType, role, fromObjects, f);
}

QList<GObject*> GObjectUtils::findObjectsRelatedToObjectByRole(const GObjectReference& obj, GObjectType resultObjType, GObjectRelationRole role, const QList<GObject*>& fromObjects, UnloadedObjectFilter f) {
    QList<GObject*> res;
    QSet<GObject*> loadedObjs;
    QSet<GObject*> unloadedObjs;

    if (UOF_LoadedAndUnloaded == f) {
        foreach (GObject* o, fromObjects) {
            bool isUnloaded = o->getGObjectType() == GObjectTypes::UNLOADED;
            if ((resultObjType.isEmpty() && !isUnloaded) || o->getGObjectType() == resultObjType) {
                loadedObjs.insert(o);
            } else if (isUnloaded) {
                auto uo = qobject_cast<UnloadedObject*>(o);
                if (uo->getLoadedObjectType() == resultObjType) {
                    unloadedObjs.insert(o);
                }
            }
        }
    } else if (UOF_LoadedOnly == f) {
        loadedObjs = select(fromObjects, resultObjType, f).toSet();
    } else {
        FAIL("Unexpected unloaded object filter detected", res);
    }

    if (!unloadedObjs.isEmpty()) {
        res.append(findRelatedObjectsForUnloadedObjects(obj, role, unloadedObjs));
    }
    if (!loadedObjs.isEmpty()) {
        res.append(findRelatedObjectsForLoadedObjects(obj, role, loadedObjs));
    }
    return res;
}

QList<GObject*> GObjectUtils::selectObjectsWithRelation(const QList<GObject*>& objs, GObjectType type, GObjectRelationRole relationRole, UnloadedObjectFilter f, bool availableObjectsOnly) {
    QList<GObject*> res;
    for (GObject* obj : qAsConst(objs)) {
        QList<GObjectRelation> relations = obj->getObjectRelations();
        for (const GObjectRelation& r : qAsConst(relations)) {
            if (r.role != relationRole || (!type.isEmpty() && r.ref.objType != type)) {
                continue;
            }
            if (availableObjectsOnly) {
                Document* doc = AppContext::getProject()->findDocumentByURL(r.ref.docUrl);
                GObject* refObj = doc == nullptr ? nullptr : doc->findGObjectByName(r.ref.objName);
                if (refObj == nullptr || (f == UOF_LoadedOnly && refObj->getGObjectType() == GObjectTypes::UNLOADED)) {
                    continue;
                }
            }
            res.append(obj);
        }
    }
    return res;
}

GObject* GObjectUtils::selectObjectByReference(const GObjectReference& r, UnloadedObjectFilter f) {
    return selectObjectByReference(r, findAllObjects(f, r.objType), f);
}

GObject* GObjectUtils::selectObjectByReference(const GObjectReference& r, const QList<GObject*>& fromObjects, UnloadedObjectFilter f) {
    GObject* firstMatchedByName = nullptr;
    foreach (GObject* o, fromObjects) {
        Document* parentDoc = o->getDocument();
        if (r.entityRef.isValid() && !(r.entityRef == o->getEntityRef()) && parentDoc == nullptr) {
            continue;
        }
        if (o->getGObjectName() != r.objName) {
            continue;
        }
        if ((parentDoc == nullptr && !r.docUrl.isEmpty()) || (parentDoc->getURL().getURLString() != r.docUrl)) {
            continue;
        }
        if (r.objType != o->getGObjectType()) {
            if (f != UOF_LoadedAndUnloaded) {
                continue;
            }
            if (o->getGObjectType() != GObjectTypes::UNLOADED) {
                continue;
            }
            GObjectType oLoadedType = qobject_cast<UnloadedObject*>(o)->getLoadedObjectType();
            if (r.objType != oLoadedType) {
                continue;
            }
        }
        if (r.entityRef.isValid() && r.entityRef == o->getEntityRef()) {
            return o;  // matched by entityRef
        } else if (firstMatchedByName == nullptr) {
            firstMatchedByName = o;
        }
    }

    return firstMatchedByName;
}

DNATranslation* GObjectUtils::findComplementTT(const DNAAlphabet* al) {
    if (!al || !al->isNucleic()) {
        return nullptr;
    }
    return AppContext::getDNATranslationRegistry()->lookupComplementTranslation(al);
}

DNATranslation* GObjectUtils::findAminoTT(U2SequenceObject* so, bool fromHintsOnly, const QString& table) {
    if (!so || !so->getAlphabet() || !so->getAlphabet()->isNucleic()) {
        return nullptr;
    }
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QString tid = so->getGHints()->get(AMINO_TT_GOBJECT_HINT).toString();
    if (table != nullptr) {
        DNATranslation* res = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_NUCL_2_AMINO, table);
        return res;
    }
    DNATranslation* res = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_NUCL_2_AMINO, tid);
    if (res != nullptr || fromHintsOnly) {
        return res;
    }
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_NUCL_2_AMINO);
    if (!aminoTs.empty()) {
        res = tr->getStandardGeneticCodeTranslation(so->getAlphabet());
    }
    return res;
}

DNATranslation* GObjectUtils::findBackTranslationTT(U2SequenceObject* so, const QString& table) {
    if (!so || !so->getAlphabet() || !so->getAlphabet()->isAmino()) {
        return nullptr;
    }
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    if (table != nullptr) {
        DNATranslation* res = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_AMINO_2_NUCL, table);
        return res;
    }
    QList<DNATranslation*> dnaTs = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_AMINO_2_NUCL);
    DNATranslation* res = nullptr;
    if (!dnaTs.empty()) {
        res = dnaTs.first();
    }
    return res;
}

bool GObjectUtils::hasType(GObject* obj, const GObjectType& type) {
    if (obj->getGObjectType() == type) {
        return true;
    }
    if (obj->getGObjectType() != GObjectTypes::UNLOADED) {
        return false;
    }
    auto uo = qobject_cast<UnloadedObject*>(obj);
    return uo->getLoadedObjectType() == type;
}

void GObjectUtils::updateRelationsURL(GObject* o, const GUrl& fromURL, const GUrl& toURL) {
    updateRelationsURL(o, fromURL.getURLString(), toURL.getURLString());
}

void GObjectUtils::updateRelationsURL(GObject* o, const QString& fromURL, const QString& toURL) {
    QList<GObjectRelation> relations = o->getObjectRelations();
    bool changed = false;
    for (int i = 0; i < relations.size(); i++) {
        GObjectRelation& r = relations[i];
        if (r.ref.docUrl == fromURL) {
            r.ref.docUrl = toURL;
            changed = true;
        }
    }
    if (changed) {
        o->setObjectRelations(relations);
    }
}

void GObjectUtils::replaceAnnotationQualfier(SharedAnnotationData& a, const QString& name, const QString& newVal, bool create) {
    QVector<U2Qualifier> quals;
    a->findQualifiers(name, quals);
    QList<U2Qualifier> qualifiersList = a->qualifiers.toList();

    foreach (const U2Qualifier& q, quals) {
        qualifiersList.removeAll(q);
    }
    a->qualifiers = qualifiersList.toVector();

    if (create || !quals.isEmpty()) {
        a->qualifiers << U2Qualifier(name, newVal);
    }
}

GObject* GObjectUtils::createObject(const U2DbiRef& ref, const U2DataId& id, const QString& name) {
    const U2EntityRef entityRef(ref, id);

    U2OpStatus2Log os;
    DbiConnection con(ref, os);
    SAFE_POINT_OP(os, nullptr);

    const U2DataType type = con.dbi->getEntityTypeById(id);

    switch (type) {
        case U2Type::Sequence:
            return new U2SequenceObject(name, entityRef);
        case U2Type::Mca:
            return new MultipleChromatogramAlignmentObject(name, entityRef);
        case U2Type::Msa:
            return new MultipleSequenceAlignmentObject(name, entityRef);
        case U2Type::Assembly:
            return new AssemblyObject(name, entityRef);
        case U2Type::VariantTrack:
            return new VariantTrackObject(name, entityRef);
        case U2Type::AnnotationTable:
            return new AnnotationTableObject(name, entityRef);
        case U2Type::Text:
            return new TextObject(name, entityRef);
        case U2Type::PhyTree:
            return new PhyTreeObject(name, entityRef);
        case U2Type::BioStruct3D:
            return new BioStruct3DObject(name, entityRef);
        case U2Type::Chromatogram:
            return new DNAChromatogramObject(name, entityRef);
        case U2Type::PFMatrix:
            return new PFMatrixObject(name, entityRef);
        case U2Type::PWMatrix:
            return new PWMatrixObject(name, entityRef);
        case U2Type::CrossDatabaseReference:
            return nullptr;
        default:
            coreLog.trace(QObject::tr("Unsupported object type: %1").arg(type));
            return nullptr;
    }

    return nullptr;
}

}  // namespace U2
