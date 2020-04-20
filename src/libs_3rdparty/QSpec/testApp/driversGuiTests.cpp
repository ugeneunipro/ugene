#include "driversGuiTests.h"
#include "mainwindow.h"
#include "EventFilter.h"
#include "systemTests.h"

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>

#include <QApplication>
#include <QLineEdit>

namespace {
QLineEdit* getTestLine(){
    return MainWindow::getInstance()->findChild<QLineEdit*>("testLine");
}

QWidget* getRecorder(){
    return MainWindow::getInstance()->findChild<QWidget*>("eventRecorder");
}

QPoint getWidgetCenter(QWidget* w){
    return w->mapToGlobal(w->rect().center());
}
}

using namespace HI;
driversGuiTestsLauncher::driversGuiTestsLauncher(): GUITestsLauncher()
{
    guiTestBase.registerTest(new Test1());
    guiTestBase.registerTest(new Test2());
    guiTestBase.registerTest(new Test3());
    guiTestBase.registerTest(new Test4());
    guiTestBase.registerTest(new Test5());
    guiTestBase.registerTest(new Test6());
    guiTestBase.registerTest(new Test7());
    guiTestBase.registerTest(new FilePermissionTest());
    guiTestBase.registerTest(new DirPermissionTest());
}

TEST_CLASS_DEFINITION(Test1){
    QLineEdit* line = getTestLine();
    GTMouseDriver::moveTo(getWidgetCenter(line));
    GTMouseDriver::click();
    GTGlobals::sleep();
    GTKeyboardDriver::keySequence("abcdefghigklmnopqrstuvwxy z");
	CHECK_SET_ERR(line->text() == "abcdefghigklmnopqrstuvwxy z", "expected: abcdefghigklmnopqrstuvwxy z, actual: " + line->text())
	GTGlobals::sleep(500);
}

TEST_CLASS_DEFINITION(Test2){
    QLineEdit* line = getTestLine();
    GTMouseDriver::moveTo(getWidgetCenter(line));
    GTMouseDriver::click();
    //GTGlobals::sleep();
    GTKeyboardDriver::keySequence("0123456789,<.>!@#$%^&*()-_=+[{]};:\'\"/\\|");
    CHECK_SET_ERR(line->text() == "0123456789,<.>!@#$%^&*()-_=+[{]};:\'\"/\\|", "expected: 0123456789,<.>!@#$%^&*()-_=+[{]};:\'\"/\\|, actual: " + line->text())
	GTGlobals::sleep(500);
}

TEST_CLASS_DEFINITION(Test3){
    QWidget* recorder = getRecorder();
    GTMouseDriver::moveTo(getWidgetCenter(recorder));
    GTMouseDriver::click();

    GTKeyboardDriver::keyClick( Qt::Key_Up);
    GTGlobals::sleep(500);

    QList<KeyEvent> list = MainWindow::getInstance()->eventFilter->keyEvents;
    int i = list.size();
    CHECK_SET_ERR(i == 2, QString("unexpected number of key events: %1").arg(i));
    CHECK_SET_ERR(list.first().getKey() == Qt::Key_Up, QString("unexpected first key: %1").arg(list.first().getKey()));
    CHECK_SET_ERR(list.first().getType() == QEvent::KeyPress, QString("unexpected first type: %1").arg(list.first().getType()));
    CHECK_SET_ERR(list.last().getKey() == Qt::Key_Up, QString("unexpected second key: %1").arg(list.last().getKey()));
    CHECK_SET_ERR(list.last().getType() == QEvent::KeyRelease, QString("unexpected second type: %1").arg(list.last().getType()));
}

TEST_CLASS_DEFINITION(Test4){
    QWidget* recorder = getRecorder();
    GTMouseDriver::moveTo(getWidgetCenter(recorder));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick( Qt::Key_Enter, Qt::ControlModifier);
    GTGlobals::sleep(500);

    QList<KeyEvent> list = MainWindow::getInstance()->eventFilter->keyEvents;
    int i = list.size();

    CHECK_SET_ERR(i == 4, QString("unexpected number of key events: %1").arg(i));

    CHECK_SET_ERR(list[0].getKey() == Qt::Key_Control, QString("unexpected 1 key: %1").arg(list[0].getKey()));
    CHECK_SET_ERR(list[0].getType() == QEvent::KeyPress, QString("unexpected 1 type: %1").arg(list[0].getType()));

    CHECK_SET_ERR(list[1].getKey() == Qt::Key_Return, QString("unexpected 2 key: %1").arg(list[1].getKey()));
    CHECK_SET_ERR(list[1].getType() == QEvent::KeyPress, QString("unexpected 2 key: %1").arg(list[1].getType()));

    CHECK_SET_ERR(list[2].getKey() == Qt::Key_Return, QString("unexpected 3 key: %1").arg(list[2].getKey()));
    CHECK_SET_ERR(list[2].getType() == QEvent::KeyRelease, QString("unexpected 3 type: %1").arg(list[2].getType()));

    CHECK_SET_ERR(list[3].getKey() == Qt::Key_Control, QString("unexpected 4 key: %1").arg(list[3].getKey()));
    CHECK_SET_ERR(list[3].getType() == QEvent::KeyRelease, QString("unexpected 4 type: %1").arg(list[3].getType()));
}

TEST_CLASS_DEFINITION(Test5){
    QWidget* recorder = getRecorder();
    GTMouseDriver::moveTo(getWidgetCenter(recorder));
    GTMouseDriver::click();
    GTGlobals::sleep(500);

    QList<MouseEvent> list = MainWindow::getInstance()->eventFilter->mouseEvents;
    int i = list.size();

    CHECK_SET_ERR(i == 2, QString("unexpected number of mouse events: %1").arg(i));

    CHECK_SET_ERR(list[0].getButton() == Qt::LeftButton, QString("unexpected 1 button: %1").arg(list[0].getButton()));
    CHECK_SET_ERR(list[0].getType() == QEvent::MouseButtonPress, QString("unexpected 1 type: %1").arg(list[0].getType()));

    CHECK_SET_ERR(list[1].getButton() == Qt::LeftButton, QString("unexpected 2 button: %1").arg(list[1].getButton()));
    CHECK_SET_ERR(list[1].getType() == QEvent::MouseButtonRelease, QString("unexpected 2 type: %1").arg(list[1].getType()));
}

TEST_CLASS_DEFINITION(Test6){
    QWidget* recorder = getRecorder();
    GTMouseDriver::moveTo(getWidgetCenter(recorder));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(500);

    QList<MouseEvent> list = MainWindow::getInstance()->eventFilter->mouseEvents;
    int i = list.size();

    CHECK_SET_ERR(i == 2, QString("unexpected number of mouse events: %1").arg(i));

    CHECK_SET_ERR(list[0].getButton() == Qt::RightButton, QString("unexpected 1 button: %1").arg(list[0].getButton()));
    CHECK_SET_ERR(list[0].getType() == QEvent::MouseButtonPress, QString("unexpected 1 type: %1").arg(list[0].getType()));

    CHECK_SET_ERR(list[1].getButton() == Qt::RightButton, QString("unexpected 2 button: %1").arg(list[1].getButton()));
    CHECK_SET_ERR(list[1].getType() == QEvent::MouseButtonRelease, QString("unexpected 2 type: %1").arg(list[1].getType()));
}

TEST_CLASS_DEFINITION(Test7){
    QWidget* recorder = getRecorder();
    GTMouseDriver::moveTo(getWidgetCenter(recorder));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep(500);

    QList<MouseEvent> list = MainWindow::getInstance()->eventFilter->mouseEvents;
    int i = list.size();

    CHECK_SET_ERR(i == 2, QString("unexpected number of mouse events: %1").arg(i));

    CHECK_SET_ERR(list[0].getButton() == Qt::LeftButton, QString("unexpected 1 button: %1").arg(list[0].getButton()));
    CHECK_SET_ERR(list[0].getType() == QEvent::MouseButtonPress, QString("unexpected 1 type: %1").arg(list[0].getType()));

    CHECK_SET_ERR(list[1].getButton() == Qt::LeftButton, QString("unexpected 2 button: %1").arg(list[1].getButton()));
    CHECK_SET_ERR(list[1].getType() == QEvent::MouseButtonRelease, QString("unexpected 2 type: %1").arg(list[1].getType()));
}
