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

#ifndef _U2_ACE_IMPORT_UTILS_H_
#define _U2_ACE_IMPORT_UTILS_H_

#include <U2Core/IOAdapter.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class Assembly {
public:
    class Sequence {   // it is consensus in the ACE format specification
    public:
        Sequence() : offset(0), isComplemented(false) {}

        bool isValid() const { return !name.isEmpty() && offset >= 0; }

        QByteArray data;
        QByteArray name;
        int offset;
        bool isComplemented;
    };

    Assembly();

    const Sequence & getReference();
    void setReference(const Sequence &reference);

    QList<U2AssemblyRead> getReads() const;
    int getReadsCount() const;
    void addRead(const Sequence &read);
    void setReads(const QList<Sequence> &reads);

    const QByteArray &getName() const;
    void setName(const QByteArray &name);

    bool isValid() const;

private:
    QList<U2AssemblyRead> convertReads() const;

    Sequence reference;
    QList<Sequence> reads;
    QByteArray name;
};

class AceReader {
public:
    AceReader(IOAdapter &io, U2OpStatus &os);

    Assembly getAssembly();
    bool isFinish();
    int getContigsCount() { return contigsCount; }

private:
    void skipBreaks(IOAdapter *io, char *buff, qint64 *len);
    int getContigCount(const QByteArray& cur_line);
    int getSubString(QByteArray &line, int pos);
    int getReadsCount(const QByteArray &cur_line);
    void parseConsensus(IOAdapter *io, char *buff, QSet<QByteArray> &names, QByteArray &headerLine, Assembly::Sequence &consensus);
    QByteArray getName(const QByteArray &line);
    bool checkSeq(const QByteArray &seq);
    void parseAfTag(IOAdapter *io, char *buff, int count, QMap<QByteArray, int> &posMap, QMap<QByteArray, bool> &complMap, QSet<QByteArray> &names);
    int readsPos(const QByteArray &cur_line);
    int prepareLine(QByteArray &line, int pos);
    int readsComplement(const QByteArray &cur_line);
    int paddedStartCons(const QByteArray &cur_line);
    int getSmallestOffset(const QMap<QByteArray, int> &posMap);
    void parseRdAndQaTag(U2::IOAdapter *io, char *buff, QSet<QByteArray> &names, Assembly::Sequence &read);
    int getClearRangeStart(const QByteArray &cur_line);
    int getClearRangeEnd(const QByteArray &cur_line);
    void formatSequence(QByteArray& data);

    IOAdapter *io;
    U2OpStatus *os;
    int contigsCount;
    int currentContig;

    static const int CONTIG_COUNT_POS;
    static const int READS_COUNT_POS;
    static const int READS_POS;
    static const int COMPLEMENT_POS;
    static const int PADDED_START_POS;
    static const int FIRST_QA_POS;
    static const int LAST_QA_POS;

    static const QByteArray AS;
    static const QByteArray BS;
    static const QByteArray CO;
    static const QByteArray BQ;
    static const QByteArray AF;
    static const QByteArray RD;
    static const QByteArray QA;
    static const QByteArray COMPLEMENT;
    static const QByteArray UNCOMPLEMENT;
};

class AceIterator {
public:
    AceIterator(AceReader &reader, U2OpStatus &os);

    bool hasNext();
    Assembly next();
    Assembly peek();

private:
    AceReader *reader;
    U2OpStatus *os;
};

}   // namespace U2

#endif // _U2_ACE_IMPORT_UTILS_H_
