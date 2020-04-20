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

#ifndef _U2_ADVANNOTATION_CREATION_H_
#define _U2_ADVANNOTATION_CREATION_H_

#include <QPointer>

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

class QAction;

namespace U2 {

class GObjectReference;
class CreateAnnotationsTask;
class AnnotatedDNAView;
class U2SequenceObject;

class U2VIEW_EXPORT ADVAnnotationCreation : public QObject {
    Q_OBJECT
public:
    ADVAnnotationCreation(AnnotatedDNAView* v);

    QAction* getCreateAnnotationAction() const {return createAction;}

private slots:
    void sl_createAnnotation();

private:
    AnnotatedDNAView* ctx;
    QAction* createAction;
};


class U2VIEW_EXPORT ADVCreateAnnotationsTask : public Task {
    Q_OBJECT
public:
    ADVCreateAnnotationsTask(AnnotatedDNAView *ctx, const GObjectReference &aobjRef, const QString &group, const QList<SharedAnnotationData> &data);

    ReportResult report();

private:
    QPointer<AnnotatedDNAView> ctx;
    CreateAnnotationsTask* t;
};

} // namespace U2

#endif
