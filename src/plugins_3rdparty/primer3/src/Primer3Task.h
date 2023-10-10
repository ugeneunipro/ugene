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

#ifndef _PRIMER3_TASK_H_
#define _PRIMER3_TASK_H_

#include <QScopedPointer>

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

#include "FindExonRegionsTask.h"
#include "Primer3TaskSettings.h"
#include "primer3_core/libprimer3.h"

#include <QSharedPointer>

namespace U2 {

class AnnotationTableObject;
class CheckComplementTask;

class PrimerSingle {
public:
    PrimerSingle(oligo_type type = oligo_type::OT_LEFT);
    PrimerSingle(const primer_rec& primerRec, oligo_type type, int offset);

    int getStart() const;
    int getLength() const;
    double getMeltingTemperature() const;
    double getBound() const;
    double getGcContent() const;
    double getSelfAny() const;
    double getSelfEnd() const;
    double getTemplateMispriming() const;
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
    void setBound(double bound);
    void setGcContent(double gcContent);
    void setSelfAny(double selfAny);
    void setSelfEnd(double selfEnd);
    void setTemplateMispriming(double templateMispriming);
    void setHairpin(double hairpin);
    void setEndStability(double endStability);
    void setQuality(double quality);
    void setRepeatSim(double repeatSim);
    void setRepeatSimName(const QString& repeatSimName);
    void setSelfAnyStruct(const QString& selfAnyStruct);
    void setSelfEndStruct(const QString& selfEndStruct);

    /**
     * Calculates positions where, on the sequence of set length, this primer is located.
     * \param sequenceLength Length of the sequence.
     * \return Returns regions where this primer is located. If there are 2 regions,
     * primer intersects junction point.
     */
    QVector<U2Region> getSequenceRegions(int sequenceLength) const;

private:
    int start = 0;
    int length = 0;
    double meltingTemperature = 0.0;
    double bound = 0.0;
    double gcContent = 0.0;
    double selfAny = 0.0;
    double selfEnd = 0.0;
    double templateMispriming = 0.0;
    double hairpin = 0.0;
    double endStability = 0.0;
    double quality = 0.0;
    double repeatSim = 0.0;
    QString repeatSimName;
    QString selfAnyStruct;
    QString selfEndStruct;
    oligo_type type = OT_LEFT;
};

class PrimerPair {
public:
    PrimerPair() = default;
    PrimerPair(const primer_pair& primerPair, int offset = 0);
    PrimerPair(const PrimerPair& primerPair);
    PrimerPair& operator=(const PrimerPair& other);
    PrimerPair(PrimerPair&& other) noexcept = delete;
    PrimerPair& operator=(PrimerPair&& other) noexcept = delete;
    bool operator==(const PrimerPair& primerPair) const = delete;

    QSharedPointer<PrimerSingle> getLeftPrimer() const;
    QSharedPointer<PrimerSingle> getRightPrimer() const;
    QSharedPointer<PrimerSingle> getInternalOligo() const;
    double getComplAny() const;
    double getComplEnd() const;
    int getProductSize() const;
    double getProductQuality() const;
    double getProductTm() const;
    double getRepeatSim() const;
    const QString& getRepeatSimName() const;
    const QString& getComplAnyStruct() const;
    const QString& getComplEndStruct() const;

    void setLeftPrimer(const QSharedPointer<PrimerSingle>& leftPrimer);
    void setRightPrimer(const QSharedPointer<PrimerSingle>& rightPrimer);
    void setInternalOligo(const QSharedPointer<PrimerSingle>& internalOligo);
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
    QSharedPointer<PrimerSingle> leftPrimer;
    QSharedPointer<PrimerSingle> rightPrimer;
    QSharedPointer<PrimerSingle> internalOligo;
    double complAny = 0.0;
    double complEnd = 0.0;
    int productSize = 0;
    double quality = 0.0;
    double tm = 0.0;
    double repeatSim = 0.0;
    QString repeatSimName;
    QString complAnyStruct;
    QString complEndStruct;
};

class Primer3Task : public Task {
    Q_OBJECT
public:
    Primer3Task(const QSharedPointer<Primer3TaskSettings>& settings);

    void prepare() override;
    void run() override;
    Task::ReportResult report() override;

    const QList<QSharedPointer<PrimerPair>>& getBestPairs() const {
        return bestPairs;
    }
    const QList<QSharedPointer<PrimerSingle>>& getSinglePrimers() const {
        return singlePrimers;
    }

private:
    void selectPairsSpanningExonJunction(p3retval* primers, int toReturn);
    void selectPairsSpanningIntron(p3retval* primers, int toReturn);

    QSharedPointer<Primer3TaskSettings> settings;
    QList<QSharedPointer<PrimerPair>> bestPairs;
    QList<QSharedPointer<PrimerSingle>> singlePrimers;

    int offset = 0;
};

class Primer3ToAnnotationsTask : public Task {
    Q_OBJECT
public:
    Primer3ToAnnotationsTask(const QSharedPointer<Primer3TaskSettings>& settings,
                             U2SequenceObject* seqObj_,
                             AnnotationTableObject* aobj_,
                             const QString& groupName_,
                             const QString& annName_,
                             const QString& annDescription);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask)  override;

    QString generateReport() const override;

private:
    void findExonTaskIsfinished();
    QMap<QString, QList<SharedAnnotationData>> getResultAnnotations() const;
    SharedAnnotationData oligoToAnnotation(const QString& title, const QSharedPointer<PrimerSingle>& primer, int productSize, U2Strand strand) const;

    QSharedPointer<Primer3TaskSettings> settings;
    QPointer<AnnotationTableObject> annotationTableObject;
    U2SequenceObject* seqObj;
    QString groupName;
    QString annName;
    const QString annDescription;

    Primer3Task* primer3Task = nullptr;
    FindExonRegionsTask* findExonsTask = nullptr;
    CheckComplementTask* checkComplementTask = nullptr;
};

}  // namespace U2

#endif  //_PRIMER3_TASK_H_
