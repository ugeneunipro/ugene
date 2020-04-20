#include "EventRecorderWidget.h"

#include <QEvent>
#include <QKeyEvent>

EventRecorderWidget::EventRecorderWidget(QWidget *parent) :
    QWidget(parent)
{
    setGeometry(0,50,50,50);
    setFocusPolicy(Qt::ClickFocus);

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkBlue);
    setAutoFillBackground(true);
    setPalette(Pal);
}


