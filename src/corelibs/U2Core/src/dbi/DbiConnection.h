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

#ifndef _U2_DBI_CONNECTION_H_
#define _U2_DBI_CONNECTION_H_

#include <U2Core/global.h>

namespace U2 {

class U2Dbi;
class U2DbiRef;
class U2OpStatus;

/**
    Helper class that allocates connection in constructor and automatically releases it in the destructor
    It uses app-global connection pool.

    Note: DbiConnection caches U2OpStatus and reuses it in destructor on DBI release. Ensure that
    U2OpStatus live range contains DbiHandle live range
*/
class U2CORE_EXPORT DbiConnection {
public:
    /** Opens connection to existing DBI */
    DbiConnection(const U2DbiRef &ref, U2OpStatus &os);

    /** Opens connection to existing DBI or create news DBI*/
    DbiConnection(const U2DbiRef &ref, bool create, U2OpStatus &os, const QHash<QString, QString> &properties = (QHash<QString, QString>()));

    DbiConnection(const DbiConnection &dbiConnection);

    /** Constructs not opened dbi connection */
    DbiConnection();

    ~DbiConnection();

    /** Opens connection to existing DBI */
    void open(const U2DbiRef &ref, U2OpStatus &os);

    /** Opens connection to existing DBI or create news DBI*/
    void open(const U2DbiRef &ref, bool create, U2OpStatus &os, const QHash<QString, QString> &properties = (QHash<QString, QString>()));

    void close(U2OpStatus &os);

    bool isOpen() const;

    DbiConnection & operator=(DbiConnection const &dbiConnection);

    U2Dbi*          dbi;

private:
    void copy(const DbiConnection &dbiConnection);
};

}   // namespace U2

#endif // _U2_DBI_CONNECTION_H_
