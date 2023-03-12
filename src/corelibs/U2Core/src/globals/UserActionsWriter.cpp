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

#include "UserActionsWriter.h"

#include <QAbstractSpinBox>
#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>
#include <QTreeWidget>
#include <QWidget>

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

bool UserActionsWriter::eventFilter(QObject*, QEvent* event) {
    QEvent::Type eventType = event->type();

    QThread* currentThread = QThread::currentThread();
    SAFE_POINT(currentThread == QCoreApplication::instance()->thread(), "Got UX event not in the main thread: " + QString::number(eventType), false);

    if (eventType == QEvent::MouseButtonPress ||
        eventType == QEvent::MouseButtonRelease ||
        eventType == QEvent::MouseButtonDblClick) {
        logMouseEvent(dynamic_cast<QMouseEvent*>(event));
    } else if (eventType == QEvent::KeyPress ||
               eventType == QEvent::KeyRelease) {
        logKeyEvent(dynamic_cast<QKeyEvent*>(event));
    }
    return false;
}

void UserActionsWriter::logMouseEvent(QMouseEvent* mouseEvent) {
    SAFE_POINT(mouseEvent != nullptr, "logMouseEvent: Mouse event is nul", );

    QString message;
    message.append(getActiveModalWidgetInfo());
    message.prepend(loggableEventNames.value(mouseEvent->type()) + " ");
    message.append(getMouseButtonInfo(mouseEvent));

    QMainWindow* mainWindow = AppContext::getMainWindow()->getQMainWindow();
    CHECK_EXT(mainWindow != nullptr, userActLog.error("Main window is null"), );

    // Window size info.
    QSize currentWindowSize = mainWindow->geometry().size();
    if (currentWindowSize != lastReportedWindowSize) {
        userActLog.trace(QString("WINDOW SIZE: %1x%2").arg(currentWindowSize.width()).arg(currentWindowSize.height()));
        lastReportedWindowSize = currentWindowSize;
    }

    // Coordinates.
    QPoint mouseWindowLocalPos = mainWindow->mapFromGlobal(mouseEvent->globalPos());
    message.append(QString("%1 %2 ").arg(mouseWindowLocalPos.x()).arg(mouseWindowLocalPos.y()));

    // Widget info.
    QWidget* underMouseWidget = QApplication::widgetAt(mouseEvent->globalPos());
    if (underMouseWidget != nullptr) {
        QString className = underMouseWidget->metaObject()->className();

        // tree widget and list widget
        // sometimes QWidget is on top. it does not give any information, but it's parent does
        if (className == "QWidget") {
            if (auto parent = qobject_cast<QWidget*>(underMouseWidget->parent())) {
                message.append(getTreeWidgetInfo(mouseEvent, parent));
                underMouseWidget = parent;
            }
        } else {
            message.append("CLASS_NAME: ").append(className);
        }
        message.append(" ");

        // Additional information.
        message.append(getAdditionalWidgetInfo(mouseEvent, underMouseWidget));
    } else {
        message.append("Widget under cursor is NULL");
    }
    logMouseEventMessage(message);
}

QString UserActionsWriter::getMouseButtonInfo(QMouseEvent* mouseEvent) {
    switch (mouseEvent->button()) {
        case Qt::RightButton: {
            return "Right_button ";
        }
        case Qt::LeftButton: {
            return "Left_button ";
        }
        default: {
            return "Other_button ";
        }
    }
}

QString UserActionsWriter::getTreeWidgetInfo(QMouseEvent* mouseEvent, QWidget* parent) {
    QString message;
    message.append("CLASS_NAME: ").append(parent->metaObject()->className());

    if (auto tree = qobject_cast<QTreeWidget*>(parent)) {
        QTreeWidgetItem* item = tree->itemAt(mouseEvent->pos());
        if (item) {
            message.append(" TREE_ITEM: " + item->text(0));
        }
    } else if (auto list = qobject_cast<QListWidget*>(parent)) {
        QListWidgetItem* item = list->itemAt(list->mapFromGlobal(mouseEvent->globalPos()));
        if (item != nullptr) {
            message.append(" LIST_ITEM: " + item->text());
        }
    }
    return message;
}

QString UserActionsWriter::getAdditionalWidgetInfo(QMouseEvent* mouseEvent, QWidget* widget) {
    QString text = getWidgetText(mouseEvent, widget);
    QString objectName = widget->objectName();
    QString tooltip = widget->toolTip();
    QString message;

    if (!text.isEmpty() && text != "...") {
        message.append("TEXT: " + text);
    } else if (!tooltip.isEmpty()) {
        message.append("TOOLTIP: " + tooltip);
    } else if (!objectName.isEmpty()) {
        message.append("OBJECT_NAME: " + objectName);
    }
    if (auto spinBox = qobject_cast<QAbstractSpinBox*>(widget)) {
        message.append(" " + spinBox->text());
    }
    return message;
}

QString UserActionsWriter::getWidgetText(QMouseEvent* mouseEvent, QWidget* widget) {
    QString text("");

    if (auto label = qobject_cast<QLabel*>(widget)) {
        text.append(label->text());
    } else if (auto button = qobject_cast<QAbstractButton*>(widget)) {
        text.append(button->text());
    } else if (auto menu = qobject_cast<QMenu*>(widget)) {
        QAction* menuAct = menu->actionAt(menu->mapFromGlobal(mouseEvent->globalPos()));
        if (menuAct) {
            text.append(menuAct->text());
        }
    } else if (auto menuBar = qobject_cast<QMenuBar*>(widget)) {
        QAction* menuBarAct = menuBar->actionAt(menuBar->mapFromGlobal(mouseEvent->globalPos()));
        if (menuBarAct) {
            text.append(menuBarAct->text());
        }
    } else if (auto lineEdit = qobject_cast<QLineEdit*>(widget)) {
        text.append(lineEdit->text());
    }
    return text;
}

void UserActionsWriter::logKeyEvent(QKeyEvent* keyEvent) {
    SAFE_POINT(keyEvent != nullptr, "logKeyEvent: Key event is null", );

    QString eventName = keyEvent->text();
    QString keyName = keyNameByKeyCode.value(Qt::Key(keyEvent->key()));

    QString message = loggableEventNames.value(keyEvent->type()) + " ";
    message.append(getActiveModalWidgetInfo());
    message.append(getKeyModifiersInfo(keyEvent));

    if (!keyName.isEmpty()) {
        message.append(QString("%1").arg(keyName));
    } else if (!eventName.isEmpty()) {
        message.append(eventName).append(QString(" code: %1").arg(keyEvent->key()));
    } else {
        message.append(QString("Undefined key, code: %1").arg(keyEvent->key()));
    }

    logKeyEventMessage(keyEvent, message);
}

void UserActionsWriter::logMouseEventMessage(const QString& message) {
    CHECK(message != prevMessage, );

    if (!typedTextBuffer.isEmpty()) {
        userActLog.trace(QString("Typed string. Length=%1").arg(typedTextBuffer.length()));
        typedTextBuffer = "";
    }

    if (keyPressCounter != 0) {
        userActLog.trace(QString("pressed %1 times").arg(keyPressCounter + 1));
        keyPressCounter = 0;
    }

    // Do not duplicate event information when logging mouse release event
    QString prevMessageWithNoPressEventName = prevMessage.right(prevMessage.length() - loggableEventNames.value(QEvent::MouseButtonPress).length());
    QString currentMessageWithNoReleaseEventName = message.right(message.length() - loggableEventNames.value(QEvent::MouseButtonRelease).length());
    if (prevMessageWithNoPressEventName == currentMessageWithNoReleaseEventName) {
        userActLog.trace("mouse_release");
        prevMessage = message;
        return;
    }
    prevMessage = message;
    userActLog.trace(message);
}

void UserActionsWriter::logKeyEventMessage(QKeyEvent* keyEvent, const QString& message) {
    CHECK(message != prevMessage && loggableEventNames.value(keyEvent->type()) != nullptr, );

    /*Do not duplicate event information when logging key release event*/
    QString prevMessageWithNoPressEventName = prevMessage.right(prevMessage.length() - loggableEventNames.value(QEvent::KeyPress).length());
    QString currentMessageWithNoReleaseEventName = message.right(message.length() - loggableEventNames.value(QEvent::KeyRelease).length());
    if (prevMessageWithNoPressEventName == currentMessageWithNoReleaseEventName) {
        prevMessage = message;
        return;
    }

    // If the same key pressed multiple times - count presses.
    int key = keyEvent->key();
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
    bool isBufferedEvent = key <= Qt::Key_QuoteLeft && key >= Qt::Key_Space && (modifiers.testFlag(Qt::NoModifier) || modifiers.testFlag(Qt::KeypadModifier));
    if (!isBufferedEvent) {
        QString prevMessageWithNoReleaseEventName = prevMessage.right(prevMessage.length() - loggableEventNames.value(QEvent::KeyRelease).length());
        QString currentMessageWithNoPressEventName = message.right(message.length() - loggableEventNames.value(QEvent::KeyPress).length());
        if (prevMessageWithNoReleaseEventName == currentMessageWithNoPressEventName) {
            prevMessage = message;
            keyPressCounter++;
            return;
        }
    }

    if (keyPressCounter != 0) {
        userActLog.trace(QString("pressed %1 times").arg(keyPressCounter + 1));
        keyPressCounter = 0;
    }

    prevMessage = message;

    if (isBufferedEvent) {
        typedTextBuffer.append(keyEvent->text());
        return;
    }

    if (!typedTextBuffer.isEmpty()) {
        userActLog.trace(QString("Typed string. Length=%1").arg(typedTextBuffer.length()));
        typedTextBuffer = "";
    }

    userActLog.trace(message);
}

QString UserActionsWriter::getActiveModalWidgetInfo() {
    auto dialog = qobject_cast<QDialog*>(QApplication::activeModalWidget());
    CHECK(dialog != nullptr, "");

    QString message;
    message.append(QString("DIALOG: \"%1\" ").arg(dialog->windowTitle()));
    if (auto messageBox = qobject_cast<QMessageBox*>(dialog)) {
        message.append("MESSAGEBOX_TEXT: ").append(messageBox->text()).append(" ");
    }
    return message;
}

QString UserActionsWriter::getKeyModifiersInfo(QKeyEvent* keyEvent) {
    int key = keyEvent->key();
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();

    QString message;
    if (modifiers.testFlag(Qt::ControlModifier) && key != Qt::Key_Control) {
        message += "ctrl + ";
    }
    if (modifiers.testFlag(Qt::AltModifier) && key != Qt::Key_Alt) {
        message += "alt + ";
    }
    if (modifiers.testFlag(Qt::ShiftModifier) && key != Qt::Key_Shift) {
        message += "shift + ";
    }
    if (modifiers.testFlag(Qt::MetaModifier) && key != Qt::Key_Meta) {
        message += "meta + ";
    }
    if (modifiers.testFlag(Qt::GroupSwitchModifier) && key != Qt::Key_Mode_switch) {
        message += "switch + ";
    }
    return message;
}

UserActionsWriter::UserActionsWriter() {
    loggableEventNames = {
        {QEvent::MouseButtonPress, "mouse_press"},
        {QEvent::MouseButtonRelease, "mouse_release"},
        {QEvent::MouseButtonDblClick, "mouse_double_click"},
        {QEvent::KeyPress, "press"},
        {QEvent::KeyRelease, "release"}};

    keyNameByKeyCode = {
        {Qt::Key_Return, "enter"},
        {Qt::Key_Escape, "esc"},
        {Qt::Key_Tab, "tab"},
        {Qt::Key_Backtab, "back_tab"},
        {Qt::Key_Backspace, "backspace"},
        {Qt::Key_Insert, "insert"},
        {Qt::Key_Delete, "delete"},
        {Qt::Key_Enter, "keypad_enter"},
        {Qt::Key_Home, "home"},
        {Qt::Key_End, "end"},
        {Qt::Key_Left, "left_arrow"},
        {Qt::Key_Up, "up_arrow"},
        {Qt::Key_Right, "right_arrow"},
        {Qt::Key_Down, "down_arrow"},
        {Qt::Key_PageUp, "page_up"},
        {Qt::Key_PageDown, "page_down"},
        {Qt::Key_Shift, "shift"},
        {Qt::Key_Control, "ctrl"},
        {Qt::Key_Alt, "alt"},
        {Qt::Key_CapsLock, "caps_lock"},
        {Qt::Key_NumLock, "num_lock"},
        {Qt::Key_F1, "F1"},
        {Qt::Key_F2, "F2"},
        {Qt::Key_F3, "F3"},
        {Qt::Key_F4, "F4"},
        {Qt::Key_F5, "F5"},
        {Qt::Key_F6, "F6"},
        {Qt::Key_F7, "F7"},
        {Qt::Key_F8, "F8"},
        {Qt::Key_F9, "F9"},
        {Qt::Key_F10, "F10"},
        {Qt::Key_F11, "F11"},
        {Qt::Key_F12, "F12"},
        {Qt::Key_Space, "space"},
        {Qt::Key_0, "0"},
        {Qt::Key_1, "1"},
        {Qt::Key_2, "2"},
        {Qt::Key_3, "3"},
        {Qt::Key_4, "4"},
        {Qt::Key_5, "5"},
        {Qt::Key_6, "6"},
        {Qt::Key_7, "7"},
        {Qt::Key_8, "8"},
        {Qt::Key_9, "9"},
        {Qt::Key_A, "a"},
        {Qt::Key_B, "b"},
        {Qt::Key_C, "c"},
        {Qt::Key_D, "d"},
        {Qt::Key_E, "e"},
        {Qt::Key_F, "f"},
        {Qt::Key_G, "g"},
        {Qt::Key_H, "h"},
        {Qt::Key_I, "i"},
        {Qt::Key_J, "j"},
        {Qt::Key_K, "k"},
        {Qt::Key_L, "l"},
        {Qt::Key_M, "m"},
        {Qt::Key_N, "n"},
        {Qt::Key_O, "o"},
        {Qt::Key_P, "p"},
        {Qt::Key_Q, "q"},
        {Qt::Key_R, "r"},
        {Qt::Key_S, "s"},
        {Qt::Key_T, "t"},
        {Qt::Key_U, "u"},
        {Qt::Key_V, "v"},
        {Qt::Key_W, "w"},
        {Qt::Key_X, "x"},
        {Qt::Key_Y, "y"},
        {Qt::Key_Z, "z"}};
}

}  // namespace U2
