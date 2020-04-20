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

#include <U2Lang/BaseAttributes.h>
#include <U2Lang/Dataset.h>

#include <limits.h>

#include "DatasetsCountValidator.h"

namespace U2 {
namespace Workflow {

const QString DatasetsCountValidator::ID("datasets-count");
static const QString MIN("min");
static const QString MAX("max");
static const QString ATTR("attribute");

namespace {
    int minimum(const QMap<QString, QString> &options) {
        if (options.contains(MIN)) {
            bool ok = true;
            int val = options[MIN].toInt(&ok);
            if (ok && val >= 0) {
                return val;
            }
        }
        return 0;
    }
    int maximum(const QMap<QString, QString> &options) {
        if (options.contains(MAX)) {
            bool ok = true;
            int val = options[MAX].toInt(&ok);
            if (ok && val >= 0) {
                return val;
            }
        }
        return INT_MAX;
    }
    QString attributeId(const QMap<QString, QString> &options) {
        if (options.contains(ATTR)) {
            return options[ATTR];
        }
        return BaseAttributes::URL_IN_ATTRIBUTE().getId();
    }
}

bool DatasetsCountValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const {
    int min = minimum(options);
    int max = maximum(options);
    QString attrId = attributeId(options);

    QList<Dataset> sets = getValue< QList<Dataset> >(actor, attrId);
    bool result = true;
    if (sets.size() < min) {
        notificationList << WorkflowNotification(QObject::tr("The minimum datasets count is %1. The current count is %2").arg(min).arg(sets.size()));
        result = false;
    }
    if (sets.size() > max) {
        notificationList << WorkflowNotification(QObject::tr("The maximum datasets count is %1. The current count is %2").arg(max).arg(sets.size()));
        result = false;
    }
    return result;
}

} // Workflow
} // U2
