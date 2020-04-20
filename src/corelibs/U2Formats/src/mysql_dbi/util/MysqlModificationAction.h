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

#ifndef _U2_MYSQL_MODIFICATION_ACTION_H_
#define _U2_MYSQL_MODIFICATION_ACTION_H_

#include <U2Core/U2Mod.h>

#include <QSet>

namespace U2 {

class MysqlDbi;

/** Helper class to track info about an object */
class MysqlModificationAction : public ModificationAction {
public:
    MysqlModificationAction(MysqlDbi* dbi, const U2DataId& masterObjId);

    /**
        Verifies if modification tracking is enabled for the object.
        If it is, gets the object version.
        If there are tracking steps with greater or equal version (e.g. left from "undo"), removes these records.
        Returns the type of modifications  tracking for the object.
     */
    U2TrackModType prepare(U2OpStatus& os);

    /**
        Adds the object ID to the object IDs set.
        If tracking is enabled, adds a new single step to the list.
     */
    void addModification(const U2DataId& objId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os);

    /**
        If tracking is enabled, creates modification steps in the database.
        Increments version of all objects in the set.
     */
    void complete(U2OpStatus& os);

private:
    MysqlDbi* getDbi() const;
};

}   // namespace U2

#endif // _U2_MYSQL_MODIFICATION_ACTION_H_
