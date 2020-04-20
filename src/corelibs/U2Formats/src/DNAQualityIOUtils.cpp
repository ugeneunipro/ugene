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

#include "DNAQualityIOUtils.h"

#include <U2Core/IOAdapter.h>
#include <U2Core/AppContext.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>
#include <U2Core/IOAdapterUtils.h>

#include <time.h>

namespace U2  {

void DNAQualityIOUtils::writeDNAQuality(const U2SequenceObject* seqObj, const QString& dstFilePath,
                                        bool appendData, bool decode, U2OpStatus& stateInfo) {

    const DNAQuality& seqQuality = seqObj->getQuality();
    const QString& seqName = seqObj->getSequenceName();
    writeDNAQuality(seqName, seqQuality,dstFilePath, appendData, decode, stateInfo );
}

static QByteArray getDecodedQuality(const DNAQuality& quality) {
    QByteArray res;
    for (int i = 0, sz = quality.qualCodes.size(); i < sz; ++i) {
        QByteArray buf;
        buf.setNum( quality.getValue(i) );
        res.append(buf);
        res.append(" ");
    }
    return res;
}

void DNAQualityIOUtils::writeDNAQuality( const QString& seqName, const DNAQuality& seqQuality,
                                        const QString& dstFilePath, bool appendData, bool decode,
                                        U2OpStatus& stateInfo )
{
    if (seqQuality.isEmpty()) {
        stateInfo.setError("Quality score is not set!");
        return;
    }

    QScopedPointer<IOAdapter> ioAdapter;

    IOAdapterId ioAdapterId = IOAdapterUtils::url2io(dstFilePath);
    IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioAdapterId);
    CHECK_EXT (ioAdapterFactory != NULL, stateInfo.setError(tr("No IO adapter found for URL: %1").arg(dstFilePath)), );
    ioAdapter.reset(ioAdapterFactory->createIOAdapter());

    if (!ioAdapter->open(dstFilePath, appendData ? IOAdapterMode_Append : IOAdapterMode_Write)) {
        stateInfo.setError(L10N::errorOpeningFileWrite(dstFilePath));
        return;
    }

    QByteArray data;
    data.append(">");
    data.append(seqName.toLatin1());
    data.append("\n");
    data.append( decode ? getDecodedQuality(seqQuality) : seqQuality.qualCodes );
    data.append("\n");

    if (0 == ioAdapter->writeBlock(data)) {
        stateInfo.setError(L10N::errorWritingFile(dstFilePath));
    }

    ioAdapter->close();
}

} // namespace

