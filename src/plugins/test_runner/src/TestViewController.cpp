/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "TestViewController.h"

#include <QDialogButtonBox>
#include <QDomDocument>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/U2FileDialog.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/TestRunnerTask.h>

#include "ExcludeReasonDialog.h"
#include "TestRunnerPlugin.h"
#include "TestViewReporter.h"

// todo: remember splitter geom

namespace U2 {

#define SETTINGS_ROOT QString("test_runner/view/")

#define NO_REASON QString("no")

#define ICON_FAILD_DIR QIcon(":/plugins/test_runner/images/folder_faild.png")
#define ICON_SUCCES_DIR QIcon(":/plugins/test_runner/images/folder_ok.png")
#define ICON_NOTRUN_DIR QIcon(":/plugins/test_runner/images/folder.png")
#define ICON_FAILD_TEST QIcon(":/plugins/test_runner/images/test_faild.png")
#define ICON_SUCCES_TEST QIcon(":/plugins/test_runner/images/test_ok.png")
#define ICON_NOTRUN_TEST QIcon(":/plugins/test_runner/images/test.png")

static Logger teamcityLog(ULOG_CAT_TEAMCITY);

TestViewController::TestViewController(TestRunnerService* s, bool _cmd)
    : MWMDIWindow(tr("Test runner")), service(s), cmd(_cmd) {
    task = nullptr;
    setupUi(this);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->setColumnWidth(0, AppContext::getSettings()->getValue(SETTINGS_ROOT + "treeColWidth", 400).toInt());
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->sortByColumn(0, Qt::AscendingOrder);

    splitter->setStretchFactor(0, 10);
    splitter->setStretchFactor(1, 3);

    addTestSuiteAction = new QAction(tr("Add suite"), this);
    addTestSuiteAction->setObjectName("action_add_suite");
    connect(addTestSuiteAction, SIGNAL(triggered()), SLOT(sl_addTestSuiteAction()));

    removeTestSuiteAction = new QAction(tr("Remove suite"), this);
    removeTestSuiteAction->setObjectName("action_remove_suite");
    connect(removeTestSuiteAction, SIGNAL(triggered()), SLOT(sl_removeTestSuiteAction()));

    runAllSuitesAction = new QAction(tr("Run all tests"), this);
    runAllSuitesAction->setObjectName("action_run_all_tests");
    connect(runAllSuitesAction, SIGNAL(triggered()), SLOT(sl_runAllSuitesAction()));

    runSelectedSuitesAction = new QAction(tr("Run selected"), this);
    runSelectedSuitesAction->setObjectName("action_run_selected");
    QKeySequence runAllKS(Qt::CTRL + Qt::Key_R);
    runSelectedSuitesAction->setShortcut(runAllKS);
    connect(runSelectedSuitesAction, SIGNAL(triggered()), SLOT(sl_runSelectedSuitesAction()));

    stopSuitesActions = new QAction(tr("Stop"), this);
    stopSuitesActions->setObjectName("action_stop");
    connect(stopSuitesActions, SIGNAL(triggered()), SLOT(sl_stopSuitesActions()));

    setEnvAction = new QAction(tr("Set environment"), this);
    setEnvAction->setObjectName("action_setup_environment");
    connect(setEnvAction, SIGNAL(triggered()), SLOT(sl_setEnvAction()));

    report = new QAction(tr("Generate report"), this);
    report->setObjectName("action_generate_report");
    connect(report, SIGNAL(triggered()), SLOT(sl_report()));

    refreshAction = new QAction(tr("Reload suites"), this);
    refreshAction->setObjectName("action_reload_suites");
    refreshAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(refreshAction, SIGNAL(triggered()), service, SLOT(sl_refresh()));

    selectAllAction = new QAction(tr("Select All"), this);
    selectAllAction->setObjectName("action_select_all");
    QKeySequence selectAllKS(Qt::CTRL + Qt::Key_A);
    selectAllAction->setShortcut(selectAllKS);
    connect(selectAllAction, SIGNAL(triggered()), SLOT(sl_selectAllSuiteAction()));

    setTestsEnabledAction = new QAction(tr("Set Tests Enabled"), this);
    setTestsEnabledAction->setObjectName("action_set_tests_enabled");
    QKeySequence setTestsEnabledKS(Qt::CTRL + Qt::Key_E);
    setTestsEnabledAction->setShortcut(setTestsEnabledKS);
    connect(setTestsEnabledAction, SIGNAL(triggered()), SLOT(sl_setTestsEnabledAction()));

    setTestsDisabledAction = new QAction(tr("Set Tests Disabled"), this);
    setTestsDisabledAction->setObjectName("action_set_tests_disabled");
    QKeySequence setTestsDisabledKS(Qt::CTRL + Qt::Key_D);
    setTestsDisabledAction->setShortcut(setTestsDisabledKS);
    connect(setTestsDisabledAction, SIGNAL(triggered()), SLOT(sl_setTestsDisabledAction()));

    setTestsChangeExcludedAction = new QAction(tr("Change Tests Excluded State"), this);
    setTestsChangeExcludedAction->setObjectName("action_change_tests_excluded_state");
    connect(setTestsChangeExcludedAction, SIGNAL(triggered()), SLOT(sl_setTestsChangeExcludedAction()));

    saveSelectedSuitesAction = new QAction(tr("Save Selected Suites"), this);
    saveSelectedSuitesAction->setObjectName("action_save_selected_suites");
    connect(saveSelectedSuitesAction, SIGNAL(triggered()), SLOT(sl_saveSelectedSuitesAction()));

    connect(s, SIGNAL(si_testSuiteAdded(GTestSuite*)), SLOT(sl_suiteAdded(GTestSuite*)));
    connect(s, SIGNAL(si_testSuiteRemoved(GTestSuite*)), SLOT(sl_suiteRemoved(GTestSuite*)));

    connect(tree, SIGNAL(itemSelectionChanged()), SLOT(sl_treeItemSelectionChanged()));
    connect(tree, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(sl_treeCustomContextMenuRequested(const QPoint&)));

    connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(sl_treeDoubleClicked(QTreeWidgetItem*, int)));

    connect(saveButton, SIGNAL(clicked()), SLOT(sl_saveTest()));

    const QList<GTestSuite*> suites = s->getTestSuites();
    foreach (GTestSuite* ts, suites) {
        addTestSuite(ts);
    }
    updateState();
}

bool TestViewController::onCloseEvent() {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "treeColWidth", tree->columnWidth(0));

    if (task != nullptr)
        task->cancel();
    return true;
}

void TestViewController::setupMDIToolbar(QToolBar* tb) {
    tb->addAction(addTestSuiteAction);
    tb->addAction(removeTestSuiteAction);
    tb->addAction(setEnvAction);
    tb->addAction(runAllSuitesAction);
    tb->addAction(runSelectedSuitesAction);
    tb->addAction(stopSuitesActions);
    tb->addAction(report);
}

void TestViewController::setupViewMenu(QMenu* m) {
    m->addAction(addTestSuiteAction);
    m->addAction(removeTestSuiteAction);
    m->addAction(refreshAction);
    m->addAction(setEnvAction);
    m->addAction(runAllSuitesAction);
    m->addAction(runSelectedSuitesAction);
    m->addAction(stopSuitesActions);
    m->addAction(report);
    m->addAction(selectAllAction);

    auto excludeMenu = new QMenu("Exclude actions", m);
    excludeMenu->setObjectName("exclude_actions_menu");
    excludeMenu->addAction(setTestsEnabledAction);
    excludeMenu->addAction(setTestsDisabledAction);
    excludeMenu->addAction(setTestsChangeExcludedAction);
    excludeMenu->addAction(saveSelectedSuitesAction);
    m->addMenu(excludeMenu);
}

TVTSItem* TestViewController::findTestSuiteItem(GTestSuite* ts) const {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->type == TVItem_TestSuite);
        auto tsi = static_cast<TVTSItem*>(item);
        if (tsi->ts == ts) {
            return tsi;
        }
    }
    return nullptr;
}

TVTestItem* TestViewController::findTestViewItem(GTestRef* testRef) const {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->type == TVItem_TestSuite);
        auto sItem = static_cast<TVTSItem*>(item);
        TVTestItem* rezult = findTestViewItemRecursive(testRef, sItem);
        if (rezult)
            return rezult;
    }
    return nullptr;
}
TVTestItem* TestViewController::findTestViewItemRecursive(GTestRef* testRef, TVItem* sItem) const {
    for (int j = 0, m = sItem->childCount(); j < m; j++) {
        auto item = static_cast<TVItem*>(sItem->child(j));
        if (item->isTest()) {
            auto tItem = static_cast<TVTestItem*>(item);
            if (tItem->testState->getTestRef() == testRef) {
                return tItem;
            }
        } else {
            assert(item->isSuite());
            auto rezult = findTestViewItemRecursive(testRef, static_cast<TVTSItem*>(item));
            if (rezult)
                return rezult;
        }
    }
    return nullptr;
}

TVTSItem* TestViewController::getFolder(TVItem* element, const QString* firstDirName) const {
    for (int j = 0, m = element->childCount(); j < m; j++) {
        auto item = static_cast<TVItem*>(element->child(j));
        if (item->isSuite()) {
            auto ditem = static_cast<TVTSItem*>(item);
            if (ditem->name == firstDirName) {
                return ditem;
            }
        }
    }
    return nullptr;
}

//<-----------------------------------------------------------------------------------
void TestViewController::addTestSuite(GTestSuite* ts) {
    auto tsi = new TVTSItem(ts);
    // add to tree Excluded Tests
    QMap<GTestRef*, QString> exCopy = ts->getExcludedTests();
    QMap<QString, GTestRef*> excludedSorted;  //<test_ShortName, test> sorted by name
    QMap<GTestRef*, QString>::iterator iter;

    for (iter = exCopy.begin(); iter != exCopy.end(); ++iter) {
        excludedSorted.insert(dynamic_cast<GTestRef*>(iter.key())->getShortName(), dynamic_cast<GTestRef*>(iter.key()));
    }
    foreach (GTestRef* t, excludedSorted.values()) {
        QString firstDirName = t->getShortName().section('/', 0, 0);  // find first folder name
        if (t->getShortName() == firstDirName) {
            addTest(tsi, t, ts->getExcludedTests().value(t));
        } else {
            TVTSItem* curDir = getFolder(tsi, &firstDirName);
            QString otherPath = t->getShortName().section('/', 1);  // find other path
            if (curDir) {  // find if dir already exist
                addFolderTests(curDir, t, &otherPath, true);
            } else {
                auto newDir = new TVTSItem(firstDirName);
                tsi->addChild(newDir);
                addFolderTests(newDir, t, &otherPath, true);
            }
        }
    }
    // add to tree Tests to run
    foreach (GTestRef* t, ts->getTests()) {
        QString firstDirName = t->getShortName().section('/', 0, 0);  // find first folder name
        if (t->getShortName() == firstDirName) {
            addTest(tsi, t, "");
        } else {
            TVTSItem* curDir = getFolder(tsi, &firstDirName);
            QString otherPath = t->getShortName().section('/', 1);  // find other path
            if (curDir) {  // find if dir already exist
                addFolderTests(curDir, t, &otherPath, false);
            } else {
                auto newDir = new TVTSItem(firstDirName);
                tsi->addChild(newDir);
                addFolderTests(newDir, t, &otherPath, false);
            }
        }
    }

    tsi->updateVisual();
    tree->addTopLevelItem(tsi);
}

void TestViewController::addFolderTests(TVTSItem* tsi, GTestRef* testRef, const QString* curPath, bool haveExcludedTests) {
    QString firstDirName = curPath->section('/', 0, 0);
    if (*curPath == firstDirName) {
        addTest(tsi, testRef, testRef->getSuite()->getExcludedTests().value(testRef));
    } else {
        TVTSItem* curDir = getFolder(tsi, &firstDirName);
        QString otherPath = curPath->section('/', 1);  // find other path
        if (curDir) {  // find if dir already exist
            addFolderTests(curDir, testRef, &otherPath, haveExcludedTests);
        } else {
            auto newDir = new TVTSItem(firstDirName);
            newDir->isExcluded = haveExcludedTests;
            tsi->addChild(newDir);
            addFolderTests(newDir, testRef, &otherPath, haveExcludedTests);
        }
    }
    tsi->updateVisual();
}

void TestViewController::addTest(TVTSItem* tsi, GTestRef* testRef, const QString& excludeReason) {
    auto testState = new GTestState(testRef);
    connect(testState, SIGNAL(si_stateChanged(GTestState*)), SLOT(sl_testStateChanged(GTestState*)));
    auto ti = new TVTestItem(testState);
    ti->excludeReason = excludeReason;
    if (!excludeReason.isEmpty()) {
        ti->isExcluded = true;
    }
    ti->updateVisual();
    tsi->addChild(ti);
}

void TestViewController::sl_suiteAdded(GTestSuite* ts) {
    addTestSuite(ts);
    updateState();
}

void TestViewController::sl_suiteRemoved(GTestSuite* ts) {
    TVTSItem* item = findTestSuiteItem(ts);
    assert(item != nullptr && item->parent() == nullptr);
    // need this way of removal to avoid crash in destructor if
    // child item of the suite is selected and is asked for rich desc during parent removal
    tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));
    delete item;
    updateState();
}

void TestViewController::sl_treeCustomContextMenuRequested(const QPoint& pos) {
    Q_UNUSED(pos);

    QMenu menu(tree);
    menu.addAction(addTestSuiteAction);
    menu.addAction(removeTestSuiteAction);
    menu.addAction(refreshAction);
    menu.addAction(runAllSuitesAction);
    menu.addAction(runSelectedSuitesAction);
    menu.addAction(stopSuitesActions);
    menu.addAction(setEnvAction);
    menu.addAction(report);
    menu.addAction(setTestsChangeExcludedAction);
    menu.addAction(saveSelectedSuitesAction);
    menu.exec(QCursor::pos());
}

void TestViewController::sl_treeItemSelectionChanged() {
    updateState();
}

QList<TVTSItem*> TestViewController::getSelectedSuiteItems() const {
    QList<TVTSItem*> res;
    QList<QTreeWidgetItem*> items = tree->selectedItems();
    foreach (QTreeWidgetItem* i, items) {
        auto item = static_cast<TVItem*>(i);
        if (item->isSuite()) {
            res.append(static_cast<TVTSItem*>(item));
        }
    }
    return res;
}

QList<TVTestItem*> TestViewController::getSelectedTestItems() const {
    QList<TVTestItem*> res;
    QList<QTreeWidgetItem*> items = tree->selectedItems();
    foreach (QTreeWidgetItem* i, items) {
        auto item = static_cast<TVItem*>(i);
        if (item->isTest()) {
            res.append(static_cast<TVTestItem*>(item));
        }
    }
    return res;
}

bool TestViewController::allSuitesIsInRoot(const QList<TVTSItem*>& suites) const {
    if (suites.isEmpty())
        return false;
    bool rezult = true;
    foreach (TVTSItem* si, suites) {
        if (si->ts == nullptr)
            rezult = false;
    }
    return rezult;
}

void TestViewController::updateState() {
    QList<TVTSItem*> suites = getSelectedSuiteItems();
    QList<TVTestItem*> tests = getSelectedTestItems();

    removeTestSuiteAction->setEnabled(allSuitesIsInRoot(suites) && task == nullptr && tests.isEmpty());
    setEnvAction->setEnabled(task == nullptr);
    runAllSuitesAction->setEnabled(!service->getTestSuites().isEmpty() && task == nullptr);
    report->setEnabled(!service->getTestSuites().isEmpty() && task == nullptr);
    runSelectedSuitesAction->setEnabled((!suites.isEmpty() || !tests.isEmpty()) && task == nullptr);
    stopSuitesActions->setEnabled(task != nullptr);

    if (tree->currentItem() != nullptr) {
        auto i = static_cast<TVItem*>(tree->currentItem());
        contextInfoEdit->setText(i->getRichDesc());
        auto testItem = dynamic_cast<TVTestItem*>(i);
        if (testItem != nullptr) {
            testEdit->setText(testItem->getTestContent());
        }
    }
}

void TestViewController::addTestSuiteList(const QString& url) {
    // QString dir = AppContext::getSettings()->getValue(SETTINGS_ROOT + "lastDir", QString()).toString();
    if (url.isEmpty())
        return;

    QStringList errs;
    QList<GTestSuite*> lst = GTestSuite::readTestSuiteList(url, errs);
    if (!errs.isEmpty()) {
        QMessageBox::critical(this, tr("error"), tr("Error reading test suites: \n\n %1").arg(errs.join("\n")));
        // return;
    }
    foreach (GTestSuite* ts, lst) {
        QString urlfs = ts->getURL();
        if (service->findTestSuiteByURL(urlfs) != nullptr) {
            delete ts;
        } else {
            service->addTestSuite(ts);
        }
    }
}

void TestViewController::sl_addTestSuiteAction() {
    QString dir = AppContext::getSettings()->getValue(SETTINGS_ROOT + "lastDir", QString()).toString();
    QStringList fileNames = U2FileDialog::getOpenFileNames(this, tr("Select test suite file"), dir);
    bool saveLastDir = true;
    foreach (QString file, fileNames) {
        if (file.isEmpty()) {
            return;
        }
        QFileInfo fi(file);
        dir = fi.absoluteDir().absolutePath();
        if (saveLastDir) {
            saveLastDir = false;
            AppContext::getSettings()->setValue(SETTINGS_ROOT + "lastDir", dir);
        }
        QString url = fi.absoluteFilePath();
        if (fi.suffix() == "list") {
            addTestSuiteList(url);
        } else {
            if (service->findTestSuiteByURL(url) != nullptr) {
                // QMessageBox::critical(this, tr("error"), tr("Test suite is already loaded"));
                // return;
            } else {
                QString err;
                GTestSuite* ts = GTestSuite::readTestSuite(url, err);
                if (ts == nullptr) {
                    assert(!err.isEmpty());
                    QMessageBox::critical(this, tr("error"), tr("Error reading test suite: %1").arg(err));
                    // return;
                } else {
                    service->addTestSuite(ts);
                }
            }
        }
    }
}

void TestViewController::sl_removeTestSuiteAction() {
    if (tree->topLevelItemCount() == 0) {
        return;
    }
    for (int i = tree->topLevelItemCount() - 1, n = 0; i >= n; i--) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        if (item->isSelected()) {
            auto sItem = static_cast<TVTSItem*>(item);
            service->removeTestSuite(sItem->ts);
        }
    }
}
void TestViewController::sl_selectAllSuiteAction() {
    if (tree->topLevelItemCount() == 0) {
        return;
    }
    for (int i = tree->topLevelItemCount() - 1, n = 0; i >= n; i--) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        item->setSelected("true");
    }
    updateState();
}

void TestViewController::sl_setTestsEnabledAction() {
    CHECK(task == nullptr, );

    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        bool allSelected = false;
        bool newState = false;
        if (item->isSelected())
            allSelected = true;
        setExcludedState(item, allSelected, newState);
    }
}
void TestViewController::sl_setTestsDisabledAction() {
    CHECK(task == nullptr, );

    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        bool allSelected = false;
        bool newState = true;
        if (item->isSelected())
            allSelected = true;
        setExcludedState(item, allSelected, newState);
    }
}
void TestViewController::sl_setTestsChangeExcludedAction() {
    CHECK(task == nullptr, );

    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        bool allSelected = false;
        if (item->isSelected())
            allSelected = true;
        setExcludedState(item, allSelected, NO_REASON);
    }
}
void TestViewController::sl_saveSelectedSuitesAction() {
    CHECK(task == nullptr, );
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        QList<GTestState*> testsToRun;
        QMap<GTestRef*, QString> testsToEx;
        QMap<GTestRef*, QString> oldToAdd;
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        bool runAll = false;
        bool mustBeSaved = false;
        if (item->isSelected())
            runAll = true;
        testsToRun += getSubTestToRun(item, runAll);
        testsToEx.unite(getSubRefToExclude(item, runAll));
        auto tlItem = static_cast<TVTSItem*>(item);
        if (testsToEx.isEmpty() && testsToRun.isEmpty()) {
            // in current suite no selected elements
        } else {
            if (!testsToEx.isEmpty() && tlItem->ts->getExcludedTests().isEmpty()) {
                mustBeSaved = true;
            }
            foreach (GTestRef* t, tlItem->ts->getExcludedTests().keys()) {  // get one of the old excluded tests
                bool flag = true;
                for (GTestState* ttr : qAsConst(testsToRun)) {  // get one by one new enabled tests
                    if (t->getShortName() == ttr->getTestRef()->getShortName()) {
                        flag = false;
                        break;
                    }
                }
                if (flag) {  // if the old exclude-value is not changed to enabled
                    if (testsToEx.isEmpty()) {
                        oldToAdd.insert(t, tlItem->ts->getExcludedTests().value(t));
                    } else {
                        bool flag2 = true;
                        QList<GTestRef*> excludedTestKeys = testsToEx.keys();
                        for (GTestRef* tte : qAsConst(excludedTestKeys)) {
                            if (tte->getShortName() == t->getShortName()) {
                                flag2 = false;
                                break;
                            }
                        }
                        if (flag2) {
                            oldToAdd.insert(t, tlItem->ts->getExcludedTests().value(t));
                            mustBeSaved = true;
                        }
                    }
                } else {  // old value excluded change to enabled
                    mustBeSaved = true;
                }
            }
        }
        testsToEx.unite(oldToAdd);
        if ((testsToEx.size() != tlItem->ts->getExcludedTests().size()) && !testsToEx.isEmpty()) {
            mustBeSaved = true;
        }
        if (mustBeSaved) {  // than save
            QString err;
            saveTestSuite(tlItem->ts->getURL(), testsToEx, err);
            if (!err.isEmpty()) {
                return;  // todo throw error
            }
            // reload saved suite
            QString urlToLoad = tlItem->ts->getURL();
            service->removeTestSuite(tlItem->ts);
            GTestSuite* ts = GTestSuite::readTestSuite(urlToLoad, err);
            service->addTestSuite(ts);
        }
    }
}

void TestViewController::saveTestSuite(const QString& url, QMap<GTestRef*, QString> testsToEx, QString& err) {
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) {
        err = ("cant_open_file");
        return;
    }
    QByteArray xmlData = f.readAll();
    f.close();

    QDomDocument suiteDoc;
    bool res = suiteDoc.setContent(xmlData);
    if (!res) {
        err = ("not_an_xml_suite_file");
        return;
    }

    QDomElement suiteEl = suiteDoc.documentElement();
    if (suiteEl.tagName() != "suite") {
        err = ("suite_elem_not_found");
        return;
    }

    QDomNodeList excludeEls = suiteEl.elementsByTagName("excluded");
    int countToremove = excludeEls.size();
    for (int i = 0; i < countToremove; i++) {
        QDomNode node = excludeEls.at(0);
        suiteEl.removeChild(node);
    }

    QMap<QString, QString> tests;  // sorted
    QMap<GTestRef*, QString>::iterator i = testsToEx.begin();
    while (i != testsToEx.end()) {
        tests.insert(dynamic_cast<GTestRef*>(i.key())->getShortName(), i.value());

        /*QDomElement exEl = suiteDoc.createElement("excluded");
        exEl.setAttribute("test", dynamic_cast<GTestRef*>(i.key())->getShortName());
        exEl.setAttribute("reason", i.value());
        suiteEl.appendChild(exEl);*/
        ++i;
    }

    foreach (QString test, tests.keys()) {
        QDomElement exEl = suiteDoc.createElement("excluded");
        exEl.setAttribute("test", test);
        exEl.setAttribute("reason", tests.value(test));
        suiteEl.appendChild(exEl);
    }

    // Tests
    if (!err.isEmpty()) {
        return;
    }
    // time to save
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        err = ("error save suite file");
        return;
    }
    io->writeBlock(suiteDoc.toByteArray());
}

QStringList TestViewController::findAllTestFilesInDir(const QString& dirPath, const QString& ext, bool recursive, int rec) {
    QStringList res;
    if (rec > 100) {  // symlink or other err
        // todo: report err?
        return res;
    }
    QDir dir(dirPath);

    // add files first
    QStringList files = ext.isEmpty() ? dir.entryList(QDir::Files) : dir.entryList(ext.split(":"), QDir::Files);
    foreach (const QString& file, files) {
        QFileInfo fi(dir.absolutePath() + "/" + file);
        res.append(fi.absoluteFilePath());
    }

    // process subdirs if needed
    if (recursive) {
        QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (QString sub, subDirs) {
            QString subDirPath = dirPath + "/" + sub;
            QStringList subRes = findAllTestFilesInDir(subDirPath, ext, true, rec++);
            res += subRes;
        }
    }
    return res;
}
void TestViewController::setExcludedState(TVItem* sItem, bool allSelected, bool newState) {
    for (int j = 0, m = sItem->childCount(); j < m; j++) {
        auto item = static_cast<TVItem*>(sItem->child(j));
        if (item->isTest()) {
            auto tItem = static_cast<TVTestItem*>(item);
            if (tItem->isSelected() || allSelected) {
                tItem->isExcluded = newState;
                tItem->updateVisual();
                auto temp_parent = static_cast<TVTSItem*>(tItem->parent());
                temp_parent->updateVisual();
            }
        } else {
            assert(item->isSuite());
            auto tItem2 = static_cast<TVTSItem*>(item);
            if (tItem2->isSelected()) {
                setExcludedState(tItem2, true, newState);
            } else {
                setExcludedState(tItem2, allSelected, newState);
            }
        }
    }
}
void TestViewController::setExcludedState(TVItem* sItem, bool allSelected, QString reason) {
    if (allSelected && reason == NO_REASON) {
        QObjectScopedPointer<ExcludeReasonDialog> dlg = new ExcludeReasonDialog;
        const int rc = dlg->exec();
        CHECK(!dlg.isNull(), );

        if (rc != QDialog::Accepted) {
            return;
        }
        reason = dlg->getReason();
    }

    for (int j = 0, m = sItem->childCount(); j < m; j++) {
        auto item = static_cast<TVItem*>(sItem->child(j));
        if (item->isTest()) {
            auto tItem = static_cast<TVTestItem*>(item);
            if (tItem->isSelected() || allSelected) {
                tItem->isExcluded = !tItem->isExcluded;
                if (tItem->isExcluded) {
                    if (!allSelected && reason == NO_REASON) {
                        QObjectScopedPointer<ExcludeReasonDialog> dlg = new ExcludeReasonDialog;
                        const int rc = dlg->exec();
                        CHECK(!dlg.isNull(), );

                        if (rc != QDialog::Accepted) {
                            return;
                        }
                        reason = dlg->getReason();
                    }
                    tItem->excludeReason = reason;
                }
                tItem->updateVisual();
                auto temp_parent = static_cast<TVTSItem*>(tItem->parent());
                temp_parent->updateVisual();
            }
        } else {
            assert(item->isSuite());
            auto tItem2 = static_cast<TVTSItem*>(item);
            if (tItem2->isSelected()) {
                setExcludedState(tItem2, true, reason);
            } else {
                setExcludedState(tItem2, allSelected, reason);
            }
        }
    }
}

void TestViewController::sl_runAllSuitesAction() {
    assert(task == nullptr);
    GTestEnvironment* env = service->getEnv();
    if (env->containsEmptyVars()) {
        QMessageBox::critical(this, tr("error"), tr("Not all environment variables set"));
        return;
    }
    QList<GTestState*> testsToRun;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        assert(item->isSuite());
        bool runAll = true;
        testsToRun += getSubTestToRun(item, runAll);
    }
    if (!testsToRun.isEmpty()) {
        createAndRunTask(testsToRun);
    }
}

void TestViewController::sl_runSelectedSuitesAction() {
    GTestEnvironment* env = service->getEnv();
    if (env->containsEmptyVars()) {
        QMessageBox::critical(this, tr("error"), tr("Not all environment variables set"));
        return;
    }
    QList<GTestState*> testsToRun;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto item = static_cast<TVItem*>(tree->topLevelItem(i));
        SAFE_POINT(item->isSuite(), "Top level item is not a test suite!", );
        bool runAll = item->isSelected();
        testsToRun += getSubTestToRun(item, runAll);
    }
    if (!testsToRun.isEmpty()) {
        createAndRunTask(testsToRun);
    }
}

QList<GTestState*> TestViewController::getSubTestToRun(TVItem* sItem, bool runAll) const {
    QList<GTestState*> testsToRun;
    for (int j = 0, m = sItem->childCount(); j < m; j++) {
        auto item = static_cast<TVItem*>(sItem->child(j));
        if (item->isTest()) {
            auto tItem = static_cast<TVTestItem*>(item);
            if (!tItem->isExcluded && (tItem->isSelected() || runAll)) {
                testsToRun.append(tItem->testState);
            }
        } else {
            assert(item->isSuite());
            auto tItem2 = static_cast<TVTSItem*>(item);
            testsToRun += getSubTestToRun(tItem2, tItem2->isSelected() || runAll);
        }
    }
    return testsToRun;
}
QMap<GTestRef*, QString> TestViewController::getSubRefToExclude(TVItem* sItem, bool runAll) const {
    QMap<GTestRef*, QString> testsToEx;
    for (int j = 0, m = sItem->childCount(); j < m; j++) {
        auto item = static_cast<TVItem*>(sItem->child(j));
        if (item->isTest()) {
            auto tItem = static_cast<TVTestItem*>(item);
            if (tItem->isExcluded) {
                if (tItem->isSelected() || runAll) {
                    testsToEx.insert(tItem->testState->getTestRef(), tItem->excludeReason);
                }
            }
        } else {
            assert(item->isSuite());
            auto tItem2 = static_cast<TVTSItem*>(item);
            if (tItem2->isSelected()) {
                testsToEx.unite(getSubRefToExclude(tItem2, true));
            } else {
                testsToEx.unite(getSubRefToExclude(tItem2, runAll));
            }
        }
    }
    return testsToEx;
}

void TestViewController::sl_stopSuitesActions() {
    task->cancel();
}

void TestViewController::togglePopupMenuItems(bool enabled) {
    removeTestSuiteAction->setEnabled(enabled);
    refreshAction->setEnabled(enabled);
    saveSelectedSuitesAction->setEnabled(enabled);
    setTestsChangeExcludedAction->setEnabled(enabled);
    setTestsDisabledAction->setEnabled(enabled);
}

void TestViewController::createAndRunTask(const QList<GTestState*>& testsToRun) {
    bool ok;
    startRunTime = QTime::currentTime();
    int numberTestsToRun = service->getEnv()->getVar("NUM_THREADS").toInt(&ok);
    if (!ok || numberTestsToRun <= 0) {
        numberTestsToRun = 5;
    }
    auto ttask = new TestRunnerTask(testsToRun, service->getEnv(), numberTestsToRun);

    togglePopupMenuItems(false);
    task = ttask;
    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));

    AppContext::getTaskScheduler()->registerTopLevelTask(ttask);
    updateState();
}

void TestViewController::sl_taskStateChanged(Task* t) {
    if (t != task) {
        return;
    }
    if (!t->isFinished()) {
        return;
    }
    togglePopupMenuItems(true);

    task = nullptr;
    AppContext::getTaskScheduler()->disconnect(this);
    endRunTime = QTime::currentTime();
    int ttime = endRunTime.second() + 60 * (endRunTime.minute() + (60 * (endRunTime.hour())));
    time = ttime - (startRunTime.second() + 60 * (startRunTime.minute() + (60 * (startRunTime.hour()))));
    updateState();
    if (cmd) {
        if (!t->isCanceled()) {
            QString repDir;
            if (AppContext::getCMDLineRegistry()->hasParameter(CMDLineCoreOptions::TEST_REPORT)) {
                repDir = AppContext::getCMDLineRegistry()->getParameterValue(CMDLineCoreOptions::TEST_REPORT);
            } else {
                repDir = "test_report.html";
            }
            QString htmlReport = TestViewReporter::generateHtmlReport(tree, time);
            TestViewReporter::saveReport(repDir, htmlReport);
        }
        AppContext::getTaskScheduler()->cancelAllTasks();
        exit(0);
    }
}

void TestViewController::sl_setEnvAction() {
    GTestEnvironment* env = service->getEnv();
    const QMap<QString, QString>& vars = env->getVars();
    if (vars.isEmpty()) {
        QMessageBox::information(this, tr("info"), tr("No environment variables found"));
        return;
    }

    // todo: create custom utility class for properties like this
    QObjectScopedPointer<QDialog> d = new QDialog(this);
    d->setMinimumWidth(400);
    d->setWindowTitle(tr("Set Environment"));
    auto vl = new QVBoxLayout();
    d->setLayout(vl);

    auto gl = new QGridLayout();
    vl->addLayout(gl);

    QMap<QString, QLineEdit*> valsByName;
    int row = 0;
    foreach (const QString& name, vars.keys()) {
        QString val = vars.value(name);
        auto le = new QLineEdit(val, d.data());
        le->setObjectName(name + "_EditBox");
        le->setMinimumWidth(300);
        auto la = new QLabel(name + ":");
        la->setObjectName(name + "_Label");
        la->setBuddy(le);
        valsByName[name] = le;
        gl->addWidget(la, row, 0);
        gl->addWidget(le, row, 1);
        if (name.endsWith("_DIR")) {
            auto fileOpenButton = new QToolButton();
            fileOpenButton->setText("...");
            gl->addWidget(fileOpenButton, row, 2);
            connect(fileOpenButton, &QToolButton::clicked, this, [le] {
                QString dir = U2FileDialog::getExistingDirectory();
                if (!dir.isEmpty()) {
                    le->setText(dir);
                }
            });
        }
        row++;
    }

    vl->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    auto hl = new QHBoxLayout();
    vl->addLayout(hl);
    auto dbb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    d->connect(dbb, SIGNAL(accepted()), SLOT(accept()));
    d->connect(dbb, SIGNAL(rejected()), SLOT(reject()));
    vl->addWidget(dbb);
    int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }

    // save info from dialog to model
    foreach (const QString& name, vars.keys()) {
        QLineEdit* le = valsByName.value(name);
        assert(le);
        QString val = le->text();
        env->setVar(name, val);
    }
}

void TestViewController::sl_report() {
    TestViewReporter::saveReportToFileAndOpenBrowser(tree, time);
}

void TestViewController::sl_treeDoubleClicked(QTreeWidgetItem* i, int) {
    auto tvItem = static_cast<TVItem*>(i);
    CHECK(tvItem->isTest(), )
    sl_runSelectedSuitesAction();
}

void TestViewController::sl_testStateChanged(GTestState* ts) {
    TVTestItem* tItem = findTestViewItem(ts->getTestRef());
    assert(tItem);
    tItem->updateVisual();
    auto temp_parent = static_cast<TVTSItem*>(tItem->parent());
    temp_parent->updateVisual();
}

void TestViewController::sl_saveTest() {
    auto i = dynamic_cast<TVTestItem*>(tree->currentItem());
    QString url = i->testState->getTestRef()->getURL();
    QFile f(url);
    bool ok = f.open(QIODevice::WriteOnly);
    if (!ok) {
        coreLog.error(QString("test file %1 can not be opened").arg(url));
        return;
    }
    QString s = testEdit->toPlainText();
    QString tempString = testEdit->toPlainText();
    tempString.replace("&lt;", "<");
    tempString.replace("&gt;", ">");
    f.write(tempString.toUtf8());

    f.close();
}

//////////////////////////////////////////////////////////////////////////
// tree items;

TVTSItem::TVTSItem(GTestSuite* _ts)
    : TVItem(TVItem_TestSuite), ts(_ts) {
    name = "/";
    updateVisual();
    if (!ts->getExcludedTests().empty()) {
        setForeground(0, QBrush(QColor(Qt::darkYellow)));
    }
}

TVTSItem::TVTSItem(const QString& _name)
    : TVItem(TVItem_TestSuite), name(_name) {
    ts = nullptr;
    updateVisual();
}

void TVTSItem::updateVisual() {
    QString text0;
    if (!ts) {
        text0 = name;
    } else {
        text0 = ts->getName();
        setToolTip(0, ts->getURL());
    }
    setText(0, text0);

    int notrun = 0;
    int passed = 0;
    int failed = 0;
    int excluded = 0;
    getTestsState(&passed, &failed, &notrun, &excluded);
    int total = passed + failed + notrun + excluded;

    QString text1;
    text1 += "T:" + QString::number(total) + " [";
    if (excluded != total) {
        text1 += " P:" + QString::number(passed);
        text1 += " F:" + QString::number(failed);
        text1 += " N:" + QString::number(notrun);
    }
    if (excluded) {
        text1 += " X:" + QString::number(excluded);
    }
    text1 += " ]";

    setText(1, text1);
    // add icon
    if (failed) {
        this->setIcon(0, ICON_FAILD_DIR);
    } else {
        if (notrun) {
            this->setIcon(0, ICON_NOTRUN_DIR);
        } else if (passed) {
            this->setIcon(0, ICON_SUCCES_DIR);
        } else {
            this->setIcon(0, ICON_NOTRUN_DIR);
        }
    }
    if (this->parent() != nullptr) {
        (static_cast<TVTSItem*>(parent()))->updateVisual();
    }
}

QString TVTSItem::getRichDesc() const {
    // todo:
    if (ts != nullptr) {
        return ts->getName();
    } else {
        return getURL();
    }
}

void TVTSItem::getTestsState(int* rPassed, int* rFailed, int* rNone, int* excluded) {
    int total = 0;
    int passed = 0;
    int failed = 0;
    int exclud = 0;
    for (int i = 0; i < childCount(); i++) {
        auto item = static_cast<TVItem*>(child(i));
        if (item->isTest()) {
            total++;
            auto tItem = static_cast<TVTestItem*>(item);
            GTestState* testState = tItem->testState;
            if (tItem->isExcluded) {
                exclud++;
            } else if (testState->isFailed()) {
                failed++;
            } else if (testState->isPassed()) {
                passed++;
            }
        } else {
            assert(item->isSuite());
            auto tItem = static_cast<TVTSItem*>(item);
            tItem->getTestsState(rPassed, rFailed, rNone, excluded);
        }
    }
    *rPassed = *rPassed + passed;
    *rFailed = *rFailed + failed;
    *excluded = *excluded + exclud;
    *rNone = (((*rNone + total) - passed) - failed) - exclud;
}

QString TVTSItem::getURL() const {
    if (ts != nullptr) {
        return name;
    } else {
        return ((static_cast<TVTSItem*>(parent()))->getURL() + name + "/");
    }
}
//---------------------------------------------------------------------------
TVTestItem::TVTestItem(GTestState* _t)
    : TVItem(TVItem_Test), testState(_t) {
    updateVisual();
}

TVTestItem::~TVTestItem() {
    delete testState;
}

static QString getStateName(const GTestState* testState) {
    if (testState->isPassed()) {
        return TestViewController::tr("passed");
    }
    if (testState->isFailed()) {
        return TestViewController::tr("failed");
    }
    return TestViewController::tr("not_run");
}

void TVTestItem::updateVisual() {
    QString name = testState->getTestRef()->getShortName().section('/', -1);
    setText(0, name);
    setToolTip(0, testState->getTestRef()->getURL());
    if (this->isExcluded) {
        setText(1, QString("excluded(%1)").arg(this->excludeReason));
        this->setForeground(1, Qt::blue);
        this->setIcon(0, ICON_NOTRUN_TEST);
    } else {
        setText(1, getStateName(testState));

        // add icon
        this->setForeground(1, Qt::black);
        this->setIcon(0, ICON_NOTRUN_TEST);
        if (testState->isFailed()) {
            this->setForeground(1, Qt::red);
            this->setIcon(0, ICON_FAILD_TEST);
            // log.info(QString("##teamcity[testFailed name='%1 : %2' message='%3' details='%3']").arg(testState->getTestRef()->getSuite()->getName(),testState->getTestRef()->getShortName(),QString(testState->getErrorMessage()).replace("'","|'")));
            // log.info(QString("##teamcity[testFinished name='%1 : %2']").arg(testState->getTestRef()->getSuite()->getName(),testState->getTestRef()->getShortName()));
        }
        if (testState->isPassed()) {
            this->setForeground(1, Qt::darkGreen);
            this->setIcon(0, ICON_SUCCES_TEST);
            // log.info(QString("##teamcity[testFinished name='%1 : %2']").arg(testState->getTestRef()->getSuite()->getName(),testState->getTestRef()->getShortName()));
        }
    }
}

QString TVTestItem::getRichDesc() const {
    QString text = "<b>" + testState->getTestRef()->getShortName() + "</b><br>";
    text += "<b>" + TestViewController::tr("state: ") + "</b>" + getStateName(testState) + "<br>";
    if (testState->isFailed()) {
        text += "<b>" + TestViewController::tr("fail_desc: ") + "</b>" + testState->getErrorMessage() + "<br>";
    }

    text += "<b>" + TestViewController::tr("source_file:") + "</b>" + testState->getTestRef()->getURL() + "<br>";
    return text;
}

QString TVTestItem::getTestContent() const {
    QString text;

    QFile myFile(testState->getTestRef()->getURL());
    if (myFile.open(QIODevice::ReadOnly)) {
        QTextStream textStream(&myFile);
        while (!textStream.atEnd()) {
            QString tempString = textStream.readLine();
            tempString.replace("<", "&lt;");
            tempString.replace(">", "&gt;");
            text += tempString + "<br>";
        }
        // Close the file
        myFile.close();
    }

    return text;
}

}  // namespace U2
