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

#include "QtGlobal"

#ifdef Q_OS_DARWIN

#    include <ApplicationServices/ApplicationServices.h>
#    include <Carbon/Carbon.h>

#    include "GTGlobals.h"
#    include "GTKeyboardDriver.h"

namespace HI {

static bool shiftDown = false;
static bool ctrlDown = false;
static bool altDown = false;
static bool cmdDown = false;
static bool fnDown = false;

static int asciiToVirtual(char key) {
    if (isalpha(key)) {
        key = (char)tolower(key);
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
        default:
            break;
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
        default:
            break;
    }
    return key;
}

static void keyPressMac(CGKeyCode key) {
    if (key == kVK_Shift) {
        shiftDown = true;
    } else if (key == kVK_Control) {
        ctrlDown = true;
    } else if (key == kVK_Option) {
        altDown = true;
    } else if (key == kVK_Command) {
        cmdDown = true;
    } else if (key == kVK_Function) {
        fnDown = true;
    }

    CGEventFlags flags = 0;
    if (shiftDown) {
        flags = flags | kCGEventFlagMaskShift;
    }
    if (ctrlDown) {
        flags = CGEventFlags(flags | kCGEventFlagMaskControl);
    }
    if (altDown) {
        flags = CGEventFlags(flags | kCGEventFlagMaskAlternate);
    }
    if (cmdDown) {
        flags = CGEventFlags(flags | kCGEventFlagMaskCommand);
    }
    if (fnDown) {
        flags = CGEventFlags(flags | kCGEventFlagMaskSecondaryFn);
    }
    CGEventSourceRef source = flags == 0 ? nullptr : CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
    CGEventRef command = CGEventCreateKeyboardEvent(source, key, true);
    if (flags != 0) {
        CGEventSetFlags(command, flags);
    }
    CGEventPost(kCGSessionEventTap, command);
    GTGlobals::sleep(10);
    CFRelease(command);
    if (source != nullptr) {
        CFRelease(source);
    }
}

static void dumpState(const char* action) {
    auto state = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);

    qDebug("============= Dump keyboard state %s", action);
    qDebug("maskAlphaShift %llu", state & kCGEventFlagMaskAlphaShift);
    qDebug("maskShift %llu", state & kCGEventFlagMaskShift);
    qDebug("maskControl %llu", state & kCGEventFlagMaskControl);
    qDebug("maskCommand %llu", state & kCGEventFlagMaskCommand);
    qDebug("maskAlternate %llu", state & kCGEventFlagMaskAlternate);
    qDebug("maskHelp %llu", state & kCGEventFlagMaskHelp);
    qDebug("maskSecondaryFn %llu", state & kCGEventFlagMaskSecondaryFn);
    qDebug("maskNumericPad %llu", state & kCGEventFlagMaskNumericPad);
    qDebug("maskNonCoalesced %llu", state & kCGEventFlagMaskNonCoalesced);
    qDebug("=============");
}

static bool keyReleaseMac(CGKeyCode key, int attempt = 1) { // NOLINT(misc-no-recursion)
    CGEventFlags modifierFlagToClean = 0;
    if (key == kVK_Shift) {
        shiftDown = false;
        modifierFlagToClean = kCGEventFlagMaskShift;
    } else if (key == kVK_Control) {
        ctrlDown = false;
        modifierFlagToClean = kCGEventFlagMaskControl;
    } else if (key == kVK_Option) {
        altDown = false;
        modifierFlagToClean = kCGEventFlagMaskAlternate;
    } else if (key == kVK_Command) {
        cmdDown = false;
        modifierFlagToClean = kCGEventFlagMaskCommand;
    } else if (key == kVK_Function) {
        fnDown = false;
        modifierFlagToClean = kCGEventFlagMaskSecondaryFn;
    }
    CGEventRef command = CGEventCreateKeyboardEvent(nullptr, key, false);
    CGEventPost(kCGSessionEventTap, command);
    GTGlobals::sleep(10);
    CFRelease(command);

    if (modifierFlagToClean != 0) {
        // Sometimes (unclear why?) MacOS does not remove modifier. This may be a timing issue.
        // In this case we repeat the request to clean.
        auto state = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
        if (state & modifierFlagToClean) {
            DRIVER_CHECK(attempt < 10, QString("Failed to clean modifier in 10 attempts: %1").arg(key));
            GTGlobals::sleep(50);
            dumpState("keyReleaseMac before retry");
            keyReleaseMac(key, attempt + 1);
            dumpState("keyReleaseMac after retry");
        }
    }
}

#    define GT_CLASS_NAME "GTKeyboardDriverMac"
#    define GT_METHOD_NAME "keyPress_char"
bool GTKeyboardDriver::keyPress(char origKey, Qt::KeyboardModifiers modifiers) {
    // printf("GTKeyboardDriver::keyPress %c\n", origKey);
    // dumpState("before press");
    DRIVER_CHECK(origKey != 0, "key = 0");
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    char keyWithNoShift = toKeyWithNoShift(origKey);
    if (origKey != keyWithNoShift && modKeys.isEmpty()) {
        // printf("Adding SHIFT modifier\n");
        modKeys.append(Qt::Key_Shift);
    }
    foreach (Qt::Key mod, modKeys) {
        keyPressMac(GTKeyboardDriver::key[mod]);
        // dumpState("after modifier");
    }
    CGKeyCode keyCode = asciiToVirtual(keyWithNoShift);
    keyPressMac(keyCode);
    // dumpState("after press");
    return true;
}
#    undef GT_METHOD_NAME

#    define GT_METHOD_NAME "keyRelease_char"
bool GTKeyboardDriver::keyRelease(char origKey, Qt::KeyboardModifiers modifiers) {
    // printf("GTKeyboardDriver::key release %c\n", origKey);
    // dumpState("before release");
    DRIVER_CHECK(origKey != 0, "key = 0");
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    char keyWithNoShift = toKeyWithNoShift(origKey);
    if (origKey != keyWithNoShift && modKeys.isEmpty()) {
        // printf("Adding SHIFT modifier\n");
        modKeys.append(Qt::Key_Shift);
    }
    CGKeyCode keyCode = asciiToVirtual(keyWithNoShift);
    keyReleaseMac(keyCode);
    foreach (Qt::Key mod, modKeys) {
        keyReleaseMac(GTKeyboardDriver::key[mod]);
    }
    // dumpState("after release");
    return true;
}
#    undef GT_METHOD_NAME

bool GTKeyboardDriver::keyPress(Qt::Key qtKey, Qt::KeyboardModifiers modifiers) {
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    for (const Qt::Key& mod : qAsConst(modKeys)) {
        keyPressMac(GTKeyboardDriver::key[mod]);
    }
    keyPressMac(GTKeyboardDriver::key[qtKey]);
    return true;
}

bool GTKeyboardDriver::keyRelease(Qt::Key qtKey, Qt::KeyboardModifiers modifiers) {
    keyReleaseMac(GTKeyboardDriver::key[qtKey]);
    if (qtKey == Qt::Key_Delete || qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F12) {
        // For some reason MacOS does not release FN qtKey used for the internal ForwardDelete emulation (Fn + Delete).
        // Check GUITest_regression_scenarios_test_2971 as an example of Delete
        // or GUITest_regression_scenarios_test_3335 as an example of F2.
        auto state = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
        if (state & kCGEventFlagMaskSecondaryFn) {
            keyReleaseMac(kVK_Function);
        }
    }
    QList<Qt::Key> modKeys = modifiersToKeys(modifiers);
    for (const Qt::Key& mod : qAsConst(modKeys)) {
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

void GTKeyboardDriver::releasePressedKeys() {
    auto state = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
    if (state & kCGEventFlagMaskShift) {
        keyReleaseMac(kVK_Shift);
    }
    if (state & kCGEventFlagMaskControl) {
        keyReleaseMac(kVK_Control);
    }
    if (state & kCGEventFlagMaskAlternate) {
        keyReleaseMac(kVK_Option);
    }
    if (state & kCGEventFlagMaskCommand) {
        keyReleaseMac(kVK_Command);
    }
    if (state & kCGEventFlagMaskSecondaryFn) {
        keyReleaseMac(kVK_Function);
    }
    dumpState("releasePressedKeys");
}

#    undef GT_CLASS_NAME

}  // namespace HI

#endif
