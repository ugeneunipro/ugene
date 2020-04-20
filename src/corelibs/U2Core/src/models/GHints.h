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

#ifndef _U2_GHINTS_H_
#define _U2_GHINTS_H_

#include <U2Core/global.h>

#include <QVariantMap>

namespace U2 {

class U2CORE_EXPORT GHints {
public:
    virtual ~GHints(){};

    virtual QVariantMap getMap() const  = 0;

    virtual void setMap(const QVariantMap& map) = 0;

    virtual QVariant get(const QString& key) const = 0;

    virtual QVariant get(const QString& key, const QVariant &defaultValue) const = 0;

    virtual void set(const QString& key, const QVariant& val) = 0;

    virtual void setAll(const QVariantMap& map);

    virtual int remove(const QString& key) = 0;

    static void dump(const QVariantMap& map);
};

class U2CORE_EXPORT GHintsDefaultImpl : public GHints {
public:
    GHintsDefaultImpl(const QVariantMap& _map = QVariantMap()) : map(_map) {}
    virtual QVariantMap getMap() const {return map;}

    virtual void setMap(const QVariantMap& _map) {map = _map;}

    virtual QVariant get(const QString& key) const  {return map.value(key);}

    virtual QVariant get(const QString& key, const QVariant &defaultValue) const  {return map.value(key, defaultValue);}

    virtual void set(const QString& key, const QVariant& val) {map[key] = val;}

    virtual int remove(const QString& key) {return map.remove(key);}

protected:
    QVariantMap map;
};


class StateLockableTreeItem;

class U2CORE_EXPORT ModTrackHints : public GHintsDefaultImpl {
public:
    ModTrackHints(StateLockableTreeItem* _p, const QVariantMap& _map, bool _topParentMode)
        : GHintsDefaultImpl(_map), p(_p), topParentMode(_topParentMode){}

    virtual void setMap(const QVariantMap& _map);

    virtual void set(const QString& key, const QVariant& val);

    virtual int remove(const QString& key);

private:
    void                    setModified();
    StateLockableTreeItem*  p;
    bool                    topParentMode;
};

}//namespace

#endif
