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

#ifndef _U2_PROJECT_VIEW_H_
#define _U2_PROJECT_VIEW_H_

#include <U2Core/PluginModel.h>
#include <U2Core/ServiceTypes.h>
#include <U2Core/SelectionModel.h>
#include <U2Core/DocumentModel.h>

class QMenu;
class QAction;

namespace U2 {

class DocumentSelection;
class GObjectSelection;
class GObject;
class Annotation;
class GUrl;

class U2GUI_EXPORT ProjectView : public Service {
    Q_OBJECT
public:
    ProjectView(const QString& sname, const QString& sdesc) 
        : Service(Service_ProjectView, sname, sdesc, QList<ServiceType>()<<Service_Project)
    {
    }

    virtual const DocumentSelection* getDocumentSelection() const = 0;

    virtual const GObjectSelection* getGObjectSelection() const = 0;

    virtual void highlightItem(Document*) = 0;

signals:
    void si_onDocTreePopupMenuRequested(QMenu& m);
};

}//namespace
#endif
