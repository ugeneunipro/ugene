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

#ifndef _U2_TRIMMOMATIC_WORKER_FACTORY_H_
#define _U2_TRIMMOMATIC_WORKER_FACTORY_H_

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class TrimmomaticWorkerFactory : public DomainFactory {
public:
    TrimmomaticWorkerFactory();

    Worker *createWorker(Actor *actor);

    static void init();
    static void cleanup();

    static const QString ACTOR_ID;

    static const QString INPUT_PORT_ID;
    static const QString OUTPUT_PORT_ID;

    static const QString INPUT_SLOT;
    static const QString PAIRED_INPUT_SLOT;

    static const QString OUT_SLOT;
    static const QString PAIRED_OUT_SLOT;

    static const QString INPUT_DATA_ATTR_ID;
    static const QString TRIMMING_STEPS_ATTR_ID;
    static const QString OUTPUT_URL_ATTR_ID;
    static const QString PAIRED_URL_1_ATTR_ID;
    static const QString PAIRED_URL_2_ATTR_ID;
    static const QString UNPAIRED_URL_1_ATTR_ID;
    static const QString UNPAIRED_URL_2_ATTR_ID;
    static const QString GENERATE_LOG_ATTR_ID;
    static const QString LOG_URL_ATTR_ID;
    static const QString THREADS_NUMBER_ATTR_ID;

    static const QString SINGLE_END_TEXT;
    static const QString PAIRED_END_TEXT;
};

} // namespace LocalWorkflow
} // namespace U2

#endif // _U2_TRIMMOMATIC_WORKER_FACTORY_H_
