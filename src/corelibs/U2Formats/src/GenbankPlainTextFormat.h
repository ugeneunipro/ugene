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

#pragma once

#include <U2Core/GenbankFeatures.h>

#include "EMBLGenbankAbstractDocument.h"

namespace U2 {

class U2FORMATS_EXPORT GenbankPlainTextFormat : public EMBLGenbankAbstractDocument {
    Q_OBJECT
public:
    GenbankPlainTextFormat(QObject* p);

    void storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os) override;

    bool isStreamingSupport() override;

    void storeEntry(IOAdapter* io, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;

    bool hasModifiableName() const override;

    static bool checkCircularity(const GUrl& filePath, U2OpStatus& os);

protected:
    FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const override;

    typedef QPair<QString, QString> StrPair;

    static bool readIdLine(ParserState*);
    bool readEntry(ParserState*, U2SequenceImporter&, int& seqSize, int& fullSeqSize, bool merge, int gapSize, U2OpStatus&) override;
    void readHeaderAttributes(QVariantMap& tags, DbiConnection& con, U2SequenceObject* so) override;

    void writeAnnotations(IOAdapter* io, const QList<GObject*>& aos, U2OpStatus& si);
    static QString genLocusString(const QList<GObject*>& aos, U2SequenceObject* so, const QString& locusStrFromAttr);
    static bool writeKeyword(IOAdapter* io, U2OpStatus& os, const QString& key, const QString& value);
    static void writeSequence(IOAdapter* io, U2SequenceObject* ao, const QList<U2Region>& lowerCaseRegs, U2OpStatus& si);
    static void prepareMultiline(QString& line, int spacesOnLineStart, bool lineBreakOnlyOnSpace = true, bool newLineAtTheEnd = true, int maxLineLen = 79);
    void writeQualifier(const QString& name, const QString& val, IOAdapter* io, U2OpStatus& si, const char* spaceLine);
    QString prepareQualifierSingleString(const QString& qualifierName, const QString& qualifierValue) const;
    QList<StrPair> formatKeywords(const QVariantMap& varMap, bool withLocus = false);
    virtual QList<StrPair> processCommentKeys(QMultiMap<QString, QVariant>& tags);

    bool isNcbiLikeFormat() const override;
    void createCommentAnnotation(const QStringList& comments, int sequenceLength, AnnotationTableObject* annTable) const override;
    U2FeatureType getFeatureType(const QString& typeString) const override;
    static QString getFeatureTypeString(U2FeatureType featureType, bool isAmino) ;
    bool breakQualifierOnSpaceOnly(const QString& qualifierName) const override;

    static constexpr int VAL_OFF = 12;

    static const QMap<U2FeatureType, GBFeatureKey> additionalFeatureTypes;  // some feature types might be converted to the GBFeatureKey
    static QMap<U2FeatureType, GBFeatureKey> initAdditionalFeatureTypes();
};

}  // namespace U2
