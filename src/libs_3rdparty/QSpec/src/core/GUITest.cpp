#include "GUITest.h"
#include "core/GUITestOpStatus.h"

#include <QApplication>
#include <QDate>
#include <QDesktopWidget>
#include <QDir>
#include <QScreen>

namespace HI {

QString getScreenshotDir(){ //TODO:rewrite to use working directory /screenshots
    QString result;
#ifdef Q_OS_MAC
    result = "../../../../../../screenshotFol/";
#else
    QString guiTestOutputDirectory = qgetenv("GUI_TESTING_OUTPUT");
    if(guiTestOutputDirectory.isEmpty()){
        result = QDir::homePath() + "/gui_testing_output/" +
                QDate::currentDate().toString("dd.MM.yyyy") + "/screenshots/";
    }else{
        result = guiTestOutputDirectory + "/gui_testing_output/" +
                QDate::currentDate().toString("dd.MM.yyyy") + "/screenshots/";
    }
#endif
    return result;
}

const QString GUITest::screenshotDir = getScreenshotDir();

void GUITest::sl_fail(){
#if (QT_VERSION < 0x050000) // deprecated method
    QPixmap originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
#else
    QPixmap originalPixmap = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
#endif
    originalPixmap.save(GUITest::screenshotDir + name + ".jpg");
    qCritical("GUItest timed out");
    qCritical("\nGT_DEBUG_MESSAGE !!!FIRST FAIL");
    GUITestOpStatus os;// = new GUITestOpStatus();
    os.setError("time out");
}



} // namespace
