#ifndef _HI_GUI_TEST_BASE_H_
#define _HI_GUI_TEST_BASE_H_

#include <QtGui>

#include "core/GUITest.h"

namespace HI {

typedef QMap<QString, HI::GUITest *> GUITestMap;

class HI_EXPORT GUITestBase {
public:
    virtual ~GUITestBase();

    /**
     * Registers new test. Returns true if the test was registered successfully
     * or false if there is another test registered with the given name.
     */
    bool registerTest(GUITest *test);

    /** Finds registered test by the full test name and returns the test or nullptr. */
    GUITest *getTest(const QString &fullTestName) const;

    /** Same as getTest but also removes the tests from the test-base. */
    GUITest *takeTest(const QString &fullTestName);

    /** Returns true if the test suite contains the test with the given full test name. */
    bool hasTest(const QString &fullTestName) const;

    /** Finds and returns a test matched registered by the full test name: suite + name. */
    GUITest *getTest(const QString &suiteName, const QString &testName) const;

    /** Same as getTest but also removes the tests from the test-base. */
    GUITest *takeTest(const QString &suiteName, const QString &testName);

    /** Returns list of all registered tests. */
    QList<GUITest *> getTests() const;

    /** Returns list of all registered tests and clears the internal list of tests. */
    QList<GUITest *> takeTests();

protected:
    GUITestMap tests;
};

}    // namespace HI

#endif
