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

#ifndef _U2_MSA_HIGHLIGHTING_SCHEME_H_
#define _U2_MSA_HIGHLIGHTING_SCHEME_H_

#include <QObject>

#include <U2Core/global.h>

class QColor;

namespace U2 {

class MultipleAlignmentObject;
class MsaHighlightingSchemeFactory;

class U2ALGORITHM_EXPORT MsaHighlightingScheme : public QObject {
    Q_OBJECT
public:
    MsaHighlightingScheme(QObject *parent, const MsaHighlightingSchemeFactory *factory, MultipleAlignmentObject *maObj);

    virtual void process(const char refChar, char &seqChar, QColor &color, bool &highlight, int refCharColumn, int refCharRow) const;
    const MsaHighlightingSchemeFactory * getFactory() const;

    void setUseDots(bool use);
    bool getUseDots() const;

    virtual void applySettings(const QVariantMap &settings);
    virtual QVariantMap getSettings() const;

    static const QString EMPTY;
    static const QString AGREEMENTS;
    static const QString DISAGREEMENTS;
    static const QString TRANSITIONS;
    static const QString TRANSVERSIONS;
    static const QString GAPS;
    static const QString CONSERVATION;

    static const QString THRESHOLD_PARAMETER_NAME;
    static const QString LESS_THAN_THRESHOLD_PARAMETER_NAME;

protected:
    const MsaHighlightingSchemeFactory *factory;
    MultipleAlignmentObject *maObj;
    bool useDots;
};

class U2ALGORITHM_EXPORT MsaHighlightingSchemeFactory : public QObject {
    Q_OBJECT
public:
    MsaHighlightingSchemeFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets,
                                 bool refFree = false, bool needThreshold = false);

    virtual MsaHighlightingScheme * create(QObject *parent, MultipleAlignmentObject *maObj) const = 0;

    const QString & getId() const;
    const QString& getName() const;
    bool isRefFree() const;
    bool isNeedThreshold() const;
    bool isAlphabetTypeSupported(DNAAlphabetType alphabetType) const;
    const AlphabetFlags& getSupportedAlphabets() const;
private:
    QString         id;
    QString         name;
    bool            refFree;
    bool            needThreshold;
    AlphabetFlags supportedAlphabets;
};

class U2ALGORITHM_EXPORT MsaHighlightingSchemeRegistry : public QObject {
    Q_OBJECT
public:
    MsaHighlightingSchemeRegistry();
    ~MsaHighlightingSchemeRegistry();

    MsaHighlightingSchemeFactory * getSchemeFactoryById(const QString &id) const;
    MsaHighlightingSchemeFactory *getEmptySchemeFactory() const;
    QList<MsaHighlightingSchemeFactory *> getAllSchemes(DNAAlphabetType alphabetType) const;
    QMap<AlphabetFlags, QList<MsaHighlightingSchemeFactory*> > getAllSchemesGrouped() const;
private:
    QList<MsaHighlightingSchemeFactory *> schemes;
};

}   // namespace U2

#endif // _U2_MSA_HIGHLIGHTING_SCHEME_H_
