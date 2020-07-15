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

#ifndef _U2_IOADAPTER_UTILS_H_
#define _U2_IOADAPTER_UTILS_H_

#include <U2Core/IOAdapter.h>
#include <U2Core/global.h>

namespace U2 {

class U2OpStatus;

class U2CORE_EXPORT IOAdapterUtils {
public:
    static IOAdapterId url2io(const GUrl &url);

    static QByteArray readFileHeader(const GUrl &url, int size = 65536);

    // io - opened IO adapter. before and after the call pos in file the same
    static QByteArray readFileHeader(IOAdapter *io, int size = 65536);

    static IOAdapter *open(const GUrl &url, U2OpStatus &os, IOAdapterMode mode = IOAdapterMode_Read, IOAdapterFactory *iof = NULL);

    static IOAdapterFactory *get(const IOAdapterId &);

    /** Reads whole file into string. Returns null string in case if error. */
    static QString readTextFile(const QString &path, const char *codecName = "UTF-8");

    /** Writes (overwrites) a string to file. Returns false of failed. */
    static bool writeTextFile(const QString &path, const QString &content, const char *codecName = "UTF-8");
};

}    // namespace U2

#endif
