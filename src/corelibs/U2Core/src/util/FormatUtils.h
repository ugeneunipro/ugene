/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_FORMAT_UTILS_H_
#define _U2_FORMAT_UTILS_H_

#include <U2Core/global.h>

namespace U2 {

class DocumentFormatConstraints;

class U2CORE_EXPORT FormatUtils {
public:
    static QString splitThousands(qint64 num);

    static QString formatNumber(qint64 num);
    static QString formatNumberWithSeparators(int num);
    static QString insertSeparators(int num);

    // QDate::getShortMonthName() returns date in local translation
    // This is English version of this function, required by EMBL and Genbank
    // for correct date formatting
    static QString getShortMonthName(int num);
};

}  // namespace U2

#endif
