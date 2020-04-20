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

#include <cctype>
#include "GTKeyboardDriver.h"
#include <utils/GTThread.h>

namespace HI {

#define GT_CLASS_NAME "GTKeyboardDriver"

#define GT_METHOD_NAME "keyClick"

QList<Qt::Key> GTKeyboardDriver::modifiersToKeys(Qt::KeyboardModifiers mod){
    QList<Qt::Key> result;
    if(mod.testFlag(Qt::ShiftModifier)){result.append(Qt::Key_Shift);}
    if(mod.testFlag(Qt::AltModifier)){result.append(Qt::Key_Alt);}
    if(mod.testFlag(Qt::ControlModifier)){result.append(Qt::Key_Control);}
    if(mod.testFlag(Qt::MetaModifier)){result.append(Qt::Key_Meta);}
    return result;
}

bool GTKeyboardDriver::keyClick(char key, Qt::KeyboardModifiers modifiers)
{
    DRIVER_CHECK(key != 0, "key = 0");
    DRIVER_CHECK(keyPress(key, modifiers), "key could not be pressed");
    DRIVER_CHECK(keyRelease(key, modifiers), "key could not be released");
    return true;
}

bool GTKeyboardDriver::keyClick(Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    DRIVER_CHECK(key != 0, "key = 0");
    DRIVER_CHECK(keyPress(key, modifiers), "key could not be pressed");
    DRIVER_CHECK(keyRelease(key, modifiers), "key could not be released");
    return true;
}
#undef GT_METHOD_NAME

bool GTKeyboardDriver::keySequence(const QString &str, Qt::KeyboardModifiers modifiers)
{
    QList<Qt::Key> modifierKeys = modifiersToKeys(modifiers);
    foreach (Qt::Key mod, modifierKeys) {
        DRIVER_CHECK(keyPress(mod), "modifier could not be pressed");
    }

    foreach(QChar ch, str) {
        char asciiChar = ch.toLatin1();
        if(isalpha(asciiChar) && !islower(asciiChar)) {
            DRIVER_CHECK(keyClick( asciiChar, Qt::ShiftModifier), QString("%1 char could not be clicked with shift modifier").arg(asciiChar));
        } else {
            DRIVER_CHECK(keyClick( asciiChar), QString("%1 char could not be clicked").arg(asciiChar));
        }
        GTGlobals::sleep(10);
    }

    foreach (Qt::Key mod, modifierKeys) {
        DRIVER_CHECK(keyRelease(mod), "modifier could not be released");
    }
    GTThread::waitForMainThread();
    return true;
}

/******************************************************************************/
int GTKeyboardDriver::keys::operator [] (const Qt::Key &key) const
{
    return value(key);
}

GTKeyboardDriver::keys GTKeyboardDriver::key;

#undef GT_CLASS_NAME

} //namespace
