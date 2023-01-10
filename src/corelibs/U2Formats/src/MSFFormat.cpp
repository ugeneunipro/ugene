/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "MSFFormat.h"

#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentWalker.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

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

// TODO: recheck if it does support streaming! Fix isObjectOpSupported if not!

MSFFormat::MSFFormat(QObject* parent)
    : TextDocumentFormat(parent, BaseDocumentFormats::MSF, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject, QStringList("msf")) {
    formatName = tr("MSF");
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatDescription = tr("MSF format is used to store multiple aligned sequences. Files include the sequence name and the sequence itself, which is usually aligned with other sequences in the file.");
}

/** Parses MSF field value from the line. Returns empty line if the field was not found. */
static QString parseField(const QString& line, const QString& name) {
    int p = line.indexOf(name);
    if (p >= 0) {
        p += name.length();
        if (line[p] == ' ') {
            ++p;
        }
        int q = line.indexOf(' ', p);
        return q >= 0 ? line.mid(p, q - p) : line.mid(p);
    }
    return "";
}

int MSFFormat::getCheckSum(const QByteArray& seq) {
    int sum = 0;
    static int CHECK_SUM_COUNTER_MOD = 57;
    for (int i = 0; i < seq.length(); ++i) {
        char ch = seq[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = (char)(ch + 'A' - 'a');
        }
        sum = (sum + ((i % CHECK_SUM_COUNTER_MOD) + 1) * ch) % MSFFormat::CHECK_SUM_MOD;
    }
    return sum;
}

struct MsfRow {
    MsfRow()
        : checksum(0), length(0) {
    }

    QString name;
    int checksum;
    int length;
};

void MSFFormat::load(IOAdapterReader& reader, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os) {
    QString objName = reader.getURL().baseFileName();
    MultipleSequenceAlignment al(objName);
    int lineNumber = 0;  // Current line number from the object start. Used for error reporing.

    // Skip comments.
    int checkSum = -1;
    while (!os.isCoR() && checkSum < 0 && !reader.atEnd()) {
        QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE).simplified();
        lineNumber++;
        CHECK_OP(os, );
        if (line.endsWith(END_OF_HEADER_LINE)) {
            bool ok;
            checkSum = parseField(line, CHECK_FIELD).toInt(&ok);
            if (!ok || checkSum < 0) {
                checkSum = CHECK_SUM_MOD;
            }
        }
        os.setProgress(reader.getProgress());
    }

    // Read MSF structure.
    int sum = 0;
    QList<MsfRow> msfRows;

    while (!os.isCoR() && !reader.atEnd()) {
        QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE).simplified();
        lineNumber++;
        CHECK_OP(os, );
        if (line.startsWith(SECTION_SEPARATOR)) {
            break;
        }

        bool ok = false;
        QString name = parseField(line, NAME_FIELD);
        if (name.isEmpty()) {
            continue;
        }
        int check = parseField(line, CHECK_FIELD).toInt(&ok);
        if (!ok || check < 0) {
            sum = check = CHECK_SUM_MOD;
        }

        MsfRow row;
        row.name = name;
        row.checksum = check;
        msfRows << row;
        al->addRow(name, QByteArray());
        if (sum < CHECK_SUM_MOD) {
            sum = (sum + check) % CHECK_SUM_MOD;
        }

        os.setProgress(reader.getProgress());
    }
    if (checkSum < CHECK_SUM_MOD && sum < CHECK_SUM_MOD && sum != checkSum) {
        coreLog.info(tr("File check sum is incorrect: expected value: %1, current value %2").arg(checkSum).arg(sum));
    }

    // Read sequences.
    QRegExp coordsRegexp("^\\d+(\\s+\\d+)?$");
    int maRowIndex = 0;
    bool prevLineIsEmpty = false;
    while (!os.isCoR() && !reader.atEnd()) {
        QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE).trimmed();
        CHECK_OP(os, )
        lineNumber++;

        // Skip empty lines and lines with coordinates.
        // 2 empty lines in a row or a line with coordinates make a new block: this way we support both
        // MSFs with block coordinates and without (blocks separated by 2-empty lines only).
        bool isCoordsRegexMatched = coordsRegexp.indexIn(line) != -1;
        if (line.isEmpty() || isCoordsRegexMatched) {
            if (isCoordsRegexMatched || prevLineIsEmpty) {
                maRowIndex = 0;
            }
            prevLineIsEmpty = line.isEmpty();
            continue;
        }
        CHECK_EXT(maRowIndex < msfRows.length(), os.setError(tr("MSF: too many rows in the block, line: %1").arg(QString::number(lineNumber))), );

        int nameAndValueSeparatorIndex = line.indexOf(" ");
        CHECK_EXT(nameAndValueSeparatorIndex >= 0, os.setError(tr("MSF: can't find name and value separator spacing, line: %1").arg(QString::number(lineNumber))), );

        QString name = line.mid(0, nameAndValueSeparatorIndex);
        CHECK_EXT(name == msfRows[maRowIndex].name,
                  os.setError(tr("MSF: row names do not match: %1 vs %2, line: %3").arg(msfRows[maRowIndex].name, name, QString::number(lineNumber))), );

        QByteArray value = line.mid(nameAndValueSeparatorIndex + 1).simplified().replace(" ", "").toLatin1();
        al->appendChars(maRowIndex, msfRows[maRowIndex].length, value.constData(), value.length());
        msfRows[maRowIndex].length += value.length();
        maRowIndex++;
        os.setProgress(reader.getProgress());
    }

    // checksum
    U2OpStatus2Log seqCheckOs;
    const int numRows = al->getRowCount();
    for (int i = 0; i < numRows; i++) {
        const MultipleSequenceAlignmentRow row = al->getMsaRow(i);
        const int expectedCheckSum = msfRows[i].checksum;
        const int sequenceCheckSum = getCheckSum(row->toByteArray(seqCheckOs, al->getLength()));
        if (expectedCheckSum < CHECK_SUM_MOD && sequenceCheckSum != expectedCheckSum) {
            coreLog.info(tr("Unexpected check sum in the row number %1, name: %2; expected value: %3, current value %4").arg(i + 1).arg(row->getName()).arg(expectedCheckSum).arg(sequenceCheckSum));
        }
        al->replaceChars(i, '.', U2Msa::GAP_CHAR);
        al->replaceChars(i, '~', U2Msa::GAP_CHAR);
    }

    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet() != nullptr, os.setError(MSFFormat::tr("Alphabet unknown")), );

    QString folder = hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, );
    objects.append(obj);
}

Document* MSFFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QList<GObject*> objs;
    load(reader, dbiRef, objs, fs, os);

    CHECK_OP_EXT(os, qDeleteAll(objs), nullptr);
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objs, fs);
}

void MSFFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    CHECK_OP(os, );
    QMap<GObjectType, QList<GObject*>> objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = document->getObjects();
    storeTextEntry(writer, objectsMap, os);
}

static const QString SUFFIX_SEPARATOR = "_";

static void splitCompleteName(const QString& completeName, QString& baseName, QString& suffix) {
    const int separatorIndex = completeName.lastIndexOf(SUFFIX_SEPARATOR);
    if (separatorIndex == -1) {
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

static QString increaseSuffix(const QString& completeName) {
    QString baseName;
    QString suffix;
    splitCompleteName(completeName, baseName, suffix);
    if (suffix.isEmpty()) {
        return completeName + SUFFIX_SEPARATOR + QString::number(1);
    }
    return baseName + SUFFIX_SEPARATOR + QString("%1").arg(suffix.toInt() + 1, suffix.length(), 10, QChar('0'));
}

static QString rollRowName(const QString& rowName, const QList<QString>& nonUniqueNames) {
    QString resultName = rowName;
    while (nonUniqueNames.contains(resultName)) {
        resultName = increaseSuffix(resultName);
    }
    return resultName;
}

void MSFFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "MSF entry storing: no alignment", );
    const QList<GObject*>& objectList = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(objectList.size() == 1, "MSFFormat::storeTextEntry can store only 1 object per file. Got: " + QString::number(objectList.size()), );

    auto* obj = dynamic_cast<MultipleSequenceAlignmentObject*>(objectList.first());
    SAFE_POINT(obj != nullptr, "MSF entry storing: the object is not an alignment", );

    const MultipleSequenceAlignment& msa = obj->getMultipleAlignment();

    // Make row names unique
    QMap<qint64, QString> uniqueRowNames;
    int maxNameLen = 0;
    foreach (const MultipleSequenceAlignmentRow& row, msa->getMsaRows()) {
        QString rolledRowName = rollRowName(row->getName().replace(' ', '_'), uniqueRowNames.values());
        uniqueRowNames.insert(row->getRowId(), rolledRowName);
        maxNameLen = qMax(maxNameLen, uniqueRowNames.last().length());
    }

    // Precalculate sequence writing params.
    int maLen = msa->getLength();
    int checkSum = 0;
    QMap<qint64, int> checkSums;
    foreach (const MultipleSequenceAlignmentRow& row, msa->getMsaRows()) {
        QByteArray sequence = row->toByteArray(os, maLen).replace(U2Msa::GAP_CHAR, '.');
        int seqCheckSum = getCheckSum(sequence);
        checkSums.insert(row->getRowId(), seqCheckSum);
        checkSum = (checkSum + seqCheckSum) % CHECK_SUM_MOD;
    }
    int maxLengthLen = QString::number(maLen).length();

    // Write the first line.
    QString line = "  " + MSF_FIELD;
    line += " " + QString::number(maLen);
    line += "  " + TYPE_FIELD;
    line += " " + (obj->getAlphabet()->isAmino() ? TYPE_VALUE_PROTEIN : TYPE_VALUE_NUCLEIC);
    line += "  " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    line += "  " + CHECK_FIELD;
    line += " " + QString::number(checkSum);
    line += "  " + END_OF_HEADER_LINE + "\n\n";
    writer.write(os, line);
    CHECK_OP(os, );

    // Write per-row info.
    int maxCheckSumLen = 4;
    foreach (const MultipleSequenceAlignmentRow& row, msa->getMsaRows()) {
        line = " " + NAME_FIELD;
        line += " " + uniqueRowNames[row->getRowId()].leftJustified(maxNameLen + 1);
        line += "  " + LEN_FIELD;
        line += " " + QString("%1").arg(maLen, -maxLengthLen);
        line += "  " + CHECK_FIELD;
        line += " " + QString("%1").arg(checkSums[row->getRowId()], -maxCheckSumLen);
        line += "  " + WEIGHT_FIELD;
        line += " " + QByteArray::number(WEIGHT_VALUE) + "\n";
        writer.write(os, line);
        CHECK_OP(os, );
    }

    writer.write(os, "\n" + SECTION_SEPARATOR + "\n\n");
    CHECK_OP(os, );

    MultipleSequenceAlignmentWalker walker(msa, '.');
    for (int i = 0; !os.isCoR() && i < maLen; i += CHARS_IN_ROW) {
        /* write numbers */ {
            line = QString(maxNameLen + 2, ' ');
            QString t = QString("%1").arg(i + 1);
            QString s = QString("%1").arg(i + CHARS_IN_ROW < maLen ? i + CHARS_IN_ROW : maLen);
            int r = maLen - i < CHARS_IN_ROW ? maLen % CHARS_IN_ROW : CHARS_IN_ROW;
            r += (r - 1) / CHARS_IN_WORD - (t.length() + s.length());
            line += t;
            if (r > 0) {
                line += QString(r, ' ');
                line += s;
            }
            line += '\n';
            writer.write(os, line);
            CHECK_OP(os, );
        }

        // write sequence
        QList<QByteArray> sequenceList = walker.nextData(CHARS_IN_ROW, os);
        CHECK_OP(os, );
        QList<QByteArray>::ConstIterator si = sequenceList.constBegin();
        QList<MultipleSequenceAlignmentRow> msaRowList = msa->getMsaRows();
        QList<MultipleSequenceAlignmentRow>::ConstIterator ri = msaRowList.constBegin();
        for (; si != sequenceList.constEnd(); si++, ri++) {
            const MultipleSequenceAlignmentRow& row = *ri;
            QString rowName = uniqueRowNames[row->getRowId()].leftJustified(maxNameLen + 1);
            for (int j = 0; j < CHARS_IN_ROW && i + j < maLen; j += CHARS_IN_WORD) {
                rowName += ' ';
                int nChars = qMin(CHARS_IN_WORD, maLen - (i + j));
                QString sequencePart = si->mid(j, nChars);
                rowName += sequencePart;
            }
            rowName += '\n';
            writer.write(os, rowName);
            CHECK_OP(os, );
        }
        writer.write(os, "\n");
        CHECK_OP(os, );
    }
}

FormatCheckResult MSFFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    if (dataPrefix.contains("MSF:") ||
        dataPrefix.contains("!!AA_MULTIPLE_ALIGNMENT 1.0") ||
        dataPrefix.contains("!!NA_MULTIPLE_ALIGNMENT 1.0") ||
        (dataPrefix.contains("Name:") && dataPrefix.contains("Len:") &&
         dataPrefix.contains("Check:") && dataPrefix.contains("Weight:"))) {
        return FormatDetection_VeryHighSimilarity;
    }

    if (dataPrefix.contains("GDC ")) {
        return FormatDetection_AverageSimilarity;
    }

    // MSF documents may contain unlimited number of comment lines in header ->
    // it is impossible to determine if file has MSF format by some predefined
    // amount of raw data read from it.
    if (dataPrefix.contains("GCG ") || dataPrefix.contains("MSF ")) {
        return FormatDetection_LowSimilarity;
    }
    return FormatDetection_NotMatched;
}

}  // namespace U2
