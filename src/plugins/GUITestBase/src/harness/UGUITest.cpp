/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <core/GUITestOpStatus.h>

#include <QApplication>
#include <QDate>
#include <QDesktopWidget>
#include <QDir>
#include <QScreen>

#include <U2Core/BundleInfo.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include "UGUITest.h"

namespace U2 {

static QString getTestDirImpl() {
    const QString testDir = qgetenv("UGENE_TESTS_PATH");
    if (!testDir.isEmpty()) {
        if (!QFileInfo(testDir).exists()) {
            coreLog.error(QString("UGENE_TESTS_PATH is defined, but doesn't exist: '%1'").arg(testDir));
        } else {
            return testDir + (testDir.endsWith("/") ? "" : "/");
        }
    }

    bool ok;
    int i = qgetenv("UGENE_GUI_TEST_SUITE_NUMBER").toInt(&ok);
    if (isOsMac()) {
        if (ok && i > 1) {
            return QString("../../../../../../test%1/").arg(i - 1);
        } else {
            return QString("../../../../../../test/");
        }
    } else {
        if (ok && i > 1) {
            return QString("../../test%1/").arg(i - 1);
        } else {
            return QString("../../test/");
        }
    }
}

static QString getTestDir() {
    QString result = getTestDirImpl();
    if (qgetenv("UGENE_GUI_TEST") == "1") {  // In gui test mode dump test & data dir.
        qDebug("Test dir: '%s' -> '%s'", result.toLocal8Bit().constData(), QFileInfo(result).absoluteFilePath().toLocal8Bit().constData());
    }
    return result;
}

static QString getDataDirImpl() {
    QString dataDir = qgetenv("UGENE_DATA_PATH");
    if (!dataDir.isEmpty()) {
        if (!QFileInfo::exists(dataDir)) {
            coreLog.error(QString("UGENE_DATA_PATH is defined, but doesn't exist: '%1'").arg(dataDir));
        } else {
            QString resultDataDir = dataDir + (dataDir.endsWith("/") ? "" : "/");
            return resultDataDir.replace('\\', '/');
        }
    }

    bool ok = false;
    int suiteNumber = qEnvironmentVariableIntValue("UGENE_GUI_TEST_SUITE_NUMBER", &ok);
    if (isOsMac()) {
        if (ok && suiteNumber > 1) {
            dataDir = QString("data%1/").arg(suiteNumber - 1);
        } else {
            dataDir = QString("data/");
        }

        if (!QFileInfo::exists(dataDir) &&
            !BundleInfo::getDataSearchPath().isEmpty()) {
            dataDir = BundleInfo::getDataSearchPath() + "/";
        }
    } else {
        if (ok && suiteNumber > 1) {
            dataDir = QString("../../data%1/").arg(suiteNumber - 1);
        } else {
            dataDir = "../../data/";
        }

        if (!QFileInfo::exists(dataDir)) {
            dataDir = "data/";
        }
    }

    if (!QFileInfo::exists(dataDir)) {
        coreLog.error(QString("dataDir not found in the default places"));
    }

    return dataDir;
}

static QString getDataDir() {
    QString result = getDataDirImpl();
    if (qgetenv("UGENE_GUI_TEST") == "1") {  // In gui test mode dump test & data dir.
        qDebug("Data dir: '%s' -> '%s'", result.toLocal8Bit().constData(), QFileInfo(result).absoluteFilePath().toLocal8Bit().constData());
    }
    return result;
}

static QString getScreenshotDir() {
    QString result;
    if (isOsMac()) {
        result = "../../../../../../screenshotFol/";
    } else {
        QString guiTestOutputDirectory = qgetenv("GUI_TESTING_OUTPUT");
        if (guiTestOutputDirectory.isEmpty()) {
            result = QDir::homePath() + "/gui_testing_output/" +
                     QDate::currentDate().toString("dd.MM.yyyy") + "/screenshots/";
        } else {
            result = guiTestOutputDirectory + "/gui_testing_output/" +
                     QDate::currentDate().toString("dd.MM.yyyy") + "/screenshots/";
        }
    }
    return result;
}

/** Returns true if the test has Ignored or IgnoredOn<CurrentOS>. */
bool UGUITestLabels::hasIgnoredLabel(const GUITest* test) {
    QString ignoreOnPlatformLabel = isOsLinux()     ? UGUITestLabels::IgnoredOnLinux
                                    : isOsMac()     ? UGUITestLabels::IgnoredOnMacOS
                                    : isOsWindows() ? UGUITestLabels::IgnoredOnWindows
                                                    : "";
    SAFE_POINT(!ignoreOnPlatformLabel.isEmpty(), "Platform is not supported!", true);
    return test->labelSet.contains(UGUITestLabels::Ignored) || test->labelSet.contains(ignoreOnPlatformLabel);
}

/** Returns true if the test has current platform label: Linux, MacOS or Windows. */
bool UGUITestLabels::hasPlatformLabel(const GUITest* test) {
    QString platformLabel = isOsLinux()     ? UGUITestLabels::Linux
                            : isOsMac()     ? UGUITestLabels::MacOS
                            : isOsWindows() ? UGUITestLabels::Windows
                                            : "";
    SAFE_POINT(!platformLabel.isEmpty(), "Platform is not supported!", true);
    return test->labelSet.contains(platformLabel);
}

const QString UGUITest::testDir = getTestDir();
const QString UGUITest::dataDir = getDataDir();
const QString UGUITest::sandBoxDir = testDir + "_common_data/scenarios/sandbox/";
const QString UGUITest::screenshotDir = getScreenshotDir();

const QString UGUITestLabels::Nightly = "Nightly";
const QString UGUITestLabels::Linux = "Linux";
const QString UGUITestLabels::MacOS = "MacOS";
const QString UGUITestLabels::Windows = "Windows";
const QString UGUITestLabels::Ignored = "Ignored";
const QString UGUITestLabels::IgnoredOnLinux = "IgnoredOnLinux";
const QString UGUITestLabels::IgnoredOnMacOS = "IgnoredOnMacOS";
const QString UGUITestLabels::IgnoredOnWindows = "IgnoredOnWindows";

}  // namespace U2
