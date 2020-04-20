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

#ifndef _U2_OUTPUTSTREAM_H_
#define _U2_OUTPUTSTREAM_H_

#include <U2Core/U2OpStatus.h>

namespace U2 {

/**
 * This abstract class is the superclass of all classes representing an output stream of bytes.
 */
class U2CORE_EXPORT OutputStream {
public:
    OutputStream() {}
    virtual ~OutputStream() {}

    /**
     * Closes this output stream and releases any system resources associated with this stream.
     */
    virtual void close() = 0;
    /**
     * Writes @length bytes from the specified byte array starting to this output stream.
     */
    virtual void write(const char *buffer, int length, U2OpStatus &os) = 0;
};

} // U2

#endif // _U2_OUTPUTSTREAM_H_
