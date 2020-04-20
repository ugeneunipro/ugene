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

#ifndef _U2_HMMSEARCH_TASK_H_
#define _U2_HMMSEARCH_TASK_H_

#include "uhmmsearch.h"
#include "HMMIO.h"
#include <QMutex>
#include <U2Core/U2Region.h>
#include <U2Core/Task.h>
#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/SequenceWalkerTask.h>

struct plan7_s;

namespace U2 {

class DNATranslation;

class HMMSearchTaskResult {
public:
    HMMSearchTaskResult() : evalue(0), score(0), onCompl(false), onAmino(false), borderResult(false), filtered(false){} 
    float   evalue;
    float   score;
    bool    onCompl;
    bool    onAmino;
    bool    borderResult;
    bool    filtered;
    U2Region r;

};


class HMMSearchTask: public Task, SequenceWalkerCallback {
    Q_OBJECT
public:
    HMMSearchTask(plan7_s* hmm, const DNASequence& seq, const UHMMSearchSettings& s);

    HMMSearchTask(const QString& hFile, const DNASequence& seq, const UHMMSearchSettings& s);

    virtual void prepare();
    
    const QList<HMMSearchTaskResult>& getResults() const {return results;}

    virtual void onRegion(SequenceWalkerSubtask* t, TaskStateInfo& stateInfo);

    Task::ReportResult report();

    QList<SharedAnnotationData> getResultsAsAnnotations(U2FeatureType type, const QString &name) const;

    QList<Task *> onSubTaskFinished(Task *subTask);

private:
    bool checkAlphabets(int hmmAl, const DNAAlphabet* seqAl, DNATranslation*& complTrans, DNATranslation*& aminoTrans);

    SequenceWalkerTask* getSWSubtask();
private:
    plan7_s*                            hmm;
    DNASequence                         seq;
    UHMMSearchSettings                  settings;
    DNATranslation*                     complTrans;
    DNATranslation*                     aminoTrans;
    QList<HMMSearchTaskResult>          results;
    QList<HMMSearchTaskResult>          overlaps;
    QString                             fName;
    QMutex                              lock;
    HMMReadTask*                        readHMMTask;
    SequenceWalkerTask*                 swTask;
};



}//namespace
#endif
