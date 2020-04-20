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

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2SafePoints.h>

#include "ace/AceImportUtils.h"
#include "DocumentFormatUtils.h"

namespace U2 {

///////////////////////////////////
//// Assembly
///////////////////////////////////

Assembly::Assembly() {
}

const Assembly::Sequence & Assembly::getReference() {
    return reference;
}

void Assembly::setReference(const Sequence &_reference) {
    reference = _reference;
    if (name.isEmpty()) {
        name = reference.name;
        if (name.endsWith("_ref")) {
            name.chop(QString("_ref").length());
        }
    }
}

QList<U2AssemblyRead> Assembly::getReads() const {
    return convertReads();
}

void Assembly::addRead(const Sequence &read) {
    reads << read;
}

void Assembly::setReads(const QList<Sequence> &_reads) {
    reads = _reads;
}

int Assembly::getReadsCount() const {
    return reads.count();
}

const QByteArray &Assembly::getName() const {
    return name;
}

void Assembly::setName(const QByteArray &_name) {
    name = _name;
}

bool Assembly::isValid() const {
    bool ok = reference.isValid();
    foreach(Sequence read, reads) {
        ok &= read.isValid();
    }
    return ok;
}

QList<U2AssemblyRead> Assembly::convertReads() const {
    QList<U2AssemblyRead> res;
    foreach (Assembly::Sequence localRead, reads) {
        U2AssemblyRead r(new U2AssemblyReadData);
        r->name = localRead.name;
        r->leftmostPos = localRead.offset;
        r->effectiveLen = localRead.data.length();
        r->readSequence = localRead.data;
        if (localRead.isComplemented) {
            r->flags = Reverse;
        }
        r->cigar = QList<U2CigarToken>() << U2CigarToken(U2CigarOp_M, localRead.data.length());
        res << r;
    }
    return res;
}

///////////////////////////////////
//// AceReader
///////////////////////////////////
const int AceReader::CONTIG_COUNT_POS = 1;
const int AceReader::READS_COUNT_POS = 3;
const int AceReader::READS_POS = 3;
const int AceReader::COMPLEMENT_POS = 2;
const int AceReader::PADDED_START_POS = 3;
const int AceReader::FIRST_QA_POS = 3;
const int AceReader::LAST_QA_POS = 4;

const QByteArray AceReader::AS = "AS";
const QByteArray AceReader::BS = "BS";
const QByteArray AceReader::CO = "CO";
const QByteArray AceReader::BQ = "BQ";
const QByteArray AceReader::AF = "AF";
const QByteArray AceReader::RD = "RD";
const QByteArray AceReader::QA = "QA";
const QByteArray AceReader::COMPLEMENT = "C";
const QByteArray AceReader::UNCOMPLEMENT = "U";

AceReader::AceReader(IOAdapter& _io, U2OpStatus &_os) :
    io(&_io),
    os(&_os),
    currentContig(0) {
    QByteArray readBuff(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buff = readBuff.data();
    qint64 len = 0;

    skipBreaks(io, buff, &len);
    CHECK_OP((*os), );
    QByteArray headerLine = (QByteArray(buff, len)).trimmed();
    CHECK_EXT(headerLine.startsWith(AS), os->setError(DocumentFormatUtils::tr("First line is not an ace header")), );

    contigsCount = getContigCount(headerLine);
    CHECK_OP((*os), );
    CHECK_EXT(contigsCount > 0, os->setError((DocumentFormatUtils::tr("There is no assemblies in input file"))), );
}

Assembly AceReader::getAssembly() {
    Assembly result;
    Assembly::Sequence reference;

    QByteArray readBuff(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buff = readBuff.data();
    qint64 len = 0;
    int readsCount = 0;
    QByteArray headerLine;

    QSet<QByteArray> names;
    QMap<QByteArray, bool> complMap;

    CHECK_EXT(currentContig < contigsCount, os->setError(DocumentFormatUtils::tr("There are not enough assemblies")), result);

    do {
        skipBreaks(io, buff, &len);
        CHECK_OP((*os), result);
        headerLine = (QByteArray(buff, len)).trimmed();
    } while (!headerLine.startsWith(CO));

    readsCount = getReadsCount(headerLine);
    CHECK_OP((*os), result);

    // consensus, is set as reference in assembly
    parseConsensus(io, buff, names, headerLine, reference);
    CHECK_OP((*os), result);

    // read AF tag
    QMap<QByteArray, int> posMap;
    parseAfTag(io, buff, readsCount, posMap, complMap, names);
    CHECK_OP((*os), result);

    int smallestOffset = getSmallestOffset(posMap);
    if (smallestOffset < 0) {
        QByteArray gaps(qAbs(smallestOffset), '*');
        reference.data.prepend(gaps);
    }
    result.setReference(reference);

    // read RD and QA tags
    while (readsCount > 0) {
        Assembly::Sequence read;

        parseRdAndQaTag(io, buff, names, read);
        CHECK_OP((*os), result);

        read.isComplemented = complMap.take(read.name);
        read.offset = posMap.value(read.name) - 1;
        if (smallestOffset < 0) {
            read.offset += qAbs(smallestOffset);
        }

        result.addRead(read);
        readsCount--;
    }

    currentContig++;

    return result;
}

bool AceReader::isFinish() {
    return currentContig >= contigsCount || io->isEof();
}

void AceReader::skipBreaks(IOAdapter *io, char *buff, qint64 *len) {
    bool lineOk = true;
    *len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
    CHECK_EXT(*len != 0, os->setError(DocumentFormatUtils::tr("Unexpected end of file")), );
    CHECK_EXT(lineOk || io->isEof(), os->setError(DocumentFormatUtils::tr("Line is too long")), );
}

int AceReader::getContigCount(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    int contigC = getSubString(line, CONTIG_COUNT_POS);
    CHECK_OP((*os), 0);
    CHECK_EXT(-1 != contigC, os->setError(DocumentFormatUtils::tr("No contig count tag in the header line")), -1);
    return contigC;
}

int AceReader::getSubString(QByteArray &line, int pos) {
    int curIdx = 0;
    char space = ' ';

    line = line.simplified();

    for (int i = 0; i < pos; i++) {
        curIdx = line.indexOf(space);
        CHECK_EXT(-1 != curIdx, os->setError(DocumentFormatUtils::tr("Not enough parameters in current line")), -1);
        line = line.mid(curIdx + 1);
    }

    curIdx = line.indexOf(space);
    if (-1 != curIdx) {
        line = line.mid(0, curIdx);
    }

    bool ok = false;
    int result = line.toInt(&ok);
    CHECK_EXT(ok, os->setError(DocumentFormatUtils::tr("Parameter is not a digit")), -1);

    return result;
}

int AceReader::getReadsCount(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    int readsCount = getSubString(line, READS_COUNT_POS);
    CHECK_OP_EXT((*os), os->setError(DocumentFormatUtils::tr("There is no note about reads count")), 0);
    return readsCount;
}

void AceReader::parseConsensus(IOAdapter *io, char *buff, QSet<QByteArray> &names, QByteArray &headerLine, Assembly::Sequence &consensus) {
    char aceBStartChar = 'B';
    QBitArray aceBStart = TextUtils::createBitMap(aceBStartChar);
    qint64 len = 0;
    bool ok = true;
    QByteArray line;

    consensus.name = getName(headerLine);
    CHECK_EXT(!names.contains(consensus.name), os->setError(DocumentFormatUtils::tr("A name is duplicated")), );

    names.insert(consensus.name);
    consensus.name += "_ref";

    do {
        len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, aceBStart, IOAdapter::Term_Exclude, &ok);
        CHECK_EXT(len > 0, os->setError(DocumentFormatUtils::tr("No consensus")), );

        len = TextUtils::remove(buff, len, TextUtils::WHITES);
        buff[len] = 0;
        consensus.data.append(buff);
        os->setProgress(io->getProgress());
    } while (!ok);

    len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &ok);
    line = (QByteArray(buff, len)).trimmed();
    CHECK_EXT(line.startsWith(BQ), os->setError(DocumentFormatUtils::tr("BQ keyword hasn't been found")), );

    formatSequence(consensus.data);
    CHECK_EXT(checkSeq(consensus.data), os->setError(DocumentFormatUtils::tr("Unexpected symbols in consensus data")), );
}

QByteArray AceReader::getName(const QByteArray &line) {
    int curIdx = 0;
    char space = ' ';

    QByteArray name = line.simplified();

    // Cut off tag
    curIdx = name.indexOf(space);
    CHECK_EXT(-1 != curIdx, os->setError(DocumentFormatUtils::tr("Can't find a sequence name in current line")), "");

    name = name.mid(curIdx + 1);

    curIdx = name.indexOf(space);
    if (-1 != curIdx) {
        // There is something else in this line
        name = name.mid(0, curIdx);
    }

    CHECK_EXT(!name.isEmpty(), os->setError(DocumentFormatUtils::tr("An empty sequence name")), "");
    return name;
}

bool AceReader::checkSeq(const QByteArray &seq) {
    DNAAlphabetRegistry* alRegistry = AppContext::getDNAAlphabetRegistry();
    SAFE_POINT(alRegistry, "Alphabet registry is NULL", false);
    const DNAAlphabet* al = alRegistry->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    SAFE_POINT(al, "Alphabet is NULL", false);

    return al->containsAll(seq.constData(), seq.length());
}

void AceReader::parseAfTag(U2::IOAdapter *io, char *buff, int count, QMap<QByteArray, int> &posMap, QMap<QByteArray, bool> &complMap, QSet<QByteArray> &names) {
    int readsCount = count;
    QByteArray afBlock;
    QByteArray readLine;
    QByteArray name;
    qint64 len = 0;

    CHECK(0 < readsCount, );

    do {    // skip unused BQ part
        skipBreaks(io, buff, &len);
        CHECK_OP((*os), );
        readLine = (QByteArray(buff, len)).trimmed();
    } while (!readLine.startsWith(AF));

    do {    // Read all AF entries
        afBlock += readLine + " ";
        skipBreaks(io, buff, &len);
        CHECK_OP((*os), );
        readLine = (QByteArray(buff, len)).trimmed();
    } while (!readLine.startsWith(BS));
    afBlock = afBlock.simplified();

    while (!afBlock.isEmpty()) {
        QByteArray afLine = afBlock;
        int afIndex = 0;

        // the first AF entry won't be found: it hasn't space before
        if (-1 != (afIndex = afBlock.indexOf(" " + AF))) {
            afLine = afBlock.mid(0, afIndex);
            afBlock.remove(0, afIndex + 1);     // with space before AF
        } else {
            afBlock.clear();
        }

        afLine.replace('\n', "");

        SAFE_POINT_EXT(afLine.startsWith(AF), os->setError(DocumentFormatUtils::tr("Invalid AF tag")), );

        name = getName(afLine);
        CHECK_OP((*os), );

        int readPos = readsPos(afLine);
        CHECK_OP((*os), );
        int complStrand = readsComplement(afLine);
        CHECK_OP((*os), );

        posMap.insert(name, readPos);

        bool cur_compl = (complStrand == 1);
        complMap.insert(name, cur_compl);

        QList<QByteArray> b = names.toList();

        CHECK_EXT(!names.contains(name), os->setError(DocumentFormatUtils::tr("A name is duplicated: %1").arg(QString(name))), );
        names.insert(name);

        readsCount--;
    }

    CHECK_EXT(0 == readsCount, os->setError(DocumentFormatUtils::tr("Not all reads were found")), );
    os->setProgress(io->getProgress());
}

int AceReader::readsPos(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    char space = ' ';

    prepareLine(line, READS_POS);
    CHECK_EXT(!line.contains(space), os->setError(DocumentFormatUtils::tr("Bad AF note")), 0);

    bool ok = true;
    int result = line.toInt(&ok);
    CHECK_EXT(ok, os->setError(DocumentFormatUtils::tr("Bad AF note")), 0);

    return result;
}

int AceReader::prepareLine(QByteArray &line, int pos) {
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

int AceReader::readsComplement(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    prepareLine(line, COMPLEMENT_POS);

    if (line.startsWith(UNCOMPLEMENT)) {
        return 0;
    } else if (line.startsWith(COMPLEMENT)) {
        return 1;
    } else {
        os->setError(DocumentFormatUtils::tr("Bad AF note"));
        return -1;
    }
}

int AceReader::paddedStartCons(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    getSubString(line, PADDED_START_POS);
    CHECK_OP((*os), 0);

    bool ok = true;
    int result = line.toInt(&ok);
    CHECK_EXT(ok, os->setError(DocumentFormatUtils::tr("Bad AF note")), 0);

    return result;
}

int AceReader::getSmallestOffset(const QMap<QByteArray, int> &posMap) {
    int smallestOffset = 0;
    foreach (int value, posMap) {
        smallestOffset = qMin(smallestOffset, value - 1);
    }

    return smallestOffset;
}

void AceReader::parseRdAndQaTag(U2::IOAdapter *io, char *buff, QSet<QByteArray> &names, Assembly::Sequence &read) {
    QByteArray rdBlock;
    qint64 len = 0;
    bool ok = true;
    char aceQStartChar = 'Q';
    QBitArray aceQStart = TextUtils::createBitMap(aceQStartChar);

    do {    // skip unused BS part
        skipBreaks(io, buff, &len);
        CHECK_OP((*os), );
        rdBlock = (QByteArray(buff, len)).trimmed();
    } while (!rdBlock.startsWith(RD));
    CHECK_EXT(rdBlock.startsWith(RD), os->setError(DocumentFormatUtils::tr("There is no read note")), );

    do {    // read the tail of RD part
        len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, aceQStart, IOAdapter::Term_Exclude, &ok);
        CHECK_EXT(len > 0, os->setError(DocumentFormatUtils::tr("Unexpected end of file")), );
        buff[len] = 0;
        rdBlock += QByteArray(" ") + QByteArray(buff);
        os->setProgress(io->getProgress());
    } while (!ok);
    rdBlock = rdBlock.simplified();

    QList<QByteArray> rdSplitted = rdBlock.split(' ');
    // Magic numbers: RD tag, name, three numbers, sequence data
    CHECK_EXT(6 <= rdSplitted.count(), os->setError(DocumentFormatUtils::tr("Invalid RD part")), );
    SAFE_POINT_EXT(RD == rdSplitted[0], os->setError(DocumentFormatUtils::tr("Can't find the RD tag")), );
    read.name = rdSplitted[1];

    for (int chain = 5; chain < rdSplitted.count(); chain++) {
        read.data += rdSplitted[chain];
    }

    len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &ok);
    QByteArray qaBlock = (QByteArray(buff, len)).trimmed();
    CHECK_EXT(qaBlock.startsWith(QA), os->setError(DocumentFormatUtils::tr("QA keyword hasn't been found")), );

    int clearRangeStart = getClearRangeStart(qaBlock);
    CHECK_OP((*os), );
    int clearRangeEnd = getClearRangeEnd(qaBlock);
    CHECK_OP((*os), );

    CHECK_EXT(clearRangeStart <= clearRangeEnd &&
              clearRangeEnd - clearRangeStart <= read.data.length(),
              os->setError(DocumentFormatUtils::tr("QA error bad range")), );

    formatSequence(read.data);
    CHECK_EXT(checkSeq(read.data), os->setError(DocumentFormatUtils::tr("Unexpected symbols in sequence data")), );

    CHECK_EXT(names.contains(read.name), os->setError(DocumentFormatUtils::tr("A name is not match with AF names")), );
    names.remove(read.name);
}

int AceReader::getClearRangeStart(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    int result = getSubString(line, FIRST_QA_POS);
    CHECK_OP_EXT((*os), os->setError(DocumentFormatUtils::tr("Can't find clear range start in current line")), 0);
    CHECK_EXT(result > 0, os->setError(DocumentFormatUtils::tr("Clear range start is invalid")), 0);
    return result;
}

int AceReader::getClearRangeEnd(const QByteArray &cur_line) {
    QByteArray line = cur_line;
    int result = getSubString(line, LAST_QA_POS);
    CHECK_OP_EXT((*os), os->setError(DocumentFormatUtils::tr("Can't find clear range end in current line")), 0);
    CHECK_EXT(result > 0, os->setError(DocumentFormatUtils::tr("Clear range end is invalid")), 0);
    return result;
}

void AceReader::formatSequence(QByteArray& data) {
    data = data.toUpper();
    data.replace('X', 'N');
    data.replace('*', 'N');
}

///////////////////////////////////
//// AceIterator
///////////////////////////////////

AceIterator::AceIterator(AceReader &_reader, U2OpStatus &_os) :
    reader(&_reader),
    os(&_os) {
}

bool AceIterator::hasNext() {
    return !reader->isFinish();
}

Assembly AceIterator::next() {
    CHECK_EXT(hasNext(), os->setError(DocumentFormatUtils::tr("There is no next element")), Assembly());
    return reader->getAssembly();
}

}   // namespace U2
