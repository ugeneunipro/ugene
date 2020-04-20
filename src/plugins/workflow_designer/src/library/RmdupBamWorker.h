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

#ifndef _U2_RMDUP_BAM_WORKER_H_
#define _U2_RMDUP_BAM_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

class RmdupBamPrompter;
typedef PrompterBase<RmdupBamPrompter> RmdupBamBase;

class RmdupBamPrompter : public RmdupBamBase {
    Q_OBJECT
public:
    RmdupBamPrompter(Actor* p = 0) : RmdupBamBase(p) {}
protected:
    QString composeRichDoc();
}; //RmdupBamPrompter

class RmdupBamWorker: public BaseWorker {
    Q_OBJECT
public:
    RmdupBamWorker(Actor *a);
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
}; //RmdupBamWorker

class RmdupBamWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    RmdupBamWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new RmdupBamWorker(a); }
}; //RmdupBamWorkerFactory

class BamRmdupSetting{
public:
    BamRmdupSetting(): outDir(""), outName(""),inputUrl(""), removeSingleEnd(false), treatReads(false){}

    QString outDir;
    QString outName;
    QString inputUrl;
    bool    removeSingleEnd;
    bool    treatReads;

    QStringList getSamtoolsArguments() const;
};

class SamtoolsRmdupTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    SamtoolsRmdupTask(const BamRmdupSetting &settings);

    void prepare();
    void run();

    QString getResult(){return resultUrl;}

private:
    void start(const ProcessRun &pRun, const QString &toolName);
    void checkExitCode(QProcess *process, const QString &toolName);

private:
    BamRmdupSetting settings;
    QString resultUrl;

    static const QString SAMTOOLS_ID;       // this constant is taken from the appropriate file in external_tool_support plugin
};


} //LocalWorkflow
} //U2

#endif //_U2_REMDUP_BAM_WORKER_H_
