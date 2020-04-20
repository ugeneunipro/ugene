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

#ifndef _U2_ANNOTATION_GROUP_H_
#define _U2_ANNOTATION_GROUP_H_

#include <QMetaType>

namespace U2 {

class Annotation;
class AnnotationTableObject;
class U2Feature;

class U2CORE_EXPORT AnnotationGroup : public U2Entity {
public:
                            AnnotationGroup();
                            AnnotationGroup(const U2DataId &featureId, const QString &name, AnnotationGroup *parentGroup,
                                AnnotationTableObject *parentObject);

                            ~AnnotationGroup();
    /*
     * Returning value specifies whether the @name may be used as an annotation name or not.
     * @pathMode allows to take into account group path separation symbol (currently "/")
     * I.e. if @pathMode is true then that symbol is allowed, otherwise it is not.
     */
    static bool             isValidGroupName(const QString &name, bool pathMode);
    /*
     * After calling this function @set additionally contains all the distinct annotations
     * belonging to the subtree of this annotation group
     */
    void                    findAllAnnotationsInGroupSubTree(QList<Annotation *> &set) const;

    QList<Annotation *>     getAnnotations(bool recurcively = false) const;

    bool                    hasAnnotations() const;

    QList<Annotation *>     addAnnotations(const QList<SharedAnnotationData> &anns);

    void                    addShallowAnnotations(const QList<Annotation *> &anns, bool newAnnotations);

    void                    removeAnnotations(const QList<Annotation *> &anns);

    QList<AnnotationGroup *> getSubgroups() const;

    void                    removeSubgroup(AnnotationGroup *g);

    QString                 getName() const;

    void                    setName(const QString &newName);

    QString                 getGroupPath() const;

    AnnotationTableObject * getGObject() const;

    /*
     * Returns parent annotation group. Returns *this (i.e. the same group) for a top-level group
     */
    AnnotationGroup *       getParentGroup();
    /*
     * Returns subgroup located in @path. @create specifies whether it's required to create
     * a new group(s) accordingly to @path if they are not exist. If @create == false and
     * there are no subgroups satisfying @path, then *this is returned.
     */
    AnnotationGroup *       getSubgroup(const QString &path, bool create);

    AnnotationGroup *       addSubgroup(const U2Feature &feature);

    Annotation *            findAnnotationById(const U2DataId &featureId) const;

    AnnotationGroup *       findSubgroupById(const U2DataId &featureId) const;

    void                    getSubgroupPaths(QStringList &res) const;
    /**
     * Removes all references to subgroups and annotations
     */
    void                    clear();
    /**
     * Returns the depth of a root subtree containing the feature. For top-level features depth is "1"
     */
    int                     getGroupDepth() const;

    bool                    isParentOf(AnnotationGroup *g) const;

    bool                    isRootGroup() const;
    /**
     * Returns "true" if the parent of the group is root group
     */
    bool                    isTopLevelGroup() const;

    bool                    operator ==(const AnnotationGroup &other) const;

    static const QString    ROOT_GROUP_NAME;
    static const QChar      GROUP_PATH_SEPARATOR;

private:
    AnnotationTableObject *         parentObject;
    QString                         name;

    AnnotationGroup *               parentGroup;
    QList<AnnotationGroup *>        subgroups;
    QList<Annotation *>             annotations;
    QHash<U2DataId, Annotation *>   annotationById;

    static const bool annotationGroupMetaRegistered;
};

} // namespace U2

Q_DECLARE_METATYPE(U2::AnnotationGroup)
Q_DECLARE_METATYPE(U2::AnnotationGroup *)

#endif //
