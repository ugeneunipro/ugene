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

#ifndef _U2_SEQUENCE_CONTENT_FILTER_TASK_H_
#define _U2_SEQUENCE_CONTENT_FILTER_TASK_H_

#include <U2Core/AbstractProjectFilterTask.h>

#include <U2Algorithm/FindAlgorithm.h>

namespace U2 {

class U2SequenceObject;

//////////////////////////////////////////////////////////////////////////
/// SequenceConentFilterTask
//////////////////////////////////////////////////////////////////////////

class SequenceContentFilterTask : public AbstractProjectFilterTask, public FindAlgorithmResultsListener {
    Q_OBJECT
public:
    SequenceContentFilterTask(const ProjectTreeControllerModeSettings &settings, const QList<QPointer<Document> > &docs);

    void onResult(const FindAlgorithmResult &r);

protected:
    bool filterAcceptsObject(GObject *obj);

private:
    bool sequenceContainsPattern(U2SequenceObject *seqObject, const QString &pattern, const FindAlgorithmSettings &findSettings);
    void searchThroughRegion(U2SequenceObject *seqObject, const U2Region &searchRegion, const QString &pattern,
        const FindAlgorithmSettings &findSettings);

    static bool patternFitsSequenceAlphabet(U2SequenceObject *seqObject, const QString &pattern);
    static bool initFindAlgorithmSettings(U2SequenceObject *seqObject, FindAlgorithmSettings &findSettings);

    int searchStopFlag;

    static const qint64 SEQUENCE_CHUNK_SIZE;
};

//////////////////////////////////////////////////////////////////////////
/// SequenceConentFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

class U2ALGORITHM_EXPORT SequenceContentFilterTaskFactory : public ProjectFilterTaskFactory {
protected:
    AbstractProjectFilterTask * createNewTask(const ProjectTreeControllerModeSettings &settings,
        const QList<QPointer<Document> > &docs) const;
};

} // namespace U2

#endif // _U2_SEQUENCE_CONTENT_FILTER_TASK_H_
