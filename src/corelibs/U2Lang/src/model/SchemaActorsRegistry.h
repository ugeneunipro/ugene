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

#include <QMutex>

#include <U2Lang/Schema.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT SchemaActorsRegistry {
public:
    bool registerSchema(const QString& protoId, Schema* schema);
    Schema* unregisterSchema(const QString& protoId);
    Schema* getSchema(const QString& protoId);

private:
    QMap<QString, Schema*> schemas;
    QMutex mutex;
};

}  // namespace Workflow
}  // namespace U2
