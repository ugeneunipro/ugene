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

#ifndef _U2_INPUTSTREAM_H_
#define _U2_INPUTSTREAM_H_

#include <U2Core/U2OpStatus.h>

namespace U2 {

/**
 * This abstract class is the superclass of all classes representing an input stream of bytes.
 */
class U2CORE_EXPORT InputStream {
public:
    InputStream() {}
    virtual ~InputStream() {}

    /**
     * Returns the number of bytes that can be read from this input stream.
     */
    virtual qint64 available() = 0;
    /**
     * Closes this input stream and releases any system resources associated with the stream.
     */
    virtual void close() = 0;
    /**
     * Reads up to @length bytes of data from the input stream into an array of bytes.
     * Returns the total number of bytes read into the buffer,
     * or -1 if there is no more data because the end of the stream has been reached.
     */
    virtual int read(char *buffer, int length, U2OpStatus &os) = 0;
    /**
     * Skips over and discards n bytes of data from this input stream.
     * Returns the actual number of bytes skipped.
     */
    virtual qint64 skip(qint64 n, U2OpStatus &os) = 0;
};

} // U2

#endif // _U2_INPUTSTREAM_H_
