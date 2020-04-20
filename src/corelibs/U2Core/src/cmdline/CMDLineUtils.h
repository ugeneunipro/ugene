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

#ifndef _U2_CMDLINE_UTILS_H_
#define _U2_CMDLINE_UTILS_H_

#include <QString>
#include <QList>

#include <U2Core/CMDLineRegistry.h>

namespace U2 {

class U2CORE_EXPORT CMDLineRegistryUtils {
public:
    // -1 if not found
    static int getParameterIndex( const QString & paramName, int startWith = 0 );
    // in case: '-t a b c' return {a, b, c}
    static QStringList getParameterValues( const QString & paramName, int startWith = 0 );
    // in case '-t "a b" c' return {a, b, c} not {"a b", c}
    static QStringList getParameterValuesByWords( const QString & paramName, int startWith = 0 );
    // return list of all values that doesn't have keys ("", paramValue)
    // search starts at startWithIdx, stops at first (paramKey, paramValue) pair
    // by default, search starts at 1 because at params[0] is usually ("", programName) pair
    static QStringList getPureValues( int startWithIdx = 1 );
    // Returns the path to the UGENE command line executable
    static QString getCmdlineUgenePath();

private:
    static void setCMDLineParams( QList<StrStrPair> & to );

}; // CMDLineRegistryUtils

class U2CORE_EXPORT CMDLineUtils {
public:
    static void init();

}; // CMDLineUtils

} // U2

#endif // _U2_CMDLINE_UTILS_H_
