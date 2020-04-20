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

#include <limits.h>

#include <QBuffer>
#include <QCoreApplication>
#include <QScopedPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAInfo.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Attribute.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "EMBLGenbankAbstractDocument.h"
#include "GenbankLocationParser.h"

namespace U2 {

const int ParserState::LOCAL_READ_BUFFER_SIZE = 4 * 1024 * 1024;

const QString EMBLGenbankAbstractDocument::REMOTE_ENTRY_WARNING_MESSAGE = QCoreApplication::translate("EMBLGenbankAbstractDocument", "The file contains features of another remote GenBank file. These features have been skipped.");
const QString EMBLGenbankAbstractDocument::JOIN_COMPLEMENT_WARNING_MESSAGE = QCoreApplication::translate("EMBLGenbankAbstractDocument", "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.");
const QString EMBLGenbankAbstractDocument::LOCATION_PARSING_ERROR_MESSAGE = QCoreApplication::translate("EMBLGenbankAbstractDocument", "Location parsing error.");
const QString EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE = QCoreApplication::translate("EMBLGenbankAbstractDocument", "The number of valid sequence characters does not coincide with the declared size in the sequence header.");

EMBLGenbankAbstractDocument::EMBLGenbankAbstractDocument(const DocumentFormatId& _id, const QString& _formatName, int mls,
    DocumentFormatFlags flags, QObject* p)
    : TextDocumentFormat(p, _id, flags),  maxAnnotationLineLen(mls), savedInUgene(false) {
    formatName = _formatName;
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
    supportedObjectTypes += GObjectTypes::SEQUENCE;
}


//////////////////////////////////////////////////////////////////////////
// loading

Document* EMBLGenbankAbstractDocument::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& _fs, U2OpStatus& os) {
    QVariantMap fs = _fs;
    QList<GObject*> objects;
    QString writeLockReason;
    load(dbiRef, io, objects, fs, os, writeLockReason);

    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    DocumentFormatUtils::updateFormatHints(objects, fs);
    fs[DocumentReadingMode_LoadAsModified] = os.hasWarnings() && checkFlags(DocumentFormatFlag_SupportWriting);
    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs, writeLockReason);
    return doc;
}

const QString EMBLGenbankAbstractDocument::UGENE_MARK("UNIMARK");
const QString EMBLGenbankAbstractDocument::DEFAULT_OBJ_NAME("unnamed");
const QString EMBLGenbankAbstractDocument::LOCUS_TAG_CIRCULAR("circular");
const QString EMBLGenbankAbstractDocument::LOCUS_TAG_LINEAR("linear");

void EMBLGenbankAbstractDocument::load(const U2DbiRef& dbiRef, IOAdapter* io, QList<GObject*>& objects, QVariantMap& fs, U2OpStatus& os, QString& writeLockReason) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );
    Q_UNUSED(opBlock);
    writeLockReason.clear();

    io->setFormatMode(IOAdapter::TextMode);
    //get settings
    int gapSize = qBound(-1, DocumentFormatUtils::getMergeGap(fs), 1000 * 1000);
    bool merge = gapSize != -1;

    QScopedPointer<AnnotationTableObject> mergedAnnotations(NULL);
    QStringList contigs;
    QVector<U2Region> mergedMapping;

    // Sequence loading is 'lazy', so, if there is no sequence, it won't be created and there is no need to remove it.
    U2SequenceImporter seqImporter(fs, true);
    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    QSet<QString> usedNames;

    GObjectReference sequenceRef(GObjectReference(io->getURL().getURLString(), "", GObjectTypes::SEQUENCE));

    QByteArray readBuffer(ParserState::LOCAL_READ_BUFFER_SIZE, '\0');
    ParserState st(isNcbiLikeFormat() ? 12 : 5, io, NULL, os);
    st.buff = readBuffer.data();

    TmpDbiObjects dbiObjects(dbiRef, os);
    int num_sequence = 0;

    qint64 sequenceStart = 0;
    int fullSequenceSize = 0;
    const int objectsCountLimit = fs.contains(DocumentReadingMode_MaxObjectsInDoc) ? fs[DocumentReadingMode_MaxObjectsInDoc].toInt() : -1;

    for (int i = 0; !os.isCoR(); i++, ++num_sequence) {
        if (objectsCountLimit > 0 && objects.size() >= objectsCountLimit) {
            os.setError(EMBLGenbankAbstractDocument::tr("File \"%1\" contains too many sequences to be displayed. "
                "However, you can process these data using instruments from the menu <i>Tools -> NGS data analysis</i> "
                "or pipelines built with Workflow Designer.")
                .arg(io->getURL().getURLString()));
            break;
        }

        //TODO: reference to a local variable??? Such a pointer will become invalid
        EMBLGenbankDataEntry data;
        st.entry = &data;

        if (num_sequence == 0 || merge == false) {
            seqImporter.startSequence(os, dbiRef, folder, "default sequence name", false); //change name and circularity after finalize method
            CHECK_OP(os, );
        }

        int sequenceSize = 0;
        os.setDescription(tr("Reading entry header"));
        int offset = 0;
        if (merge && num_sequence > 0) {
            offset = gapSize;
        }
        if (!readEntry(&st, seqImporter, sequenceSize, fullSequenceSize, merge, offset, os)) {
            break;
        }

        if (merge && sequenceSize > 0 && num_sequence > 0) {
            sequenceStart = fullSequenceSize - sequenceSize;
            sequenceStart += gapSize;
            fullSequenceSize += gapSize;
        }

        // tolerate blank lines between records
        char ch;
        bool b;
        while ((b = st.io->getChar(&ch)) && (ch == '\n' || ch == '\r')) {}
        CHECK_EXT(!st.io->hasError(), os.setError(st.io->errorString()), );

        if (b) {
            st.io->skip(-1);
            CHECK_EXT(!st.io->hasError(), os.setError(st.io->errorString()), );
        }

        AnnotationTableObject *annotationsObject = NULL;

        if (data.hasAnnotationObjectFlag) {
            QString annotationName = genObjectName(usedNames, data.name, data.tags, i + 1, GObjectTypes::ANNOTATION_TABLE);

            QVariantMap hints;
            hints.insert(DBI_FOLDER_HINT, fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
            if (Q_UNLIKELY(merge && NULL == mergedAnnotations)) {
                mergedAnnotations.reset(new AnnotationTableObject(annotationName, dbiRef, hints));
            }
            annotationsObject = merge ? mergedAnnotations.data() : new AnnotationTableObject(annotationName, dbiRef, hints);

            QStringList groupNames;
            QMap<QString, QList<SharedAnnotationData> > groupName2Annotations;
            for (int i = 0, n = data.features.size(); i < n; ++i) {
                SharedAnnotationData &d = data.features[i];
                if (!d->location->regions.isEmpty()) {
                    for (int i = 0, n = d->location->regions.size(); i < n; ++i) {
                        // for some reason larger numbers cannot be stored within rtree SQLite tables
                        if (d->location->regions[i].endPos() > 9223371036854775807LL) {
                            d->location->regions[i].length = 9223371036854775807LL - d->location->regions[i].startPos;
                        }
                    }
                }
                groupNames.clear();
                d->removeAllQualifiers(GBFeatureUtils::QUALIFIER_GROUP, groupNames);
                if (groupNames.isEmpty()) {
                    groupName2Annotations[""].append(d);
                } else {
                    foreach(const QString &gName, groupNames) {
                        groupName2Annotations[gName].append(d);
                    }
                }
                CHECK_OP(os, );
            }
            foreach(const QString &groupName, groupName2Annotations.keys()) {
                annotationsObject->addAnnotations(groupName2Annotations[groupName], groupName);
            }

            createCommentAnnotation(data.tags.value(DNAInfo::COMMENT).toStringList(), sequenceSize, annotationsObject);

            if (!merge) {
                objects.append(annotationsObject);
            }
        } else {
            assert(data.features.isEmpty());
        }

        if (!os.isCoR()) {
            QString sequenceName = genObjectName(usedNames, data.name, data.tags, i + 1, GObjectTypes::SEQUENCE);
            if (merge && sequenceSize == 0 && annotationsObject != NULL) {
                os.setError(tr("Merge error: found annotations without sequence"));
                break;
            } else if (merge) {
                contigs.append(sequenceName);
                mergedMapping.append(U2Region(sequenceStart, sequenceSize));
            } else {
                U2Sequence u2seq = seqImporter.finalizeSequence(os);
                CHECK_OP(os, );

                if (sequenceSize != 0) {
                    u2seq.visualName = sequenceName;
                    u2seq.circular = data.circular;
                    DbiConnection con(dbiRef, os);
                    con.dbi->getSequenceDbi()->updateSequenceObject(u2seq, os);

                    fullSequenceSize = 0;

                    U2SequenceObject* seqObj = new U2SequenceObject(sequenceName, U2EntityRef(dbiRef, u2seq.id));
                    QString translation = U1AnnotationUtils::guessAminoTranslation(annotationsObject, seqObj->getAlphabet());
                    if (!translation.isEmpty()) {
                        seqObj->setStringAttribute(translation, Translation_Table_Id_Attribute);
                    }

                    objects << seqObj;
                    dbiObjects.objects << u2seq.id;

                    SAFE_POINT(seqObj != NULL, "DocumentFormatUtils::addSequenceObject returned NULL but didn't set error", );

                    sequenceRef.objName = seqObj->getGObjectName();
                    sequenceRef.entityRef = seqObj->getEntityRef();
                    if (annotationsObject != NULL) {
                        annotationsObject->addObjectRelation(GObjectRelation(sequenceRef, ObjectRole_Sequence));
                    }
                    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, annotationsObject, fs);

                    readHeaderAttributes(data.tags, con, seqObj);

                    // try to guess relevant translation from a CDS feature (if any)
                }
            }
        }
    }
    CHECK_OP(os, );
    CHECK_EXT(!objects.isEmpty() || merge, os.setError(Document::tr("Document is empty.")), );
    SAFE_POINT(contigs.size() == mergedMapping.size(), "contigs <-> regions mapping failed!", );

    if (!merge) {
        return;
    }
    writeLockReason = QObject::tr("Document sequences were merged");

    U2Sequence u2seq = seqImporter.finalizeSequenceAndValidate(os);
    dbiObjects.objects << u2seq.id;

    CHECK_OP(os, );

    u2seq.visualName = "Sequence";
    DbiConnection con(dbiRef, os);
    con.dbi->getSequenceDbi()->updateSequenceObject(u2seq, os);

    sequenceRef.objName = u2seq.visualName;
    sequenceRef.entityRef = U2EntityRef(dbiRef, u2seq.id);

    CHECK_OP(os, );
    U2SequenceObject* so = new U2SequenceObject(u2seq.visualName, U2EntityRef(dbiRef, u2seq.id));
    objects << so;
    objects << DocumentFormatUtils::addAnnotationsForMergedU2Sequence(sequenceRef, dbiRef, contigs, mergedMapping, fs);
    AnnotationTableObject *mergedAnnotationsPtr = mergedAnnotations.take();
    if (NULL != mergedAnnotationsPtr) {
        sequenceRef.entityRef = U2EntityRef(dbiRef, u2seq.id);
        mergedAnnotationsPtr->addObjectRelation(GObjectRelation(sequenceRef, ObjectRole_Sequence));
        objects.append(mergedAnnotationsPtr);
    }
    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, mergedAnnotationsPtr, fs);
}

DNASequence* EMBLGenbankAbstractDocument::loadTextSequence(IOAdapter* io, U2OpStatus& os) {
    QSet<QString> usedNames;

    QByteArray sequenceData;
    U2MemorySequenceImporter seqImporter(sequenceData);
    QByteArray readBuffer(ParserState::LOCAL_READ_BUFFER_SIZE, '\0');
    ParserState st(isNcbiLikeFormat() ? 12 : 5, io, NULL, os);
    st.buff = readBuffer.data();

    int sequenceSize = 0;
    int fullSequenceSize = 0;

    EMBLGenbankDataEntry data;
    st.entry = &data;

    sequenceSize = 0;
    os.setDescription(tr("Reading entry header"));
    int offset = 0;
    bool merge = false;
    if (!readEntry(&st, seqImporter, sequenceSize, fullSequenceSize, merge, offset, os)) {
        return NULL;
    }

    // tolerate blank lines between records
    char ch;
    bool b;
    while ((b = st.io->getChar(&ch)) && (ch == '\n' || ch == '\r')) {}
    CHECK_EXT(!st.io->hasError(), os.setError(st.io->errorString()), NULL);

    if (b) {
        st.io->skip(-1);
        CHECK_EXT(!st.io->hasError(), os.setError(st.io->errorString()), NULL);
    }

    if (os.isCoR()) {
        return NULL;
    }
    QString sequenceName = genObjectName(usedNames, data.name, data.tags, 1, GObjectTypes::SEQUENCE);

    if (sequenceSize != 0) {
        DNASequence* seq = new DNASequence(sequenceName, sequenceData, U2AlphabetUtils::getById(seqImporter.getAlphabet()));
        return seq;
    }

    return NULL;
}



//column annotation data starts with
#define A_COL 21
//column qualifier name starts with
#define QN_COL 22
//column annotation key starts with
#define K_COL 5


static bool isNewQStart(const char* s, int l) {
    if (l < A_COL + 1 || s[A_COL] != '/') {
        return false;
    }
    const QBitArray& WHITES = TextUtils::WHITES;
    int i = QN_COL;
    bool hasWhites = false;
    bool hasEqualitySign = false; // qualifier with '=' char if true and without otherwise
    bool hasWhitesBeforeVal = false;
    for (; i < l; i++) {
        char c = s[i];
        if (c == '=' && i > QN_COL) {
            hasEqualitySign = true;
            if (WHITES[s[i + 1]]) { // there is whites between potential qual and val
                hasWhitesBeforeVal = true;
            }
            break;
        }
        if (WHITES[(uchar)c]) {
            hasWhites = true;
        }
        if (hasWhites && !WHITES[(uchar)c]) { // there is !whites characters after qualifier without '=' char
            hasWhitesBeforeVal = true;
            break;
        }
    }

    if (hasEqualitySign) {
        if (hasWhites) { // whites between qual and '=' char
            return false;
        }
        return true;
    } else if (hasWhitesBeforeVal) {
        return false;
    }

    return true; // qualifier without '=' char
}

static int numQuotesInLine(char* cbuff, int len) {
    QString line = QString(QByteArray(cbuff, len));
    int pos = 0;
    int numQuotes = 0;
    while ((pos = line.indexOf('\"', pos + 1)) != -1) {
        if (line[pos + 1] == '\"') { // skip ""
            pos++;
            continue;
        }
        numQuotes++;
    }
    return numQuotes;
}

//TODO: make it IO active -> read util the end. Otherwise qualifier is limited in size by maxSize
int EMBLGenbankAbstractDocument::readMultilineQualifier(IOAdapter* io, char* cbuff, int maxSize, bool _prevLineHasMaxSize, int lenFirstLine,
    U2OpStatus& os) {
    io->setFormatMode(IOAdapter::TextMode);
    int len = 0;
    bool lineOk = true;
    static const int MAX_LINE = 256;
    int sizeToSkip = maxSize - MAX_LINE;
    const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;

    int numQuotes = 0;
    numQuotes += numQuotesInLine(cbuff, lenFirstLine);

    cbuff += lenFirstLine;

    bool breakWords = !_prevLineHasMaxSize; //todo: create a parameter and make it depends on annotation name.
    do {
        if (len >= sizeToSkip) {
            QByteArray skip(MAX_LINE, 0);
            char* skipBuff = skip.data();
            do {
                int readLen = io->readUntil(skipBuff, MAX_LINE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
                CHECK_EXT(!io->hasError(), os.setError(io->errorString()), -1);
                CHECK_CONTINUE(lineOk);//todo: report error!

                int lineLen = readLen;
                for (; A_COL < lineLen && LINE_BREAKS[(uchar)skipBuff[lineLen - 1]]; lineLen--) {}; //remove line breaks
                if (lineLen == 0 || lineLen < A_COL || skip[0] != fPrefix[0] || skip[1] != fPrefix[1]
                    || skip[K_COL] != ' ' || (skip[A_COL] == '/' && isNewQStart(skip, lineLen) && (numQuotes % 2) == 0)) {
                    io->skip(-readLen);
                    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), -1);
                    break;
                } else {
                    numQuotes += numQuotesInLine(skipBuff, lineLen);
                }
            } while (true);
            break;
        }
        char* lineBuf = cbuff + len;
        int readLen = io->readUntil(lineBuf, maxSize - len, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), -1);

        int lineLen = readLen;
        for (; A_COL < lineLen && LINE_BREAKS[(uchar)lineBuf[lineLen - 1]]; lineLen--) {}; //remove line breaks
        if (!lineOk || lineLen == 0 || lineLen < A_COL || lineBuf[0] != fPrefix[0]
            || lineBuf[1] != fPrefix[1] || lineBuf[K_COL] != ' ' || (lineBuf[A_COL] == '/' && isNewQStart(lineBuf, lineLen) && (numQuotes % 2) == 0)) {
            io->skip(-readLen);
            CHECK_EXT(!io->hasError(), os.setError(io->errorString()), -1);
            break;
        } else {
            numQuotes += numQuotesInLine(lineBuf, lineLen);
        }

        if (breakQualifierOnSpaceOnly(cbuff) && breakWords && lineLen - A_COL > 0) { //add space to separate words
            cbuff[len] = ' ';
            len++;
        }

        memmove(cbuff + len, lineBuf + A_COL, lineLen - A_COL);
        len += lineLen - A_COL;
        breakWords = breakWords || lineLen < maxAnnotationLineLen;
    } while (true);

    // ignore that error if the file was saved by UGENE (version <1.14.1)
    if (numQuotes != 2 && numQuotes != 0 && !savedInUgene) {
        os.setError(tr("The file contains an incorrect data that describes a qualifier value. ") +
            tr("The value cannot contain a single quote character. The qualifier is \'%1\'").arg(QByteArray(cbuff - lenFirstLine, len + lenFirstLine).data()));
        return 0;
    }
    return len;
}


QString EMBLGenbankAbstractDocument::genObjectName(QSet<QString>& usedNames, const QString& seqName, const QVariantMap& tags, int n, const GObjectType& t) {
    QString name;

    //try to check UGENE_MARK first
    QStringList unimark = tags.value(UGENE_MARK).toStringList();
    if (1 == unimark.size()) {
        name = unimark[0];
    } else if (2 == unimark.size()) {
        int idx = (GObjectTypes::ANNOTATION_TABLE == t) ? 0 : 1;
        name = unimark[idx];
    }

    if (name.isEmpty()) {
        name = seqName;
        if (name.isEmpty()) {
            name = DNAInfo::getPrimaryAccession(tags);
            int spaceIdx = name.indexOf(' ');
            if (spaceIdx > 0) {
                name = name.left(spaceIdx - 1);
            }
            if (name.isEmpty()) {
                name = EMBLGenbankAbstractDocument::DEFAULT_OBJ_NAME;
            }
        }
    }

    const QString featuresPostfix = "features";
    const QString annotsPostfix = "annotations";
    if (t == GObjectTypes::ANNOTATION_TABLE && !name.endsWith(featuresPostfix, Qt::CaseInsensitive)
        && !name.endsWith(annotsPostfix, Qt::CaseInsensitive)) {
        name += " " + featuresPostfix;
    }
    //now variate name if there are duplicates
    int n2 = 1;
    QString res = name;
    while (usedNames.contains(res)) {
        res = name + " " + QString::number(n) + (n2 == 1 ? QString("") : ("." + QString::number(n2)));
        ++n2;
    }
    usedNames.insert(res);
    return res;
}

static void checkQuotes(const char* str, int len, bool& outerQuotes, bool& doubleQuotes) {
    char qChar = '\"';
    assert(len >= 0);
    outerQuotes = str[0] == qChar && str[len - 1] == qChar;
    for (int i = 1; i < len; i++) {
        if (str[i - 1] == qChar && str[i] == qChar) {
            doubleQuotes = true;
            break;
        }
    }
}
namespace {

enum AnnotationProcessStatus {
    ProcessAnnotation,
    SkipAnnotation
};

void addUniqueWarning(U2OpStatus &si, const QString &warning) {
    const QStringList warnings = si.getWarnings();
    if (!warnings.contains(warning)) {
        si.addWarning(warning);
    }
}

AnnotationProcessStatus processParsingResult(const U2Location &location, Genbank::LocationParser::ParsingResult parsingResult, const QStringList &parsingMessages, U2OpStatus &si) {
    switch (parsingResult) {
    case Genbank::LocationParser::Success:
        return ProcessAnnotation;
    case Genbank::LocationParser::ParsedWithWarnings:
        foreach(const QString &message, parsingMessages) {
            if (message.contains(Genbank::LocationParser::REMOTE_ENTRY_WARNING)) {
                addUniqueWarning(si, EMBLGenbankAbstractDocument::REMOTE_ENTRY_WARNING_MESSAGE);
            } else if (message.contains(Genbank::LocationParser::JOIN_COMPLEMENT_WARNING)) {
                addUniqueWarning(si, EMBLGenbankAbstractDocument::JOIN_COMPLEMENT_WARNING_MESSAGE);
            } else {
                si.addWarning(message);
            }
        }
        return location->isEmpty() ? SkipAnnotation : ProcessAnnotation;
    case Genbank::LocationParser::Failure:
        si.setError(parsingMessages.isEmpty() ? EMBLGenbankAbstractDocument::LOCATION_PARSING_ERROR_MESSAGE : parsingMessages.last());
        return SkipAnnotation;
    default:
        assert(false);
        return ProcessAnnotation;
    }
}

}

SharedAnnotationData EMBLGenbankAbstractDocument::readAnnotation(IOAdapter* io, char* cbuff, int len,
    int READ_BUFF_SIZE, U2OpStatus& si, int offset, int seqLen) {
    AnnotationData* a = new AnnotationData();
    SharedAnnotationData f(a);
    QString key = QString::fromLatin1(cbuff + 5, 15).trimmed();
    if (key.isEmpty()) {
        si.setError(EMBLGenbankAbstractDocument::tr("Annotation name is empty"));
        return SharedAnnotationData();
    }
    a->name = key;
    a->type = getFeatureType(key);

    //qualifier starts on offset 22;
    int qlen = len + readMultilineQualifier(io, cbuff, READ_BUFF_SIZE - len, true, len, si);
    CHECK_OP(si, SharedAnnotationData());

    if (qlen < 21) {
        si.setError(EMBLGenbankAbstractDocument::tr("Error parsing location"));
        return SharedAnnotationData();
    }

    QStringList messages;
    Genbank::LocationParser::ParsingResult parsingResult = Genbank::LocationParser::parseLocation(cbuff + 21, qlen - 21, a->location, messages, seqLen);
    if (SkipAnnotation == processParsingResult(a->location, parsingResult, messages, si)) {
        skipInvalidAnnotation(si, len, io, cbuff, READ_BUFF_SIZE);
        return SharedAnnotationData();
    }

    if (offset > 0) {
        U2Region::shift(offset, a->location->regions);
    }

    const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;

    //here we have valid key and location;
    //reading qualifiers
    bool lineOk = true;
    while ((len = io->readUntil(cbuff, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk)) > 0) {
        CHECK_EXT(!io->hasError(), si.setError(io->errorString()), SharedAnnotationData());

        if (len == 0 || len < QN_COL + 1 || cbuff[K_COL] != ' ' || cbuff[A_COL] != '/' || cbuff[0] != fPrefix[0] || cbuff[1] != fPrefix[1]) {
            io->skip(-len);
            CHECK_EXT(!io->hasError(), si.setError(io->errorString()), SharedAnnotationData());
            break;
        }
        CHECK_EXT_BREAK(lineOk, si.setError(EMBLGenbankAbstractDocument::tr("Unexpected line format")));

        for (; QN_COL < len && LINE_BREAKS[(uchar)cbuff[len - 1]]; len--) {}; //remove line breaks

        int flen = len + readMultilineQualifier(io, cbuff, READ_BUFF_SIZE - len, len >= maxAnnotationLineLen, len, si);
        CHECK_OP(si, SharedAnnotationData());

        //now the whole feature is in cbuff
        int valStart = A_COL + 1;
        for (; valStart < flen && cbuff[valStart] != '='; valStart++) {}; //find '==' and valStart
        if (valStart < flen) {
            valStart++; //skip '=' char
        }
        const QBitArray& WHITE_SPACES = TextUtils::WHITES;
        for (; valStart < flen && WHITE_SPACES[(uchar)cbuff[flen - 1]]; flen--) {}; //trim value
        const char* qname = cbuff + QN_COL;
        int qnameLen = valStart - (QN_COL + 1);

        // Check the case when a qualifier has no value
        QString noValueQualifier = QString::fromLocal8Bit(qname, qnameLen + 1);
        if (GBFeatureUtils::isFeatureHasNoValue(noValueQualifier)) {
            qnameLen += 1;
        }

        const char* qval = cbuff + valStart;
        int qvalLen = flen - valStart;
        bool removeQuotes = false;
        bool containsDoubleQuotes = false;
        checkQuotes(qval, qvalLen, removeQuotes, containsDoubleQuotes);
        if (removeQuotes) {
            qval++;
            qvalLen -= 2;
        }

        QString nameQStr = QString::fromLocal8Bit(qname, qnameLen);
        QString valQStr = QString::fromLocal8Bit(qval, qvalLen);

        if (!breakQualifierOnSpaceOnly(QString::fromLocal8Bit(qname, qnameLen))) {
            valQStr.replace("\\ ", " ");
        }

        if (nameQStr == GBFeatureUtils::QUALIFIER_NAME) {
            a->name = valQStr;
        } else {
            a->qualifiers << createQualifier(nameQStr, valQStr, containsDoubleQuotes);
        }
    }
    return f;
}

bool EMBLGenbankAbstractDocument::readSequence(ParserState* st, U2SequenceImporter& seqImporter, int& sequenceLen, int& fullSequenceLen, U2OpStatus& os) {
    // FIXME use ParserState instead
    QByteArray res;
    IOAdapter* io = st->io;
    U2OpStatus& si = st->si;
    si.setDescription(tr("Reading sequence %1").arg(st->entry->name));
    QByteArray readBuffer(DocumentFormat::READ_BUFF_SIZE, '\0');
    char* buff = readBuffer.data();

    //reading sequence
    int len;
    sequenceLen = 0;
    fullSequenceLen = 0;
    while ((len = io->readLine(buff, DocumentFormat::READ_BUFF_SIZE)) > 0) {
        CHECK_EXT_BREAK(!si.isCoR(), res.clear());
        CHECK_EXT_BREAK(len > 0, si.setError(tr("Error parsing sequence: unexpected empty line")));
        CHECK_BREAK(buff[0] != '/');//end of the sequence

        len = TextUtils::remove(buff, len, TextUtils::WHITES | TextUtils::NUMS);
        seqImporter.addBlock(buff, len, os);
        CHECK_OP_BREAK(os);

        sequenceLen += len;
        fullSequenceLen += len;

        si.setProgress(io->getProgress());
    }
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), false);

    if (!si.isCoR() && buff[0] != '/') {
        si.setError(tr("Sequence is truncated"));
    }
    return true; //FIXME
}

void EMBLGenbankAbstractDocument::readAnnotations(ParserState* st, int offset) {
    st->si.setDescription(tr("Reading annotations %1").arg(st->entry->name));
    st->entry->hasAnnotationObjectFlag = true;
    do {
        if (st->hasKey("XX") && getFormatId() == BaseDocumentFormats::PLAIN_EMBL) {
            continue;
        }
        int fplen = fPrefix.length();
        if (st->len >= 6 && TextUtils::equals(fPrefix.data(), st->buff, fplen)) {
            while (fplen < 5) {
                if (st->buff[fplen++] != ' ') {
                    st->si.setError(tr("Invalid format of feature table"));
                    break;
                }
            }
        } else {
            // end of feature table
            break;
        }
        //parsing feature;
        SharedAnnotationData f = readAnnotation(st->io, st->buff, st->len, ParserState::LOCAL_READ_BUFFER_SIZE, st->si, offset, st->entry->seqLen);
        if (f == NULL) {
            continue;
        }
        st->entry->features.push_back(f);
    } while (st->readNextLine());
}

bool EMBLGenbankAbstractDocument::isNcbiLikeFormat() const {
    return false;
}

void EMBLGenbankAbstractDocument::createCommentAnnotation(const QStringList & /*comments*/, int /*sequenceLength*/, AnnotationTableObject * /*annTable*/) const {
    // Do nothing
}

U2FeatureType EMBLGenbankAbstractDocument::getFeatureType(const QString & /*typeString*/) const {
    return U2FeatureTypes::MiscFeature;
}

U2Qualifier EMBLGenbankAbstractDocument::createQualifier(const QString &qualifierName, const QString &qualifierValue, bool containsDoubleQuotes) const {
    QString parsedQualifierValue = qualifierValue;
    if (containsDoubleQuotes) {
        parsedQualifierValue = parsedQualifierValue.replace("\"\"", "\"");
    }
    return U2Qualifier(qualifierName, parsedQualifierValue);
}

bool EMBLGenbankAbstractDocument::breakQualifierOnSpaceOnly(const QString & /*qualifierName*/) const {
    return true;
}

void EMBLGenbankAbstractDocument::skipInvalidAnnotation(U2OpStatus& si, int len, IOAdapter* io, char* cbuff, int READ_BUFF_SIZE) {
    bool lineOk = true;
    bool isQuotesOpened = false;
    while ((len = io->readUntil(cbuff, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk)) > 0) {
        CHECK_EXT(!io->hasError(), si.setError(io->errorString()), );

        QByteArray line(cbuff, len);
        bool isOpenedOnThisLine = line.count('"') % 2 == 1;
        if (isQuotesOpened || isOpenedOnThisLine) {
            if (isQuotesOpened && isOpenedOnThisLine) {
                isQuotesOpened = false;
            } else {
                isQuotesOpened = true;
            }
            continue;
        }
        if (len == 0 || len < QN_COL + 1 || cbuff[K_COL] != ' ' || cbuff[A_COL] != '/' || cbuff[0] != fPrefix[0] || cbuff[1] != fPrefix[1]) {
            io->skip(-len);
            CHECK_EXT(!io->hasError(), si.setError(io->errorString()), );
            break;
        }
    }
    return;
}

//-------------------------------------------------------------------
//  ParserState
//-------------------------------------------------------------------

bool ParserState::hasKey(const char* key, int slen) const {
    assert(slen <= valOffset);
    if (slen <= len && TextUtils::equals(key, buff, slen)) {
        while (slen < qMin(valOffset, len)) {
            if (buff[slen++] != ' ') {
                return false;
            }
        }
        return true;
    }
    return false;
}

QString ParserState::value() const {
    return len > valOffset ? QString::fromLocal8Bit(buff + valOffset, len - valOffset) : QString();
}

bool ParserState::readNextLine(bool emptyOK) {
    CHECK_OP_EXT(si, len = 0, false);

    bool ok = false;
    len = io->readLine(buff, LOCAL_READ_BUFFER_SIZE, &ok);
    CHECK_EXT(!io->hasError(), si.setError(io->errorString()), false);

    si.setProgress(io->getProgress());

    if (!ok && len == LOCAL_READ_BUFFER_SIZE) {
        si.setError(U2::EMBLGenbankAbstractDocument::tr("Line is too long."));
    } else if (len == -1) {
        si.setError(U2::EMBLGenbankAbstractDocument::tr("IO error."));
    }
    return (len > 0 || (emptyOK && ok));
}

QString ParserState::key() const {
    //assert(len > 0);
    return QString::fromLocal8Bit(buff, qMin(valOffset - 1, len));
}

}//namespace
