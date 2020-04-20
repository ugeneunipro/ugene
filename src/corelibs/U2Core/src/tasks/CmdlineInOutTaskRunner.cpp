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

#include <U2Core/DbiConnection.h>
#include <U2Core/GObject.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "CmdlineInOutTaskRunner.h"

namespace U2 {

const QString CmdlineInOutTaskRunner::IN_DB_ARG = "input-db";
const QString CmdlineInOutTaskRunner::IN_ID_ARG = "input-id";
const QString CmdlineInOutTaskRunner::OUT_DB_ARG = "output-db";

CmdlineInOutTaskConfig::CmdlineInOutTaskConfig()
: CmdlineTaskConfig(), emptyOutputPossible(false)
{
}

namespace {
    CmdlineTaskConfig prepareConfig(const CmdlineInOutTaskConfig &config) {
        CmdlineTaskConfig result = config;

        QStringList dbList;
        QStringList idList;
        foreach (GObject *object, config.inputObjects) {
            U2EntityRef entityRef = object->getEntityRef();
            dbList << CmdlineInOutTaskRunner::toString(entityRef.dbiRef);
            idList << QString::number(U2DbiUtils::toDbiId(entityRef.entityId));
        }

        QString argString = "--%1=\"%2\"";
        result.arguments << argString.arg(CmdlineInOutTaskRunner::IN_DB_ARG).arg(dbList.join(";"));
        result.arguments << argString.arg(CmdlineInOutTaskRunner::IN_ID_ARG).arg(idList.join(";"));
        result.arguments << argString.arg(CmdlineInOutTaskRunner::OUT_DB_ARG).arg(CmdlineInOutTaskRunner::toString(config.outDbiRef));
        return result;
    }
}

CmdlineInOutTaskRunner::CmdlineInOutTaskRunner(const CmdlineInOutTaskConfig &config)
: CmdlineTaskRunner(prepareConfig(config)), config(config)
{
}

Task::ReportResult CmdlineInOutTaskRunner::report() {
    ReportResult result = CmdlineTaskRunner::report();
    CHECK_OP(stateInfo, result);

    if (ReportResult_Finished == result) {
        if (outputObjects.isEmpty() && !config.emptyOutputPossible) {
            setError(tr("An error occurred during the task. See the log for details."));
        }
    }
    return result;
}

const QList<U2DataId> & CmdlineInOutTaskRunner::getOutputObjects() const {
    return outputObjects;
}

QString CmdlineInOutTaskRunner::toString(const U2DbiRef &dbiRef) {
    return dbiRef.dbiFactoryId + ">" + dbiRef.dbiId;
}

U2DbiRef CmdlineInOutTaskRunner::parseDbiRef(const QString &string, U2OpStatus &os) {
    QStringList dbTokens = string.split(">");
    if (1 == dbTokens.size()) {
        return U2DbiRef(DEFAULT_DBI_ID, string);
    }
    if (2 != dbTokens.size()) {
        os.setError(tr("Wrong database string: ") + string);
        return U2DbiRef();
    }
    return U2DbiRef(dbTokens[0], dbTokens[1]);
}

U2DataId CmdlineInOutTaskRunner::parseDataId(const QString &string, const U2DbiRef &dbiRef, U2OpStatus &os) {
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, U2DataId());
    return con.dbi->getObjectDbi()->getObject(string.toLongLong(), os);
}

namespace {
    const QString OUTPUT_OBJECT_TAG = "ugene-output-object-id=";
}

void CmdlineInOutTaskRunner::logOutputObject(const U2DataId &id) {
    coreLog.info(OUTPUT_OBJECT_TAG + QString::number(U2DbiUtils::toDbiId(id)));
}

bool CmdlineInOutTaskRunner::isCommandLogLine(const QString &logLine) const {
    return logLine.startsWith(OUTPUT_OBJECT_TAG);
}

bool CmdlineInOutTaskRunner::parseCommandLogWord(const QString &logWord) {
    if (logWord.startsWith(OUTPUT_OBJECT_TAG)) {
        QString idString = logWord.mid(OUTPUT_OBJECT_TAG.size());
        U2DataId objectId = parseDataId(idString, config.outDbiRef, stateInfo);
        CHECK_OP(stateInfo, true);
        outputObjects << objectId;
        return true;
    }
    return false;
}

} // U2
