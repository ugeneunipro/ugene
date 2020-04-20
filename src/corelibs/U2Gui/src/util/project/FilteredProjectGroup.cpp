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

#include <U2Core/Folder.h>
#include <U2Core/L10n.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "ProjectFilterNames.h"

#include "FilteredProjectGroup.h"

namespace U2 {

namespace {

const QList<GObject *> emptyObjList;

}

//////////////////////////////////////////////////////////////////////////
/// WrappedObject
//////////////////////////////////////////////////////////////////////////

WrappedObject::WrappedObject(GObject *obj, FilteredProjectGroup *parentGroup)
    : obj(obj), parentGroup(parentGroup)
{
    SAFE_POINT(NULL != obj, L10N::nullPointerError("object"), );
    SAFE_POINT(NULL != parentGroup, L10N::nullPointerError("project filter group"), );
}

GObject * WrappedObject::getObject() const {
    return obj;
}

FilteredProjectGroup * WrappedObject::getParentGroup() const {
    return parentGroup;
}

bool WrappedObject::objectLessThan(const WrappedObject *first, const WrappedObject *second) {
    return GObject::objectLessThan(first->getObject(), second->getObject());
}

//////////////////////////////////////////////////////////////////////////
/// FilteredProjectGroup
//////////////////////////////////////////////////////////////////////////

FilteredProjectGroup::FilteredProjectGroup(const QString &name)
    : name(!name.isEmpty() ? name : tr("Unnamed group"))
{
    SAFE_POINT(!name.isEmpty(), "Project filter group has empty name", );
}

FilteredProjectGroup::~FilteredProjectGroup() {
    qDeleteAll(filteredObjs);
}

const QString & FilteredProjectGroup::getGroupName() const {
    return name;
}

void FilteredProjectGroup::addObject(GObject *obj, int objNumber) {
    SAFE_POINT(NULL != obj, L10N::nullPointerError("object"), );
    SAFE_POINT(0 <= objNumber && objNumber <= filteredObjs.size(), "Object index is out of range", );

    filteredObjs.insert(objNumber, new WrappedObject(obj, this));
}

void FilteredProjectGroup::removeAt(int objNumber) {
    SAFE_POINT(0 <= objNumber && objNumber < filteredObjs.size(), "Object index is out of range", );
    delete filteredObjs.takeAt(objNumber);
}

bool FilteredProjectGroup::contains(GObject *obj) const {
    SAFE_POINT(NULL != obj, L10N::nullPointerError("object"), false);
    
    foreach (WrappedObject *wrappedObj, filteredObjs) {
        if (wrappedObj->getObject() == obj) {
            return true;
        }
    }
    return false;
}

int FilteredProjectGroup::getObjectsCount() const {
    return filteredObjs.size();
}

int FilteredProjectGroup::getNewObjectNumber(GObject *obj) const {
    SAFE_POINT(NULL != obj, L10N::nullPointerError("object"), -1);

    WrappedObject testObject(obj, const_cast<FilteredProjectGroup *>(this));
    const QList<WrappedObject *>::const_iterator begin = filteredObjs.constBegin();
    const QList<WrappedObject *>::const_iterator insertPos = std::upper_bound(begin, filteredObjs.constEnd(),
        &testObject, WrappedObject::objectLessThan);
    return insertPos - begin;
}

WrappedObject * FilteredProjectGroup::getWrappedObject(GObject *obj) const {
    SAFE_POINT(NULL != obj, L10N::nullPointerError("object"), NULL);

    foreach (WrappedObject *wrappedObj, filteredObjs) {
        if (wrappedObj->getObject() == obj) {
            return wrappedObj;
        }
    }
    return NULL;
}

WrappedObject * FilteredProjectGroup::getWrappedObject(int position) const {
    SAFE_POINT(0 <= position && position < filteredObjs.size(), "Object index is out of range", NULL);
    return filteredObjs[position];
}

int FilteredProjectGroup::getWrappedObjectNumber(WrappedObject *obj) const {
    return filteredObjs.indexOf(obj);
}

bool FilteredProjectGroup::groupLessThan(FilteredProjectGroup *first, FilteredProjectGroup *second) {
    const QString firstGroupName = first->getGroupName();
    const QString secondGroupName = second->getGroupName();

    if (firstGroupName == ProjectFilterNames::OBJ_NAME_FILTER_NAME) {
        return true;
    } else if (secondGroupName == ProjectFilterNames::OBJ_NAME_FILTER_NAME) {
        return false;
    } else {
        return QString::compare(first->getGroupName(), second->getGroupName(), Qt::CaseInsensitive) < 0;
    }
}

} // namespace U2
