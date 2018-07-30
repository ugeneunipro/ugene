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

#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "AprFormat.h"

namespace U2 {

static const QStringList HEADERS = QStringList()
                            << "|AlignmentProject"
                            << "obj|Project|"
                            << "obj|MolList|"
                            << "obj|Object*|" ;

static const QString AREA_ALIGN = "AlnList";
static const QString AREA_SEQUENCE_QUANTITY = "Object*";
static const QString AREA_SEQUENCE = "IxAlignment";
static const QString NO_SEQUENCES_IN_ALIGNMENT_STRING = "null";
static const short SIZE_BEFORE_NUMBER_ALIGNMENT_LENGTH = QString("obj|AlnList|\\").length();
static const short SIZE_BEFORE_NUMBER_SEQUENCE_QUANTITY = QString("obj|Object*|").length();
static const short SIZE_BEFORE_NUMBER_SEQUENCE_LENGTH = QString("obj|IxAlignment|\\").length();

/**
*returns the string from which the alignment information is started
*/
static QString getLine(IOAdapter* io, char* buff, const QString& pattern, U2OpStatus& os) {
    bool lineOk = false;
    bool finishedReading = false;
    QString line;
    while (!finishedReading) {
        io->readLine(buff, AprFormat::READ_BUFF_SIZE, &lineOk);
        if (!lineOk) {
            os.setError(AprFormat::tr("Unexpected end of file"));
            line = QString();
            finishedReading = true;
        }
        QString bufferString(buff);
        QTextStream bufferStream(&bufferString);
        line = bufferStream.readLine();
        if (line.contains(pattern)) {
            finishedReading = true;
        } else if (line.contains(NO_SEQUENCES_IN_ALIGNMENT_STRING)) {
            os.setError(AprFormat::tr("There is no sequences in alignment"));
            line = QString();
            finishedReading = true;
        }
    }
    return line;
}

static int getNumber(QString string, int startPos, U2OpStatus& os) {
    string = string.simplified();
    int resultLength = 0;
    int currentLength = 0;
    int i = 0;
    bool ok = true;
    int stringSize = string.size();
    int resultLengthSize = 0;
    while (ok && stringSize > startPos + resultLengthSize) {
        resultLength = currentLength;
        resultLengthSize = QString::number(resultLength).size();
        i++;
        QString stringLength = string.mid(startPos, i);
        currentLength = stringLength.toInt(&ok);
    }
    if (i == 1) {
        resultLength = currentLength;
    }
    if (resultLength == 0) {
        os.setError(AprFormat::tr("Attemt to find any number in the string failed"));
    }
    return resultLength;
}

static QString getRowName(QString string, int sequenceLength) {
    string = string.simplified();
    int sequenceLengthSize = QString::number(sequenceLength).size();
    int namePos = SIZE_BEFORE_NUMBER_SEQUENCE_LENGTH + sequenceLengthSize + sequenceLength + 2;
    QString name = string.mid(namePos);
    if (name.startsWith("\\")) {
        const int colonNumber = name.indexOf(':');
        if (colonNumber != -1) {
            bool lengthToInt = false;
            const int nameLength = name.mid(1, colonNumber - 1).toInt(&lengthToInt);
            const QString newName = name.right(name.size() - colonNumber - 1);
            if (lengthToInt && newName.size() == nameLength) {
                name = newName;
            }
        }
    }
    return name;
}

static QByteArray getSequenceContent(QString string, int sequenceLength) {
    string = string.simplified();
    int sequenceLengthSize = QString::number(sequenceLength).size();
    int infoPos = SIZE_BEFORE_NUMBER_SEQUENCE_LENGTH + sequenceLengthSize + 1;
    QString info = string.mid(infoPos, sequenceLength);
    QByteArray byteArrayInfo = info.toUtf8();
    return byteArrayInfo;
}

static void createRows(IOAdapter* io, char* buff, const int sequnenceNum, const int alignmentLength, MultipleSequenceAlignment& al, U2OpStatus& os) {
    for (int i = 0; i < sequnenceNum; i++) {
        QString rowInfo = getLine(io, buff, AREA_SEQUENCE, os);
        CHECK_OP(os, );

        int sequenceLength = getNumber(rowInfo, SIZE_BEFORE_NUMBER_SEQUENCE_LENGTH, os);
        CHECK_OP(os, );
        if (sequenceLength != alignmentLength) {
            os.setError("Incorrect sequence length");
            return;
        }
        QString rowName = getRowName(rowInfo, sequenceLength);
        QByteArray sequenceContent = getSequenceContent(rowInfo, sequenceLength);
        al->addRow(rowName, sequenceContent);
    }
}

AprFormat::AprFormat(QObject* p) : TextDocumentFormat(p, DocumentFormatFlags(DocumentFormatFlag_CannotBeCreated), QStringList("apr")) {
    formatName = tr("Vector NTI/AlignX");
    formatDescription = tr("Vector NTI/AlignX is a Vector NTI format for multiple alignment");
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

FormatCheckResult AprFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    if (TextUtils::contains(TextUtils::BINARY, rawData.constData(), rawData.size())) {
        return FormatDetection_NotMatched;
    }

    QTextStream s(rawData);
    foreach(const QString& header, HEADERS) {
        QString line = s.readLine();
        bool containHeader = line.contains(header);
        if (!containHeader) {
            return FormatDetection_NotMatched;
        }
    }
    return FormatDetection_Matched;
}

QString AprFormat::getRadioButtonText() const {
    return tr("Open in read-only mode");
}

Document* AprFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QList <GObject*> objs;
    load(io, dbiRef, objs, fs, os);

    CHECK_OP_EXT(os, qDeleteAll(objs), NULL);

    if (objs.isEmpty()){
        os.setError(AprFormat::tr("File doesn't contain any msa objects"));
        return NULL;
    }
    Document *doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objs, fs);

    return doc;
}

void AprFormat::load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap &hints, U2OpStatus& os) {
    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff = readBuffer.data();

    QString objName = io->getURL().baseFileName();
    MultipleSequenceAlignment al(objName);
    bool lineOk = false;

    io->readLine(buff, READ_BUFF_SIZE, &lineOk);
    QString bufferString(buff);
    QTextStream bufferStream(&bufferString);
    QString header = bufferStream.readLine();
    QByteArray mainHeader = header.toUtf8();
    if (!lineOk || !readBuffer.startsWith(mainHeader)) {
        os.setError(AprFormat::tr("Illegal header line"));
        return;
    }

    QString alignString = getLine(io, buff, AREA_ALIGN, os);
    CHECK_OP(os, );

    int alignmentLength = getNumber(alignString, SIZE_BEFORE_NUMBER_ALIGNMENT_LENGTH, os);
    CHECK_OP(os, );

    QString sequenceQuantityString = getLine(io, buff, AREA_SEQUENCE_QUANTITY, os);
    CHECK_OP(os, );

    int sequenceNum = getNumber(sequenceQuantityString, SIZE_BEFORE_NUMBER_SEQUENCE_QUANTITY, os);
    CHECK_OP(os, );
    if (sequenceNum == 0) {
        os.setError(AprFormat::tr("Sequences not found"));
        return;
    }

    createRows(io, buff, sequenceNum, alignmentLength, al, os);
    CHECK_OP(os, );

    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet() != NULL, os.setError(AprFormat::tr("Alphabet is unknown")), );

    const QString folder = hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, );
    objects.append(obj);
}

} //namespace
