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


#ifndef _U2_MOD_DBI_H_
#define _U2_MOD_DBI_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Mod.h>


namespace U2 {

/** An interface to access tracks of modifications of dbi objects */
class U2ModDbi : public U2ChildDbi {
protected:
    U2ModDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi) {}

public:
    /** Returns a modification step with the specified version for the object */
    virtual U2SingleModStep getModStep(const U2DataId& objectId, qint64 trackVersion, U2OpStatus& os) = 0;

    /** Removes all modification tracks and steps for the object */
    virtual void removeObjectMods(const U2DataId& objectId, U2OpStatus& os) = 0;

    /**
     * Starts a common user modifications step (can be only one at a time).
     * Do not use this method, create a "U2UseCommonUserModStep" instance instead!
     * The method can be called only in the main thread.
     */
    virtual void startCommonUserModStep(const U2DataId& masterObjId, U2OpStatus& os) = 0;

    /**
     * Ends a common user modifications step (can be only one at a time).
     * Do not use this method, use "U2UseCommonUserModStep" instead!
     * The method can be called only in the main thread.
     */
    virtual void endCommonUserModStep(const U2DataId& masterObjId, U2OpStatus &os) = 0;
};

} //namespace

#endif
