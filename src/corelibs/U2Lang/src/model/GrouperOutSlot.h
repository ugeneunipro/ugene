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

#ifndef _GROUPER_SLOT_ACTION_
#define _GROUPER_SLOT_ACTION_

#include <U2Core/global.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/Descriptor.h>

namespace U2 {

class U2LANG_EXPORT ActionTypes {
public:
    static const QString MERGE_SEQUENCE;
    static const QString SEQUENCE_TO_MSA;
    static const QString MERGE_MSA;
    static const QString MERGE_STRING;
    static const QString MERGE_ANNS;

    static bool isValidType(const QString &actionType);
    static DataTypePtr getDataTypeByAction(const QString &actionType);
};

class U2LANG_EXPORT ActionParameters {
public:
    enum ParameterType {
        INTEGER,
        BOOLEAN,
        STRING
    };

    static const QString GAP;
    static const QString UNIQUE;
    static const QString SEPARATOR;
    static const QString MSA_NAME;
    static const QString SEQ_NAME;
    static const QString SEQ_SLOT;

    static ParameterType getType(const QString &parameter);
    static bool isValidParameter(const QString &actionType, const QString &parameter);
};

class U2LANG_EXPORT GrouperSlotAction {
public:
    GrouperSlotAction(const QString &type);
    GrouperSlotAction(const GrouperSlotAction &other);

    QString getType() const;
    const QVariantMap &getParameters() const;
    bool hasParameter(const QString &parameterId) const;
    QVariant getParameterValue(const QString &parameterId) const;
    void setParameterValue(const QString &parameterId, const QVariant &value);

private:
    QString type;
    QVariantMap parameters;
};

class U2LANG_EXPORT GroupOperations {
public:
    static Descriptor BY_VALUE();
    static Descriptor BY_ID();
    static Descriptor BY_NAME();
};

class U2LANG_EXPORT GrouperOutSlot {
public:
    GrouperOutSlot(const QString &outSlotId, const QString &inSlotStr);
    GrouperOutSlot(const GrouperOutSlot &another);
    ~GrouperOutSlot();

    bool operator==(const GrouperOutSlot &other) const;

    GrouperSlotAction *getAction();
    GrouperSlotAction *getAction() const;
    void setAction(const GrouperSlotAction &action);

    QString getOutSlotId() const;
    void setOutSlotId(const QString &outSlotId);

    QString getInSlotStr() const;
    QString getBusMapInSlotId() const;
    void setInSlotStr(const QString &slotStr);
    void setBusMapInSlotStr(const QString &busMapSlotStr);

    static QString readable2busMap(const QString &readableSlotStr);
    static QString busMap2readable(const QString &busMapSlotStr);

private:
    QString outSlotId;
    QString inSlotStr;
    GrouperSlotAction *action;
};

} // U2

#endif //_GROUPER_SLOT_ACTION_
