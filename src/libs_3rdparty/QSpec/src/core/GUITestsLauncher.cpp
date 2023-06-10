#include "GUITestsLauncher.h"

#include "core/TestThread.h"

namespace HI {

void GUITestsLauncher::sl_runTest() {
    QString fullTestName = qgetenv("HI_GUI_TEST");  // Format is "Suite_name:Test_name"
    GUITest* test = guiTestBase.getTest(fullTestName);
    if (test == nullptr) {
        qCritical("Test not found: %s", fullTestName.toLocal8Bit().constData());
        qApp->exit(1);
    }

    auto testThread = new TestThread(test);
    connect(testThread, SIGNAL(finished()), this, SLOT(sl_onTestFinished()));
    testThread->start();
}
void GUITestsLauncher::sl_onTestFinished() {
    sender()->deleteLater();

    if (GTGlobals::getOpStatus().hasError()) {
        qCritical("Test failed: %s", GTGlobals::getOpStatus().getError().toLocal8Bit().constData());
        qApp->exit(1);
    } else {
        qDebug("Success");
        qApp->exit(0);
    }
}

}  // namespace HI
