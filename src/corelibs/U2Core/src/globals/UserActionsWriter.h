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

#ifndef _U2_USER_ACTIONS_WRITER_H_
#define _U2_USER_ACTIONS_WRITER_H_

#include <QEvent>
#include <QMouseEvent>
#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT UserActionsWriter : public QObject {
    Q_OBJECT
public:
    UserActionsWriter();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    /** Logs message for the current event. Calls 'logMouseEventMessage' to do it. */
    void logMouseEvent(QMouseEvent* mouseEvent);

    /** Logs mouse event message only if it is different from the previous message. Saves the logged event into 'prevMessage'.  */
    void logMouseEventMessage(const QString& message);

    /** Logs message for the current event. Calls 'logKeyEventMessage' to do it. */
    void logKeyEvent(QKeyEvent* keyEvent);

    /** Logs key event message only if it is different from the previous message. Saves the logged event into 'prevMessage'.  */
    void logKeyEventMessage(QKeyEvent* keyEvent, const QString& message);

    static QString getTreeWidgetInfo(QMouseEvent* mouseEvent, QWidget* parent);

    static QString getAdditionalWidgetInfo(QMouseEvent* mouseEvent, QWidget* widget);

    static QString getMouseButtonInfo(QMouseEvent* mouseEvent);

    static QString getWidgetText(QMouseEvent* mouseEvent, QWidget* widget);

    static QString getKeyModifiersInfo(QKeyEvent* keyEvent);

    static QString getActiveModalWidgetInfo();

    /** Map of all loggable events by QEvent:Type. */
    QMap<QEvent::Type, QString> loggableEventNames;

    /** Map of all loggable key names. */
    QMap<Qt::Key, QString> keyNameByKeyCode;

    /** Previously logged message. */
    QString prevMessage;

    /** Stores typed text. Used to avoid 'by-char' logging in favor of 'by-full-string' logging. */
    QString typedTextBuffer;

    /** Keeps the number of times the same key is pressed before logging. */
    int keyPressCounter = 0;

    /** Last reported windows size. */
    QSize lastReportedWindowSize;
};

}  // namespace U2

#endif  // _U2_USER_ACTIONS_WRITER_H_
