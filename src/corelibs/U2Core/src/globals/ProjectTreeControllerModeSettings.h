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

#ifndef _U2_PROJECT_TREE_CONTROLLER_MODE_SETTINGS_H_
#define _U2_PROJECT_TREE_CONTROLLER_MODE_SETTINGS_H_

#include <QStringList>

#include <QFont>

#include <U2Core/GObject.h>

namespace U2 {

class LoadDocumentTaskProvider;

enum ProjectTreeGroupMode {
    ProjectTreeGroupMode_Flat,         //objects are not grouped, only unloaded documents are shown
    ProjectTreeGroupMode_ByDocument,   //objects are grouped by document

    ProjectTreeGroupMode_Min = ProjectTreeGroupMode_Flat,
    ProjectTreeGroupMode_Max = ProjectTreeGroupMode_ByDocument
};

//filtered objects and documents are not shown in project tree
class PTCObjectFilter : public QObject {
public:
    PTCObjectFilter(QObject *p) : QObject(p) {}
    virtual bool filter(GObject *o) const = 0;
};

class PTCDocumentFilter : public QObject {
public:
    PTCDocumentFilter(QObject *p): QObject(p) {}
    virtual bool filter(Document *d) const = 0;
};

class U2CORE_EXPORT PTCObjectRelationFilter : public PTCObjectFilter {
public:
    PTCObjectRelationFilter(const GObjectRelation &rel, QObject *p = NULL) : PTCObjectFilter(p), rel(rel) {}
    bool filter(GObject *o) const;
    GObjectRelation rel;
};


class U2CORE_EXPORT ProjectTreeControllerModeSettings {
public:
    ProjectTreeControllerModeSettings() :
        allowMultipleSelection(true),
        readOnlyFilter(TriState_Unknown),
        loadTaskProvider(NULL),
        groupMode(ProjectTreeGroupMode_ByDocument),
        allowSelectUnloaded(false),
        ignoreRemoteObjects(false),
        objectFilter(NULL),
        documentFilter(NULL),
        markActive(false)
    {

    }

    QSet<GObjectType>          objectTypesToShow;  // show only objects of specified type
    QSet<GObjectConstraints*>  objectConstraints;  // show only objects that fits constraints
    QList<QPointer<GObject> >   excludeObjectList;  // do not show these objects
    QList<QPointer<Document> >  excludeDocList;     // do not show these documents
    QStringList                 tokensToShow;       // show documents/objects with all of tokens in a name
    bool                        allowMultipleSelection; //use multiple selection in tree
    TriState                    readOnlyFilter;     // unknown->all, true->filter(exclude) readonly, false -> keep only readonly
    LoadDocumentTaskProvider*   loadTaskProvider;   // use custom LoadDocumentTask factory instead of default
    ProjectTreeGroupMode        groupMode;          // group mode for objects
    bool                        allowSelectUnloaded; // ability to select unloaded objects
    bool                        ignoreRemoteObjects; // do not load from remote database

    //Note that objectFilter and documentFilter are called only on object add/remove ops!
    //WARN: object and document filters live-range is controlled by the side created these objects
    PTCObjectFilter*            objectFilter;
    PTCDocumentFilter*          documentFilter;

    bool                        markActive;
    QFont                       activeFont;

    bool isDocumentShown(Document* doc) const;
    bool isTypeShown(GObjectType t) const;
    bool isObjectShown(GObject* o) const;
    bool isObjectFilterActive() const;
    bool nameFilterAcceptsString(const QString &str) const;
};

} // U2

#endif // _U2_PROJECT_TREE_CONTROLLER_MODE_SETTINGS_H_
