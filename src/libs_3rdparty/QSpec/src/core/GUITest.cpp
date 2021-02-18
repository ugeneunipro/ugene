#include "GUITest.h"

#include <QApplication>
#include <QDate>
#include <QDesktopWidget>
#include <QDir>
#include <QScreen>

#include "core/GUITestOpStatus.h"

namespace HI {

QString getScreenshotDir() {    //TODO:rewrite to use working directory /screenshots
    QString result;
#ifdef Q_OS_MAC
    result = "../../../../../../screenshotFol/";
#else
    QString guiTestOutputDirectory = qgetenv("GUI_TESTING_OUTPUT");
    if (guiTestOutputDirectory.isEmpty()) {
        result = QDir::homePath() + "/gui_testing_output/" +
                 QDate::currentDate().toString("dd.MM.yyyy") + "/screenshots/";
    } else {
        result = guiTestOutputDirectory + "/gui_testing_output/" +
                 QDate::currentDate().toString("dd.MM.yyyy") + "/screenshots/";
    }
#endif
    return result;
}

const QString GUITest::screenshotDir = getScreenshotDir();

}    // namespace HI
