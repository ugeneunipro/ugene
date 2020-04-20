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

#ifndef _U2_SARRAY_INDEX_SERIALIZER_H_
#define _U2_SARRAY_INDEX_SERIALIZER_H_

#include "SArrayIndex.h"
#include <U2Core/global.h>
#include <QFile>

namespace U2 {

class U2ALGORITHM_EXPORT SArrayIndexSerializer {

public:
    static void serialize(const SArrayIndex *index, const QString &indexFileName, const QString &refFileName);
    static void deserialize(SArrayIndex *index, const QString &indexFileName, TaskStateInfo& ti);

private:
    static const QString SARRAY_HEADER;
    static const QString SARRAY_PARAMETERS;
    static const int BUFF_SIZE = 1048576;
    static void writeArray(QFile &file, char *buff, quint32 *array, int len);
    static void readArray(QFile &file, char *buff, int *len, int *pos, int *bytes,
    int *lineIdx, quint32 *array, int arrLen, TaskStateInfo& ti);
};

} //namespace

#endif
