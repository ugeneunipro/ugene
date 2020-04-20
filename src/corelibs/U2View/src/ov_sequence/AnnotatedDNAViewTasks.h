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

#ifndef _U2_ANNOTATED_DNA_VIEW_TASKS_H_
#define _U2_ANNOTATED_DNA_VIEW_TASKS_H_

#include <U2Gui/ObjectViewTasks.h>
#include <U2Core/GObjectReference.h>

namespace U2 {

class U2SequenceObject;
class AnnotatedDNAView;

class OpenAnnotatedDNAViewTask : public ObjectViewTask {
    Q_OBJECT
public:
    //opens a single view for all sequence object in the list of sequence objects related to the objects in the list
    OpenAnnotatedDNAViewTask(const QList<GObject*>& objects);

    virtual void open();

    static void updateTitle(AnnotatedDNAView* v);

private:
    void populateSeqObjectRefs( GObject *object, QList<Document *> &docsToLoad,
        QSet<GObject *> &refsAdded );

    QList<GObjectReference> sequenceObjectRefs;
};

class OpenSavedAnnotatedDNAViewTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenSavedAnnotatedDNAViewTask(const QString& viewName, const QVariantMap& stateData);
    virtual void open();

};


class UpdateAnnotatedDNAViewTask : public ObjectViewTask {
public:
    UpdateAnnotatedDNAViewTask(AnnotatedDNAView* v, const QString& stateName, const QVariantMap& stateData);

    virtual void update();
};


} // namespace

#endif
