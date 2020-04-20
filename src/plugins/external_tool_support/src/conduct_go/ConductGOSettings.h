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

#ifndef _U2_CONDUCT_GO_SETTINGS_
#define _U2_CONDUCT_GO_SETTINGS_

#include <QString>
#include <QStringList>

namespace U2 {


class ConductGOSettings {
public:
    const static QString UNIVERSE_HGU133A;
    const static QString UNIVERSE_HGU133B;
    const static QString UNIVERSE_HGU133PLUS2;
    const static QString UNIVERSE_HGU95AV2;
    const static QString UNIVERSE_MOUSE_430a2;
    const static QString UNIVERSE_CELEGANS;
    const static QString UNIVERSE_DROSOPHILA2;
    const static QString UNIVERSE_ORG_HS_EG;
    const static QString UNIVERSE_ORG_MM_EG;
    const static QString UNIVERSE_ORG_CE_EG;
    const static QString UNIVERSE_ORG_DM_EG;

    ConductGOSettings();

    QString     outDir;
    QString     title;
    QString     treatUrl;
    QString     geneUniverse;

public:
     QStringList getArguments() const;

private:
    void initDefault();
};

} // U2

#endif // _U2_CONDUCT_GO_SETTINGS_
