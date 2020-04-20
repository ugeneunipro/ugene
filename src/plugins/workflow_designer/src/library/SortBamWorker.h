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

#ifndef _U2_SORT_BAM_WORKER_H_
#define _U2_SORT_BAM_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

class SortBamPrompter;
typedef PrompterBase<SortBamPrompter> SortBamBase;

class SortBamPrompter : public SortBamBase {
    Q_OBJECT
public:
    SortBamPrompter(Actor* p = 0) : SortBamBase(p) {}
protected:
    QString composeRichDoc();
}; //SortBamPrompter

class SortBamWorker: public BaseWorker {
    Q_OBJECT
public:
    SortBamWorker(Actor *a);
    void init();
    Task * tick();
    void cleanup();

private:
    IntegralBus *inputUrlPort;
    IntegralBus *outputUrlPort;
    QStringList outUrls;
public slots:
    void sl_taskFinished( Task *task );

private:
    QString takeUrl();
    QString getTargetName(const QString& fileUrl, const QString& outDir);
    void sendResult(const QString &url);
}; //SortBamWorker

class SortBamWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    SortBamWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new SortBamWorker(a); }
}; //SortBamWorkerFactory

class BamSortSetting{
public:
    BamSortSetting(): outDir(""), outName(""),inputUrl(""), index(true){}

    QString outDir;
    QString outName;
    QString inputUrl;
    bool    index;
};

class SamtoolsSortTask : public Task {
    Q_OBJECT
public:
    SamtoolsSortTask(const BamSortSetting &settings);

    void prepare();
    void run();

    QString getResult(){return resultUrl;}

private:
    BamSortSetting settings;
    QString resultUrl;
};


} //LocalWorkflow
} //U2

#endif //_U2_SORT_BAM_WORKER_H_
