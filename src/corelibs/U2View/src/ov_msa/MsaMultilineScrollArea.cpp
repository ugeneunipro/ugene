#include "MsaMultilineScrollArea.h"

#include <QScrollBar>
#include <QWheelEvent>

#include <U2Gui/GScrollBar.h>

#include "MaEditor.h"
#include "MaEditorSelection.h"
#include "MaEditorWgt.h"
#include "MultilineScrollController.h"
#include "RowHeightController.h"
#include "ScrollController.h"
#include "ov_msa/BaseWidthController.h"

namespace U2 {

MsaMultilineScrollArea::MsaMultilineScrollArea(MaEditor* maEditor, MaEditorMultilineWgt* maEditorUi)
    : QScrollArea(maEditorUi),
      maEditor(maEditor),
      maEditorUi(maEditorUi) {
    this->installEventFilter(this);
}

bool MsaMultilineScrollArea::eventFilter(QObject* obj, QEvent* event) {
    if (obj == this && maEditor->getMultilineMode() && event->type() == QEvent::KeyPress) {
        auto kEvent = static_cast<QKeyEvent*>(event);
        bool isShiftPressed = kEvent->modifiers().testFlag(Qt::ShiftModifier);
        bool isCtrlPressed = kEvent->modifiers().testFlag(Qt::ControlModifier);
        auto key = kEvent->key();

        switch (key) {
            case Qt::Key_Escape:
            case Qt::Key_Delete:
            case Qt::Key_Backspace:
            case Qt::Key_Insert:
                // ignore MSA sequence view widget keys
                return true;
            case Qt::Key_Space:
                // ignore MSA sequence view widget keys
                return true;
            case Qt::Key_Left:
            case Qt::Key_Right:
                return maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed);
            case Qt::Key_Up:
                if (maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed))
                    return true;
                scrollVert(MultilineScrollController::Up, true, true);
                return true;
            case Qt::Key_Down:
                if (maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed))
                    return true;
                scrollVert(MultilineScrollController::Down, true, true);
                return true;
            case Qt::Key_Home:
                if (maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed))
                    return true;
                maEditorUi->getScrollController()->scrollToEnd(MultilineScrollController::SliderMinimum);
                return true;
            case Qt::Key_End:
                if (maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed))
                    return true;
                maEditorUi->getScrollController()->scrollToEnd(MultilineScrollController::SliderMaximum);
                return true;
            case Qt::Key_PageUp:
                if (maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed))
                    return true;
                scrollVert(MultilineScrollController::PageUp, false, true);
                return true;
            case Qt::Key_PageDown:
                if (maEditorUi->moveSelection(key, isShiftPressed, isCtrlPressed))
                    return true;
                scrollVert(MultilineScrollController::PageDown, false, true);
                return true;
        }
    }
    // default behavior
    return QObject::eventFilter(obj, event);
}

void MsaMultilineScrollArea::scrollVert(const MultilineScrollController::Directions& directions,
                                        bool byStep,
                                        bool wheel) {
    GScrollBar* globalVBar = maEditorUi->getScrollController()->getVerticalScrollBar();
    maEditorUi->setUpdatesEnabled(false);

    if (directions.testFlag(MultilineScrollController::SliderMoved)) {
        moveVSlider(globalVBar->value(),
                    globalVBar->sliderPosition(),
                    wheel ? directions : MultilineScrollController::None);
    } else if (byStep) {
        moveVSlider(globalVBar->value(),
                    globalVBar->sliderPosition(),
                    wheel ? directions : MultilineScrollController::None);
    } else {
        moveVSlider(globalVBar->value(),
                    globalVBar->sliderPosition(),
                    wheel ? directions : MultilineScrollController::None);
    }
    maEditorUi->setUpdatesEnabled(true);
}

void MsaMultilineScrollArea::moveVSlider(int currPos,
                                         int newPos,
                                         const MultilineScrollController::Directions& wheelDirections) {
    QScrollBar* vbar = verticalScrollBar();
    GScrollBar* globalVBar = maEditorUi->getScrollController()->getVerticalScrollBar();
    int currAreaScroll = vbar->value();
    int currGlobalScroll = globalVBar->value();
    int currFirstVisibleBase = maEditorUi->getFirstVisibleBase(0);
    int newAreaScroll = currAreaScroll;
    int newGlobalScroll = currGlobalScroll;
    int newFirstVisibleBase = currFirstVisibleBase;
    int linesCount = maEditorUi->getChildrenCount();
    int length = maEditorUi->getLastVisibleBase(0) + 1 - maEditorUi->getFirstVisibleBase(0);
    int fullLength = maEditor->getAlignmentLen();
    int lineHeight = maEditorUi->getUI(0)->height();
    int maxAreaScroll = vbar->maximum();
    int maxGlobalScroll = globalVBar->maximum();
    int direction = (newPos - currPos) > 0 ? 1 : (newPos - currPos) < 0 ? -1 : 0;
    int step = abs(newPos - currPos);

    if (wheelDirections.testFlag(MultilineScrollController::Down)) {
        direction = 1;
        step = verticalScrollBar()->singleStep();
    } else if (wheelDirections.testFlag(MultilineScrollController::Up)) {
        direction = -1;
        step = verticalScrollBar()->singleStep();
    } else if (wheelDirections.testFlag(MultilineScrollController::PageDown)) {
        direction = 1;
        step = verticalScrollBar()->pageStep();
    } else if (wheelDirections.testFlag(MultilineScrollController::PageUp)) {
        direction = -1;
        step = verticalScrollBar()->pageStep();
    }

    if (direction > 0) {
        if ((currAreaScroll + step) < maxAreaScroll) {
            newAreaScroll += step;
            newGlobalScroll += step;
        } else {
            newGlobalScroll = currGlobalScroll + step;
            if (newGlobalScroll >= maxGlobalScroll) {
                newGlobalScroll = maxGlobalScroll;
                newAreaScroll = maxAreaScroll;
                newFirstVisibleBase = (fullLength / length + (fullLength % length ? 1 : 0)) *
                                          length -
                                      length * maEditorUi->getChildrenCount();
            } else {
                newFirstVisibleBase = newGlobalScroll / lineHeight * length;
                newAreaScroll = newGlobalScroll - (newGlobalScroll / lineHeight) * lineHeight;
                while ((newFirstVisibleBase + length * (linesCount - 1)) > fullLength && newFirstVisibleBase >= 0) {
                    newFirstVisibleBase -= length;
                    if ((newAreaScroll + lineHeight) < maxAreaScroll) {
                        newAreaScroll += lineHeight;
                    } else {
                        newAreaScroll = maxAreaScroll;
                    }
                }
                if (newFirstVisibleBase < 0) {
                    newGlobalScroll = 0;
                    newFirstVisibleBase = 0;
                    newAreaScroll = 0;
                }
            }
        }
        maEditorUi->getScrollController()->setFirstVisibleBase(newFirstVisibleBase);
        if (newFirstVisibleBase != maEditorUi->getScrollController()->getFirstVisibleBase()) {
            newFirstVisibleBase = maEditorUi->getScrollController()->getFirstVisibleBase();
            newGlobalScroll = newFirstVisibleBase / length * lineHeight;
            newAreaScroll = newGlobalScroll - (newGlobalScroll / lineHeight) * lineHeight;
            vbar->setValue(newAreaScroll);
            globalVBar->setValue(newGlobalScroll);
        } else {
            vbar->setValue(newAreaScroll);
            globalVBar->setValue(newGlobalScroll);
        }
    } else if (direction < 0) {
        if ((currAreaScroll - step) > 0) {
            newAreaScroll -= step;
            newGlobalScroll -= step;
        } else {
            newGlobalScroll = currGlobalScroll - step;
            if (newGlobalScroll < 0) {
                newGlobalScroll = 0;
                newAreaScroll = 0;
            } else {
                newFirstVisibleBase = newGlobalScroll / lineHeight * length;
                newAreaScroll = newGlobalScroll - (newGlobalScroll / lineHeight) * lineHeight;
                while ((newFirstVisibleBase) < 0) {
                    newFirstVisibleBase += length;
                    if ((newAreaScroll - step) > 0) {
                        newAreaScroll -= step;
                    }
                }
            }
        }
        maEditorUi->getScrollController()->setFirstVisibleBase(newFirstVisibleBase);
        vbar->setValue(newAreaScroll);
        globalVBar->setValue(newGlobalScroll);
    }
}

void MsaMultilineScrollArea::wheelEvent(QWheelEvent* event) {
    if (maEditor->getMultilineMode()) {
        int inverted = event->inverted() ? -1 : 1;
        int direction = event->angleDelta().isNull()
                            ? 0
                            : event->angleDelta().y() == 0
                                  ? 0
                                  : inverted * (event->angleDelta().y() > 0 ? 1 : -1);

        if (direction == 0) {
            event->accept();
            return;
        } else if (direction < 0) {
            scrollVert(MultilineScrollController::Down, true, true);
            event->accept();
            return;
        } else if (direction > 0) {
            scrollVert(MultilineScrollController::Up, true, true);
            event->accept();
            return;
        }
    }
    QScrollArea::wheelEvent(event);
}

}  // namespace U2
