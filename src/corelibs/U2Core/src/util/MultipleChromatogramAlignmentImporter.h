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

#ifndef _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_IMPORTER_H_
#define _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_IMPORTER_H_

#include "McaRowInnerData.h"

namespace U2 {

class DbiConnection;
class MultipleChromatogramAlignment;
class MultipleChromatogramAlignmentObject;
class U2OpStatus;

class U2CORE_EXPORT MultipleChromatogramAlignmentImporter {
public:
    static MultipleChromatogramAlignmentObject * createAlignment(U2OpStatus &os, const U2DbiRef &dbiRef, const QString &folder, MultipleChromatogramAlignment &mca);

private:
    static U2Mca importMcaObject(U2OpStatus &os, const DbiConnection &connection, const QString &folder, const MultipleChromatogramAlignment &mca);
    static void importMcaInfo(U2OpStatus &os, const DbiConnection &connection, const U2DataId &mcaId, const MultipleChromatogramAlignment &mca);
    static QList<McaRowDatabaseData> importRowChildObjects(U2OpStatus &os, const DbiConnection &connection, const QString &folder, const MultipleChromatogramAlignment &mca);
    static QList<U2McaRow> importRows(U2OpStatus &os, const DbiConnection &connection, U2Mca &dbMca, const QList<McaRowDatabaseData> &mcaRowDatabaseData);
    static U2Chromatogram importChromatogram(U2OpStatus &os, const DbiConnection &connection, const QString &folder, const DNAChromatogram &chromatogram);
    static U2Sequence importSequence(U2OpStatus &os, const DbiConnection &connection, const QString &folder, const DNASequence &sequence, const U2AlphabetId &alphabetId);
    static void importRowAdditionalInfo(U2OpStatus &os, const DbiConnection &connection, const U2Chromatogram &chromatogram, const QVariantMap &additionalInfo);
    static void createRelation(U2OpStatus &os, const DbiConnection &connection, const U2Sequence &sequence, const U2DataId &chromatogramId);
};

}   // namespace U2

#endif // _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_IMPORTER_H_
