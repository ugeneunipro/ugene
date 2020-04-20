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

#ifndef _U2_SELECTORVALUE_H_
#define _U2_SELECTORVALUE_H_

#include <U2Lang/ActorModel.h>

#include "PortMapping.h"

namespace U2 {

using namespace Workflow;

class U2LANG_EXPORT SelectorValue {
public:
    SelectorValue(const QString &value, const QString &replaceProtoId);

    void addPortMapping(const PortMapping &value);
    void setName(const QString &value);

    const QString & getValue() const;
    const QString & getProtoId() const;
    const QString & getName() const;
    const QList<PortMapping> & getMappings() const;

    void validate(Actor *actor, U2OpStatus &os) const;

private:
    QString value;
    QString replaceProtoId;
    QString name;
    QList<PortMapping> portList;

private:
    /** Returns found port or NULL */
    Port * validateSrcPort(const PortMapping &mapping, Actor *actor, U2OpStatus &os) const;
    /** Returns found port descriptor or NULL */
    PortDescriptor * validateDstPort(const PortMapping &mapping,
        const QList<PortDescriptor*> &descs, U2OpStatus &os) const;
    void validateDuplicates(const PortMapping &mapping,
        const QSet<QString> &srcIdSet, U2OpStatus &os) const;
    void validatePortsCount(const QList<Port*> &src,
        const QList<PortDescriptor*> &dst, U2OpStatus &os) const;
    void validateMappingsCount(const QList<Port*> &srcPorts, U2OpStatus &os) const;
};

} // U2

#endif // _U2_SELECTORVALUE_H_
