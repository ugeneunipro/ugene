#include "mainwindow.h"
#include "driversGuiTests.h"
#include <QApplication>
#include <QLineEdit>
#include <QTime>

#include <drivers/GTMouseDriver.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* w = MainWindow::getInstance();
    w->show();

    QString runGuiTest = qgetenv("HI_GUI_TEST");
    if (!runGuiTest.isEmpty()) {

        GUITestsLauncher* launcher = new driversGuiTestsLauncher();
        QTimer::singleShot(1000, launcher, SLOT(sl_runTest()));
    }


    return a.exec();
}
