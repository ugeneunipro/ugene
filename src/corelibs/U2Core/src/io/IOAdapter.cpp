/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "IOAdapter.h"

#include <U2Core/TextUtils.h>

namespace U2 {

const IOAdapterId BaseIOAdapters::LOCAL_FILE("local_file");
const IOAdapterId BaseIOAdapters::GZIPPED_LOCAL_FILE("local_file_gzip");
const IOAdapterId BaseIOAdapters::HTTP_FILE( "http_file" );
const IOAdapterId BaseIOAdapters::GZIPPED_HTTP_FILE( "http_file_gzip" );
const IOAdapterId BaseIOAdapters::VFS_FILE( "memory_buffer" );
const IOAdapterId BaseIOAdapters::STRING( "string" );
const IOAdapterId BaseIOAdapters::DATABASE_CONNECTION("database_connection");


qint64 IOAdapter::readUntil(char* buf, qint64 maxSize,
                            const QBitArray& readTerminators,
                            TerminatorHandling th,  bool* terminatorFound)
{
    const qint64 CHUNK = (qint64)1024;
    const char *chunk_start, *start = buf, *end = buf + maxSize;

    bool found = false;
    qint64 len;
    qint64 termsSkipped = 0;
    do {
        chunk_start = buf;
        len = readBlock(buf, qMin(CHUNK, (qint64)end - (qint64)buf));
        if (len == -1){
            //error
            return -1;
        }
        if (len < CHUNK) {
            // last chunk, no more data or buffer space
            end = buf + len;
        }
        const char* buf_end = buf + len;
        for(; buf < buf_end; buf++ ) {
            // loop exit invariant: buf is positioned after last accepted char
            if (readTerminators[(uchar)*buf]) {
                found = true;
                if (th == Term_Exclude) {
                    break;
                } else if( Term_Skip == th ) {
                    termsSkipped++;
                }
            } else if (found) {
                assert( Term_Include == th || Term_Skip == th );
                break;
            }
        }
    } while (!found && buf < end);

    if (found) {
        bool b = skip((qint64)buf - (qint64)chunk_start - len);
        assert(b);// Cannot put back unused data;
        Q_UNUSED(b);
    }

    if (terminatorFound != NULL) {
        *terminatorFound = found;
    }

    return (qint64)buf - (qint64)start - termsSkipped;
}

bool IOAdapter::isEof() {
    char ch;
    int ret = readBlock( &ch, 1 );
    if (ret == 1) {
        skip( -ret );
    }
    return 0 == ret;
}

qint64 IOAdapter::readLine( char* buff, qint64 maxSize, bool* terminatorFound /* = 0*/ ) {
    bool b = false;
    if (!terminatorFound) {
        terminatorFound = &b;
    }
    qint64 len = readUntil(buff, maxSize, TextUtils::LINE_BREAKS, Term_Exclude, terminatorFound);
    if (*terminatorFound) {
        char ch;
        bool b = getChar(&ch);
        assert(b);
        if (ch == '\r') {
            // may be Windows EOL \r\n
            b = getChar(&ch);
            if (b && ch != '\n') {
                skip(-1);
            }
        } else {
            assert(ch == '\n');
        }
    }
    return len;
}

void IOAdapter::cutByteOrderMarks(char* data, qint64& length) {
    QByteArray dataByteArray(data, length);
    qint64 newCached = TextUtils::cutByteOrderMarks(data, length);
    QByteArray newDatatByteArray(data);
    if (dataByteArray != newDatatByteArray) {
        length = newCached;
    }
}

}//namespace
