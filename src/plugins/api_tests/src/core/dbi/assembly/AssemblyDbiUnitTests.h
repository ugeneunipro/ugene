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

#ifndef _U2_ASSEMBLY_DBI_UNIT_TESTS_H_
#define _U2_ASSEMBLY_DBI_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Test/TestRunnerSettings.h>
#include <unittest.h>

namespace U2 {

class AssemblyTestData {
public:
    static void init();
    static void shutdown();
    static U2AssemblyDbi* getAssemblyDbi();
    static void close();
    static QList<U2DataId>* getAssemblyIds() { return assemblyIds; };

public:
    static U2AssemblyDbi* assemblyDbi;
    static QList<U2DataId>* assemblyIds;

    static const QString& ASS_DB_URL;

protected:
    static TestDbiProvider dbiProvider;
    static bool registerTest;
};

class AssemblyDbiUnitTests_getAssemblyObject : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getAssemblyObjectInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_countReads : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_countReadsInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getReads : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getReadsInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getReadsByRow : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getReadsByRowInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getReadsByName : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getReadsByNameInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getMaxPackedRow : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getMaxPackedRowInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getMaxEndPos : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_getMaxEndPosInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_createAssemblyObject : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_removeReads : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_removeReadsInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_addReads : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_addReadsInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_pack : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_packInvalid : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_calculateCoverage : public UnitTest {
public:
    void Test();
};

class AssemblyDbiUnitTests_calculateCoverageInvalid : public UnitTest {
public:
    void Test();
};

}

Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_addReads);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_addReadsInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_calculateCoverage);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_calculateCoverageInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_countReads);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_countReadsInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_createAssemblyObject);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getAssemblyObject);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getAssemblyObjectInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getMaxEndPos);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getMaxEndPosInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getMaxPackedRow);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getMaxPackedRowInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getReads);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getReadsInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getReadsByName);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getReadsByNameInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getReadsByRow);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_getReadsByRowInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_pack);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_packInvalid);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_removeReads);
Q_DECLARE_METATYPE(U2::AssemblyDbiUnitTests_removeReadsInvalid);

#endif //_U2_ASSEMBLY_DBI_UNITTESTS_H_
