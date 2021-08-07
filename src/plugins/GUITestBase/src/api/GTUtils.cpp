/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GTUtils.h"

#include <QDateTime>

#include <U2Core/AppContext.h>
#include <U2Core/ServiceModel.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtils"
static qint64 counter = QDateTime::currentMSecsSinceEpoch();

QString GTUtils::genUniqueString(const QString &prefix) {
    counter++;
    return prefix + "_" + QString ::number(counter);
}

#define GT_METHOD_NAME "checkServiceIsEnabled"
void GTUtils::checkServiceIsEnabled(HI::GUITestOpStatus &os, const QString &serviceName) {
    for (int time = 0; time < GT_OP_WAIT_MILLIS; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        QList<Service *> services = AppContext::getServiceRegistry()->getServices();
        foreach (Service *service, services) {
            if (service->getName() == serviceName && service->isEnabled()) {
                return;
            }
        }
    }
    GT_CHECK(false, "Service was not enabled within required period: " + serviceName);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
