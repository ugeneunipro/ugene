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

#ifndef _U2_CREATE_ANNOTATION_TASK_H_
#define _U2_CREATE_ANNOTATION_TASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/Task.h>

namespace U2 {

class U2CORE_EXPORT CreateAnnotationsTask : public Task {
    Q_OBJECT
public:
    // Adds annotations to the object. Waits object to be unlocked if needed
    // Works only in a context of active project
                                    CreateAnnotationsTask(AnnotationTableObject *o, const QList<SharedAnnotationData> &data,
                                        const QString &group = QString());
                                    CreateAnnotationsTask(const GObjectReference &ref, const QList<SharedAnnotationData> &data,
                                        const QString &group = QString());
                                    CreateAnnotationsTask(AnnotationTableObject *o, const QMap<QString, QList<SharedAnnotationData> > &data);

    void                            run();
    ReportResult                    report();
    AnnotationTableObject *         getGObject() const;
    int                             getAnnotationCount() const;
    QList<Annotation *>             getResultAnnotations() const;

private:
    void                                            initAnnObjectRef();

    GObjectReference                                aRef;
    QPointer<AnnotationTableObject>                 aobj;
    QMap<AnnotationGroup *, QList<Annotation *> >   group2Annotations;
    QMap<QString, QList<SharedAnnotationData> >     aData;
    QList<Annotation *>                             resultAnnotations;
};

} //namespace U2

#endif
