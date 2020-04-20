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

#ifndef _U2_CLARK_BUILD_WORKER_H_
#define _U2_CLARK_BUILD_WORKER_H_

#include <QCoreApplication>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/BaseNGSWorker.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

//////////////////////////////////////////////////
//ClarkBuildValidator

class ClarkBuildValidator : public ActorValidator {
    Q_DECLARE_TR_FUNCTIONS(ClarkBuildValidator)
public:
    bool validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const;

private:
    bool validateTaxonomy(const Actor *actor, NotificationsList &notificationList) const;
};

//////////////////////////////////////////////////
//ClarkBuild
class ClarkBuildPrompter;
typedef PrompterBase<ClarkBuildPrompter> ClarkBuildBase;

class ClarkBuildPrompter : public ClarkBuildBase {
    Q_OBJECT
public:
    ClarkBuildPrompter(Actor* p = 0) : ClarkBuildBase(p) {}
protected:
    QString composeRichDoc();
}; //ClarkBuildPrompter

class ClarkBuildWorker: public BaseWorker {
    Q_OBJECT
public:
    ClarkBuildWorker(Actor *a);
protected:
    void init();
    Task * tick();
    void cleanup() {}

private slots:
    void sl_taskFinished(Task *task);

protected:
    IntegralBus *output;

}; //ClarkBuildWorker

class ClarkBuildWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;
public:
    static void init();
    static void cleanup();
    ClarkBuildWorkerFactory() : DomainFactory(ACTOR_ID) {}
    Worker* createWorker(Actor* a) { return new ClarkBuildWorker(a); }
}; //ClarkBuildWorkerFactory

class ClarkBuildTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    ClarkBuildTask(const QString &dbUrl, const QStringList &genomeUrls, int rank, const QString &taxdataUrl);

    const QString &getDbUrl() const {return dbUrl;}

private:
    void prepare();

    QStringList getArguments();

    const QString dbUrl;
    const QString taxdataUrl;
    const QStringList genomeUrls;
    int rank;
};

} //LocalWorkflow
} //U2

#endif //_U2_CLARK_BUILD_WORKER_H_
