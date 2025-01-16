/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/CmdlineTaskRunner.h>

#include <U2Lang/WorkflowManager.h>

namespace U2 {

namespace Workflow {
class CommunicationChannel;
class Schema;
class WorkflowMonitor;
}  // namespace Workflow
using namespace Workflow;

class U2LANG_EXPORT WorkflowAbstractRunner : public CmdlineTask {
    Q_OBJECT
public:
    WorkflowAbstractRunner(const QString& name, TaskFlags flags);
    virtual QList<WorkerState> getState(Actor*) = 0;
    virtual int getMsgNum(const Link*) = 0;
    virtual int getMsgPassed(const Link*) = 0;

    const QList<WorkflowMonitor*>& getMonitors() const;

protected:
    QList<WorkflowMonitor*> monitors;
};  // WorkflowAbstractRunner

class U2LANG_EXPORT WorkflowAbstractIterationRunner : public Task {
    Q_OBJECT
public:
    WorkflowAbstractIterationRunner(const QString& name, TaskFlags flags);
    ~WorkflowAbstractIterationRunner() override {
    }
    virtual WorkerState getState(const ActorId& actor) = 0;
    virtual int getMsgNum(const Link* l) = 0;
    virtual int getMsgPassed(const Link* l) = 0;

    virtual int getDataProduced(const ActorId& actor) = 0;

signals:
    void si_updateProducers();
};

typedef QMap<ActorId, ActorId> ActorMap;

class U2LANG_EXPORT WorkflowRunTask : public WorkflowAbstractRunner {
    Q_OBJECT
public:
    WorkflowRunTask(const Schema&, const ActorMap& rmap = ActorMap(), WorkflowDebugStatus* debugInfo = nullptr);
    QList<WorkerState> getState(Actor*) override;
    int getMsgNum(const Link*) override;
    int getMsgPassed(const Link*) override;

private:
    QString generateReport() const override;
    // CmdlineTask
    QString getTaskError() const override;

signals:
    void si_ticked();

private:
    QMap<ActorId, ActorId> rmap;
    QList<Link*> flows;

};  // WorkflowRunTask

class WorkflowIterationRunTask : public WorkflowAbstractIterationRunner {
    Q_OBJECT
public:
    WorkflowIterationRunTask(const Schema&, WorkflowDebugStatus* initDebugInfo);
    ~WorkflowIterationRunTask() override;
    void prepare() override;
    ReportResult report() override;

    WorkerState getState(const ActorId& actor) override;
    int getMsgNum(const Link* l) override;
    int getMsgPassed(const Link* l) override;
    int getDataProduced(const ActorId& actor) override;

    WorkflowMonitor* getMonitor() const;

signals:
    void si_ticked();

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private slots:
    void sl_pauseStateChanged(bool isPaused);
    void sl_busInvestigationIsRequested(const Workflow::Link* bus, int messageNumber);
    void sl_busCountOfMessagesRequested(const Workflow::Link* bus);
    void sl_singleStepIsRequested(const ActorId& actor);
    void sl_convertMessages2Documents(const Workflow::Link* bus, const QString& messageType, int messageNumber, const QString& schemeName);

private:
    static TaskFlags getAdditionalFlags();

    QList<CommunicationChannel*> getActorLinks(const QString& actor);

    WorkflowContext* context = nullptr;
    Schema* schema = nullptr;
    Scheduler* scheduler = nullptr;
    QMap<ActorId, ActorId> rmap;
    QMap<QString, CommunicationChannel*> lmap;

    QPointer<WorkflowDebugStatus> debugInfo;
    bool nextTickRestoring = false;
    bool contextInitialized = false;
};

}  // namespace U2
