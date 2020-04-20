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

#ifndef _U2_DNA_QUALITY_H_
#define _U2_DNA_QUALITY_H_

#include <U2Core/global.h>

#include <QByteArray>
#include <QStringList>

namespace U2 {

enum DNAQualityType {
    DNAQualityType_Sanger,
    DNAQualityType_Illumina,
    DnaQualityType_Solexa
};

typedef QString DNAQualityFormat;

// diagnose
//   SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS.....................................................
//   ..........................XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX......................
//   ...............................IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII......................
//   .................................JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ......................
//   LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL....................................................
//   !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//   |                         |    |        |                              |                     |
//  33                        59   64       73                            104                   126 <- maxValue is value from here
// S 0........................26...31.......40
// X                          -5....0........9.............................40
// I                                0........9.............................40
// J                                   3.....9.............................40
// L 0.2......................26...31........41
//
//  S - Sanger        Phred+33,  raw reads typically (0, 40)
//  X - Solexa        Solexa+64, raw reads typically (-5, 40)
//  I - Illumina 1.3+ Phred+64,  raw reads typically (0, 40)
//  J - Illumina 1.5+ Phred+64,  raw reads typically (3, 40) with 0=unused, 1=unused, 2=Read Segment Quality Control Indicator (bold)
//  L - Illumina 1.8+ Phred+33,  raw reads typically (0, 41)

class U2CORE_EXPORT DNAQuality {
public:
    DNAQuality() : type (DNAQualityType_Sanger) {}
    DNAQuality(const QByteArray &qualScore);
    DNAQuality(const QByteArray &qualScore, DNAQualityType type);

    bool isEmpty() const { return qualCodes.isEmpty(); }
    int getValue(int pos) const;
    static char encode(int val, DNAQualityType type);

    static QString getDNAQualityNameByType(DNAQualityType t);
    static DNAQualityType getDNAQualityTypeByName(const QString& name);
    static QStringList getDNAQualityTypeNames();
    static DNAQualityType detectTypeByCodes(const QByteArray &qualCodes);
    static DNAQualityType detectTypeByMinMaxQualityValues(int minQualityValue, int maxQualityValue);

    qint64 memoryHint() const;

    QByteArray      qualCodes;
    void setQualCodes(const QByteArray& qualCodes);

    DNAQualityType  type;

    static const DNAQualityFormat QUAL_FORMAT;
    static const DNAQualityFormat ENCODED;
    static const int MAX_PHRED33_VALUE;
    static const int MIN_PHRED64_VALUE;
};

}//namespace


#endif
