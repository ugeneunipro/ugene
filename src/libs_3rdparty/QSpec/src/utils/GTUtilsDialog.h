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

#ifndef _HI_GUI_DIALOG_UTILS_H_
#define _HI_GUI_DIALOG_UTILS_H_

#include <QDialogButtonBox>

#include <core/CustomScenario.h>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT Runnable {
public:
    virtual void run() = 0;
    virtual ~Runnable(){}
};

class HI_EXPORT GUIDialogWaiter : public QObject {
    Q_OBJECT
public:
    enum DialogType {
        Modal,
        Popup
    };
    enum DialogDestiny {
        MustBeRun,
        MustNotBeRun,
        NoMatter
    };

    struct WaitSettings {
        WaitSettings(const QString& _objectName = "",
                     DialogType _dialogType = GUIDialogWaiter::Modal,
                     int _timeout = 120000,
                     DialogDestiny _destiny = MustBeRun) :
            dialogType(_dialogType),
            objectName(_objectName),
            timeout(_timeout),
            destiny(_destiny) {}

        DialogType dialogType;
        QString objectName;
        int timeout;
        DialogDestiny destiny;
    };

    GUIDialogWaiter(GUITestOpStatus &os, Runnable* _r, const WaitSettings& settings = WaitSettings());
    virtual ~GUIDialogWaiter();

    WaitSettings getSettings(){return settings;}
    void stopTimer();
    static const int timerPeriod = 100;

    bool hadRun;
    int waiterId;

    bool isExpectedName(const QString& widgetObjectName, const QString& expectedObjectName);

    bool operator ==(Runnable const * const runnable) const;

public slots:
    void checkDialog();
    void checkDialogPool();

private:
    GUITestOpStatus &os;
    Runnable *runnable;
    WaitSettings settings;

    QTimer* timer;
    int waitingTime;

    void finishWaiting(); // deletes timer and runnable

};

class HI_EXPORT Filler : public Runnable {
public:
    Filler(GUITestOpStatus &os, const GUIDialogWaiter::WaitSettings &settings, CustomScenario *scenario = NULL);
    Filler(GUITestOpStatus &os, const QString &objectName, CustomScenario *scenario = NULL);
    ~Filler();

    GUIDialogWaiter::WaitSettings getSettings() const;
    void run();
    virtual void commonScenario() {}

protected:
    GUITestOpStatus &os;
    GUIDialogWaiter::WaitSettings settings;
    CustomScenario *scenario;

private:
    void releaseMouseButtons();
};

#define DIALOG_FILLER_DECLARATION(className, DialogName) \
    class className : public Filler { \
    public: \
        className(HI::GUITestOpStatus& os):Filler(os, DialogName){} \
        virtual void run(); \
    }

#define DIALOG_FILLER_DEFFINITION(className) \
    void className::run()


class HI_EXPORT HangChecker: public QObject{
    Q_OBJECT
public:
    HangChecker(GUITestOpStatus &_os);
    QTimer* timer;
    void startChecking();
    GUITestOpStatus &os;
    bool mightHung;
public slots:
    void sl_check();
};

class HI_EXPORT GTUtilsDialog{
    friend class TimerLauncher;
    friend class GUIDialogWaiter;
    friend class HangChecker;
public:
    enum CleanupSettings {
        FailOnUnfinished, NoFailOnUnfinished
    };

    static QDialogButtonBox * buttonBox(GUITestOpStatus &os, QWidget *dialog);

    static void clickButtonBox(GUITestOpStatus &os, QDialogButtonBox::StandardButton button);
    static void clickButtonBox(GUITestOpStatus &os, QWidget *dialog, QDialogButtonBox::StandardButton button);

    // if objectName is not empty, waits for QWidget with a given name
    static void waitForDialog(GUITestOpStatus &os, Runnable *r, const GUIDialogWaiter::WaitSettings& settings);

    static void waitForDialog(GUITestOpStatus &os, Runnable *r, int timeout = 0);

    static void waitForDialogClosed();

    static void waitForDialogWhichMustNotBeRun(GUITestOpStatus &os, Runnable *r);

    static void waitForDialogWhichMayRunOrNot(GUITestOpStatus &os, Runnable *r);

    static void removeRunnable(Runnable const * const runnable);

    // deletes all GUIDialogWaiters, sets err if there are unfinished waiters
    static void cleanup(GUITestOpStatus &os, CleanupSettings s = FailOnUnfinished);

    static void startHangChecking(GUITestOpStatus &os);
    static void stopHangChecking();

    static bool isButtonEnabled(GUITestOpStatus& os, QWidget* dialog, QDialogButtonBox::StandardButton button);

private:
    static void checkAllFinished(GUITestOpStatus &os);

    static QList<GUIDialogWaiter*> pool;
    static HangChecker* hangChecker;
    static const int timerPeriod = 100;
};

} // namespace

#endif
