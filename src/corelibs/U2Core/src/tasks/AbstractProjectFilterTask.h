/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/ProjectTreeControllerModeSettings.h>
#include <U2Core/Task.h>

namespace U2 {

typedef QList<QPointer<GObject>> SafeObjList;

//////////////////////////////////////////////////////////////////////////
/// AbstractProjectFilterTask
//////////////////////////////////////////////////////////////////////////

class U2CORE_EXPORT AbstractProjectFilterTask : public Task {
    Q_OBJECT
public:
    void run() override;

signals:
    void si_objectsFiltered(const QString& filterGroupName, const SafeObjList& objects);

protected:
    AbstractProjectFilterTask(const ProjectTreeControllerModeSettings& settings, const QString& filterGroupName, const QList<QPointer<Document>>& docs);

    virtual bool filterAcceptsObject(GObject* obj);

    const ProjectTreeControllerModeSettings settings;
    const QList<QPointer<Document>> docs;
    const QString filterGroupName;
    SafeObjList filteredObjs;

    int filteredObjCountPerIteration;
    int totalObjectCount;
    int processedObjectCount;

private:
    void filterDocument(const QPointer<Document>& doc);

    static void doStaticInitialization();
};

//////////////////////////////////////////////////////////////////////////
/// ProjectFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

class U2CORE_EXPORT ProjectFilterTaskFactory {
public:
    ProjectFilterTaskFactory(QList<GObjectType> _acceptedObjects = {GObjectTypes::UNKNOWN,
                                                                    GObjectTypes::UNLOADED,
                                                                    GObjectTypes::TEXT,
                                                                    GObjectTypes::SEQUENCE,
                                                                    GObjectTypes::ANNOTATION_TABLE,
                                                                    GObjectTypes::VARIANT_TRACK,
                                                                    GObjectTypes::CHROMATOGRAM,
                                                                    GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT,
                                                                    GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT,
                                                                    GObjectTypes::PHYLOGENETIC_TREE,
                                                                    GObjectTypes::BIOSTRUCTURE_3D,
                                                                    GObjectTypes::ASSEMBLY});
    virtual ~ProjectFilterTaskFactory();

    AbstractProjectFilterTask* registerNewTask(const ProjectTreeControllerModeSettings& settings, const QList<QPointer<Document>>& docs) const;

protected:
    virtual AbstractProjectFilterTask* createNewTask(const ProjectTreeControllerModeSettings& settings,
                                                     const QList<QPointer<Document>>& docs) const = 0;
    QList<QPointer<Document>> getAcceptedDocs(const QList<QPointer<Document>>& docs) const;

private:
    QList<GObjectType> acceptableObjectTypes;
};

}  // namespace U2
