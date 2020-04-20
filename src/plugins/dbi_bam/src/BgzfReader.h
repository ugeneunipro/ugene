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

#ifndef _U2_BAM_BGZF_READER_H_
#define _U2_BAM_BGZF_READER_H_

#include <3rdparty/zlib/zlib.h>

#include <U2Core/IOAdapter.h>
#include "VirtualOffset.h"

namespace U2 {
namespace BAM {

class BgzfReader
{
public:
    BgzfReader(IOAdapter &ioAdapter);
    ~BgzfReader();

    qint64 read(char *buff, qint64 maxSize);
    qint64 skip(qint64 size);

    bool isEof()const;

    VirtualOffset getOffset()const;
    void seek(VirtualOffset offset);
private:
    void nextBlock();

    static const int BUFFER_SIZE = 16384;
    IOAdapter &ioAdapter;
    z_stream stream;
    char buffer[BUFFER_SIZE];
    quint64 headerOffset;
    bool endOfFile;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_BGZF_READER_H_
