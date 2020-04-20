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

#ifndef _U2_SEQUENCE_DBI_UNITTESTS_H_
#define _U2_SEQUENCE_DBI_UNITTESTS_H_

#include "core/dbi/DbiTest.h"

#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Test/TestRunnerSettings.h>

#include <unittest.h>

namespace U2 {

class UpdateSequenceArgs {
public:
    int sequenceId;
    QList<U2Region> regionsToReplace;
    QList<QByteArray> datazToInsert;
};

class SequenceTestData {
public:
    static void init();
    static void shutdown();
    static U2SequenceDbi* getSequenceDbi();
    static QList<U2DataId>* getSequences() { return sequences; };
    static bool compareSequences(const U2Sequence& s1, const U2Sequence& s2);
    static void checkUpdateSequence(UnitTest *t, const UpdateSequenceArgs& args);
    static void replaceRegion(UnitTest *t, const QByteArray& originalSequence,
        const QByteArray& dataToInsert,
        const U2Region& region,
        QByteArray& resultSequence);

public:
    static U2SequenceDbi* sequenceDbi;
    static QList<U2DataId>* sequences;

    static const QString& SEQ_DB_URL;

protected:
    static TestDbiProvider dbiProvider;
    static bool registerTest;
};

class SequenceDbiUnitTests_getSequenceObject : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_getAllSequenceObjects : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_getSequenceObjectInvalid : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_createSequenceObject : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_updateSequenceObject : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_getSequenceData : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_getLongSequenceData : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_getSequenceDataInvalid : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_updateSequenceData : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_updateSequencesData : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_updateHugeSequenceData : public UnitTest {
public:
    void Test();
};

class SequenceDbiUnitTests_updateSequencesObject : public UnitTest {
public:
    void Test();
};

} // namespace U2

Q_DECLARE_METATYPE(U2::U2Sequence);
Q_DECLARE_METATYPE(U2::UpdateSequenceArgs);

Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_createSequenceObject);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_getAllSequenceObjects);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_getSequenceData);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_getLongSequenceData);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_getSequenceDataInvalid);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_getSequenceObject);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_getSequenceObjectInvalid);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_updateHugeSequenceData);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_updateSequenceData);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_updateSequenceObject);
Q_DECLARE_METATYPE(U2::SequenceDbiUnitTests_updateSequencesData);

#endif //_U2_SEQUENCE_DBI_UNITTESTS_H_
