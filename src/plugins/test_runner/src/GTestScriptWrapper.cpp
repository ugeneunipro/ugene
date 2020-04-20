/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <QScriptContext>
#include <QScriptEngine>

#include "GTestScriptWrapper.h"

namespace U2 {

QScriptValue GTestScriptWrapper::qtestMouseClickE(QScriptContext *ctx, QScriptEngine *eng){
//mouseClick( QWidget * widget, Qt::MouseButton button, Qt::KeyboardModifiers modifier = 0, QPoint pos = QPoint(), int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<6){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Mouse Click can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::mouseClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::mouseClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::mouseClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint());
                    //result=true;
                    result="correctly";
                    break;
                case 5:
                    QTest::mouseClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint(),ctx->argument(4).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Mouse Click  no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError(" no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestMouseDClickE(QScriptContext *ctx, QScriptEngine *eng){
//void mouseDClick ( QWidget * widget, Qt::MouseButton button, Qt::KeyboardModifiers modifier = 0, QPoint pos = QPoint(), int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<6){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Mouse DClick can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::mouseDClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::mouseDClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::mouseDClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint());
                    //result=true;
                    result="correctly";
                    break;
                case 5:
                    QTest::mouseDClick(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint(),ctx->argument(4).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Mouse DClick  no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestMouseMoveE(QScriptContext *ctx, QScriptEngine *eng){
//void mouseMove ( QWidget * widget, QPoint pos = QPoint(), int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>0 && argsCount<4){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Mouse Move can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 1:
                    QTest::mouseMove(widget);
                    //result=true;
                    result="correctly";
                    break;
                case 2:
                    QTest::mouseMove(widget,ctx->argument(1).toVariant().toPoint());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::mouseMove(widget,ctx->argument(1).toVariant().toPoint(),ctx->argument(2).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Mouse Move  no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}
QScriptValue GTestScriptWrapper::qtestMousePressE(QScriptContext *ctx, QScriptEngine *eng){
//void mousePress ( QWidget * widget, Qt::MouseButton button, Qt::KeyboardModifiers modifier = 0, QPoint pos = QPoint(), int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<6){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Mouse Press can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::mousePress(widget,(Qt::MouseButton)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::mousePress(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::mousePress(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint());
                    //result=true;
                    result="correctly";
                    break;
                case 5:
                    QTest::mousePress(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint(),ctx->argument(4).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Mouse Press  no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestMouseReleaseE(QScriptContext *ctx, QScriptEngine *eng){
//void mouseRelease ( QWidget * widget, Qt::MouseButton button, Qt::KeyboardModifiers modifier = 0, QPoint pos = QPoint(), int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<6){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Mouse Release can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::mouseRelease(widget,(Qt::MouseButton)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::mouseRelease(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::mouseRelease(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint());
                    //result=true;
                    result="correctly";
                    break;
                case 5:
                    QTest::mouseRelease(widget,(Qt::MouseButton)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toVariant().toPoint(),ctx->argument(4).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Mouse Release no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestKeyClickE(QScriptContext *ctx, QScriptEngine *eng){
//void keyClick ( QWidget * widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<5){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Key Click can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::keyClick(widget,(Qt::Key)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::keyClick(widget,(Qt::Key)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::keyClick(widget,(Qt::Key)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Key Click no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestKeyClicksE(QScriptContext *ctx, QScriptEngine *eng){
//void keyClicks ( QWidget * widget, const QString & sequence, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<5){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Key Clicks can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::keyClicks(widget,ctx->argument(1).toString());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::keyClicks(widget,ctx->argument(1).toString(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::keyClicks(widget,ctx->argument(1).toString(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Key Clicks no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestKeyEventE(QScriptContext *ctx, QScriptEngine *eng){
//void keyEvent ( KeyAction action, QWidget * widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>2 && argsCount<6){
        QWidget* widget =(QWidget*) ctx->argument(1).toQObject();
        if(widget==NULL){
            result=" Key Event can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 3:
                    QTest::keyEvent((QTest::KeyAction) ctx->argument(0).toInt32(),widget,(Qt::Key)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::keyEvent((QTest::KeyAction) ctx->argument(0).toInt32(),widget,(Qt::Key)ctx->argument(2).toInt32(),(Qt::KeyboardModifiers)ctx->argument(3).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 5:
                    QTest::keyEvent((QTest::KeyAction) ctx->argument(0).toInt32(),widget,(Qt::Key)ctx->argument(2).toInt32(),(Qt::KeyboardModifiers)ctx->argument(3).toInt32(),ctx->argument(4).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Key Event no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestKeyPressE(QScriptContext *ctx, QScriptEngine *eng){
//void keyPress ( QWidget * widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<5){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Key Press can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::keyPress(widget,(Qt::Key)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::keyPress(widget,(Qt::Key)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::keyPress(widget,(Qt::Key)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Key Press no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestKeyReleaseE(QScriptContext *ctx, QScriptEngine *eng){
//void keyRelease ( QWidget * widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay = -1 )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount>1 && argsCount<5){
        QWidget* widget =(QWidget*) ctx->argument(0).toQObject();
        if(widget==NULL){
            result=" Key Release can't find widget ";
            ctx->throwError(" can't find widget ");
        }
        else{
            switch(argsCount){
                case 2:
                    QTest::keyRelease(widget,(Qt::Key)ctx->argument(1).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 3:
                    QTest::keyRelease(widget,(Qt::Key)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32());
                    //result=true;
                    result="correctly";
                    break;
                case 4:
                    QTest::keyRelease(widget,(Qt::Key)ctx->argument(1).toInt32(),(Qt::KeyboardModifiers)ctx->argument(2).toInt32(),ctx->argument(3).toInteger());
                    //result=true;
                    result="correctly";
                    break;
            }
        }
    }
    else{
        result.append("Key Release no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}

QScriptValue GTestScriptWrapper::qtestSleepE(QScriptContext *ctx, QScriptEngine *eng){
//void qSleep ( int ms )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount==1){
        switch(argsCount){

            case 1:
                QTest::qSleep(ctx->argument(0).toInteger());
                //result=true;
                result="correctly";
                break;
        }
    }
    else{
        result.append("qSleep no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}
QScriptValue GTestScriptWrapper::qtestWaitE(QScriptContext *ctx, QScriptEngine *eng){
//void qWait ( int ms )
    //bool result=false;
    QString result="bad result";
    int argsCount=ctx->argumentCount();
    if (argsCount==1){
        switch(argsCount){

            case 1:
                QTest::qWait(ctx->argument(0).toInteger());
                //result=true;
                result="correctly";
                break;
        }
    }
    else{
        result.append("qWait no overloaded function takes ");
        result.append(argsCount);
        result.append(" arguments");
        ctx->throwError("no overloaded function takes so arguments");
    }
    return QScriptValue(eng, result);
}
void GTestScriptWrapper::setQTest(QScriptEngine* curEng){
    QScriptValue qTestSV =(curEng->newArray());

    QScriptValue scriptFunctionMouseClickE = curEng->newFunction(qtestMouseClickE);
    qTestSV.setProperty("mouseClick", scriptFunctionMouseClickE);

    QScriptValue scriptFunctionMouseDClickE = curEng->newFunction(qtestMouseDClickE);
    qTestSV.setProperty("mouseDClick", scriptFunctionMouseDClickE);

    QScriptValue scriptFunctionMouseMoveE = curEng->newFunction(qtestMouseMoveE);
    qTestSV.setProperty("mouseMove", scriptFunctionMouseMoveE);

    QScriptValue scriptFunctionMousePressE = curEng->newFunction(qtestMousePressE);
    qTestSV.setProperty("mousePress", scriptFunctionMousePressE);

    QScriptValue scriptFunctionMouseReleaseE = curEng->newFunction(qtestMouseReleaseE);
    qTestSV.setProperty("mouseRelease", scriptFunctionMouseReleaseE);

    QScriptValue scriptFunctionKeyClickE= curEng->newFunction(qtestKeyClickE);
    qTestSV.setProperty("keyClick", scriptFunctionKeyClickE);

    QScriptValue scriptFunctionKeyClicksE = curEng->newFunction(qtestKeyClicksE);
    qTestSV.setProperty("keyClicks", scriptFunctionKeyClicksE);

    QScriptValue scriptFunctionKeyEventE = curEng->newFunction(qtestKeyEventE);
    qTestSV.setProperty("keyEvent", scriptFunctionKeyEventE);

    QScriptValue scriptFunctionKeyPressE = curEng->newFunction(qtestKeyPressE);
    qTestSV.setProperty("keyPress", scriptFunctionKeyPressE);

    QScriptValue scriptFunctionKeyReleaseE = curEng->newFunction(qtestKeyReleaseE);
    qTestSV.setProperty("keyRelease", scriptFunctionKeyReleaseE);

    QScriptValue scriptFunctionSleepE = curEng->newFunction(qtestSleepE);
    qTestSV.setProperty("qSleep", scriptFunctionSleepE);

    QScriptValue scriptFunctionWaitE = curEng->newFunction(qtestWaitE);
    qTestSV.setProperty("qWait", scriptFunctionWaitE);

    curEng->globalObject().setProperty("QTest", qTestSV);
}
}
