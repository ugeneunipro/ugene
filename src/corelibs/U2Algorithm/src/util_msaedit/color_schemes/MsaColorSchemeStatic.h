/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QColor>
#include <QVector>

#include "MsaColorScheme.h"

namespace U2 {

class U2ALGORITHM_EXPORT MsaColorSchemeStatic : public MsaColorScheme {
    Q_OBJECT
public:
    MsaColorSchemeStatic(QObject* parent, const MsaColorSchemeFactory* factory, MsaObject* maObj, const QVector<QColor>& colorsPerChar);

    QColor getBackgroundColor(int seq, int pos, char c) const override;
    QColor getFontColor(int seq, int pos, char c) const override;

private:
    QVector<QColor> colorsPerChar;
};

class MsaColorSchemeStaticFactory : public MsaColorSchemeFactory {
    Q_OBJECT
public:
    MsaColorSchemeStaticFactory(QObject* parent, const QString& id, const QString& name, const AlphabetFlags& supportedAlphabets, const QVector<QColor>& colorsPerChar);

    MsaColorScheme* create(QObject* parent, MsaObject* maObj) const override;

private:
    QVector<QColor> colorsPerChar;
};

}  // namespace U2
