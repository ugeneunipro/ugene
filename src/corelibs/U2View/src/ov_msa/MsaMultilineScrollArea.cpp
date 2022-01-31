#include <QScrollBar>
#include <QWheelEvent>

#include <U2Gui/GScrollBar.h>
#include "MaEditor.h"
#include "MaEditorWgt.h"
#include "MsaMultilineScrollArea.h"
#include "MultilineScrollController.h"
#include "ScrollController.h"

namespace U2 {

MsaMultilineScrollArea::MsaMultilineScrollArea(MaEditor *maEditor, MaEditorMultilineWgt *maEditorUi)
    : QScrollArea(maEditorUi),
      maEditor(maEditor),
      maEditorUi(maEditorUi)
{

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
            int index = maEditorUi->getChildrenCount() - 1;
            int width = maEditorUi->getSequenceAreaBaseWidth(index);
            int newScrollValue = maEditorUi->getScrollController()->getFirstVisibleBase(0) + width;

            maEditorUi->setUpdatesEnabled(false);
            maEditorUi->getScrollController()->setFirstVisibleBase(newScrollValue);
            maEditorUi->setUpdatesEnabled(true);

            event->accept();
            return;
        } else if (verticalScrollBar()->value() == verticalScrollBar()->minimum() && direction > 0) {
            int index = 0;
            int width = maEditorUi->getSequenceAreaBaseWidth(index);
            int newScrollValue = maEditorUi->getScrollController()->getFirstVisibleBase(index) - width;

            maEditorUi->setUpdatesEnabled(false);
            maEditorUi->getScrollController()->setFirstVisibleBase(newScrollValue);
            maEditorUi->setUpdatesEnabled(true);

            event->accept();
            return;
        }
    }
    QScrollArea::wheelEvent(event);
}

}  // namespace U2
