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

#ifndef _U2_ASSEMBLY_IMPORTER_H_
#define _U2_ASSEMBLY_IMPORTER_H_

#include <U2Core/AssemblyObject.h>
#include <U2Core/U2Assembly.h>

namespace U2 {

class U2AssemblyReadsImportInfo;

class U2CORE_EXPORT AssemblyImporter {
    Q_DISABLE_COPY(AssemblyImporter)
public:
    AssemblyImporter(U2OpStatus &os);
    AssemblyImporter(const U2DbiRef &dbiRef, U2Assembly &assembly, U2OpStatus &os);
    ~AssemblyImporter();

    void createAssembly(const U2DbiRef &dbiRef, const QString &folder, U2Assembly &assembly);
    void createAssembly(const U2DbiRef &dbiRef, const QString &folder, U2DbiIterator<U2AssemblyRead> *readsIterator, U2AssemblyReadsImportInfo &importInfo, U2Assembly &assembly);

    void addReads(U2DbiIterator<U2AssemblyRead> *readsIterator);
    void packReads(U2AssemblyReadsImportInfo &importInfo);

    bool isObjectExist() const;
    const U2Assembly & getAssembly() const;

private:
    void finalizeAssembly();

    U2DbiRef dbiRef;
    U2Assembly assembly;
    U2OpStatus &os;
    bool objectExists;
};

}   // namespace U2

#endif // _U2_ASSEMBLY_IMPORTER_H_
