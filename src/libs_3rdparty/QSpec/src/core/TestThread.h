#ifndef TESTTHREAD_H
#define TESTTHREAD_H

#include <QThread>

#include "core/GUITest.h"
#include "core/GUITestOpStatus.h"

namespace HI {

class HI_EXPORT TestThread : public QThread {
public:
    TestThread(GUITest* test, GUITestOpStatus& os)
        : test(test), os(os) {
    }

public:
    virtual void run();

private:
    GUITest* test;
    GUITestOpStatus os;
};

}  // namespace HI

#endif  // TESTTHREAD_H
