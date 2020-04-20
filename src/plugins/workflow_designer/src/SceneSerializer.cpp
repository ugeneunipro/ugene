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

#include <QDomElement>
#include <QPointF>

#include <U2Core/Log.h>

#include <U2Core/QVariantUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "SceneSerializer.h"
#include "WorkflowViewController.h"
#include "WorkflowViewItems.h"

/* TRANSLATOR U2::LocalWorkflow::WorkflowView */

namespace U2 {
using namespace Workflow;
static const QString WORKFLOW_EL = "workflow";
static const QString PROCESS_EL = "process";
static const QString PORT_EL = "port";
static const QString PARAMS_EL = "params";
static const QString DATAFLOW_EL = "dataflow";
static const QString POSITION_ATTR = "pos";
static const QString ID_ATTR = "id";
static const QString NAME_ATTR = "name";
static const QString NUM_IN_SCHEMA = "numberInSchema";
static const QString TYPE_ATTR = "type";
static const QString SRC_PORT_ATTR = "sprt";
static const QString SRC_PROC_ATTR = "sprc";
static const QString DST_PORT_ATTR = "dprt";
static const QString DST_PROC_ATTR = "dprc";
static const QString SCRIPT_TEXT = "scriptText";

static void saveProcess(const WorkflowProcessItem* pit, QDomElement& proj) {
    QDomElement docElement = SchemaSerializer::saveActor(pit->getProcess(), proj);
    pit->saveState(docElement);
    foreach(WorkflowPortItem* iot, pit->getPortItems()) {
        Port* port = iot->getPort();
        QDomElement portElement = SchemaSerializer::savePort(port, docElement);
        portElement.setAttribute(POSITION_ATTR, iot->getOrientarion());
    }
}

static void saveFlow(const WorkflowBusItem* dit, QDomElement& proj) {
    QDomElement el = SchemaSerializer::saveLink(dit->getBus(), proj);
    dit->saveState(el);
}

void SceneSerializer::scene2xml(const WorkflowScene* scene, QDomDocument& xmlDoc){
    QDomElement projectElement = xmlDoc.createElement(WORKFLOW_EL);
    // TODO save scale and view rect??
    xmlDoc.appendChild(projectElement);
    saveItems(scene->items(), projectElement);
}

void SceneSerializer::saveItems(const QList<QGraphicsItem*>& items, QDomElement& proj) {
    foreach(QGraphicsItem* item, items) {
        switch (item->type()) {
        case WorkflowProcessItemType:
            saveProcess(qgraphicsitem_cast<WorkflowProcessItem*>(item), proj);
            break;
        case WorkflowBusItemType:
            saveFlow(static_cast<WorkflowBusItem*>(item), proj);
            break;
        }
    }
}

static void initProcMap(QMap<ActorId, WorkflowProcessItem*> & procMap, WorkflowScene* scene) {
    foreach(QGraphicsItem* item, scene->items()) {
        if (item->type() == WorkflowProcessItemType) {
            WorkflowProcessItem* wit = qgraphicsitem_cast<WorkflowProcessItem*>(item);
            procMap[wit->getProcess()->getId()] = wit;
        }
    }
}

QString SceneSerializer::xml2scene(const QDomElement& projectElement, WorkflowScene* scene,
                                   QMap<ActorId, ActorId>& /*remapping*/,
                                                 bool ignoreErrors, bool select) {
    QMap<ActorId, WorkflowProcessItem*> procMap;
    QMap<ActorId, Actor*> actorMap;
    initProcMap(procMap, scene);

    ActorPrototypeRegistry* registry = WorkflowEnv::getProtoRegistry();

    QDomNodeList procNodes = projectElement.elementsByTagName(PROCESS_EL);
    for(int i=0; i<procNodes.size(); i++) {
        QDomNode n = procNodes.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement procElement = n.toElement();

        const ActorId id = str2aid(procElement.attribute(ID_ATTR));
        if (!ignoreErrors && procMap.contains(id)) {
            return WorkflowView::tr("Invalid content: duplicate process %1").arg(id);
        }

        const QString name = SchemaSerializer::getElemType(procElement.attribute(TYPE_ATTR));
        ActorPrototype* proto = registry->getProto(name);
        if (!proto) {
            return WorkflowView::tr("Invalid content: unknown process type %1").arg(name);
        }

        Actor* proc = proto->createInstance(id, NULL);
        actorMap[id] = proc;
        proc->setLabel(procElement.attribute(NAME_ATTR));
        if (NULL != proto->getEditor()) {
            ActorConfigurationEditor *actorEd = dynamic_cast<ActorConfigurationEditor*>(proto->getEditor());
            if (NULL != actorEd) {
                ActorConfigurationEditor *editor = dynamic_cast<ActorConfigurationEditor*>(proto->getEditor()->clone());
                editor->setConfiguration(proc);
                proc->setEditor(editor);
            }
        }
        WorkflowProcessItem* it = new WorkflowProcessItem(proc);
        it->loadState(procElement);
        scene->addItem(it);
        if (select) {
            it->setSelected(true);
        }
        procMap[id] = it;
//        foreach(PortDescriptor * pd, proto->getPortDesciptors()) {
//            if(pd == NULL) {
//                continue;
//            }
//            WorkflowPortItem * p = it->getPort(pd->getId());
//        }
    }
    foreach(Actor* proc, actorMap) {
        ActorPrototype *proto = proc->getProto();
        if (NULL != proto->getEditor()) {
            ActorConfigurationEditor *actorEd = dynamic_cast<ActorConfigurationEditor*>(proto->getEditor());
            if (NULL != actorEd) {
                ActorConfigurationEditor *editor = dynamic_cast<ActorConfigurationEditor*>(proto->getEditor()->clone());
                editor->setConfiguration(proc);
                proc->setEditor(editor);
            }
        }
    }
    return QString();
}

}//namespace
