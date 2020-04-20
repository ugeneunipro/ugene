#ifndef _HI_GUI_TEST_BASE_H_
#define _HI_GUI_TEST_BASE_H_

#include <QtGui>
#include "core/GUITest.h"

namespace HI {

typedef QMap<QString, HI::GUITest*> GUITestMap;

class HI_EXPORT GUITestBase {
public:

    virtual ~GUITestBase();

    bool registerTest(GUITest *test);
    GUITest *getTest(const QString &suite, const QString &name);
    GUITest *takeTest(const QString &suite, const QString &name); // removes item from GUITestBase

    GUITests getTests();
    GUITests takeTests(); // removes items from GUITestBase

    GUITest *findTest(const QString &name);
    bool containsTest(const QString &name);

    static const QString unnamedTestsPrefix;

private:
    GUITestMap tests;

    bool isNewTest(GUITest *test);
    void addTest(GUITest *test);

    QString getNextTestName();
    QString nameUnnamedTest(GUITest* test);
};

}

#endif
