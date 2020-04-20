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

#ifndef DBITEST_H
#define DBITEST_H

#include <U2Core/U2Dbi.h>
#include <unittest.h>

namespace U2 {

/*Helper to provide dbi for tests tests.
In case you need to open a connection within your test useConnectionPool must be true to use the connection pool
if you don't need to open connections within your test useConnectionPool must be false to use created dbi without the pool*/

class TestDbiProvider{
public:
    TestDbiProvider();
    ~TestDbiProvider();

    bool init(const QString& dbiFileName, bool useConnectionPool);
    void close();
    U2Dbi* getDbi();
private:
    bool initialized;
    bool useConnectionPool;
    QString dbUrl;
    U2Dbi* dbi;
};

template<> inline QString toString<U2DataId>(const U2DataId &a) { return "0x" + QString(a.toHex()); }
template<> inline QString toString<U2Region>(const U2Region &r) { return r.toString(); }

} // namespace U2

#endif // DBITEST_H
