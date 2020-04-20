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

#ifndef _U2_APP_GLOBAL_OBJECT_H_
#define _U2_APP_GLOBAL_OBJECT_H_

#include <QObject>

#include <U2Core/Identifiable.h>

namespace U2 {

// Represents some Application global resource identified by ID.
// Only 1 resource of the given ID can exists per time

// Note: This entity class is separated into a separate class to
// make it QObject and add more features to it in the future,
// for example like deallocation features.

class U2CORE_EXPORT AppGlobalObject : public QObject, public Identifiable<QString> {
    Q_OBJECT

public:
    AppGlobalObject(const QString& id) : Identifiable<QString>(id) {}
};

} //namespace

#endif
