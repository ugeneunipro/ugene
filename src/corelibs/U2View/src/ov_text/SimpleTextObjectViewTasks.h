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

#ifndef _U2_SIMPLE_TEXT_OBJECT_VIEW_TASKS_H_
#define _U2_SIMPLE_TEXT_OBJECT_VIEW_TASKS_H_

#include <U2Gui/ObjectViewTasks.h>

namespace U2 {

class OpenSimpleTextObjectViewTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenSimpleTextObjectViewTask(const QList<GObject *> &objects);
    virtual void open();

private:
    QList<GObject *> objects;
};

class OpenSavedTextObjectViewTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenSavedTextObjectViewTask(const QString& viewName, const QVariantMap& stateData);
    virtual void open();

private:
    QPointer<Document> doc;
};

class UpdateSimpleTextObjectViewTask : public ObjectViewTask {
public:
    UpdateSimpleTextObjectViewTask(GObjectView* v, const QString& stateName, const QVariantMap& stateData);

    virtual void update();
};


} // namespace

#endif

