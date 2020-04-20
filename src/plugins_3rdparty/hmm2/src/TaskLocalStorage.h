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

#ifndef _TASK_LOCAL_CONTEXT_
#define _TASK_LOCAL_CONTEXT_

#include <hmmer2/funcs.h>

#include <QThreadStorage>
#include <QHash>
#include <QMutex>

namespace U2 {

// this struct is stored in TLS
struct ContextIdContainer {
    ContextIdContainer(qint64 id) : contextId(id){}
    qint64 contextId;
};


class TaskLocalData {
public:
    static HMMERTaskLocalData* current();

    // initializes HMMContext for current thread
    static HMMERTaskLocalData* createHMMContext(qint64 contextId, bool bindThreadToContext);

    static void freeHMMContext(qint64 contextId);



    // binds to existing HMMContext 
    static void bindToHMMContext(qint64 contextId);

    static qint64 detachFromHMMContext();


private:
    static QHash<qint64, struct HMMERTaskLocalData*> data;
    static QThreadStorage<ContextIdContainer*> tls;
    static QMutex mutex;
};
} //namespace

#endif
