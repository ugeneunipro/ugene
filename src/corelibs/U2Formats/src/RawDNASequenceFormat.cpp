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

#include "RawDNASequenceFormat.h"

#include <QBuffer>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "PlainTextFormat.h"

namespace U2 {

RawDNASequenceFormat::RawDNASequenceFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::RAW_DNA_SEQUENCE, DocumentFormatFlags_W1, {"seq", "txt"}) {
    formatName = tr("Raw sequence");
    supportedObjectTypes += GObjectTypes::SEQUENCE;
    formatDescription = tr("Raw sequence file - a whole content of the file is treated either as a single/multiple nucleotide or peptide sequence(s). UGENE will remove all non-alphabetic chars from the result sequence. By default the characters in the file are considered a single sequence.");
}

static void finishSequenceImport(QList<GObject*>& objects, const QString& url, U2OpStatus& os, const U2DbiRef& dbiRef, const QVariantMap& fs, TmpDbiObjects& dbiObjects, U2SequenceImporter& seqImporter) {
    U2Sequence u2seq = seqImporter.finalizeSequence(os);
    dbiObjects.objects << u2seq.id;
    CHECK_OP(os, );

    GObjectReference sequenceRef(url, u2seq.visualName, GObjectTypes::SEQUENCE, U2EntityRef(dbiRef, u2seq.id));
    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, nullptr, fs);

    objects << new U2SequenceObject(u2seq.visualName, U2EntityRef(dbiRef, u2seq.id));
}

static void load(IOAdapterReader& reader, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );

    U2SequenceImporter seqImporter(hints, true);
    QString folder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    QBitArray ALPHAS = TextUtils::ALPHA_NUMS;
    ALPHAS['-'] = true;
    ALPHAS['*'] = true;

    QString sequenceFileName = reader.getURL().baseFileName();
    TmpDbiObjects dbiObjects(dbiRef, os);
    bool isAppendToCurrentObject = false;
    int sequenceCounter = 0;
    bool isSplit = hints.value((DocumentReadingMode_SequenceAsSeparateHint), false).toBool();

    while (!reader.atEnd()) {
        QByteArray unfilteredSequenceLine = reader.readLine(os, -1).toLatin1();
        bool isLastLine = reader.atEnd();
        CHECK_OP(os, );
        CHECK_CONTINUE(!unfilteredSequenceLine.isEmpty());

        // Filter all non-alpha characters.
        QByteArray sequenceLine;
        sequenceLine.reserve(unfilteredSequenceLine.size());
        for (int i = 0; i < unfilteredSequenceLine.length(); i++) {
            char c = unfilteredSequenceLine[i];
            if (ALPHAS[(uchar)c]) {
                sequenceLine.append(c);
            }
        }
        if (sequenceLine.size() > 0 && isAppendToCurrentObject == false) {
            QString sequenceObjectName = sequenceCounter == 0 ? sequenceFileName : sequenceFileName + QString("_%1").arg(sequenceCounter);
            isAppendToCurrentObject = true;
            seqImporter.startSequence(os, dbiRef, folder, sequenceObjectName, false);
            CHECK_OP(os, );
        }
        if (isAppendToCurrentObject) {
            seqImporter.addBlock(sequenceLine.data(), sequenceLine.size(), os);
            CHECK_OP(os, );
        }
        if (sequenceLine.size() > 0 && isAppendToCurrentObject && isSplit && !isLastLine) {
            finishSequenceImport(objects, sequenceFileName, os, dbiRef, hints, dbiObjects, seqImporter);
            CHECK_OP(os, );
            sequenceCounter++;
            isAppendToCurrentObject = false;
        }
        os.setProgress(reader.getProgress());
    }

    if (sequenceCounter == 0) {
        CHECK_EXT(isAppendToCurrentObject, os.setError(RawDNASequenceFormat::tr("Sequence is empty")), );
    }
    if (isAppendToCurrentObject) {
        finishSequenceImport(objects, sequenceFileName, os, dbiRef, hints, dbiObjects, seqImporter);
    }
}

Document* RawDNASequenceFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    QList<GObject*> objects;
    load(reader, dbiRef, objects, hints, os);
    CHECK_OP(os, nullptr);
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints);
}

FormatCheckResult RawDNASequenceFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    if (QRegExp("[a-zA-Z\r\n\\*-]*").exactMatch(dataPrefix)) {
        return FormatDetection_VeryHighSimilarity;
    }
    // returning 'very low chance' here just because it's impossible to have 100% detection for this format.
    return FormatDetection_VeryLowSimilarity;
}

void RawDNASequenceFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    QList<GObject*> objects = document->findGObjectByType(GObjectTypes::SEQUENCE);
    CHECK(objects.size() == 1, );
    auto* sequenceObject = qobject_cast<U2SequenceObject*>(objects.first());
    SAFE_POINT(sequenceObject != nullptr, L10N::nullPointerError("Sequence object"), );
    QByteArray seqData = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    writer.write(os, QString::fromLatin1(seqData));  // Note: we limit DNA sequence alphabet to Latin1.
}

void RawDNASequenceFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::SEQUENCE), "Raw sequence entry storing: no sequences", );
    const QList<GObject*>& sequenceObjectList = objectsMap[GObjectTypes::SEQUENCE];
    SAFE_POINT(sequenceObjectList.size() == 1, "Raw sequence entry storing: sequence objects count error", );

    auto sequenceObject = dynamic_cast<U2SequenceObject*>(sequenceObjectList.first());
    SAFE_POINT(sequenceObject != nullptr, "Raw sequence entry storing: NULL sequence object", );

    QByteArray seqData = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    writer.write(os, QString::fromLatin1(seqData));  // Note: we limit DNA sequence alphabet to Latin1.
    CHECK_OP(os, );
    writer.write(os, "\n");
}

bool RawDNASequenceFormat::isStreamingSupport() {
    return true;
}

}  // namespace U2
