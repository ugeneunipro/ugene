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

#ifndef _U2_TEST_VIEW_CONTROLLER_H_
#define _U2_TEST_VIEW_CONTROLLER_H_

#include <U2Gui/MainWindow.h>
#include <ui_TestView.h>
#include <U2Core/CMDLineRegistry.h>

#include <QTime>
#include <QTreeWidgetItem>
#include <QMainWindow>

namespace U2 {

class GTestSuite;
class GTestRef;
class GTestState;
class TVTSItem;
class TVTestItem;
class TVItem;
class TestRunnerService;
class TestRunnerTask;
class TestViewReporter;

class TestViewController : public MWMDIWindow, Ui_TestView {
    Q_OBJECT
public:
    TestViewController(TestRunnerService* s, bool cmd = false);

    virtual void setupMDIToolbar(QToolBar* tb);
    virtual void setupViewMenu(QMenu* n);

private slots:

    void sl_treeCustomContextMenuRequested(const QPoint & pos);
    void sl_treeItemSelectionChanged();
    void sl_treeDoubleClicked(QTreeWidgetItem*, int);

    void sl_suiteAdded(GTestSuite* ts);
    void sl_suiteRemoved(GTestSuite* ts);

    void sl_addTestSuiteAction();
    void sl_removeTestSuiteAction();
    void sl_runAllSuitesAction();
    void sl_runSelectedSuitesAction();
    void sl_stopSuitesActions();
    void sl_setEnvAction();
    void sl_report();
    void sl_testStateChanged(GTestState*);
    void sl_taskStateChanged(Task* t);

    void sl_selectAllSuiteAction();
    void sl_setTestsEnabledAction();
    void sl_setTestsDisabledAction();
    void sl_setTestsChangeExcludedAction();
    void sl_saveSelectedSuitesAction();
    void sl_saveTest();


protected:
    virtual bool onCloseEvent();

private:
    void togglePopupMenuItems(bool enabled);

    void updateState();

    void createAndRunTask(const QList<GTestState*>& testsToRun);
    QList<TVTSItem*> getSelectedSuiteItems() const;
    QList<TVTestItem*> getSelectedTestItems() const;

    void addTestSuiteList(QString url);
    void addTestSuite(GTestSuite* ts);
    void addFolderTests(TVTSItem* tsi, GTestRef* testRef,const QString* curPath,bool haveExcludedTests);
    void addTest(TVTSItem* tsi, GTestRef* t,QString excludeReason);
    TVTSItem* findTestSuiteItem(GTestSuite* ts) const;
    TVTestItem* findTestViewItem(GTestRef* tr) const;
    TVTestItem* findTestViewItemRecursive(GTestRef* testRef,TVItem* sItem) const;
    TVTSItem* getFolder(TVItem* element,const QString* folderName)const;

    QList<GTestState*> getSubTestToRun(TVItem* sItem,bool runAll)const;
    QMap<GTestRef *, QString> getSubRefToExclude(TVItem* sItem,bool runAll)const;
    bool  allSuitesIsInRoot(const QList<TVTSItem*> suitesList) const;
    void setExcludedState(TVItem* sItem,bool allSelected, bool newState);
    void setExcludedState(TVItem* sItem, bool allSelected, QString reason);
    void saveTestSuite(const QString& url, QMap<GTestRef *, QString> testsToEx, QString& err);
    QStringList findAllTestFilesInDir(const QString& dirPath, const QString& ext, bool recursive, int rec);

    TestRunnerService* service;

    QAction* addTestSuiteAction;
    QAction* removeTestSuiteAction;
    QAction* runAllSuitesAction;
    QAction* runSelectedSuitesAction;
    QAction* stopSuitesActions;
    QAction* setEnvAction;
    QAction* report;
    QAction* refreshAction;
    QAction* selectAllAction;

    QAction* setTestsEnabledAction;
    QAction* setTestsDisabledAction;
    QAction* setTestsChangeExcludedAction;
    QAction* saveSelectedSuitesAction;

    TestRunnerTask* task;
    QTime startRunTime;
    QTime endRunTime;
    int time;
    bool cmd;

    friend class TestRunnerPlugin;
};

enum TVItemType {
    TVItem_TestSuite,
    TVItem_Test
};

class TVItem : public QTreeWidgetItem {
public:
    TVItem(TVItemType t) : type(t), excludedTests(false) {}

    virtual void updateVisual() = 0;

    virtual QString getRichDesc() const = 0;

    const TVItemType type;

    bool excludedTests;

    QString excludeReason;

    bool isSuite() const {return type == TVItem_TestSuite;}

    bool isTest() const {return type == TVItem_Test;}

    bool operator<(const QTreeWidgetItem &other) const {
        if (type != dynamic_cast<const TVItem &>(other).type) {
            return type == TVItem_TestSuite;
        }
        return text(0) < other.text(0);
    }

};

class TVTSItem : public  TVItem {
public:
    TVTSItem(GTestSuite* ts);

    TVTSItem(const QString& _name);

    virtual void updateVisual();

    virtual QString getRichDesc() const;

    GTestSuite* ts;

    QString name;

    QString getURL() const;

    void getTestsState(int *passed,int *failed,int *none,int *excluded);

};

class TVTestItem : public  TVItem {
public:
    TVTestItem(GTestState* testState);

    ~TVTestItem();

    virtual void updateVisual();

    virtual QString getRichDesc() const;

    QString getTestContent();

    GTestState* testState;
};

}//namespace

#endif

