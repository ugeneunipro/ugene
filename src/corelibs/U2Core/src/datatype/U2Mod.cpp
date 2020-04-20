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

#include "U2Mod.h"

#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ModDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const qint64 U2ModType::objUpdatedName        = 1;

const qint64 U2ModType::sequenceUpdatedData   = 1001;

const qint64 U2ModType::msaUpdatedAlphabet    = 3001;
const qint64 U2ModType::msaAddedRows          = 3002;
const qint64 U2ModType::msaAddedRow           = 3003;
const qint64 U2ModType::msaRemovedRows        = 3004;
const qint64 U2ModType::msaRemovedRow         = 3005;
const qint64 U2ModType::msaUpdatedRowInfo     = 3006;
const qint64 U2ModType::msaUpdatedGapModel    = 3007;
const qint64 U2ModType::msaSetNewRowsOrder    = 3008;
const qint64 U2ModType::msaLengthChanged      = 3009;

const qint64 U2ModType::udrUpdated            = 4001;


U2UseCommonUserModStep::U2UseCommonUserModStep(const U2EntityRef &entity, U2OpStatus &os)
: dbi(NULL), valid(false), con(NULL), masterObjId(entity.entityId)
{
    // Open connection
    con.reset(new DbiConnection(entity.dbiRef, os));
    CHECK_OP(os, );
    CHECK_EXT(NULL != con->dbi, os.setError("NULL root dbi"), );
    dbi = con->dbi;
    init(os);
}

U2UseCommonUserModStep::U2UseCommonUserModStep(U2Dbi* _dbi, const U2DataId& _masterObjId, U2OpStatus& os)
: dbi(_dbi), valid(false), con(NULL), masterObjId(_masterObjId)
{
    init(os);
}

void U2UseCommonUserModStep::init(U2OpStatus &os) {
    // No mutexes are needed because start/end of
    // user mod steps are made only in main thread
    CHECK_EXT(NULL != dbi, os.setError("NULL dbi!"), );

    dbi->getModDbi()->startCommonUserModStep(masterObjId, os);
    if (!os.hasError()) {
        valid = true;
    }
}

U2UseCommonUserModStep::~U2UseCommonUserModStep() {
    if (valid) {
        U2OpStatus2Log os;
        dbi->getModDbi()->endCommonUserModStep(masterObjId, os);
    }
}

U2Dbi * U2UseCommonUserModStep::getDbi() const {
    return dbi;
}

ModificationAction::ModificationAction(U2AbstractDbi* _dbi, const U2DataId& _masterObjId)
    : dbi(_dbi),
      masterObjId(_masterObjId),
      trackMod(NoTrack)
{
    objIds.insert(masterObjId);
}

ModificationAction::~ModificationAction() {

}

} // namespace
