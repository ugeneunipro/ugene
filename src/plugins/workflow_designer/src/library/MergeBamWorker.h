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

#ifndef _U2_MEGER_BAM_WORKER_H_
#define _U2_MEGER_BAM_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

class MergeBamPrompter;
typedef PrompterBase<MergeBamPrompter> MergeBamBase;

class MergeBamPrompter : public MergeBamBase {
    Q_OBJECT
public:
    MergeBamPrompter(Actor* p = 0) : MergeBamBase(p) {}
protected:
    QString composeRichDoc();
}; //MergeBamPrompter

class MergeBamWorker: public BaseWorker {
    Q_OBJECT
public:
    MergeBamWorker(Actor *a);
    void init();
    Task * tick();
    void cleanup();

private:
    IntegralBus *inputUrlPort;
    IntegralBus *outputUrlPort;
    QString outputDir;
    QStringList urls;
public slots:
    void sl_taskFinished( Task *task );

private:
    QString takeUrl();
    QString getOutputName(const QString& fileUrl);
    void sendResult(const QString &url);
}; //MergeBamWorker

class MergeBamWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    MergeBamWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new MergeBamWorker(a); }
}; //MergeBamWorkerFactory

} //LocalWorkflow
} //U2

#endif //_U2_MEGER_BAM_WORKER_H_
