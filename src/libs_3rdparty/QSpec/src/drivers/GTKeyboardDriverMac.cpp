/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "GTKeyboardDriver.h"
#include "GTGlobals.h"

#ifdef  Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#endif

namespace HI {

#ifdef  Q_OS_MAC

int asciiToVirtual(int);
bool extractShiftModifier(char &key);
bool keyPressMac(int key);
bool keyReleaseMac(int key);

#define GT_CLASS_NAME "GTKeyboardDriverMac"
#define GT_METHOD_NAME "keyPress_char"
bool  GTKeyboardDriver::keyPress(char key, Qt::KeyboardModifiers modifiers) {
    DRIVER_CHECK(key != 0, "key = 0");

    const bool isChanged = extractShiftModifier(key);
    if (isChanged) {
        keyPressMac(GTKeyboardDriver::key[Qt::Key_Shift]);
    } else {
        key = asciiToVirtual(key);
    }

    GTGlobals::sleep(1);
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        keyPressMac(GTKeyboardDriver::key[mod]);
    }

    return keyPressMac((int)key);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "keyRelease_char"
bool GTKeyboardDriver::keyRelease(char key, Qt::KeyboardModifiers modifiers) {
    DRIVER_CHECK(key != 0, "key = 0");

    const bool isChanged = extractShiftModifier(key);
    if (!isChanged) {
        key = asciiToVirtual(key);
    } else {
        keyReleaseMac(GTKeyboardDriver::key[Qt::Key_Shift]);
    }

    GTGlobals::sleep(1);
    keyReleaseMac((int)key);
    GTGlobals::sleep(1);

    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        keyReleaseMac(GTKeyboardDriver::key[mod]);
    }
    GTGlobals::sleep(1);

    return true;
}
#undef GT_METHOD_NAME

bool GTKeyboardDriver::keyPress(Qt::Key key, Qt::KeyboardModifiers modifiers){
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        keyPressMac(GTKeyboardDriver::key[mod]);
    }
    return keyPressMac(GTKeyboardDriver::key[key]);
}

bool GTKeyboardDriver::keyRelease(Qt::Key key, Qt::KeyboardModifiers modifiers){
    keyReleaseMac(GTKeyboardDriver::key[key]);

    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        keyReleaseMac(GTKeyboardDriver::key[mod]);
    }

    return true;
}

GTKeyboardDriver::keys::keys()
{
    ADD_KEY(Qt::Key_Control, kVK_Command);
    ADD_KEY(Qt::Key_Tab, kVK_Tab);
    ADD_KEY(Qt::Key_Enter, kVK_Return);
    ADD_KEY(Qt::Key_Shift, kVK_Shift);
    ADD_KEY(Qt::Key_Meta, kVK_Control);
    ADD_KEY(Qt::Key_Alt, kVK_Option);
    ADD_KEY(Qt::Key_Escape, kVK_Escape);
    ADD_KEY(Qt::Key_Space, kVK_Space);
    ADD_KEY(Qt::Key_Left, kVK_LeftArrow);
    ADD_KEY(Qt::Key_Up, kVK_UpArrow);
    ADD_KEY(Qt::Key_Right, kVK_RightArrow);
    ADD_KEY(Qt::Key_Down, kVK_DownArrow);
    ADD_KEY(Qt::Key_Delete, kVK_ForwardDelete);
    ADD_KEY(Qt::Key_Backspace, kVK_Delete);
    ADD_KEY(Qt::Key_Help, kVK_Help);
    ADD_KEY(Qt::Key_F1, kVK_F1);
    ADD_KEY(Qt::Key_F2, kVK_F2);
    ADD_KEY(Qt::Key_F3, kVK_F3);
    ADD_KEY(Qt::Key_F4, kVK_F4);
    ADD_KEY(Qt::Key_F5, kVK_F5);
    ADD_KEY(Qt::Key_F6, kVK_F6);
    ADD_KEY(Qt::Key_F7, kVK_F7);
    ADD_KEY(Qt::Key_F8, kVK_F8);
    ADD_KEY(Qt::Key_F9, kVK_F9);
    ADD_KEY(Qt::Key_F10, kVK_F10);
    ADD_KEY(Qt::Key_F12, kVK_F12);
    ADD_KEY(Qt::Key_Home, kVK_Home);
    ADD_KEY(Qt::Key_End, kVK_End);
    ADD_KEY(Qt::Key_PageUp, kVK_PageUp);
    ADD_KEY(Qt::Key_PageDown, kVK_PageDown);

// feel free to add other keys
// macro kVK_* defined in Carbon.framework/Frameworks/HIToolbox.framework/Headers/Events.h
}

#undef GT_CLASS_NAME

int asciiToVirtual(int key)
{
    if (isalpha(key)) {
        key = tolower(key);
    }

    switch(key) {
    case ' ':
        key = kVK_Space;
        break;
    case '0':
        key = kVK_ANSI_0;
        break;
    case '1':
        key = kVK_ANSI_1;
        break;
    case '2':
        key = kVK_ANSI_2;
        break;
    case '3':
        key = kVK_ANSI_3;
        break;
    case '4':
        key = kVK_ANSI_4;
        break;
    case '5':
        key = kVK_ANSI_5;
        break;
    case '6':
        key = kVK_ANSI_6;
        break;
    case '7':
        key = kVK_ANSI_7;
        break;
    case '8':
        key = kVK_ANSI_8;
        break;
    case '9':
        key = kVK_ANSI_9;
        break;
    case 'a':
        key = kVK_ANSI_A;
        break;
    case 'b':
        key = kVK_ANSI_B;
        break;
    case 'c':
        key = kVK_ANSI_C;
        break;
    case 'd':
        key = kVK_ANSI_D;
        break;
    case 'e':
        key = kVK_ANSI_E;
        break;
    case 'f':
        key = kVK_ANSI_F;
        break;
    case 'g':
        key = kVK_ANSI_G;
        break;
    case 'h':
        key = kVK_ANSI_H;
        break;
    case 'i':
        key = kVK_ANSI_I;
        break;
    case 'j':
        key = kVK_ANSI_J;
        break;
    case 'k':
        key = kVK_ANSI_K;
        break;
    case 'l':
        key = kVK_ANSI_L;
        break;
    case 'm':
        key = kVK_ANSI_M;
        break;
    case 'n':
        key = kVK_ANSI_N;
        break;
    case 'o':
        key = kVK_ANSI_O;
        break;
    case 'p':
        key = kVK_ANSI_P;
        break;
    case 'q':
        key = kVK_ANSI_Q;
        break;
    case 'r':
        key = kVK_ANSI_R;
        break;
    case 's':
        key = kVK_ANSI_S;
        break;
    case 't':
        key = kVK_ANSI_T;
        break;
    case 'u':
        key = kVK_ANSI_U;
        break;
    case 'v':
        key = kVK_ANSI_V;
        break;
    case 'w':
        key = kVK_ANSI_W;
        break;
    case 'x':
        key = kVK_ANSI_X;
        break;
    case 'y':
        key = kVK_ANSI_Y;
        break;
    case 'z':
        key = kVK_ANSI_Z;
        break;
    case '=':
        key = kVK_ANSI_Equal;
        break;
    case '-':
        key = kVK_ANSI_Minus;
        break;
    case ']':
        key = kVK_ANSI_RightBracket;
        break;
    case '[':
        key = kVK_ANSI_LeftBracket;
        break;
    case '\'':
        key = kVK_ANSI_Quote;
        break;
    case ';':
        key = kVK_ANSI_Semicolon;
        break;
    case '\\':
        key = kVK_ANSI_Backslash;
        break;
    case ',':
        key = kVK_ANSI_Comma;
        break;
    case '/':
        key = kVK_ANSI_Slash;
        break;
    case '.':
        key = kVK_ANSI_Period;
        break;
    case '\n':
        key = kVK_Return;
        break;
    }

    return key;
}

bool extractShiftModifier(char &key) {
    switch(key) {
    case '_':
        key = asciiToVirtual('-');
        return true;
    case '+':
        key = asciiToVirtual('=');
        return true;
    case '<':
        key = asciiToVirtual(',');
        return true;
    case '>':
        key = asciiToVirtual('.');
        return true;
    case ')':
        key = asciiToVirtual('0');
        return true;
    case '!':
        key = asciiToVirtual('1');
        return true;
    case '@':
        key = asciiToVirtual('2');
        return true;
    case '#':
        key = asciiToVirtual('3');
        return true;
    case '$':
        key = asciiToVirtual('4');
        return true;
    case '%':
        key = asciiToVirtual('5');
        return true;
    case '^':
        key = asciiToVirtual('6');
        return true;
    case '&':
        key = asciiToVirtual('7');
        return true;
    case '*':
        key = asciiToVirtual('8');
        return true;
    case '(':
        key = asciiToVirtual('9');
        return true;
    case '\"':
        key = asciiToVirtual('\'');
        return true;
    case '|':
        key = asciiToVirtual('\\');
        return true;
    case ':':
        key = asciiToVirtual(';');
        return true;
    case '{':
        key = asciiToVirtual('[');
        return true;
    case '}':
        key = asciiToVirtual(']');
        return true;
    }

    return false;
}

bool keyPressMac(int key){
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, key, true);
    DRIVER_CHECK(event != NULL, "Can't create event");
    CGEventSetFlags(event, CGEventGetFlags(event) & ~kCGEventFlagMaskNumericPad);

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(1);

    return true;
}

bool keyReleaseMac(int key){
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, key, false);
    DRIVER_CHECK(event != NULL, "Can't create event");
    CGEventSetFlags(event, CGEventGetFlags(event) & ~kCGEventFlagMaskNumericPad);

    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(1);

    return true;
}

#endif
} //namespace
