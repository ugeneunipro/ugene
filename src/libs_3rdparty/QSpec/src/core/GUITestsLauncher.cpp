#include "GUITestsLauncher.h"

#include "core/TestThread.h"

namespace HI {

void GUITestsLauncher::sl_runTest(){
    QString guiTestName = qgetenv("HI_GUI_TEST");// Format is "Suite_name:Test_name"

    if(!guiTestBase.containsTest(guiTestName)){
        qCritical("Test not found: %s", guiTestName.toLocal8Bit().constData());
        qApp->exit(1);
    }

    TestThread* testThread = new TestThread(guiTestBase.findTest(guiTestName),os);
    connect(testThread, SIGNAL(finished()), this, SLOT(sl_onTestFinished()));

    testThread->start();
}
void GUITestsLauncher::sl_onTestFinished(){
    sender()->deleteLater();

    if (os.hasError()){
        qCritical("Test failed: %s", os.getError().toLocal8Bit().constData());
        qApp->exit(1);
    }else {
        qDebug("Success");
        qApp->exit(0);
    }
}


}
