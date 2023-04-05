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

#include "DbiDataHandler.h"

#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Lang/DbiDataStorage.h>

namespace U2 {
namespace Workflow {

DbiDataHandler::DbiDataHandler(const U2EntityRef& entityRef, U2ObjectDbi* dbi, bool useGC)
    : entityRef(entityRef), dbi(dbi), useGC(useGC) {
}

DbiDataHandler::~DbiDataHandler() {
    if (useGC) {
        U2OpStatusImpl os;
        // TODO: removing is forbidden because of performance problems
        // dbi->removeObject(id, os);
        // TODO: how do you want to check @os???
    }
}

DbiDataHandler::DbiDataHandler(const DbiDataHandler& /*other*/)
    : QSharedData() {
}

DbiDataHandler& DbiDataHandler::operator=(const DbiDataHandler&) {
    return *this;
}

bool DbiDataHandler::equals(const DbiDataHandler* other) const {
    if (other == nullptr) {
        return false;
    }

    return (other->entityRef == entityRef) && (other->dbi == dbi);
}

int DbiDataHandler::getReferenceCount() const {
    return this->ref.load();
}

U2DbiRef DbiDataHandler::getDbiRef() const {
    return entityRef.dbiRef;
}

const U2EntityRef& DbiDataHandler::getEntityRef() const {
    return entityRef;
}

bool DbiDataHandler::isValid() const {
    return entityRef.isValid();
}

}  // namespace Workflow
}  // namespace U2
