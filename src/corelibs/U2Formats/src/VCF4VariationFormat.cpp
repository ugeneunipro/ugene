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

#include "VCF4VariationFormat.h"

namespace U2 {

VCF4VariationFormat::VCF4VariationFormat(QObject *p)
: AbstractVariationFormat(p, BaseDocumentFormats::VCF4, QStringList()<<"vcf", true)
{
    formatName = QString("VCFv4");

    columnRoles.insert(0, ColumnRole_ChromosomeId);
    columnRoles.insert(1, ColumnRole_StartPos);
    columnRoles.insert(2, ColumnRole_PublicId);
    columnRoles.insert(3, ColumnRole_RefData);
    columnRoles.insert(4, ColumnRole_ObsData);
    columnRoles.insert(7, ColumnRole_Info);

    maxColumnNumber = columnRoles.keys().last();

    indexing = AbstractVariationFormat::OneBased;
}

bool VCF4VariationFormat::checkFormatByColumnCount(int columnCount) const {
    return (columnCount >= maxColumnNumber+1);
}

} // U2
