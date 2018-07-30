/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DocumentModel.h>
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
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectRelationsDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UnloadedObject.h>
#include <U2Core/VariantTrackObject.h>

#include "GObjectUtils.h"

namespace U2 {

QList<GObject*> GObjectUtils::select(const QList<GObject*>& objs, GObjectType t, UnloadedObjectFilter f) {
    QList<GObject*> res;
    foreach(GObject* o, objs) {
        bool isUnloaded = o->getGObjectType() == GObjectTypes::UNLOADED;
        if ((t.isEmpty() && (f == UOF_LoadedAndUnloaded || !isUnloaded)) || o->getGObjectType() == t) {
            res.append(o);
        } else if (f == UOF_LoadedAndUnloaded && isUnloaded) {
            UnloadedObject* uo = qobject_cast<UnloadedObject*>(o);
            if (uo->getLoadedObjectType() == t) {
                res.append(o);
            }
        }
    }
    return res;
}

GObject* GObjectUtils::selectOne(const QList<GObject*>& objects, GObjectType type, UnloadedObjectFilter f) {
    QList<GObject*> res = select(objects, type, f);
    return res.isEmpty() ? NULL : res.first();
}


QList<GObject*> GObjectUtils::findAllObjects(UnloadedObjectFilter f, GObjectType t) {
    QList<GObject*> res;
    SAFE_POINT(AppContext::getProject()!=NULL, "No active project found", res);

    foreach(Document* doc, AppContext::getProject()->getDocuments()) {
        if (t.isEmpty()) {
            if (doc->isLoaded() || f == UOF_LoadedAndUnloaded) {
                res+=doc->getObjects();
            }
        } else {
            res+=doc->findGObjectByType(t, f);
        }
    }
    return res;
}


QList<GObject*> GObjectUtils::selectRelations(GObject* obj, GObjectType type,
    GObjectRelationRole relationRole, const QList<GObject*>& fromObjects,
    UnloadedObjectFilter f)
{
    QList<GObject*> res;
    QList<GObjectRelation> relations = obj->getObjectRelations();
    foreach(const GObjectRelation& r, relations) {
        if (r.role != relationRole || (!type.isEmpty() && r.ref.objType!=type)) {
            continue;
        }
        GObject* obj = selectObjectByReference(r.ref, fromObjects, f);
        if (obj!=NULL) {
            res.append(obj);
        }
    }
    return res;
}

QList<GObject *> GObjectUtils::selectRelationsFromParentDoc(const GObject* obj, const GObjectType &type, GObjectRelationRole relationRole) {
    QList<GObject *> result;

    Document *parentDoc = obj->getDocument();
    SAFE_POINT(NULL != parentDoc, "Invalid parent document detected", result);

    U2OpStatus2Log os;
    DbiConnection con(parentDoc->getDbiRef(), os);
    U2ObjectRelationsDbi *relationsDbi = con.dbi->getObjectRelationsDbi();
    SAFE_POINT(NULL != relationsDbi, "Invalid object relations DBI", result);

    const QList<U2ObjectRelation> relations = relationsDbi->getObjectRelations(obj->getEntityRef().entityId, os);
    CHECK_OP(os, result);
    foreach (const U2ObjectRelation &relation, relations) {
        if (type == relation.referencedType && relationRole == relation.relationRole) {
            GObject *referenceObj = parentDoc->getObjectById(relation.referencedObject);
            if (NULL != referenceObj) {
                result.append(referenceObj);
            } else {
                os.setError(QString("Reference object with ID '%1' and name '%2' not found in the document")
                    .arg(QString(relation.referencedObject)).arg(relation.referencedName));
            }
        }
    }
    return result;
}

namespace {

QList<GObject *> findRelatedObjectsForUnloadedObjects(const GObjectReference &obj, GObjectRelationRole role, const QSet<GObject*> &fromObjects) {
    QList<GObject *> res;
    GObjectRelation objRelation(obj, role);
    foreach(GObject* o, fromObjects) {
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

bool objectHasInMemoryRelationToReference(GObject *object, const GObjectReference &reference, GObjectRelationRole role) {
    SAFE_POINT(NULL != object && reference.isValid(), "Invalid object reference detected", false);
    return object->getGHints()->get(GObjectHint_RelatedObjects).value<QList<GObjectRelation> >().contains(GObjectRelation(reference, role));
}

QList<GObject *> findRelatedObjectsForLoadedObjects(const GObjectReference &obj, GObjectRelationRole role, const QSet<GObject*> &fromObjects) {
    QList<GObject *> res;

    const GObjectRelation objRelation(obj, role);
    QHash<Document *, U2DbiRef> doc2DbiRef;
    foreach (GObject *object, fromObjects) {
        Document *doc = object->getDocument();
        SAFE_POINT(NULL != doc, "Invalid parent document detected", res);
        if (!doc->isDatabaseConnection()) {
            if (object->hasObjectRelation(objRelation)) { // this 'if' branch has to be distinctive from the enclosing one
                res.append(object);
            }
        } else {
            const U2DbiRef dbiRef = object->getEntityRef().dbiRef;
            if (obj.entityRef.dbiRef == dbiRef) {
                doc2DbiRef.insert(doc, obj.entityRef.dbiRef);
            } else if (objectHasInMemoryRelationToReference(object, obj, role)) {
                res.append(object);
            }
        }
    }

    U2OpStatusImpl os;
    foreach (Document *doc, doc2DbiRef.keys()) {
        const U2DbiRef dbiRef = doc2DbiRef.value(doc);
        if (!dbiRef.isValid()) {
            coreLog.error("Invalid DBI reference detected");
            continue;
        }
        DbiConnection con(dbiRef, os);
        U2ObjectRelationsDbi *relationsDbi = con.dbi->getObjectRelationsDbi();
        SAFE_POINT(NULL != relationsDbi, "Invalid object relations DBI", res);

        const QList<U2DataId> relatedIds = relationsDbi->getReferenceRelatedObjects(obj.entityRef.entityId, role, os);
        SAFE_POINT_OP(os, res);

        foreach (const U2DataId &objId, relatedIds) {
            GObject *object = doc->getObjectById(objId);
            if (fromObjects.contains(object)) {
                res.append(object);
            }
        }
    }

    return res;
}

}

QList<GObject*> GObjectUtils::findObjectsRelatedToObjectByRole(const GObject* obj, GObjectType resultObjType, GObjectRelationRole role,
    const QList<GObject*>& fromObjects, UnloadedObjectFilter f)
{
    return findObjectsRelatedToObjectByRole(GObjectReference(obj), resultObjType, role, fromObjects, f);
}

QList<GObject*> GObjectUtils::findObjectsRelatedToObjectByRole(const GObjectReference &obj, GObjectType resultObjType, GObjectRelationRole role,
    const QList<GObject*>& fromObjects, UnloadedObjectFilter f)
{
    QList<GObject *> res;
    QSet<GObject *> loadedObjs;
    QSet<GObject *> unloadedObjs;

    if (UOF_LoadedAndUnloaded == f) {
        foreach(GObject* o, fromObjects) {
            bool isUnloaded = o->getGObjectType() == GObjectTypes::UNLOADED;
            if ((resultObjType.isEmpty() && !isUnloaded) || o->getGObjectType() == resultObjType) {
                loadedObjs.insert(o);
            } else if (isUnloaded) {
                UnloadedObject* uo = qobject_cast<UnloadedObject*>(o);
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

QList<GObject*> GObjectUtils::selectObjectsWithRelation(const QList<GObject*>& objs, GObjectType type, GObjectRelationRole relationRole,
    UnloadedObjectFilter f, bool availableObjectsOnly)
{
    QList<GObject*> res;
    foreach(GObject* obj, objs) {
        QList<GObjectRelation> relations = obj->getObjectRelations();
        foreach(const GObjectRelation& r, relations) {
            if (r.role != relationRole || (!type.isEmpty() && r.ref.objType!=type)) {
                continue;
            }
            if (availableObjectsOnly) {
                Document* doc = AppContext::getProject()->findDocumentByURL(r.ref.docUrl);
                GObject* refObj = doc == NULL ? NULL : doc->findGObjectByName(r.ref.objName);
                if (refObj == NULL || (f == UOF_LoadedOnly && refObj->getGObjectType() == GObjectTypes::UNLOADED)) {
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
    GObject* firstMatchedByName = NULL;
    foreach(GObject* o, fromObjects) {
        Document *parentDoc = o->getDocument();
        if (r.entityRef.isValid() && !(r.entityRef == o->getEntityRef()) && (parentDoc == NULL || parentDoc->isDatabaseConnection())) {
            continue;
        }
        if (o->getGObjectName() != r.objName) {
            continue;
        }
        if ((parentDoc == NULL && !r.docUrl.isEmpty()) || (parentDoc->getURL().getURLString() != r.docUrl)) {
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
            return o; // matched by entityRef
        } else if (firstMatchedByName == NULL) {
            firstMatchedByName = o;
        }
    }

    return firstMatchedByName;
}

DNATranslation* GObjectUtils::findComplementTT(const DNAAlphabet* al) {
    if (!al || !al->isNucleic()) {
        return NULL;
    }
    return AppContext::getDNATranslationRegistry()->lookupComplementTranslation(al);
}

DNATranslation* GObjectUtils::findAminoTT(U2SequenceObject* so, bool fromHintsOnly, const QString& table) {
    if (!so || !so->getAlphabet() || !so->getAlphabet()->isNucleic()) {
        return NULL;
    }
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QString tid = so->getGHints()->get(AMINO_TT_GOBJECT_HINT).toString();
    if (table != NULL) {
        DNATranslation* res = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_NUCL_2_AMINO, table);
        return res;
    }
    DNATranslation* res = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_NUCL_2_AMINO, tid);
    if (res != NULL || fromHintsOnly) {
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
        return NULL;
    }
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    if (table != NULL) {
        DNATranslation* res = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_AMINO_2_NUCL, table);
        return res;
    }
    QList<DNATranslation*> dnaTs = tr->lookupTranslation(so->getAlphabet(), DNATranslationType_AMINO_2_NUCL);
    DNATranslation* res = NULL;
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
    UnloadedObject* uo = qobject_cast<UnloadedObject*>(obj);
    return uo->getLoadedObjectType() == type;
}


void GObjectUtils::updateRelationsURL(GObject* o, const GUrl& fromURL, const GUrl& toURL) {
    updateRelationsURL(o, fromURL.getURLString(), toURL.getURLString());
}

void GObjectUtils::updateRelationsURL(GObject* o, const QString& fromURL, const QString& toURL) {
    QList<GObjectRelation> relations = o->getObjectRelations();
    bool changed = false;
    for(int i=0; i<relations.size(); i++) {
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

void GObjectUtils::replaceAnnotationQualfier(SharedAnnotationData &a, const QString &name, const QString &newVal, bool create) {
    QVector<U2Qualifier> quals;
    a->findQualifiers(name, quals);
    QList<U2Qualifier> qualifiersList = a->qualifiers.toList();

    foreach (const U2Qualifier &q, quals) {
        qualifiersList.removeAll(q);
    }
    a->qualifiers = qualifiersList.toVector();

    if (create || !quals.isEmpty()) {
        a->qualifiers << U2Qualifier(name, newVal);
    }
}

GObject *GObjectUtils::createObject(const U2DbiRef &ref, const U2DataId &id, const QString &name) {
    const U2EntityRef entityRef(ref, id);

    U2OpStatus2Log os;
    DbiConnection con(ref, os);
    SAFE_POINT_OP(os, NULL);

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
        return NULL;
    default:
        coreLog.trace(QObject::tr("Unsupported object type: %1").arg(type));
        return NULL;
    }

    return NULL;
}

} // namespace U2
