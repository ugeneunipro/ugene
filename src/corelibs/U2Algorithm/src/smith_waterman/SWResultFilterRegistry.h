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

#ifndef _U2_SW_RESULT_FILTER_REGISTRY_H_
#define _U2_SW_RESULT_FILTER_REGISTRY_H_

#include <QStringList>
#include <QMutex>

#include <U2Algorithm/SmithWatermanResultFilters.h>


namespace U2 {

class U2ALGORITHM_EXPORT SWResultFilterRegistry: public QObject {
    Q_OBJECT
public:
    SWResultFilterRegistry(QObject* pOwn = 0);
    ~SWResultFilterRegistry();

    QStringList getFiltersIds() const;

    SmithWatermanResultFilter* getFilter(const QString& id);

    bool isRegistered(const QString& id) const;

    bool registerFilter( SmithWatermanResultFilter* filter );

    const QString& getDefaultFilterId() {return defaultFilterId;}

private:
    QMutex mutex;
    QMap<QString, SmithWatermanResultFilter*> filters;
    QString defaultFilterId;
};

} // namespace

#endif
