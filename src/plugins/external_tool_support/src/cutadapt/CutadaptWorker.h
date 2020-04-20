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

#ifndef _U2_CUTADAPT_WORKER_H_
#define _U2_CUTADAPT_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/BaseNGSWorker.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

//////////////////////////////////////////////////
//CutAdaptFastq
class CutAdaptFastqPrompter;
typedef PrompterBase<CutAdaptFastqPrompter> CutAdaptFastqBase;

class CutAdaptFastqPrompter : public CutAdaptFastqBase {
    Q_OBJECT
public:
    CutAdaptFastqPrompter(Actor* p = 0) : CutAdaptFastqBase(p) {}
protected:
    QString composeRichDoc();
}; //CutAdaptFastqPrompter

class CutAdaptFastqWorker: public BaseNGSWorker {
    Q_OBJECT
public:
    CutAdaptFastqWorker(Actor *a);
protected:
    QVariantMap getCustomParameters() const;
    QString getDefaultFileName() const;
    Task *getTask(const BaseNGSSetting &settings) const;

    QStringList inputUrls;

}; //CutAdaptFastqWorker

class CutAdaptFastqWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    CutAdaptFastqWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new CutAdaptFastqWorker(a); }
}; //CutAdaptFastqWorkerFactory

class CutAdaptFastqTask : public BaseNGSTask {
    Q_OBJECT
public:
    CutAdaptFastqTask (const BaseNGSSetting &settings);
protected:
    void prepareStep();
    QStringList getParameters(U2OpStatus& os);
};

class CutAdaptParser : public ExternalToolLogParser {
public:
    void parseErrOutput(const QString& partOfLog);

    static QString parseTextForErrors(const QStringList &lastPartOfLog);

private:
    static QStringList initStringsToIgnore();

    QString lastErrLine;

    static const QStringList stringsToIgnore;
};

class CutAdaptLogProcessor : public ExternalToolLogProcessor {
public:
    CutAdaptLogProcessor(WorkflowMonitor *monitor, const QString &actor);

    void processLogMessage(const QString &message);

private:
    WorkflowMonitor *monitor;
    const QString actor;
};

} //LocalWorkflow
} //U2

#endif //_U2_CUTADAPT_WORKER_H_
