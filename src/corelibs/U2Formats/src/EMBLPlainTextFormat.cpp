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

#include "EMBLPlainTextFormat.h"
#include "GenbankLocationParser.h"
#include "DocumentFormatUtils.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectUtils.h>

#include <U2Core/IOAdapter.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAInfo.h>
#include <U2Core/QVariantUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

/* TRANSLATOR U2::EMBLPlainTextFormat */
/* TRANSLATOR U2::EMBLGenbankAbstractDocument */

EMBLPlainTextFormat::EMBLPlainTextFormat(QObject* p)
: EMBLGenbankAbstractDocument(BaseDocumentFormats::PLAIN_EMBL, tr("EMBL"), 80, DocumentFormatFlag_SupportStreaming, p)
{
    fileExtensions << "em" << "emb" << "embl";
    formatDescription = tr("EMBL Flat File Format is a rich format for storing sequences and associated annotations");
    sequenceStartPrefix = "SQ";
    fPrefix = "FT";

    tagMap["DT"] = DNAInfo::DATE;
    tagMap["PR"] = DNAInfo::PROJECT;
    tagMap["DE"] = DNAInfo::DEFINITION;
    tagMap["KW"] = DNAInfo::KEYWORDS;
    tagMap["CC"] = DNAInfo::COMMENT;
    tagMap["CO"] = DNAInfo::CONTIG;
}

FormatCheckResult EMBLPlainTextFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
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
            return FormatDetection_NotMatched;
        }
        return FormatDetection_HighSimilarity;
    }
    return FormatDetection_NotMatched;
}

//////////////////////////////////////////////////////////////////////////
// loading

bool EMBLPlainTextFormat::readIdLine(ParserState* s) {
    if (!s->hasKey("ID", 2)) {
        s->si.setError(EMBLPlainTextFormat::tr("ID is not the first line"));
        return false;
    }

    QString idLineStr= s->value();
    QStringList tokens = idLineStr.split(";");
    if (idLineStr.length() < 6 || tokens.isEmpty()) {
        s->si.setError(EMBLPlainTextFormat::tr("Error parsing ID line"));
        return false;
    }
    s->entry->name = tokens[0];
    DNALocusInfo loi;
    loi.name = tokens[0];
    if (tokens.size() > 1) {
        QString sec = tokens[1];
        if (sec.startsWith("SV ")) {
            s->entry->tags[DNAInfo::VERSION] = tokens[0] + "." + sec.mid(3);
        }
        QString last = tokens.last();
        if (last.endsWith("BP.")) {
            last.chop(3);
            s->entry->seqLen = last.toInt();
        }
    }
    if (tokens.size() == 7) {
        // seems to be canonical header
        // http://www.ebi.ac.uk/embl/Documentation/User_manual/printable.html
        //1. Primary accession number
        //2. Sequence version number
        //3. Topology: 'circular' or 'linear'
        //4. Molecule type (see note 1 below)
        //5. Data class (methodological approach)
        //6. Taxonomic division (see section 3.2)
        //7. Sequence length (see note 2 below)
        loi.topology = tokens[2];
        loi.molecule = tokens[3];
        loi.division = tokens[5];
        s->entry->circular = loi.topology.compare(LOCUS_TAG_CIRCULAR, Qt::CaseInsensitive) == 0;
    } else {
        // remember just in case
        s->entry->tags.insert(DNAInfo::EMBL_ID, idLineStr);
        s->entry->circular = idLineStr.contains(LOCUS_TAG_CIRCULAR, Qt::CaseInsensitive);
    }
    s->entry->tags.insert(DNAInfo::LOCUS, qVariantFromValue<DNALocusInfo>(loi));

    return true;
}

bool EMBLPlainTextFormat::readEntry(ParserState* st,U2SequenceImporter& seqImporter, int& sequenceLen,int& fullSequenceLen, bool merge, int gapSize,U2OpStatus& os) {
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
        if (st->hasKey("FH") || st->hasKey("XX") || st->hasKey("AH")) {
            continue;
        }
        if (st->hasKey("AC")) {
            QVariant v = st->entry->tags.value(DNAInfo::ACCESSION);
            QStringList l = st->value().split(QRegExp(";\\s*"), QString::SkipEmptyParts);
            st->entry->tags[DNAInfo::ACCESSION] = QVariantUtils::addStr2List(v, l);
            continue;
        }
        if (st->hasKey("OS")) {
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
            if (st->hasKey("OC")) {
                soi.taxonomy += st->value();
                while (st->readNextLine()) {
                    if (st->hasKey("OC")) {
                        soi.taxonomy.append(st->value());
                    } else if (!st->hasKey("XX")) {
                        break;
                    }
                }
            }
            if (st->hasKey("OG")) {
                soi.organelle = st->value();
            } else {
                hasLine = true;
            }
            st->entry->tags.insertMulti(DNAInfo::SOURCE, qVariantFromValue<DNASourceInfo>(soi));
            continue;
        }
        if (st->hasKey("RF") || st->hasKey("RN")) {
            while (st->readNextLine() && st->buff[0] == 'R')
            {
                //TODO
            }
            hasLine = true;
            continue;
        }

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
            if(merge && gapSize){
                seqImporter.addDefaultSymbolsBlock(gapSize,os);
                CHECK_OP(os,false);
            }
            readSequence(st,seqImporter,sequenceLen,fullSequenceLen,os);
            if (fullSequenceLen != st->entry->seqLen && !si.getWarnings().contains(EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE)) {
                si.addWarning(EMBLGenbankAbstractDocument::SEQ_LEN_WARNING_MESSAGE);
            }
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

}//namespace
