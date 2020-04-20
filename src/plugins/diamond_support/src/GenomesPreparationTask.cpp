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

#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/StreamSequenceReader.h>
#include <U2Formats/StreamSequenceWriter.h>

#include "GenomesPreparationTask.h"

namespace U2 {

GenomesPreparationTask::GenomesPreparationTask(const QStringList &_genomesUrls, const QString &_preparedGenomesFileUrl)
    : Task(tr("Genomes preparation"), TaskFlag_None),
      genomesUrls(_genomesUrls),
      preparedGenomesFileUrl(_preparedGenomesFileUrl)
{
    CHECK_EXT(genomesUrls.count() > 0, setError(tr("Genomes URLs are not set")), );
    CHECK_EXT(!preparedGenomesFileUrl.isEmpty(), setError(tr("File URL to write prepared genomes is empty")), );
}

const QString &GenomesPreparationTask::getPreparedGenomesFileUrl() const {
    return preparedGenomesFileUrl;
}

void GenomesPreparationTask::run() {
    if (1 == genomesUrls.count()) {
        preparedGenomesFileUrl = genomesUrls.first();
        return;
    }

    StreamSequenceReader reader;
    const bool readerInited = reader.init(genomesUrls);
    CHECK_EXT(readerInited, setError(reader.getErrorMessage()), );

    StreamGzippedShortReadWriter writer;
    const bool writerInited = writer.init(preparedGenomesFileUrl);
    CHECK_EXT(writerInited, setError(L10N::errorOpeningFileWrite(preparedGenomesFileUrl)), );

    while (reader.hasNext()) {
        CHECK_OP(stateInfo, );
        DNASequence *sequence(reader.getNextSequenceObject());
        CHECK_EXT(NULL != sequence, setError(reader.getErrorMessage()), );
        const bool written = writer.writeNextSequence(*sequence);
        CHECK_EXT(written, setError(L10N::errorWritingFile(preparedGenomesFileUrl)), );
    }
}

}   // namespace U2
