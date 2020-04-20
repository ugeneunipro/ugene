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

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/SupportClass.h>

#include "WriteSequenceValidator.h"

namespace U2 {
namespace Workflow {

WriteSequenceValidator::WriteSequenceValidator(const QString &attr, const QString &port, const QString &slot)
: ScreenedParamValidator(attr, port, slot)
{

}

bool WriteSequenceValidator::validate(const Configuration *cfg, NotificationsList &notificationList) const {
    const Actor *actor = dynamic_cast<const Actor*>(cfg);
    SAFE_POINT(NULL != actor, "NULL actor", false);
    if (!isAnnotationsBinded(actor)) {
        return true;
    }

    DocumentFormat *format = getFormatSafe(actor);
    CHECK(NULL != format, true);
    if (!isAnnotationsSupported(format)) {
        QString warning = QObject::tr("The format %1 does not support annotations").arg(format->getFormatId().toUpper());
        notificationList << WorkflowNotification(warning, "", WorkflowNotification::U2_WARNING);
        cmdLog.trace(warning);
    }

    return true;
}

DocumentFormat * WriteSequenceValidator::getFormatSafe(const Actor *actor) {
    Attribute *attr = actor->getParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId());
    SAFE_POINT(NULL != attr, "NULL format attribute", NULL);
    CHECK(actor->isAttributeVisible(attr), NULL);
    QString formatId = attr->getAttributePureValue().toString();
    return AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
}

bool WriteSequenceValidator::isAnnotationsBinded(const Actor *actor) const {
    Port *p = actor->getPort(port);
    SAFE_POINT(NULL != p, "NULL port", false);
    Attribute *attr = p->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID);
    SAFE_POINT(NULL != attr, "NULL busmap attribute", false);
    StrStrMap busMap = attr->getAttributeValueWithoutScript<StrStrMap>();
    QString bindData = busMap.value(BaseSlots::ANNOTATION_TABLE_SLOT().getId(), "");
    return !bindData.isEmpty();
}

bool WriteSequenceValidator::isAnnotationsSupported(const DocumentFormat *format) {
    return format->getSupportedObjectTypes().contains(GObjectTypes::ANNOTATION_TABLE);
}

bool WriteSequencePortValidator::validate(const IntegralBusPort *port, NotificationsList &notificationList) const {
    bool result = true;
    Actor *actor = port->owner();

    QStringList screenedSlots(BaseSlots::URL_SLOT().getId());

    if (!isBinded(port, BaseSlots::ANNOTATION_TABLE_SLOT().getId())) {
        DocumentFormat *format = WriteSequenceValidator::getFormatSafe(actor);
        CHECK(NULL != format, result);
        if (!WriteSequenceValidator::isAnnotationsSupported(format)) {
            screenedSlots << BaseSlots::ANNOTATION_TABLE_SLOT().getId();
        }
    }
    result &= ScreenedSlotValidator::validate(screenedSlots, port, notificationList);
    return result;
}

} // Workflow
} // U2
