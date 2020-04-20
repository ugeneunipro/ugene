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

#ifndef _U2_SNPEFF_INFO_PARSER_H_
#define _U2_SNPEFF_INFO_PARSER_H_

#include <QMap>

#include <U2Core/AnnotationData.h>

namespace U2 {

class AnnotationCreationPattern;
class InfoPartParser;
class U2OpStatus;

class U2FORMATS_EXPORT SnpeffInfoParser : public QObject {
    Q_OBJECT
public:
    SnpeffInfoParser();
    ~SnpeffInfoParser();

    /**
     * Each @snpeffInfo string can contain data for several annotations.
     * For each annotation a U2Qualifier list will be produced.
     * So, @return will contain data for several annotations that are produced from one variation.
     */
    QList<QList<U2Qualifier> > parse(U2OpStatus &os, const QString &snpeffInfo) const;

private:
    void initPartParsers();

    QMap<QString, InfoPartParser *> partParsers;

    static const QString PAIRS_SEPARATOR;
    static const QString KEY_VALUE_SEPARATOR;
};

class InfoPartParser : public QObject {
    Q_OBJECT
public:
    InfoPartParser(const QString &keyWord, bool canStoreMessages = false);

    const QString & getKeyWord() const;
    QList<QList<U2Qualifier> > parse(U2OpStatus &os, const QString &infoPart) const;

protected:
    virtual QStringList getQualifierNames() const = 0;
    virtual QStringList getValues(const QString &entry) const = 0;
    virtual QList<U2Qualifier> processValue(const QString &qualifierName, const QString &value) const;

    static const QString ERROR;
    static const QString WARNING;
    static const QString INFO;
    static const QString MESSAGE;
    static const QString MESSAGE_DESCRIPTION;

private:
    QList<U2Qualifier> parseEntry(U2OpStatus &os, const QString &entry) const;

    const QString keyWord;
    const bool canStoreMessages;

    static const QString ANNOTATION_SEPARATOR;
    static const QString SNPEFF_TAG;
};

class AnnParser : public InfoPartParser {
public:
    AnnParser();

    static const QString KEY_WORD;

private:
    QStringList getQualifierNames() const;
    QStringList getValues(const QString &entry) const;
    QList<U2Qualifier> processValue(const QString &qualifierName, const QString &value) const;

    static const QString VALUES_SEPARATOR;
    static const QString EFFECTS_SEPARATOR;
    static const QString EFFECT;
    static const QString EFFECT_DESCRIPTION;
    static const QString PUTATIVE_IMPACT;
    static const QString PUTATIVE_IMPACT_DESCRIPTION;
};

class EffParser : public InfoPartParser {
public:
    EffParser();

    static const QString KEY_WORD;

private:
    QStringList getQualifierNames() const;
    QStringList getValues(const QString &entry) const;
    QList<U2Qualifier> processValue(const QString &qualifierName, const QString &value) const;

    static const QString EFFECT_DATA_SEPARATOR;
    static const QString EFFECT;
    static const QString EFFECT_DESCRIPTION;
    static const QString EFFECT_IMPACT;
    static const QString EFFECT_IMPACT_DESCRIPTION;
};

class LofParser : public InfoPartParser {
public:
    LofParser();

    static const QString KEY_WORD;

private:
    QStringList getQualifierNames() const;
    QStringList getValues(const QString &entry) const;

    static const QString VALUES_SEPARATOR;
};

class NmdParser : public InfoPartParser {
public:
    NmdParser();

    static const QString KEY_WORD;

private:
    QStringList getQualifierNames() const;
    QStringList getValues(const QString &entry) const;

    static const QString VALUES_SEPARATOR;
};

}   // namespace U2

#endif // _U2_SNPEFF_INFO_PARSER_H_
