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
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/DocumentFormatUtils.h>

#include "MegaFormat.h"

namespace U2 {

const QByteArray MegaFormat::MEGA_HEADER="mega";
const QByteArray MegaFormat::MEGA_UGENE_TITLE="!Title Ugene Mega;\n\n";
const QByteArray MegaFormat::MEGA_TITLE="TITLE";
const char MegaFormat::MEGA_SEPARATOR='#';
const char MegaFormat::MEGA_IDENTICAL='.';
const char MegaFormat::MEGA_INDEL='-';
const char MegaFormat::MEGA_START_COMMENT='!';
const char MegaFormat::MEGA_END_COMMENT=';';

MegaFormat::MegaFormat(QObject* p) : TextDocumentFormat(p, BaseDocumentFormats::MEGA, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject, QStringList("meg")) {
    formatName = tr("Mega");
    formatDescription = tr("Mega is a file format of native MEGA program");
    supportedObjectTypes+=GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

Document* MegaFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){
    QList<GObject*> objs;
    load(io, dbiRef, objs, fs, os);
    CHECK_OP_EXT(os, qDeleteAll(objs), NULL);
    return new Document(this, io->getFactory(), io->getURL(), dbiRef, objs, fs);
}

void MegaFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    CHECK_EXT(d!=NULL, os.setError(L10N::badArgument("doc")), );
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), );

    MultipleSequenceAlignmentObject* obj = NULL;
    if( (d->getObjects().size() != 1)
        || ((obj = qobject_cast<MultipleSequenceAlignmentObject*>(d->getObjects().first())) == NULL)) {
            os.setError("No data to write;");
            return;
    }

    QList<GObject*> als; als << obj;
    QMap< GObjectType, QList<GObject*> > objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = als;
    storeEntry(io, objectsMap, os);
    CHECK_EXT(!os.isCoR(), os.setError(L10N::errorWritingFile(d->getURL())), );
}

FormatCheckResult MegaFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    QByteArray line=rawData.trimmed();

    if (!line.startsWith(MEGA_SEPARATOR)) {
        return FormatDetection_NotMatched;
    }
    line=line.mid(1);
    line=line.trimmed();
    if (!line.startsWith(MEGA_HEADER)) {
        return FormatDetection_NotMatched;
    }
    return FormatDetection_Matched;
}

bool MegaFormat::getNextLine(IOAdapter* io, QByteArray& line, U2OpStatus &ti) {
    line.clear();
    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff = readBuffer.data();

    qint64 len;
    bool eolFound = false, eof = false;
    while (!eolFound) {
        len = io->readLine(buff, READ_BUFF_SIZE, &eolFound);
        CHECK_EXT(!io->hasError(), ti.setError(io->errorString()), false);

        if (len < READ_BUFF_SIZE && !eolFound)
            eolFound = eof = true;
        line += readBuffer;
    }

    if (len != READ_BUFF_SIZE)
        line.resize(line.size() + len - READ_BUFF_SIZE);

    return eof;
}

bool MegaFormat::checkName(QByteArray &name) {
    if (name.contains(MEGA_SEPARATOR) ||
        name.contains(MEGA_START_COMMENT) ||
        name.contains(MEGA_END_COMMENT)) {
        return false;
    }

    return true;
}

bool MegaFormat::readName(IOAdapter* io, QByteArray &line, QByteArray &name, U2OpStatus &ti) {
    bool eof = false;

    line = line.mid(1);
    line = line.trimmed();
    skipWhites(io, line, ti);
    CHECK_OP(ti, eof);
    CHECK(!line.isEmpty(), true);

    line = line.simplified();

    int spaceIdx = line.indexOf(' ');
    if (-1 != spaceIdx) {
        name = line.left(spaceIdx);
        line = line.mid(spaceIdx);
    } else {
        name = line;
        eof = getNextLine(io, line, ti);
        CHECK_OP(ti, eof);

        line = line.simplified();
    }
    if (!checkName(name)) {
        ti.setError(MegaFormat::tr("Bad name of sequence"));
    }

    ti.setProgress(io->getProgress());
    return eof;
}

bool MegaFormat::skipComments(IOAdapter *io, QByteArray &line, U2OpStatus &ti) {
    int i = 0;
    bool eof = false;
    bool hasEnd = false;

    while (1) {
        while (i < line.length() && !hasEnd) {
            if (MEGA_END_COMMENT == line[i]) {
                i++;
                hasEnd = true;
                break;
            }
            if (MEGA_SEPARATOR == line[i]) {
                ti.setError(MegaFormat::tr("Unexpected # in comments"));
                return eof;
            }
            i++;
        }
        if (line.length() == i) {
            if (eof) {
                line.clear();
                if (!hasEnd) {
                    ti.setError(MegaFormat::tr("A comment has not end"));
                    return eof;
                }
                break;
            }
            eof = getNextLine(io, line, ti);
            CHECK_OP(ti, eof);

            line = line.simplified();
            i = 0;
            if (!hasEnd) {
                continue;
            }
        }
        hasEnd = true;
        while (i < line.length()) {
            if (MEGA_START_COMMENT == line[i]) {
                hasEnd = false;
                break;
            } else if (MEGA_SEPARATOR == line[i]) {
                line = line.mid(i);
                i = -1;
                break;
            } else if (' '!=line[i]) {
                ti.setError(MegaFormat::tr("Unexpected symbol between comments"));
                return eof;
            }
            i++;
        }
        if (!hasEnd) {
            continue;
        }
        if (line.length() != i) {
            break;
        }
        if (line.length() == i && eof) {
            line.clear();
            break;
        }
    }

    ti.setProgress(io->getProgress());
    return eof;
}

void MegaFormat::workUpIndels(MultipleSequenceAlignment& al) {
    QByteArray firstSequence=al->getMsaRow(0)->getData();

    for (int i=1; i<al->getNumRows(); i++) {
        QByteArray newSeq=al->getMsaRow(i)->getData();
        for (int j=0; j<newSeq.length(); j++) {
            if (MEGA_IDENTICAL==al->charAt(i, j)) {
                newSeq[j]=firstSequence[j];
            }
        }
        al->setRowContent(i, newSeq);
    }
}

void MegaFormat::load(U2::IOAdapter *io, const U2DbiRef& dbiRef, QList<GObject*> &objects, const QVariantMap& fs, U2::U2OpStatus &os) {
    MultipleSequenceAlignment al(io->getURL().baseFileName());
    QByteArray line;
    bool eof = false;
    bool firstBlock = true;
    int sequenceIdx = 0;
    bool lastIteration = false;

    readHeader(io, line, os);
    CHECK_OP(os, );

    readTitle(io, line, os);
    CHECK_OP(os, );

    //read data
    QList<int> rowLens;
    while (!os.isCoR() && !lastIteration) {
        QByteArray name;
        QByteArray value;

        //read name of a sequence
        if (readName(io, line, name, os)) {
            if (!eof && name.isEmpty()) {
                os.setError(MegaFormat::tr("Incorrect format"));
                return;
            } else if (name.isEmpty()) {
                break;
            }
        }
        CHECK_OP(os, );

        //read the sequence
        eof = readSequence(io, line, os, value, &lastIteration);
        CHECK_OP(os, );

        if ((0 == sequenceIdx) && value.contains(MEGA_IDENTICAL)) {
            os.setError(MegaFormat::tr("Identical symbol at the first sequence"));
            return;
        }

        if (firstBlock) {
            for (int i = 0; i < al->getNumRows(); i++) {
                if (al->getMsaRow(i)->getName() == name) {
                    firstBlock = false;
                    sequenceIdx = 0;
                    break;
                }
            }
        }
        //add the sequence to the list
        if (firstBlock) {
            al->addRow(name, value);
            rowLens.append(value.size());
            sequenceIdx++;
        } else {
            if (sequenceIdx<al->getNumRows()) {
                if (al->getMsaRow(sequenceIdx)->getName() != name) {
                    os.setError(MegaFormat::tr("Incorrect order of sequences' names"));
                    return;
                }
                al->appendChars(sequenceIdx, rowLens[sequenceIdx], value.constData(), value.size());
                rowLens[sequenceIdx] = rowLens[sequenceIdx] + value.size();
            } else {
                os.setError( MegaFormat::tr("Incorrect sequence") );
                break;
            }
            sequenceIdx++;
            if (sequenceIdx == al->getNumRows()) {
                sequenceIdx = 0;
            }
        }
    }

    foreach (int rowLen, rowLens) {
        if (rowLen != al->getLength()) {
            os.setError( MegaFormat::tr("Found sequences of different sizes"));
            break;
        }
    }

    CHECK_OP(os, );

    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet() != NULL, os.setError( tr("Alphabet is unknown")), );

    workUpIndels(al); //replace '.' by symbols from the first sequence

    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, );
    objects.append(obj);
}

void MegaFormat::storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &ti) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "Mega entry storing: no alignment", );
    const QList<GObject*> &als = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(1 == als.size(), "Mega entry storing: alignment objects count error", );

    const MultipleSequenceAlignmentObject* obj = dynamic_cast<MultipleSequenceAlignmentObject*>(als.first());
    SAFE_POINT(NULL != obj, "Mega entry storing: NULL alignment object", );

    const MultipleSequenceAlignment msa = obj->getMultipleAlignment();

    //write header
    QByteArray header;
    header.append(MEGA_SEPARATOR).append(MEGA_HEADER).append("\n").append(MEGA_UGENE_TITLE);
    int len = io->writeBlock(header);
    if (len != header.length()) {
        ti.setError(L10N::errorTitle());
        return;
    }

    int maxNameLength=0;
    foreach (const MultipleSequenceAlignmentRow &item, msa->getMsaRows()) {
        maxNameLength = qMax(maxNameLength, item->getName().length());
    }

    //write data
    int seqLength=msa->getLength();
    int writtenLength=0;
    MultipleSequenceAlignmentWalker walker(msa);
    while (writtenLength<seqLength) {
        QList<QByteArray> seqs = walker.nextData(BLOCK_LENGTH, ti);
        CHECK_OP(ti, );
        QList<QByteArray>::ConstIterator si = seqs.constBegin();
        QList<MultipleSequenceAlignmentRow> rows = msa->getMsaRows();
        QList<MultipleSequenceAlignmentRow>::ConstIterator ri = rows.constBegin();
        for (; si != seqs.constEnd(); si++, ri++) {
            const MultipleSequenceAlignmentRow &item = *ri;
            QByteArray line;
            line.append(MEGA_SEPARATOR).append(item->getName());
            TextUtils::replace(line.data(), line.length(), TextUtils::WHITES, '_');

            for (int i=0; i<maxNameLength-item->getName().length()+1; i++) {
                line.append(' ');
            }

            line.append(*si).append('\n');

            len = io->writeBlock(line);
            if (len != line.length()) {
                ti.setError(L10N::errorTitle());
                return;
            }
        }
        writtenLength+=BLOCK_LENGTH;

        io->writeBlock("\n\n");
    }
}

void MegaFormat::readHeader(U2::IOAdapter *io, QByteArray &line, U2::U2OpStatus &ti) {
    skipWhites(io, line, ti);
    CHECK_OP(ti, );
    CHECK_EXT(!line.isEmpty(), ti.setError(MegaFormat::tr("No header")), );
    CHECK_EXT(line.startsWith(MEGA_SEPARATOR), ti.setError(MegaFormat::tr("No # before header")), );

    line = line.mid(1);
    line = line.trimmed();
    skipWhites(io, line, ti);
    CHECK_OP(ti, );
    CHECK_EXT(!line.isEmpty(), ti.setError(MegaFormat::tr("No header")), );
    CHECK_EXT(line.startsWith(MEGA_HEADER), ti.setError(MegaFormat::tr("Not MEGA-header")), );

    line = line.mid(MEGA_HEADER.length());
    line = line.trimmed();
    ti.setProgress(io->getProgress());
}

void MegaFormat::skipWhites(U2::IOAdapter *io, QByteArray &line, U2::U2OpStatus &ti) {
    while (line.isEmpty()) {
        bool nexLine = getNextLine(io, line, ti);
        CHECK_OP(ti, );

        if (nexLine) {
            CHECK(!line.isEmpty(), );
        }

        line = line.trimmed();
    }
}

void MegaFormat::readTitle(U2::IOAdapter *io, QByteArray &line, U2::U2OpStatus &ti) {
    skipWhites(io, line, ti);
    CHECK_OP(ti, );
    CHECK_EXT(!line.isEmpty(), ti.setError(MegaFormat::tr("No data in file")), );

    bool comment = false;
    if (MEGA_START_COMMENT == line[0]) {
        line = line.mid(1);
        line = line.trimmed();
        comment = true;
        skipWhites(io, line, ti);
        CHECK_OP(ti, );
        CHECK_EXT(!line.isEmpty(), ti.setError(MegaFormat::tr("No data in file")), );
    }

    line = line.simplified();
    QByteArray word = line.left(MEGA_TITLE.length());
    word = word.toUpper();
    CHECK_EXT(word == MEGA_TITLE, ti.setError(MegaFormat::tr("Incorrect title")), );

    line = line.mid(MEGA_TITLE.length());
    if (!line.isEmpty() &&
        (TextUtils::ALPHA_NUMS[line[0]] || MEGA_IDENTICAL == line[0] || MEGA_INDEL == line[0])){
        ti.setError(MegaFormat::tr("Incorrect title"));
        return;
    }

    //read until #
    if (comment) {
        skipComments(io, line, ti);
        CHECK_OP(ti, );
    } else {
        int sepIdx = line.indexOf(MEGA_SEPARATOR);
        while (-1 == sepIdx) {
            bool nexLine = getNextLine(io, line, ti);
            CHECK_OP(ti, );
            if (nexLine) {
                CHECK_EXT(!line.isEmpty(), ti.setError(MegaFormat::tr("No data in file")), );
            }
            sepIdx = line.indexOf(MEGA_SEPARATOR);
        }
        line = line.mid(sepIdx);
    }
    ti.setProgress(io->getProgress());
}

bool MegaFormat::readSequence(U2::IOAdapter *io, QByteArray &line, U2::U2OpStatus &ti,
                              QByteArray &value, bool *lastIteration) {
    bool hasPartOfSequence = false;
    bool eof = false;
    while (!ti.isCoR()) {
        //delete spaces from the sequence until #
        int spaceIdx=line.indexOf(' ');
        int separatorIdx;
        while (-1 != spaceIdx) {
            separatorIdx = line.indexOf(MEGA_SEPARATOR);
            if (-1 != separatorIdx && separatorIdx<spaceIdx) {
                break;
            }
            line = line.left(spaceIdx).append(line.mid(spaceIdx+1));
            spaceIdx=line.indexOf(' ');
        }

        //read another part if it is needed
        if (line.isEmpty()) {
            bool nextLine = getNextLine(io, line, ti);
            CHECK_OP(ti, eof);

            if (nextLine) {
                if (!hasPartOfSequence) {
                    ti.setError(MegaFormat::tr("Sequence has empty part"));
                    return eof;
                } else {
                    eof=true;
                    break;
                }
            }
            ti.setProgress(io->getProgress());
            line = line.simplified();
            continue;
        }

        separatorIdx = line.indexOf(MEGA_SEPARATOR);
        int commentIdx = line.indexOf(MEGA_START_COMMENT);

        int sequenceEnd = (-1 == separatorIdx) ? line.size() : separatorIdx;
        sequenceEnd = (-1 == commentIdx) ? sequenceEnd : qMin(sequenceEnd, commentIdx);
        //check symbols in the sequence
        for (int i = 0; i < sequenceEnd; i++) {
            if (!(TextUtils::ALPHAS[line[i]]) && !(line[i] == MEGA_INDEL) && !(line[i] == MEGA_IDENTICAL)) {
                ti.setError(MegaFormat::tr("Bad symbols in a sequence"));
                return eof;
            }
        }
        value.append(line, sequenceEnd);
        hasPartOfSequence = true;

        if (-1 != commentIdx) { //skip comments untill #
            if ((-1 != separatorIdx && commentIdx < separatorIdx)
             || -1 == separatorIdx) {
                line = line.mid(commentIdx);
                eof = skipComments(io, line, ti);
                if (ti.hasError()) {
                    return eof;
                }
                line = line.simplified();
                if (!line.isEmpty()) {
                    separatorIdx = 0;
                }
            }
        }
        if (eof) {
            (*lastIteration) = true;
            break;
        }
        if (-1 == separatorIdx) {
            bool nextLine = getNextLine(io, line, ti);
            CHECK_OP(ti, eof);

            if (nextLine) {
                if (!line.isEmpty()) {
                    ti.setProgress(io->getProgress());
                    line = line.simplified();
                    continue;
                }
                eof = true;
                break;
            }
            ti.setProgress(io->getProgress());
            line = line.simplified();
            continue;
        } else {
            line = line.mid(separatorIdx);
            break;
        }
    }

    return eof;
}
} //namespace
