#ifndef MSAMULTILINESCROLLAREA_H
#define MSAMULTILINESCROLLAREA_H

#include <QScrollArea>

#include <U2Core/global.h>

#include "MsaEditorMultilineWgt.h"
#include "MultilineScrollController.h"

namespace U2 {

class GScrollBar;

class U2VIEW_EXPORT MsaMultilineScrollArea : public QScrollArea {
    Q_OBJECT

public:
    MsaMultilineScrollArea(MaEditor* maEditor, MaEditorMultilineWgt* ui);
    // wheel arg signals that this is non scrollbar scrolling
    void scrollVert(const MultilineScrollController::Directions& directions,
                    bool byStep,
                    bool wheel = false);
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    void wheelEvent(QWheelEvent* we) override;

private:
    MaEditor* maEditor;
    MaEditorMultilineWgt* maEditorUi;

    void moveVSlider(int currPos,
                     int newPos,
                     const MultilineScrollController::Directions& wheelDirections);
};

}  // namespace U2

#endif  // MSAMULTILINESCROLLAREA_H
