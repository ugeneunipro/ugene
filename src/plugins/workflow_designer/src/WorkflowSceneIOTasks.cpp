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

#include "WorkflowSceneIOTasks.h"

#include <QDomDocument>

#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/L10n.h>

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/Wizard.h>

#include "SceneSerializer.h"
#include "WorkflowDocument.h"
#include "WorkflowViewController.h"

namespace U2 {
using namespace Workflow;

/**********************************
 * LoadWorkflowSceneTask
 **********************************/
LoadWorkflowSceneTask::LoadWorkflowSceneTask(const QSharedPointer<Schema>& _schema, Metadata* _meta, WorkflowScene* _scene, const QString& _url, bool _noUrl, bool _disableWizardAutorun)
    : Task(tr("Load workflow scene"), TaskFlag_None),
      schema(_schema),
      meta(_meta),
      scene(_scene),
      url(_url),
      noUrl(_noUrl),
      disableWizardAutorun(_disableWizardAutorun) {
    GCOUNTER(cvar, "LoadWorkflowSceneTask");
    assert(schema != nullptr);
    assert(meta != nullptr);
    assert(scene != nullptr);
}

void LoadWorkflowSceneTask::run() {
    QFile file(url);
    if (!file.open(QIODevice::ReadOnly)) {
        setError(L10N::errorOpeningFileRead(url));
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    rawData = in.readAll();
    format = LoadWorkflowTask::detectFormat(rawData);
    if (format == LoadWorkflowTask::UNKNOWN) {
        setError(tr("Undefined format: plain text or xml expected"));
        return;
    }
}

Task::ReportResult LoadWorkflowSceneTask::report() {
    if (hasError()) {
        return ReportResult_Finished;
    }
    CHECK(scene != nullptr, ReportResult_Finished);

    QString err;
    if (!scene->items().isEmpty()) {
        resetSceneAndScheme();
    }
    if (format == LoadWorkflowTask::HR) {
        err = HRSchemaSerializer::string2Schema(rawData, schema.get(), meta);
    } else if (format == LoadWorkflowTask::XML) {
        QDomDocument xml;
        QMap<ActorId, ActorId> remapping;
        xml.setContent(rawData);
        err = SceneSerializer::xml2scene(xml.documentElement(), scene, remapping);
        SchemaSerializer::readMeta(meta, xml.documentElement());
        scene->setModified(false);
        meta->url = url;
    } else {
        // cause check for errors in the begin
        assert(false);
    }

    if (!err.isEmpty()) {
        setError(tr("Error while parsing file: %1").arg(err));
        resetSceneAndScheme();
        return ReportResult_Finished;
    }

    if (disableWizardAutorun && !schema->getWizards().isEmpty()) {
        schema->getWizards().first()->setAutoRun(false);
    }

    SceneCreator sc(schema.get(), *meta);
    sc.recreateScene(scene);
    scene->setModified(false);
    scene->connectConfigurationEditors();
    if (!noUrl) {
        meta->url = url;
    }
    return ReportResult_Finished;
}

void LoadWorkflowSceneTask::resetSceneAndScheme() {
    scene->sl_reset();
    schema->reset();
    meta->reset();
}

}  // namespace U2
