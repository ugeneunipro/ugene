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

#ifndef _U2_PRIMERLIBRARY_H_
#define _U2_PRIMERLIBRARY_H_

#include <QMutex>

#include <U2Core/U2OpStatus.h>
#include <U2Core/UdrSchema.h>

#include "Primer.h"

namespace U2 {

class DbiConnection;
class UdrDbi;

class PrimerLibrary : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(PrimerLibrary)
public:
    ~PrimerLibrary();

    static PrimerLibrary * getInstance(U2OpStatus &os);
    static void release();

    void addPrimer(const Primer &primer, U2OpStatus &os);
    void updatePrimer(const Primer &primer, U2OpStatus &os);
    Primer getPrimer(const U2DataId &primerId, U2OpStatus &os) const;
    QList<Primer> getPrimers(U2OpStatus &os) const;
    void removePrimer(const Primer &primer, U2OpStatus &os);

    /** Warning: these methods can be time-consuming, beware of calling them in the main thread */
    void addRawPrimer(Primer primer, U2OpStatus &os);
    void updateRawPrimer(Primer primer, U2OpStatus &os);

signals:
    void si_primerAdded(const U2DataId &primerId);
    void si_primerChanged(const U2DataId &primerId);
    void si_primerRemoved(const U2DataId &primerId);

private:
    static void initPrimerUdr(U2OpStatus &os);
    static void setTmAndGcOfPrimer(Primer& primer);

    PrimerLibrary(DbiConnection *connection);

private:
    static QScopedPointer<PrimerLibrary> instance;
    static QMutex mutex;

    DbiConnection *connection;
    UdrDbi *udrDbi;
};

} // U2

#endif // _U2_PRIMERLIBRARY_H_
