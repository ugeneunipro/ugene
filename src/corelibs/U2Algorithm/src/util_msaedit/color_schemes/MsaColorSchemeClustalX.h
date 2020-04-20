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

#ifndef _U2_MSA_COLOR_SCHEME_CLUSTAL_X_H_
#define _U2_MSA_COLOR_SCHEME_CLUSTAL_X_H_

#include <QColor>
#include <QVector>

#include "MsaColorScheme.h"

namespace U2 {

// 0.5 * alisize mem use, slow update
class U2ALGORITHM_EXPORT MsaColorSchemeClustalX : public MsaColorScheme {
    Q_OBJECT
public:
    MsaColorSchemeClustalX(QObject *parent, const MsaColorSchemeFactory *factory, MultipleAlignmentObject *maObj);

    QColor getBackgroundColor(int seq, int pos, char c) const override;
    QColor getFontColor(int seq, int pos, char c) const override;

private slots:
    void sl_alignmentChanged();

private:
    void updateCache() const;
    int getCacheIdx(int seq, int pos, bool &low) const;

    int getColorIdx(int seq, int pos) const;
    void setColorIdx(int seq, int pos, int cidx) const;

    enum ClustalColor {
        ClustalColor_NO_COLOR,
        ClustalColor_BLUE,
        ClustalColor_RED,
        ClustalColor_GREEN,
        ClustalColor_PINK,
        ClustalColor_MAGENTA,
        ClustalColor_ORANGE,
        ClustalColor_CYAN,
        ClustalColor_YELLOW,
        ClustalColor_NUM_COLORS
    };

    int                     objVersion;
    mutable int             cacheVersion;
    mutable int             aliLen;
    mutable QVector<quint8> colorsCache;
    QColor                  colorByIdx[ClustalColor_NUM_COLORS];
};

class MsaColorSchemeClustalXFactory : public MsaColorSchemeFactory {
    Q_OBJECT
public:
    MsaColorSchemeClustalXFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets);

    MsaColorScheme * create(QObject *parent, MultipleAlignmentObject *maObj) const;
};

}   // namespace U2

#endif // _U2_MSA_COLOR_SCHEME_CLUSTAL_X_H_
