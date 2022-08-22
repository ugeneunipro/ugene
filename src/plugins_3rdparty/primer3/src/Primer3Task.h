/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _PRIMER3_TASK_H_
#define _PRIMER3_TASK_H_

#include <QScopedPointer>

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

#include "FindExonRegionsTask.h"
#include "Primer3TaskSettings.h"
#include "primer3_core/libprimer3.h"


namespace U2 {

class AnnotationTableObject;

class PrimerSingle {
public:
    PrimerSingle(oligo_type type = oligo_type::OT_LEFT);
    PrimerSingle(const primer_rec& primerRec, oligo_type type, int offset);
    bool operator==(const PrimerSingle& primer) const;
    static bool areEqual(const PrimerSingle* p1, const PrimerSingle* p2);

    int getStart() const;
    int getLength() const;
    double getMeltingTemperature() const;
    double getGcContent() const;
    double getSelfAny() const;
    double getSelfEnd() const;
    double getHairpin() const;
    double getEndStability() const;
    double getQuality() const;
    double getRepeatSim() const;
    const QString& getRepeatSimName() const;
    const QString& getSelfAnyStruct() const;
    const QString& getSelfEndStruct() const;
    oligo_type getType() const;

    void setStart(int start);
    void setLength(int length);
    void setMeltingTemperature(double meltingTemperature);
    void setGcContent(double gcContent);
    void setSelfAny(double selfAny);
    void setSelfEnd(double selfEnd);
    void setHairpin(double hairpin);
    void setEndStability(double endStability);
    void setQuality(double quality);
    void setRepeatSim(double repeatSim);
    void setRepeatSimName(const QString& repeatSimName);
    void setSelfAnyStruct(const QString& selfAnyStruct);
    void setSelfEndStruct(const QString& selfEndStruct);

private:
    int start;
    int length;
    double meltingTemperature;
    double gcContent;
    double selfAny;
    double selfEnd;
    double hairpin;
    double endStability;
    double quality;
    double repeatSim;
    QString repeatSimName;
    QString selfAnyStruct;
    QString selfEndStruct;
    oligo_type type;
};

class PrimerPair {
public:
    PrimerPair();
    PrimerPair(const primer_pair& primerPair, int offset = 0);
    PrimerPair(const PrimerPair& primerPair);
    PrimerPair& operator=(const PrimerPair& primerPair);
    bool operator==(const PrimerPair& primerPair) const;

    PrimerSingle* getLeftPrimer() const;
    PrimerSingle* getRightPrimer() const;
    PrimerSingle* getInternalOligo() const;
    double getComplAny() const;
    double getComplEnd() const;
    int getProductSize() const;
    double getProductQuality() const;
    double getProductTm() const;
    double getRepeatSim() const;
    const QString& getRepeatSimName() const;
    const QString& getComplAnyStruct() const;
    const QString& getComplEndStruct() const;

    void setLeftPrimer(PrimerSingle* leftPrimer);
    void setRightPrimer(PrimerSingle* rightPrimer);
    void setInternalOligo(PrimerSingle* internalOligo);
    void setComplAny(double complAny);
    void setComplEnd(double complEnd);
    void setProductSize(int productSize);
    void setProductQuality(double quality);
    void setProductTm(double tm);
    void setRepeatSim(double repeatSim);
    void setRepeatSimName(const QString& repeatSimName);
    void setComplAnyStruct(const QString& complAnyStruct);
    void setComplEndStruct(const QString& complEndStruct);

    bool operator<(const PrimerPair& pair) const;

private:
    // don't forget to change copy constructor and assignment operator when changing this!
    QScopedPointer<PrimerSingle> leftPrimer;
    QScopedPointer<PrimerSingle> rightPrimer;
    QScopedPointer<PrimerSingle> internalOligo;
    double complAny;
    double complEnd;
    int productSize;
    double quality;
    double tm;
    double repeatSim;
    QString repeatSimName;
    QString complAnyStruct;
    QString complEndStruct;
};

class Primer3Task : public Task {
    Q_OBJECT
public:
    Primer3Task(Primer3TaskSettings* settings);

    void run();
    Task::ReportResult report();
    void sumStat(Primer3TaskSettings* st);
    void selectPairsSpanningExonJunction(p3retval* primers, int toReturn);
    void selectPairsSpanningIntron(p3retval* primers, int toReturn);

    const QList<PrimerPair>& getBestPairs() const {
        return bestPairs;
    }
    const QList<PrimerSingle>& getSinglePrimers() const {
        return singlePrimers;
    }

private:
    Primer3TaskSettings* settings;
    QList<PrimerPair> bestPairs;
    QList<PrimerSingle> singlePrimers;

    p3retval* resultPrimers = nullptr;

    int offset = 0;
};

class Primer3ToAnnotationsTask;

class Primer3SWTask : public Task {
    Q_OBJECT
public:
    Primer3SWTask(Primer3TaskSettings* settings);

    void prepare();
    Task::ReportResult report();

    const QList<PrimerPair>& getBestPairs() const {
        return bestPairs;
    }
    QList<PrimerSingle> getSinglePrimers() const {
        return singlePrimers;
    }

private:
    void relocatePrimerOverMedian(PrimerSingle* primer);

    static const int CHUNK_SIZE = 1024 * 256;

    QList<Primer3Task*> regionTasks;
    QList<Primer3Task*> circRegionTasks;
    int median;
    Primer3TaskSettings* settings;
    QList<PrimerPair> bestPairs;
    QList<PrimerSingle> singlePrimers;

    friend class Primer3ToAnnotationsTask;
};

class Primer3ToAnnotationsTask : public Task {
    Q_OBJECT
public:
    Primer3ToAnnotationsTask(Primer3TaskSettings* settings,
                             U2SequenceObject* seqObj_,
                             AnnotationTableObject* aobj_,
                             const QString& groupName_,
                             const QString& annName_,
                             const QString& annDescription);
    ~Primer3ToAnnotationsTask();

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    virtual QString generateReport() const;
    Task::ReportResult report();

private:
    SharedAnnotationData oligoToAnnotation(const QString& title, const PrimerSingle& primer, int productSize, U2Strand strand);

    Primer3TaskSettings* settings;
    QPointer<AnnotationTableObject> annotationTableObject;
    U2SequenceObject* seqObj;
    QString groupName;
    QString annName;
    const QString annDescription;

    Primer3SWTask* searchTask;
    FindExonRegionsTask* findExonsTask;
};

}  // namespace U2

#endif  //_PRIMER3_TASK_H_
