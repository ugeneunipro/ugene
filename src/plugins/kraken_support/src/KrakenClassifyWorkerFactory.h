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

#ifndef _U2_KRAKEN_CLASSIFY_WORKER_FACTORY_H_
#define _U2_KRAKEN_CLASSIFY_WORKER_FACTORY_H_

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class KrakenClassifyWorkerFactory : public DomainFactory {

public:
    KrakenClassifyWorkerFactory();

    Worker *createWorker(Actor *actor);

    static void init();
    static void cleanup();

    static const QString ACTOR_ID;

    static const QString INPUT_PORT_ID;
    static const QString OUTPUT_PORT_ID;

    static const QString INPUT_SLOT;
    static const QString PAIRED_INPUT_SLOT;

    static const QString INPUT_DATA_ATTR_ID;
    static const QString DATABASE_ATTR_ID;
    static const QString OUTPUT_URL_ATTR_ID;
    static const QString QUICK_OPERATION_ATTR_ID;
    static const QString MIN_HITS_NUMBER_ATTR_ID;
    static const QString THREADS_NUMBER_ATTR_ID;
    static const QString PRELOAD_DATABASE_ATTR_ID;

    static const QString SINGLE_END_TEXT;
    static const QString PAIRED_END_TEXT;

    static const QString WORKFLOW_CLASSIFY_TOOL_KRAKEN;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_KRAKEN_CLASSIFY_WORKER_FACTORY_H_
