/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/U2Type.h>

namespace U2 {

class PrimerLibrary;
class PrimerLibraryTable;

class PrimerLibraryTableController : public QObject {
    Q_OBJECT
public:
    PrimerLibraryTableController(QObject* parent, PrimerLibraryTable* table);

private slots:
    void sl_primerAdded(const U2DataId& primerId);
    void sl_primerChanged(const U2DataId& primerId);
    void sl_primerRemoved(const U2DataId& primerId);

private:
    PrimerLibraryTable* table;
    PrimerLibrary* library;
};

}  // namespace U2
