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

#ifndef _U2_ORF_MARKER_TASK_H_
#define _U2_ORF_MARKER_TASK_H_

#include <QPointer>

#include <U2Algorithm/ORFAlgorithmTask.h>
#include <U2Core/AutoAnnotationsSupport.h>

namespace U2 {

class AnnotationTableObject;
class Settings;
class DNASequence;

struct ORFSettingsKeys {
public:
    static const QString STRAND;
    static const QString AMINO_TRANSL;
    static const QString SEARCH_REGION;
    static const QString MIN_LEN;
    static const QString MUST_FIT;
    static const QString MUST_INIT;
    static const QString ALLOW_ALT_START;
    static const QString ALLOW_OVERLAP;
    static const QString INCLUDE_STOP_CODON;
    static const QString CIRCULAR_SEARCH;
    static const QString MAX_RESULT;
    static const QString IS_RESULT_LIMITED;

    static void save(const ORFAlgorithmSettings& cfg, Settings* st);
    static void read(ORFAlgorithmSettings& cfg, const Settings* st);
};

class FindORFsToAnnotationsTask :      public Task
{
    Q_OBJECT
public:
                                        FindORFsToAnnotationsTask(AnnotationTableObject *aobj, const U2EntityRef &entityRef,
                                            const ORFAlgorithmSettings &settings, const QString &groupName = QString(), const QString &annDescription = "");

    QList<Task *>                       onSubTaskFinished(Task *subTask);

private:
    QPointer<AnnotationTableObject>     aObj;
    ORFFindTask *                       fTask;
    ORFAlgorithmSettings                cfg;
    QString                             groupName;
    const QString                       annDescription;
    U2EntityRef                         entityRef;
};

class ORFAutoAnnotationsUpdater : public AutoAnnotationsUpdater {
    Q_OBJECT
public:
     ORFAutoAnnotationsUpdater();
     Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* aa);
     bool checkConstraints(const AutoAnnotationConstraints& constraints);
};

} //namespace

#endif // _U2_ORF_MARKER_TASK_

