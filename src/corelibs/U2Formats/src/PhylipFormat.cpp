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

#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusUtils.h>

#include "PhylipFormat.h"

namespace U2 {

// PhylipFormat
PhylipFormat::PhylipFormat(QObject *p, const DocumentFormatId& id)
    : TextDocumentFormat(p, id, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject,
                     QStringList() << "phy" << "ph"){
    formatDescription = tr("PHYLIP multiple alignment format for phylogenetic applications.");
    supportedObjectTypes+=GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

void PhylipFormat::storeDocument(Document *d, IOAdapter *io, U2OpStatus &os) {
    CHECK_EXT(d!=NULL, os.setError(L10N::badArgument("doc")), );
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), );

    MultipleSequenceAlignmentObject *obj = NULL;
    CHECK_EXT(d->getObjects().size() == 1, os.setError("Incorrect number of objects in document"), );
    CHECK_EXT((obj = qobject_cast<MultipleSequenceAlignmentObject*>(d->getObjects().first())) != NULL, os.setError("No data to write"), );

    QList<GObject*> als;
    als << obj;
    QMap< GObjectType, QList<GObject*> > objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = als;
    storeEntry(io, objectsMap, os);
    CHECK_EXT(!os.isCoR(), os.setError(L10N::errorWritingFile(d->getURL())), );
}

MultipleSequenceAlignmentObject* PhylipFormat::load(IOAdapter *io, const U2DbiRef &dbiRef, const QVariantMap& fs, U2OpStatus &os) {
    SAFE_POINT(io != NULL, "IO adapter is NULL!", NULL);

    MultipleSequenceAlignment al = parse(io, os);
    CHECK_OP(os, NULL);
    MSAUtils::checkPackedModelSymmetry(al, os);
    CHECK_OP(os, NULL);

    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet()!=NULL, os.setError( PhylipFormat::tr("Alphabet is unknown")), NULL);

    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, NULL);
    return obj;
}

bool PhylipFormat::parseHeader(QByteArray data, int &species, int &characters) const {
    QTextStream stream(data);
    stream >> species >> characters;
    if ((species == 0) && (characters == 0)) {
        return false;
    }
    return true;
}

void PhylipFormat::removeSpaces(QByteArray &data) const {
    while (data.contains(' ')) {
        data.remove(data.indexOf(' '), 1);
    }
}

Document* PhylipFormat::loadTextDocument(IOAdapter *io, const U2DbiRef &dbiRef, const QVariantMap &fs, U2OpStatus &os) {
    SAFE_POINT(io != NULL, "IO adapter is NULL!", NULL);
    QList<GObject*> objects;
    objects.append( load(io, dbiRef, fs, os) );
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);
    assert(objects.size() == 1);
    return new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
}


#define MAX_NAME_LEN    10  // max name length for phylip format is 10

#define SEQ_BLOCK_SIZE  100
#define INT_BLOCK_SIZE  50

const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;

// PhylipSequentialFormat
PhylipSequentialFormat::PhylipSequentialFormat(QObject *p)
    : PhylipFormat(p, BaseDocumentFormats::PHYLIP_SEQUENTIAL) {
    formatName = tr("PHYLIP Sequential");
}

void PhylipSequentialFormat::storeEntry(IOAdapter *io, const QMap<GObjectType, QList<GObject *> > &objectsMap, U2OpStatus &os) {
    SAFE_POINT(io != NULL, "IO adapter is NULL!", );
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "PHYLIP entry storing: no alignment", );
    const QList<GObject*> &als = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(1 == als.size(), "PHYLIP entry storing: alignment objects count error", );

    const MultipleSequenceAlignmentObject* obj = dynamic_cast<MultipleSequenceAlignmentObject*>(als.first());
    SAFE_POINT(NULL != obj, "PHYLIP entry storing: NULL alignment object", );

    const MultipleSequenceAlignment msa = obj->getMultipleAlignment();

    //write header
    int numberOfSpecies = msa->getNumRows();
    int numberOfCharacters = msa->getLength();
    QByteArray header( (QString::number(numberOfSpecies) + " " + QString::number(numberOfCharacters)).toLatin1() + "\n");
    int len = io->writeBlock(header);
    CHECK_EXT(len == header.length(), os.setError(L10N::errorTitle()), );

    //write sequences
    for (int i = 0; i < numberOfSpecies; i++) {
        QByteArray line = msa->getMsaRow(i)->getName().toLatin1();
        if (line.length() < MAX_NAME_LEN) {
            int difference = MAX_NAME_LEN - line.length();
            for (int j = 0; j < difference; j++) {
                line.append(" ");
            }
        }
        if (line.length() > MAX_NAME_LEN) {
            line = line.left(MAX_NAME_LEN);
        }
        io->writeBlock(line);
        QByteArray sequence = msa->getMsaRow(i)->toByteArray(os, numberOfCharacters);
        int blockCounter = 0;
        while ((blockCounter*SEQ_BLOCK_SIZE) <= numberOfCharacters) {
            line.clear();
            line.append(sequence.mid(blockCounter*SEQ_BLOCK_SIZE, SEQ_BLOCK_SIZE));
            line.append('\n');
            io->writeBlock(line);
            blockCounter++;
        }
    }
}

FormatCheckResult PhylipSequentialFormat::checkRawTextData(const QByteArray &rawData, const GUrl &) const {
    if (TextUtils::contains(TextUtils::BINARY, rawData.constData(), rawData.size())) {
        return FormatDetection_NotMatched;
    }
    int species = 0, characters = 0;
    if (!parseHeader(rawData, species, characters)) {
        return FormatDetection_NotMatched;
    }
    QTextStream s(rawData);
    for (int i = 0; i  < species + 1; i++) {
        if (s.atEnd()) {
            return FormatDetection_AverageSimilarity;
        }
        s.readLine();
    }
    // if line after row names is not empty and contains characters at the beginning,
    // it is more probably a sequential phylip example
    QString line = s.readLine();
    if ((line.size() != 0) && (line.at(0) != ' ')) {
        return FormatDetection_Matched;
    }

    return FormatDetection_AverageSimilarity;
}

MultipleSequenceAlignment PhylipSequentialFormat::parse(IOAdapter *io, U2OpStatus &os) const {
    SAFE_POINT(io != NULL, "IO adapter is NULL!", MultipleSequenceAlignment());
    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff = readBuffer.data();
    QString objName = io->getURL().baseFileName();
    MultipleSequenceAlignment al(objName);
    bool resOk = false;

    // Header: "<number of species> <number of characters>"
    int len = io->readLine(buff, READ_BUFF_SIZE, &resOk);
    CHECK_EXT(len != 0, os.setError(PhylipSequentialFormat::tr("Error parsing file")), MultipleSequenceAlignment());
    CHECK_EXT(resOk, os.setError( PhylipSequentialFormat::tr("Illegal line")), MultipleSequenceAlignment());

    QByteArray line = QByteArray(buff, len).trimmed();

    int numberOfSpecies = 0;
    int numberOfCharacters = 0;
    resOk = parseHeader(line, numberOfSpecies, numberOfCharacters);
    CHECK_EXT(resOk, os.setError( PhylipSequentialFormat::tr("Wrong header") ), MultipleSequenceAlignment());

    for (int i = 0; i < numberOfSpecies; i++) {
        CHECK_EXT(!io->isEof(), os.setError( PhylipSequentialFormat::tr("There is not enough data")), MultipleSequenceAlignment());
        // get name
        len = io->readBlock(buff, MAX_NAME_LEN);
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());
        CHECK_EXT(len != 0, os.setError(PhylipSequentialFormat::tr("Error parsing file")), MultipleSequenceAlignment());

        QByteArray name = QByteArray(buff, len).trimmed();
        // get sequence
        QByteArray value;
        while ((value.size() != numberOfCharacters) && (!io->isEof())) {
            len = io->readUntil(buff, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Skip, &resOk);
            CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());

            QByteArray line = QByteArray(buff, len);
            removeSpaces(line);
            value.append(line);
        }
        al->addRow(name, value);

        os.setProgress(io->getProgress());
    }
    CHECK_EXT(al->getLength() == numberOfCharacters, os.setError( PhylipSequentialFormat::tr("Number of characters does not correspond to the stated number") ),
               MultipleSequenceAlignment());
    return al;
}


// PhylipInterleavedFormat
PhylipInterleavedFormat::PhylipInterleavedFormat(QObject *p)
    :PhylipFormat(p, BaseDocumentFormats::PHYLIP_INTERLEAVED) {
    formatName = tr("PHYLIP Interleaved");
}

void PhylipInterleavedFormat::storeEntry(IOAdapter *io, const QMap<GObjectType, QList<GObject *> > &objectsMap, U2OpStatus &os) {
    SAFE_POINT(io != NULL, "IO adapter is NULL!", );
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "PHYLIP entry storing: no alignment", );
    const QList<GObject*> &als = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(1 == als.size(), "PHYLIP entry storing: alignment objects count error", );

    const MultipleSequenceAlignmentObject* obj = dynamic_cast<MultipleSequenceAlignmentObject*>(als.first());
    SAFE_POINT(NULL != obj, "PHYLIP entry storing: NULL alignment object", );

    const MultipleSequenceAlignment msa = obj->getMultipleAlignment();

    //write header
    int numberOfSpecies = msa->getNumRows();
    int numberOfCharacters = msa->getLength();
    QByteArray header( (QString::number(numberOfSpecies) + " " + QString::number(numberOfCharacters)).toLatin1() + "\n");
    int len = io->writeBlock(header);

    CHECK_EXT(len == header.length(), os.setError(L10N::errorTitle()), );

    //write first block with names
    for (int i = 0; i < numberOfSpecies; i++) {
        QByteArray line = msa->getMsaRow(i)->getName().toLatin1();
        if (line.length() < MAX_NAME_LEN) {
            int difference = MAX_NAME_LEN - line.length();
            for (int j = 0; j < difference; j++)
                line.append(" ");
        }
        if (line.length() > MAX_NAME_LEN) {
            line = line.left(MAX_NAME_LEN);
        }

        QByteArray sequence = msa->getMsaRow(i)->toByteArray(os, numberOfCharacters);
        line.append(sequence.left(INT_BLOCK_SIZE));
        line.append('\n');

        io->writeBlock(line);
    }

    //write sequence blockss
    int blockCounter = 1;
    QByteArray spacer(MAX_NAME_LEN, ' ');
    while (blockCounter*INT_BLOCK_SIZE <= numberOfCharacters) {
        io->writeBlock("\n", 1);
        for (int i = 0; i < numberOfSpecies; i++) {
            QByteArray sequence = msa->getMsaRow(i)->toByteArray(os, numberOfCharacters);
            QByteArray line;
            line.append(spacer);
            line.append(sequence.mid(blockCounter*INT_BLOCK_SIZE, INT_BLOCK_SIZE));
            line.append('\n');

            io->writeBlock(line, line.size());
        }
        blockCounter++;
    }
}

FormatCheckResult PhylipInterleavedFormat::checkRawTextData(const QByteArray &rawData, const GUrl &) const {
    if (TextUtils::contains(TextUtils::BINARY, rawData.constData(), rawData.size())) {
        return FormatDetection_NotMatched;
    }
    int species, characters;
    if (!parseHeader(rawData, species, characters)) {
        return FormatDetection_NotMatched;
    }

    QTextStream s(rawData);
    for (int i = 0; i  < species + 1; i++) {
        if (s.atEnd()) {
            return FormatDetection_AverageSimilarity;
        }
        s.readLine();
    }
    // if line after row names is empty or contains spaces at the beginning,
    // it is more probably an interleaved phylip example
    QString line = s.readLine();
    if (((line.size() != 0) && (line.at(0) == ' '))
            || (line.isEmpty())) {
        return FormatDetection_Matched;
    }

    return FormatDetection_AverageSimilarity;
}

MultipleSequenceAlignment PhylipInterleavedFormat::parse(IOAdapter *io, U2OpStatus &os) const {
    SAFE_POINT(io != NULL, "IO adapter is NULL!", MultipleSequenceAlignment());

    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff = readBuffer.data();
    QString objName = io->getURL().baseFileName();
    MultipleSequenceAlignment al(objName);

    bool resOk = false;

    // First line: "<number of species> <number of characters>"
    int len = io->readLine(buff, READ_BUFF_SIZE, &resOk);
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());
    CHECK_EXT(resOk, os.setError( PhylipInterleavedFormat::tr("Illegal line") ), MultipleSequenceAlignment());

    QByteArray line = QByteArray(buff, len).trimmed();

    int numberOfSpecies;
    int numberOfCharacters;
    resOk = parseHeader(line, numberOfSpecies, numberOfCharacters);
    CHECK_EXT(resOk,  os.setError( PhylipInterleavedFormat::tr("Wrong header") ), MultipleSequenceAlignment());

    //the first block with the names
    for (int i = 0; i < numberOfSpecies; i++) {
        CHECK_EXT(!io->isEof(), os.setError( PhylipSequentialFormat::tr("There is not enough data")), MultipleSequenceAlignment());
        len = io->readBlock(buff, MAX_NAME_LEN);
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());
        CHECK_EXT(len != 0, os.setError( PhylipFormat::tr("Error parsing file") ), MultipleSequenceAlignment());

        QByteArray name = QByteArray(buff, len).trimmed();

        QByteArray value;
        do {
            len = io->readUntil(buff, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Skip, &resOk);
            CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());
            CHECK_EXT(len != 0, os.setError(PhylipSequentialFormat::tr("Error parsing file")), MultipleSequenceAlignment());

            value.append(QByteArray(buff, len));
        } while (!resOk);

        removeSpaces(value);
        al->addRow(name, value);

        os.setProgress(io->getProgress());
    }
    int currentLen = al->getLength();

    // sequence blocks
    while (!os.isCoR() && len > 0 && !io->isEof()) {
        int blockSize = -1;
        for (int i = 0; i < numberOfSpecies; i++) {
            QByteArray value;
            do {
                len = io->readUntil(buff, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Skip, &resOk);
                CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());

                value.append(QByteArray(buff, len));
            } while (!resOk && !io->isEof());
            if (value.size() == 0) {
                if (i != 0) {
                    os.setError( PhylipInterleavedFormat::tr("Block is incomplete"));
                }
                break;
            }

            removeSpaces(value);

            al->appendChars(i, currentLen, value.constData(), value.size());
            if (blockSize == -1) {
                blockSize = value.size();
            } else if (blockSize != value.size()) {
                os.setError( PhylipInterleavedFormat::tr("Block is incomlete") );
                break;
            }
        }
        os.setProgress(io->getProgress());
        currentLen += blockSize;
    }
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), MultipleSequenceAlignment());
    CHECK_EXT(al->getLength() == numberOfCharacters, os.setError( PhylipInterleavedFormat::tr("Number of characters does not correspond to the stated number") ),
              MultipleSequenceAlignment());
    return al;
}

} //namespace
