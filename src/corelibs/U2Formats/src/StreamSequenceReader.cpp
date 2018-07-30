/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Timer.h>

#include "StreamSequenceReader.h"

namespace U2 {

DNASequence* StreamSequenceReader::getNextSequenceObject() {
    if (hasNext()) {
        DNASequence* result = currentSeq.data();
        lookupPerformed = false;
        return result;
    }
    return NULL;
}

StreamSequenceReader::StreamSequenceReader()
: currentReaderIndex(-1), currentSeq(NULL), errorOccured(false), lookupPerformed(false)
{

}

bool StreamSequenceReader::hasNext() {
    if (readers.isEmpty()) {
        return false;
    }

    if (!lookupPerformed) {
        if (currentReaderIndex < 0 || currentReaderIndex >= readers.count()) {
            return false;
        }

        while (currentReaderIndex < readers.count()) {
            ReaderContext ctx = readers.at(currentReaderIndex);
            DNASequence *newSeq = ctx.format->loadSequence(ctx.io, taskInfo);
            currentSeq.reset(newSeq);
            if (NULL == newSeq) {
                ++currentReaderIndex;
            } else {
                lookupPerformed = true;
                break;
            }
        }

    }

    if (currentSeq.isNull()) {
        return false;
    }

    return true;
}

bool StreamSequenceReader::init(const QStringList &urls) {
    QList<GUrl> gUrls;
    foreach (const QString &url, urls) {
        gUrls << url;
    }
    return init(gUrls);
}

bool StreamSequenceReader::init( const QList<GUrl>& urls ) {
    foreach (const GUrl& url, urls) {
        QList<FormatDetectionResult> detectedFormats = DocumentUtils::detectFormat(url);
        if (detectedFormats.isEmpty()) {
            taskInfo.setError(tr("File %1 unsupported format.").arg(url.getURLString()));
            break;
        }
        ReaderContext ctx;
        ctx.format = detectedFormats.first().format;
        if ( ctx.format->getFlags().testFlag(DocumentFormatFlag_SupportStreaming) == false  ) {
            break;
        }
        IOAdapterFactory* factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
        IOAdapter* io = factory->createIOAdapter();
        if (!io->open(url, IOAdapterMode_Read)) {
            break;
        }
        ctx.io = io;
        readers.append(ctx);
    }

    if (readers.isEmpty()) {
        taskInfo.setError(tr("Unsupported file format or short reads list is empty"));
        return false;
    } else {
        currentReaderIndex = 0;
        return true;
    }


}

const IOAdapter *StreamSequenceReader::getIO() const
{
    if (currentReaderIndex < readers.count()) {
        ReaderContext ctx = readers.at(currentReaderIndex);
        return ctx.io;
    }
    return NULL;
}

DocumentFormat *StreamSequenceReader::getFormat() const
{
    if (currentReaderIndex < readers.count()) {
        ReaderContext ctx = readers.at(currentReaderIndex);
        return ctx.format;
    }
    return NULL;
}

QString StreamSequenceReader::getErrorMessage() {
    return taskInfo.getError();
}

int StreamSequenceReader::getProgress() {
    if (readers.count() == 0) {
        return 0;
    }

    float factor = 1/readers.count();
    int progress = 0;
    for (int i = 0; i < readers.count(); ++i) {
        progress += (int)(factor * readers[i].io->getProgress());
    }

    return progress;
}

StreamSequenceReader::~StreamSequenceReader() {
    for(int i =0; i < readers.size(); ++i) {
        delete readers[i].io;
        readers[i].io = NULL;
    }
}

} //namespace
