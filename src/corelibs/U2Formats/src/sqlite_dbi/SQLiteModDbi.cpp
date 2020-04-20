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

#include "SQLiteModDbi.h"

#include <QCoreApplication>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/SQLiteObjectDbi.h>


namespace U2 {

/************************************************************************/
/* U2UseCommonMultiModStep                                              */
/************************************************************************/
U2UseCommonMultiModStep::U2UseCommonMultiModStep(SQLiteDbi *_sqliteDbi, const U2DataId &_masterObjId, U2OpStatus& os)
: sqliteDbi(_sqliteDbi),
  valid(false),
  masterObjId(_masterObjId)
{
    SAFE_POINT(NULL != sqliteDbi, "NULL sqliteDbi!", );
    QMutexLocker m(&sqliteDbi->getDbRef()->lock);

    sqliteDbi->getSQLiteModDbi()->startCommonMultiModStep(masterObjId, os);
    if (!os.hasError()) {
        valid = true;
    }
}

U2UseCommonMultiModStep::~U2UseCommonMultiModStep() {
    SAFE_POINT(NULL != sqliteDbi, "NULL sqliteDbi!", );
    QMutexLocker m(&sqliteDbi->getDbRef()->lock);
    if (valid) {
        U2OpStatus2Log os;
        sqliteDbi->getSQLiteModDbi()->endCommonMultiModStep(masterObjId, os);
    }
}

/************************************************************************/
/* ModStepsDescriptor                                                   */
/************************************************************************/

ModStepsDescriptor:: ModStepsDescriptor()
    : userModStepId(-1),
      multiModStepId(-1),
      removeUserStepWithMulti(false){}

/************************************************************************/
/* SQLiteModDbi                                                         */
/************************************************************************/
QMap<U2DataId, ModStepsDescriptor> SQLiteModDbi::modStepsByObject;

SQLiteModDbi::SQLiteModDbi(SQLiteDbi *dbi) : U2ModDbi(dbi), SQLiteChildDBICommon(dbi) {
}

void SQLiteModDbi::initSqlSchema(U2OpStatus &os) {
    if (os.hasError()) {
        return;
    }

    // UserModStep - user modification steps
    //   id                     - id of the user modifications step
    //   object, otype, oextra  - data id of the master object (i.e. object for which "undo/redo" was initiated)
    //   version                - master object was modified from this version
    SQLiteWriteQuery("CREATE TABLE UserModStep (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        " object INTEGER NOT NULL,"
        " otype INTEGER NOT NULL,"
        " oextra BLOB NOT NULL,"
        " version INTEGER NOT NULL, "
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE)", db, os).execute();

    // MultiModStep - multiple modifications step with reference to a user modifications step
    //   id          - id of the multiple modifications step
    //   userStepId  - id of the user modifications step
    SQLiteWriteQuery("CREATE TABLE MultiModStep (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        " userStepId INTEGER NOT NULL,"
        " FOREIGN KEY(userStepId) REFERENCES UserModStep(id) ON DELETE CASCADE)", db, os).execute();

    // SingleModStep - single modification of a dbi object
    //   id                    - id of the modification
    //   object, otype, oextra - data id of the object that was modified
    //   version               - this is a modification from 'version' to 'version + 1' of the object
    //   modType               - type of the object modification
    //   details               - detailed description of the object modification
    //   multiStepId           - id of the multiModStep
    SQLiteWriteQuery("CREATE TABLE SingleModStep (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        " object INTEGER NOT NULL,"
        " otype INTEGER NOT NULL,"
        " oextra BLOB NOT NULL,"
        " version INTEGER NOT NULL,"
        " modType INTEGER NOT NULL,"
        " details TEXT NOT NULL,"
        " multiStepId INTEGER NOT NULL, "
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE, "
        " FOREIGN KEY(multiStepId) REFERENCES MultiModStep(id) ON DELETE CASCADE)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX SingleModStep_object ON SingleModStep(object)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX SingleModStep_object_version ON SingleModStep(object, version)", db, os).execute();
}

U2SingleModStep SQLiteModDbi::getModStep(const U2DataId &objectId, qint64 trackVersion, U2OpStatus &os) {
    U2SingleModStep res;
    SQLiteReadQuery q("SELECT id, object, otype, oextra, version, modType, details, multiStepId FROM SingleModStep WHERE object = ?1 AND version = ?2 ORDER BY id", db, os);
    SAFE_POINT_OP(os, res);

    q.bindDataId(1, objectId);
    q.bindInt64(2, trackVersion);

    if (q.step()) {
        res.id = q.getInt64(0);
        res.objectId = q.getDataIdExt(1);
        res.version = q.getInt64(4);
        res.modType = q.getInt64(5);
        res.details = q.getBlob(6);
        q.ensureDone();
    }
    else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("An object single modification step not found!"));
    }

    return res;
}

qint64 SQLiteModDbi::getNearestUserModStepVersion(const U2DataId &masterObjId, qint64 version, U2OpStatus &os) {
    SQLiteReadQuery qVersion("SELECT MAX(version) FROM UserModStep WHERE object = ?1 AND version <= ?2", db, os);
    qVersion.bindDataId(1, masterObjId);
    qVersion.bindInt64(2, version);

    qint64 userStepVersion = version;
    if (qVersion.step()) {
        userStepVersion = qVersion.getInt64(0);
    }
    SAFE_POINT_OP(os, userStepVersion);
    return userStepVersion;
}

QList< QList<U2SingleModStep> > SQLiteModDbi::getModSteps(const U2DataId &masterObjId, qint64 version, U2OpStatus &os) {
    QList< QList<U2SingleModStep> > steps;
    SQLiteTransaction t(db, os);

    qint64 userStepId = -1;
    SQLiteWriteQuery qGetUserStepId("SELECT id FROM UserModStep WHERE object = ?1 AND version = ?2", db, os);
    SAFE_POINT_OP(os, QList< QList<U2SingleModStep> >());

    qGetUserStepId.bindDataId(1, masterObjId);
    qGetUserStepId.bindInt64(2, version);

    if (qGetUserStepId.step()) {
        userStepId = qGetUserStepId.getInt64(0);
        qGetUserStepId.ensureDone();
    }
    else if (!os.hasError()) {
        os.setError("Failed to find user step ID!");
        return steps;
    }

    SQLiteReadQuery qMultiStepId("SELECT id FROM MultiModStep WHERE userStepId = ?1", db, os);
    qMultiStepId.bindInt64(1, userStepId);

    SQLiteReadQuery qSingleStep("SELECT id, object, otype, oextra, version, modType, details, multiStepId FROM SingleModStep WHERE multiStepId = ?1", db, os);
    while (qMultiStepId.step()) {
        qint64 multiStepId = qMultiStepId.getInt64(0);

        qSingleStep.reset();
        qSingleStep.bindInt64(1, multiStepId);

        QList<U2SingleModStep> currentMultiStepSingleSteps;

        while (qSingleStep.step()) {
            U2SingleModStep step;
            step.id = qSingleStep.getInt64(0);
            step.objectId = qSingleStep.getDataIdExt(1);
            step.version = qSingleStep.getInt64(4);
            step.modType = qSingleStep.getInt64(5);
            step.details = qSingleStep.getBlob(6);

            SAFE_POINT_OP(os, QList< QList<U2SingleModStep> >());
            currentMultiStepSingleSteps.append(step);
        }
        steps.append(currentMultiStepSingleSteps);
    }
    return steps;
}

void SQLiteModDbi::createModStep(const U2DataId &masterObjId, U2SingleModStep &step, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    bool closeMultiStep = false;
    if (!isMultiStepStarted(masterObjId)) {
        startCommonMultiModStep(masterObjId, os);
        SAFE_POINT_OP(os, );
        SAFE_POINT(isMultiStepStarted(masterObjId), "A multiple modifications step must have been started!", );
        closeMultiStep = true;
    }

    SQLiteWriteQuery qSingle("INSERT INTO SingleModStep(object, otype, oextra, version, modType, details, multiStepId) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7) ", db, os);
    SAFE_POINT_OP(os, );

    qSingle.bindDataId(1, step.objectId);
    qSingle.bindType(2, U2DbiUtils::toType(step.objectId));
    qSingle.bindBlob(3, U2DbiUtils::toDbExtra(step.objectId));
    qSingle.bindInt64(4, step.version);
    qSingle.bindInt64(5, step.modType);
    qSingle.bindBlob(6, step.details);
    qSingle.bindInt64(7, modStepsByObject[masterObjId].multiModStepId);

    step.id = qSingle.insert();
    step.multiStepId = modStepsByObject[masterObjId].multiModStepId;

    if (closeMultiStep) {
        endCommonMultiModStep(masterObjId, os);
    }
}

void SQLiteModDbi::removeModsWithGreaterVersion(const U2DataId &masterObjId, qint64 masterObjVersion, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    // Get user step IDs
    QList<qint64> userStepIds;
    SQLiteReadQuery qSelectUserSteps("SELECT id FROM UserModStep WHERE object = ?1 AND version >= ?2", db, os);
    SAFE_POINT_OP(os, );

    qSelectUserSteps.bindDataId(1, masterObjId);
    qSelectUserSteps.bindInt64(2, masterObjVersion);

    while (qSelectUserSteps.step()) {
        qint64 userStepId = qSelectUserSteps.getInt64(0);
        userStepIds.append(userStepId);
    }
    SAFE_POINT_OP(os, );

    // Remove all affected steps (user, multi, single)
    removeSteps(userStepIds, os);
    SAFE_POINT_OP(os, );
}

void SQLiteModDbi::removeDuplicateUserStep(const U2DataId &masterObjId, qint64 masterObjVersion, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    // Get user step IDs
    QList<qint64> userStepIds;
    SQLiteReadQuery qSelect("SELECT id FROM UserModStep WHERE object = ?1 AND version = ?2", db, os);
    SAFE_POINT_OP(os, );

    qSelect.bindDataId(1, masterObjId);
    qSelect.bindInt64(2, masterObjVersion);

    while (qSelect.step()) {
        qint64 id = qSelect.getInt64(0);
        userStepIds.append(id);
    }
    SAFE_POINT_OP(os, );

    if (userStepIds.count() < 2) {
        return;
    }

    assert(2 == userStepIds.count());

    // Don't take into account user step with the greatest id
    userStepIds.removeLast();

    // Remove user step with lower ID
    removeSteps(userStepIds, os);
}

void SQLiteModDbi::removeSteps(QList<qint64> userStepIds, U2OpStatus &os) {
    if (userStepIds.isEmpty()) {
        return;
    }

    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    // Get multiple steps IDs
    QList<qint64> multiStepIds;
    SQLiteReadQuery qSelectMultiSteps("SELECT id FROM MultiModStep WHERE userStepId = ?1", db, os);
    SAFE_POINT_OP(os, );
    foreach (qint64 userStepId, userStepIds) {
        qSelectMultiSteps.reset();
        qSelectMultiSteps.bindInt64(1, userStepId);

        while (qSelectMultiSteps.step()) {
            qint64 multiStepId = qSelectMultiSteps.getInt64(0);
            multiStepIds.append(multiStepId);
        }
    }

    // Remove single steps
    SQLiteWriteQuery qDeleteSingleSteps("DELETE FROM SingleModStep WHERE multiStepId = ?1", db, os);
    SAFE_POINT_OP(os, );
    foreach (qint64 multiStepId, multiStepIds) {
        qDeleteSingleSteps.reset();
        qDeleteSingleSteps.bindInt64(1, multiStepId);
        qDeleteSingleSteps.execute();
    }
    SAFE_POINT_OP(os, );

    // Remove multiple steps
    SQLiteWriteQuery qDeleteMultiSteps("DELETE FROM MultiModStep WHERE id = ?1", db, os);
    SAFE_POINT_OP(os, );
    foreach (qint64 multiStepId, multiStepIds) {
        qDeleteMultiSteps.reset();
        qDeleteMultiSteps.bindInt64(1, multiStepId);
        qDeleteMultiSteps.execute();
    }

    // Remove user steps
    SQLiteWriteQuery qDeleteUserSteps("DELETE FROM UserModStep WHERE id = ?1", db, os);
    foreach (qint64 userStepId, userStepIds) {
        qDeleteUserSteps.reset();
        qDeleteUserSteps.bindInt64(1, userStepId);
        qDeleteUserSteps.execute();
    }
}

void SQLiteModDbi::removeObjectMods(const U2DataId &objectId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    // Get user step IDs
    QList<qint64> userStepIds;
    SQLiteReadQuery qSelectUserSteps("SELECT id FROM UserModStep WHERE object = ?1", db, os);
    SAFE_POINT_OP(os, );

    qSelectUserSteps.bindDataId(1, objectId);

    while (qSelectUserSteps.step()) {
        qint64 userStepId = qSelectUserSteps.getInt64(0);
        userStepIds.append(userStepId);
    }
    SAFE_POINT_OP(os, );

    // Remove all affected steps (user, multi, single)
    removeSteps(userStepIds, os);
    SAFE_POINT_OP(os, );
}

void SQLiteModDbi::cleanUpAllStepsOnError() {
    U2OpStatus2Log os;
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    SQLiteWriteQuery("DELETE FROM SingleModStep", db, os).execute();
    SQLiteWriteQuery("DELETE FROM MultiModStep", db, os).execute();
    SQLiteWriteQuery("DELETE FROM UserModStep", db, os).execute();
}

static void checkMainThread(U2OpStatus &os) {
    QThread *mainThread = QCoreApplication::instance()->thread();
    QThread *thisThread = QThread::currentThread();

    if (mainThread != thisThread) {
        os.setError("Not main thread");
    }
}

void SQLiteModDbi::startCommonUserModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    checkMainThread(os);
    CHECK_OP(os, );
    SQLiteTransaction t(db, os);

    // Only one common step at a time
    if (isUserStepStarted(masterObjId)) {
        os.setError("Can't create a common user modifications step, previous one is not complete!");
        return;
    }

    if(!modStepsByObject.contains(masterObjId)) {
        modStepsByObject[masterObjId] = ModStepsDescriptor();
    }

    // Create a new user modifications step in the database
    createUserModStep(masterObjId, os);
    SAFE_POINT_OP(os, );

}

void SQLiteModDbi::endCommonUserModStep(const U2DataId &userMasterObjId, U2OpStatus &os) {
    checkMainThread(os);
    CHECK_OP(os, );
    SAFE_POINT(modStepsByObject.contains(userMasterObjId), QString("There are not modification steps for object with id %1").arg(userMasterObjId.toLong()),);

    qint64 userModStepId = modStepsByObject[userMasterObjId].userModStepId;
    qint64 multiModStepId = modStepsByObject[userMasterObjId].multiModStepId;

    modStepsByObject.remove(userMasterObjId);

    if (-1 == multiModStepId) {
        SQLiteTransaction t(db, os);
        Q_UNUSED(t);

        // Get multiple steps IDs
        SQLiteReadQuery qSelectMultiSteps("SELECT id FROM MultiModStep WHERE userStepId = ?1", db, os);
        SAFE_POINT_OP(os, );

        qSelectMultiSteps.bindInt64(1, userModStepId);

        // If user modification step doesn't contain any multi modification steps
        if (!qSelectMultiSteps.step()) {
            SQLiteWriteQuery qDeleteUserSteps("DELETE FROM UserModStep WHERE id = ?1", db, os);
            qDeleteUserSteps.bindInt64(1, userModStepId);
            qDeleteUserSteps.execute();
            SAFE_POINT_OP(os, );
        }
    }
}

void SQLiteModDbi::startCommonMultiModStep(const U2DataId &userMasterObjId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    if(!modStepsByObject.contains(userMasterObjId)) {
        modStepsByObject[userMasterObjId] = ModStepsDescriptor();
    }
    if (!isUserStepStarted(userMasterObjId)) {
        startCommonUserModStep(userMasterObjId, os);
        SAFE_POINT_OP(os, );
        SAFE_POINT(isUserStepStarted(userMasterObjId), "A user modifications step must have been started!", );
        modStepsByObject[userMasterObjId].removeUserStepWithMulti = true;
    }
    else {
        modStepsByObject[userMasterObjId].removeUserStepWithMulti = false;
    }

    if (isMultiStepStarted(userMasterObjId)) {
        os.setError("Can't create a common multiple modifications step, previous one is not complete!");
        U2OpStatus2Log innerOs;
        endCommonUserModStep(userMasterObjId, innerOs);
        return;
    }

    // Create a new multiple modifications step in the database
    createMultiModStep(userMasterObjId, os);
    SAFE_POINT_OP(os, );
}

void SQLiteModDbi::endCommonMultiModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    if (modStepsByObject[masterObjId].removeUserStepWithMulti) {
        endCommonUserModStep(masterObjId, os);
    }
    else {
        modStepsByObject[masterObjId].multiModStepId = -1;
    }
}


void SQLiteModDbi::createUserModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    qint64 masterObjVersion = dbi->getSQLiteObjectDbi()->getObjectVersion(masterObjId, os);
    SAFE_POINT_OP(os, );

    SQLiteWriteQuery qUser("INSERT INTO UserModStep(object, otype, oextra, version) VALUES(?1, ?2, ?3, ?4)", db, os);
    SAFE_POINT_OP(os, );

    qUser.bindDataId(1, masterObjId);
    qUser.bindType(2, U2DbiUtils::toType(masterObjId));
    qUser.bindBlob(3, U2DbiUtils::toDbExtra(masterObjId));
    qUser.bindInt64(4, masterObjVersion);

    qint64 curUserModStepId = qUser.insert();

    if (-1 == curUserModStepId) {
        os.setError("Failed to create a common user modifications step!");
        return;
    }
    else {
        modStepsByObject[masterObjId].userModStepId = curUserModStepId;
    }
}

void SQLiteModDbi::createMultiModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    SAFE_POINT(isUserStepStarted(masterObjId), "A user modifications step must have been started!", );

    SQLiteWriteQuery qMulti("INSERT INTO MultiModStep(userStepId) VALUES(?1)", db, os);
    SAFE_POINT_OP(os, );

    qMulti.bindInt64(1, modStepsByObject[masterObjId].userModStepId);

    qint64 curMultiModStepId = qMulti.insert();

    if (-1 == curMultiModStepId) {
        os.setError("Failed to create a common multiple modifications step!");
        return;
    } else {
        modStepsByObject[masterObjId].multiModStepId = curMultiModStepId;
    }
}

bool SQLiteModDbi::isUserStepStarted(const U2DataId& userMasterObjId) {
    if(!modStepsByObject.contains(userMasterObjId)) {
        return false;
    }
    return modStepsByObject[userMasterObjId].userModStepId != -1;
}
bool SQLiteModDbi::isMultiStepStarted(const U2DataId& userMasterObjId) {
    if(!modStepsByObject.contains(userMasterObjId)) {
        return false;
    }
    return modStepsByObject[userMasterObjId].multiModStepId != -1;
}

bool SQLiteModDbi::canUndo(const U2DataId &objectId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);

    // Get current object version
    qint64 objVersion = dbi->getSQLiteObjectDbi()->getObjectVersion(objectId, os);
    SAFE_POINT_OP(os, false);

    // Verify if there are steps
    SQLiteReadQuery q("SELECT id FROM UserModStep WHERE object = ?1 AND version < ?2", db, os);
    SAFE_POINT_OP(os, false);

    q.bindDataId(1, objectId);
    q.bindInt64(2, objVersion);

    if (q.step()) {
        return true;
    }

    return false;
}

bool SQLiteModDbi::canRedo(const U2DataId &objectId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);

    // Get current object version
    qint64 objVersion = dbi->getSQLiteObjectDbi()->getObjectVersion(objectId, os);
    SAFE_POINT_OP(os, false);

    // Verify if there are steps
    SQLiteReadQuery q("SELECT id FROM UserModStep WHERE object = ?1 AND version >= ?2", db, os);
    SAFE_POINT_OP(os, false);

    q.bindDataId(1, objectId);
    q.bindInt64(2, objVersion);

    if (q.step()) {
        return true;
    }

    return false;
}

} // namespace
