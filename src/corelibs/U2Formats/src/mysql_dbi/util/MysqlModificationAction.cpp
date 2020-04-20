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

#include <U2Core/U2SafePoints.h>

#include "MysqlHelpers.h"
#include "MysqlModificationAction.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/MysqlModDbi.h"
#include "mysql_dbi/MysqlObjectDbi.h"

namespace U2 {

MysqlModificationAction::MysqlModificationAction(MysqlDbi* _dbi, const U2DataId& _masterObjId)
    : ModificationAction(_dbi, _masterObjId)
{

}

U2TrackModType MysqlModificationAction::prepare(U2OpStatus& os) {
    CHECK_OP(os, NoTrack);
    MysqlTransaction t(getDbi()->getDbRef(), os);
    Q_UNUSED(t);

    trackMod = dbi->getObjectDbi()->getTrackModType(masterObjId, os);
    if (os.hasError()) {
        trackMod = NoTrack;
        FAIL("Failed to get trackMod", NoTrack);
    }

    if (TrackOnUpdate == trackMod) {
        qint64 masterObjVersionToTrack = dbi->getObjectDbi()->getObjectVersion(masterObjId, os);
        CHECK_OP(os, trackMod);

        // If a user mod step has already been created for this action
        // then it can not be deleted. The version must be incremented.
        // Obsolete duplicate step must be deleted
        if (getDbi()->getMysqlModDbi()->isUserStepStarted(masterObjId)) {
            getDbi()->getMysqlModDbi()->removeDuplicateUserStep(masterObjId, masterObjVersionToTrack, os);

            // Increment the object version
            masterObjVersionToTrack++;
        }

        // A user pressed "Undo" (maybe several times), did another action => there is no more "Redo" history
        getDbi()->getMysqlModDbi()->removeModsWithGreaterVersion(masterObjId, masterObjVersionToTrack, os);
        if (os.hasError()) {
            getDbi()->getMysqlModDbi()->cleanUpAllStepsOnError();
            return trackMod;
        }
    }

    return trackMod;
}

void MysqlModificationAction::addModification(const U2DataId& objId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    CHECK_OP(os, );

    objIds.insert(objId);

    if (TrackOnUpdate == trackMod) {
        SAFE_POINT(!modDetails.isEmpty(), "Empty modification details", );

        qint64 objVersion = dbi->getObjectDbi()->getObjectVersion(objId, os);
        CHECK_OP(os, );

        if ((objId == masterObjId) && (getDbi()->getMysqlModDbi()->isUserStepStarted(masterObjId))) {
            objVersion++;
        }

        U2SingleModStep singleModStep;
        singleModStep.objectId = objId;
        singleModStep.version = objVersion;
        singleModStep.modType = modType;
        singleModStep.details = modDetails;

        singleSteps.append(singleModStep);
    }
}

void MysqlModificationAction::complete(U2OpStatus& os) {
    // TODO: rewrite it with another U2UseCommonMultiModStep
    CHECK_OP(os, );
    MysqlTransaction t(getDbi()->getDbRef(), os);
    Q_UNUSED(t);

    // Save modification tracks, if required
    if (TrackOnUpdate == trackMod) {
        if (0 == singleSteps.size()) {
            // do nothing
        }
        else if (1 == singleSteps.size()) {
            getDbi()->getMysqlModDbi()->createModStep(masterObjId, singleSteps.first(), os);
            CHECK_OP(os, );
        }
        else {
            MysqlUseCommonMultiModStep multi(getDbi(), masterObjId, os);
            CHECK_OP(os, );
            Q_UNUSED(multi);

            foreach (U2SingleModStep singleStep, singleSteps) {
                getDbi()->getMysqlModDbi()->createModStep(masterObjId, singleStep, os);
                CHECK_OP(os, );
            }
        }
    }

    // Increment versions of all objects
    foreach (const U2DataId& objId, objIds) {
        MysqlObjectDbi::incrementVersion(objId, getDbi()->getDbRef(), os);
        CHECK_OP(os, );
    }
}

MysqlDbi* MysqlModificationAction::getDbi() const {
    return static_cast<MysqlDbi*>(dbi);

}

}   // namespace U2
