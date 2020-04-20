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

#include "U2CoreAttributes.h"

namespace U2 {

/************************************************************************/
/* Assembly attributes */
/************************************************************************/
const QString U2BaseAttributeName::reference_length("reference_length_attribute");
const QString U2BaseAttributeName::reference_uri("reference_uri_attribute");
const QString U2BaseAttributeName::reference_md5("reference_md5_attribute");
const QString U2BaseAttributeName::reference_species("reference_species_attribute");
const QString U2BaseAttributeName::max_prow("max_prow_attribute");
const QString U2BaseAttributeName::count_reads("count_reads_attribute");
const QString U2BaseAttributeName::coverage_statistics("coverageStat");

const QStringList U2BaseAttributeName::getReadsRelatedAttributes() {
    QStringList result;
    result.append(count_reads);
    result.append(coverage_statistics);
    result.append(max_prow);
    return result;
}

} // U2
