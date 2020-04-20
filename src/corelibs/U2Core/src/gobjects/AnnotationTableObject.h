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

#ifndef _U2_FEATURES_TABLE_OBJECT_H_
#define _U2_FEATURES_TABLE_OBJECT_H_

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/GObject.h>

namespace U2 {

class AnnotationModification;

class U2CORE_EXPORT AnnotationTableObject : public GObject {
    Q_OBJECT
public:
    /**
     * This constructor is intended for interaction with Document. It encapsulates creation of
     * annotation table entity in DB.
     */
    AnnotationTableObject(const QString &objectName, const U2DbiRef &dbiRef,
        const QVariantMap &hintsMap = QVariantMap());
    /**
     * This constructor works with existed annotation table entity available via @tableRef.
     */
    AnnotationTableObject(const QString &objectName, const U2EntityRef &tableRef,
        const QVariantMap &hintsMap = QVariantMap());

    ~AnnotationTableObject();
    /**
     * Converts all the features stored in DB to annotations and returns the result
     */
    QList<Annotation *>     getAnnotations() const;
    /**
     * Returning value specifies if at least a single annotation belongs to the object
     */
    bool                    hasAnnotations() const;
    /**
     * Returns root group
     */
    AnnotationGroup *       getRootGroup();
    /**
     * Adds to DB a set of features based on @annotations
     */
    QList<Annotation *>     addAnnotations(const QList<SharedAnnotationData> &annotations, const QString &groupName = QString());
    /**
     * Removes features corresponding to @annotations from DB
     */
    void                    removeAnnotations(const QList<Annotation *> &annotations);
    /**
     * Reimplemented from GObject
     */
    GObject *               clone(const U2DbiRef &ref, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;
    /**
     * Returns list of annotations having @name
     */
    QList<Annotation *>     getAnnotationsByName(const QString &name) const;
    /**
     * Returns list of annotations having belonging to the @region. @contains specifies
     * whether the result set should include only annotations that has no region or its part
     * beyond the @region or each annotation that intersects it.
     */
    QList<Annotation *>     getAnnotationsByRegion(const U2Region &region, bool contains = false) const;
    /**
    * Return the list of annotations having @featureType
    */
    QList<Annotation *>     getAnnotationsByType(const U2FeatureType featureType) const;
    /**
     * Reimplemented from GObject
     */
    bool                    checkConstraints(const GObjectConstraints *c) const;
    void                    setGObjectName(const QString &newName);
    /**
     * Returns the root feature
     */
    U2DataId               getRootFeatureId() const;

    //////////////////////////////////////////////////////////////////////////
    // Access to signals that drive GUI //////////////////////////////////////

    void                    emit_onAnnotationsAdded(const QList<Annotation *> &l);
    void                    emit_onAnnotationsModified(const AnnotationModification &annotationModification);
    void                    emit_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications);
    void                    emit_onAnnotationsRemoved(const QList<Annotation *> &a);
    void                    emit_onGroupCreated(AnnotationGroup *g);
    void                    emit_onGroupRemoved(AnnotationGroup *p, AnnotationGroup *g);
    void                    emit_onGroupRenamed(AnnotationGroup *g);
    void                    emit_onAnnotationsInGroupRemoved(const QList<Annotation *> &l, AnnotationGroup *gr);

signals:
    void                    si_onAnnotationsAdded(const QList<Annotation *> &a);
    void                    si_onAnnotationsRemoved(const QList<Annotation *> &a);
    void                    si_onAnnotationsInGroupRemoved(const QList<Annotation *> &l, AnnotationGroup *g);
    void                    si_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications);
    void                    si_onGroupCreated(AnnotationGroup *g);
    void                    si_onGroupRemoved(AnnotationGroup *p, AnnotationGroup *removed);
    void                    si_onGroupRenamed(AnnotationGroup *g);

protected:
    void                    loadDataCore(U2OpStatus &os);

private:
    AnnotationGroup *       rootGroup;
};

} // namespace U2

#endif
