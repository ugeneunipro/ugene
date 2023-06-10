#ifndef TESTTHREAD_H
#define TESTTHREAD_H

#include <QThread>

#include "core/GUITest.h"
#include "core/GUITestOpStatus.h"

namespace HI {

class HI_EXPORT TestThread : public QThread {
public:
    TestThread(GUITest* _test)
        : test(_test) {
    }

public:
    void run() override;

private:
    GUITest* test;
    GUITestOpStatus os;
};

}  // namespace HI

#endif  // TESTTHREAD_H
