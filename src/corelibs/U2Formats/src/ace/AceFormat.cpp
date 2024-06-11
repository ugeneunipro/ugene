/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "AceFormat.h"

#include <U2Core/AppContext.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MsaImportUtils.h>
#include <U2Core/MsaObject.h>
#include <U2Core/MsaUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

namespace U2 {

const QString ACEFormat::CO = "CO";
const QString ACEFormat::RD = "RD";
const QString ACEFormat::QA = "QA";
const QString ACEFormat::AS = "AS";
const QString ACEFormat::AF = "AF";
const QString ACEFormat::BQ = "BQ";

ACEFormat::ACEFormat(QObject* p)
    : TextDocumentFormatDeprecated(p, BaseDocumentFormats::ACE, DocumentFormatFlags(0), QStringList("ace")) {
    formatName = tr("ACE");
    formatDescription = tr("ACE is a format used for storing information about genomic confgurations");
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

static qint64 modifyLine(QString& line, qint64 pos) {
    int curIdx = 0;
    char space = ' ';

    line = line.simplified();

    for (qint64 i = 0; i < pos; i++) {
        curIdx = line.indexOf(space);
        if (-1 == curIdx) {
            return 0;
        }

        line = line.mid(curIdx + 1);
    }
    curIdx = line.indexOf(space);
    if (-1 == curIdx) {
        return 0;
    }

    line = line.mid(0, curIdx);

    bool ok = false;
    qint64 result = line.toLongLong(&ok);
    if (ok == false) {
        return -1;
    } else {
        return result;
    }
}

static int prepareLine(QString& line, int pos) {
    int curIdx = 0;
    char space = ' ';

    line = line.simplified();

    for (int i = 0; i < pos; i++) {
        curIdx = line.indexOf(space);
        if (-1 == curIdx) {
            return -1;
        }

        line = line.mid(curIdx + 1);
    }

    return curIdx;
}

#define READS_COUNT_POS 3
static int readsCount(const QString& cur_line) {
    QString line = cur_line;
    return modifyLine(line, READS_COUNT_POS);
}

#define CONTIG_COUNT_POS 1
static int contigCount(const QString& cur_line) {
    QString line = cur_line;
    return modifyLine(line, CONTIG_COUNT_POS);
}

#define LAST_QA_POS 4
static qint64 clearRange(const QString& cur_line) {
    QString line = cur_line;
    modifyLine(line, LAST_QA_POS);

    bool ok = true;
    qint64 result = line.toLongLong(&ok);
    if (!ok) {
        return LLONG_MAX;
    } else {
        return result;
    }
}
#define PADDED_START_POS 3
static qint64 paddedStartCons(const QString& cur_line) {
    QString line = cur_line;
    modifyLine(line, PADDED_START_POS);

    bool ok = true;
    qint64 result = line.toLongLong(&ok);
    if (!ok) {
        return LLONG_MAX;
    } else {
        return result;
    }
}

#define READS_POS 3
static qint64 readsPos(const QString& cur_line) {
    QString line = cur_line;
    prepareLine(line, READS_POS);

    if (-1 != line.indexOf(' ')) {
        return LLONG_MAX;
    }

    line = line.mid(0, line.length());

    bool ok = true;
    qint64 result = line.toLongLong(&ok);
    if (!ok) {
        return LLONG_MAX;
    } else {
        return result;
    }
}
#define COMPLEMENT_POS 2
static int readsComplement(const QString& cur_line) {
    QString line = cur_line;
    prepareLine(line, COMPLEMENT_POS);

    if (line.startsWith("U")) {
        return 0;
    } else if (line.startsWith("C")) {
        return 1;
    } else {
        return -1;
    }
}

static QString getName(const QString& line) {
    int curIdx = 0;
    char space = ' ';

    QString name = line.simplified();

    curIdx = name.indexOf(space);
    if (-1 == curIdx) {
        return "";
    }

    name = name.mid(curIdx + 1);

    curIdx = name.indexOf(space);
    if (-1 == curIdx) {
        return "";
    }

    name = name.mid(0, curIdx);

    return name;
}

static bool checkSeq(const QByteArray& seq) {
    const DNAAlphabet* alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    for (int i = 0; i < seq.length(); i++) {
        if (!(alphabet->contains(seq[i]) || seq[i] == '*')) {
            return false;
        }
    }
    return true;
}

static inline void skipBreaks(U2::IOAdapter* io, U2OpStatus& ti, char* buff, qint64* len) {
    bool lineOk = true;
    *len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
    CHECK_EXT(!io->hasError(), ti.setError(io->errorString()), );
    CHECK_EXT(*len != 0, ti.setError(ACEFormat::tr("Unexpected end of file")), );  // end if stream
    CHECK_EXT(lineOk, ti.setError(ACEFormat::tr("Line is too long")), );
}

static inline void parseConsensus(U2::IOAdapter* io, U2OpStatus& ti, char* buff, QString& consName, QString& headerLine, QByteArray& consensus) {
    char aceBStartChar = 'B';
    QBitArray aceBStart = TextUtils::createBitMap(aceBStartChar);
    qint64 len = 0;
    bool ok = true;
    QString line;
    consName = getName(headerLine);
    CHECK_EXT(!consName.isEmpty(), ti.setError(ACEFormat::tr("There is no AF note")), );

    consensus.clear();
    do {
        len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, aceBStart, IOAdapter::Term_Exclude, &ok);
        CHECK_EXT(!io->hasError(), ti.setError(io->errorString()), );
        CHECK_EXT(len > 0, ti.setError(ACEFormat::tr("No consensus")), );

        len = TextUtils::remove(buff, len, TextUtils::WHITES);
        buff[len] = 0;
        consensus.append(buff);
        ti.setProgress(io->getProgress());
    } while (!ti.isCoR() && !ok);

    len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &ok);
    CHECK_EXT(!io->hasError(), ti.setError(io->errorString()), );

    line = QString(QByteArray(buff, len)).trimmed();
    CHECK_EXT(line.startsWith("BQ"), ti.setError(ACEFormat::tr("BQ keyword hasn't been found")), );

    consensus = consensus.toUpper();
    CHECK_EXT(checkSeq(consensus), ti.setError(ACEFormat::tr("Bad consensus data")), );

    consensus.replace('*', U2Msa::GAP_CHAR);
}

void ACEFormat::parseAFTag(U2::IOAdapter* io, U2OpStatus& ti, char* buff, int count, QList<Assembly::Sequence>& reads) {
    int count1 = count;
    QString readLine;
    QString name;
    qint64 len = 0;
    while (!ti.isCoR() && count1 > 0) {
        do {
            skipBreaks(io, ti, buff, &len);
            CHECK_OP(ti, );

            readLine = QString(QByteArray(buff, len)).trimmed();
        } while (!readLine.startsWith("AF"));

        name = getName(readLine);
        if (!readLine.startsWith("AF") || "" == name) {
            ti.setError(ACEFormat::tr("There is no AF note"));
            return;
        }

        qint64 readPos = readsPos(readLine);
        int complStrand = readsComplement(readLine);
        if ((LLONG_MAX == readPos) || (-1 == complStrand)) {
            ti.setError(ACEFormat::tr("Bad AF note"));
            return;
        }

        qint64 paddedStart = paddedStartCons(readLine);
        CHECK_EXT(paddedStart != LLONG_MAX, ti.setError(ACEFormat::tr("Bad AF note")), );

        Assembly::Sequence read;
        read.name = name.toLocal8Bit();
        read.offset = paddedStart;
        read.isComplemented = (complStrand == 1);
        reads << read;

        count1--;
        ti.setProgress(io->getProgress());
    }
}

void ACEFormat::parseRDandQATag(U2::IOAdapter* io, U2OpStatus& ti, char* buff, QString& name, QByteArray& sequence) {
    QString line;
    qint64 len = 0;
    bool ok = true;
    char aceQStartChar = 'Q';
    QBitArray aceQStart = TextUtils::createBitMap(aceQStartChar);
    do {
        skipBreaks(io, ti, buff, &len);
        CHECK_OP(ti, );

        line = QString(QByteArray(buff, len)).trimmed();
    } while (!line.startsWith("RD"));

    name = getName(line);
    if (!line.startsWith("RD") || "" == name) {
        ti.setError(ACEFormat::tr("There is no read note"));
        return;
    }

    sequence.clear();
    do {
        len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, aceQStart, IOAdapter::Term_Exclude, &ok);
        CHECK_EXT(!io->hasError(), ti.setError(io->errorString()), );
        CHECK_EXT(len > 0, ti.setError(ACEFormat::tr("No sequence")), );

        len = TextUtils::remove(buff, len, TextUtils::WHITES);
        buff[len] = 0;
        sequence.append(buff);
        ti.setProgress(io->getProgress());
    } while (!ti.isCoR() && !ok);

    len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &ok);
    CHECK_EXT(!io->hasError(), ti.setError(io->errorString()), );

    line = QString(QByteArray(buff, len)).trimmed();
    CHECK_EXT(line.startsWith("QA"), ti.setError(ACEFormat::tr("QA keyword hasn't been found")), );

    qint64 clearRangeStart = 0;
    qint64 clearRangeEnd = 0;

    clearRangeStart = readsCount(line);
    CHECK_EXT(clearRangeStart != -1, ti.setError(ACEFormat::tr("QA error no clear range")), );

    clearRangeEnd = clearRange(line);
    CHECK_EXT(clearRangeEnd != 0, ti.setError(ACEFormat::tr("QA error no clear range")), );

    len = sequence.length();
    if (clearRangeStart > clearRangeEnd || clearRangeEnd > len) {
        ti.setError(ACEFormat::tr("QA error bad range"));
        return;
    }

    sequence = sequence.toUpper();
    CHECK_EXT(checkSeq(sequence), ti.setError(ACEFormat::tr("Bad sequence data")), );

    sequence.replace('*', U2Msa::GAP_CHAR);
    sequence.replace('N', U2Msa::GAP_CHAR);
    sequence.replace('X', U2Msa::GAP_CHAR);
}

qint64 ACEFormat::getSmallestOffset(const QList<Assembly::Sequence>& reads) {
    qint64 smallestOffset = 0;
    for (const auto& read : qAsConst(reads)) {
        smallestOffset = qMin(smallestOffset, read.offset - 1);
    }

    return smallestOffset;
}

void ACEFormat::load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os) {
    QScopedPointer<AceReader> aceReader(new AceReader(*io, os));
    CHECK_OP(os, );
    CHECK(aceReader->getContigsCount() > 0, );

    QScopedPointer<AceIterator> iterator;
    iterator.reset(new AceIterator(*aceReader, os));

    int contigsImportedNum = 0;
    QSet<QString> consensusNames;
    while (iterator->hasNext()) {
        CHECK_OP(os, );
        Assembly aceAssembly = iterator->next();
        CHECK_OP(os, );

        const auto& consensus = aceAssembly.getReference();
        QString consensusName = consensus.name;
        consensusName = GUrlUtils::rollFileName(consensusName, " ", consensusNames);
        consensusNames.insert(consensusName);
        Msa al(consensusName);
        QByteArray consensusData = consensus.data;
        consensusData.replace('*', U2Msa::GAP_CHAR);
        al->addRow(consensus.name, consensusData);

        const auto& sequences = aceAssembly.getOriginalReads();
        qint64 smallestOffset = getSmallestOffset(sequences);
        for (const auto& sequence : qAsConst(sequences)) {
            QString rowName = sequence.name;
            if (sequence.isComplemented) {
                rowName.append("(rev-compl)");
            }
            QByteArray sequencesData = sequence.data;
            sequencesData.replace('N', U2Msa::GAP_CHAR);

            qint64 pos = sequence.offset - 1;
            if (smallestOffset < 0) {
                pos += qAbs(smallestOffset);
            }
            QByteArray offsetGaps;
            offsetGaps.fill(U2Msa::GAP_CHAR, pos);
            sequencesData.prepend(offsetGaps);

            al->addRow(rowName, sequencesData);
        }

        U2AlphabetUtils::assignAlphabet(al);
        CHECK_EXT(al->getAlphabet() != nullptr, ACEFormat::tr("Alphabet unknown"), );

        const QString folder = hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

        MsaObject* obj = MsaImportUtils::createMsaObject(dbiRef, al, os, folder);
        CHECK_OP(os, );
        objects.append(obj);
        os.setProgress(io->getProgress());
        contigsImportedNum++;
    }
    CHECK_EXT(aceReader->getContigsCount() == contigsImportedNum, os.setError(tr("Invalid source file")), );

    return;
}

FormatCheckResult ACEFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    static const char* formatTag = "AS";

    if (!rawData.startsWith(formatTag)) {
        return FormatDetection_NotMatched;
    }
    return FormatDetection_AverageSimilarity;
}

Document* ACEFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QList<GObject*> objs;
    load(io, dbiRef, objs, fs, os);

    CHECK_OP_EXT(os, qDeleteAll(objs), nullptr);

    if (objs.isEmpty()) {
        os.setError(ACEFormat::tr("File doesn't contain any msa objects"));
        return nullptr;
    }
    auto doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objs, fs);

    return doc;
}

}  // namespace U2
