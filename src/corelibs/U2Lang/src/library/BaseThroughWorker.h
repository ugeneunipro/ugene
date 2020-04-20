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

#ifndef _U2_BASE_THROUGH_WORKER_H_
#define _U2_BASE_THROUGH_WORKER_H_

#include <U2Lang/BaseOneOneWorker.h>

namespace U2 {
namespace LocalWorkflow {

/**
 * The base class for simple workers:
 * 1) one input and one output port;
 * 2) no grouping by datasets.
 */
class U2LANG_EXPORT BaseThroughWorker : public BaseOneOneWorker {
public:
    BaseThroughWorker(Actor *a, const QString &inPortId, const QString &outPortId);

    void cleanup();

protected:
    // BaseOneOneWorker
    Task * processNextInputMessage();
    Task * onInputEnded();
    Message composeMessage(const QVariantMap &data);

    virtual Task * createTask(const Message &message, U2OpStatus &os) = 0;
};

} // LocalWorkflow
} // U2

#endif // _U2_BASE_THROUGH_WORKER_H_
