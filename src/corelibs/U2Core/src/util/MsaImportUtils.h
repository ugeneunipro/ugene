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

#include <U2Core/ChromatogramObject.h>
#include <U2Core/Msa.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2Sequence.h>

namespace U2 {

class DbiConnection;
class MsaObject;
class U2DbiRef;
class U2OpStatus;

class U2CORE_EXPORT MsaDbRowSnapshot {
public:
    U2Chromatogram chromatogram;
    U2Sequence sequence;
    QVector<U2MsaGap> gapModel;
    qint64 rowLength = 0;
    QVariantMap additionalInfo;
};

/** Utilities to import multiple alignment into database. */
class U2CORE_EXPORT MsaImportUtils {
public:
    /**
     * Creates an alignment in the db.
     * The alignment is completely removed in case of an error.
     */
    static MsaObject* createMsaObject(const U2DbiRef& dbiRef,
                                                    Msa& al,
                                                    U2OpStatus& os,
                                                    const QString& folder = U2ObjectDbi::ROOT_FOLDER);

    static MsaObject* createMcaObject(const U2DbiRef& dbiRef,
                                                    Msa& mca,
                                                    U2OpStatus& os,
                                                    const QString& folder = U2ObjectDbi::ROOT_FOLDER);

private:
    static U2DataId createEmptyMsaObject(const DbiConnection& con, const QString& folder, const QString& name, const DNAAlphabet* alphabet, U2OpStatus& os);
    static void importMsaInfo(const DbiConnection& con, const U2DataId& msaId, const QVariantMap& alInfo, U2OpStatus& os);
    static QList<U2Sequence> importMsaSequences(const DbiConnection& con, const QString& folder, const Msa& al, U2OpStatus& os);
    static QList<U2MsaRow> importMsaRows(const DbiConnection& con, Msa& al, const U2DataId& msaId, const QList<U2Sequence>& rows, const QList<QVector<U2MsaGap>>& msaGapModel, U2OpStatus& os);

    static U2Msa importMcaObject(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const Msa& mca);
    static void importMcaInfo(U2OpStatus& os, const DbiConnection& connection, const U2DataId& mcaId, const Msa& mca);
    static QList<MsaDbRowSnapshot> importRowChildObjects(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const Msa& mca);
    static QList<U2MsaRow> importMcaRows(U2OpStatus& os, const DbiConnection& connection, U2Msa& dbMca, const QList<MsaDbRowSnapshot>& snapshots);
    static U2Chromatogram importChromatogram(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const Chromatogram& chromatogram);
    static U2Sequence importReferenceSequence(U2OpStatus& os, const DbiConnection& connection, const QString& folder, const DNASequence& sequence, const U2AlphabetId& alphabetId);
    static void importRowAdditionalInfo(U2OpStatus& os, const DbiConnection& connection, const U2Chromatogram& chromatogram, const QVariantMap& additionalInfo);
    static void createRelation(U2OpStatus& os, const DbiConnection& connection, const U2Sequence& sequence, const U2DataId& chromatogramId);
};

}  // namespace U2
