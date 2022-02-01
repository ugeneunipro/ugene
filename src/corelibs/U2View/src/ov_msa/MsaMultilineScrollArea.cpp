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
    int rowHeight = maEditor->getSequenceRowHeight();
    int step = rowHeight / 2;

    verticalScrollBar()->setSingleStep(step);
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
        } else if (verticalScrollBar()->value() == verticalScrollBar()->maximum() && direction < 0) {
            maEditorUi->setUpdatesEnabled(false);

            maEditorUi->updateChildrenCount();

            int linesCount = maEditorUi->getChildrenCount();
            int index = linesCount - 1;
            int width = maEditorUi->getSequenceAreaBaseWidth(index);
            int fullLength = maEditor->getAlignmentLen();
            int newScrollValue = maEditorUi->getScrollController()->getFirstVisibleBase(0) + width;

            if (maEditorUi->getLastVisibleBase(index) < (fullLength - width)) {
                int lineHeight = maEditorUi->getUI(0)->height();
                int vertValue = verticalScrollBar()->value();
                verticalScrollBar()->setValue(vertValue - lineHeight + verticalScrollBar()->singleStep());
            }
            maEditorUi->getScrollController()->setFirstVisibleBase(newScrollValue);

            maEditorUi->setUpdatesEnabled(true);
            event->accept();
            return;
        } else if (verticalScrollBar()->value() == verticalScrollBar()->minimum() && direction > 0) {
            maEditorUi->setUpdatesEnabled(false);

            int index = 0;
            int width = maEditorUi->getSequenceAreaBaseWidth(index);
            int newScrollValue = maEditorUi->getScrollController()->getFirstVisibleBase(index) - width;

            if (maEditorUi->getFirstVisibleBase(index) > width) {
                int lineHeight = maEditorUi->getUI(0)->height();
                verticalScrollBar()->setValue(lineHeight - verticalScrollBar()->singleStep());
            }
            maEditorUi->getScrollController()->setFirstVisibleBase(newScrollValue);

            maEditorUi->setUpdatesEnabled(true);
            event->accept();
            return;
        }
    }
    QScrollArea::wheelEvent(event);
}

}  // namespace U2
