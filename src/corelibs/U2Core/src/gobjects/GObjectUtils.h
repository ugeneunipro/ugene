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

#ifndef _U2_GOBJECT_UTILS_H_
#define _U2_GOBJECT_UTILS_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/GObject.h>

namespace U2 {

class U2SequenceObject;
class DNATranslation;
class DNASequence;
class GUrl;
class DNAAlphabet;

class U2CORE_EXPORT GObjectUtils {
public:

    static QList<GObject*>  select(const QList<GObject*>& objects, GObjectType type, UnloadedObjectFilter f);

    static GObject*         selectOne(const QList<GObject*>& objects, GObjectType type, UnloadedObjectFilter f);

    static QList<GObject*>  findAllObjects(UnloadedObjectFilter f, GObjectType t = GObjectType());

    /*
     * Select objects from @fromObjects that are referenced by relations stored in @obj with @relationRole and @type.
     */
    static QList<GObject*> selectRelations(GObject* obj, GObjectType type,
        GObjectRelationRole relationRole, const QList<GObject*>& fromObjects,
        UnloadedObjectFilter f);

    static QList<GObject *> selectRelationsFromParentDoc(const GObject* obj, const GObjectType &type, GObjectRelationRole relationRole);

    /*
     * Returns list of objects that references the @obj with @relationRole and have @resultObjType as well.
     * Result list is a sublist of @fromObjects list.
     */
    static QList<GObject *>  findObjectsRelatedToObjectByRole(const GObject *obj, GObjectType resultObjType, GObjectRelationRole role,
        const QList<GObject *> &fromObjects, UnloadedObjectFilter f);

    static QList<GObject*> findObjectsRelatedToObjectByRole(const GObjectReference &obj, GObjectType resultObjType, GObjectRelationRole role,
        const QList<GObject*>& fromObjects, UnloadedObjectFilter f);

    /*
     * Selects objects from 'fromObjects' with filter 'f' that have relation(relationRole, type)
     * 'availableObjectsOnly'  -> check if the related object is in project
     */
    static QList<GObject *>  selectObjectsWithRelation(const QList<GObject *> &fromObjects,
        GObjectType type, GObjectRelationRole relationRole, UnloadedObjectFilter f,
        bool availableObjectsOnly);

    static GObject*         selectObjectByReference(const GObjectReference& r, const QList<GObject*>& fromObjects, UnloadedObjectFilter f);

    static GObject*         selectObjectByReference(const GObjectReference& r, UnloadedObjectFilter f);

    static DNATranslation*  findComplementTT(const DNAAlphabet* al);

    static DNATranslation*  findAminoTT(U2SequenceObject* so, bool fromHintsOnly, const QString& table = NULL);

    static DNATranslation*  findBackTranslationTT(U2SequenceObject* so, const QString& table = NULL);

    //checks object type for both loaded and unloaded states
    static bool             hasType(GObject* obj, const GObjectType& type);

    static void             updateRelationsURL(GObject* o, const QString& fromURL, const QString& toURL);

    static void             updateRelationsURL(GObject* o, const GUrl& fromURL, const GUrl& toURL);

    static void             replaceAnnotationQualfier(SharedAnnotationData &a, const QString &name, const QString &newVal, bool create = false);

    static GObject *        createObject(const U2DbiRef &ref, const U2DataId &id, const QString &name);
};

} // namespace

#endif
