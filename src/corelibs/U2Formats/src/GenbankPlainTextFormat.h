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

#ifndef _U2_GENBANK_PLAIN_TEXT_FORMAT_H_
#define _U2_GENBANK_PLAIN_TEXT_FORMAT_H_

#include <U2Core/GenbankFeatures.h>

#include "EMBLGenbankAbstractDocument.h"

namespace U2 {

class U2FORMATS_EXPORT GenbankPlainTextFormat : public EMBLGenbankAbstractDocument {
    Q_OBJECT
public:
    GenbankPlainTextFormat(QObject* p);

    virtual void storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os);

    virtual bool isStreamingSupport();

    virtual void storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os);

    static bool checkCircularity(const GUrl& filePath, U2OpStatus& os);

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;

    typedef QPair<QString, QString> StrPair;

    static bool readIdLine(ParserState*);
    bool readEntry(ParserState*, U2SequenceImporter&,int& seqSize,int& fullSeqSize,bool merge, int gapSize,U2OpStatus&);
    void readHeaderAttributes(QVariantMap& tags, DbiConnection& con, U2SequenceObject* so);

    void writeAnnotations(IOAdapter *io, const QList<GObject*> &aos, U2OpStatus &si);
    QString genLocusString(const QList<GObject*> &aos, U2SequenceObject* so, const QString &locusStrFromAttr);
    bool writeKeyword(IOAdapter* io, U2OpStatus& os, const QString& key, const QString& value, bool wrap = true /*TODO*/);
    void writeSequence(IOAdapter* io, U2SequenceObject* ao, const QList<U2Region> &lowerCaseRegs, U2OpStatus& si);
    void prepareMultiline(QString& line, int spacesOnLineStart, bool lineBreakOnlyOnSpace = true, bool newLineAtTheEnd = true, int maxLineLen = 79);
    void writeQualifier(const QString& name, const QString& val, IOAdapter* io, U2OpStatus& si, const char* spaceLine);
    QString prepareQualifierSingleString(const QString &qualifierName, const QString &qualifierValue) const;
    QList<StrPair> formatKeywords(const QVariantMap &varMap, bool withLocus = false);
    virtual QList<StrPair> processCommentKeys(QMultiMap<QString, QVariant> &tags);

    bool isNcbiLikeFormat() const;
    void createCommentAnnotation(const QStringList &comments, int sequenceLength, AnnotationTableObject *annTable) const;
    U2FeatureType getFeatureType(const QString &typeString) const;
    QString getFeatureTypeString(U2FeatureType featureType, bool isAmino) const;
    bool breakQualifierOnSpaceOnly(const QString &qualifierName) const;

    static const int VAL_OFF = 12;

    static const QMap<U2FeatureType, GBFeatureKey> additionalFeatureTypes;  // some feature types might be converted to the GBFeatureKey
    static QMap<U2FeatureType, GBFeatureKey> initAdditionalFeatureTypes();
};


}//namespace

#endif
