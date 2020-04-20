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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "PrimerLibrary.h"
#include "PrimerLibraryTable.h"
#include "PrimerLibraryTableController.h"

namespace U2 {

PrimerLibraryTableController::PrimerLibraryTableController(QObject *parent, PrimerLibraryTable *table) :
    QObject(parent),
    table(table)
{
    SAFE_POINT(NULL != table, "Primer library table is NULL", );

    U2OpStatus2Log os;
    library = PrimerLibrary::getInstance(os);
    SAFE_POINT_OP(os, );

    connect(library, SIGNAL(si_primerAdded(const U2DataId &)), SLOT(sl_primerAdded(const U2DataId &)));
    connect(library, SIGNAL(si_primerChanged(const U2DataId &)), SLOT(sl_primerChanged(const U2DataId &)));
    connect(library, SIGNAL(si_primerRemoved(const U2DataId &)), SLOT(sl_primerRemoved(const U2DataId &)));
}

void PrimerLibraryTableController::sl_primerAdded(const U2DataId &primerId) {
    U2OpStatus2Log os;
    Primer primer = library->getPrimer(primerId, os);
    CHECK_OP(os, );
    table->addPrimer(primer);
}

void PrimerLibraryTableController::sl_primerChanged(const U2DataId &primerId) {
    U2OpStatus2Log os;
    Primer primer = library->getPrimer(primerId, os);
    CHECK_OP(os, );
    table->updatePrimer(primer);
}

void PrimerLibraryTableController::sl_primerRemoved(const U2DataId &primerId) {
    U2OpStatus2Log os;
    table->removePrimer(primerId, os);
}

}   // namespace U2
