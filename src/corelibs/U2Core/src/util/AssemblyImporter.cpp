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

#include <U2Core/L10n.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AssemblyImporter.h"

namespace U2 {

AssemblyImporter::AssemblyImporter(U2OpStatus &os) :
    os(os),
    objectExists(false)
{

}

AssemblyImporter::AssemblyImporter(const U2DbiRef &dbiRef, U2Assembly &assembly, U2OpStatus &os) :
    dbiRef(dbiRef),
    assembly(assembly),
    os(os),
    objectExists(true)
{

}

AssemblyImporter::~AssemblyImporter() {
    finalizeAssembly();
}

void AssemblyImporter::createAssembly(const U2DbiRef &dbiRef, const QString &folder, U2Assembly &assembly) {
    U2AssemblyReadsImportInfo importInfo;
    createAssembly(dbiRef, folder, NULL, importInfo, assembly);
}

void AssemblyImporter::createAssembly(const U2DbiRef &dbiRef, const QString &folder, U2DbiIterator<U2AssemblyRead> *readsIterator, U2AssemblyReadsImportInfo &importInfo, U2Assembly &assembly) {
    this->dbiRef = dbiRef;
    this->assembly = assembly;

    SAFE_POINT(dbiRef.isValid(), "Database reference is invalid", );
    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);

    DbiConnection connection(dbiRef, true, os);
    SAFE_POINT_OP(os, );
    CHECK(!os.isCanceled(), );
    SAFE_POINT(connection.isOpen(), "Connection is closed", );
    U2AssemblyDbi *assemblyDbi = connection.dbi->getAssemblyDbi();
    SAFE_POINT(NULL != assemblyDbi, L10N::nullPointerError("assembly dbi"), );

    assemblyDbi->createAssemblyObject(assembly, canonicalFolder, readsIterator, importInfo, os);

    this->assembly = assembly;
    objectExists = true;
}

void AssemblyImporter::addReads(U2DbiIterator<U2AssemblyRead> *readsIterator) {
    CHECK(objectExists, );
    SAFE_POINT(dbiRef.isValid(), "Database reference is invalid", );
    SAFE_POINT(assembly.hasValidId(), "Assembly ID is invalid", );

    DbiConnection connection(dbiRef, os);
    SAFE_POINT_OP(os, );
    CHECK(!os.isCanceled(), );
    SAFE_POINT(connection.isOpen(), "Connection is closed", );
    U2AssemblyDbi *assemblyDbi = connection.dbi->getAssemblyDbi();
    SAFE_POINT(NULL != assemblyDbi, L10N::nullPointerError("assembly dbi"), );

    assemblyDbi->addReads(assembly.id, readsIterator, os);
}

void AssemblyImporter::packReads(U2AssemblyReadsImportInfo &importInfo) {
    CHECK(!importInfo.packed, );
    CHECK(objectExists, );

    SAFE_POINT(dbiRef.isValid(), "Database reference is invalid", );
    SAFE_POINT(assembly.hasValidId(), "Assembly ID is invalid", );

    DbiConnection connection(dbiRef, os);
    SAFE_POINT_OP(os, );
    CHECK(!os.isCanceled(), );
    SAFE_POINT(connection.isOpen(), "Connection is closed", );
    U2AssemblyDbi *assemblyDbi = connection.dbi->getAssemblyDbi();
    SAFE_POINT(NULL != assemblyDbi, L10N::nullPointerError("assembly dbi"), );

    U2AssemblyPackStat stat;
    assemblyDbi->pack(assembly.id, stat, os);
    importInfo.packStat = stat;
}

bool AssemblyImporter::isObjectExist() const {
    return objectExists;
}

const U2Assembly &AssemblyImporter::getAssembly() const {
    return assembly;
}

void AssemblyImporter::finalizeAssembly() {
    CHECK(objectExists, );

    U2OpStatusImpl innerOs;

    DbiConnection connection(dbiRef, innerOs);
    SAFE_POINT_OP(innerOs, );

    if (connection.dbi->isTransactionActive()) {
        coreLog.trace("Assembly finalization inside a transaction occurred: there can be some troubles");
        Q_ASSERT(false);
    }

    U2AssemblyDbi *assemblyDbi = connection.dbi->getAssemblyDbi();
    SAFE_POINT(NULL != assemblyDbi, L10N::nullPointerError("assembly dbi"), );
    assemblyDbi->finalizeAssemblyObject(assembly, os);
}

}   // namespace U2
