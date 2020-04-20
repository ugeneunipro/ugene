#include "calcguitests.h"

#include <primitives/GTAction.h>
#include <primitives/GTWidget.h>
#include <drivers/GTMouseDriver.h>

#include <QToolButton>
#include <QLineEdit>
#include <QApplication>

CalcGUITestsLauncher::CalcGUITestsLauncher(): GUITestsLauncher()
{
    guiTestBase.registerTest(new Test1());
    guiTestBase.registerTest(new Test2());
}

TEST_CLASS_DEFINITION(Test1){
    GTGlobals::sleep();

    QToolButton* button_2 = qobject_cast<QToolButton*>(GTWidget::findButtonByText(os,"2"));

    QToolButton* button_plus = qobject_cast<QToolButton*>(GTWidget::findWidget(os,"plus"));

    QToolButton* button_equal = qobject_cast<QToolButton*>(GTWidget::findButtonByText(os,"="));

    GTWidget::click(os,button_2);
    //GTButton::click(os,"2");

    GTWidget::click(os,button_plus);

    GTWidget::click(os,button_2);

    GTWidget::click(os,button_equal);

    QLineEdit* display = qobject_cast<QLineEdit*>(GTWidget::findWidget(os,"display"));

    CHECK_SET_ERR(display->text() == "4", "result is not equal 4");
}


TEST_CLASS_DEFINITION(Test2){
    GTGlobals::sleep();

    GTWidget::click(os, GTWidget::findButtonByText(os,"2"));

    GTWidget::click(os, GTWidget::findWidget(os,"plus"));

    GTWidget::click(os, GTWidget::findButtonByText(os,"3"));

    GTWidget::click(os, GTWidget::findButtonByText(os,"="));

    QLineEdit* display = qobject_cast<QLineEdit*>(GTWidget::findWidget(os,"display"));

    CHECK_SET_ERR(display->text() == "5", "result is not equal 5");
}
