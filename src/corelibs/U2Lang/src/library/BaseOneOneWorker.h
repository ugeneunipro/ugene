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

#ifndef _U2_BASE_ONE_ONE_WORKER_H_
#define _U2_BASE_ONE_ONE_WORKER_H_

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

/**
 * The base class for the workers with one input and one output port.
 */
class U2LANG_EXPORT BaseOneOneWorker : public BaseWorker {
    Q_OBJECT
public:
    BaseOneOneWorker(Actor *a, bool autoTransitBus, const QString &inPortId, const QString &outPortId);

    void init();
    Task * tick();

protected:
    virtual Task * createPrepareTask(U2OpStatus &os) const;
    virtual void onPrepared(Task *task, U2OpStatus &os);
    virtual Task * processNextInputMessage() = 0;
    virtual Task * onInputEnded() = 0;
    virtual QList<Message> fetchResult(Task *task, U2OpStatus &os) = 0;

private slots:
    void sl_taskFinished();
    void sl_prepared();

private:
    Task * prepare(U2OpStatus &os);

protected:
    const QString inPortId;
    const QString outPortId;
    IntegralBus *input;
    IntegralBus *output;

    bool prepared;
};

} // LocalWorkflow
} // U2

#endif // _U2_BASE_ONE_ONE_WORKER_H_
