/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "McaRowInnerData.h"

namespace U2 {

class DbiConnection;
class MultipleAlignment;
class MultipleChromatogramAlignmentObject;
class U2OpStatus;

class U2CORE_EXPORT MultipleChromatogramAlignmentImporter {
public:
    static MultipleChromatogramAlignmentObject* createAlignment(U2OpStatus& os, const U2DbiRef& dbiRef, const QString& folder, MultipleAlignment& mca);

private:
    static U2Msa importMcaObject(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const MultipleAlignment& mca);
    static void importMcaInfo(U2OpStatus& os, const DbiConnection& connection, const U2DataId& mcaId, const MultipleAlignment& mca);
    static QList<McaRowDatabaseData> importRowChildObjects(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const MultipleAlignment& mca);
    static QList<U2MsaRow> importRows(U2OpStatus& os, const DbiConnection& connection, U2Msa& dbMca, const QList<McaRowDatabaseData>& mcaRowDatabaseData);
    static U2Chromatogram importChromatogram(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const DNAChromatogram& chromatogram);
    static U2Sequence importSequence(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const DNASequence& sequence, const U2AlphabetId& alphabetId);
    static void importRowAdditionalInfo(U2OpStatus& os, const DbiConnection& connection, const U2Chromatogram& chromatogram, const QVariantMap& additionalInfo);
    static void createRelation(U2OpStatus& os, const DbiConnection& connection, const U2Sequence& sequence, const U2DataId& chromatogramId);
};

}  // namespace U2
