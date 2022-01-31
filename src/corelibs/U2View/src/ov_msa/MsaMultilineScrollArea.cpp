#include <QScrollBar>
#include <QWheelEvent>

#include <U2Gui/GScrollBar.h>
#include "MaEditor.h"
#include "MsaMultilineScrollArea.h"
#include "MultilineScrollController.h"

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
        int val = maEditorUi->getScrollController()->getVerticalScrollBar()->value();
        const int columnWidth = maEditor->getColumnWidth();
        //const int rowHeight = maEditor->getSequenceRowHeight();

        if (direction == 0) {
            event->accept();
            return;
        } else if (verticalScrollBar()->value() == verticalScrollBar()->maximum() && direction < 0) {
            maEditorUi->getScrollController()->getVerticalScrollBar()->setValue(val + columnWidth);
            event->accept();
            return;
        } else if (verticalScrollBar()->value() == verticalScrollBar()->minimum() && direction > 0) {
            maEditorUi->getScrollController()->getVerticalScrollBar()->setValue(val - columnWidth);
            event->accept();
            return;
        }
    }
    QScrollArea::wheelEvent(event);
}

}  // namespace U2
