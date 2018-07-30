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

#ifndef _U2_EMBL_GENBANK_ABSTRACT_DOCUMENT_H_
#define _U2_EMBL_GENBANK_ABSTRACT_DOCUMENT_H_

#include <QStringList>

#include <U2Core/AnnotationData.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SequenceUtils.h>

#include "TextDocumentFormat.h"

namespace U2 {

class AnnotationTableObject;
class EMBLGenbankDataEntry;
class IOAdapter;
class ParserState;

class U2FORMATS_EXPORT EMBLGenbankAbstractDocument : public TextDocumentFormat {
    Q_OBJECT
public:
    EMBLGenbankAbstractDocument(const DocumentFormatId& id, const QString& formatName,
                                int maxLineSize, DocumentFormatFlags flags, QObject* p);

    virtual DocumentFormatId getFormatId() const {return id;}

    virtual const QString& getFormatName() const {return formatName;}

    static const QString UGENE_MARK;
    static const QString DEFAULT_OBJ_NAME;
    static const QString LOCUS_TAG_CIRCULAR;
    static const QString LOCUS_TAG_LINEAR;

    static const QString REMOTE_ENTRY_WARNING_MESSAGE;
    static const QString JOIN_COMPLEMENT_WARNING_MESSAGE;
    static const QString LOCATION_PARSING_ERROR_MESSAGE;
    static const QString SEQ_LEN_WARNING_MESSAGE;

    // move to utils??
    static QString genObjectName(QSet<QString>& usedNames, const QString& name, const QVariantMap& info, int n, const GObjectType& t);

protected:
    virtual DNASequence* loadTextSequence(IOAdapter*, U2OpStatus& os);

    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

    int readMultilineQualifier(IOAdapter* io, char* cbuff, int maxSize, bool prevLineHasMaxSize, int lenFirstQualLine, U2OpStatus& os);

    virtual bool readSequence(ParserState*, U2SequenceImporter&, int&, int&, U2OpStatus&);

    virtual bool readEntry(ParserState*, U2SequenceImporter& ,int& seqSize,int& fullSeqSize,bool merge, int gapSize,U2OpStatus&) = 0;
    virtual void readAnnotations(ParserState*, int offset);
    virtual void readHeaderAttributes(QVariantMap& tags, DbiConnection& con, U2SequenceObject* so) {
        Q_UNUSED(tags); Q_UNUSED(con); Q_UNUSED(so);
    } // does nothing if not overloaded

    virtual bool isNcbiLikeFormat() const;
    virtual void createCommentAnnotation(const QStringList &comments, int sequenceLength, AnnotationTableObject *annTable) const;
    virtual U2FeatureType getFeatureType(const QString &typeString) const;
    virtual U2Qualifier createQualifier(const QString &qualifierName, const QString &qualifierValue, bool containsDoubleQuotes) const;
    virtual bool breakQualifierOnSpaceOnly(const QString &qualifierName) const;

    DocumentFormatId id;
    QString     formatName;
    QByteArray  fPrefix;
    QByteArray  sequenceStartPrefix;
    int         maxAnnotationLineLen;
    bool        savedInUgene; // saveInUgene marker is a hack for opening genbank files that were saved incorrectly(!) in UGENE version <1.14.1

private:
    SharedAnnotationData readAnnotation(IOAdapter* io, char* cbuff, int contentLen, int bufSize, U2OpStatus& si, int offset, int seqLen = -1);
    void load(const U2DbiRef& dbiRef, IOAdapter* io, QList<GObject*>& objects, QVariantMap& fs, U2OpStatus& si, QString& writeLockReason);
    void skipInvalidAnnotation(int len, IOAdapter* io, char* cbuff, int READ_BUFF_SIZE);
};

//////////////////////////////////////////////////////////////////////////
// header model

class EMBLGenbankDataEntry {
public:
    EMBLGenbankDataEntry() : seqLen(0), hasAnnotationObjectFlag(false), circular(false) {}
     /** locus name */
    QString name;

    /** sequence len*/
    int	seqLen;

    QVariantMap tags;
    QList<SharedAnnotationData> features;

    // hasAnnotationObjectFlag parameter is used to indicate that
    // annotation table object must present even if result list is empty
    bool hasAnnotationObjectFlag;
    bool circular;
};

class ParserState {
public:
    ParserState(int off, IOAdapter* io, EMBLGenbankDataEntry* e, U2OpStatus& si)
        : valOffset(off), entry(e), io(io), buff(NULL), len(0), si(si) {}

    const int valOffset;
    EMBLGenbankDataEntry* entry;
    IOAdapter* io;
    char* buff;
    int len;
    U2OpStatus& si;

    QString value() const;
    QString key () const;
    bool hasKey(const char*, int slen) const;
    bool hasKey(const char* s) const {return hasKey(s, strlen(s));}
    bool hasContinuation() const { return len > valOffset && hasKey(" ");}
    bool hasValue() const {return len > valOffset;}
    bool readNextLine(bool emptyOK = false);
    bool isNull() const {return entry->name.isNull();}

    static const int LOCAL_READ_BUFFER_SIZE;
};

}//namespace

#endif
