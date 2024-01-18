// https://wiki.qt.io/Clickable_QLabel

#include "ClickableLabel.h"

namespace U2 {
ClickableLabel::ClickableLabel(QWidget* p, Qt::WindowFlags)
    : QLabel(p) {
}

void ClickableLabel::mousePressEvent(QMouseEvent*) {
    emit clicked();
}
}  // namespace U2
