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

#ifndef _U2_TASK_TESTS_H_
#define _U2_TASK_TESTS_H_

#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <QDomElement>
namespace U2 {

class StateOrderTestTask;

class InfiniteTestTask : public Task {
    Q_OBJECT
public:
    InfiniteTestTask(QString _taskName, TaskFlags _f) : Task(_taskName, _f) {}
    void run() override;
};

class DestructorCleanupTask : public Task {
    Q_OBJECT
public:
    DestructorCleanupTask(QString taskName, TaskFlags f);
    ~DestructorCleanupTask();
};

enum StateOrderType {
    StateOrder_Prepare, StateOrder_Run, StateOrder_Report, StateOrder_Done
};

class StateOrderTestTaskCallback {
public:
    virtual void func(StateOrderTestTask *t,StateOrderType st) = 0;
    virtual ~StateOrderTestTaskCallback() {}
};

class StateOrderTestTask : public Task {
    Q_OBJECT
public:
    StateOrderTestTask(StateOrderTestTaskCallback *ptr, TaskFlags _f);
    ~StateOrderTestTask();
    void prepare();
    void run();
    Task::ReportResult report();
    int step;
private:
    StateOrderTestTaskCallback *callback;
};

class GTest_TaskCreateTest : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_TaskCreateTest, "task-create");

    ReportResult report();
    void cleanup();
private:
    Task *task;
    bool deleteTask;
    QString resultContextName;
};

class GTest_TaskAddSubtaskTest : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_TaskAddSubtaskTest, "task-add-subtask");

    ReportResult report();
private:
    QString taskContextName;
    QString subtaskContextName;
};

class GTest_TaskCancelTest : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_TaskCancelTest, "task-cancel");

    ReportResult report();
private:
    QString objContextName;
};

class GTest_TaskCheckFlag : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_TaskCheckFlag, "task-check-flag");

    ReportResult report();
private:
    TaskFlags flag;
    QString taskContextName;
};

class GTest_TaskCheckState : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_TaskCheckState, "task-check-state");

    ReportResult report();
private:
    bool checkState;
    State taskState;
    bool checkProgress;
    bool checkCancelFlag;
    bool checkError;
    bool checkStateDesc;
    TaskStateInfo taskStateInfo;
    QString taskContextName;
};

class GTest_TaskExec : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_TaskExec, "task-exec");

    void prepare();
    ReportResult report();
private:
    QString taskContextName;
};

class GTest_TaskStateOrder : public XmlTest, public StateOrderTestTaskCallback{
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_TaskStateOrder, "task-state-order-test", TaskFlags_FOSCOE);
    void func(StateOrderTestTask *t,StateOrderType st);
    Task::ReportResult report();
    void run();
private:
    bool done_flag;
    StateOrderTestTask *task;
    QList<StateOrderTestTask*> subs;
    int subtask_num;
    bool serial_flag;
    bool cancel_flag;
    bool run_after_all_subs_flag;
};

class GTest_Wait : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Wait, "wait",TaskFlags(TaskFlags_FOSCOE));
    Task::ReportResult report();
    void prepare();
    void run();
public slots:
    void sl_WaitCond_StateChanged();
private:
    enum WaitCond {WaitCond_None,WaitCond_StateChanged};
    WaitCond    condition;
    State       waitForState;
    QString     objContextName;
    int         ms;
    bool        waitOk;
    QString     waitCondString;
    QString     waitStateString;

};

class TaskTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}//namespace
#endif
