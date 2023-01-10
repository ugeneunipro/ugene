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

#include "PhylipFormat.h"

#include <QTextStream>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

static MultipleSequenceAlignmentObject* getMsaObjectToStore(const QMap<GObjectType, QList<GObject*>>& objectsMap) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "PHYLIP entry storing: no alignment", nullptr);
    const QList<GObject*>& alignmentObjects = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(alignmentObjects.size() == 1, "PHYLIP entry storing: alignment objects count error", nullptr);

    auto msaObject = dynamic_cast<MultipleSequenceAlignmentObject*>(alignmentObjects.first());
    SAFE_POINT(msaObject != nullptr, "PHYLIP entry storing: no alignment object is found", nullptr);
    return msaObject;
}

/** Sequence characters per line. Used by 'storeTextEntry'. */
static constexpr int PHYLIP_SEQUENCE_CHARS_PER_LINE = 80;

/** Number of columns reserved for the name.*/
static constexpr int PHYLIP_NAME_COLUMNS_COUNT = 10;

/** Max supported line length by UGENE. Used when reading Phylip files. */
static constexpr int PHYLIP_MAX_SUPPORTED_LINE_LENGTH = 1000 * 1000;

static void writeSequenceName(IOAdapterWriter& writer, const QString& sequenceName, U2OpStatus& os) {
    QString nameToWrite = sequenceName;
    if (nameToWrite.length() > PHYLIP_NAME_COLUMNS_COUNT) {
        nameToWrite = nameToWrite.left(PHYLIP_NAME_COLUMNS_COUNT);
    }
    writer.write(os, nameToWrite + TextUtils::getLineOfSpaces(PHYLIP_NAME_COLUMNS_COUNT - nameToWrite.length()));
}

/** Returns number of non-space (' ') characters in the 'text'. */
static int countNonSpaceChars(const QString& text) {
    return text.length() - text.count(' ');
}

/** Checks that MSA has expected sequence and column counts. Sets error into 'os' if not. */
static void validateMsaByHeaderData(const MultipleSequenceAlignment& msa, int sequenceCountInHeader, int columnCountInHeader, U2OpStatus& os) {
    CHECK_EXT(msa->getRowCount() == sequenceCountInHeader,
              os.setError(PhylipFormat::tr("Wrong row count. Header: %1, actual: %2").arg(sequenceCountInHeader).arg(msa->getRowCount())), );
    CHECK_EXT(msa->getLength() == columnCountInHeader,
              os.setError(PhylipFormat::tr("Wrong column count. Header: %1, actual: %2").arg(columnCountInHeader).arg(msa->getLength())), );
}

PhylipFormat::PhylipFormat(QObject* p, const DocumentFormatId& id)
    : TextDocumentFormat(p, id, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject, {"phy", "ph"}) {
    formatDescription = tr("PHYLIP multiple alignment format for phylogenetic applications.");
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

void PhylipFormat::storeTextDocument(IOAdapterWriter& writer, Document* doc, U2OpStatus& os) {
    CHECK_EXT(doc->getObjects().size() == 1, os.setError(tr("Incorrect number of objects in document: %1").arg(doc->getObjects().size())), );

    auto obj = qobject_cast<MultipleSequenceAlignmentObject*>(doc->getObjects().first());
    CHECK_EXT(obj != nullptr, os.setError(L10N::internalError("No MSA object in document")), );

    QList<GObject*> als = {obj};
    QMap<GObjectType, QList<GObject*>> objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = als;
    storeTextEntry(writer, objectsMap, os);
    CHECK_EXT(!os.isCoR(), os.setError(L10N::errorWritingFile(doc->getURL())), );
}

MultipleSequenceAlignmentObject* PhylipFormat::load(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    MultipleSequenceAlignment msa = parse(reader, os);
    CHECK_OP(os, nullptr);
    MSAUtils::checkPackedModelSymmetry(msa, os);
    CHECK_OP(os, nullptr);

    U2AlphabetUtils::assignAlphabet(msa);
    CHECK_EXT(msa->getAlphabet() != nullptr, os.setError(tr("Alphabet is unknown")), nullptr);

    QString folder = hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    auto msaObject = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, msa, os);
    CHECK_OP(os, nullptr);
    return msaObject;
}

bool PhylipFormat::parseHeader(const QString& line, int& sequenceCount, int& columnCount) const {
    QStringList tokens = line.simplified().split(" ");
    CHECK(tokens.length() == 2, false);

    bool parserResult = true;
    sequenceCount = tokens[0].toInt(&parserResult);
    CHECK(parserResult, false);

    columnCount = tokens[1].toInt(&parserResult);
    return parserResult;
}

Document* PhylipFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    auto msaObject = load(reader, dbiRef, hints, os);
    CHECK_OP(os, nullptr);

    QList<GObject*> objects = {msaObject};
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints);
}

PhylipSequentialFormat::PhylipSequentialFormat(QObject* p)
    : PhylipFormat(p, BaseDocumentFormats::PHYLIP_SEQUENTIAL) {
    formatName = tr("PHYLIP Sequential");
}

void PhylipSequentialFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    auto msaObject = getMsaObjectToStore(objectsMap);
    CHECK_EXT(msaObject != nullptr, os.setError(PhylipFormat::tr("Failed to find MSA object to store")), );
    const MultipleSequenceAlignment& msa = msaObject->getMultipleAlignment();

    // Write header.
    int sequenceCount = msa->getRowCount();
    int columnCount = msa->getLength();
    QString headerLine = QString::number(sequenceCount) + " " + QString::number(columnCount) + "\n";
    writer.write(os, headerLine);
    CHECK_OP(os, );
    CHECK(sequenceCount > 0, );

    // Write sequences.
    for (int sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++) {
        if (sequenceIndex > 0) {
            writer.write(os, "\n");  // Blocks separator empty line.
            CHECK_OP(os, );
        }
        auto row = msa->getMsaRow(sequenceIndex);
        writeSequenceName(writer, row->getName(), os);
        CHECK_OP(os, );
        QByteArray sequence = row->toByteArray(os, columnCount);
        for (int pos = 0; pos < sequence.length(); pos += PHYLIP_SEQUENCE_CHARS_PER_LINE) {
            QByteArray line = sequence.mid(pos, qMin(sequence.length() - pos, PHYLIP_SEQUENCE_CHARS_PER_LINE));
            writer.write(os, line);
            CHECK_OP(os, );
            writer.write(os, "\n");
            CHECK_OP(os, );
        }
    }
}

FormatCheckResult PhylipSequentialFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QString dataPrefixCopyForTextStream = dataPrefix;  // Make a copy. QTextStream requires a writable copy.
    QTextStream textStream(&dataPrefixCopyForTextStream);
    QString line = textStream.readLine(PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
    int sequenceCountInHeader = 0;
    int columnCountInHeader = 0;
    if (!parseHeader(line, sequenceCountInHeader, columnCountInHeader)) {
        return FormatDetection_NotMatched;
    }
    // Check that data prefix has no more than expected sequence count and each of the sequences has an expected length.
    int readColumnCount = 0;
    int readSequenceCount = 0;
    bool isMultiLineSequence = false;
    for (; readSequenceCount < sequenceCountInHeader && !textStream.atEnd();) {
        CHECK(readSequenceCount < sequenceCountInHeader, FormatDetection_NotMatched);
        line = textStream.readLine(PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
        CHECK_CONTINUE(!TextUtils::isWhiteSpace(line));  // Allow empty lines between blocks.
        CHECK(line.length() > PHYLIP_NAME_COLUMNS_COUNT, FormatDetection_NotMatched);
        readColumnCount += countNonSpaceChars(line.mid(PHYLIP_NAME_COLUMNS_COUNT));
        while (readColumnCount < columnCountInHeader) {
            isMultiLineSequence = true;
            line = textStream.readLine(PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
            CHECK(!TextUtils::isWhiteSpace(line), FormatDetection_NotMatched);
            readColumnCount += countNonSpaceChars(line);
            CHECK(readColumnCount <= columnCountInHeader, FormatDetection_NotMatched);
            if (readColumnCount == columnCountInHeader) {
                break;
            }
        }
        readSequenceCount++;
    }
    return readSequenceCount == sequenceCountInHeader
               // Return 'Matched' only for multiline. Otherwise, give a higher priority to the interleaved version of the Phylip format.
               ? (isMultiLineSequence ? FormatDetection_Matched : FormatDetection_VeryHighSimilarity)
               : FormatDetection_HighSimilarity;
}

MultipleSequenceAlignment PhylipSequentialFormat::parse(IOAdapterReader& reader, U2OpStatus& os) const {
    QString msaName = reader.getURL().baseFileName();
    MultipleSequenceAlignment msa(msaName);

    QString firstLine = reader.readLine(os, PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
    CHECK_OP(os, {});

    int sequenceCountInHeader = 0;
    int columnCountInHeader = 0;
    CHECK_EXT(parseHeader(firstLine, sequenceCountInHeader, columnCountInHeader), os.setError(PhylipFormat::tr("Failed to parse header line")), {});

    for (int sequenceIndex = 0; sequenceIndex < sequenceCountInHeader;) {
        CHECK_EXT(!reader.atEnd(), os.setError(PhylipFormat::tr("Unexpected end of data in Phylip file")), {});

        // Read name.
        QString line = reader.readLine(os, PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
        CHECK_OP(os, {});
        CHECK_CONTINUE(!TextUtils::isWhiteSpace(line));  // Allow empty lines between different sequences.
        CHECK_EXT(line.length() > PHYLIP_NAME_COLUMNS_COUNT, os.setError(PhylipFormat::tr("Line with a name is too short %1").arg(line)), {});
        QString name = line.left(PHYLIP_NAME_COLUMNS_COUNT).trimmed();

        // Read sequence.
        QByteArray sequence = line.mid(PHYLIP_NAME_COLUMNS_COUNT).replace(" ", "").toLatin1();
        while (sequence.length() != columnCountInHeader) {
            CHECK_EXT(!reader.atEnd(), os.setError(PhylipFormat::tr("Unexpected end of file")), {});
            line = reader.readLine(os, PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
            CHECK_OP(os, {});
            sequence += line.replace(" ", "").toLatin1();
        }
        msa->addRow(name, sequence);
        os.setProgress(reader.getProgress());
        sequenceIndex++;
    }
    validateMsaByHeaderData(msa, sequenceCountInHeader, columnCountInHeader, os);
    CHECK_OP(os, {});
    return msa;
}

PhylipInterleavedFormat::PhylipInterleavedFormat(QObject* p)
    : PhylipFormat(p, BaseDocumentFormats::PHYLIP_INTERLEAVED) {
    formatName = tr("PHYLIP Interleaved");
}

void PhylipInterleavedFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    auto msaObject = getMsaObjectToStore(objectsMap);
    CHECK_EXT(msaObject != nullptr, os.setError(PhylipFormat::tr("Failed to find MSA object to store")), );
    const MultipleSequenceAlignment& msa = msaObject->getMultipleAlignment();

    // Write header.
    int sequenceCount = msa->getRowCount();
    int columnCount = msa->getLength();
    QString headerLine = QString::number(sequenceCount) + " " + QString::number(columnCount) + "\n";
    writer.write(os, headerLine);
    CHECK_OP(os, );
    CHECK(sequenceCount > 0, );

    // Write sequences.
    QList<QByteArray> sequenceList;
    int pos = 0;
    do {
        if (pos > 0) {
            writer.write(os, "\n");  // Blocks separator empty line.
            CHECK_OP(os, );
        }
        for (int sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++) {
            auto row = msa->getMsaRow(sequenceIndex);
            if (pos == 0) {
                writeSequenceName(writer, row->getName(), os);
                CHECK_OP(os, );
                sequenceList << row->toByteArray(os, columnCount);
                CHECK_OP(os, );
            }
            QByteArray sequence = sequenceList[sequenceIndex];
            QByteArray line = sequence.mid(pos, qMin(sequence.length() - pos, PHYLIP_SEQUENCE_CHARS_PER_LINE));
            writer.write(os, line + "\n");
            CHECK_OP(os, );
        }
        pos += PHYLIP_SEQUENCE_CHARS_PER_LINE;
    } while (pos < columnCount);
}

FormatCheckResult PhylipInterleavedFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QString dataPrefixCopyForTextStream = dataPrefix;  // Make a copy. QTextStream requires a writable copy.
    QTextStream textStream(&dataPrefixCopyForTextStream);
    QString line = textStream.readLine(PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
    int sequenceCountInHeader = 0;
    int columnCountInHeader = 0;
    if (!parseHeader(line, sequenceCountInHeader, columnCountInHeader)) {
        return FormatDetection_NotMatched;
    }
    // Check that first 'sequenceCountInHeader' lines contain unique names and all lines have equal sequence lengths within their block.
    int readLineCount = 0;
    int readColumnCount = 0;
    QSet<QString> nameSet;
    int sequenceLengthInBlock = -1;
    while (true) {
        bool isFirstBlock = readLineCount < sequenceCountInHeader;
        bool isFirstLineInBlock = readLineCount % sequenceCountInHeader == 0;

        CHECK(!textStream.atEnd(),
              isFirstBlock
                  ? FormatDetection_AverageSimilarity
              : isFirstLineInBlock && readColumnCount == columnCountInHeader
                  ? FormatDetection_Matched
                  : FormatDetection_HighSimilarity);

        line = textStream.readLine(PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
        if (TextUtils::isWhiteSpace(line)) {
            CHECK(isFirstLineInBlock, FormatDetection_NotMatched);
            continue;  // Allow empty lines between blocks.
        }
        if (isFirstBlock) {
            CHECK(line.length() > PHYLIP_NAME_COLUMNS_COUNT, textStream.atEnd() ? FormatDetection_NotMatched : FormatDetection_AverageSimilarity)
            QString name = line.left(PHYLIP_NAME_COLUMNS_COUNT);
            CHECK(!nameSet.contains(name), FormatDetection_NotMatched);
            nameSet.insert(name);
        }
        int sequenceLengthInLine = countNonSpaceChars(line.mid(isFirstBlock ? PHYLIP_NAME_COLUMNS_COUNT : 0));
        if (isFirstLineInBlock) {
            sequenceLengthInBlock = sequenceLengthInLine;
            readColumnCount += sequenceLengthInBlock;
            CHECK(readColumnCount <= columnCountInHeader, FormatDetection_NotMatched);
        } else {
            CHECK(sequenceLengthInLine == sequenceLengthInBlock, FormatDetection_NotMatched);
        }
        readLineCount++;
    }
}

MultipleSequenceAlignment PhylipInterleavedFormat::parse(IOAdapterReader& reader, U2OpStatus& os) const {
    QString msaName = reader.getURL().baseFileName();
    MultipleSequenceAlignment msa(msaName);

    QString firstLine = reader.readLine(os, PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
    CHECK_OP(os, {});

    int sequenceCountInHeader = 0;
    int columnCountInHeader = 0;
    CHECK_EXT(parseHeader(firstLine, sequenceCountInHeader, columnCountInHeader), os.setError(PhylipFormat::tr("Failed to parse header line")), {});

    // Read the first block with names.
    for (int sequenceIndex = 0; sequenceIndex < sequenceCountInHeader && !reader.atEnd(); sequenceIndex++) {
        QString line = reader.readLine(os, PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
        CHECK_OP(os, {});
        CHECK_EXT(line.length() > PHYLIP_NAME_COLUMNS_COUNT, os.setError(PhylipFormat::tr("Line with a name is too short %1").arg(line)), {});
        QString name = line.left(PHYLIP_NAME_COLUMNS_COUNT).trimmed();
        QByteArray sequence = line.mid(PHYLIP_NAME_COLUMNS_COUNT).replace(" ", "").toLatin1();
        msa->addRow(name, sequence);
    }
    os.setProgress(reader.getProgress());

    // Sequence blocks.
    while (!reader.atEnd()) {
        int msaLength = msa->getLength();
        for (int sequenceIndex = 0; sequenceIndex < sequenceCountInHeader && !reader.atEnd();) {
            QString line = reader.readLine(os, PHYLIP_MAX_SUPPORTED_LINE_LENGTH);
            CHECK_OP(os, {});
            CHECK_CONTINUE(!TextUtils::isWhiteSpace(line) || sequenceIndex > 0);  // Allow empty lines only before the first block.
            QByteArray sequence = line.replace(" ", "").toLatin1();
            msa->appendChars(sequenceIndex, msaLength, sequence.constData(), sequence.length());
            sequenceIndex++;
        }
        os.setProgress(reader.getProgress());
    }
    validateMsaByHeaderData(msa, sequenceCountInHeader, columnCountInHeader, os);
    CHECK_OP(os, {});
    return msa;
}

}  // namespace U2
