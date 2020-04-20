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

#ifndef _U2_IDENTITY_H_
#define _U2_IDENTITY_H_

#include <QHash>
#include <QString>

namespace U2 {

template <class T> class AbstractId {
protected:
    AbstractId(const T& _id) : id(_id) {}
    virtual ~AbstractId() {}

public:
    virtual bool isValid() const  = 0;

    bool operator==(const AbstractId<T>& oid) const {
        return oid.id == id;
    }

    bool operator!=(const AbstractId<T>& oid) const {
        return !(*this == oid);
    }

    bool operator<(const AbstractId<T>& oid) const {
        return id < oid.id;
    }

    T id;
};

class AbstractStringId  : public AbstractId<QString> {
protected:
    AbstractStringId(const QString& id = QString()) : AbstractId<QString>(id){}

public:
    virtual bool isValid() const {return !id.isEmpty();}
};

class AbstractInt32Id : public AbstractId<int> {
protected:
    AbstractInt32Id(int id  = 0) : AbstractId<int>(id){}
public:
    virtual bool isValid() const {return id > 0;}
};


template <class T> uint qHash(const AbstractId<T>& key) {
    return ::qHash(key.id);
}

#define INT32_ID(Class) \
class Class : public AbstractInt32Id { \
public: \
    Class() : AbstractInt32Id() {} \
    Class(int id) : AbstractInt32Id(id) {} \
};

#define STRING_ID(Class) \
class Class : public QString { \
public: \
    Class() : QString() {} \
    Class(const QString& id) : QString(id) {} \
    Class(const char* id) : QString(QString(id)) {} \
};


}//namespace

#endif
