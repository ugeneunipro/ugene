/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QColor>
#include <QDialog>
#include <QMouseEvent>
#include <QObject>
#include <QTimer>

#include <U2Core/Log.h>  // TODO_SVEDIT: remove later
#include <U2Core/U2Region.h>

namespace U2 {

class DetView;
class DNASequence;
class ModifySequenceContentTask;
class Task;
class U2SequenceObject;

class DetViewSequenceEditor : public QObject {
    Q_OBJECT
public:
    // DetView can be reduced to GSequenceView or template for MA
    DetViewSequenceEditor(DetView* view);
    ~DetViewSequenceEditor();

    void reset();
    bool isEditMode() const;
    QAction* getEditAction() const {
        return editAction;
    }

    bool eventFilter(QObject* watched, QEvent* event) override;

    int getCursorPosition() const {
        return cursor;
    }
    QColor getCursorColor() const {
        return cursorColor;
    }

private:
    void setCursor(qint64 newPos);
    void navigate(qint64 newPos, bool shiftPressed = false);

    void insertChar(int character);
    void deleteChar(int key);

    void modifySequence(U2SequenceObject* seqObj, const U2Region& region, const DNASequence& sequence);
    void cancelSelectionResizing();

private slots:
    void sl_editMode(bool active);

    /** Called periodically as a timer callback to animate blinking edit-mode cursor. */
    void sl_cursorAnimationTimerCallback();

    void sl_objectLockStateChanged();
    void sl_paste(Task* pasteTask);

private:
    qint64 cursor = 0;  // TODO_SVEDIT: can be separate class
    QColor cursorColor;
    QTimer animationTimer;
    DetView* view;

    QAction* editAction;
};

}  // namespace U2
