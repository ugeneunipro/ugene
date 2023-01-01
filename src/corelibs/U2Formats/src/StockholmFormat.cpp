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

#include "StockholmFormat.h"

#include <QTextStream>

#include <U2Core/GAutoDeleteList.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleAlignmentInfo.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentWalker.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString StockholmFormat::FILE_ANNOTATION_ID = "#=GF ID";
const QString StockholmFormat::FILE_ANNOTATION_AC = "#=GF AC";
const QString StockholmFormat::FILE_ANNOTATION_DE = "#=GF DE";
const QString StockholmFormat::FILE_ANNOTATION_GA = "#=GF GA";
const QString StockholmFormat::FILE_ANNOTATION_NC = "#=GF NC";
const QString StockholmFormat::FILE_ANNOTATION_TC = "#=GF TC";

const QString StockholmFormat::UNI_ANNOTATION_MARK = "# UNIMARK";

const QString StockholmFormat::COLUMN_ANNOTATION_SS_CONS = "#=GC SS_cons";
const QString StockholmFormat::COLUMN_ANNOTATION_RF = "#=GC RF";

/** Maximum line length supported by UGENE when parsing Stockholm format. */
static constexpr int MAX_STOCKHOLM_LINE_LENGTH = 1024 * 1024;

static const QString HEADER = "# STOCKHOLM 1.0\n\n";
static const QString HEADER_PREFIX = "# STOCKHOLM 1.";
static const QString END_OF_MSA_OBJECT_TOKEN = "//";

static constexpr char COMMENT_OR_MARKUP_LINE = '#';

static constexpr int WRITE_BLOCK_LENGTH = 50;

/** Set of supported flags. */
enum AnnotationTag {
    NO_TAG = -1,
    ID,
    AC,
    DE,
    SS_CONS,
    RF,
    GA,
    NC,
    TC
};

/** Set of supported annotation types. */
enum AnnotationType {
    FILE_ANNOTATION,
    COLUMN_ANNOTATION,
    UNI_ANNOTATION
};

/** Internal representation of the annotation used internally during parsing. */
struct StockholmAnnotation {
    StockholmAnnotation(const AnnotationType& _type, const AnnotationTag& _tag, const QString& _value)
        : type(_type), tag(_tag), value(_value) {
    }
    const AnnotationType type;
    const AnnotationTag tag;
    QString value;
};

/**
 * A list of StockholmAnnotation with an adjusted 'add' method.
 * Auto-deletes all annotations when destroyed.
 */
class AnnotationBank {
public:
    void add(StockholmAnnotation* newAnnotation) {
        CHECK(newAnnotation != nullptr, );
        if (newAnnotation->type == COLUMN_ANNOTATION && (newAnnotation->tag == SS_CONS || newAnnotation->tag == RF)) {
            auto existingAnnotation = findAnnotation(COLUMN_ANNOTATION, newAnnotation->tag);
            if (existingAnnotation != nullptr) {
                existingAnnotation->value += newAnnotation->value;
                delete newAnnotation;
                return;
            }
        }
        annotationsList.qlist.append(newAnnotation);
    }

    /** Finds annotations by the type & tag pair. */
    StockholmAnnotation* findAnnotation(const AnnotationType& type, const AnnotationTag& tag = NO_TAG) const {
        for (StockholmAnnotation* annotation : qAsConst(annotationsList.qlist)) {
            if (annotation->type == type && (annotation->tag == tag || tag == NO_TAG)) {
                return annotation;
            }
        }
        return nullptr;
    }

    /** Returns map of annotation values by name. */
    QHash<QString, QString> createValueByNameMap() const {
        QHash<QString, QString> valueByName;
        for (StockholmAnnotation* annotation : qAsConst(annotationsList.qlist)) {
            QString name = getGenbankAnnotationName(annotation);
            valueByName[name] = annotation->value;
        }
        return valueByName;
    }

    /** Derives Genbank annotation name from the given Stockholm annotation. */
    static QString getGenbankAnnotationName(StockholmAnnotation* annotation) {
        switch (annotation->type) {
            case UNI_ANNOTATION:
                return StockholmFormat::UNI_ANNOTATION_MARK;
            case FILE_ANNOTATION: {
                switch (annotation->tag) {
                    case ID:
                        return StockholmFormat::FILE_ANNOTATION_ID;
                    case AC:
                        return StockholmFormat::FILE_ANNOTATION_AC;
                    case DE:
                        return StockholmFormat::FILE_ANNOTATION_DE;
                    case GA:
                        return StockholmFormat::FILE_ANNOTATION_GA;
                    case NC:
                        return StockholmFormat::FILE_ANNOTATION_NC;
                    case TC:
                        return StockholmFormat::FILE_ANNOTATION_TC;
                    default:
                        break;
                }
                FAIL(QString("Bad FILE annotation tag: %1").arg(annotation->tag), "");
            }
            case COLUMN_ANNOTATION: {
                switch (annotation->tag) {
                    case SS_CONS:
                        return StockholmFormat::COLUMN_ANNOTATION_SS_CONS;
                    case RF:
                        return StockholmFormat::COLUMN_ANNOTATION_RF;
                    default:
                        break;
                }
                FAIL(QString("Bad COLUMN annotation tag: %1").arg(annotation->tag), "");
            }
            default:
                FAIL(QString("Unsupported annotation tag: %1").arg(annotation->tag), "");
        }
    }

private:
    GAutoDeleteList<StockholmAnnotation> annotationsList;
};  // AnnotationBank

/** Parses annotation from the given line. Returns nullptr if annotation can't be parsed. */
static StockholmAnnotation* parseAnnotation(const QString& lineText) {
    QString line = lineText.trimmed();

    if (line.startsWith(StockholmFormat::FILE_ANNOTATION_ID)) {
        QString val = line.mid(StockholmFormat::FILE_ANNOTATION_ID.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(FILE_ANNOTATION, ID, val) : nullptr;
    } else if (line.startsWith(StockholmFormat::FILE_ANNOTATION_AC)) {
        QString val = line.mid(StockholmFormat::FILE_ANNOTATION_AC.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(FILE_ANNOTATION, AC, val) : nullptr;
    } else if (line.startsWith(StockholmFormat::FILE_ANNOTATION_DE)) {
        QString val = line.mid(StockholmFormat::FILE_ANNOTATION_DE.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(FILE_ANNOTATION, DE, val) : nullptr;
    } else if (line.startsWith(StockholmFormat::FILE_ANNOTATION_GA)) {
        QString val = line.mid(StockholmFormat::FILE_ANNOTATION_GA.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(FILE_ANNOTATION, GA, val) : nullptr;
    } else if (line.startsWith(StockholmFormat::FILE_ANNOTATION_NC)) {
        QString val = line.mid(StockholmFormat::FILE_ANNOTATION_NC.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(FILE_ANNOTATION, NC, val) : nullptr;
    } else if (line.startsWith(StockholmFormat::FILE_ANNOTATION_TC)) {
        QString val = line.mid(StockholmFormat::FILE_ANNOTATION_TC.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(FILE_ANNOTATION, TC, val) : nullptr;
    } else if (StockholmFormat::UNI_ANNOTATION_MARK == line) {
        return new StockholmAnnotation(UNI_ANNOTATION, NO_TAG, line);
    } else if (line.startsWith(StockholmFormat::COLUMN_ANNOTATION_SS_CONS)) {
        QString val = line.mid(StockholmFormat::COLUMN_ANNOTATION_SS_CONS.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(COLUMN_ANNOTATION, SS_CONS, val) : nullptr;
    } else if (line.startsWith(StockholmFormat::COLUMN_ANNOTATION_RF)) {
        QString val = line.mid(StockholmFormat::COLUMN_ANNOTATION_RF.size()).trimmed();
        return !val.isEmpty() ? new StockholmAnnotation(COLUMN_ANNOTATION, RF, val) : nullptr;
    }
    return nullptr;
}

/** Returns MSA object name using ID annotation value or returns an empty string if no ID annotation has been found. */
static QString getMsaNameFromFileAnnotation(const AnnotationBank& annotationBank) {
    auto annotation = annotationBank.findAnnotation(FILE_ANNOTATION, ID);
    return annotation == nullptr ? "" : annotation->value;
}

/** Returns 'true' if the file was created by UGENE: the annotations list has an annotation with a special UGENE mark. */
static bool hasCreatedByUGENEMarker(const AnnotationBank& annotationBank) {
    auto annotation = annotationBank.findAnnotation(UNI_ANNOTATION);
    return annotation != nullptr && annotation->value == StockholmFormat::UNI_ANNOTATION_MARK;
}

/** Skips comment of markup blocks. Returns true if the whole line was skipped. */
static bool skipCommentOrMarkup(IOAdapterReader& reader, U2OpStatus& os, AnnotationBank& annotationBank) {
    CHECK(!reader.atEnd(), false);
    QString firstChar;
    reader.read(os, firstChar, 1);
    CHECK_OP(os, false);
    if (firstChar != COMMENT_OR_MARKUP_LINE) {
        reader.undo(os);
        return false;
    }
    QString line = firstChar + reader.readLine(os, MAX_STOCKHOLM_LINE_LENGTH);
    CHECK_OP(os, false);
    annotationBank.add(parseAnnotation(line));
    return true;
}

/** Skips blank lines in the input. Returns number of lines skipped. */
static int skipBlankLines(IOAdapterReader& reader, U2OpStatus& os) {
    int lineCount = 0;
    while (!reader.atEnd()) {
        QString line = reader.readLine(os, MAX_STOCKHOLM_LINE_LENGTH);
        CHECK_OP(os, lineCount);
        if (!TextUtils::isWhiteSpace(line)) {
            reader.undo(os);
            break;
        }
        lineCount++;
    }
    return lineCount;
}

/** Skips all comments and unsupported markup tokens. */
static void skipCommentsAndBlankLines(IOAdapterReader& reader, U2OpStatus& os, AnnotationBank& annotationBank) {
    QString ch;
    while (!reader.atEnd()) {
        reader.read(os, ch, 1);
        CHECK_OP(os, );
        if (ch == COMMENT_OR_MARKUP_LINE) {
            reader.undo(os);
            CHECK_OP(os, );
            skipCommentOrMarkup(reader, os, annotationBank);
            CHECK_OP(os, );
            continue;
        }
        if (TextUtils::isWhiteSpace(ch, 0) || TextUtils::isLineBreak(ch, 0)) {
            skipBlankLines(reader, os);
            CHECK_OP(os, );
            continue;
        }
        reader.undo(os);
        CHECK_OP(os, );
        break;
    }
}

/**
 * Returns true if the next line in the input is a Stockholm's end-of-msa marker line.
 * Does not advance reader position.
 */
static bool isEndOfMsaLine(IOAdapterReader& reader, U2OpStatus& os) {
    CHECK(!reader.atEnd(), false);

    QString line = reader.readLine(os, MAX_STOCKHOLM_LINE_LENGTH);
    CHECK_OP(os, false);

    reader.undo(os);
    CHECK_OP(os, false);

    return line.trimmed() == END_OF_MSA_OBJECT_TOKEN;
}

/**
 * Returns true if the reader is at the end position of the MSA object: >1 of blank lines or 'end of MSA' line.
 * Advances reader to the first non-blank line in the input (may be 'end of MSA' line).
 */
static bool isEndOfMsaBlock(IOAdapterReader& reader, U2OpStatus& os) {
    int lineCount = skipBlankLines(reader, os);
    CHECK_OP(os, false);
    return lineCount > 0 || isEndOfMsaLine(reader, os);
}

/** Returns true if there is a row in the 'msa' with the given name. */
static bool hasRowWithName(const MultipleSequenceAlignment& msa, const QString& name) {
    const QList<MultipleAlignmentRow>& rows = msa->getRows();
    return std::any_of(rows.begin(), rows.end(), [name](auto& row) { return row->getName() == name; });
}

/** Loads a single MSA object data and related annotations. */
static void loadOneMsa(IOAdapterReader& reader, U2OpStatus& os, MultipleSequenceAlignment& msa, AnnotationBank& annotationBank) {
    skipBlankLines(reader, os);
    CHECK_OP(os, );

    QString line = reader.readLine(os, MAX_STOCKHOLM_LINE_LENGTH);
    CHECK_OP(os, );

    if (!line.startsWith(HEADER_PREFIX)) {
        os.setError(QString("Invalid file format: bad header line: %1").arg(reader.getURL().getURLString()));
        return;
    }

    int currentLen = 0;
    bool isFirstBlock = true;
    while (!reader.atEnd()) {
        skipCommentsAndBlankLines(reader, os, annotationBank);
        CHECK_OP(os, );

        if (isEndOfMsaLine(reader, os)) {
            break;
        }
        CHECK_OP(os, );

        bool isEndOfBlock = false;
        int sequenceIndex = 0;
        int blockSize = -1;
        while (!isEndOfBlock && !reader.atEnd()) {
            line = reader.readLine(os, MAX_STOCKHOLM_LINE_LENGTH).trimmed();
            CHECK_OP(os, );
            int firstSpaceIndex = TextUtils::findIndexOfFirstWhiteSpace(line);
            if (firstSpaceIndex == -1) {
                firstSpaceIndex = line.length();
            }
            QString name = line.left(firstSpaceIndex);
            QByteArray seq = line.mid(firstSpaceIndex + 1).toLatin1().trimmed();

            if (name.startsWith(COMMENT_OR_MARKUP_LINE)) {
                annotationBank.add(parseAnnotation(line));
                isEndOfBlock = isEndOfMsaBlock(reader, os);
                CHECK_OP(os, );

                os.setProgress(reader.getProgress());
                continue;
            }
            seq.replace('.', '-');
            if (isFirstBlock) {
                CHECK_EXT(!name.isEmpty(), os.setError(StockholmFormat::tr("Invalid file: empty sequence name")), );
                CHECK_EXT(!hasRowWithName(msa, name), os.setError(StockholmFormat::tr("Invalid file: duplicate sequence names in one block: %1").arg(name)), );
                msa->addRow(name, seq);
            } else {
                QString rowName = msa->getMsaRow(sequenceIndex)->getName();
                CHECK_EXT(name == rowName, os.setError(StockholmFormat::tr("Invalid file: sequence names are not equal in blocks")), );
                msa->appendChars(sequenceIndex, currentLen, seq.constData(), seq.size());
            }

            if (blockSize == -1) {
                blockSize = seq.size();
            } else {
                CHECK_EXT(blockSize == seq.size(), os.setError(StockholmFormat::tr("Invalid file: sequences in block are not of equal size")), );
            }

            sequenceIndex++;
            isEndOfBlock = isEndOfMsaBlock(reader, os);
            CHECK_OP(os, );

            os.setProgress(reader.getProgress());
        }
        isFirstBlock = false;
        currentLen += blockSize;
    }  // while( true )

    SAFE_POINT_EXT(reader.atEnd() || isEndOfMsaLine(reader, os), os.setError(L10N::internalError("expected and of MSA object marker")), );
    CHECK_OP(os, );

    reader.readLine(os, MAX_STOCKHOLM_LINE_LENGTH);
    CHECK_OP(os, );

    skipBlankLines(reader, os);
    CHECK_OP(os, );

    CHECK_EXT(msa->getRowCount() > 0, os.setError(StockholmFormat::tr("invalid file: empty sequence alignment")), );

    U2AlphabetUtils::assignAlphabet(msa);
    CHECK_EXT(msa->getAlphabet() != nullptr, os.setError(StockholmFormat::tr("invalid file: unknown alphabet")), )
}

static void setMsaInfoCutoffs(QVariantMap& info,
                              const QString& string,
                              const MultipleAlignmentInfo::Cutoffs& cof1,
                              const MultipleAlignmentInfo::Cutoffs& cof2) {
    QByteArray str = string.toLatin1();
    QTextStream txtStream(str);
    float val1 = .0f;
    float val2 = .0f;
    txtStream >> val1 >> val2;
    MultipleAlignmentInfo::setCutoff(info, cof1, val1);
    MultipleAlignmentInfo::setCutoff(info, cof2, val2);
}

static void setMsaInfo(const QHash<QString, QString>& annMap, MultipleSequenceAlignment& ma) {
    QVariantMap info = ma->getInfo();

    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_AC)) {
        MultipleAlignmentInfo::setAccession(info, annMap[StockholmFormat::FILE_ANNOTATION_AC]);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_DE)) {
        MultipleAlignmentInfo::setDescription(info, annMap[StockholmFormat::FILE_ANNOTATION_DE]);
    }
    if (annMap.contains(StockholmFormat::COLUMN_ANNOTATION_SS_CONS)) {
        MultipleAlignmentInfo::setSSConsensus(info, annMap[StockholmFormat::COLUMN_ANNOTATION_SS_CONS]);
    }
    if (annMap.contains(StockholmFormat::COLUMN_ANNOTATION_RF)) {
        MultipleAlignmentInfo::setReferenceLine(info, annMap[StockholmFormat::COLUMN_ANNOTATION_RF]);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_GA)) {
        setMsaInfoCutoffs(info, annMap[StockholmFormat::FILE_ANNOTATION_GA], MultipleAlignmentInfo::CUTOFF_GA1, MultipleAlignmentInfo::CUTOFF_GA2);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_NC)) {
        setMsaInfoCutoffs(info, annMap[StockholmFormat::FILE_ANNOTATION_NC], MultipleAlignmentInfo::CUTOFF_NC1, MultipleAlignmentInfo::CUTOFF_NC2);
    }
    if (annMap.contains(StockholmFormat::FILE_ANNOTATION_TC)) {
        setMsaInfoCutoffs(info, annMap[StockholmFormat::FILE_ANNOTATION_TC], MultipleAlignmentInfo::CUTOFF_TC1, MultipleAlignmentInfo::CUTOFF_TC2);
    }
    ma->setInfo(info);
}

/** Loads list of MSA objects from the given 'reader'. Adds all loaded objects into the 'objectList'. */
static void load(IOAdapterReader& reader, const U2DbiRef& dbiRef, QList<GObject*>& objectList, const QVariantMap& hints, U2OpStatus& os, bool& isCreatedByUGENE) {
    QSet<QString> objectNameList;
    QString baseFileName = reader.getURL().baseFileName();
    while (!reader.atEnd()) {
        MultipleSequenceAlignment msa;
        AnnotationBank annotationBank;
        loadOneMsa(reader, os, msa, annotationBank);
        CHECK_OP(os, );

        isCreatedByUGENE = isCreatedByUGENE || hasCreatedByUGENEMarker(annotationBank);

        QString name = getMsaNameFromFileAnnotation(annotationBank);
        name = name.isEmpty() || objectNameList.contains(name) ? baseFileName + "_" + QString::number(objectList.size()) : name;
        objectNameList.insert(name);
        msa->setName(name);

        QHash<QString, QString> valueByNameMap = annotationBank.createValueByNameMap();
        setMsaInfo(valueByNameMap, msa);

        QString folder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
        auto msaObject = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, msa, os);
        CHECK_OP(os, );
        msaObject->setIndexInfo(valueByNameMap);
        objectList.append(msaObject);
    }
}

/** Returns maximum row name length in the msa. */
static int getMaxNameLen(const MultipleSequenceAlignment& msa) {
    const QList<MultipleAlignmentRow>& rows = msa->getRows();
    CHECK(rows.isEmpty(), 0);
    auto it = std::max_element(rows.begin(), rows.end(), [](auto& r1, auto& r2) { return r1->getName().length() < r2->getName().length(); });
    return (*it)->getName().length();
}

/** Saves all objects in 'msa' into the 'writer' stream. */
static void save(IOAdapterWriter& writer, const MultipleSequenceAlignment& msa, const QString& name, U2OpStatus& os) {
    writer.write(os, HEADER);
    CHECK_OP(os, );

    writer.write(os, StockholmFormat::UNI_ANNOTATION_MARK + "\n\n");
    CHECK_OP(os, );

    QString idValue = QString(name).replace(QRegExp("\\s"), "_");
    writer.write(os, StockholmFormat::FILE_ANNOTATION_ID + " " + idValue + "\n\n");
    CHECK_OP(os, );

    // Write sequences.
    int maxNameLength = getMaxNameLen(msa);
    int remainingSequenceLength = msa->getLength();
    MultipleSequenceAlignmentWalker walker(msa);
    const QList<MultipleAlignmentRow>& rows = msa->getRows();
    while (remainingSequenceLength > 0) {
        // Write a block.
        int blockLength = qMin(remainingSequenceLength, WRITE_BLOCK_LENGTH);
        QList<QByteArray> sequences = walker.nextData(blockLength, os);
        CHECK_OP(os, );
        SAFE_POINT(sequences.size() == rows.size(), "Sequences and rows counts do not match!", );
        for (int i = 0; i < rows.size(); i++) {
            const MultipleAlignmentRow& row = rows[i];
            QByteArray safeRowName = row->getName().toLatin1();
            TextUtils::replace(safeRowName.data(), safeRowName.length(), TextUtils::WHITES, '_');
            writer.write(os, safeRowName);
            CHECK_OP(os, );
            writer.write(os, QString(" ").repeated(qMax(1, maxNameLength - row->getName().size())));
            CHECK_OP(os, );
            writer.write(os, sequences[i]);
            CHECK_OP(os, );
            writer.write(os, "\n");
            CHECK_OP(os, );
        }
        writer.write(os, "\n\n");
        CHECK_OP(os, );
        remainingSequenceLength -= blockLength;
    }
    // Write end of MSA object mark.
    writer.write(os, END_OF_MSA_OBJECT_TOKEN + "\n");
}

StockholmFormat::StockholmFormat(QObject* obj)
    : TextDocumentFormat(obj,
                         BaseDocumentFormats::STOCKHOLM,
                         DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject | DocumentFormatFlag_LockedIfNotCreatedByUGENE,
                         {"sto"}) {
    formatName = tr("Stockholm");
    formatDescription = tr("A multiple sequence alignments file format");
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

Document* StockholmFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    QList<GObject*> objects;
    bool isCreatedByUGENE = false;
    load(reader, dbiRef, objects, hints, os, isCreatedByUGENE);
    CHECK_OP_EXT(os, qDeleteAll(objects), nullptr);

    // Set lock to avoid change data that we can't read/write safely.
    QString lockReason = isCreatedByUGENE ? "" : QObject::tr("The document is not created by UGENE");
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints, lockReason);
}

void StockholmFormat::storeTextDocument(IOAdapterWriter& writer, Document* doc, U2OpStatus& os) {
    QList<GObject*> objects = doc->getObjects();
    for (GObject* obj : qAsConst(objects)) {
        auto alnObj = qobject_cast<const MultipleSequenceAlignmentObject*>(obj);
        SAFE_POINT_EXT(alnObj != nullptr, os.setError(tr("Not an alignment object: ") + obj->getGObjectName()), );
        save(writer, alnObj->getMultipleAlignment(), alnObj->getGObjectName(), os);
        CHECK_OP(os, );
    }
}

FormatCheckResult StockholmFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    return dataPrefix.startsWith(HEADER_PREFIX) ? FormatDetection_VeryHighSimilarity : FormatDetection_NotMatched;
}

bool StockholmFormat::isObjectOpSupported(const Document*, DocObjectOp, GObjectType type) const {
    return type == GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

}  // namespace U2
