/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#ifndef _U2_ANNOTATION_SELECTION_H_
#define _U2_ANNOTATION_SELECTION_H_

#include <U2Core/SelectionModel.h>
#include <U2Core/U2Region.h>

namespace U2 {

class Annotation;
class AnnotationGroup;
class AnnotationTableObject;
class DNATranslation;
class U2EntityRef;
class U2OpStatus;

class U2CORE_EXPORT AnnotationSelectionData {
public:
    AnnotationSelectionData(Annotation *a, int lIdx);
    AnnotationSelectionData(Annotation *a, const QList<int> &listIdx);

    bool                operator ==(const AnnotationSelectionData &d) const;
    int                 getSelectedRegionsLen() const;
    /**
     * Returns all regions from selected annotations locations without any modification
     */
    QVector<U2Region>   getSelectedRegions() const;

    bool contains(int locIndx) const;
    bool contains(const AnnotationSelectionData& other) const;

    /**
     * Returns true if the selection is empty after the deletion
     * MUST CHECK result after using this method
     */
    bool deselectLocation(int locIndx);

    void addLocation(int locIdx);

    /**
     * The annotation selected
     */
    Annotation *        annotation;
    /**
     * List of Location idx that is preferable for visualization
     * 0..N == idx of U2Region in 'location' field
     */
    QList<int>          locationIdxList;
};

class  U2CORE_EXPORT AnnotationSelection : public GSelection {
    Q_OBJECT
public:
                                            AnnotationSelection(QObject *p = NULL);

    const QList<AnnotationSelectionData> &  getSelection() const;

    const AnnotationSelectionData *         getAnnotationData(Annotation *a) const;
    /**
     * Adds annotation to selection.
     * If annotation is already in selection and have a different locationIdx
     * -> removes the old annotaiton selection data
     * and adds the annotation again with updated locationIdx
     */
    void                                    addToSelection(Annotation *a, int locationIdx = -1);

    void                                    removeFromSelection(Annotation *a, int locationIdx = -1);

    bool                                    isEmpty() const;

    void                                    clear();

    void                                    removeObjectAnnotations(AnnotationTableObject *obj);

    bool                                    contains(Annotation *a) const;
    bool                                    contains(Annotation *a, int locationIdx) const;
    bool                                    contains(const AnnotationSelectionData &selData) const;

    static void                             getAnnotationSequence(QByteArray &res, const AnnotationSelectionData &ad, char gapSym,
                                                const U2EntityRef &ref, const DNATranslation *complTT, const DNATranslation *aminoTT, U2OpStatus &os);
    /**
     * Returns list of locations of all selected annotations that belongs to the objects in list
     */
    QVector<U2Region>                       getSelectedLocations(const QSet<AnnotationTableObject *> &objects) const;

signals:
    void si_selectionChanged(AnnotationSelection *thiz, const QList<Annotation *> &added, const QList<Annotation *> &removed);

private:
    QList<AnnotationSelectionData> selection;
};

//////////////////////////////////////////////////////////////////////////
// AnnotationGroupSelection

class  U2CORE_EXPORT AnnotationGroupSelection : public GSelection {
    Q_OBJECT
public:
                                        AnnotationGroupSelection(QObject *p = NULL);

    const QList<AnnotationGroup *> &    getSelection() const;

    void                                addToSelection(AnnotationGroup *g);

    void                                removeFromSelection(AnnotationGroup *g);

    bool                                isEmpty() const;

    void                                clear();

    void                                removeObjectGroups(AnnotationTableObject *obj);

    bool                                contains(AnnotationGroup *g) const;

signals:
    void si_selectionChanged(AnnotationGroupSelection *thiz, const QList<AnnotationGroup *> &added, const QList<AnnotationGroup *> &removed);

private:
    QList<AnnotationGroup *> selection;
};


}//namespace

#endif

