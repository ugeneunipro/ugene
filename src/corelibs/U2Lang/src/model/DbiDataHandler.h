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

#ifndef _WORKFLOW_DBI_DATA_HANDLER_H_
#define _WORKFLOW_DBI_DATA_HANDLER_H_

#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2Type.h>

namespace U2 {
namespace Workflow {

class DbiDataStorage;
class DbiDataHandler;

typedef QSharedDataPointer<DbiDataHandler> SharedDbiDataHandler;

class U2LANG_EXPORT DbiDataHandler : public QSharedData {
    friend class DbiDataStorage;
public:
    DbiDataHandler(const U2EntityRef &entRef, U2ObjectDbi *dbi, bool useGC);
    virtual ~DbiDataHandler();

    int getReferenceCount() const;
    U2DbiRef getDbiRef() const;

    bool equals(const DbiDataHandler *other) const;

    bool isValid() const;

private:
    const U2EntityRef entRef;
    U2ObjectDbi *dbi;
    bool useGC;

    DbiDataHandler(const DbiDataHandler &);
    DbiDataHandler &operator=(const DbiDataHandler &);
};

} // Workflow
} // U2

Q_DECLARE_METATYPE(U2::Workflow::SharedDbiDataHandler);
Q_DECLARE_METATYPE(QList<U2::Workflow::SharedDbiDataHandler>);

#endif
