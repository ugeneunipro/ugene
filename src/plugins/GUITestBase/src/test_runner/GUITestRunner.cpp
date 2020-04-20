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

#include <QTreeWidgetItem>
#include <QTreeView>
#include <QMessageBox>

#include <U2Core/Settings.h>
#include <U2Core/AppContext.h>

#include <U2Test/GUITestThread.h>
#include <U2Test/UGUITestBase.h>
#include <U2Test/GUITestService.h>

#include "GUITestRunner.h"

namespace U2 {

#define LAST_FILTER_SETTING_NAME QString("GUI_TEST_RUNNER/last-filter")

#define ULOG_CAT_TEST_RUNNER "GUI Test Runner Log"
static Logger log(ULOG_CAT_TEST_RUNNER);

GUITestRunner::GUITestRunner(UGUITestBase* _guiTestBase, QWidget *parent) :
    QWidget(parent),
    guiTestBase(_guiTestBase)
{
    setupUi(this);
    setWindowIcon(QIcon(QString(":gui_test/images/open_gui_test_runner.png")));
    setAttribute(Qt::WA_DeleteOnClose);

    tree->setColumnWidth(0,550);

    const GUITests tests = guiTestBase->getTests();
    foreach(HI::GUITest* t, tests) {
        //addTestSuite(ts);
        QStringList list = QStringList();
        QString suiteName = t->getSuite();
        QList<QTreeWidgetItem*> suites = tree->findItems(suiteName, Qt::MatchExactly);
        if (!suites.isEmpty()){
            if (suites.size() == 1){
                list.append(t->getName());
                list.append("Not run");
                suites.first()->addChild(new QTreeWidgetItem(list));
            }
        }else{
            list.append(t->getSuite());
            tree->addTopLevelItem(new QTreeWidgetItem(list));
            QList<QTreeWidgetItem*> suites = tree->findItems(suiteName, Qt::MatchExactly);
            list.removeFirst();
            list.append(t->getName());
            list.append("Not run");
            suites.first()->addChild(new QTreeWidgetItem(list));
        }
    }

    delTextAction = new QAction(this);
    delTextAction->setShortcut(QKeySequence(tr("Esc")));
    filter->addAction(delTextAction);

    QString lastUsedFilter = AppContext::getSettings()->getValue(LAST_FILTER_SETTING_NAME, "").toString();
    filter->setText(lastUsedFilter);

    connect(delTextAction, SIGNAL(triggered()), this, SLOT(sl_filterCleared()));

    connect(filter, SIGNAL(textChanged(const QString &)), this, SLOT(sl_filterChanged(const QString &)));

    connect(startSelectedButton,SIGNAL(clicked(bool)),this,SLOT(sl_runSelected()));
    connect(tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(sl_runSelected()));

    connect(startAllButton,SIGNAL(clicked(bool)),this,SLOT(sl_runAllGUITests()));

    show();
    filter->setFocus();

    revisible(filter->text());
}

GUITestRunner::~GUITestRunner()
{

}

void GUITestRunner::sl_runSelected(){
    GUITestService::setEnvVariablesForGuiTesting();
    QList<QTreeWidgetItem*> selectedItems = tree->selectedItems();
    foreach (QTreeWidgetItem* item, selectedItems) {
        if(item->childCount() == 0){ // single test, not suite
            QString suite = item->parent()->text(0);
            QString name = item->text(0);
            HI::GUITest* test = guiTestBase->getTest(suite, name);
            GUITestThread *testThread = new GUITestThread(test, log, false);
            connect(testThread, SIGNAL(finished()), this, SLOT(sl_testFinished()));
            hide();
            testThread->start();

        }

    }
}
void GUITestRunner::sl_runAllGUITests(){
    GUITestService::setEnvVariablesForGuiTesting();
    if(GUITestService::getGuiTestService()->isEnabled()){
        hide();
        GUITestService::getGuiTestService()->runAllGUITests();
        show();
    }
}

void GUITestRunner::sl_testFinished(){
    GUITestThread *testThread = qobject_cast<GUITestThread *>(sender());
    //SAFE_POINT(NULL != testThread, "TestThread is null", );
    HI::GUITest* test = testThread->getTest();
    QString result = testThread->getTestResult();
    for (int suiteIdx = 0; suiteIdx < tree->topLevelItemCount(); suiteIdx++) {
        QTreeWidgetItem *suite = tree->topLevelItem(suiteIdx);
        if(test->getSuite() == suite->text(0)){
            for (int testIdx = 0; testIdx < suite->childCount(); testIdx++) {
                QTreeWidgetItem *testItem = suite->child(testIdx);
                if(testItem->text(0) == test->getName()){
                    testItem->setText(1,result);
                }
            }
        }
    }
    show();
}

////Filter

void GUITestRunner::sl_filterCleared(){
    filter->clear();
    tree->collapseAll();
}
void GUITestRunner::sl_filterChanged(const QString &nameFilter) {
    AppContext::getSettings()->setValue(LAST_FILTER_SETTING_NAME, nameFilter);
    revisible(nameFilter);
}

namespace {
bool filterMatched(const QString &nameFilter, const QString &name) {
    static QRegExp spaces("\\s");
    QStringList filterWords = nameFilter.split(spaces);
    foreach (const QString &word, filterWords) {
        if (!name.contains(word, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}
}
void GUITestRunner::revisible(const QString &nameFilter) {
    setMouseTracking(false);
    for (int catIdx=0; catIdx<tree->topLevelItemCount(); catIdx++) {
        QTreeWidgetItem *category = tree->topLevelItem(catIdx);
        bool hasVisibleSamples = false;
        QString catName = category->text(0);
        for (int childIdx=0; childIdx<category->childCount(); childIdx++) {
            QTreeWidgetItem *sample = category->child(childIdx);
            QString name = sample->text(0);
            if (!filterMatched(nameFilter, name) &&
                !filterMatched(nameFilter, catName)) {
                sample->setHidden(true);
            } else {
                sample->setHidden(false);
                hasVisibleSamples = true;
            }
        }
        category->setHidden(!hasVisibleSamples);
        category->setExpanded(hasVisibleSamples);
    }
    setMouseTracking(true);
}

}
