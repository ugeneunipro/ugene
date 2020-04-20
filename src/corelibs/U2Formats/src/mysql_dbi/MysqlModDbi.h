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

#ifndef _U2_MYSQL_MOD_DBI_H_
#define _U2_MYSQL_MOD_DBI_H_

#include "MysqlDbi.h"

#include <U2Core/U2AbstractDbi.h>

namespace U2 {

class U2FORMATS_EXPORT MysqlUseCommonMultiModStep {
public:
    /**
     * Master object ID refers to the user modifications step.
     * WARNING!: If a user step has been already created, the master object must be the same!
     */
    MysqlUseCommonMultiModStep(MysqlDbi *_mysqlDbi, const U2DataId &_masterObjId, U2OpStatus &os);
    ~MysqlUseCommonMultiModStep();

private:
    MysqlDbi *mysqlDbi;
    bool valid;
    const U2DataId &masterObjId;
};

class MysqlModStepsDescriptor {
public:
    MysqlModStepsDescriptor();

    qint64 userModStepId;
    qint64 multiModStepId;
    bool removeUserStepWithMulti;
};

class U2FORMATS_EXPORT MysqlModDbi : public U2ModDbi, public MysqlChildDbiCommon {
public:
    MysqlModDbi(MysqlDbi *dbi);

    /** Creates all required tables */
    virtual void initSqlSchema(U2OpStatus &os);

    /** Returns a modification step with the specified version for the object */
    virtual U2SingleModStep getModStep(const U2DataId &objectId, qint64 trackVersion, U2OpStatus &os);

    /** Returns single steps of the master object ID of the specified version, divided bu multi steps */
    QList< QList<U2SingleModStep> > getModSteps(const U2DataId &masterObjId, qint64 version, U2OpStatus &os);

    /** Gets version of the user step equal or less than the specified version (the maximum of them) */
    qint64 getNearestUserModStepVersion(const U2DataId &masterObjId, qint64 version, U2OpStatus &os);

    /**
     * Adds a modification step into the database.
     * The step must contain valid object ID and info about modifications,
     * the method sets correct ID and multiStepId for the step.
     */
    void createModStep(const U2DataId &masterObjId, U2SingleModStep &step, U2OpStatus &os);

    /**
     * Removes user steps for the object with version EQUAL or GREATER than the specified version.
     * Removes all multi and single steps of the user steps.
     */
    void removeModsWithGreaterVersion(const U2DataId &masterObjId, qint64 masterObjVersion, U2OpStatus &os);

    /**
     * When there are two user steps that differs by ID only, removes user step with lower ID.
     * This is required for the case "action > undo > action > undo", when the action uses "U2UseCommonUserModStep".
     */
    void removeDuplicateUserStep(const U2DataId &masterObjId, qint64 masterObjVersion, U2OpStatus& os);

    /** Removes all modification tracks and steps for the object */
    virtual void removeObjectMods(const U2DataId &objectId, U2OpStatus &os);

    /**
     * Starts a common user modifications step.
     * Do not use this method, create a "U2UseCommonUserModStep" instance instead!
     */
    virtual void startCommonUserModStep(const U2DataId &masterObjId, U2OpStatus &os);

    /**
     * Ends a common user modifications step.
     * Deletes the user modifications step if it doesn't contain any multi modification steps.
     * Do not use this method, use "U2UseCommonUserModStep" instead!
     */
    virtual void endCommonUserModStep(const U2DataId& masterObjId, U2OpStatus &os);

    /**
     * Starts a common multiple modifications step.
     * If there is a common user modifications step, verifies that the passed object ID is the same.
     * Creates a common user modifications step, if it doesn't exist.
     * Do not use this method, create a "U2UseCommonMultiModStep" instance instead!
     */
    void startCommonMultiModStep(const U2DataId& userMasterObjId, U2OpStatus& os);

    /**
     * Ends a common multiple modifications step.
     * Do not use this method, use "U2UseCommonMultiModStep" instead!
     */
    void endCommonMultiModStep(const U2DataId& userMasterObjId, U2OpStatus &os);

    /** Specifies whether a step has been started */
    static bool isUserStepStarted(const U2DataId& userMasterObjId);
    static bool isMultiStepStarted(const U2DataId& userMasterObjId);

    /** Specifies whether there are user steps that can be undone/redone */
    bool canUndo(const U2DataId &objectId, U2OpStatus &os);
    bool canRedo(const U2DataId &objectId, U2OpStatus &os);

    /**
     * Should never be called except when an unexpected error has occurred
     * Removes ALL single, multi and user steps from the database.
     * WARNING: No check is done the steps were deleted properly
     */
    void cleanUpAllStepsOnError();

private:
    /**
     * Create a record in the UserModStep table.
     * Sets "currentUserModStepId" to the added record ID.
     */
    void createUserModStep(const U2DataId &masterObjId, U2OpStatus &os);

    /**
     * Creates a record in the MultiModStep table.
     * Sets "currentMultiModStepId" to the added record ID.
     * Warning: it is assumed that a user modification step has been started!
     */
    void createMultiModStep(const U2DataId& userMasterObjId, U2OpStatus &os);

    /** Removes user steps with the specified IDs and all affected multiple and single steps */
    void removeSteps(QList<qint64> userStepIds, U2OpStatus &os);

    static QMap<U2DataId, MysqlModStepsDescriptor> modStepsByObject;
};

}   // namespace U2

#endif  // _U2_MYSQL_MOD_DBI_H_
