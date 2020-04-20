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

#ifndef _U2_BAM_BAI_WRITER_H_
#define _U2_BAM_BAI_WRITER_H_

#include <U2Core/IOAdapter.h>
#include "Index.h"

namespace U2 {
namespace BAM {

class BaiWriter
{
public:
    BaiWriter(IOAdapter &ioAdapter);
    void writeIndex(const Index &index);
private:
    void writeBytes(const char *buff, qint64 size);
    void writeBytes(const QByteArray &buffer);
    void writeUint64(quint64 value);
    void writeInt32(qint32 value);
    void writeUint32(quint32 value);

    IOAdapter &ioAdapter;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_BAI_WRITER_H_
