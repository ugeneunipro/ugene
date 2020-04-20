#QSpec [![Build Status](https://travis-ci.org/ugeneunipro/QSpec.svg?branch=master)]	(https://travis-ci.org/ugeneunipro/QSpec)

## Introduction
`QSpec` is a GUI testing library for Qt desktop applications.

## Writing Tests
You need to create declarations for your tests like the following:
```cpp
#include <core/GUITest.h>
using namespace HI;
#define GUI_TEST_SUITE "My_base_tests"
    TEST_CLASS_DECLARATION(Test1)
    TEST_CLASS_DECLARATION(Test2)
#undef GUI_TEST_SUITE
```
Also you need to create definitions of the tests:
```cpp
TEST_CLASS_DEFINITION(Test1){
    //body of test
}

TEST_CLASS_DEFINITION(Test2){
    //body of test
}
```
After this you need to implement `GUITestsLauncher` interface:
```cpp
#include <core/GUITestsLauncher.h>
using namespace HI;
class MyGUITestsLauncher: public GUITestsLauncher
{
public:
    MyGUITestsLauncher();
};
```
```cpp
MyGUITestsLauncher::MyGUITestsLauncher(): GUITestsLauncher()
{
    guiTestBase.registerTest(new Test1());
    guiTestBase.registerTest(new Test2());
}
```
In the main function of your application you need to add something like this:
```cpp
    QString runGuiTest = qgetenv("HI_GUI_TEST");
    if (!runGuiTest.isEmpty()) {
        MyGUITestsLauncher* tests = new MyGUITestsLauncher();
        tests->connect(&myMainWindow,SIGNAL(si_applicationStarted()),tests,SLOT(sl_runTest()));
    }
```

## Running Tests
A single test can be run just using 'HI_GUI_TEST' environment variable.

First set the variable.

For Unix systems:
```bash
export HI_GUI_TEST=<suite_name>:<test_name>
```

For Windows systems:
```bat
set HI_GUI_TEST=<suite_name>:<test_name>
```
Then run the application.

## Requirements

Qt5

* Linux:
  * X11 Record extension library (sudo apt-get install libxtst-dev)

## Development and contributions

See more info [here](https://github.com/ugeneunipro/QSpec/wiki).

## License

This project is available under GNU GPL V2 license.
