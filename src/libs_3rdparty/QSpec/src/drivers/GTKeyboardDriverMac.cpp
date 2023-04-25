/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,¡¡
 * MA 02110-1301, USA.
 */

#include "GTGlobals.h"
#include "GTKeyboardDriver.h"

#ifdef Q_OS_DARWIN
#    include <ApplicationServices/ApplicationServices.h>
#    include <Carbon/Carbon.h>
#endif

namespace HI {

#ifdef Q_OS_DARWIN
static int asciiToVirtual(char key) {
    if (isalpha(key)) {
        key = tolower(key);
    }
    switch (key) {
        case ' ':
            return kVK_Space;
        case '0':
            return kVK_ANSI_0;
        case '1':
            return kVK_ANSI_1;
        case '2':
            return kVK_ANSI_2;
        case '3':
            return kVK_ANSI_3;
        case '4':
            return kVK_ANSI_4;
        case '5':
            return kVK_ANSI_5;
        case '6':
            return kVK_ANSI_6;
        case '7':
            return kVK_ANSI_7;
        case '8':
            return kVK_ANSI_8;
        case '9':
            return kVK_ANSI_9;
        case 'a':
            return kVK_ANSI_A;
            break;
        case 'b':
            return kVK_ANSI_B;
        case 'c':
            return kVK_ANSI_C;
        case 'd':
            return kVK_ANSI_D;
        case 'e':
            return kVK_ANSI_E;
        case 'f':
            return kVK_ANSI_F;
        case 'g':
            return kVK_ANSI_G;
        case 'h':
            return kVK_ANSI_H;
        case 'i':
            return kVK_ANSI_I;
        case 'j':
            return kVK_ANSI_J;
        case 'k':
            return kVK_ANSI_K;
        case 'l':
            return kVK_ANSI_L;
        case 'm':
            return kVK_ANSI_M;
        case 'n':
            return kVK_ANSI_N;
        case 'o':
            return kVK_ANSI_O;
        case 'p':
            return kVK_ANSI_P;
        case 'q':
            return kVK_ANSI_Q;
        case 'r':
            return kVK_ANSI_R;
        case 's':
            return kVK_ANSI_S;
        case 't':
            return kVK_ANSI_T;
        case 'u':
            return kVK_ANSI_U;
        case 'v':
            return kVK_ANSI_V;
        case 'w':
            return kVK_ANSI_W;
        case 'x':
            return kVK_ANSI_X;
        case 'y':
            return kVK_ANSI_Y;
        case 'z':
            return kVK_ANSI_Z;
        case '=':
            return kVK_ANSI_Equal;
        case '-':
            return kVK_ANSI_Minus;
        case ']':
            return kVK_ANSI_RightBracket;
        case '[':
            return kVK_ANSI_LeftBracket;
        case '\'':
            return kVK_ANSI_Quote;
        case ';':
            return kVK_ANSI_Semicolon;
        case '\\':
            return kVK_ANSI_Backslash;
        case ',':
            return kVK_ANSI_Comma;
        case '/':
            return kVK_ANSI_Slash;
        case '.':
            return kVK_ANSI_Period;
        case '\n':
            return kVK_Return;
    }
    return key;
}

static char toKeyWithNoShift(char key) {
    switch (key) {
        case '_':
            return '-';
        case '+':
            return '=';
        case '<':
            return ',';
        case '>':
            return '.';
        case ')':
            return '0';
        case '!':
            return '1';
        case '@':
            return '2';
        case '#':
            return '3';
        case '$':
            return '4';
        case '%':
            return '5';
        case '^':
            return '6';
        case '&':
            return '7';
        case '*':
            return '8';
        case '(':
            return '9';
        case '\"':
            return '\'';
        case '|':
            return '\\';
        case ':':
            return ';';
        case '{':
            return '[';
        case '}':
            return ']';
    }
    return key;
}

static bool keyPressMac(CGKeyCode key) {
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, key, true);
    DRIVER_CHECK(event != NULL, "Can't create event");
    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(10);
    return true;
}

static bool keyReleaseMac(CGKeyCode key) {
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, key, false);
    DRIVER_CHECK(event != NULL, "Can't create event");
    CGEventPost(kCGSessionEventTap, event);
    CFRelease(event);
    GTGlobals::sleep(10);
    return true;
}

#    define GT_CLASS_NAME "GTKeyboardDriverMac"
#    define GT_METHOD_NAME "keyPress_char"
bool GTKeyboardDriver::keyPress(char origKey, Qt::KeyboardModifiers modifiers) {
    DRIVER_CHECK(origKey != 0, "key = 0");
    char keyWithNoShift = toKeyWithNoShift(origKey);
    bool isShiftRequired = origKey != keyWithNoShift;
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        if (mod == Qt::Key_Shift && isShiftRequired) {
            continue;
        }
        keyPressMac(GTKeyboardDriver::key[mod]);
    }
    if (isShiftRequired) {
        keyPressMac(GTKeyboardDriver::key[Qt::Key_Shift]);
    }
    CGKeyCode keyCode = asciiToVirtual(keyWithNoShift);
    return keyPressMac(keyCode);
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "keyRelease_char"
bool GTKeyboardDriver::keyRelease(char origKey, Qt::KeyboardModifiers modifiers) {
    DRIVER_CHECK(origKey != 0, "key = 0");
    char keyWithNoShift = toKeyWithNoShift(origKey);
    bool isShiftRequired = origKey != keyWithNoShift;

    CGKeyCode keyCode = asciiToVirtual(keyWithNoShift);
    keyReleaseMac(keyCode);
    if (isShiftRequired) {
        keyReleaseMac(GTKeyboardDriver::key[Qt::Key_Shift]);
    }
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        if (mod == Qt::Key_Shift && isShiftRequired) continue;
        keyReleaseMac(GTKeyboardDriver::key[mod]);
    }
    return true;
}
#    undef GT_METHOD_NAME

bool GTKeyboardDriver::keyPress(Qt::Key key, Qt::KeyboardModifiers modifiers) {
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        keyPressMac(GTKeyboardDriver::key[mod]);
    }
    return keyPressMac(GTKeyboardDriver::key[key]);
}

bool GTKeyboardDriver::keyRelease(Qt::Key key, Qt::KeyboardModifiers modifiers) {
    keyReleaseMac(GTKeyboardDriver::key[key]);

    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modKeys) {
        keyReleaseMac(GTKeyboardDriver::key[mod]);
    }

    return true;
}

GTKeyboardDriver::keys::keys() {
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
#    undef GT_CLASS_NAME

#endif
}  // namespace HI
