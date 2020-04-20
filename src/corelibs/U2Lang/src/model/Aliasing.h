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

#ifndef _U2_WORKFLOW_ALIASING_H_
#define _U2_WORKFLOW_ALIASING_H_

#include <U2Core/global.h>

#include <U2Lang/Port.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT SlotAlias {
public:
    SlotAlias(const Port *sourcePort, const QString &slotId, const QString &alias);

    const Port *getSourcePort() const;
    QString getSourceSlotId() const;
    QString getAlias() const;

private:
    const Port *port;
    QString slotId;
    QString alias;
};

class U2LANG_EXPORT PortAlias {
public:
    PortAlias(const Port *sourcePort, const QString &alias, const QString &description);
    bool operator ==(const PortAlias &another);

    // these return true if adding is successful
    bool addSlot(const SlotAlias &slot);
    bool addSlot(const Port *sourcePort, const QString &slotId, const QString &alias);

    const Port *getSourcePort() const;
    QString getAlias() const;
    QString getDescription() const;
    const QList<SlotAlias> &getSlotAliases() const;

    void setNewSlotAliases(const QList<SlotAlias> &newSlotAliases);
    void setNewSourcePort(const Port *sourcePort);

    bool isInput() const;

private:
    const Port *port;
    QString alias;
    QString description;
    QList<SlotAlias> slotAliases;
};

} // Workflow
} // U2

#endif // _U2_WORKFLOW_ALIASING_H_
