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

#ifndef _U2_KRAKEN_BUILD_WORKER_FACTORY_H_
#define _U2_KRAKEN_BUILD_WORKER_FACTORY_H_

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class KrakenBuildWorkerFactory : public DomainFactory {
public:
    KrakenBuildWorkerFactory();

    Worker *createWorker(Actor *actor);

    static void init();
    static void cleanup();

    static const QString ACTOR_ID;

    static const QString OUTPUT_PORT_ID;

    static const QString MODE_ATTR_ID;
    static const QString INPUT_DATABASE_NAME_ATTR_ID;
    static const QString NEW_DATABASE_NAME_ATTR_ID;
    static const QString GENOMIC_LIBRARY_ATTR_ID;
    static const QString NUMBER_OF_K_MERS_ATTR_ID;
    static const QString K_MER_LENGTH_ATTR_ID;
    static const QString MINIMIZER_LENGTH_ATTR_ID;
    static const QString MAXIMUM_DATABASE_SIZE_ATTR_ID;
    static const QString SHRINK_BLOCK_OFFSET_ATTR_ID;
    static const QString CLEAN_ATTR_ID;
    static const QString WORK_ON_DISK_ATTR_ID;
    static const QString JELLYFISH_HASH_SIZE_ATTR_ID;
    static const QString THREADS_NUMBER_ATTR_ID;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_KRAKEN_BUILD_WORKER_FACTORY_H_
