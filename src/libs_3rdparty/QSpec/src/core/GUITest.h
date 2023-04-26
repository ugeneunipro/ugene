#ifndef _HI_GUI_TEST_H_
#define _HI_GUI_TEST_H_

#include <QSet>
#include <QTimer>

#include "GTGlobals.h"
#include "core/GUITestOpStatus.h"

namespace HI {

class HI_EXPORT GUITest : public QObject {
    Q_OBJECT
public:
    GUITest(const QString& name, const QString& suite, int timeout, const QSet<QString>& labelSet = QSet<QString>())
        : name(name), suite(suite), timeout(timeout), labelSet(labelSet) {
    }
    virtual ~GUITest() {
    }

    /** Returns full test name: suite + testName. */
    QString getFullName() const {
        return getFullTestName(suite, name);
    }

    /** Screenshot dir for tests. TODO: this field is not used inside QSpec. Move it to the correct module. */
    static const QString screenshotDir;

    /** Scenario of the test. Must be implemented in the child class. */
    virtual void run(GUITestOpStatus& os) = 0;

    /** Post-run cleanup. Optional. */
    virtual void cleanup() {
    }

    /** Unique GUI test name within the suite. */
    const QString name;

    /** Name of the GUI test suite. */
    const QString suite;

    /** Timeout millis for the test. The test execution is interrupted if the test runs above that limit. */
    const int timeout;

    /** Set of test labels. */
    QSet<QString> labelSet;

    static QString getFullTestName(const QString& suiteName, const QString& testName) {
        return suiteName + ":" + testName;
    }

    /** See docs for the field. */
    const QString& getDescription() const {
        return description;
    }

    /** See docs for the field. */
    void setDescription(const QString& newDescription) {
        description = newDescription;
    }

private:
    GUITest(const GUITest&);
    GUITest& operator=(const GUITest&);

    /** Extra description about the test available to the test runner (example: Teamcity). */
    QString description;
};

typedef QList<GUITest*> GUITests;

#define TESTNAME(className) #className
#define SUITENAME(className) QString(GUI_TEST_SUITE)

#define TEST_CLASS_DECLARATION(className) \
    class className : public HI::GUITest { \
    public: \
        className() : HI::GUITest(TESTNAME(className), SUITENAME(className)) { \
        } \
\
    protected: \
        virtual void run(HI::GUITestOpStatus& os); \
    };

#define TEST_CLASS_DECLARATION_SET_TIMEOUT(className, timeout) \
    class className : public HI::GUITest { \
    public: \
        className() : HI::GUITest(TESTNAME(className), SUITENAME(className), timeout) { \
        } \
\
    protected: \
        virtual void run(HI::GUITestOpStatus& os); \
    };

#define TEST_CLASS_DEFINITION(className) \
    void className::run(HI::GUITestOpStatus& os)

}  // namespace HI

#endif
