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

#ifndef _U2_ACTORVALIDATOR_H_
#define _U2_ACTORVALIDATOR_H_

#include <U2Lang/Actor.h>
#include <U2Lang/ConfigurationValidator.h>

#include <QMutex>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT ActorValidator : public ConfigurationValidator {
public:
    virtual ~ActorValidator() {}

    virtual bool validate(const Configuration *cfg, NotificationsList &notificationList) const;
    virtual bool validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const = 0;

protected:
    template<class T>
    T getValue(const Actor *actor, const QString &attrId) const;
};

class U2LANG_EXPORT ActorValidatorRegistry {
public:
    ActorValidatorRegistry();
    ~ActorValidatorRegistry();
    bool addValidator(const QString &id, ActorValidator *validator);
    ActorValidator * findValidator(const QString &id);

private:
    QMutex mutex;
    QMap<QString, ActorValidator*> validators;
};

template<class T>
T ActorValidator::getValue(const Actor *actor, const QString &attrId) const {
    Attribute *attr = actor->getParameter(attrId);
    if (NULL == attr) {
        return T();
    }
    return attr->getAttributePureValue().value<T>();
}

} // Workflow
} // U2

#endif // _U2_ACTORVALIDATOR_H_
