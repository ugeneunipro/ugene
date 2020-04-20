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

#include <QCoreApplication>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "util/MysqlHelpers.h"
#include "MysqlModDbi.h"
#include "MysqlObjectDbi.h"

namespace U2 {

/************************************************************************/
/* MysqlUseCommonMultiModStep                                           */
/************************************************************************/
MysqlUseCommonMultiModStep::MysqlUseCommonMultiModStep(MysqlDbi *_mysqlDbi, const U2DataId &_masterObjId, U2OpStatus& os) :
    mysqlDbi(_mysqlDbi),
    valid(false),
    masterObjId(_masterObjId)
{
    CHECK_OP(os, );
    SAFE_POINT(NULL != mysqlDbi, "Dbi is NULL", );

    mysqlDbi->getMysqlModDbi()->startCommonMultiModStep(masterObjId, os);
    if (!os.hasError()) {
        valid = true;
    }
}

MysqlUseCommonMultiModStep::~MysqlUseCommonMultiModStep() {
    SAFE_POINT(NULL != mysqlDbi, "Dbi is NULL", );
    if (valid) {
        U2OpStatus2Log os;
        mysqlDbi->getMysqlModDbi()->endCommonMultiModStep(masterObjId, os);
    }
}

/************************************************************************/
/* ModStepsDescriptor                                                   */
/************************************************************************/

MysqlModStepsDescriptor::MysqlModStepsDescriptor() :
    userModStepId(-1),
    multiModStepId(-1),
    removeUserStepWithMulti(false) {}

/************************************************************************/
/* MysqlModDbi                                                          */
/************************************************************************/
QMap<U2DataId, MysqlModStepsDescriptor> MysqlModDbi::modStepsByObject;

MysqlModDbi::MysqlModDbi(MysqlDbi *dbi) : U2ModDbi(dbi), MysqlChildDbiCommon(dbi) {
}

void MysqlModDbi::initSqlSchema(U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // UserModStep - user modification steps
    //   id                     - id of the user modifications step
    //   object, otype, oextra  - data id of the master object (i.e. object for which "undo/redo" was initiated)
    //   version                - master object was modified from this version
    U2SqlQuery("CREATE TABLE UserModStep (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,"
        " object BIGINT NOT NULL,"
        " otype INTEGER NOT NULL,"
        " oextra LONGBLOB NOT NULL,"
        " version LONGBLOB NOT NULL, "
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // MultiModStep - multiple modifications step with reference to a user modifications step
    //   id          - id of the multiple modifications step
    //   userStepId  - id of the user modifications step
    U2SqlQuery("CREATE TABLE MultiModStep (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,"
        " userStepId BIGINT NOT NULL,"
        " FOREIGN KEY(userStepId) REFERENCES UserModStep(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // SingleModStep - single modification of a dbi object
    //   id                    - id of the modification
    //   object, otype, oextra - data id of the object that was modified
    //   version               - this is a modification from 'version' to 'version + 1' of the object
    //   modType               - type of the object modification
    //   details               - detailed description of the object modification
    //   multiStepId           - id of the multiModStep
    U2SqlQuery("CREATE TABLE SingleModStep (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,"
        " object BIGINT NOT NULL,"
        " otype INTEGER NOT NULL,"
        " oextra LONGBLOB NOT NULL,"
        " version BIGINT NOT NULL,"
        " modType INTEGER NOT NULL,"
        " details LONGTEXT NOT NULL,"
        " multiStepId BIGINT NOT NULL, "
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE, "
        " FOREIGN KEY(multiStepId) REFERENCES MultiModStep(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    U2SqlQuery("CREATE INDEX SingleModStep_object ON SingleModStep(object)", db, os).execute();
    U2SqlQuery("CREATE INDEX SingleModStep_object_version ON SingleModStep(object, version)", db, os).execute();
}

U2SingleModStep MysqlModDbi::getModStep(const U2DataId &objectId, qint64 trackVersion, U2OpStatus &os) {
    U2SingleModStep res;

    static const QString queryString = "SELECT id, object, otype, oextra, version, modType, details FROM SingleModStep WHERE object = :object AND version = :version LIMIT 1";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", objectId);
    q.bindInt64(":version", trackVersion);

    if (q.step()) {
        res.id = q.getInt64(0);
        res.objectId = q.getDataIdExt(1);
        res.version = q.getInt64(4);
        res.modType = q.getInt64(5);
        res.details = q.getBlob(6);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("An object single modification step not found"));
    }

    return res;
}

qint64 MysqlModDbi::getNearestUserModStepVersion(const U2DataId &masterObjId, qint64 version, U2OpStatus &os) {
    qint64 userStepVersion = version;

    static const QString qeuryString = "SELECT MAX(version) FROM UserModStep WHERE object = :object AND version <= :version";
    U2SqlQuery qVersion(qeuryString, db, os);
    qVersion.bindDataId(":object", masterObjId);
    qVersion.bindInt64(":version", version);

    if (qVersion.step()) {
        userStepVersion = qVersion.getInt64(0);
    }

    return userStepVersion;
}

QList< QList<U2SingleModStep> > MysqlModDbi::getModSteps(const U2DataId &masterObjId, qint64 version, U2OpStatus &os) {
    QList< QList<U2SingleModStep> > steps;
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 userStepId = -1;
    static const QString qUserStepIdString = "SELECT id FROM UserModStep WHERE object = :object AND version = :version LIMIT 1";
    U2SqlQuery qGetUserStepId(qUserStepIdString, db, os);
    qGetUserStepId.bindDataId(":object", masterObjId);
    qGetUserStepId.bindInt64(":version", version);

    if (qGetUserStepId.step()) {
        userStepId = qGetUserStepId.getInt64(0);
        qGetUserStepId.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Failed to find user step ID"));
        return steps;
    }

    static const QString qMultiStepIdString = "SELECT id FROM MultiModStep WHERE userStepId = :userStepId";
    static const QString qSingleStepString = "SELECT id, object, otype, oextra, version, modType, details FROM SingleModStep WHERE multiStepId = :multiStepId";

    U2SqlQuery qMultiStepId(qMultiStepIdString, db, os);
    qMultiStepId.bindInt64(":userStepId", userStepId);

    while (qMultiStepId.step()) {
        qint64 multiStepId = qMultiStepId.getInt64(0);

        U2SqlQuery qSingleStep(qSingleStepString, db, os);
        qSingleStep.bindInt64(":multiStepId", multiStepId);

        QList<U2SingleModStep> currentMultiStepSingleSteps;
        while (qSingleStep.step()) {
            CHECK_OP(os, steps);

            U2SingleModStep step;
            step.id = qSingleStep.getInt64(0);
            step.objectId = qSingleStep.getDataIdExt(1);
            step.version = qSingleStep.getInt64(4);
            step.modType = qSingleStep.getInt64(5);
            step.details = qSingleStep.getBlob(6);
            CHECK_OP(os, steps);

            currentMultiStepSingleSteps.append(step);
        }

        steps.append(currentMultiStepSingleSteps);
    }

    return steps;
}

void MysqlModDbi::createModStep(const U2DataId &masterObjId, U2SingleModStep &step, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    bool closeMultiStep = false;
    if (!isMultiStepStarted(masterObjId)) {
        startCommonMultiModStep(masterObjId, os);
        CHECK_OP(os, );
        SAFE_POINT(isMultiStepStarted(masterObjId), "A multiple modifications step must have been started", );
        closeMultiStep = true;
    }

    static const QString qSingleString = "INSERT INTO SingleModStep(object, otype, oextra, version, modType, details, multiStepId)"
            " VALUES(:object, :otype, :oextra, :version, :modType, :details, :multiStepId)";
    U2SqlQuery qSingle(qSingleString, db, os);
    qSingle.bindDataId(":object", step.objectId);
    qSingle.bindType(":otype", U2DbiUtils::toType(step.objectId));
    qSingle.bindBlob(":oextra", U2DbiUtils::toDbExtra(step.objectId));
    qSingle.bindInt64(":version", step.version);
    qSingle.bindInt64(":modType", step.modType);
    qSingle.bindBlob(":details", step.details);
    qSingle.bindInt64(":multiStepId", modStepsByObject[masterObjId].multiModStepId);
    step.id = qSingle.insert();
    CHECK_OP(os, );

    step.multiStepId = modStepsByObject[masterObjId].multiModStepId;

    if (closeMultiStep) {
        endCommonMultiModStep(masterObjId, os);
    }
}

void MysqlModDbi::removeModsWithGreaterVersion(const U2DataId &masterObjId, qint64 masterObjVersion, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get user step IDs
    QList<qint64> userStepIds;
    static const QString qSelectUserStepsString = "SELECT id FROM UserModStep WHERE object = :object AND version >= :version";
    U2SqlQuery qSelectUserSteps(qSelectUserStepsString, db, os);
    qSelectUserSteps.bindDataId(":object", masterObjId);
    qSelectUserSteps.bindInt64(":version", masterObjVersion);

    while (qSelectUserSteps.step()) {
        qint64 userStepId = qSelectUserSteps.getInt64(0);
        userStepIds.append(userStepId);
    }
    CHECK_OP(os, );

    // Remove all affected steps (user, multi, single)
    removeSteps(userStepIds, os);
}

void MysqlModDbi::removeDuplicateUserStep(const U2DataId &masterObjId, qint64 masterObjVersion, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get user step IDs
    QList<qint64> userStepIds;
    static const QString qSelectString = "SELECT id FROM UserModStep WHERE object = :object AND version = :version";
    U2SqlQuery qSelect(qSelectString, db, os);
    qSelect.bindDataId(":object", masterObjId);
    qSelect.bindInt64(":version", masterObjVersion);

    while (qSelect.step()) {
        qint64 id = qSelect.getInt64(0);
        userStepIds.append(id);
    }
    CHECK_OP(os, );

    if (userStepIds.count() < 2) {
        // There are no user steps for the object with the same version, do nothing
        return;
    }

    SAFE_POINT(2 == userStepIds.count(), "There must be two user steps in this case", );

    // Don't take into account user step with the greatest id
    userStepIds.removeLast();

    // Remove user step with lower ID
    removeSteps(userStepIds, os);
}

void MysqlModDbi::removeSteps(QList<qint64> userStepIds, U2OpStatus &os) {
    if (userStepIds.isEmpty()) {
        return;
    }

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get multiple steps IDs
    QList<qint64> multiStepIds;
    static const QString qSelectMultiStepsString = "SELECT id FROM MultiModStep WHERE userStepId = :userStepId";

    foreach (qint64 userStepId, userStepIds) {
        U2SqlQuery qSelectMultiSteps(qSelectMultiStepsString, db, os);
        qSelectMultiSteps.bindInt64(":userStepId", userStepId);

        while (qSelectMultiSteps.step()) {
            qint64 multiStepId = qSelectMultiSteps.getInt64(0);
            multiStepIds.append(multiStepId);
        }
    }
    CHECK_OP(os, );

    // Remove single steps
    static const QString qDeleteSingleStepsString = "DELETE FROM SingleModStep WHERE multiStepId = :multiStepId";
    U2SqlQuery qDeleteSingleSteps(qDeleteSingleStepsString, db, os);
    foreach (qint64 multiStepId, multiStepIds) {
        qDeleteSingleSteps.bindInt64(":multiStepId", multiStepId);
        qDeleteSingleSteps.execute();
    }
    CHECK_OP(os, );

    // Remove multiple steps
    static const QString qDeleteMultiStepsString = "DELETE FROM MultiModStep WHERE id = :id";
    U2SqlQuery qDeleteMultiSteps(qDeleteMultiStepsString, db, os);
    foreach (qint64 multiStepId, multiStepIds) {
        qDeleteMultiSteps.bindInt64(":id", multiStepId);
        qDeleteMultiSteps.execute();
    }
    CHECK_OP(os, );

    // Remove user steps
    static const QString qDeleteUserStepsString = "DELETE FROM UserModStep WHERE id = :id";
    U2SqlQuery qDeleteUserSteps(qDeleteUserStepsString, db, os);
    foreach (qint64 userStepId, userStepIds) {
        qDeleteUserSteps.bindInt64(":id", userStepId);
        qDeleteUserSteps.execute();
    }
}

void MysqlModDbi::removeObjectMods(const U2DataId &objectId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get user step IDs
    QList<qint64> userStepIds;
    static const QString qSelectUserStepsString = "SELECT id FROM UserModStep WHERE object = :object";
    U2SqlQuery qSelectUserSteps(qSelectUserStepsString, db, os);
    qSelectUserSteps.bindDataId(":object", objectId);

    while (qSelectUserSteps.step()) {
        qint64 userStepId = qSelectUserSteps.getInt64(0);
        userStepIds.append(userStepId);
    }
    CHECK_OP(os, );

    // Remove all affected steps (user, multi, single)
    removeSteps(userStepIds, os);
}

void MysqlModDbi::cleanUpAllStepsOnError() {
    U2OpStatus2Log os;
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery("DELETE FROM SingleModStep", db, os).execute();
    os.setError("");
    U2SqlQuery("DELETE FROM MultiModStep", db, os).execute();
    os.setError("");
    U2SqlQuery("DELETE FROM UserModStep", db, os).execute();
}

static void checkMainThread(U2OpStatus &os) {
    QThread *mainThread = QCoreApplication::instance()->thread();
    QThread *thisThread = QThread::currentThread();

    if (mainThread != thisThread) {
        os.setError(U2DbiL10n::tr("Not main thread"));
    }
}

void MysqlModDbi::startCommonUserModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    checkMainThread(os);
    CHECK_OP(os, );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Only one common step at a time
    CHECK_EXT(!isUserStepStarted(masterObjId), os.setError(U2DbiL10n::tr("Can't create a common user modifications step, previous one is not complete")),);

    if (!modStepsByObject.contains(masterObjId)) {
        modStepsByObject[masterObjId] = MysqlModStepsDescriptor();
    }

    // Create a new user modifications step in the database
    createUserModStep(masterObjId, os);
    CHECK_OP(os, );
}

void MysqlModDbi::endCommonUserModStep(const U2DataId &userMasterObjId, U2OpStatus &os) {
    checkMainThread(os);
    CHECK_OP(os, );
    SAFE_POINT(modStepsByObject.contains(userMasterObjId), QString("There are not modification steps for object with id %1").arg(userMasterObjId.toLong()),);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 userModStepId = modStepsByObject[userMasterObjId].userModStepId;
    qint64 multiModStepId = modStepsByObject[userMasterObjId].multiModStepId;

    modStepsByObject.remove(userMasterObjId);

    if (-1 == multiModStepId) {
        MysqlTransaction t(db, os);
        Q_UNUSED(t);

        // Get multiple steps IDs
        static const QString qSelectMultiStepsString = "SELECT id FROM MultiModStep WHERE userStepId = :userStepId LIMIT 1";
        U2SqlQuery qSelectMultiSteps(qSelectMultiStepsString, db, os);
        qSelectMultiSteps.bindInt64(":userStepId", userModStepId);

        // If user modification step doesn't contain any multi modification steps
        if (!qSelectMultiSteps.step()) {
            CHECK_OP(os, );

            static const QString qDeleteUserStepsString = "DELETE FROM UserModStep WHERE id = :id";
            U2SqlQuery qDeleteUserSteps(qDeleteUserStepsString, db, os);
            qDeleteUserSteps.bindInt64(":id", userModStepId);
            qDeleteUserSteps.execute();
        }
    }
}

void MysqlModDbi::startCommonMultiModStep(const U2DataId &userMasterObjId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    if(!modStepsByObject.contains(userMasterObjId)) {
        modStepsByObject[userMasterObjId] = MysqlModStepsDescriptor();
    }

    if (!isUserStepStarted(userMasterObjId)) {
        startCommonUserModStep(userMasterObjId, os);
        CHECK_OP(os, );
        SAFE_POINT(isUserStepStarted(userMasterObjId), "A user modifications step must have been started", );
        modStepsByObject[userMasterObjId].removeUserStepWithMulti = true;
    } else {
        modStepsByObject[userMasterObjId].removeUserStepWithMulti = false;
    }

    if (isMultiStepStarted(userMasterObjId)) {
        os.setError(U2DbiL10n::tr("Can't create a common multiple modifications step, previous one is not complete"));
        U2OpStatus2Log innerOs;
        endCommonUserModStep(userMasterObjId, innerOs);
        return;
    }

    // Create a new multiple modifications step in the database
    createMultiModStep(userMasterObjId, os);
}

void MysqlModDbi::endCommonMultiModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    if (modStepsByObject[masterObjId].removeUserStepWithMulti) {
        endCommonUserModStep(masterObjId, os);
    } else {
        modStepsByObject[masterObjId].multiModStepId = -1;
    }
}

void MysqlModDbi::createUserModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 masterObjVersion = dbi->getMysqlObjectDbi()->getObjectVersion(masterObjId, os);
    CHECK_OP(os, );

    static const QString qUserString = "INSERT INTO UserModStep(object, otype, oextra, version) VALUES(:object, :otype, :oextra, :version)";
    U2SqlQuery qUser(qUserString, db, os);
    qUser.bindDataId(":object", masterObjId);
    qUser.bindType(":otype", U2DbiUtils::toType(masterObjId));
    qUser.bindBlob(":oextra", U2DbiUtils::toDbExtra(masterObjId));
    qUser.bindInt64(":version", masterObjVersion);

    qint64 curUserModStepId = qUser.insert();
    CHECK_OP(os, );

    if (-1 == curUserModStepId) {
        os.setError(U2DbiL10n::tr("Failed to create a common user modifications step"));
        return;
    } else {
        modStepsByObject[masterObjId].userModStepId = curUserModStepId;
    }
}

void MysqlModDbi::createMultiModStep(const U2DataId &masterObjId, U2OpStatus &os) {
    SAFE_POINT(isUserStepStarted(masterObjId), "A user modifications step must have been started", );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString qMultiString = "INSERT INTO MultiModStep(userStepId) VALUES(:userStepId)";
    U2SqlQuery qMulti(qMultiString, db, os);
    qMulti.bindInt64(":userStepId", modStepsByObject[masterObjId].userModStepId);

    qint64 curMultiModStepId = qMulti.insert();
    CHECK_OP(os, );

    if (-1 == curMultiModStepId) {
        os.setError(U2DbiL10n::tr("Failed to create a common multiple modifications step"));
        return;
    } else {
        modStepsByObject[masterObjId].multiModStepId = curMultiModStepId;
    }
}

bool MysqlModDbi::isUserStepStarted(const U2DataId& userMasterObjId) {
    if (!modStepsByObject.contains(userMasterObjId)) {
        return false;
    }

    return modStepsByObject[userMasterObjId].userModStepId != -1;
}
bool MysqlModDbi::isMultiStepStarted(const U2DataId& userMasterObjId) {
    if (!modStepsByObject.contains(userMasterObjId)) {
        return false;
    }

    return modStepsByObject[userMasterObjId].multiModStepId != -1;
}


bool MysqlModDbi::canUndo(const U2DataId &objectId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get current object version
    qint64 objVersion = dbi->getMysqlObjectDbi()->getObjectVersion(objectId, os);
    CHECK_OP(os, false);

    // Verify if there are steps
    static const QString qString = "SELECT id FROM UserModStep WHERE object = :object AND version < :version LIMIT 1";
    U2SqlQuery q(qString, db, os);
    q.bindDataId(":object", objectId);
    q.bindInt64(":version", objVersion);

    return q.step();
}

bool MysqlModDbi::canRedo(const U2DataId &objectId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get current object version
    qint64 objVersion = dbi->getMysqlObjectDbi()->getObjectVersion(objectId, os);
    CHECK_OP(os, false);

    // Verify if there are steps
    static const QString qString = "SELECT id FROM UserModStep WHERE object = :object AND version >= :version LIMIT 1";
    U2SqlQuery q(qString, db, os);
    q.bindDataId(":object", objectId);
    q.bindInt64(":version", objVersion);

    return q.step();
}

}   // namespace U2
