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

#ifndef _U2_SAM_READER_H_
#define _U2_SAM_READER_H_

#include "Reader.h"
#include "Header.h"
#include "Alignment.h"

namespace U2 {
namespace BAM {

class SamReader : public Reader {
public:
    SamReader(IOAdapter &ioAdapter);
    const Header &getHeader()const;
    Alignment readAlignment(bool &eof);
    bool isEof()const;
private:
    QByteArray readString(bool &eof);
    void readHeader();
    Alignment parseAlignmentString(QByteArray line);

    static const int LOCAL_READ_BUFFER_SIZE;
    QByteArray readBuffer;
friend class AlignmentParser;
};

} // namespace BAM
} // namespace U2

#endif // _U2_SAM_READER_H_
