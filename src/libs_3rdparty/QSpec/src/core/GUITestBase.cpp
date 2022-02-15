#include "GUITestBase.h"

namespace HI {

GUITestBase::~GUITestBase() {
    qDeleteAll(tests);
}

bool GUITestBase::registerTest(GUITest* test) {
    Q_ASSERT(test != nullptr && !test->name.isEmpty());
    if (hasTest(test->name)) {
        return false;
    }
    tests.insert(test->getFullName(), test);
    return true;
}

GUITest* GUITestBase::getTest(const QString& fullTestName) const {
    return tests.value(fullTestName);
}

GUITest* GUITestBase::takeTest(const QString& fullTestName) {
    return tests.take(fullTestName);
}

bool GUITestBase::hasTest(const QString& fullTestName) const {
    return tests.contains(fullTestName);
}

GUITest* GUITestBase::getTest(const QString& suiteName, const QString& testName) const {
    return getTest(GUITest::getFullTestName(suiteName, testName));
}

GUITest* GUITestBase::takeTest(const QString& suiteName, const QString& testName) {
    return takeTest(GUITest::getFullTestName(suiteName, testName));
}

QList<GUITest*> GUITestBase::getTests() const {
    return tests.values();
}

QList<GUITest*> GUITestBase::takeTests() {
    QList<GUITest*> testList = getTests();
    tests.clear();
    return testList;
}

}  // namespace HI
