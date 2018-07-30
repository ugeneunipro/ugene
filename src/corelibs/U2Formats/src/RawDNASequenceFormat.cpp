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

#include <QBuffer>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "PlainTextFormat.h"
#include "RawDNASequenceFormat.h"

namespace U2 {

/* TRANSLATOR U2::RawDNASequenceFormat */
/* TRANSLATOR U2::IOAdapter */

RawDNASequenceFormat::RawDNASequenceFormat(QObject* p) : TextDocumentFormat(p, DocumentFormatFlags_W1) {
    formatName = tr("Raw sequence");
    fileExtensions << "seq" << "txt";
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    formatDescription = tr("Raw sequence file - a whole content of the file is treated either as a single/multiple nucleotide or peptide sequence(s). UGENE will remove all non-alphabetic chars from the result sequence. By default the characters in the file are considered a single sequence.");
}

void finishSequence(QList<GObject*>& objects, IOAdapter* io, U2OpStatus& os, const U2DbiRef& dbiRef, const QVariantMap& fs, TmpDbiObjects& dbiObjects, U2SequenceImporter& seqImporter){
    U2Sequence u2seq = seqImporter.finalizeSequence(os);
    dbiObjects.objects << u2seq.id;
    CHECK_OP(os, );

    GObjectReference sequenceRef(io->getURL().getURLString(), u2seq.visualName, GObjectTypes::SEQUENCE, U2EntityRef(dbiRef, u2seq.id));
    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, NULL, fs);

    objects << new U2SequenceObject(u2seq.visualName,U2EntityRef(dbiRef, u2seq.id));
}

static void load(IOAdapter* io, const U2DbiRef& dbiRef,  QList<GObject*>& objects, const QVariantMap& fs, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );
    Q_UNUSED(opBlock);

    U2SequenceImporter seqImporter(fs, true);
    const QString folder = fs.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    QByteArray readBuffer(DocumentFormat::READ_BUFF_SIZE, '\0');
    char* buff  = readBuffer.data();

    QBitArray ALPHAS = TextUtils::ALPHA_NUMS;
    ALPHAS['-'] = true;
    ALPHAS['*'] = true;

    QByteArray seq;
    QString seqName(io->getURL().baseFileName());
    //reading sequence
    QBuffer writer(&seq);
    writer.open(QIODevice::WriteOnly);
    TmpDbiObjects dbiObjects(dbiRef, os);
    bool ok = true;
    bool isStarted = false;
    int sequenceCounter = 0;
    bool terminatorFound = false;
    bool isSplit = fs.value((DocumentReadingMode_SequenceAsSeparateHint), false).toBool();


    while (ok && !io->isEof()) {
        int len = io->readLine(buff, DocumentFormat::READ_BUFF_SIZE, &terminatorFound);
        if (len <= 0){
            continue;
        }

        seq.clear();
        bool isSeek = writer.seek(0);
                assert(isSeek); Q_UNUSED(isSeek);
        if (os.isCoR()) {
            break;
        }

        for (int i=0; i<len && ok; i++) {
            char c = buff[i];
            if (ALPHAS[(uchar)c]) {
                ok = writer.putChar(c);
            }
        }
        if(seq.size()>0 && isStarted == false ){
            QString name = sequenceCounter == 0 ? seqName : seqName + QString("_%1").arg(sequenceCounter);
            isStarted = true;
            seqImporter.startSequence(os, dbiRef, folder, name, false);
        }
        if(isStarted){
            seqImporter.addBlock(seq.data(),seq.size(),os);
        }
        if (seq.size()>0 && isStarted && terminatorFound && isSplit){
            finishSequence(objects, io, os, dbiRef, fs, dbiObjects, seqImporter);
            sequenceCounter++;
            isStarted = false;
        }
        if (os.isCoR()) {
            break;
        }
        os.setProgress(io->getProgress());
    }
    writer.close();

    CHECK_OP(os, );
    if (sequenceCounter == 0){
        CHECK_EXT(isStarted == true, os.setError(RawDNASequenceFormat::tr("Sequence is empty")), );
    }
    if (isStarted){
        finishSequence(objects, io, os, dbiRef, fs, dbiObjects, seqImporter);
    }
}

Document* RawDNASequenceFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QList<GObject*> objects;
    load(io, dbiRef, objects, fs, os);
    CHECK_OP(os, NULL);
    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
    return doc;
}

FormatCheckResult RawDNASequenceFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    if (QRegExp("[a-zA-Z\r\n\\*-]*").exactMatch(rawData)) {
        return FormatDetection_VeryHighSimilarity;
    }
    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    //returning 'very low chance' here just because it's impossible to have 100% detection for this format
    return hasBinaryData ? FormatDetection_NotMatched : FormatDetection_VeryLowSimilarity;
}

void RawDNASequenceFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    QList<GObject*> objects = d->findGObjectByType(GObjectTypes::SEQUENCE);
    CHECK(objects.size() == 1, );
    GObject* obj = objects.first();
    U2SequenceObject* so = qobject_cast<U2SequenceObject*>(obj);
    SAFE_POINT(NULL != so, L10N::nullPointerError("Sequence object"), );
    QByteArray seqData = so->getWholeSequenceData(os);
    SAFE_POINT_OP(os, );
    PlainTextFormat::storeRawData(seqData, os, io);
}

void RawDNASequenceFormat::storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::SEQUENCE), "Raw sequence entry storing: no sequences", );
    const QList<GObject*> &seqs = objectsMap[GObjectTypes::SEQUENCE];
    SAFE_POINT(1 == seqs.size(), "Raw sequence entry storing: sequence objects count error", );

    U2SequenceObject *seq = dynamic_cast<U2SequenceObject*>(seqs.first());
    SAFE_POINT(NULL != seq, "Raw sequence entry storing: NULL sequence object", );

    QByteArray seqData = seq->getWholeSequenceData(os);
    SAFE_POINT_OP(os, );
    PlainTextFormat::storeRawData(seqData, os, io);
    CHECK_OP(os, );

    io->writeBlock("\n", 1);
}


}//namespace
