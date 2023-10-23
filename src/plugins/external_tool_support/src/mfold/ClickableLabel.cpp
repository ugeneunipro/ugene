// https://wiki.qt.io/Clickable_QLabel

#include "ClickableLabel.h"

namespace U2 {
ClickableLabel::ClickableLabel(QWidget* p, Qt::WindowFlags f)
    : QLabel(p) {
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}
}  // namespace U2
