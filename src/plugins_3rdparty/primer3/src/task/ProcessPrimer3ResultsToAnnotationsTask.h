/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

#include <QSharedPointer>

namespace U2 {

class AnnotationTableObject;
class CheckComplementTask;
class FindExonRegionsTask;
class Primer3Task;
class Primer3TaskSettings;
class U2SequenceObject;
struct PrimerSingle;
struct PrimerPair;

/**
 * This class represents a task, which process results, received by Primer3, to UGENE annotations.
 */
class ProcessPrimer3ResultsToAnnotationsTask : public Task {
    Q_OBJECT
public:
    /**
    * This class represents a task, which process results, received by Primer3, to UGENE annotations.
    * \param settings Primer3 settings.
    * \param bestPairs primer pairs Primer3 has calculated.
    * \param filteredPairs primer pairs have been filtered by CheckComplementTask.
    * \param singlePrimers single primers Primer3 has calculated.
    * \param groupName name of annotation group.
    * \param annName name of annotation.
    * \param annDescription description of annotation.
    * \param sequenceLength sequence length.
    * \param pairNumberOffset Annotation group with result primer pair has a number.
    * For example, if one primer have been found, we will have the following
    * result group name - "pair 1". But, if @pairNumberOffset is more than, for example, 10,
    * the group name will be "pair 11".
    */
    ProcessPrimer3ResultsToAnnotationsTask(const QSharedPointer<Primer3TaskSettings>& settings,
                                           const QList<QSharedPointer<PrimerPair>>& bestPairs,
                                           const QList<QSharedPointer<PrimerPair>>& filteredPairs,
                                           const QList<QSharedPointer<PrimerSingle>>& singlePrimers,
                                           const QString& groupName,
                                           const QString& annName,
                                           const QString& annDescription,
                                           qint64 sequenceLength,
                                           int pairNumberOffset);

    void run() override;

    const QMap<QString, QList<SharedAnnotationData>>& getResultAnnotations() const;

private:
    SharedAnnotationData oligoToAnnotation(const QString& title, const QSharedPointer<PrimerSingle>& primer, int productSize, U2Strand strand) const;

    QSharedPointer<Primer3TaskSettings> settings;
    QList<QSharedPointer<PrimerPair>> bestPairs;
    QList<QSharedPointer<PrimerPair>> filteredPairs;
    QList<QSharedPointer<PrimerSingle>> singlePrimers;
    QString groupName;
    QString annName;
    QString annDescription;
    qint64 sequenceLength;
    int pairNumberOffset = 0;

    QMap<QString, QList<SharedAnnotationData>> resultAnnotations;
};


}
