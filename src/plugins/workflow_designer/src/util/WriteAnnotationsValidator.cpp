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

#include "WriteAnnotationsValidator.h"

#include <U2Lang/BasePorts.h>
#include <U2Lang/BaseSlots.h>

namespace U2 {
namespace Workflow {

bool WriteAnnotationsValidator::validate(const Actor* actor, NotificationsList& notificationList, const QMap<QString, QString>& options) const {
    Q_UNUSED(options);

    Port* port = actor->getPort(BasePorts::IN_ANNOTATIONS_PORT_ID());
    if (port == nullptr) {
        notificationList << WorkflowNotification(tr("Input port is NULL"), actor->getId(), WorkflowNotification::U2_ERROR);
        FAIL("Input port is NULL", false);
    }

    auto input = qobject_cast<IntegralBusPort*>(port);
    if (input == nullptr) {
        notificationList << WorkflowNotification(tr("IntegralBusPort is NULL"), actor->getId(), WorkflowNotification::U2_ERROR);
        FAIL("IntegralBusPort is NULL", false);
    }

    Attribute* attribute = actor->getParameter("write_names");
    if (attribute == nullptr) {
        notificationList << WorkflowNotification(tr("Attribute is NULL"), actor->getId(), WorkflowNotification::U2_ERROR);
        FAIL("Attribute is NULL", false);
    }
    const bool writeSequence = attribute->getAttributeValueWithoutScript<bool>();

    const QList<Actor*> inputSeq = input->getProducers(BaseSlots::DNA_SEQUENCE_SLOT().getId());
    if (inputSeq.isEmpty() && writeSequence) {
        notificationList << WorkflowNotification(QObject::tr("Sequence names were not saved, the input slot 'Sequence' is empty."), actor->getId(), WorkflowNotification::U2_WARNING);
    }

    return true;
}

}  // namespace Workflow
}  // namespace U2
