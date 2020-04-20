#ifndef DRIVERSGUITESTS_H
#define DRIVERSGUITESTS_H

#include <core/GUITest.h>
#include <core/GUITestBase.h>
#include <core/GUITestsLauncher.h>
#include <GTGlobals.h>

#include <QThread>

using namespace HI;
class driversGuiTestsLauncher: public GUITestsLauncher
{
public:
    driversGuiTestsLauncher();
};
#define GUI_TEST_SUITE "Drivers_gui_test"
    TEST_CLASS_DECLARATION(Test1)
    TEST_CLASS_DECLARATION(Test2)
    TEST_CLASS_DECLARATION(Test3)
    TEST_CLASS_DECLARATION(Test4)
    TEST_CLASS_DECLARATION(Test5)
    TEST_CLASS_DECLARATION(Test6)
    TEST_CLASS_DECLARATION(Test7)
#undef GUI_TEST_SUITE

#endif // DRIVERSGUITESTS_H
