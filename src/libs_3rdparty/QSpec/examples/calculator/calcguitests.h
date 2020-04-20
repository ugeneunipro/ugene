#ifndef CALCGUITESTS_H
#define CALCGUITESTS_H

#include <core/GUITest.h>
#include <core/GUITestBase.h>
#include <core/GUITestsLauncher.h>
#include <GTGlobals.h>
#include "calculator.h"

#include <QThread>

using namespace HI;
class CalcGUITestsLauncher: public GUITestsLauncher
{
public:
    CalcGUITestsLauncher();
};

#define GUI_TEST_SUITE "Calc_base_tests"
    TEST_CLASS_DECLARATION(Test1)
    TEST_CLASS_DECLARATION(Test2)
#undef GUI_TEST_SUITE

#endif // CALCGUITESTS_H
