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

#ifndef _U2_FILTERED_PROJECT_GROUP_H_
#define _U2_FILTERED_PROJECT_GROUP_H_

#include <QObject>

namespace U2 {

class FilteredProjectGroup;
class GObject;

//////////////////////////////////////////////////////////////////////////
/// WrappedObject
//////////////////////////////////////////////////////////////////////////

class WrappedObject : public QObject {
    Q_OBJECT
public:
    WrappedObject(GObject *obj, FilteredProjectGroup *parentGroup);

    GObject * getObject() const;
    FilteredProjectGroup * getParentGroup() const;

    static bool objectLessThan(const WrappedObject *first, const WrappedObject *second);

private:
    GObject *obj;
    FilteredProjectGroup *parentGroup;
};

//////////////////////////////////////////////////////////////////////////
/// FilteredProjectGroup
//////////////////////////////////////////////////////////////////////////

class FilteredProjectGroup : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(FilteredProjectGroup)
public:
    explicit FilteredProjectGroup(const QString &name);
    ~FilteredProjectGroup();

    const QString & getGroupName() const;

    void addObject(GObject *obj, int objNumber);
    void removeAt(int objNumber);

    bool contains(GObject *obj) const;
    WrappedObject * getWrappedObject(GObject *obj) const;
    WrappedObject * getWrappedObject(int position) const;
    int getWrappedObjectNumber(WrappedObject *obj) const;
    int getObjectsCount() const;
    int getNewObjectNumber(GObject *obj) const;

    static bool groupLessThan(FilteredProjectGroup *first, FilteredProjectGroup *second);

private:
    const QString name;
    QList<WrappedObject *> filteredObjs;
};

} // namespace U2

#endif // _U2_FILTERED_PROJECT_GROUP_H_
