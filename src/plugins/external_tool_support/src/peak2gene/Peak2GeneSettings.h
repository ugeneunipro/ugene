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

#ifndef _U2_PEAK2GENE_SETTINGS_
#define _U2_PEAK2GENE_SETTINGS_

#include <QList>
#include <QString>
#include <QStringList>

namespace U2 {

class Peak2GeneSettings {
public:
    static const QString OUT_TYPE_UPSTREAM;
    static const QString OUT_TYPE_DOWNSTREAM;
    static const QString OUT_TYPE_ALL;

    static const QString DEFAULT_NAME;

    Peak2GeneSettings();

    QString     outpos;
    bool        symbol;
    qint64      distance;
    QString     genomePath;

public:
     QStringList getArguments(const QString& treatFilePath);

private:
    void initDefault();
};

} // U2

#endif // _U2_PEAK2GENE_SETTINGS_
