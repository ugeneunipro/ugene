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

class U2CORE_EXPORT AnnotationSelection : public GSelection {
    Q_OBJECT
public:
    AnnotationSelection(QObject* p = nullptr);

    const QList<Annotation*>& getAnnotations() const;

    /** Adds annotation to selection. Does nothing if annotation is already in the selection. */
    void add(Annotation* a);

    /** Clears the current selection and adds all annotation from the list into the selection. */
    void setAnnotations(QList<Annotation*> annotationList);

    void remove(Annotation* a);

    bool isEmpty() const;

    void clear();

    void removeObjectAnnotations(const AnnotationTableObject* obj);

    bool contains(Annotation* a) const;

    /**
     * Returns sequence under annotation.
     * If complTT or transTT is not 'nullptr', the corresponding transformation is applied.
     */
    static QByteArray getSequenceUnderAnnotation(const U2EntityRef& sequenceObjectRef,
                                                 const Annotation* annotation,
                                                 const DNATranslation* complTT,
                                                 const DNATranslation* aminoTT,
                                                 U2OpStatus& os);

signals:

    void si_selectionChanged(AnnotationSelection* thiz, const QList<Annotation*>& added, const QList<Annotation*>& removed);

private:
    QList<Annotation*> selection;
};

//////////////////////////////////////////////////////////////////////////
// AnnotationGroupSelection

class U2CORE_EXPORT AnnotationGroupSelection : public GSelection {
    Q_OBJECT
public:
    AnnotationGroupSelection(QObject* p = nullptr);

    const QList<AnnotationGroup*>& getSelection() const;

    void addToSelection(AnnotationGroup* g);

    void removeFromSelection(AnnotationGroup* g);

    bool isEmpty() const;

    void clear();

    bool contains(AnnotationGroup* g) const;

signals:

    void si_selectionChanged(AnnotationGroupSelection* thiz, const QList<AnnotationGroup*>& added, const QList<AnnotationGroup*>& removed);

private:
    QList<AnnotationGroup*> selection;
};

}  // namespace U2

#endif
