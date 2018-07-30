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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "PDWFormat.h"

namespace U2 {

/* TRANSLATOR U2::FastaFormat */
/* TRANSLATOR U2::IOAdapter */
/* TRANSLATOR U2::Document */


#define PDW_FORMAT_TAG      "VERSION  pDRAW 1.0"
#define PDW_SEQUENCE_TAG    "Sequence .."
#define PDW_DNANAME_TAG     "DNAname"
#define PDW_ELEMENT_TAG     "Element"
#define PDW_ANNOTATION_TAG  "Annotation"
#define PDW_CIRCULAR_TAG    "IScircular"

PDWFormat::PDWFormat(QObject* p)
: TextDocumentFormat(p, DocumentFormatFlag(DocumentFormatFlag_LockedIfNotCreatedByUGENE), QStringList()<<"pdw")
{
    formatName = tr("pDRAW");
    formatDescription = tr("pDRAW is a sequence file format used by pDRAW software");
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    supportedObjectTypes+=GObjectTypes::ANNOTATION_TABLE;
}

FormatCheckResult PDWFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    if (!rawData.startsWith(PDW_FORMAT_TAG)) {
        return FormatDetection_NotMatched;
    }

    bool hasBinaryBlocks = TextUtils::contains(TextUtils::BINARY, data, size);
    return hasBinaryBlocks ? FormatDetection_NotMatched : FormatDetection_HighSimilarity;
}

void PDWFormat::load(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, const GUrl& docUrl, QList<GObject*>& objects, U2OpStatus& os,
                     U2SequenceObject*& seqObj, AnnotationTableObject*& annObj)
{
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os,);
    Q_UNUSED(opBlock);

    QByteArray readBuff(READ_BUFF_SIZE+1, 0);
    char* buff = readBuff.data();
    qint64 len = 0;

    bool lineOk = false;
    bool isCircular = false;
    QString seqName(docUrl.baseFileName());
    QList<SharedAnnotationData> annotations;

    U2SequenceImporter seqImporter(fs, true);
    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    while (!os.isCoR()) {
        //read header
        len = io->readUntil(buff, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        if (len == 0) { //end if stream
            break;
        }
        if (!lineOk) {
            os.setError(PDWFormat::tr("Line is too long"));
        }

        if (readBuff.startsWith(PDW_DNANAME_TAG)) {
            seqName = readPdwValue(readBuff, PDW_DNANAME_TAG);
            if(seqName.isEmpty()){
                seqName = "Sequence";
            }
        } else if (readBuff.startsWith(PDW_SEQUENCE_TAG)) {
            QByteArray seq = parseSequence(io, os);
            const DNAAlphabet* alphabet = U2AlphabetUtils::findBestAlphabet(seq);
            DNASequence dnaSeq(seqName, seq , alphabet);
            if (isCircular) {
                DNALocusInfo loi;
                loi.topology = "circular";
                loi.name = seqName;
                dnaSeq.info.insert(DNAInfo::LOCUS, qVariantFromValue<DNALocusInfo>(loi));
            }

            seqImporter.startSequence(os, dbiRef, folder, dnaSeq.getName(), dnaSeq.circular);
            seqImporter.addBlock(seq.constData(), seq.length(), os);
            U2Sequence u2seq = seqImporter.finalizeSequenceAndValidate(os);
            TmpDbiObjects dbiObjects(dbiRef, os);
            dbiObjects.objects << u2seq.id;
            CHECK_OP(os,);

            seqObj = new U2SequenceObject(seqName, U2EntityRef(dbiRef, u2seq.id));
            seqObj->setSequenceInfo(dnaSeq.info);
            objects << seqObj;

            GObjectReference sequenceRef(io->getURL().getURLString(), seqName, GObjectTypes::SEQUENCE);
            U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, NULL, fs);

            break;
        } else if (readBuff.startsWith(PDW_CIRCULAR_TAG)) {
            QByteArray val = readPdwValue(readBuff, PDW_CIRCULAR_TAG);
            if (val == "YES") {
                isCircular = true;
            }
        } else if (readBuff.startsWith(PDW_ANNOTATION_TAG)) {
            SharedAnnotationData a = parseAnnotation(io, os);
            annotations.append(a);
        }
    }

    if (!annotations.isEmpty()) {
        QVariantMap hints;
        hints.insert(DBI_FOLDER_HINT, fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
        annObj = new AnnotationTableObject(QString("%1 annotations").arg(seqName), dbiRef, hints);
        annObj->addAnnotations(annotations);
        objects.append(annObj);
    }

    CHECK_OP(os,);
    CHECK_EXT(!objects.isEmpty(), os.setError(Document::tr("Document is empty.")),);
}

Document * PDWFormat::loadTextDocument(IOAdapter *io, const U2DbiRef &dbiRef, const QVariantMap &fs, U2OpStatus &os) {
    U2SequenceObject* seqObj = NULL;
    AnnotationTableObject *annObj = NULL;
    CHECK_EXT(NULL != io && io->isOpen(), os.setError(L10N::badArgument("IO adapter")),
        NULL);
    QList<GObject *> objects;

    load(io, dbiRef, fs, io->getURL(), objects, os, seqObj, annObj);

    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    QString lockReason(DocumentFormat::CREATED_NOT_BY_UGENE);
    Document *doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs,
        lockReason);

    if (NULL != seqObj && NULL != annObj) {
        annObj->addObjectRelation(seqObj, ObjectRole_Sequence);
    }
    return doc;
}

QByteArray PDWFormat::parseSequence(IOAdapter* io, U2OpStatus& ti) {
    QByteArray result;

    QByteArray readBuff(READ_BUFF_SIZE+1, 0);

    while (!ti.isCoR()) {
        bool lineOk = false;
        qint64 len = io->readUntil(readBuff.data(), READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        if (len == 0) {
            break;
        }
        if (!lineOk) {
            ti.setError(PDWFormat::tr("Line is too long"));
        }

        for (int i = 0; i < readBuff.size(); ++i) {
            char c = readBuff.at(i);
            if (c == '\n') {
                break;
            }
            if (c >= 'A' && c <= 'z') {
                result.append(c);
            }
        }
    }
    return result;
}

QByteArray PDWFormat::readPdwValue(const QByteArray& readBuf, const QByteArray& valueName)
{
    int startPos = valueName.length();
    int endPos = readBuf.indexOf('\n');
    return readBuf.mid(startPos, endPos - startPos + 1).trimmed();
}

#define PDW_ANNOTATION_NUMBER   "Annotation_Number"
#define PDW_ANNOTATION_NAME     "Annotation_Name"
#define PDW_ANNOTATION_START    "Annotation_Start"
#define PDW_ANNOTATION_END      "Annotation_End"
#define PDW_ANNOTATION_ORIENT   "Annotation_Orientation"

SharedAnnotationData PDWFormat::parseAnnotation(IOAdapter *io, U2OpStatus &ti) {
    QByteArray readBuf(READ_BUFF_SIZE + 1, 0);

    int startPos = -1, endPos = -1;
    QByteArray aName;
    bool cmpl = false;

    while (!ti.isCoR()) {
        bool lineOk = false;
        qint64 len = io->readUntil(readBuf.data(), READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);

        if (!readBuf.startsWith(PDW_ANNOTATION_TAG)) {
            break;
        } else if (readBuf.startsWith(PDW_ANNOTATION_NUMBER)) {
            io->skip(-len);
            break;
        } else if (readBuf.startsWith(PDW_ANNOTATION_NAME)) {
            aName = readPdwValue(readBuf, PDW_ANNOTATION_NAME);
            if (aName.endsWith('\"')) {
                aName = aName.left(aName.length() - 1).trimmed();
            }
        } else if (readBuf.startsWith(PDW_ANNOTATION_START)) {
            startPos = readPdwValue(readBuf, PDW_ANNOTATION_START).toInt();
        } else if (readBuf.startsWith(PDW_ANNOTATION_END)) {
            endPos = readPdwValue(readBuf, PDW_ANNOTATION_END).toInt();
        } else if (readBuf.startsWith(PDW_ANNOTATION_ORIENT)) {
            int orientVal = readPdwValue(readBuf, PDW_ANNOTATION_ORIENT).toInt();
            cmpl = orientVal == 0 ? true : false;
        }

    }

    SharedAnnotationData sd(new AnnotationData);
    sd->name = aName;
    sd->location->regions << U2Region(startPos - 1, endPos - startPos + 1);
    sd->setStrand(cmpl ? U2Strand::Complementary : U2Strand::Direct);

    return sd;
}

} // namespace U2
