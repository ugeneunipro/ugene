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

#ifndef _U2_DNA_QUALITY_IO_UTILS_H_
#define _U2_DNA_QUALITY_IO_UTILS_H_

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class U2FORMATS_EXPORT DNAQualityIOUtils : public QObject {
    Q_OBJECT
public:

    static void writeDNAQuality(const U2SequenceObject* seqObj, const QString& fileName,
        bool appendData, bool decode, U2OpStatus& op);

    static void writeDNAQuality(const QString& seqName, const DNAQuality& quality,
        const QString& filePath, bool appendData, bool decode, U2OpStatus& op);

};


} //namespace


#endif // _U2_DNA_QUALITY_IO_UTILS_H_

