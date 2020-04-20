#include "TestThread.h"

namespace HI {

void TestThread::run()
{
    try {
        test->run(os);
    } catch (GUITestOpStatus *) {

    }
}

}
