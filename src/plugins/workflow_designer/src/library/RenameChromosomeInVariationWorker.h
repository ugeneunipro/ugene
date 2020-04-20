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

#ifndef _U2_RENAME_CHOMOSOME_IN_VARIATION_WORKER_H_
#define _U2_RENAME_CHOMOSOME_IN_VARIATION_WORKER_H_

#include <U2Lang/BaseThroughWorker.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class RenameChomosomeInVariationPrompter;
typedef PrompterBase<RenameChomosomeInVariationPrompter> RenameChomosomeInVariationBase;

class RenameChomosomeInVariationPrompter : public RenameChomosomeInVariationBase {
    Q_OBJECT
public:
    RenameChomosomeInVariationPrompter(Actor *actor = NULL);

private:
    QString composeRichDoc();
};

class RenameChomosomeInVariationWorker : public BaseThroughWorker {
    Q_OBJECT
public:
    RenameChomosomeInVariationWorker(Actor *actor);

private:
    QList<Message> fetchResult(Task *task, U2OpStatus &os);
    Task * createTask(const Message &message, U2OpStatus &os);

    IntegralBus *inputUrlPort;
    IntegralBus *outputUrlPort;
};

class RenameChomosomeInVariationWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    RenameChomosomeInVariationWorkerFactory();
    static void init();
    Worker *createWorker(Actor *actor);

};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_RENAME_CHOMOSOME_IN_VARIATION_WORKER_H_
