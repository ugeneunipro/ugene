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

#ifndef _HI_GUI_GTKEYBOARDDRIVER_H_
#define _HI_GUI_GTKEYBOARDDRIVER_H_

#include <QMap>
#include "GTGlobals.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define ADD_KEY(name, code) insert(name, code)

namespace HI {
/*!
 * \brief The base class for keyboard's actions imitation
 *
 * Example:
 * \code {.cpp}
 * GTKeyboardDriver::keyClick( 'A'); // print 'a'
 * GTKeyboardDriver::keyClick( 'a'); // print 'a'
 *
 * GTKeyboardDriver::keyClick( 'a', GTKeyboardDriver::key[Qt::Key_Shift]); // print 'A'
 * GTKeyboardDriver::keyClick( 'a', GTKeyboardDriver::key[Qt::Key_Shift]); // print 'A'
 * //case in ["..."] does not matter
 *
 * GTKeyboardDriver::keySequence("ThIs Is a TeSt StRiNg"); // print "ThIs Is a TeSt StRiNg"
 * //i.e. case sensitive
 * \endcode
 */
class HI_EXPORT GTKeyboardDriver {
public:
    //
    // fails if key == 0
    // Linux: fails if there is an opening X display error

    static bool keyClick(char key, Qt::KeyboardModifiers = Qt::NoModifier);
    static bool keyClick(Qt::Key, Qt::KeyboardModifiers = Qt::NoModifier);
    static bool keySequence(const QString &str, Qt::KeyboardModifiers = Qt::NoModifier);

    static bool keyPress(char key, Qt::KeyboardModifiers = Qt::NoModifier);
    static bool keyRelease(char key, Qt::KeyboardModifiers = Qt::NoModifier);
    static bool keyPress(Qt::Key, Qt::KeyboardModifiers = Qt::NoModifier);
    static bool keyRelease(Qt::Key, Qt::KeyboardModifiers = Qt::NoModifier);

    class HI_EXPORT keys : private QMap<Qt::Key, int> {
    public:
        keys();
        int operator [] (const Qt::Key &key) const;
    };

    static keys key;

private:
    static QList<Qt::Key> modifiersToKeys(Qt::KeyboardModifiers m);

};

} //namespace

#endif
