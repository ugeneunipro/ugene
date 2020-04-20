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

#ifndef _U2_QUALIFIER_H_
#define _U2_QUALIFIER_H_

#include <U2Core/global.h>

namespace U2 {

/**
 * Annotation qualifier - a textual property/remark for annotation
 */
class U2CORE_EXPORT U2Qualifier {
public:
    /** Name of qualifier */
    QString name;

    /** Value of qualifier */
    QString value;


    /** Constructs new empty (and invalid) qualifier */
    U2Qualifier();

    /** Constructs new qualifier instance with name and value set */
    U2Qualifier(const QString &name, const QString &value);

    /** U2Qualifier is valid if  its name is not empty */
    bool isValid() const;

    /** Any two qualifiers are equal if their names & values are equal */
    bool operator==(const U2Qualifier &q) const;

    /** Any two qualifiers are not equal if either their names or values are  not equal */
    bool operator!=(const U2Qualifier &q) const;

    static bool isValidQualifierName(const QString &name);
    static bool isValidQualifierValue(const QString &value);
};

} // namespace

Q_DECLARE_METATYPE(U2::U2Qualifier)

#endif
