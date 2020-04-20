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

#ifndef _U2_MSA_COLOR_SCHEME_PERCENTAGE_IDENTENTITY_COLORED_H_
#define _U2_MSA_COLOR_SCHEME_PERCENTAGE_IDENTENTITY_COLORED_H_

#include "../../MsaColorScheme.h"
#include "ColumnCharsCounter.h"

#include <QList>

namespace U2 {

class U2ALGORITHM_EXPORT MsaColorSchemePercentageIdententityColored : public MsaColorScheme {
    Q_OBJECT
public:
    MsaColorSchemePercentageIdententityColored(QObject *parent, const MsaColorSchemeFactory *factory, MultipleAlignmentObject *maObj);

    QColor getBackgroundColor(int rowNum, int columnNum, char c) const override;
    QColor getFontColor(int rowNum, int columnNum, char c) const override;

    void applySettings(const QVariantMap& settings) override;

    static const QList<char> NUCLEOTIDE_LIST;

protected:
    void updateCache(const int columnNum) const;
    virtual int getColorIndex(const int columnNum, const char c) const;

    mutable QMap<qint64, ColumnCharsCounter> cachedData;    //first value - column number

private slots:
    void sl_alignmentChanged();

private:
    static const QList<QColor> BACKGROUND_COLORS;
    static const QList<QColor> FONT_COLORS;

    mutable bool alignmentChanged;
    double threshold;
};

}   // namespace U2

#endif // _U2_MSA_COLOR_SCHEME_PERCENTAGE_IDENTENTITY_COLORED_H_
