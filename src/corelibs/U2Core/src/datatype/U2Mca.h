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

#ifndef _U2_MCA_H_
#define _U2_MCA_H_

#include "U2Msa.h"

namespace U2 {

/**
    Row of multiple chromatogram alignment: gaps map and sequence id
*/
class U2CORE_EXPORT U2McaRow : public U2MsaRow {
public:
    U2McaRow();
    U2McaRow(const U2MsaRow &msaRow);

    bool hasValidChildObjectIds() const;

    U2DataId chromatogramId;
};

class U2CORE_EXPORT U2Mca : public U2Msa {
public:
    U2Mca();
    U2Mca(const U2Msa &dbMsa);
    U2Mca(const U2DataId &id, const QString &dbId, qint64 version);

    U2DataType getType() const;
};

}   // namespace U2

#endif // _U2_MCA_H_
