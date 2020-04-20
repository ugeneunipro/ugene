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

#ifndef _U2_FILTER_BAM_WORKER_H_
#define _U2_FILTER_BAM_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/GUrl.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {
namespace LocalWorkflow {

class FilterBamPrompter;
typedef PrompterBase<FilterBamPrompter> FilterBamBase;

class FilterBamPrompter : public FilterBamBase {
    Q_OBJECT
public:
    FilterBamPrompter(Actor* p = 0) : FilterBamBase(p) {}
protected:
    QString composeRichDoc();
}; //FilterBamPrompter

class FilterBamWorker: public BaseWorker {
    Q_OBJECT
public:
    FilterBamWorker(Actor *a);
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
    void sendResult(const QString &url);
    QString getTargetName(const QString& fileUrl, const QString& outDir);

}; //FilterBamWorker

class FilterBamWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    FilterBamWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new FilterBamWorker(a); }
}; //FilterBamWorkerFactory

class BamFilterSetting{
public:
    BamFilterSetting(): outDir(""), outName(""),inputUrl(""), inputFormat(""), outputFormat(""),mapq(0), acceptFilter(""), skipFilter(""),regionFilter(""){}

    QString outDir;
    QString outName;
    QString inputUrl;
    QString inputFormat;
    QString outputFormat;
    int     mapq;
    QString acceptFilter;
    QString skipFilter;
    QString regionFilter;

    QStringList getSamtoolsArguments() const;
};

class SamtoolsViewFilterTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    SamtoolsViewFilterTask(const BamFilterSetting &settings);

    void prepare();
    void run();

    QString getResult(){return resultUrl;}

private:
    void start(const ProcessRun &pRun, const QString &toolName);
    void checkExitCode(QProcess *process, const QString &toolName);

private:
    BamFilterSetting settings;
    QString resultUrl;

   static const QString SAMTOOLS_ID;
};

} //LocalWorkflow
} //U2

#endif //_U2_FILTER_BAM_WORKER_H_
