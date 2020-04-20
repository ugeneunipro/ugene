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

#ifndef _U2_MCA_DBI_UTILS_H_
#define _U2_MCA_DBI_UTILS_H_

namespace U2 {

class MultipleChromatogramAlignment;
class U2EntityRef;
class U2McaRow;
class U2OpStatus;

class U2CORE_EXPORT McaDbiUtils : public QObject {
public:
    static void updateMca(U2OpStatus &os, const U2EntityRef &mcaRef, const MultipleChromatogramAlignment &mca);
    static void addRow(U2OpStatus &os, const U2EntityRef &mcaRef, qint64 posInMca, U2McaRow &row);
    static void addRows(U2OpStatus &os, const U2EntityRef &mcaRef, QList<U2McaRow> &rows);
    static QList<U2McaRow> getMcaRows(U2OpStatus &os, const U2EntityRef &mcaRef);
    static U2McaRow getMcaRow(U2OpStatus &os, const U2EntityRef &mcaRef, qint64 rowId);

    static void removeRow(const U2EntityRef& mcaRef, qint64 rowId, U2OpStatus& os);
    static void removeCharacters(const U2EntityRef& mcaRef, const QList<qint64>& rowIds, qint64 pos, qint64 count, U2OpStatus& os);

    static void replaceCharacterInRow(const U2EntityRef& mcaRef, qint64 rowId, qint64 pos, char newChar, U2OpStatus& os);
    static void removeRegion(const U2EntityRef& entityRef, const qint64 rowId, qint64 pos, qint64 count, U2OpStatus& os);
};

}   // namespace U2

#endif // _U2_MCA_DBI_UTILS_H_
