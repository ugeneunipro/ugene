/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

namespace HI {

#define GT_CLASS_NAME "GUIDialogWaiter"

/** Check for dialog twice as fast as ACTIVATION_TIME. */
static const int DIALOG_CHECK_PERIOD = GUIDialogWaiter::ACTIVATION_TIME / 2;

GUIDialogWaiter::GUIDialogWaiter(Runnable* _r, const WaitSettings& _settings)
    : runnable(_r), settings(_settings) {
    GT_LOG(QString("Created a new GUIDialogWaiter for '%1' ('%2')").arg(settings.objectName).arg(settings.logName));
    GT_CHECK(runnable != nullptr, "Check that runnable is not null");

    static int totalWaiterCount = 0;
    waiterId = totalWaiterCount++;

    connect(&timer, &QTimer::timeout, this, &GUIDialogWaiter::checkDialog);
    timer.start(DIALOG_CHECK_PERIOD);
}

GUIDialogWaiter::~GUIDialogWaiter() {
    delete runnable;
}

static GUIDialogWaiter* getFirstOrNull(const QList<GUIDialogWaiter*>& waiterList) {
    return waiterList.isEmpty() ? nullptr : waiterList.first();
}

static bool checkDialogNameMatches(const QString& widgetObjectName, const QString& expectedObjectName) {
    if (expectedObjectName.isNull()) {
        GT_LOG(QString("checkDialogNameMatches widget name '%1', but expected any. Resolving as MATCH.").arg(widgetObjectName));
        return true;
    }
    GT_LOG(QString("checkDialogNameMatches: widget name '%1', expected '%2'.").arg(widgetObjectName).arg(expectedObjectName));
    return widgetObjectName == expectedObjectName;
}

void GUIDialogWaiter::checkDialog() {
    if (this != getFirstOrNull(GTUtilsDialog::waiterList)) {
        return;
    }
    QWidget* widget;
    switch (settings.dialogType) {
        case DialogType::Modal:
            widget = QApplication::activeModalWidget();
            break;
        case DialogType::Popup:
            widget = QApplication::activePopupWidget();
            break;
        default:
            GT_FAIL("Unexpected dialog type: " + QString::number((int)settings.dialogType), );
    }

    QString currentWidgetObjectName = widget != nullptr ? widget->objectName() : "";
    bool isDialogMatched = widget != nullptr && checkDialogNameMatches(currentWidgetObjectName, settings.objectName);
    GT_LOG(QString("checkDialog: tick: waiterId: %1, waiting for: '%2' ('%3'), current widget: '%4'")
               .arg(waiterId)
               .arg(settings.objectName)
               .arg(settings.logName)
               .arg(widget == nullptr ? "nullptr" : currentWidgetObjectName));
    if (isDialogMatched) {
        GT_LOG(QString("checkDialog: MATCH: waiterId: %1, objectName: '%2' ('%3')")
                   .arg(waiterId)
                   .arg(settings.objectName)
                   .arg(settings.logName));
        timer.stop();
        GTUtilsDialog::waiterList.removeOne(this);
        GTThread::waitForMainThread();
        runnable->run();
    } else {
        waitingTime += DIALOG_CHECK_PERIOD;
        if (waitingTime > settings.timeout) {
            timer.stop();
            GTUtilsDialog::waiterList.removeOne(this);
            GT_FAIL(QString("checkDialog: TIMEOUT: waiterId: %1, objectName: '%2' ('%3'), timeout: '%4')")
                        .arg(waiterId)
                        .arg(settings.objectName)
                        .arg(settings.logName)
                        .arg(settings.timeout), );
        }
    }
}
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog"

QList<GUIDialogWaiter*> GTUtilsDialog::waiterList = QList<GUIDialogWaiter*>();

QDialogButtonBox* GTUtilsDialog::buttonBox(QWidget* dialog) {
    auto buttonBox = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget("buttonBox", dialog));
    GT_CHECK_RESULT(buttonBox != nullptr, "buttonBox is nullptr. " + Filler::generateFillerStackInfo(), nullptr);
    return buttonBox;
}

void GTUtilsDialog::clickButtonBox(QDialogButtonBox::StandardButton button) {
    clickButtonBox(QApplication::activeModalWidget(), button);
}

void GTUtilsDialog::clickButtonBox(QWidget* dialog, QDialogButtonBox::StandardButton button) {
#ifdef Q_OS_DARWIN
    GTGlobals::sleep(500);  // TODO: check is a separate PR/test-run that this condition is still needed.
#endif
    for (int time = 0; time < 3000; time += GT_OP_CHECK_MILLIS) {
        QDialogButtonBox* box = buttonBox(dialog);
        QPushButton* pushButton = box->button(button);
        GT_CHECK(pushButton != nullptr, "pushButton is NULL");
        if (pushButton->isEnabled()) {
            GTWidget::click(pushButton);
            return;
        }
        GTGlobals::sleep(GT_OP_CHECK_MILLIS, "waiting for a button in GTUtilsDialog::clickButtonBox");
    }
    GT_FAIL("Button was not enabled. " + Filler::generateFillerStackInfo(), );
}

void GTUtilsDialog::waitForDialog(Runnable* r, const GUIDialogWaiter::WaitSettings& settings, bool isPrependToList) {
    auto waiter = new GUIDialogWaiter(r, settings);
    if (isPrependToList) {
        waiterList.prepend(waiter);
    } else {
        waiterList.append(waiter);
    }
}

void GTUtilsDialog::add(Runnable* r, const GUIDialogWaiter::WaitSettings& settings) {
    waitForDialog(r, settings, false);
}

void GTUtilsDialog::add(Runnable* r, int timeout) {
    waitForDialog(r, timeout, false);
}

void GTUtilsDialog::waitForDialog(Runnable* r, int timeout, bool isPrependToList) {
    GUIDialogWaiter::WaitSettings settings;
    if (auto filler = dynamic_cast<Filler*>(r)) {
        settings = filler->getSettings();
        if (timeout > 0) {
            settings.timeout = timeout;
        }
    }
    waitForDialog(r, settings, isPrependToList);
}

void GTUtilsDialog::checkNoActiveWaiters(int timeoutMillis) {
    GUIDialogWaiter* notFinishedWaiter = getFirstOrNull(waiterList);
    for (int time = 0; time < timeoutMillis && notFinishedWaiter != nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS, "checkNoActiveWaiters");
        notFinishedWaiter = getFirstOrNull(waiterList);
    }
    if (notFinishedWaiter != nullptr && !GTGlobals::getOpStatus().hasError()) {
        const GUIDialogWaiter::WaitSettings& settings = notFinishedWaiter->getSettings();
        GT_FAIL(QString("There are active waiters after: %1ms. First waiter details: %2 (%3)")
                    .arg(timeoutMillis)
                    .arg(settings.objectName)
                    .arg(settings.logName), );
    }
    GT_LOG("checkNoActiveWaiters found no active waiters");
}

void GTUtilsDialog::removeRunnable(Runnable* runnable) {
    for (GUIDialogWaiter* waiter : qAsConst(waiterList)) {
        if (waiter->getRunnable() == runnable) {
            waiterList.removeOne(waiter);
            delete waiter;
            return;
        }
    }
}

void GTUtilsDialog::cleanup() {
    qDeleteAll(waiterList);
    waiterList.clear();
}

#undef GT_CLASS_NAME

QStack<QString> Filler::activeFillerLogNamesStack;

Filler::Filler(const GUIDialogWaiter::WaitSettings& _settings, CustomScenario* _scenario)
    : settings(_settings), scenario(_scenario) {
}

Filler::Filler(const QString& objectName, CustomScenario* _scenario)
    : settings(objectName, GUIDialogWaiter::DialogType::Modal), scenario(_scenario) {
}

Filler::~Filler() {
    delete scenario;
}

void Filler::commonScenario() {
}

QString Filler::generateFillerStackInfo() {
    return activeFillerLogNamesStack.isEmpty() ? "Active fillers: none" : "Active fillers: " + activeFillerLogNamesStack.toList().join(",");
}

GUIDialogWaiter::WaitSettings Filler::getSettings() const {
    return settings;
}

void Filler::run() {
    activeFillerLogNamesStack.push(settings.logName);
    if (scenario == nullptr) {
        commonScenario();
    } else {
        scenario->run();
    }
    activeFillerLogNamesStack.pop();
    GTThread::waitForMainThread();
}

}  // namespace HI
