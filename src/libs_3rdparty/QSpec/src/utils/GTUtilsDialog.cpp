/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include <core/GUITest.h>

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

#include "GTUtilsDialog.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTWidget.h"
#include "utils/GTThread.h"

#if Q_OS_DARWIN
#    include "utils/GTUtilsMac.h"
#endif
namespace HI {

#define GT_CLASS_NAME "GUIDialogWaiter"

GUIDialogWaiter::GUIDialogWaiter(GUITestOpStatus &_os, Runnable *_r, const WaitSettings &_settings)
    : isFinished(false), waiterId(-1), os(_os), runnable(_r), settings(_settings), timer(NULL), waitingTime(0) {
    static int totalWaiterCount = 0;
    waiterId = totalWaiterCount++;

    timer = new QTimer();

    timer->connect(timer, SIGNAL(timeout()), this, SLOT(checkDialog()));
    timer->start(timerPeriod);
}

GUIDialogWaiter::~GUIDialogWaiter() {
    finishWaiting();
}

void GUIDialogWaiter::finishWaiting() {
    delete timer;
    timer = nullptr;
    delete runnable;
    runnable = nullptr;
}

void GUIDialogWaiter::stopTimer() {
    if (timer != nullptr) {
        timer->stop();
    }
}

bool GUIDialogWaiter::isExpectedName(const QString &widgetObjectName, const QString &expectedObjectName) {
    if (expectedObjectName.isNull()) {
        qWarning("GT_DEBUG_MESSAGE GUIDialogWaiter Warning!! Checking name, widget name '%s', but expected any, saying it's expected", widgetObjectName.toLocal8Bit().constData());
        return true;
    }

    qDebug("GT_DEBUG_MESSAGE GUIDialogWaiter Checking name, widget name '%s', expected '%s'", widgetObjectName.toLocal8Bit().constData(), expectedObjectName.toLocal8Bit().constData());
    return widgetObjectName == expectedObjectName;
}

#define GT_METHOD_NAME "checkDialog"
void GUIDialogWaiter::checkDialog() {
    try {
        QWidget *widget = NULL;
        GT_CHECK_NO_MESSAGE(runnable != NULL, "Runnable is NULL");

        switch (settings.dialogType) {
            case Modal:
                widget = QApplication::activeModalWidget();
                break;
            case Popup:
                widget = QApplication::activePopupWidget();
                break;
            default:
                break;
        }

        if (widget != nullptr && !isFinished && isExpectedName(widget->objectName(), settings.objectName)) {
            timer->stop();
            qDebug("-------------------------");
            qDebug("GT_DEBUG_MESSAGE GUIDialogWaiter::wait ID = %d, name = '%s' going to RUN", waiterId, settings.objectName.toLocal8Bit().constData());
            qDebug("-------------------------");

            GT_CHECK(settings.destiny != MustNotBeRun,
                     QString("Dialog appears which mustn't appear: %1")
                         .arg(settings.objectName.isEmpty() ? "(an unnamed dialog)" : settings.objectName));

            try {
                GTThread::waitForMainThread();
                runnable->run();
                isFinished = true;
            } catch (GUITestOpStatus *) {
                QWidget *popupWidget = QApplication::activePopupWidget();
                while (popupWidget != NULL) {
                    GTWidget::close(os, popupWidget);
                    popupWidget = QApplication::activePopupWidget();
                }

                QWidget *modalWidget = QApplication::activeModalWidget();
                while (modalWidget != NULL) {
                    GTWidget::close(os, modalWidget);
                    modalWidget = QApplication::activeModalWidget();
                }
            }
        } else {
            waitingTime += timerPeriod;
            if (waitingTime > settings.timeout) {
                qDebug("-------------------------");
                qDebug("GT_DEBUG_MESSAGE !!! GUIDialogWaiter::TIMEOUT Id = %d, going to finish waiting", waiterId);
                qDebug("-------------------------");

                finishWaiting();
                GT_CHECK(false, "TIMEOUT, waiterId = " + QString::number(waiterId));
            }
        }

    } catch (GUITestOpStatus *) {
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog"

HangChecker::HangChecker(GUITestOpStatus &_os)
    : os(_os), mightHung(false) {
    timer = new QTimer();
}

void HangChecker::startChecking() {
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(sl_check()));
    timer->start(GTUtilsDialog::timerPeriod * 100);
}

#define GT_METHOD_NAME "sl_check"
void HangChecker::sl_check() {
    QWidget *dialog = QApplication::activeModalWidget();
    try {
        if (dialog != NULL) {
            bool found = false;
            foreach (GUIDialogWaiter *waiter, GTUtilsDialog::pool) {
                if (!waiter->isFinished && waiter->isExpectedName(dialog->objectName(), waiter->getSettings().objectName)) {
                    found = true;
                    mightHung = false;
                }
            }

            if (!found) {
                if (mightHung) {
                    GT_CHECK(false, "dialog " + QString(dialog->metaObject()->className()) + " name: " + dialog->objectName() + " hang up");
                }
            }

            if (!found) {
                if (!mightHung) {
                    mightHung = true;
                    qWarning("GT_DEBUG_MESSAGE dialog might hang up");
                }
            }

        } else {
            mightHung = false;
        }
    } catch (GUITestOpStatus *) {
        GTGlobals::takeScreenShot(os, GUITest::screenshotDir + QDateTime::currentDateTime().toString() + ".jpg");
        QWidget *w = QApplication::activeModalWidget();
        while (w != NULL) {
            w->close();
            w = QApplication::activeModalWidget();
        }
        w = QApplication::activePopupWidget();
        while (w != NULL) {
            w->close();
            w = QApplication::activePopupWidget();
        }
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog"

QList<GUIDialogWaiter *> GTUtilsDialog::pool = QList<GUIDialogWaiter *>();
HangChecker *GTUtilsDialog::hangChecker = NULL;

void GTUtilsDialog::startHangChecking(GUITestOpStatus &os) {
    hangChecker = new HangChecker(os);
    hangChecker->startChecking();
}

void GTUtilsDialog::stopHangChecking() {
    if (hangChecker != NULL) {
        hangChecker->timer->stop();
    }
}

#define GT_METHOD_NAME "buttonBox"
QDialogButtonBox *GTUtilsDialog::buttonBox(GUITestOpStatus &os, QWidget *dialog) {
    return qobject_cast<QDialogButtonBox *>(GTWidget::findWidget(os, "buttonBox", dialog));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButtonBox"
void GTUtilsDialog::clickButtonBox(GUITestOpStatus &os, QDialogButtonBox::StandardButton button) {
    clickButtonBox(os, QApplication::activeModalWidget(), button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButtonBox"
void GTUtilsDialog::clickButtonBox(GUITestOpStatus &os, QWidget *dialog, QDialogButtonBox::StandardButton button) {
#ifdef Q_OS_DARWIN
    QMessageBox *mbox = qobject_cast<QMessageBox *>(dialog);
    GTUtilsMac fakeClock;
    fakeClock.startWorkaroundForMacCGEvents(16000, false);
    if (mbox != NULL && (button == QDialogButtonBox::Yes || button == QDialogButtonBox::No || button == QDialogButtonBox::NoToAll)) {
        QMessageBox::StandardButton btn =
            button == QDialogButtonBox::Yes       ? QMessageBox::Yes
            : button == QDialogButtonBox::NoToAll ? QMessageBox::NoToAll
                                                  : QMessageBox::No;
        QAbstractButton *pushButton = mbox->button(btn);
        GT_CHECK(pushButton != NULL, "pushButton is NULL");
        GTWidget::click(os, pushButton);
    } else {
        QDialogButtonBox *box = buttonBox(os, dialog);
        GT_CHECK(box != NULL, "buttonBox is NULL");
        QPushButton *pushButton = box->button(button);
        GT_CHECK(pushButton != NULL, "pushButton is NULL");
        GTWidget::click(os, pushButton);
    }
#else
    QDialogButtonBox *box = buttonBox(os, dialog);
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton *pushButton = box->button(button);
    GT_CHECK(pushButton != NULL, "pushButton is NULL");
    GTWidget::click(os, pushButton);
#endif
}
#undef GT_METHOD_NAME

void GTUtilsDialog::waitForDialog(GUITestOpStatus &os, Runnable *r, const GUIDialogWaiter::WaitSettings &settings) {
    pool.prepend(new GUIDialogWaiter(os, r, settings));
}

void GTUtilsDialog::waitForDialog(GUITestOpStatus &os, Runnable *r, int timeout) {
    GUIDialogWaiter::WaitSettings settings;
    Filler *f = dynamic_cast<Filler *>(r);
    if (f) {
        settings = f->getSettings();
        if (timeout > 0) {
            settings.timeout = timeout;
        }
    }
    waitForDialog(os, r, settings);
}

void GTUtilsDialog::waitForDialogWhichMustNotBeRun(GUITestOpStatus &os, Runnable *r) {
    GUIDialogWaiter::WaitSettings settings;
    Filler *f = dynamic_cast<Filler *>(r);
    if (f) {
        settings = f->getSettings();
    }

    settings.destiny = GUIDialogWaiter::MustNotBeRun;
    waitForDialog(os, r, settings);
}

void GTUtilsDialog::waitForDialogWhichMayRunOrNot(GUITestOpStatus &os, Runnable *r) {
    GUIDialogWaiter::WaitSettings settings;
    Filler *f = dynamic_cast<Filler *>(r);
    if (f) {
        settings = f->getSettings();
    }

    settings.destiny = GUIDialogWaiter::NoMatter;
    settings.timeout = 480000;
    waitForDialog(os, r, settings);
}

#define GT_METHOD_NAME "checkNoActiveWaiters"
void GTUtilsDialog::checkNoActiveWaiters(GUITestOpStatus &os, int timeoutMillis) {
    bool isAllFinished = pool.isEmpty();
    for (int time = 0; time < timeoutMillis && !isAllFinished; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        isAllFinished = true;
        foreach (GUIDialogWaiter *waiter, pool) {
            if (!waiter->isFinished && waiter->getSettings().destiny == GUIDialogWaiter::MustBeRun) {
                isAllFinished = false;
                break;
            }
        }
    }
    if (!isAllFinished && !os.hasError()) {
        GUIDialogWaiter *nonFinishedWaiter = nullptr;
        foreach (GUIDialogWaiter *waiter, pool) {
            if (!waiter->isFinished && waiter->getSettings().destiny == GUIDialogWaiter::MustBeRun) {
                nonFinishedWaiter = waiter;
                break;
            }
        }
        os.setError(QString("There are active waiters after: %1ms. First waiter details: %2")
                        .arg(timeoutMillis)
                        .arg(nonFinishedWaiter == nullptr ? "nullptr?" : nonFinishedWaiter->getSettings().objectName));
    }
}
#undef GT_METHOD_NAME

void GTUtilsDialog::removeRunnable(Runnable *runnable) {
    for (GUIDialogWaiter *waiter : qAsConst(pool)) {
        if (waiter->getRunnable() == runnable) {
            pool.removeOne(waiter);
            delete waiter;
            return;
        }
    }
}

#define GT_METHOD_NAME "cleanup"
void GTUtilsDialog::cleanup(GUITestOpStatus &os, CleanupSettings s) {
    for (GUIDialogWaiter *waiter : qAsConst(pool)) {
        waiter->stopTimer();
    }

    if (s == FailOnUnfinished) {
        for (GUIDialogWaiter *waiter : qAsConst(pool)) {
            GT_CHECK(waiter != nullptr, "GUIDialogWaiter is null");
            switch (waiter->getSettings().destiny) {
                case GUIDialogWaiter::MustBeRun:
                    GT_CHECK(waiter->isFinished, QString("\"%1\" not run but should be").arg((waiter->getSettings().objectName)));
                    break;
                case GUIDialogWaiter::MustNotBeRun:
                    GT_CHECK(!waiter->isFinished, QString("\"%1\" had run but should not").arg((waiter->getSettings().objectName)));
                    break;
                case GUIDialogWaiter::NoMatter:
                    break;
            }
        }
    }

    stopHangChecking();

    qDeleteAll(pool);
    pool.clear();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

Filler::Filler(GUITestOpStatus &os, const GUIDialogWaiter::WaitSettings &settings, CustomScenario *scenario)
    : os(os), settings(settings), scenario(scenario) {
}

Filler::Filler(GUITestOpStatus &os, const QString &objectName, CustomScenario *scenario)
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
