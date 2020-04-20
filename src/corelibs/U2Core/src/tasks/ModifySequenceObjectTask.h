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

#ifndef _U2_MODIFY_SEQUENCE_OBJECT_TASK_H_
#define _U2_MODIFY_SEQUENCE_OBJECT_TASK_H_

#include <U2Core/DNASequence.h>
#include <U2Core/GUrl.h>
#include <U2Core/Task.h>
#include <U2Core/U1AnnotationUtils.h>


namespace U2 {

class Document;

class U2CORE_EXPORT ModifySequenceContentTask : public Task {
    Q_OBJECT
public:
    ModifySequenceContentTask(const DocumentFormatId &dfId, U2SequenceObject *seqObj, const U2Region &regionToReplace, const DNASequence &sequence2Insert,
        bool recalculateQualifiers = false, U1AnnotationUtils::AnnotationStrategyForResize _str = U1AnnotationUtils::AnnotationStrategyForResize_Resize,
        const GUrl &url = GUrl(), bool mergeAnnotations = false);

    Task::ReportResult report();
    QString generateReport() const;

    // Returns number of bases inserted into sequence. If sequence size is reduced the returned value is < 0
    qint64 getSequenceLengthDelta() const;
    U2SequenceObject* getSequenceObject() const {return seqObj;}

private:
    void cloneSequenceAndAnnotations();

    DocumentFormatId                                        resultFormatId;
    bool                                                    mergeAnnotations;
    bool                                                    recalculateQualifiers;
    Document *                                              curDoc;
    Document *                                              newDoc;
    bool                                                    inplaceMod;
    GUrl                                                    url;
    U1AnnotationUtils::AnnotationStrategyForResize          strat;
    QList<Document *>                                       docs;
    U2SequenceObject *                                      seqObj;
    U2Region                                                regionToReplace;
    DNASequence                                             sequence2Insert;
    QMap<Annotation *, QList<QPair<QString, QString> > >    annotationForReport;
};

}//ns

#endif
