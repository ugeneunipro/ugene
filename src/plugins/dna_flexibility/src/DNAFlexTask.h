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

#ifndef _U2_DNA_FLEX_TASK_H_
#define _U2_DNA_FLEX_TASK_H_


#include "FindHighFlexRegions.h"
#include "HighFlexSettings.h"

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequence.h>
#include <U2Core/Task.h>

#include <QPointer>

namespace U2 {

class AnnotationTableObject;
class DNASequence;

/**
 * The main DNA Flexibility task: launches a task to search for high
 * flexibility regions (FindHighFlexRegions) and saves the annotations.
 */
class DNAFlexTask : public Task
{
    Q_OBJECT
public:
    DNAFlexTask(const HighFlexSettings& settings,
        AnnotationTableObject *annotObject,
        const QString& annotName,
        const QString& annotGroup,
        const QString &annDescription,
        const DNASequence& sequence);

    QList<Task*> onSubTaskFinished(Task* subTask);
    QList<SharedAnnotationData> getAnnotationsFromResults(const QList<HighFlexResult>& results);

private:
    HighFlexSettings                    settings;
    QPointer<AnnotationTableObject>     annotObject;
    U2FeatureType                       annotType;
    QString                             annotName;
    QString                             annotGroup;
    const QString                       annDescription;
    DNASequence                         sequence;
    FindHighFlexRegions*                findHighFlexTask;
};


} // namespace

#endif
