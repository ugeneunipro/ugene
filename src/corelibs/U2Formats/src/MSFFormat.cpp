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

#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentWalker.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

#include "MSFFormat.h"

namespace U2 {

const int MSFFormat::CHECK_SUM_MOD = 10000;
const QByteArray MSFFormat::MSF_FIELD = "MSF:";
const QByteArray MSFFormat::CHECK_FIELD = "Check:";
const QByteArray MSFFormat::LEN_FIELD = "Len:";
const QByteArray MSFFormat::NAME_FIELD = "Name:";
const QByteArray MSFFormat::TYPE_FIELD = "Type:";
const QByteArray MSFFormat::WEIGHT_FIELD = "Weight:";
const QByteArray MSFFormat::TYPE_VALUE_PROTEIN = "P";
const QByteArray MSFFormat::TYPE_VALUE_NUCLEIC = "N";
const double MSFFormat::WEIGHT_VALUE = 1.0;
const QByteArray MSFFormat::END_OF_HEADER_LINE = "..";
const QByteArray MSFFormat::SECTION_SEPARATOR = "//";
const int MSFFormat::CHARS_IN_ROW = 50;
const int MSFFormat::CHARS_IN_WORD = 10;

/* TRANSLATOR U2::MSFFormat */

//TODO: recheck if it does support streaming! Fix isObjectOpSupported if not!

MSFFormat::MSFFormat(QObject* p) : TextDocumentFormat(p, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject, QStringList("msf")) {
    formatName = tr("MSF");
    supportedObjectTypes+=GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatDescription = tr("MSF format is used to store multiple aligned sequences. Files include the sequence name and the sequence itself, which is usually aligned with other sequences in the file.");
}

static bool getNextLine(IOAdapter* io, QByteArray& line, bool simplify = true) {
    QByteArray readBuffer(DocumentFormat::READ_BUFF_SIZE, '\0');
    char* buff = readBuffer.data();

    qint64 len;
    bool eolFound = false, eof = false;
    while (!eolFound) {
        len = io->readLine(buff, DocumentFormat::READ_BUFF_SIZE, &eolFound);
        if (len < DocumentFormat::READ_BUFF_SIZE && !eolFound) {
            eolFound = eof = true;
        }
        line += readBuffer;
    }
    if (len != DocumentFormat::READ_BUFF_SIZE) {
        line.resize(line.size() + len - DocumentFormat::READ_BUFF_SIZE);
    }
    if (simplify) {
        line = line.simplified();
    }
    return eof;
}

static QByteArray getField(const QByteArray& line, const QByteArray& name) {
    int p = line.indexOf(name);
    if (p >= 0) {
        p += name.length();
        if (line[p] == ' ')
            ++p;
        int q = line.indexOf(' ', p);
        if (q >= 0)
            return line.mid(p, q - p);
        else
            return line.mid(p);
    }
    return QByteArray();
}

int MSFFormat::getCheckSum(const QByteArray& seq) {
    int sum = 0;
    static int CHECK_SUM_COUNTER_MOD = 57;
    for (int i = 0; i < seq.length(); ++i) {
        char ch = seq[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = ch + 'A' - 'a';
        }
        sum = (sum + ((i % CHECK_SUM_COUNTER_MOD) + 1) * ch) % MSFFormat::CHECK_SUM_MOD;
    }
    return sum;
}

struct MsfRow {
    MsfRow () : checksum(0), length(0) {}

    QString name;
    int checksum;
    int length;
};

void MSFFormat::load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& ti) {
    MultipleSequenceAlignment al(io->getURL().baseFileName());

    //skip comments
    int checkSum = -1;
    while (!ti.isCoR() && checkSum < 0) {
        QByteArray line;
        if (getNextLine(io, line)) {
            ti.setError(MSFFormat::tr("Incorrect format"));
            return;
        }
        if (line.endsWith(END_OF_HEADER_LINE)) {
            bool ok;
            checkSum = getField(line, CHECK_FIELD).toInt(&ok);
            if (!ok || checkSum < 0)
                checkSum = CHECK_SUM_MOD;
        }
        ti.setProgress(io->getProgress());
    }

    //read info
    int sum = 0;
    QList<MsfRow> msfRows;

    QMap<QString, int> duplicatedNamesCount;        // a workaround for incorrectly saved files

    while (!ti.isCoR()) {
        QByteArray line;
        if (getNextLine(io, line)) {
            ti.setError(MSFFormat::tr("Unexpected end of file"));
            return;
        }
        if (line.startsWith(SECTION_SEPARATOR)) {
            break;
        }

        bool ok = false;
        QString name = QString::fromUtf8(getField(line, NAME_FIELD).data());
        if (name.isEmpty()) {
            continue;
        }
        int check = getField(line, CHECK_FIELD).toInt(&ok);
        if (!ok || check < 0) {
            sum = check = CHECK_SUM_MOD;
        }

        foreach (const MsfRow &msfRow, msfRows) {
            if (name == msfRow.name) {
                duplicatedNamesCount[name] = duplicatedNamesCount.value(name, 1) + 1;
            }
        }

        MsfRow row;
        row.name = name;
        row.checksum = check;

        msfRows << row;
        al->addRow(name, QByteArray());
        if (sum < CHECK_SUM_MOD) {
            sum = (sum + check) % CHECK_SUM_MOD;
        }

        ti.setProgress(io->getProgress());
    }
    if (checkSum < CHECK_SUM_MOD && sum < CHECK_SUM_MOD && sum != checkSum) {
        coreLog.info(tr("File check sum is incorrect: expected value: %1, current value %2").arg(checkSum).arg(sum));
    }

    //read data
    bool eof = false;

    QRegExp coordsRegexp("^\\s+\\d+(\\s+\\d+)?\\s*$");
    QRegExp onlySpacesRegexp("^\\s+$");

    QMap<QString, int> processedDuplicatedNames;

    while (!eof && !ti.isCoR()) {
        QByteArray line;
        eof = getNextLine(io, line, false);

        if (line.isEmpty() || -1 != onlySpacesRegexp.indexIn(line) || -1 != coordsRegexp.indexIn(line)) {
            // Skip empty lines, lines with spaces only and lines with alignment coordinates
            continue;
        }

        line = line.simplified();
        const int spaceIndex = line.indexOf(" ");
        if (-1 == spaceIndex) {
            // Skip the line without spaces
            continue;
        }

        const QByteArray name = line.mid(0, spaceIndex);
        int msfRowNumber = -1;
        int duplicatesSkipped = 0;
        for (int i = 0; i < msfRows.length(); i++) {
            if (msfRows[i].name.toUtf8() == name) {
                if (duplicatesSkipped == processedDuplicatedNames.value(name, 0)) {
                    // This row is not processed yet
                    msfRowNumber = i;
                    if (duplicatedNamesCount.contains(name)) {
                        // Mark the row as processed
                        processedDuplicatedNames[name] = duplicatesSkipped + 1;
                        if (processedDuplicatedNames.value(name, 0) == duplicatedNamesCount.value(name, 0)) {
                            // All rows in this block are already processed, prepare for the next block
                            processedDuplicatedNames[name] = 0;
                        }
                    }
                    break;
                } else {
                    // This row is already processed, skip it
                    duplicatesSkipped++;
                }
            }
        }

        if (-1 == msfRowNumber) {
            // Skip the line with unknown row name
            continue;
        }

        for (int q, p = line.indexOf(' ') + 1; p > 0; p = q + 1) {
            q = line.indexOf(' ', p);
            QString subSeq = (q < 0) ? line.mid(p) : line.mid(p, q - p);
            al->appendChars(msfRowNumber, msfRows[msfRowNumber].length, subSeq.toUtf8().constData(), subSeq.length());
            msfRows[msfRowNumber].length += subSeq.length();
        }

        ti.setProgress(io->getProgress());
    }

    //checksum
    U2OpStatus2Log seqCheckOs;
    const int numRows = al->getNumRows();
    for (int i = 0; i < numRows; i++) {
        const MultipleSequenceAlignmentRow row = al->getMsaRow(i);
        const int expectedCheckSum = msfRows[i].checksum;
        const int sequenceCheckSum = getCheckSum(row->toByteArray(seqCheckOs, al->getLength()));
        if ( expectedCheckSum < CHECK_SUM_MOD &&  sequenceCheckSum != expectedCheckSum) {
            coreLog.info(tr("Unexpected check sum in the row number %1, name: %2; expected value: %3, current value %4").arg(i + 1).arg(row->getName()).arg(expectedCheckSum).arg(sequenceCheckSum));
        }
        al->replaceChars(i, '.', U2Msa::GAP_CHAR);
        al->replaceChars(i, '~', U2Msa::GAP_CHAR);
    }

    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet() != NULL, ti.setError(MSFFormat::tr("Alphabet unknown")), );

    U2OpStatus2Log os;
    const QString folder = hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, );
    objects.append(obj);
}

Document* MSFFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){
    QList<GObject*> objs;
    load(io, dbiRef, objs, fs, os);

    CHECK_OP_EXT(os, qDeleteAll(objs), NULL);
    return new Document(this, io->getFactory(), io->getURL(), dbiRef, objs, fs);
}

static bool writeBlock(IOAdapter *io, U2OpStatus& ti, const QByteArray& buf) {
    int len = io->writeBlock(buf);
    if (len != buf.length()) {
        ti.setError(L10N::errorTitle());
        return true;
    }
    return false;
}

void MSFFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    MultipleSequenceAlignmentObject* obj = NULL;
    if ((d->getObjects().size() != 1) || ((obj = qobject_cast<MultipleSequenceAlignmentObject*>(d->getObjects().first())) == NULL)) {
        os.setError("No data to write;");
        return;
    }

    QList<GObject*> als; als << obj;
    QMap< GObjectType, QList<GObject*> > objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = als;
    storeEntry(io, objectsMap, os);
    CHECK_EXT(!os.isCoR(), os.setError(L10N::errorWritingFile(d->getURL())), );
}

namespace {

const QString SUFFIX_SEPARATOR = "_";

void splitCompleteName(const QString &completeName, QString &baseName, QString &suffix) {
    const int separatorIndex = completeName.lastIndexOf(SUFFIX_SEPARATOR);
    if (-1 == separatorIndex) {
        baseName = completeName;
        suffix = QString();
        return;
    }

    suffix = completeName.mid(separatorIndex + 1);
    bool ok = false;
    suffix.toInt(&ok);
    if (!ok) {
        baseName = completeName;
        suffix = QString();
    } else {
        baseName = completeName.left(separatorIndex);
    }
}

QString increaseSuffix(const QString &completeName) {
    QString baseName;
    QString suffix;
    splitCompleteName(completeName, baseName, suffix);
    if (suffix.isEmpty()) {
        return completeName + SUFFIX_SEPARATOR + QString::number(1);
    }
    return baseName + SUFFIX_SEPARATOR + QString("%1").arg(suffix.toInt() + 1, suffix.length(), 10, QChar('0'));
}

QString rollRowName(QString rowName, const QList<QString> &nonUniqueNames) {
    while (nonUniqueNames.contains(rowName)) {
        rowName = increaseSuffix(rowName);
    }
    return rowName;
}

}

void MSFFormat::storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "MSF entry storing: no alignment", );
    const QList<GObject*> &als = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(1 == als.size(), "MSF entry storing: alignment objects count error", );

    const MultipleSequenceAlignmentObject* obj = dynamic_cast<MultipleSequenceAlignmentObject*>(als.first());
    SAFE_POINT(NULL != obj, "MSF entry storing: NULL alignment object", );

    const MultipleSequenceAlignment msa = obj->getMultipleAlignment();

    // Make row names unique
    QMap<qint64, QString> uniqueRowNames;
    int maxNameLen = 0;
    foreach (const MultipleSequenceAlignmentRow &row , msa->getMsaRows()) {
        uniqueRowNames.insert(row->getRowId(), rollRowName(row->getName().replace(' ', '_'), uniqueRowNames.values()));
        maxNameLen = qMax(maxNameLen, uniqueRowNames.last().length());
    }

    //precalculate seq writing params
    int maLen = msa->getLength();
    int checkSum = 0;
    static int maxCheckSumLen = 4;
    QMap <qint64, int> checkSums;
    foreach(const MultipleSequenceAlignmentRow& row , msa->getMsaRows()) {
        QByteArray sequence = row->toByteArray(os, maLen).replace(U2Msa::GAP_CHAR, '.');
        int seqCheckSum = getCheckSum(sequence);
        checkSums.insert(row->getRowId(), seqCheckSum);
        checkSum = (checkSum + seqCheckSum) % CHECK_SUM_MOD;
    }
    int maxLengthLen = QString::number(maLen).length();

    //write first line
    QByteArray line = "  " + MSF_FIELD;
    line += " " + QByteArray::number(maLen);
    line += "  " + TYPE_FIELD;
    line += " " + ( obj->getAlphabet()->isAmino() ? TYPE_VALUE_PROTEIN : TYPE_VALUE_NUCLEIC );
    line += "  " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    line += "  " + CHECK_FIELD;
    line += " " + QByteArray::number(checkSum);
    line += "  " + END_OF_HEADER_LINE + "\n\n";
    if (writeBlock(io, os, line))
        return;

    //write info
    foreach(const MultipleSequenceAlignmentRow& row, msa->getMsaRows()) {
        QByteArray line = " " + NAME_FIELD;
        line += " " + uniqueRowNames[row->getRowId()].leftJustified(maxNameLen + 1);
        line += "  " + LEN_FIELD;
        line += " " + QString("%1").arg(maLen, -maxLengthLen);
        line += "  " + CHECK_FIELD;
        line += " " + QString("%1").arg(checkSums[row->getRowId()], -maxCheckSumLen);
        line += "  " + WEIGHT_FIELD;
        line += " " + QByteArray::number(WEIGHT_VALUE) + "\n";
        if (writeBlock(io, os, line)) {
            return;
        }
    }
    if (writeBlock(io, os, "\n" + SECTION_SEPARATOR + "\n\n")) {
        return;
    }

    MultipleSequenceAlignmentWalker walker(msa, '.');
    for (int i = 0; !os.isCoR() && i < maLen; i += CHARS_IN_ROW) {
        /* write numbers */ {
            QByteArray line(maxNameLen + 2, ' ');
            QString t = QString("%1").arg(i + 1);
            QString s = QString("%1").arg(i + CHARS_IN_ROW < maLen ? i + CHARS_IN_ROW : maLen);
            int r = maLen - i < CHARS_IN_ROW ? maLen % CHARS_IN_ROW : CHARS_IN_ROW;
            r += (r - 1) / CHARS_IN_WORD - (t.length() + s.length());
            line += t;
            if (r > 0) {
                line += QByteArray(r, ' ');
                line += s;
            }
            line += '\n';
            if (writeBlock(io, os, line)) {
                return;
            }
        }

        //write sequence
        QList<QByteArray> seqs = walker.nextData(CHARS_IN_ROW, os);
        CHECK_OP(os, );
        QList<QByteArray>::ConstIterator si = seqs.constBegin();
        QList<MultipleSequenceAlignmentRow> rows = msa->getMsaRows();
        QList<MultipleSequenceAlignmentRow>::ConstIterator ri = rows.constBegin();
        for (; si != seqs.constEnd(); si++, ri++) {
            const MultipleSequenceAlignmentRow &row = *ri;
            QByteArray line = uniqueRowNames[row->getRowId()].leftJustified(maxNameLen + 1).toUtf8();

            for (int j = 0; j < CHARS_IN_ROW && i + j < maLen; j += CHARS_IN_WORD) {
                line += ' ';
                int nChars = qMin(CHARS_IN_WORD, maLen - (i + j));
                QByteArray bytes = si->mid(j, nChars);
                line += bytes;
            }
            SAFE_POINT_OP(os, );
            line += '\n';
            if (writeBlock(io, os, line)) {
                return;
            }
        }
        if (writeBlock(io, os, "\n")) {
            return;
        }
    }
}


FormatCheckResult MSFFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();

    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    if (hasBinaryData) {
        return FormatDetection_NotMatched;
    }
    if (rawData.contains("MSF:")
        || rawData.contains("!!AA_MULTIPLE_ALIGNMENT 1.0") || rawData.contains("!!NA_MULTIPLE_ALIGNMENT 1.0")
        || (rawData.contains("Name:") && rawData.contains("Len:") && rawData.contains("Check:") && rawData.contains("Weight:")))
    {
        return FormatDetection_VeryHighSimilarity;
    }

    if (rawData.contains("GDC ")) {
        return FormatDetection_AverageSimilarity;
    }

    //MSF documents may contain unlimited number of comment lines in header ->
    //it is impossible to determine if file has MSF format by some predefined
    //amount of raw data read from it.
    if (rawData.contains("GCG ") || rawData.contains("MSF ")) {
        return FormatDetection_LowSimilarity;
    }
    return FormatDetection_VeryLowSimilarity;
}

} //namespace U2
