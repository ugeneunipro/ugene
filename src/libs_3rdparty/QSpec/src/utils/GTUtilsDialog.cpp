/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "GTUtilsDialog.h"

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QPushButton>

#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"
#include "utils/GTThread.h"

#ifdef Q_OS_DARWIN
#    include "utils/GTUtilsMac.h"
#endif
namespace HI {

#define GT_CLASS_NAME "GUIDialogWaiter"

/** Check for dialog every twice as fast as ACTIVATION_TIME. */
static const int DIALOG_CHECK_PERIOD = GUIDialogWaiter::ACTIVATION_TIME / 2;

GUIDialogWaiter::GUIDialogWaiter(GUITestOpStatus& _os, Runnable* _r, const WaitSettings& _settings)
    : os(_os), runnable(_r), settings(_settings) {
    static int totalWaiterCount = 0;
    waiterId = totalWaiterCount++;

    connect(&timer, &QTimer::timeout, this, &GUIDialogWaiter::checkDialog);
    timer.start(DIALOG_CHECK_PERIOD);
}

GUIDialogWaiter::~GUIDialogWaiter() {
    delete runnable;
}

static QString getWaiterInfo(GUIDialogWaiter* waiter) {
    QString info = waiter->getSettings().objectName;
    if (info.isEmpty()) {
        if (auto qObject = dynamic_cast<QObject*>(waiter->getRunnable())) {
            const QMetaObject* metaObject = qObject->metaObject();
            info = metaObject != nullptr ? metaObject->className() : qObject->objectName();
        }
    }
    return info.isEmpty() ? "unnamed waiter " + QString::number(waiter->waiterId) : info;
}

static GUIDialogWaiter* getFirstOrNull(const QList<GUIDialogWaiter*>& waiterList) {
    return waiterList.isEmpty() ? nullptr : waiterList.first();
}

static bool checkDialogNameMatches(const QString& widgetObjectName, const QString& expectedObjectName) {
    if (expectedObjectName.isNull()) {
        qWarning("GT_DEBUG_MESSAGE GUIDialogWaiter Warning!! Checking name, widget name '%s', but expected any, saying it's expected",
                 widgetObjectName.toLocal8Bit().constData());
        return true;
    }
    qDebug("GT_DEBUG_MESSAGE GUIDialogWaiter Checking name, widget name '%s', expected '%s'",
           widgetObjectName.toLocal8Bit().constData(),
           expectedObjectName.toLocal8Bit().constData());

    return widgetObjectName == expectedObjectName;
}

#define GT_METHOD_NAME "checkDialog"
void GUIDialogWaiter::checkDialog() {
    if (!settings.isRandomOrderWaiter && this != getFirstOrNull(GTUtilsDialog::waiterList)) {
        return;
    }
    try {
        QWidget* widget = nullptr;
        GT_CHECK_NO_MESSAGE(runnable != nullptr, "Runnable is NULL");

        switch (settings.dialogType) {
            case DialogType::Modal:
                widget = QApplication::activeModalWidget();
                break;
            case DialogType::Popup:
                widget = QApplication::activePopupWidget();
                break;
            default:
                break;
        }

        QString widgetObjectName = widget != nullptr ? widget->objectName() : "";
        bool isDialogMatched = widget != nullptr && checkDialogNameMatches(widgetObjectName, settings.objectName);
        if (isDialogMatched) {
            qDebug("-------------------------");
            qDebug("GT_DEBUG_MESSAGE GUIDialogWaiter::wait ID = %d, name = '%s' going to RUN", waiterId, settings.objectName.toLocal8Bit().constData());
            qDebug("-------------------------");

            timer.stop();
            GTUtilsDialog::waiterList.removeOne(this);
            GTThread::waitForMainThread();
            runnable->run();
        } else {
            waitingTime += DIALOG_CHECK_PERIOD;
            if (waitingTime > settings.timeout) {
                timer.stop();
                GTUtilsDialog::waiterList.removeOne(this);
                qDebug("-------------------------");
                qDebug("GT_DEBUG_MESSAGE !!! GUIDialogWaiter::TIMEOUT Id = %d, going to finish waiting", waiterId);
                qDebug("-------------------------");
                GT_FAIL("TIMEOUT, waiterId = " + QString::number(waiterId) + ", objectName = " + settings.objectName, );
            }
        }
    } catch (GUITestOpStatus*) {
        qWarning("Caught exception in GUIDialogWaiter::checkDialog");
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog"

QList<GUIDialogWaiter*> GTUtilsDialog::waiterList = QList<GUIDialogWaiter*>();

#define GT_METHOD_NAME "buttonBox"
QDialogButtonBox* GTUtilsDialog::buttonBox(GUITestOpStatus& os, QWidget* dialog) {
    auto buttonBox = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK_RESULT(buttonBox != nullptr, "buttonBox is nullptr", nullptr);
    return buttonBox;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButtonBox"
void GTUtilsDialog::clickButtonBox(GUITestOpStatus& os, QDialogButtonBox::StandardButton button) {
    clickButtonBox(os, QApplication::activeModalWidget(), button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButtonBox"
void GTUtilsDialog::clickButtonBox(GUITestOpStatus& os, QWidget* dialog, QDialogButtonBox::StandardButton button) {
#ifdef Q_OS_DARWIN
    auto mbox = qobject_cast<QMessageBox*>(dialog);
    GTUtilsMac fakeClock;
    fakeClock.startWorkaroundForMacCGEvents(16000, false);
    if (mbox != NULL && (button == QDialogButtonBox::Yes || button == QDialogButtonBox::No || button == QDialogButtonBox::NoToAll)) {
        QMessageBox::StandardButton btn =
            button == QDialogButtonBox::Yes       ? QMessageBox::Yes
            : button == QDialogButtonBox::NoToAll ? QMessageBox::NoToAll
                                                  : QMessageBox::No;
        QAbstractButton* pushButton = mbox->button(btn);
        GT_CHECK(pushButton != NULL, "pushButton is NULL");
        GTWidget::click(os, pushButton);
    } else {
        QDialogButtonBox* box = buttonBox(os, dialog);
        GT_CHECK(box != NULL, "buttonBox is NULL");
        QPushButton* pushButton = box->button(button);
        GT_CHECK(pushButton != NULL, "pushButton is NULL");
        GTWidget::click(os, pushButton);
    }
#else
    QDialogButtonBox* box = buttonBox(os, dialog);
    QPushButton* pushButton = box->button(button);
    GT_CHECK(pushButton != nullptr, "pushButton is NULL");
    GTWidget::click(os, pushButton);
#endif
}
#undef GT_METHOD_NAME

void GTUtilsDialog::waitForDialog(GUITestOpStatus& os, Runnable* r, const GUIDialogWaiter::WaitSettings& settings, bool isPrependToList) {
    auto waiter = new GUIDialogWaiter(os, r, settings);
    if (isPrependToList) {
        waiterList.prepend(waiter);
    } else {
        waiterList.append(waiter);
    }
}

void GTUtilsDialog::add(GUITestOpStatus& os, Runnable* r, const GUIDialogWaiter::WaitSettings& settings) {
    waitForDialog(os, r, settings, false);
}

void GTUtilsDialog::add(GUITestOpStatus& os, Runnable* r, int timeout) {
    waitForDialog(os, r, timeout, false, false);
}

void GTUtilsDialog::waitForDialog(GUITestOpStatus& os, Runnable* r, int timeout, bool isRandomOrderWaiter, bool isPrependToList) {
    GUIDialogWaiter::WaitSettings settings;
    if (auto filler = dynamic_cast<Filler*>(r)) {
        settings = filler->getSettings();
        if (timeout > 0) {
            settings.timeout = timeout;
        }
    }
    settings.isRandomOrderWaiter = isRandomOrderWaiter;
    waitForDialog(os, r, settings, isPrependToList);
}

#define GT_METHOD_NAME "checkNoActiveWaiters"
void GTUtilsDialog::checkNoActiveWaiters(GUITestOpStatus& os, int timeoutMillis) {
    GUIDialogWaiter* notFinishedWaiter = getFirstOrNull(waiterList);
    for (int time = 0; time < timeoutMillis && notFinishedWaiter != nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        notFinishedWaiter = getFirstOrNull(waiterList);
    }
    if (notFinishedWaiter != nullptr && !os.hasError()) {
        os.setError(QString("There are active waiters after: %1ms. First waiter details: %2")
                        .arg(timeoutMillis)
                        .arg(notFinishedWaiter->getSettings().objectName));
    }
}
#undef GT_METHOD_NAME

void GTUtilsDialog::removeRunnable(Runnable* runnable) {
    for (GUIDialogWaiter* waiter : qAsConst(waiterList)) {
        if (waiter->getRunnable() == runnable) {
            waiterList.removeOne(waiter);
            delete waiter;
            return;
        }
    }
}

#define GT_METHOD_NAME "cleanup"
void GTUtilsDialog::cleanup(GUITestOpStatus& os, const CleanupMode& cleanupMode) {
    auto nonFinishedWaiter = getFirstOrNull(waiterList);
    bool hasNonFinishedWaiter = nonFinishedWaiter != nullptr;
    QString nonFinishedWaiterInfo = nonFinishedWaiter == nullptr ? "" : getWaiterInfo(nonFinishedWaiter);

    qDeleteAll(waiterList);
    waiterList.clear();

    if (cleanupMode == CleanupMode::FailOnUnfinished && hasNonFinishedWaiter) {
        GT_FAIL(QString("Expected dialog was not found: \"%1\"").arg(nonFinishedWaiterInfo), );
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

Filler::Filler(GUITestOpStatus& os, const GUIDialogWaiter::WaitSettings& settings, CustomScenario* scenario)
    : os(os), settings(settings), scenario(scenario) {
}

Filler::Filler(GUITestOpStatus& os, const QString& objectName, CustomScenario* scenario)
    : os(os), settings(GUIDialogWaiter::WaitSettings(objectName)), scenario(scenario) {
}

Filler::~Filler() {
    delete scenario;
}

GUIDialogWaiter::WaitSettings Filler::getSettings() const {
    return settings;
}

void Filler::run() {
    if (scenario == nullptr) {
        commonScenario();
    } else {
        scenario->run(os);
    }
    GTThread::waitForMainThread();
}

void Filler::releaseMouseButtons() {
    Qt::MouseButtons buttons = QGuiApplication::mouseButtons();

    if (buttons | Qt::LeftButton) {
        GTMouseDriver::release(Qt::LeftButton);
    }

    if (buttons | Qt::RightButton) {
        GTMouseDriver::release(Qt::RightButton);
    }
}

}  // namespace HI
