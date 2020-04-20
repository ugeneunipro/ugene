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

#include <3rdparty/zlib/zlib.h>

#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Task.h>
#include <U2Core/Version.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiRegistry.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/DocumentFormatUtils.h>

#include "Reader.h"
#include "BAMDbiPlugin.h"
#include "Exception.h"
#include "BAMFormat.h"


namespace U2 {

BAMFormat::BAMFormat()
: DbiDocumentFormat(
    BAM_DBI_ID,
    BaseDocumentFormats::BAM,
    tr("BAM"),
    QStringList("bam"),
    DocumentFormatFlags(DocumentFormatFlag_NoPack) | DocumentFormatFlag_NoFullMemoryLoad
    | DocumentFormatFlag_Hidden | DocumentFormatFlag_SupportWriting | DocumentFormatFlag_CannotBeCompressed)
{
    // DbiDocumentFormat adds a set of object types that are supported by DBI but have no relation to BAM.
    // Reset these formats and add BAM related object types only.
    supportedObjectTypes.clear();
    supportedObjectTypes += GObjectTypes::ASSEMBLY;
}

void BAMFormat::storeDocument(Document *d, IOAdapter *io, U2OpStatus &os) {
    CHECK_EXT(d != NULL, os.setError(L10N::badArgument("doc")), );
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), );

    QList<GObject*> als = d->findGObjectByType(GObjectTypes::ASSEMBLY);
    GUrl url = io->getURL();
    io->close();

    BAMUtils::writeObjects(
        d->findGObjectByType(GObjectTypes::ASSEMBLY),
        url,
        getFormatId(),
        os);
}

namespace BAM {

BAMFormatUtils::BAMFormatUtils(QObject *parent)
: QObject(parent),
  fileExtensions(QStringList("bam"))
{
}

FormatCheckResult BAMFormatUtils::checkRawData(const QByteArray &rawData, const GUrl & /*url*/) const {
    z_stream_s stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in = (Bytef *)rawData.constData();
    stream.avail_in = rawData.size();
    QByteArray magic(4, '\0');
    stream.next_out = (Bytef *)magic.data();
    stream.avail_out = magic.size();
    FormatDetectionScore result = FormatDetection_NotMatched;
    if(Z_OK == inflateInit2(&stream, 16 + 15)) {
        if(Z_OK == inflate(&stream, Z_SYNC_FLUSH)) {
            if(0 == stream.avail_out) {
                if("BAM\001" == magic) {
                    result = FormatDetection_Matched;
                }
            }
        }
        inflateEnd(&stream);
    }
    return result;
}


} // namespace BAM
} // namespace U2
