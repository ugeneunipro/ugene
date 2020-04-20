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


#include "ConductGOSettings.h"

#include <U2Core/GUrlUtils.h>

namespace U2 {

const QString ConductGOSettings::UNIVERSE_HGU133A = QString("hgu133a.db");
const QString ConductGOSettings::UNIVERSE_HGU133B = QString("hgu133b.db");
const QString ConductGOSettings::UNIVERSE_HGU133PLUS2 = QString("hgu133plus2.db");
const QString ConductGOSettings::UNIVERSE_HGU95AV2 = QString("hgu95av2.db");
const QString ConductGOSettings::UNIVERSE_MOUSE_430a2 = QString("mouse430a2.db");
const QString ConductGOSettings::UNIVERSE_CELEGANS = QString("celegans.db");
const QString ConductGOSettings::UNIVERSE_DROSOPHILA2 = QString("drosophila2.db");
const QString ConductGOSettings::UNIVERSE_ORG_HS_EG = QString("org.Hs.eg.db");
const QString ConductGOSettings::UNIVERSE_ORG_MM_EG = QString("org.Mm.eg.db");
const QString ConductGOSettings::UNIVERSE_ORG_CE_EG = QString("org.Ce.eg.db");
const QString ConductGOSettings::UNIVERSE_ORG_DM_EG = QString("org.Dm.eg.db");

ConductGOSettings::ConductGOSettings() {
    initDefault();
}

void ConductGOSettings::initDefault(){
    outDir = "";
    title = "Default";
    geneUniverse = "hgu133a";
}

QStringList ConductGOSettings::getArguments() const {
    QStringList result;

//    go_analysis.py  '$title' '$diff_expr_file' '$logmeta' '$diff_expr_file.dbkey', '$annotation'

    result << title;

    result << treatUrl;

    result << "logmeta.txt";

    result << geneUniverse + ".dbkey";

    result << geneUniverse;

    return result;
}

} // U2
