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

#ifndef _U2_SUPPORT_CLASS_H_
#define _U2_SUPPORT_CLASS_H_

#include <QMetaType>
#include <QString>

#include <U2Core/global.h>

namespace U2 {

class WorkflowNotification;
typedef QList<WorkflowNotification> NotificationsList;

#define ACTOR_ID_REF (Qt::UserRole)
#define PORT_REF (Qt::UserRole + 1)
#define TEXT_REF (Qt::UserRole + 3)
#define TYPE_REF (Qt::UserRole + 4)
#define ACTOR_NAME_REF (Qt::UserRole + 5)

class U2LANG_EXPORT WorkflowNotification {
public:
    WorkflowNotification(const QString &message = "", const QString &actorId = "", const QString &type = U2_ERROR);
    QString message;
    QString actorId;
    QString type;
    QString port;

    bool operator== (const WorkflowNotification &other) const;

    static const QString U2_ERROR;
    static const QString U2_WARNING;
    static const QString U2_INFO;
};

}   // namespace U2

Q_DECLARE_METATYPE(U2::WorkflowNotification)

#endif // _U2_SUPPORT_CLASS_H_
