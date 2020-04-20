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

#ifndef _U2_SEQUENCE_QUALITY_TRIM_WORKER_H_
#define _U2_SEQUENCE_QUALITY_TRIM_WORKER_H_

#include <U2Lang/BaseThroughWorker.h>
#include <U2Lang/WorkflowManager.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class SequenceQualityTrimPrompter;
typedef PrompterBase<SequenceQualityTrimPrompter> SequenceQualityTrimBase;

class SequenceQualityTrimPrompter : public SequenceQualityTrimBase {
    Q_OBJECT
public:
    SequenceQualityTrimPrompter(Actor *actor = NULL);

private:
    QString composeRichDoc();
};

class SequenceQualityTrimWorker : public BaseThroughWorker {
    Q_OBJECT
public:
    SequenceQualityTrimWorker(Actor *actor);

protected:
    Task *createTask(const Message &message, U2OpStatus &os);
    QList<Message> fetchResult(Task *task, U2OpStatus &os);
};

class SequenceQualityTrimWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;

public:
    SequenceQualityTrimWorkerFactory();

    static void init();
    Worker *createWorker(Actor* actor);
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_SEQUENCE_QUALITY_TRIM_WORKER_H_
