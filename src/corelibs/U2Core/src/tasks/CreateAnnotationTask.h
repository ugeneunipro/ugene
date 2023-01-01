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

#ifndef _U2_CREATE_ANNOTATION_TASK_H_
#define _U2_CREATE_ANNOTATION_TASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/Task.h>

namespace U2 {

class U2CORE_EXPORT CreateAnnotationsTask : public Task {
    Q_OBJECT

    CreateAnnotationsTask(bool isAnnotationObjectShared);

public:
    /**
     * Adds annotations to the object.
     *
     * 'isAnnotationObjectShared'- for the shared annotation object the task will add all annotations in main UI thread.
     *   if the annotation object is not 'shared' the annotations can be created in a separate thread, thus reducing the UI thread lock time.
     */
    CreateAnnotationsTask(AnnotationTableObject* annotationTableObject,
                          const QMap<QString, QList<SharedAnnotationData>>& annotationsByGroupMap,
                          bool isAnnotationObjectShared = true);

    /** Adds annotations to the object. Loads object if needed. Works only in project context. */
    CreateAnnotationsTask(const GObjectReference& ref, const QList<SharedAnnotationData>& data, const QString& groupName = "");

    void run() override;
    ReportResult report() override;

    AnnotationTableObject* getAnnotationTableObject() const;
    int getAnnotationCount() const;
    QList<Annotation*> getResultAnnotations() const;

private:
    GObjectReference annotationTableObjectRef;
    QPointer<AnnotationTableObject> annotationTableObjectPointer;
    QMap<AnnotationGroup*, QList<Annotation*>> annotationsByGroupMap;
    QMap<QString, QList<SharedAnnotationData>> annotationDatasByGroupNameMap;
    QList<Annotation*> resultAnnotations;
};

}  // namespace U2

#endif
