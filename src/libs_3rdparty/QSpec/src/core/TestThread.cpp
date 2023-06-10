#include "TestThread.h"

namespace HI {

void TestThread::run() {
    try {
        test->run();
    } catch (GUITestOpStatus*) {
    }
}

}  // namespace HI
