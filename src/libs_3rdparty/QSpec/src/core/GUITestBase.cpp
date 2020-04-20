#include "GUITestBase.h"

namespace HI {

const QString GUITestBase::unnamedTestsPrefix = "test";

GUITestBase::~GUITestBase() {

    qDeleteAll(tests);
}

bool GUITestBase::registerTest(GUITest *test) {

    Q_ASSERT(test);

    test->setName(nameUnnamedTest(test));

    if (isNewTest(test)) {
        addTest(test);
        return true;
    }

    return false;
}

QString GUITestBase::nameUnnamedTest(GUITest* test) {

    Q_ASSERT(test);
    if (!test) {
        return "";
    }

    QString testName = test->getName();
    if (testName.isEmpty()) {
        testName = getNextTestName();
    }

    return testName;
}

bool GUITestBase::isNewTest(GUITest *test) {

    return test && !findTest(test->getFullName());
}

void GUITestBase::addTest(GUITest *test) {

    if (test) {
        tests.insert(test->getFullName(), test);
    }
}

QString GUITestBase::getNextTestName() {

    int testsCount = tests.size();
    return unnamedTestsPrefix + QString::number(testsCount);
}

GUITest *GUITestBase::findTest(const QString &name) {
    return tests.value(name);
}
bool GUITestBase::containsTest(const QString &name) {
    return tests.contains(name);
}

GUITest *GUITestBase::getTest(const QString &suite, const QString &name) {

    return tests.value(suite + ":" + name);
}

GUITest *GUITestBase::takeTest(const QString &suite, const QString &name) {

    return tests.take(suite + ":" + name);
}

GUITests GUITestBase::getTests() {

    GUITests testList = tests.values();

    return testList;
}

GUITests GUITestBase::takeTests() {

    GUITests testList = tests.values();
    tests.clear();

    return testList;
}


} // namespace
