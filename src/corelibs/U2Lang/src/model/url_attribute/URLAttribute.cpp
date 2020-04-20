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

#include <U2Core/U2SafePoints.h>

#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowUtils.h>

#include "URLAttribute.h"

namespace U2 {

URLAttribute::URLAttribute(const Descriptor &d, const DataTypePtr type, bool required)
: Attribute(d, type, required)
{
    sets << Dataset();
    defaultValue = qVariantFromValue< QList<Dataset> >(sets);
    value = qVariantFromValue< QList<Dataset> >(sets);
}

void URLAttribute::setAttributeValue(const QVariant &newVal) {
    if (newVal.canConvert< QList<Dataset> >()) {
        sets = newVal.value< QList<Dataset> >();
    } else {
        QStringList urls = newVal.toString().split(";", QString::SkipEmptyParts);
        Dataset dSet;
        foreach (const QString url, urls) {
            dSet.addUrl(URLContainerFactory::createUrlContainer(url));
        }
        sets.clear();
        sets << dSet;
    }
    value = qVariantFromValue< QList<Dataset> >(sets);
}

const QVariant & URLAttribute::getAttributePureValue() const {
    return value;
}

const QVariant & URLAttribute::getDefaultPureValue() const {
    return defaultValue;
}

bool URLAttribute::isEmpty() const {
    if (Attribute::isEmpty()) {
        return true;
    }

    foreach (const Dataset &set, sets) {
        if (!set.getUrls().isEmpty()) {
            return false;
        }
    }
    return true;
}

bool URLAttribute::isDefaultValue() const {
    return false;
}

URLAttribute *URLAttribute::clone() {
    return new URLAttribute(*this);
}

QList<Dataset> & URLAttribute::getDatasets() {
    return sets;
}

void URLAttribute::updateValue() {
    QList<Dataset> res;
    foreach (const Dataset &dSet, sets) {
        res << dSet;
    }
    value = qVariantFromValue< QList<Dataset> >(res);
}

URLAttribute::URLAttribute(const URLAttribute &other)
    : Attribute(other)
{
    copy(other);
}

URLAttribute &URLAttribute::operator=(const URLAttribute &other) {
    CHECK(this != &other, *this);
    Attribute::operator =(other);
    copy(other);
    return *this;
}

QStringList URLAttribute::emptyDatasetNames(bool &hasUrl) {
    QStringList emptySets;
    hasUrl = false;
    foreach (const Dataset &dSet, sets) {
        if (dSet.getUrls().isEmpty()) {
            emptySets << dSet.getName();
        } else {
            hasUrl = true;
        }
    }
    return emptySets;
}

void URLAttribute::copy(const URLAttribute &other) {
    sets = other.sets;
    compatibleObjectTypes = other.compatibleObjectTypes;
}

bool URLAttribute::validate(NotificationsList &notificationList) {
    if (!isRequiredAttribute() || canBeEmpty()) {
        return true;
    }
    if (sets.isEmpty()) {
        notificationList << WorkflowNotification(WorkflowUtils::tr("Required parameter has no datasets specified: %1").arg(getDisplayName()));
        return false;
    }
    bool hasUrl = false;
    QStringList emptySets = emptyDatasetNames(hasUrl);

    if (!hasUrl) {
        notificationList << WorkflowNotification(WorkflowUtils::tr("Required parameter has no input urls specified: %1").arg(getDisplayName()));
        return false;
    }
    if (!emptySets.isEmpty()) {
        foreach (const QString &name, emptySets) {
            notificationList << WorkflowNotification(WorkflowUtils::tr("Required parameter %1 has empty dataset: %2").arg(getDisplayName()).arg(name));
        }
        return false;
    }
    return true;
}

const QSet<GObjectType> & URLAttribute::getCompatibleObjectTypes() const {
    return compatibleObjectTypes;
}

void URLAttribute::setCompatibleObjectTypes(const QSet<GObjectType> &types) {
    compatibleObjectTypes = types;
}

} // U2
