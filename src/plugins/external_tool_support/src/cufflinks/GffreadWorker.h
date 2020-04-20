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

#ifndef _U2_GFFREAD_WORKER_H_
#define _U2_GFFREAD_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "GffreadSupportTask.h"

namespace U2 {
namespace LocalWorkflow {

class GffreadWorker : public BaseWorker {
    Q_OBJECT
public:
    GffreadWorker(Actor *a);

    void init();
    Task * tick();
    void cleanup();

private slots:
    void sl_taskFinished();

private:
    bool hasInput() const;
    QString getOutUrl();
    GffreadSettings takeSettings(U2OpStatus &os);
    QVariantMap takeData(U2OpStatus &os);
    Task * runGffread(const GffreadSettings &settings);
    bool noMoreData() const;
    void finalize();
    void sendResult(const QString &outUrl);

private:
    QMap<QString, int> counters; // url <-> count suffix
};

class GffreadWorkerFactory : public DomainFactory {
public:
    GffreadWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker * createWorker(Actor *a);

    static const QString ACTOR_ID;
    static void init();
};

class GffreadPrompter : public PrompterBase<GffreadPrompter> {
    Q_OBJECT
public:
    GffreadPrompter(Actor *a = NULL) : PrompterBase<GffreadPrompter>(a) {}

protected:
    QString composeRichDoc();
};

} // LocalWorkflow
} // namespace

#endif // _U2_GFFREAD_WORKER_H_
