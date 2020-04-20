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

#include "PairedReadsPortValidator.h"

namespace U2 {
namespace Workflow {

PairedReadsPortValidator::PairedReadsPortValidator(const QString& inputId, const QString& inputPairedId)
    : PortValidator(), inputSlotId(inputId), pairedInputSlotId(inputPairedId) {}

bool PairedReadsPortValidator::validate(const IntegralBusPort *port, NotificationsList &notificationList) const {
    bool res = true;

    StrStrMap bm = port->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();

    const bool paired = bm.contains(pairedInputSlotId);

    if (!isBinded(bm, inputSlotId)) {
        res = false;
        notificationList.append(WorkflowNotification(tr("The mandatory \"%1\" slot is not connected.").arg(port->getSlotNameById(inputSlotId)), port->getId()));
    }

    if (paired) {
        if (!isBinded(bm, pairedInputSlotId)) {
            res = false;
            notificationList.append(WorkflowNotification(tr("The mandatory \"%1\" slot is not connected.").arg(port->getSlotNameById(pairedInputSlotId)), port->getId()));
        }
    }

    return res;
}

}   // namesapce Workflow
}   // namespace U2
