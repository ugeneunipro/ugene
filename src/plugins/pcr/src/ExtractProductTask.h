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

#ifndef _U2_EXTRACT_PRODUCT_TASK_H_
#define _U2_EXTRACT_PRODUCT_TASK_H_

#include <U2Core/DNASequence.h>

#include "InSilicoPcrTask.h"

namespace U2 {

class AnnotationTableObject;

class ExtractProductSettings {
public:
    enum AnnotationsExtraction {Inner, All, None};

    ExtractProductSettings();

    U2EntityRef sequenceRef;
    QList<U2EntityRef> annotationRefs;
    QString outputFile;
    AnnotationsExtraction annotationsExtraction;
    U2DbiRef targetDbiRef;
};

class ExtractProductTask : public Task {
    Q_OBJECT
public:
    ExtractProductTask(const InSilicoPcrProduct &product, const ExtractProductSettings &settings);
    ~ExtractProductTask();

    // Task
    void run();

    /* Moves the document to the main thread */
    Document * takeResult();
    const InSilicoPcrProduct & getProduct() const;

    static QString getProductName(const QString &sequenceName, qint64 sequenceLength, const U2Region &region, bool fileName = false);

private:
    DNASequence getProductSequence();
    DNASequence extractTargetSequence();
    QByteArray toProductSequence(const QByteArray &targetSequence) const;
    void addProductAnnotations(AnnotationTableObject *targetObject, const U2EntityRef &annsRef) const;

    static SharedAnnotationData getPrimerAnnotation(int matchLengh, U2Strand::Direction strand, int sequenceLength);

private:
    InSilicoPcrProduct product;
    ExtractProductSettings settings;
    qint64 wholeSequenceLength;

    Document *result;
};

class ExtractProductWrapperTask : public Task {
    Q_OBJECT
public:
    ExtractProductWrapperTask(const InSilicoPcrProduct &product, const QString &sequenceName, qint64 sequenceLength, const ExtractProductSettings &settings);

    // Task
    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    ReportResult report();

private:
    void prepareUrl(const InSilicoPcrProduct &product, const QString &sequenceName, qint64 sequenceLength);

private:
    ExtractProductTask *extractTask;
    ExtractProductSettings settings;
};

} // U2

#endif // _U2_EXTRACT_PRODUCT_TASK_H_
