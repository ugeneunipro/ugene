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

#include "MsaImporterExporterUnitTests.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/SQLiteDbi.h>


namespace U2 {

TestDbiProvider MsaImporterExporterTestData::dbiProvider = TestDbiProvider();
const QString& MsaImporterExporterTestData::IMP_EXP_DB_URL("imp-exp-dbi.ugenedb");
U2DbiRef MsaImporterExporterTestData::dbiRef = U2DbiRef();

void MsaImporterExporterTestData::init() {
    bool ok = dbiProvider.init(IMP_EXP_DB_URL, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize in MsaTestData::init()!",);

    U2Dbi* dbi = dbiProvider.getDbi();
    dbiRef = dbi->getDbiRef();
    dbiProvider.close();
}

const U2DbiRef& MsaImporterExporterTestData::getDbiRef() {
    if (dbiRef == U2DbiRef()) {
        init();
    }
    return dbiRef;
}


IMPLEMENT_TEST(MsaImporterExporterUnitTests, importExportAlignment) {
    const U2DbiRef& dbiRef = MsaImporterExporterTestData::getDbiRef();

    U2OpStatusImpl os;

    // Init an alignment
    QString alignmentName = "Test alignment";
    DNAAlphabetRegistry* alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    const DNAAlphabet* alphabet = alphabetRegistry->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());

    QByteArray firstSequence("---AG-T");
    QByteArray secondSequence("AG-CT-TAA");

    MultipleSequenceAlignment al(alignmentName, alphabet);

    al->addRow("First row", firstSequence);
    al->addRow("Second row", secondSequence);

    // Import the alignment
    QScopedPointer<MultipleSequenceAlignmentObject> msaObj(MultipleSequenceAlignmentImporter::createAlignment(dbiRef, al, os));
    CHECK_NO_ERROR(os);

    // Export the alignment
    MultipleSequenceAlignmentExporter alExporter;
    MultipleSequenceAlignment alActual = alExporter.getAlignment(dbiRef, msaObj->getEntityRef().entityId, os);
    CHECK_NO_ERROR(os);

    bool alsEqual = (*al == *alActual);
    CHECK_TRUE(alsEqual, "Actual alignment doesn't equal to the original!");
    CHECK_EQUAL(alignmentName, alActual->getName(), "alignment name");
}


}
