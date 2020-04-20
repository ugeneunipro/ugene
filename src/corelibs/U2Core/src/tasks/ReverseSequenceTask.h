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

#ifndef _U2_REVERSE_SEQUENCE_TASK_H_
#define _U2_REVERSE_SEQUENCE_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/DocumentModel.h>

namespace U2 {

class U2SequenceObject;
class AnnotationTableObject;
class DNATranslation;
class DNASequenceSelection;

class U2CORE_EXPORT ReverseComplementSequenceTask : public Task
{
    Q_OBJECT
public:
    ReverseComplementSequenceTask( U2SequenceObject *dnaObj,
                                   const QList<AnnotationTableObject *> &annotations,
                                   DNASequenceSelection *selection,
                                   DNATranslation *complTT );
private:
    U2SequenceObject *              seqObj;
    QList<AnnotationTableObject *>    aObjs;
    DNASequenceSelection *          selection;
    DNATranslation *                complTT;

};

class U2CORE_EXPORT ReverseSequenceTask : public Task
{
    Q_OBJECT
public:
    ReverseSequenceTask( U2SequenceObject *seqObj,
                         const QList< AnnotationTableObject* > &annotations,
                         DNASequenceSelection *selection);
    ReportResult                    report( );

private:
    U2SequenceObject*               seqObj;
    QList< AnnotationTableObject* > aObjs;
    DNASequenceSelection*           selection;
};

class U2CORE_EXPORT ComplementSequenceTask : public Task
{
    Q_OBJECT
public:
    ComplementSequenceTask( U2SequenceObject *seqObj,
                            const QList< AnnotationTableObject* > &annotations,
                            DNASequenceSelection *selection,
                            DNATranslation *complTT);
    ReportResult                    report( );

private:
    U2SequenceObject*               seqObj;
    QList< AnnotationTableObject* > aObjs;
    DNASequenceSelection*           selection;
    DNATranslation*                 complTT;
};


} // namespace U2

#endif // _U2_REVERSE_SEQUENCE_TASK_H_

