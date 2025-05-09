/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "GenbankPlainTextFormat.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DocumentFormatUtils.h"
#include "GenbankLocationParser.h"

namespace U2 {

GenbankPlainTextFormat::GenbankPlainTextFormat(QObject* p)
    : EMBLGenbankAbstractDocument(BaseDocumentFormats::PLAIN_GENBANK, tr("GenBank"), 79,
                                  DocumentFormatFlags_SW | DocumentFormatFlag_HasModifiableName, p) {
    formatDescription = tr("GenBank Flat File Format is a rich format for storing sequences and associated annotations");
    fileExtensions << "gb"
                   << "gbk"
                   << "gen"
                   << "genbank";
    sequenceStartPrefix = "ORIGIN";
    fPrefix = "  ";
}

FormatCheckResult GenbankPlainTextFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    // TODO: improve handling
    const char* data = rawData.constData();
    int size = rawData.size();
    bool textOnly = !TextUtils::contains(TextUtils::BINARY, data, size);
    if (!textOnly || size < 100) {
        return FormatDetection_NotMatched;
    }

    bool hasLocus = rawData.contains("\nLOCUS ") || rawData.startsWith("LOCUS ");
    if (!hasLocus) {
        return FormatDetection_NotMatched;
    }
    FormatCheckResult res(FormatDetection_VeryHighSimilarity);

    QByteArray seqStartPattern1 = "\n        1";
    QByteArray seqStartPattern2 = "\nORIGIN";

    res.properties[RawDataCheckResult_Sequence] = rawData.contains(seqStartPattern1) || rawData.contains(seqStartPattern2);

    bool multi = (rawData.indexOf(seqStartPattern1) != rawData.lastIndexOf(seqStartPattern1)) || (rawData.indexOf(seqStartPattern2) != rawData.lastIndexOf(seqStartPattern2));
    res.properties[RawDataCheckResult_MultipleSequences] = multi;
    return res;
}

bool GenbankPlainTextFormat::isStreamingSupport() {
    return true;
}

bool GenbankPlainTextFormat::readIdLine(ParserState* st) {
    static const QByteArray LOCUS = DNAInfo::LOCUS.toLocal8Bit();
    if (!st->hasKey(LOCUS)) {
        QByteArray rawData(st->buff);
        int locusStartPos = rawData.indexOf("\n" + LOCUS);
        if (locusStartPos != -1) {  // We are here if the "GenBank" file has some pre-description (see UGENE-4463)
            while (locusStartPos >= st->len) {
                st->readNextLine();
                rawData = QByteArray(st->buff);
                locusStartPos = rawData.indexOf("\n" + LOCUS);
            }
            st->buff = st->buff + locusStartPos;
        } else {
            // The GenBank file should have the indent (st->valOffset) before the real data
            // Possibly, this GenBank file doesn't have the strong indent (UGENE-7092)
            rawData = QByteArray::fromRawData(st->buff, st->len);
            CHECK_EXT(rawData.indexOf(LOCUS) == 0, st->si.setError(tr("LOCUS is not the first line")), false);

            // Indent before the beginnig of the line and name of the sequence
            int indentBeforeName = 0;
            do {
                if (indentBeforeName == 0) {
                    indentBeforeName = LOCUS.size();
                } else {
                    indentBeforeName++;
                }
                rawData = QByteArray::fromRawData(st->buff + indentBeforeName, st->len - indentBeforeName);
            } while (rawData.front() == ' ');
            int shiftToLeft = st->valOffset - indentBeforeName;
            st->buff -= shiftToLeft;
        }
    }

    QString locusStr = st->value();
    QStringList tokens = locusStr.split(QRegExp("(\t| )"), Qt::SkipEmptyParts);  // separators: tabs and spaces
    if (tokens.isEmpty()) {
        st->si.setError(tr("Error parsing LOCUS line"));
        return false;
    }
    // try improving name readability
    tokens[0] = tokens[0].replace(QRegExp("_(?![0-9])"), QChar(' '));
    st->entry->name = tokens[0];

    if (tokens.size() >= 3 && (tokens[2] == "bp" || tokens[2] == "aa")) {
        QString len = tokens[1];
        st->entry->seqLen = len.toInt();
    }
    if (tokens.size() == 7) {
        // seems to be canonical header
        DNALocusInfo loi;
        loi.name = tokens[0];
        loi.topology = tokens[4];
        loi.molecule = tokens[3];
        loi.division = tokens[5];
        loi.date = tokens[6];
        st->entry->tags.insert(DNAInfo::LOCUS, QVariant::fromValue<DNALocusInfo>(loi));
        st->entry->circular = loi.topology.compare(LOCUS_TAG_CIRCULAR, Qt::CaseInsensitive) == 0;
    } else {
        st->entry->tags.insert(DNAInfo::ID, tokens[0]);
        st->entry->tags.insert(DNAInfo::EMBL_ID, locusStr);
        st->entry->circular = locusStr.contains(LOCUS_TAG_CIRCULAR, Qt::CaseInsensitive);
    }
    return true;
}

bool GenbankPlainTextFormat::readEntry(ParserState* st, U2SequenceImporter& seqImporter, int& sequenceLen, int& fullSequenceLen, bool merge, int gapSize, U2OpStatus& os) {
    U2OpStatus& si = st->si;
    QString lastTagName;
    bool hasLine = false;
    savedInUgene = false;
    while (hasLine || st->readNextLine(true)) {
        hasLine = false;
        if (st->len == 0) {
            continue;
        }
        if (st->isNull()) {
            readIdLine(st);
            assert(si.hasError() || !st->entry->name.isEmpty());
            continue;
        }
        if (st->entry->tags.contains(UGENE_MARK) && !savedInUgene) {
            savedInUgene = true;
        }

        if (st->hasKey("FEATURES") && st->readNextLine()) {
            readAnnotations(st, fullSequenceLen + gapSize);
            hasLine = true;
            continue;
        }
        if (st->hasKey("SOURCE")) {
            DNASourceInfo soi;
            soi.name = st->value();
            while (st->readNextLine() && st->hasContinuation()) {
                soi.name.append(st->value());
            }
            if (st->hasKey("  ORGANISM")) {
                soi.organism = st->value();
                while (st->readNextLine() && st->hasContinuation()) {
                    soi.taxonomy.append(st->value());
                }
            } else {
                st->si.setError(tr("incomplete SOURCE record"));
                break;
            }
            st->entry->tags.insert(DNAInfo::SOURCE, QVariant::fromValue<DNASourceInfo>(soi));
            hasLine = true;
            continue;
        }
        if (st->hasKey("REFERENCE")) {
            DNAReferenceInfo ri;
            ri.referencesRecord.append(st->value());
            while (st->readNextLine() && (st->hasContinuation() || st->hasKey("REFERENCE") || st->hasKey("  AUTHORS") || st->hasKey("  TITLE") || st->hasKey("  JOURNAL") || st->hasKey("  MEDLINE") || st->hasKey("   PUBMED") || (st->hasValue() && st->buff[0] == ' '))) {  // read until the end of the references record
                ri.referencesRecord.append("\n" + QByteArray(st->buff, st->len));
            }

            st->entry->tags.insert(DNAInfo::REFERENCE, QVariant::fromValue<DNAReferenceInfo>(ri));
            hasLine = true;
            continue;
        }
        if (st->hasKey("COMMENT")) {
            QStringList commentSection(st->value());
            while (st->readNextLine() && st->hasContinuation()) {
                commentSection.append(st->value());
            }
            if (!commentSection.isEmpty()) {
                st->entry->tags[DNAInfo::COMMENT] = st->entry->tags[DNAInfo::COMMENT].toStringList() << commentSection.join(" ");
            }
            hasLine = true;
            continue;
        }
        if (st->hasKey("ACCESSION") || (st->hasContinuation() && lastTagName == "ACCESSION")) {
            QVariant v = st->entry->tags.value(DNAInfo::ACCESSION);
            st->entry->tags[DNAInfo::ACCESSION] = QVariantUtils::addStr2List(v, st->value().split(" "));
            lastTagName = "ACCESSION";
            continue;
        }
        if (TextUtils::equals(st->buff, "//", 2)) {
            // end of entry
            return true;
        }
        if (st->hasKey("ORIGIN")) {
            if (st->hasValue()) {
                st->entry->tags.insert(DNAInfo::ORIGIN, st->value());
            }
            if (st->readNextLine() && st->hasKey("CONTIG")) {
                QStringList s(st->value());
                while (st->readNextLine() && st->hasContinuation()) {
                    s.append(st->value());
                }
                st->entry->tags.insert(DNAInfo::CONTIG, s);
                if (!TextUtils::equals(st->buff, "//", 2)) {
                    break;
                }
            } else {
                if (st->len > 0) {
                    st->io->skip(-st->len - 1);
                }
                if (merge && gapSize) {
                    seqImporter.addDefaultSymbolsBlock(gapSize, os);
                    CHECK_OP(os, false);
                }
                readSequence(st, seqImporter, sequenceLen, fullSequenceLen, os);
                if (fullSequenceLen != st->entry->seqLen && !si.getWarnings().contains(EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE)) {
                    si.addWarning(EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE);
                }
            }
            return true;
        }

        if (st->hasContinuation()) {
            QVariant v = st->entry->tags.take(lastTagName);
            v = QVariantUtils::addStr2List(v, st->value());
            st->entry->tags.insert(lastTagName, v);
        } else if (st->hasValue()) {
            lastTagName = st->key().trimmed();
            st->entry->tags.insert(lastTagName, st->value());
        }
    }
    if (!st->isNull() && !si.isCoR()) {
        si.setError(U2::EMBLGenbankAbstractDocument::tr("Record is truncated."));
    }

    return false;
}

void GenbankPlainTextFormat::readHeaderAttributes(QVariantMap& tags, DbiConnection& con, U2SequenceObject* so) {
    QString headerAttrString;

    QList<StrPair> lst(formatKeywords(tags, true));
    foreach (const StrPair& p, lst) {
        int klen = p.first.length();
        assert(klen < VAL_OFF);
        headerAttrString.append(p.first);
        for (int i = 0; i < VAL_OFF - klen; i++) {
            headerAttrString.append(" ");
        }
        headerAttrString.append(p.second + "\n");
    }

    U2StringAttribute headerAttr(so->getSequenceRef().entityId, DNAInfo::GENBANK_HEADER, headerAttrString);
    U2OpStatus2Log os;
    con.dbi->getAttributeDbi()->createStringAttribute(headerAttr, os);
    CHECK_OP(os, );

    if (tags.keys().contains(DNAInfo::SOURCE)) {
        DNASourceInfo soi = tags.value(DNAInfo::SOURCE).value<DNASourceInfo>();
        if (!soi.name.isEmpty()) {
            U2StringAttribute sourceAttr(so->getSequenceRef().entityId, DNAInfo::SOURCE, soi.name);
            con.dbi->getAttributeDbi()->createStringAttribute(sourceAttr, os);
            CHECK_OP(os, );
        }
    }

    if (tags.keys().contains(DNAInfo::ACCESSION)) {
        QString acc;
        if (tags.value(DNAInfo::ACCESSION).canConvert<QString>()) {
            acc = tags.value(DNAInfo::ACCESSION).toString();
        } else if (tags.value(DNAInfo::ACCESSION).canConvert<QStringList>()) {
            acc = tags.value(DNAInfo::ACCESSION).toStringList().join(", ");
        }
        if (!acc.isEmpty()) {
            U2StringAttribute accAttr(so->getSequenceRef().entityId, DNAInfo::ACCESSION, acc);
            con.dbi->getAttributeDbi()->createStringAttribute(accAttr, os);
            CHECK_OP(os, );
        }
    }

    if (tags.keys().contains(DNAInfo::COMMENT)) {
        const QStringList comments = tags.value(DNAInfo::COMMENT).toStringList();
        foreach (const QString& comment, comments) {
            U2StringAttribute commentAttr(so->getSequenceRef().entityId, DNAInfo::COMMENT, comment);
            con.dbi->getAttributeDbi()->createStringAttribute(commentAttr, os);
            CHECK_OP(os, );
        }
    }

    tags.insert(UGENE_MARK, "");  // to allow writing
}

bool GenbankPlainTextFormat::isNcbiLikeFormat() const {
    return true;
}

void GenbankPlainTextFormat::createCommentAnnotation(const QStringList& comments, int sequenceLength, AnnotationTableObject* annTable) const {
    CHECK(!comments.isEmpty(), );
    SharedAnnotationData f(new AnnotationData);
    f->type = U2FeatureTypes::Comment;
    f->name = "comment";
    f->location->regions.append(U2Region(0, sequenceLength));
    for (int i = 0, digitsCount = QString::number(comments.size()).size(); i < comments.size(); ++i) {
        f->qualifiers.append(U2Qualifier(QString("%1").arg(i + 1, digitsCount, 10, QChar('0')), comments[i]));
    }

    annTable->addAnnotations(QList<SharedAnnotationData>() << f, "comment");
}

U2FeatureType GenbankPlainTextFormat::getFeatureType(const QString& typeString) const {
    const GBFeatureKey gbKey = GBFeatureUtils::getKey(typeString);
    CHECK(gbKey != GBFeatureKey_UNKNOWN, U2FeatureTypes::MiscFeature);
    return GBFeatureUtils::getKeyInfo(gbKey).type;
}

QString GenbankPlainTextFormat::getFeatureTypeString(U2FeatureType featureType, bool /*isAmino*/) {
    const GBFeatureKey gbKey = additionalFeatureTypes.value(featureType, GBFeatureUtils::getKey(featureType));
    return gbKey == GBFeatureKey_UNKNOWN ? GBFeatureUtils::DEFAULT_KEY : GBFeatureUtils::getKeyInfo(gbKey).text;
}

bool GenbankPlainTextFormat::breakQualifierOnSpaceOnly(const QString& qualifierName) const {
    QRegExp spacelessQualifierCatcher = QRegExp("^/?(" +
                                                GBFeatureUtils::QUALIFIER_TRANSLATION +
                                                "|" +
                                                GBFeatureUtils::QUALIFIER_NAME +
                                                "|" +
                                                GBFeatureUtils::QUALIFIER_GROUP +
                                                ")");
    return -1 == spacelessQualifierCatcher.indexIn(qualifierName);
}

const QMap<U2FeatureType, GBFeatureKey> GenbankPlainTextFormat::additionalFeatureTypes = GenbankPlainTextFormat::initAdditionalFeatureTypes();

QMap<U2FeatureType, GBFeatureKey> GenbankPlainTextFormat::initAdditionalFeatureTypes() {
    QMap<U2FeatureType, GBFeatureKey> result;
    result.insert(U2FeatureTypes::PromoterEukaryotic, GBFeatureKey_promoter);
    result.insert(U2FeatureTypes::PromoterProkaryotic, GBFeatureKey_promoter);
    return result;
}

//////////////////////////////////////////////////////////////////////////
/// saving

bool GenbankPlainTextFormat::writeKeyword(IOAdapter* io, U2OpStatus& os, const QString& key, const QString& value) {
    try {
        assert(key.length() < VAL_OFF);
        int klen = qMin(VAL_OFF - 1, key.length());
        qint64 len = io->writeBlock(key.left(klen).toLocal8Bit());
        if (len != klen) {
            throw 0;
        }
        static char spaces[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
        int slen = VAL_OFF - klen;
        len = io->writeBlock(spaces, slen);
        if (len != slen) {
            throw 0;
        }

        len = io->writeBlock(value.toLocal8Bit());
        if (len != value.size()) {
            throw 0;
        }

        static char eol[] = {'\n'};
        if (!io->writeBlock(eol, 1)) {
            throw 0;
        }

    } catch (int) {
        os.setError(GenbankPlainTextFormat::tr("Error writing document"));
        return false;
    }
    return true;
}

void GenbankPlainTextFormat::storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os) {
    SAFE_POINT(doc != nullptr, "GenbankPlainTextFormat::storeDocument::no document", );
    QList<GObject*> seqs = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    QList<GObject*> anns = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);

    while (!seqs.isEmpty() || !anns.isEmpty()) {
        auto so = seqs.isEmpty() ? nullptr : qobject_cast<U2SequenceObject*>(seqs.takeFirst());
        QList<GObject*> aos;
        if (so) {
            if (!anns.isEmpty()) {
                aos = GObjectUtils::findObjectsRelatedToObjectByRole(so,
                                                                     GObjectTypes::ANNOTATION_TABLE,
                                                                     ObjectRole_Sequence,
                                                                     anns,
                                                                     UOF_LoadedOnly);
                foreach (GObject* o, aos) {
                    anns.removeAll(o);
                }
            }
        } else {
            SAFE_POINT(!anns.isEmpty(), "GenbankPlainTextFormat::storeDocument::anns.isEmpty()", );
            aos << anns.takeFirst();
        }

        QMap<GObjectType, QList<GObject*>> objectsMap;
        {
            if (so != nullptr) {
                objectsMap[GObjectTypes::SEQUENCE] = {so};
            }
            if (!aos.isEmpty()) {
                objectsMap[GObjectTypes::ANNOTATION_TABLE] = aos;
            }
        }
        storeEntry(io, objectsMap, os);
        CHECK_OP(os, );
    }
}

void GenbankPlainTextFormat::storeEntry(IOAdapter* io, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    U2SequenceObject* seq = nullptr;
    QList<GObject*> anns;
    if (objectsMap.contains(GObjectTypes::SEQUENCE)) {
        const QList<GObject*>& seqs = objectsMap[GObjectTypes::SEQUENCE];
        SAFE_POINT(1 >= seqs.size(), "Genbank entry storing: sequence objects count error", );
        if (1 == seqs.size()) {
            seq = dynamic_cast<U2SequenceObject*>(seqs.first());
            SAFE_POINT(seq != nullptr, "Genbank entry storing: NULL sequence object", );
        }
    }
    if (objectsMap.contains(GObjectTypes::ANNOTATION_TABLE)) {
        anns = objectsMap[GObjectTypes::ANNOTATION_TABLE];
    }
    SAFE_POINT(seq != nullptr || !anns.isEmpty(), "Store entry: nothing to write", );

    // reading header attribute
    QString locusFromAttributes;
    QString gbHeader;
    if (seq) {
        DbiConnection con(seq->getSequenceRef().dbiRef, os);
        CHECK_OP(os, );
        U2AttributeDbi* attributeDbi = con.dbi->getAttributeDbi();
        U2StringAttribute attr = U2AttributeUtils::findStringAttribute(attributeDbi, seq->getSequenceRef().entityId, DNAInfo::GENBANK_HEADER, os);
        if (attr.hasValidId()) {
            gbHeader = attr.value;
        }

        if (gbHeader.startsWith("LOCUS")) {  // trim the first line
            int locusStringEndIndex = gbHeader.indexOf("\n");
            assert(locusStringEndIndex != -1);
            locusFromAttributes = gbHeader.left(locusStringEndIndex);
            gbHeader = gbHeader.mid(locusStringEndIndex + 1);
        }
    }
    // write mandatory locus string
    QString locusString = genLocusString(anns, seq, locusFromAttributes);
    if (!writeKeyword(io, os, DNAInfo::LOCUS, locusString)) {
        return;
    }
    // write other keywords
    if (seq) {
        // header
        io->writeBlock(gbHeader.toLocal8Bit());
    }

    // write tool mark
    QList<GObject*> annsAndSeqObjs;
    annsAndSeqObjs << anns;
    if (seq != nullptr) {
        annsAndSeqObjs << seq;
    }
    if (!annsAndSeqObjs.isEmpty()) {
        QString unimark = annsAndSeqObjs[0]->getGObjectName();
        if (!writeKeyword(io, os, UGENE_MARK, unimark)) {
            return;
        }
        for (int x = 1; x < annsAndSeqObjs.size(); x++) {
            if (!writeKeyword(io, os, QString(), annsAndSeqObjs[x]->getGObjectName())) {
                return;
            }
        }
    }

    // write annotations
    if (!anns.isEmpty()) {
        writeAnnotations(io, anns, os);
        CHECK_OP(os, );
    }

    if (seq) {
        // todo: store sequence alphabet!
        QList<U2Region> lowerCaseRegs = U1AnnotationUtils::getRelatedLowerCaseRegions(seq, anns);
        writeSequence(io, seq, lowerCaseRegs, os);
        CHECK_OP(os, );
    }

    // write last line marker
    QByteArray lastLine("//\n");
    qint64 len = io->writeBlock(lastLine);
    if (len != lastLine.size()) {
        os.setError(L10N::errorWritingFile(io->getURL()));
        return;
    }
}

bool GenbankPlainTextFormat::checkCircularity(const GUrl& filePath, U2OpStatus& os) {
    SAFE_POINT_EXT(AppContext::getIOAdapterRegistry() != nullptr, os.setError("There is no IOAdapter registry yet"), false);
    IOAdapterFactory* factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(factory != nullptr, os.setError("IOAdapterFactory is NULL"), false);
    IOAdapter* io = factory->createIOAdapter();
    SAFE_POINT_EXT(io != nullptr, os.setError("IOAdapter is NULL"), false);
    io->setFormatMode(IOAdapter::TextMode);
    bool ok = io->open(filePath, IOAdapterMode_Read);
    CHECK_EXT(ok, os.setError(L10N::errorOpeningFileRead(filePath)), false);

    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    ParserState st(12, io, nullptr, os);
    st.buff = readBuffer.data();
    EMBLGenbankDataEntry data;
    st.entry = &data;
    st.readNextLine(true);  // all information is in the first line!

    if (readIdLine(&st)) {
        return st.entry->circular;
    }
    CHECK_OP(os, false);
    return false;
}

static QString getDate() {
    const char* MonthsInEng[] = {" ", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
    return QDate::currentDate().toString("dd-") +
           MonthsInEng[QDate::currentDate().month()] +
           QDate::currentDate().toString("-yyyy");
}

static QString padToLen(const QString& s, int width) {
    if (width > s.length()) {
        return s.leftJustified(width);
    } else {
        return s + " ";
    }
}

static QString getLocusTopologyTag(U2SequenceObject* so) {
    SAFE_POINT_NN(so, EMBLGenbankAbstractDocument::LOCUS_TAG_LINEAR);
    return so->isCircular()
               ? EMBLGenbankAbstractDocument::LOCUS_TAG_CIRCULAR
               : EMBLGenbankAbstractDocument::LOCUS_TAG_LINEAR;
}

static QString getLocusMoleculeTypeFromAlphabet(const U2SequenceObject* so) {
    SAFE_POINT_NN(so, "NA");
    auto alphabet = so->getAlphabet();
    return alphabet->isRNA()
               ? "RNA"
           : alphabet->isDNA()
               ? "DNA"
               : "NA";
}

QString GenbankPlainTextFormat::genLocusString(const QList<GObject*>& aos, U2SequenceObject* so, const QString& locusStrFromAttr) {
    // See https://ftp.ncbi.nlm.nih.gov/genbank/gbrel.txt.
    if (so == nullptr) {
        SAFE_POINT(!aos.isEmpty(), "Annotation object list is empty", "");
        QString locus = !aos.isEmpty() ? aos.first()->getGObjectName() : "unknown";
        locus = padToLen(locus, 56) + getDate();
        return locus;
    }
    QString molecule;
    QString topology = getLocusTopologyTag(so);
    QString division;
    QString date;
    if (so->getSequenceInfo().contains(DNAInfo::LOCUS)) {
        DNALocusInfo locusInfo = so->getSequenceInfo().value(DNAInfo::LOCUS).value<DNALocusInfo>();
        molecule = locusInfo.molecule;
        division = locusInfo.division;
        date = locusInfo.date;
    } else if (!locusStrFromAttr.isEmpty()) {
        QStringList tokens = locusStrFromAttr.split(" ", Qt::SkipEmptyParts);
        SAFE_POINT(tokens.size() >= 5, QString("Incorrect number of tokens for attribute %1").arg(locusStrFromAttr), "");
        molecule = tokens[2];
        division = tokens[3];
    }
    // Name.
    QString name = so->getSequenceName();
    if (name.isEmpty()) {
        name = so->getGObjectName();
    }
    QString locus = name;

    // Length.
    QString length = QString::number(so->getSequenceLength());
    locus = padToLen(locus.replace(' ', '_'), qMax(0, 28 - length.length()));
    locus.append(length).append(" bp ");
    locus = padToLen(locus, 35);

    // Molecule.
    if (molecule.length() == 0) {
        molecule = getLocusMoleculeTypeFromAlphabet(so);
    }
    locus = padToLen(locus.append(molecule), 43);

    // Topology.
    locus = padToLen(locus.append(topology), 52);

    // Division.
    locus = locus.append(division);

    // Date.
    if (date.isEmpty()) {
        date = getDate();
    }
    locus = padToLen(locus, 56) + date;
    return locus;
}

void GenbankPlainTextFormat::writeQualifier(const QString& name, const QString& val, IOAdapter* io, U2OpStatus& si, const char* spaceLine) {
    int len = (int)io->writeBlock(spaceLine, 21);
    if (len != 21) {
        si.setError(GenbankPlainTextFormat::tr("Error writing document"));
        return;
    }

    QString qstr = prepareQualifierSingleString(name, val);
    CHECK_EXT(!qstr.isEmpty(), si.setError(tr("Error writing document")), );

    prepareMultiline(qstr, 21, breakQualifierOnSpaceOnly(name));
    const QByteArray dataToWrite = qstr.toLocal8Bit();
    len = (int)io->writeBlock(dataToWrite);
    if (len != dataToWrite.length()) {
        si.setError(GenbankPlainTextFormat::tr("Error writing document"));
    }
}

QString GenbankPlainTextFormat::prepareQualifierSingleString(const QString& qualifierName, const QString& qualifierValue) const {
    bool isNum = false;
    qualifierValue.toInt(&isNum);
    if (isNum) {
        return "/" + qualifierName + "=" + qualifierValue;
    }
    if (GBFeatureUtils::isFeatureHasNoValue(qualifierName)) {
        return "/" + qualifierName;
    }

    QString preparedValue = qualifierValue;
    preparedValue.replace("\"", "\"\"");

    if (!breakQualifierOnSpaceOnly(qualifierName)) {
        preparedValue.replace(" ", "\\ ");
    }

    return "/" + qualifierName + "=\"" + preparedValue + "\"";
}

QList<GenbankPlainTextFormat::StrPair> GenbankPlainTextFormat::formatKeywords(const QVariantMap& varMap, bool withLocus) {
    QList<StrPair> res;

    QMultiMap<QString, QVariant> tags(varMap);

    if (!withLocus) {
        tags.remove(DNAInfo::LOCUS);
    }
    tags.remove(DNAInfo::ID);
    tags.remove(DNAInfo::CONTIG);
    tags.remove(DNAInfo::ORIGIN);
    tags.remove(EMBLGenbankAbstractDocument::UGENE_MARK);
    if (tags.contains(DNAInfo::ACCESSION)) {
        QString accessionString = tags.take(DNAInfo::ACCESSION).toStringList().join(" ");
        prepareMultiline(accessionString, VAL_OFF, true, false);
        tags.insert(DNAInfo::ACCESSION, accessionString);
    }

    {
        QString key = DNAInfo::LOCUS;
        while (tags.contains(key)) {
            QVariant v = tags.take(key);
            DNALocusInfo li = v.value<DNALocusInfo>();
            QString locusVal = li.name + " " + li.molecule + " " + li.division + " " + li.topology + " " + li.date;
            res << qMakePair(key, locusVal);
        }
    }

    QStringList order;
    order << DNAInfo::DEFINITION << DNAInfo::ACCESSION << DNAInfo::VERSION;
    order << DNAInfo::PROJECT << DNAInfo::KEYWORDS << DNAInfo::SEGMENT;
    foreach (const QString& key, order) {
        while (tags.contains(key)) {
            QVariant v = tags.take(key);
            if (v.canConvert(QVariant::String)) {
                res << qMakePair(key, v.toString());
            } else if (v.canConvert(QVariant::StringList)) {
                QStringList l = v.toStringList();
                if (l.isEmpty()) {
                    continue;
                }

                res << qMakePair(key, l.takeFirst());
                for (const QString& s : qAsConst(l)) {
                    res << qMakePair(QString(), s);
                }
            } else {
                assert(0);
            }
        }
    }
    {
        QString key = DNAInfo::SOURCE;
        while (tags.contains(key)) {
            QVariant v = tags.take(key);
            DNASourceInfo soi = v.value<DNASourceInfo>();
            res << qMakePair(key, soi.name);
            if (!soi.organelle.isEmpty()) {
                res[res.size() - 1].second += " " + soi.organelle;
            }
            res << qMakePair(QString("  ORGANISM"), soi.organism);
            foreach (const QString& s, soi.taxonomy) {
                res << qMakePair(QString(), s);
            }
        }
    }
    {
        QString key = DNAInfo::REFERENCE;
        while (tags.contains(key)) {
            QVariant v = tags.take(key);
            DNAReferenceInfo ri = v.value<DNAReferenceInfo>();
            res << qMakePair(key, ri.referencesRecord);
        }
    }

    res << processCommentKeys(tags);

    QMapIterator<QString, QVariant> it(tags);
    while (it.hasNext()) {
        it.next();
        if (it.value().type() == QVariant::String) {
            res << qMakePair(it.key(), it.value().toString());
        } else if (it.value().type() == QVariant::StringList) {
            QStringList l = it.value().toStringList();
            if (l.isEmpty()) {
                continue;
            }
            res << qMakePair(it.key(), l.takeFirst());
            foreach (const QString& s, l) {
                res << qMakePair(QString(), s);
            }
        } else {
            assert(0);
        }
    }
    return res;
}

QList<GenbankPlainTextFormat::StrPair> GenbankPlainTextFormat::processCommentKeys(QMultiMap<QString, QVariant>& tags) {
    QList<StrPair> res;
    while (tags.contains(DNAInfo::COMMENT)) {
        const QVariant v = tags.take(DNAInfo::COMMENT);
        CHECK_EXT(v.canConvert<QStringList>(), coreLog.info("Unexpected Genbank COMMENT section"), res);
        const QStringList comments = v.value<QStringList>();
        foreach (QString comment, comments) {
            res << qMakePair(DNAInfo::COMMENT, comment.replace("\n", "\n" + QString(VAL_OFF, ' ')));
        }
    }
    return res;
}

void GenbankPlainTextFormat::writeAnnotations(IOAdapter* io, const QList<GObject*>& aos, U2OpStatus& si) {
    assert(!aos.isEmpty());
    QByteArray header("FEATURES             Location/Qualifiers\n");

    // write "FEATURES"
    qint64 len = io->writeBlock(header);
    if (len != header.size()) {
        si.setError(GenbankPlainTextFormat::tr("Error writing document"));
        return;
    }

    // write every feature
    const char* spaceLine = TextUtils::SPACE_LINE.data();
    const QByteArray& nameQ = GBFeatureUtils::QUALIFIER_NAME;
    const QByteArray& groupQ = GBFeatureUtils::QUALIFIER_GROUP;

    QList<Annotation*> sortedAnnotations;
    foreach (GObject* o, aos) {
        auto ao = qobject_cast<AnnotationTableObject*>(o);
        CHECK_EXT(ao != nullptr, si.setError(tr("Invalid annotation table!")), );
        sortedAnnotations += ao->getAnnotations();
    }

    std::stable_sort(sortedAnnotations.begin(), sortedAnnotations.end(), Annotation::annotationLessThanByRegion);

    for (auto a : qAsConst(sortedAnnotations)) {
        QString aName = a->getName();

        if (aName == U1AnnotationUtils::lowerCaseAnnotationName || aName == U1AnnotationUtils::upperCaseAnnotationName || aName == "comment") {
            continue;
        }

        // write name of the feature
        len = io->writeBlock(spaceLine, 5);
        if (len != 5) {
            si.setError(GenbankPlainTextFormat::tr("Error writing document"));
            return;
        }

        const QString keyStr = getFeatureTypeString(a->getType(), false);
        len = io->writeBlock(keyStr.toLocal8Bit());
        if (len != keyStr.length()) {
            si.setError(GenbankPlainTextFormat::tr("Error writing document"));
            return;
        }
        int nspaces = 22 - keyStr.length() - 6;
        assert(nspaces > 0);
        len = io->writeBlock(spaceLine, nspaces);
        if (len != nspaces) {
            si.setError(GenbankPlainTextFormat::tr("Error writing document"));
            return;
        }

        // write location
        const SharedAnnotationData& ad = a->getData();
        QString multiLineLocation = U1AnnotationUtils::buildLocationString(ad);
        prepareMultiline(multiLineLocation, 21);
        len = io->writeBlock(multiLineLocation.toLocal8Bit());
        if (len != multiLineLocation.size()) {
            si.setError(GenbankPlainTextFormat::tr("Error writing document"));
            return;
        }

        // write qualifiers
        foreach (const U2Qualifier& q, a->getQualifiers()) {
            writeQualifier(q.name, q.value, io, si, spaceLine);
            if (si.hasError()) {
                return;
            }
        }

        // write name if its not the same as a name
        if (aName != keyStr) {
            writeQualifier(nameQ, aName, io, si, spaceLine);
        }

        // write strand info
        // if (a->getAminoFrame() != TriState_Unknown) {
        //     const QString& val = a->getAminoFrame() == TriState_No ? aminoQNo : aminoQYes;
        //     writeQualifier(aminoQ, val, io, si, spaceLine);
        // }

        // write group
        AnnotationGroup* ag = a->getGroup();
        const bool storeGroups = !ag->isTopLevelGroup() || ag->getName() != aName;

        if (storeGroups) {
            writeQualifier(groupQ, ag->getGroupPath(), io, si, spaceLine);
        }
    }
}

void GenbankPlainTextFormat::writeSequence(IOAdapter* io, U2SequenceObject* ao, const QList<U2Region>& lowerCaseRegs, U2OpStatus& si) {
    static const int charsInLine = 60;
    static const int DB_BLOCK_SIZE = charsInLine * 3000;

    QByteArray seq;
    qint64 slen = ao->getSequenceLength();
    const char* sequence = nullptr;
    const char* spaces = TextUtils::SPACE_LINE.constData();
    QByteArray num;
    bool ok = true;
    qint64 blen = io->writeBlock(QByteArray("ORIGIN\n"));
    if (blen != 7) {
        si.setError(L10N::errorWritingFile(ao->getDocument()->getURL()));
        return;
    }

    for (qint64 pos = 0; pos < slen; pos += charsInLine) {
        if ((pos % DB_BLOCK_SIZE) == 0) {
            seq.clear();
            seq = ao->getSequenceData(U2Region(pos, qMin((qint64)DB_BLOCK_SIZE, slen - pos)));
            sequence = U1AnnotationUtils::applyLowerCaseRegions(seq.data(), 0, seq.size(), pos, lowerCaseRegs);
        }
        num.setNum(pos + 1);

        // right spaces
        blen = 10 - num.length() - 1;
        qint64 l = io->writeBlock(QByteArray::fromRawData(spaces, blen));
        if (l != blen) {
            ok = false;
            break;
        }

        // current pos
        l = io->writeBlock(num);
        if (l != num.length()) {
            ok = false;
            break;
        }

        // sequence
        qint64 last = qMin(pos + charsInLine, slen);
        for (qint64 j = pos; j < last; j += 10) {
            l = io->writeBlock(QByteArray::fromRawData(" ", 1));
            if (l != 1) {
                ok = false;
                break;
            }
            qint64 chunkLen = qMin((qint64)10, slen - j);
            l = io->writeBlock(QByteArray::fromRawData(sequence + (j % DB_BLOCK_SIZE), chunkLen));
            if (l != chunkLen) {
                ok = false;
                break;
            }
        }
        if (!ok) {
            break;
        }

        // line end
        l = io->writeBlock(QByteArray("\n", 1));
        if (l != 1) {
            ok = false;
            break;
        }
    }
    if (!ok) {
        si.setError(L10N::errorWritingFile(ao->getDocument()->getURL()));
    }
}

// splits line into multiple lines adding 'spacesOnLineStart' to every line except first one
// and '\n' to the end of every new line
void GenbankPlainTextFormat::prepareMultiline(QString& line, int spacesOnLineStart, bool lineBreakOnlyOnSpace, bool newLineAtTheEnd, int maxLineLen) {
    Q_ASSERT(spacesOnLineStart < maxLineLen);
    line.replace('\n', ';');
    const int len = line.length();
    if (spacesOnLineStart + len > maxLineLen) {
        QByteArray spacesPrefix(spacesOnLineStart, ' ');
        QString newLine;
        int charsInLine = maxLineLen - spacesOnLineStart;
        int pos = 0;
        bool skipLineBreak = false;
        do {
            if (pos != 0 && !skipLineBreak) {
                newLine.append('\n');
                newLine.append(spacesPrefix);
            }
            skipLineBreak = false;
            int pos2 = pos + charsInLine - 1;
            if (pos2 < len) {  // not the last line
                while (pos2 > pos && !line[pos2].isSpace() && lineBreakOnlyOnSpace) {
                    pos2--;
                }
                if (pos == pos2) {  // we failed to find word end
                    pos2 = pos + charsInLine - 1;
                    if (lineBreakOnlyOnSpace) {
                        skipLineBreak = true;
                    }
                }
                newLine.append(line.mid(pos, pos2 - pos + (!lineBreakOnlyOnSpace || skipLineBreak)));
            } else {  // last line
                newLine.append(line.mid(pos, len - pos));
            }
            pos = pos2 + 1;
        } while (pos < len);
        line = newLine;
    }
    if (newLineAtTheEnd) {
        line += "\n";
    }
}

}  // namespace U2
