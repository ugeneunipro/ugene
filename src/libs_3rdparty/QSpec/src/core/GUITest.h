#ifndef _HI_GUI_TEST_H_
#define _HI_GUI_TEST_H_

#include <QTimer>

#include "GTGlobals.h"
#include "core/GUITestOpStatus.h"

namespace HI {

class GUITestIgnorable {
public:
    // not ignored test, ignored by all, ignored on windows platforms, ignored on linux platforms
    enum IgnoreStatus {NotIgnored=0x0, Ignored=0x1, IgnoredWindows=0x2, IgnoredLinux=0x4, IgnoredMac=0x8};
    enum IgnoreReason {Bug, System};

    GUITestIgnorable() : ignoreStatus(NotIgnored), ignoreMessage(""), ignoreReason(Bug) {}

    void setIgnored(int status, const QString& message = "") { ignoreStatus = status; ignoreMessage = message; ignoreReason = Bug;}
    void setReason(IgnoreReason _reason){ ignoreReason = _reason; }
    int getIgnoreStatus() const {return ignoreStatus; }
    QString getIgnoreMessage() const {return ignoreMessage; }

    bool isIgnored() const {
        bool ignored = ignoreStatus & Ignored;
        bool platformIgnore = false;

#ifdef _WIN32
        platformIgnore = (ignoreStatus & IgnoredWindows);
#endif

#ifdef __linux__
        platformIgnore = (ignoreStatus & IgnoredLinux);
#endif

#ifdef Q_OS_MAC
        platformIgnore = (ignoreStatus & IgnoredMac);
#endif

        return ignored || platformIgnore;
    }
    IgnoreReason getReason(){return ignoreReason;}

private:
    int ignoreStatus;
    QString ignoreMessage;
    IgnoreReason ignoreReason;
};

class HI_EXPORT GUITest: public QObject, public GUITestIgnorable {
    Q_OBJECT
public:
    GUITest(const QString &_name = "", const QString &_suite = "", int _timeout=240000, QString _label=""):
        name(_name), suite(_suite), timeout(_timeout), label(_label) {}
    virtual ~GUITest(){}

    QString getName() const { return name; }
    QString getSuite() const { return suite; }
    QString getFullName() const { return suite + ":" + name; }
    QString getLabel() const {return label;}
    int getTimeout() const { return timeout; }
    void setName(const QString &n) { name = n; }
    void setTimeout(int _timeout) { timeout = _timeout; }
    void setLabel(QString _label) { label = _label; }

    static const QString screenshotDir;

    virtual void run(GUITestOpStatus &os) = 0;
    virtual void cleanup() {}

private slots:
    void sl_fail();

private:
    GUITest(const GUITest&);
    GUITest& operator=(const GUITest&);

    QString name;
    QString suite;
    int timeout;
    QString label;
};

typedef QList<GUITest*> GUITests;

#define TESTNAME(className) #className
#define SUITENAME(className) QString(GUI_TEST_SUITE)

#define TEST_CLASS_DECLARATION(className) \
    class className : public HI::GUITest { \
    public: \
        className () : HI::GUITest(TESTNAME(className), SUITENAME(className)){} \
    protected: \
        virtual void run(HI::GUITestOpStatus &os); \
    };

#define TEST_CLASS_DECLARATION_SET_TIMEOUT(className, timeout) \
    class className : public HI::GUITest { \
    public: \
        className () : HI::GUITest(TESTNAME(className), SUITENAME(className), timeout){} \
    protected: \
        virtual void run(HI::GUITestOpStatus &os); \
    };

#define TEST_CLASS_DEFINITION(className) \
    void className::run(HI::GUITestOpStatus &os)


} //HI

#endif
