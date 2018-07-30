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

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAInfo.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "DocumentFormatUtils.h"
#include "GenbankLocationParser.h"
#include "SwissProtPlainTextFormat.h"

namespace U2 {

/* TRANSLATOR U2::EMBLPlainTextFormat */
/* TRANSLATOR U2::EMBLGenbankAbstractDocument */

SwissProtPlainTextFormat::SwissProtPlainTextFormat(QObject* p)
: EMBLGenbankAbstractDocument(BaseDocumentFormats::PLAIN_SWISS_PROT, tr("Swiss-Prot"), 80, DocumentFormatFlag_SupportStreaming, p)
{
    formatDescription = tr("SwissProt is a format of the UniProtKB/Swiss-prot database used for "
            "storing annotated protein sequence");
    fileExtensions << "sw" << "em" << "emb" << "embl" << "txt";
    sequenceStartPrefix = "SQ";
    fPrefix = "FT";

    tagMap["DT"] = DNAInfo::DATE; //The DT (DaTe) lines shows the date of creation and last modification of the database entry.
    tagMap["DE"] = DNAInfo::DEFINITION; //The DE (DEscription) lines contain general descriptive information about the sequence stored.
    tagMap["KW"] = DNAInfo::KEYWORDS; //The KW (KeyWord) lines provide information that can be used to generate indexes of the sequence entries based on functional, structural, or other categories.
    tagMap["CC"] = DNAInfo::COMMENT; //The CC lines are free text comments on the entry, and are used to convey any useful information.
}

FormatCheckResult SwissProtPlainTextFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    //TODO: improve format checking

    const char* data = rawData.constData();
    int size = rawData.size();

    bool textOnly = !TextUtils::contains(TextUtils::BINARY, data, size);
    if (!textOnly || size < 100) {
        return FormatDetection_NotMatched;
    }
    bool tokenMatched = TextUtils::equals("ID   ", data, 5);
    if (tokenMatched) {
        if(QString(rawData).contains(QRegExp("\\d+ AA."))){
            return FormatDetection_HighSimilarity;
        }
        return FormatDetection_NotMatched;
    }
    return FormatDetection_NotMatched;
}

//////////////////////////////////////////////////////////////////////////
// loading

bool SwissProtPlainTextFormat::readIdLine(ParserState* s) {
    if (!s->hasKey("ID", 2)) {
        s->si.setError(SwissProtPlainTextFormat::tr("ID is not the first line"));
        return false;
    }

    QString idLineStr= s->value();
    QStringList tokens = idLineStr.split(" ", QString::SkipEmptyParts);
    if (idLineStr.length() < 4 || tokens.isEmpty()) {
        s->si.setError(SwissProtPlainTextFormat::tr("Error parsing ID line"));
        return false;
    }
    s->entry->name = tokens[0];
    DNALocusInfo loi;
    loi.name = tokens[0];
    QString third = tokens[2];
    bool ok=false;
    s->entry->seqLen=third.toInt(&ok);
    if (!ok) {
        s->si.setError(SwissProtPlainTextFormat::tr("Error parsing ID line. Not found sequence length"));
        return false;
    }
    s->entry->tags.insert(DNAInfo::LOCUS, qVariantFromValue<DNALocusInfo>(loi));

    return true;
}

bool SwissProtPlainTextFormat::readEntry(ParserState* st, U2SequenceImporter& seqImporter, int& sequenceLen,int& fullSequenceLen, bool merge, int gapSize, U2OpStatus& os) {
    Q_UNUSED(merge);
    Q_UNUSED(gapSize);
    U2OpStatus& si = st->si;
    QString lastTagName;
    bool hasLine = false;
    while (hasLine || st->readNextLine(false)) {
        hasLine = false;
        if (st->entry->name.isEmpty()) {
            readIdLine(st);
            assert(si.hasError() || !st->entry->name.isEmpty());
            if (si.hasError()) {
                break;
            }
            continue;
        }
        //
        if (st->hasKey("FH") || st->hasKey("AH")) {
            continue;
        }
        if (st->hasKey("AC")) { //The AC (ACcession number) line lists the accession number(s) associated with an entry.
            QVariant v = st->entry->tags.value(DNAInfo::ACCESSION);
            QStringList l = st->value().split(QRegExp(";\\s*"), QString::SkipEmptyParts);
            st->entry->tags[DNAInfo::ACCESSION] = QVariantUtils::addStr2List(v, l);
            continue;
        }
        if (st->hasKey("OS")) { //The OS (Organism Species) line specifies the organism(s) which was (were) the source of the stored sequence.
            DNASourceInfo soi;
            soi.name = st->value();
            soi.organism = soi.name;
            while (st->readNextLine()) {
                if (st->hasKey("OS")) {
                    soi.organism.append(" ").append(st->value());
                } else if (!st->hasKey("XX")) {
                    break;
                }
            }
            if (st->hasKey("OC")) { //The OC (Organism Classification) lines contain the taxonomic classification of the source organism.
                soi.taxonomy += st->value();
                while (st->readNextLine()) {
                    if (st->hasKey("OC")) {
                        soi.taxonomy.append(st->value());
                    } else if (!st->hasKey("XX")) {
                        break;
                    }
                }
            }
            if (st->hasKey("OG")) { //The OG (OrGanelle) line indicates if the gene coding for a protein originates from the mitochondria, the chloroplast, a cyanelle, or a plasmid.
                soi.organelle = st->value();
            } else {
                hasLine = true;
            }
            st->entry->tags.insertMulti(DNAInfo::SOURCE, qVariantFromValue<DNASourceInfo>(soi));
            continue;
        }
        if (st->hasKey("RF") || st->hasKey("RN")) { //The RN (Reference Number) line gives a sequential number to each reference citation in an entry.
            while (st->readNextLine() && st->buff[0] == 'R')
            {
                //TODO
            }
            hasLine = true;
            continue;
        }
        /*The FT (Feature Table) lines provide a precise but simple means for the annotation of the sequence data.
          The table describes regions or sites of interest in the sequence.
          In general the feature table lists posttranslational modifications, binding sites, enzyme active sites, local secondary structure or other characteristics reported in the cited references.
        */
        if (st->hasKey("FT", 2)) {
            readAnnotations(st, fullSequenceLen + gapSize);
            hasLine = true;
            continue;
        }
        //read simple tag;
        if (st->hasKey("//", 2)) {
            // end of entry
            return true;
        }
        else if (st->hasKey("SQ", 2)) {
            //reading sequence
            readSequence(st,seqImporter,sequenceLen,fullSequenceLen,os);
            if (fullSequenceLen != st->entry->seqLen && !si.getWarnings().contains(EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE)) {
                si.addWarning(EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE);
            }
            CHECK_OP(os,false);
            return true;
        }

        QString key = st->key().trimmed();
        if (tagMap.contains(key)) {
            key = tagMap.value(key);
        }
        if (lastTagName == key) {
            QVariant v = st->entry->tags.take(lastTagName);
            v = QVariantUtils::addStr2List(v, st->value());
            st->entry->tags.insert(lastTagName, v);
        } else if (st->hasValue()) {
            lastTagName = key;
            st->entry->tags.insertMulti(lastTagName, st->value());
        }
    }
    if (!st->isNull() && !si.isCoR()) {
        si.setError(U2::EMBLGenbankAbstractDocument::tr("Record is truncated."));
    }

    return false;
}
bool SwissProtPlainTextFormat::readSequence(ParserState *st, U2SequenceImporter& seqImporter, int& sequenceLen, int& fullSequenceLen, U2OpStatus& os){
    QByteArray res;

    IOAdapter* io = st->io;
    U2OpStatus& si = st->si;
    si.setDescription(tr("Reading sequence %1").arg(st->entry->name));
    int headerSeqLen = st->entry->seqLen;
    res.reserve(res.size() + headerSeqLen);

    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff  = readBuffer.data();

    //reading sequence
    QBuffer writer(&res);
    writer.open( QIODevice::WriteOnly);
    bool ok = true;
    int len;
    while (ok && (len = io->readLine(buff, READ_BUFF_SIZE)) > 0) {
        if (si.isCoR()) {
            res.clear();
            break;
        }

        if (len <= 0)  {
            si.setError(tr("Error parsing sequence: unexpected empty line"));
            break;
        }

        if (buff[0] == '/') { //end of the sequence
            break;
        }

        bool isSeek = writer.seek(0);
        assert(isSeek); Q_UNUSED(isSeek);

        //add buffer to result
        for (int i= 0; i < len; i++) {
            char c = buff[i];
            if (c != ' ' && c != '\t') {
                ok = writer.putChar(c);
                if (!ok) {
                    break;
                }
            }
        }

        if (!ok) {
            si.setError(tr("Error reading sequence: memory allocation failed"));
            break;
        }

        seqImporter.addBlock(res,res.size(),os);
        if(os.isCoR()){
            break;
        }
        sequenceLen += res.size();
        fullSequenceLen += res.size();
        res.clear();

        si.setProgress(io->getProgress());
    }
    if (!si.isCoR() && buff[0] != '/') {
        si.setError(tr("Sequence is truncated"));
    }
    writer.close();
    return true;
}

void SwissProtPlainTextFormat::readAnnotations(ParserState *st, int offset){
    st->si.setDescription(tr("Reading annotations %1").arg(st->entry->name));
    st->entry->hasAnnotationObjectFlag = true;
    do {
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
        SharedAnnotationData f = readAnnotation(st->io, st->buff, st->len, READ_BUFF_SIZE, st->si, offset);
        st->entry->features.push_back(f);
    } while (st->readNextLine());
}
//column annotation data starts with
#define A_COL 34
//column qualifier name starts with
#define QN_COL 35
//column annotation key starts with
#define K_COL 5

SharedAnnotationData SwissProtPlainTextFormat::readAnnotation(IOAdapter* io, char* cbuff, int len, int READ_BUFF_SIZE, U2OpStatus& si, int offset, int seqLen){

    Q_UNUSED(seqLen);
    AnnotationData* a = new AnnotationData();
    SharedAnnotationData f(a);
    QString key = QString::fromLatin1(cbuff+5, 10).trimmed();
    if (key.isEmpty()) {
        si.setError(EMBLGenbankAbstractDocument::tr("Annotation name is empty"));
        return SharedAnnotationData();
    }
    a->name = key;
    if(key == "STRAND" || key == "HELIX" || key == "TURN"){
        a->qualifiers.append(U2Qualifier(GBFeatureUtils::QUALIFIER_GROUP, "Secondary structure"));
    }
    QString start= QString::fromLatin1(cbuff+15, 5).trimmed();
    if(start.isEmpty()){
        si.setError(EMBLGenbankAbstractDocument::tr("Annotation start position is empty"));
        return SharedAnnotationData();
    }
    QString end= QString::fromLatin1(cbuff+22, 5).trimmed();
    if(end.isEmpty()){
        si.setError(EMBLGenbankAbstractDocument::tr("Annotation end position is empty"));
        return SharedAnnotationData();
    }
    a->location->reset();

    if(key == "DISULFID" && start != end){
        a->location->op=U2LocationOperator_Order;
        U2Region reg1(start.toInt()-1,1);
        U2Region reg2(end.toInt()-1,1);
        a->location->regions.append(reg1);
        a->location->regions.append(reg2);
    }else{
        U2Region reg(start.toInt()-1,end.toInt() - start.toInt()+1);
        a->location->regions.append(reg);
    }

    if (offset!=0) {
        U2Region::shift(offset, a->location->regions);
    }


    QString valQStr = QString::fromLatin1(cbuff).split(QRegExp("\\n")).first().mid(34);
    QString nameQStr = "Description";
    bool isDescription=true;

    const QByteArray& aminoQ = GBFeatureUtils::QUALIFIER_AMINO_STRAND;
    const QByteArray& nameQ = GBFeatureUtils::QUALIFIER_NAME;
    //here we have valid key and location;
    //reading qualifiers
    bool lineOk = true;
    while ((len = io->readUntil(cbuff, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk) ) > 0)  {
        if (len == 0 || len < QN_COL+1 || cbuff[K_COL]!=' ' || cbuff[0]!=fPrefix[0] || cbuff[1]!=fPrefix[1]) {
            io->skip(-len);
            if(isDescription && !valQStr.isEmpty()){
                isDescription=false;
                a->qualifiers.append(U2Qualifier(nameQStr, valQStr));
            }
            break;
        }
        if (!lineOk) {
            si.setError(EMBLGenbankAbstractDocument::tr("Unexpected line format"));
            break;
        }
        //parse line
        if(cbuff[A_COL] != '/'){//continue of description
            valQStr.append(" ");
            valQStr.append(QString::fromLatin1(cbuff).split(QRegExp("\\n")).takeAt(0).mid(34));
        }else{
            for (; QN_COL < len && TextUtils::LINE_BREAKS[(uchar)cbuff[len-1]]; len--){}; //remove line breaks
            int flen = len + readMultilineQualifier(io, cbuff, READ_BUFF_SIZE-len, len == maxAnnotationLineLen, len, si);
            //now the whole feature is in cbuff
            int valStart = A_COL + 1;
            for (; valStart < flen && cbuff[valStart] != '='; valStart++){}; //find '==' and valStart
            if (valStart < flen) {
                valStart++; //skip '=' char
            }
            const QBitArray& WHITE_SPACES = TextUtils::WHITES;
            for (; valStart < flen && WHITE_SPACES[(uchar)cbuff[flen-1]]; flen--){}; //trim value
            const char* qname = cbuff + QN_COL;
            int qnameLen = valStart - (QN_COL + 1);
            const char* qval = cbuff + valStart;
            int qvalLen = flen - valStart;
            if (qnameLen == aminoQ.length() && TextUtils::equals(qname, aminoQ.constData(), qnameLen)) {
                //a->aminoFrame = qvalLen == aminoQYes.length() && TextUtils::equals(qval, aminoQYes.constData(), qvalLen) ? TriState_Yes
                //             :  (qvalLen == aminoQNo.length()  && TextUtils::equals(qval, aminoQNo.constData(), qvalLen) ? TriState_No : TriState_Unknown);
            } else if (qnameLen == nameQ.length() && TextUtils::equals(qname, nameQ.constData(), qnameLen)) {
                a->name = QString::fromLocal8Bit(qval, qvalLen);
            } else {
                QString nameQStr = QString::fromLocal8Bit(qname, qnameLen);
                QString valQStr = QString::fromLocal8Bit(qval, qvalLen);
                a->qualifiers.append(U2Qualifier(nameQStr, valQStr));
            }
        }
    }
    return f;
}
}//namespace

