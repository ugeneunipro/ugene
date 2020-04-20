#include "EventFilter.h"
#include <QDebug>

KeyEvent::KeyEvent(){
    key = Qt::Key_unknown;
    type = QEvent::None;
}

KeyEvent::KeyEvent(QKeyEvent *e){
    key = (Qt::Key)e->key();
    type = e->type();
}

Qt::Key KeyEvent::getKey(){return key;}
QEvent::Type KeyEvent::getType(){return type;}

MouseEvent::MouseEvent(){
    button = Qt::NoButton;
    type = QEvent::None;
}

MouseEvent::MouseEvent(QMouseEvent *e){
    button = e->button();
    type = e->type();
}

Qt::MouseButton MouseEvent::getButton(){return button;}
QEvent::Type MouseEvent::getType(){return type;}

EventFilter::EventFilter(QObject *parent) :
    QObject(parent)
{
}

bool EventFilter::eventFilter(QObject *o, QEvent *e){
    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
    if(keyEvent != NULL){
        QEvent::Type type = keyEvent->type();
        if(type == QEvent::KeyPress || type == QEvent::KeyRelease){
            keyEvents.append(KeyEvent(keyEvent));
            return QObject::eventFilter(o,e);
        }
    }

    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(e);
    if(mouseEvent != NULL){
        QEvent::Type type = mouseEvent->type();
        if(type == QEvent::MouseButtonDblClick || type == QEvent::MouseButtonPress || type == QEvent::MouseButtonRelease){
            mouseEvents.append(MouseEvent(mouseEvent));
            return QObject::eventFilter(o,e);
        }
    }
    return QObject::eventFilter(o,e);
}
