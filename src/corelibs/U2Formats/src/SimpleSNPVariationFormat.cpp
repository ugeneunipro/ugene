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

#include "SimpleSNPVariationFormat.h"

namespace U2 {

SimpleSNPVariationFormat::SimpleSNPVariationFormat(QObject *p)
: AbstractVariationFormat(p, BaseDocumentFormats::SNP, QStringList()<<"snp")
{
    formatName = QString("SimpleSNP");

    columnRoles.insert(0, ColumnRole_ChromosomeId);
    columnRoles.insert(1, ColumnRole_StartPos);
    columnRoles.insert(2, ColumnRole_RefData);
    columnRoles.insert(3, ColumnRole_ObsData);

    maxColumnNumber = columnRoles.keys().last();

    indexing = AbstractVariationFormat::ZeroBased;
}

bool SimpleSNPVariationFormat::checkFormatByColumnCount(int columnCount) const {
    return (columnCount == maxColumnNumber+1);
}

} // U2
