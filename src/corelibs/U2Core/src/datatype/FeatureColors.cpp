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

#include "FeatureColors.h"

namespace U2 {

static QList<QColor> prepareColors() {
    QList<QColor> colors;
    QString mustHave = "FF";
    QStringList optional;
    optional << "FF" << "CC" << "99" << "66" << "33" << "00";
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < optional.size(); j++) {
            for(int k = 1; k < optional.size(); k++) { //todo: avoid duplicates
                QString colorName =
                    i == 0 ? mustHave + optional[j] + optional[k] :
                    i == 1 ? optional[j] + mustHave + optional[k] :
                    optional[j] + optional[k] + mustHave;
                QColor c("#" + colorName);
                assert(c.isValid());
                colors.append(c);
            }
        }
    }
    return colors;
}

QColor FeatureColors::genLightColor(const QString& name) {
    static QList<QColor> colors = prepareColors();

    int hash = 0;
    QByteArray a = name.toLatin1();
    for (int i = 0; i < a.size(); i++) {
        hash += a[i];
    }
    QColor c = colors.at((hash*hash)%colors.size());
    return c;
}

} //endif
