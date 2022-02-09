#include <QScrollBar>
#include <QWheelEvent>

#include <U2Gui/GScrollBar.h>
#include "MaEditor.h"
#include "MaEditorWgt.h"
#include "MsaMultilineScrollArea.h"
#include "MultilineScrollController.h"
#include "ScrollController.h"
#include "RowHeightController.h"

namespace U2 {

MsaMultilineScrollArea::MsaMultilineScrollArea(MaEditor *maEditor, MaEditorMultilineWgt *maEditorUi)
    : QScrollArea(maEditorUi),
      maEditor(maEditor),
      maEditorUi(maEditorUi)
{
    verticalScrollBar()->setSingleStep(maEditor->getSequenceRowHeight());
}

void MsaMultilineScrollArea::vertScroll(const MultilineScrollController::Directions &directions,
                                        bool byStep)
{
    QScrollBar *vbar = verticalScrollBar();
    maEditorUi->setUpdatesEnabled(false);
    maEditorUi->updateChildrenCount();

    if (directions.testFlag(MultilineScrollController::SliderMinimum)) {
        vbar->setValue(0);
        maEditorUi->getScrollController()->setFirstVisibleBase(0);
    } else if (directions.testFlag(MultilineScrollController::SliderMaximum)) {
        vbar->setValue(vbar->maximum());
        maEditorUi->getScrollController()->setFirstVisibleBase(
            maEditor->getAlignmentLen() - maEditorUi->getSequenceAreaAllBaseLen());
    } else if (directions.testFlag(MultilineScrollController::SliderMoved)) {
        QScrollBar *vScrollBar = maEditorUi->getScrollController()->getVerticalScrollBar();
        int vSliderPos = vScrollBar->sliderPosition();

        if (vSliderPos == vScrollBar->maximum()) {
            vertScroll(MultilineScrollController::SliderMaximum, byStep);
        } else if (vSliderPos == 0) {
            vertScroll(MultilineScrollController::SliderMinimum, byStep);
        } else {
            int length = maEditorUi->getFirstVisibleBase(1) - maEditorUi->getFirstVisibleBase(0);
            int width = maEditorUi->getSequenceAreaWidth();

            vbar->setValue(maEditorUi->getUI(0)->height() / 2);
            maEditorUi->getScrollController()->setFirstVisibleBase(
                vSliderPos / width * (length - 1));
        }
    } else if (byStep) {
        if (directions.testFlag(MultilineScrollController::Down)) {
            if (vbar->value() == vbar->maximum()) {
                int linesCount = maEditorUi->getChildrenCount();
                int index = linesCount - 1;
                int length = maEditorUi->getFirstVisibleBase(1)
                             - maEditorUi->getFirstVisibleBase(0);
                int fullLength = maEditor->getAlignmentLen();
                int newScrollValue = maEditorUi->getLastVisibleBase(0) + 1;

                if (maEditorUi->getLastVisibleBase(index) < (fullLength - length)) {
                    int lineHeight = maEditorUi->getUI(0)->height();
                    int vertValue = vbar->value();

                    vbar->setValue(vertValue - lineHeight + vbar->singleStep());
                }
                maEditorUi->getScrollController()->setFirstVisibleBase(newScrollValue);
            } else {
                vbar->setValue(vbar->value() + vbar->singleStep());
            }
        } else if (directions.testFlag(MultilineScrollController::Up)) {
            if (vbar->value() == vbar->minimum()) {
                int index = 0;
                int length = maEditorUi->getFirstVisibleBase(1)
                             - maEditorUi->getFirstVisibleBase(0);
                int newScrollValue = maEditorUi->getFirstVisibleBase(index) - length;

                if (maEditorUi->getFirstVisibleBase(index) > length) {
                    int lineHeight = maEditorUi->getUI(0)->height();
                    vbar->setValue(lineHeight - vbar->singleStep());
                }
                maEditorUi->getScrollController()->setFirstVisibleBase(newScrollValue);
            } else {
                vbar->setValue(vbar->value() - vbar->singleStep());
            }
        }
    } else {
        int currScroll = vbar->value();
        int linesCount = maEditorUi->getChildrenCount();
        int lastIndex = linesCount - 1;
        int length = maEditorUi->getFirstVisibleBase(1) - maEditorUi->getFirstVisibleBase(0);
        int fullLength = maEditor->getAlignmentLen();
        int firstBase = maEditorUi->getFirstVisibleBase(0);
        int lineHeight = maEditorUi->getUI(0)->height();

        if (directions.testFlag(MultilineScrollController::Down)) {
            int offsetBase = (height() + currScroll) / lineHeight * (length - 1);
            int newScroll = (height() + currScroll) % lineHeight;

            if ((maEditorUi->getLastVisibleBase(lastIndex) + offsetBase) <= fullLength) {
                maEditorUi->getScrollController()->setFirstVisibleBase(
                    maEditorUi->getFirstVisibleBase() + offsetBase);
                verticalScrollBar()->setValue(newScroll);
            } else {
                maEditorUi->getScrollController()->setFirstVisibleBase(fullLength - maEditorUi->getSequenceAreaAllBaseLen());
            }
        } else if (directions.testFlag(MultilineScrollController::Up)) {
            int offsetBase = (abs(currScroll - height()) / lineHeight + 1) * (length - 1);
            int newScroll = lineHeight - abs(currScroll - height()) % lineHeight;

            if ((firstBase - offsetBase) >= 0) {
                maEditorUi->getScrollController()->setFirstVisibleBase(firstBase - offsetBase);
                verticalScrollBar()->setValue(newScroll);
            } else {
                maEditorUi->getScrollController()->setFirstVisibleBase(0);
            }
        }
    }
    maEditorUi->setUpdatesEnabled(true);
}

void MsaMultilineScrollArea::wheelEvent(QWheelEvent *event)
{
    if (maEditorUi->getMultilineMode()) {
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
            vertScroll(MultilineScrollController::Down, true);
            event->accept();
            return;
        } else if (direction > 0) {
            vertScroll(MultilineScrollController::Up, true);
            event->accept();
            return;
        }
    }
    QScrollArea::wheelEvent(event);
}

}  // namespace U2
