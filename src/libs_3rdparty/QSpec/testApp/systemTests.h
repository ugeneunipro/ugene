#ifndef SYSTEMTESTS_H
#define SYSTEMTESTS_H

#include <core/GUITest.h>
#include <core/GUITestBase.h>
#include <GTGlobals.h>

using namespace HI;

#define GUI_TEST_SUITE "System_test"
    TEST_CLASS_DECLARATION(FilePermissionTest)
    TEST_CLASS_DECLARATION(DirPermissionTest)
#undef GUI_TEST_SUITE

#endif // SYSTEMTESTS_H
