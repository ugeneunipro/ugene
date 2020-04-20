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

#ifndef _U2_ASSEMBLY_UTILS_H_
#define _U2_ASSEMBLY_UTILS_H_

#include <U2Core/U2Assembly.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

/**
    Predefined integer attribute for coverage
    First 4 bytes - lower coverage bound
    Last 4 bytes - upper coverage bound
*/
#define U2_ASSEMBLY_COVERAGE_ATTRIBUTE_LOWER "assembly-coverage-1m"


/**
    U2Assembly and related structures utility functions
*/
class U2CORE_EXPORT U2AssemblyUtils : public QObject {
    Q_OBJECT
private:
    U2AssemblyUtils(){}
public:

    /**
        Returns CIGAR operation for the given char, case insensitive
        If failed, error text is stored in 'err'
    */
    static U2CigarOp char2Cigar(char c, QString& err);

    /**
        Returns char representation of the given CIGAR op
    */
    static char cigar2Char(U2CigarOp op);

    /**
        Parses CIGAR string (SAM/BAM format specs)
        Returns the result as a set of U2CigarTokens.
        If parsing is failed, the fail reason is stored in 'err' parameter
    */
    static QList<U2CigarToken> parseCigar(const QByteArray& cigarString, QString& err);

    /**
        Returns string representation of the CIGAR
    */
    static QByteArray cigar2String(const QList<U2CigarToken>& cigar);


    /**
        Returns extra read length produced by CIGAR. Can be negative.
    */
    static qint64 getCigarExtraLength(const QList<U2CigarToken>& cigar);

    /** Returns read effective length: sequence length + CIGAR */
    static qint64 getEffectiveReadLength(const U2AssemblyRead& read);

    /**
        Serializes Coverage Statistics to byte array for storing in dbi attribute
    */
    static QByteArray serializeCoverageStat(const U2AssemblyCoverageStat& coverageStat);

    /**
        Deserializes Coverage Statistics that was serialized with serializeCoverageStat function
    */
    static void deserializeCoverageStat(QByteArray data, U2AssemblyCoverageStat& res, U2OpStatus &os);

    /**
        Size of array of cached coverage
    */
    static const int MAX_COVERAGE_VECTOR_SIZE;
};

} //namespace


#endif
