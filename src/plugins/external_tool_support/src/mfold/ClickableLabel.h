// https://wiki.qt.io/Clickable_QLabel

#pragma once
#include <QLabel>

namespace U2 {
class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
};
}  // namespace U2
