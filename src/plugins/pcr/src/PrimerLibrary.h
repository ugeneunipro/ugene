/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QMutex>
#include <QSharedPointer>

#include <U2Algorithm/TmCalculator.h>

#include <U2Core/Primer.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/UdrSchema.h>
#include <U2Core/global.h>

namespace U2 {

class DbiConnection;
class UdrDbi;

class PrimerLibrary : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(PrimerLibrary)
public:
    ~PrimerLibrary();

    static PrimerLibrary* getInstance(U2OpStatus& os);
    static void release();

    void addPrimer(const Primer& primer, U2OpStatus& os);
    void updatePrimer(const Primer& primer, U2OpStatus& os);
    Primer getPrimer(const U2DataId& primerId, U2OpStatus& os) const;
    QList<Primer> getPrimers(U2OpStatus& os) const;
    void removePrimer(const Primer& primer, U2OpStatus& os);

    /** Warning: these methods can be time-consuming, beware of calling them in the main thread */
    void addRawPrimer(Primer primer, U2OpStatus& os);
    void updateRawPrimer(Primer primer, U2OpStatus& os);

    const QVariantMap& getTemperatureSettings() const;
    void setTemperatureCalculator(const QSharedPointer<TmCalculator>& newTemperatureCalculator);

signals:
    void si_primerAdded(const U2DataId& primerId);
    void si_primerChanged(const U2DataId& primerId);
    void si_primerRemoved(const U2DataId& primerId);

private:
    static void initPrimerUdrs(U2OpStatus& os);
    void setTmAndGcOfPrimer(Primer& primer);
    /**
     * PrimerSettings table was added in v 46
     * We need to create this table if it not exists
     */
    void createPrimerSettingsTableIfNotExists();
    /**
     * Init temperature calculator
     * Read the calculator ID and check registry
     * If the corresponding factory already in registry - take it and use to create calculator
     * Also switch @initializedFromDb to true, beacuse it's required only once
     * If it's not - create the default one
     * This is required because some factory may be still not initialized 
     * (if, for example, the initialization point in some other plugin, which is not loaded yet)
     */
    void initTemperatureCalculator();

    PrimerLibrary(DbiConnection* connection);

private:
    static QScopedPointer<PrimerLibrary> instance;
    static QMutex mutex;

    QSharedPointer<TmCalculator> temperatureCalculator;
    /**
     * If false - try to initialize @temperatureCalculator from the database
     * See @initTemperatureCalculator() for details
     */
    bool initializedFromDb = false;
    DbiConnection* connection = nullptr;
    UdrDbi* udrDbi = nullptr;
};

}  // namespace U2
