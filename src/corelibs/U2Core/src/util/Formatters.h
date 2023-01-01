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

#ifndef _U2_FORMATTERS_H_
#define _U2_FORMATTERS_H_

#include <U2Core/global.h>

namespace U2 {

/** A class used to format any given string according to the formatters domain rules. */
template<typename ValueType>
class U2Formatter {
public:
    virtual ~U2Formatter() = default;

    /** Returns formatted text for the given value. */
    virtual QString format(const ValueType& value) const = 0;
};

typedef U2Formatter<QString> StringFormatter;

/** Interprets property name as document format id and returns the related document format name. */
class U2CORE_EXPORT DocumentNameByIdFormatter : public StringFormatter {
public:
    QString format(const QString& documentFormatId) const override;
};

}  // namespace U2

#endif  // _U2_FORMATTERS_H_
