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

#include <U2Core/Log.h>
#include <U2Core/QVariantUtils.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowEnv.h>

#include "SchemaSerializer.h"

//Q_DECLARE_METATYPE(U2::Workflow::CfgMap)

namespace U2 {
namespace Workflow {

const QMap<QString, QString> SchemaSerializer::ELEM_TYPES_MAP = SchemaSerializer::initElemTypesMap();

const QString SchemaSerializer::WORKFLOW_DOC = "GB2WORKFLOW";

static const QString WORKFLOW_EL = "workflow";
static const QString DOMAIN_EL = "workflow";
static const QString PROCESS_EL = "process";
static const QString ITERATION_EL = "iteration";
static const QString PORT_EL = "port";
static const QString PARAMS_EL = "params";
static const QString DATAFLOW_EL = "dataflow";
static const QString ID_ATTR = "id";
static const QString NAME_ATTR = "name";
static const QString TYPE_ATTR = "type";
static const QString SRC_PORT_ATTR = "sprt";
static const QString SRC_PROC_ATTR = "sprc";
static const QString DST_PORT_ATTR = "dprt";
static const QString DST_PROC_ATTR = "dprc";
static const QString ALIASES_EL = "paramAliases";
static const QString SCRIPT_TEXT = "scriptText";

static void saveConfiguration(const Configuration& cfg, QDomElement& owner) {
    QVariantMap qm;
    foreach (Attribute* a, cfg.getParameters()) {
        qm[a->getId()] = a->toVariant();
    }
    QDomElement el = owner.ownerDocument().createElement(PARAMS_EL);
    owner.appendChild(el);
    el.appendChild(owner.ownerDocument().createTextNode(QVariantUtils::map2String(qm)));
}

static void saveParamAliases(const QMap<QString, QString> & aliases, QDomElement & owner ) {
    QDomElement el = owner.ownerDocument().createElement( ALIASES_EL );

    QMap<QString, QString>::const_iterator it = aliases.constBegin();
    while( it != aliases.constEnd() ) {
        el.setAttribute( it.key(), it.value() );
        ++it;
    }
    owner.appendChild( el );
}

QMap<QString, QString> SchemaSerializer::initElemTypesMap() {
    QMap<QString, QString> map;
    map["read.malignment"] = "read-msa";
    map["read.remote"] = "fetch-sequence";
    map["read.sequence"] = "read-sequence";
    map["read.text"] = "read-text";
    map["write.malignment"] = "write-msa";
    map["write.clustalw"] = "write-clustalw";
    map["write.fasta"] = "write-fasta";
    map["write.fastq"] = "write-fastq";
    map["write.gbk"] = "write-genbank";
    map["write.text"] = "write-text";
    map["write.sequence"] = "write-sequence";
    map["write.stockholm"] = "write-stockholm";
    map["annotator.collocation"] = "collocated-annotation-search";
    map["sequence.extract"] = "extract-annotated-sequence";
    map["repeat.finder"] = "repeats-search";
    map["find.pattern"] = "search";
    map["import.phred.quality"] = "import-phred-qualities";
    map["blastall"] = "blast";
    map["blastPlus"] = "blast-plus";
    map["orf.marker"] = "orf-search";
    map["remote.query"] = "blast-ncbi";
    map["find.smithwaterman"] = "ssearch";
    map["bowtie.assembly"] = "bowtie";
    map["bowtie.indexer"] = "bowtie-build-index";
    map["bowtie.index.reader"] = "bowtie-read-index";
    map["uhmmer.build"] = "hmm2-build";
    map["uhmmer.search"] = "hmm2-search";
    map["uhmmer.read"] = "hmm2-read-profile";
    map["uhmmer.write"] = "hmm2-write-profile";
    map["clustalw.align"] = "clustalw";
    map["kalign.align"] = "kalign";
    map["mafft.align"] = "mafft";
    map["muscle.align"] = "muscle";
    map["tcoffee.align"] = "tcoffee";
    map["pfmatrix.build"] = "fmatrix-build";
    map["sitecon.build"] = "sitecon-build";
    map["pwmatrix.build"] = "wmatrix-build";
    map["pfmatrix.convert"] = "fmatrix-to-wmatrix";
    map["pfmatrix.read"] = "fmatrix-read";
    map["sitecon.read"] = "sitecon-read";
    map["pwmatrix.read"] = "wmatrix-read";
    map["sitecon.search"] = "sitecon-search";
    map["pwmatrix.search"] = "wmatrix-search";
    map["pfmatrix.write"] = "fmatrix-write";
    map["sitecon.write"] = "sitecon-write";
    map["pwmatrix.write"] = "wmatrix-write";
    return map;
}

QString SchemaSerializer::getElemType(const QString & t) {
    if(ELEM_TYPES_MAP.contains(t)) {
        return ELEM_TYPES_MAP.value(t);
    }
    return t;
}

QDomElement SchemaSerializer::saveActor(const Actor* proc, QDomElement& proj) {
    QDomElement docElement = proj.ownerDocument().createElement(PROCESS_EL);

    docElement.setAttribute(ID_ATTR, proc->getId());
    docElement.setAttribute(TYPE_ATTR, proc->getProto()->getId());
    docElement.setAttribute(NAME_ATTR, proc->getLabel());
    docElement.setAttribute(SCRIPT_TEXT, proc->getScript() == 0? "" : proc->getScript()->getScriptText());

    saveConfiguration(*proc, docElement);
    saveParamAliases( proc->getParamAliases(), docElement );

    proj.appendChild(docElement);
    return docElement;
}

//static Actor* readActor(const QDomElement& procElement) {
//    const QString name = procElement.attribute(TYPE_ATTR);
//    ActorPrototype* proto = WorkflowEnv::getProtoRegistry()->getProto(name);
//    if (!proto) {
//        return NULL;
//    }

//    Actor* proc = proto->createInstance();
//    if (proc) {
//        SchemaSerializer::readConfiguration(proc, procElement);
//        proc->setLabel(procElement.attribute(NAME_ATTR));
//        SchemaSerializer::readParamAliases( proc->getParamAliases(), procElement );
//    }
//    return proc;
//}

QDomElement SchemaSerializer::saveLink(const Link* link, QDomElement& proj) {
    QDomElement docElement = proj.ownerDocument().createElement(DATAFLOW_EL);
    docElement.setAttribute(SRC_PORT_ATTR, link->source()->getId());
    docElement.setAttribute(SRC_PROC_ATTR, link->source()->owner()->getId());
    docElement.setAttribute(DST_PORT_ATTR, link->destination()->getId());
    docElement.setAttribute(DST_PROC_ATTR, link->destination()->owner()->getId());
    proj.appendChild(docElement);
    return docElement;
}

QDomElement SchemaSerializer::savePort(const Port* port, QDomElement& owner) {
    QDomElement el = owner.ownerDocument().createElement(PORT_EL);
    el.setAttribute(ID_ATTR, port->getId());
    saveConfiguration(*port, el);
    owner.appendChild(el);
    return el;
}


void SchemaSerializer::schema2xml(const Schema& schema, QDomDocument& xml) {
    QDomElement projectElement = xml.createElement(WORKFLOW_EL);
    xml.appendChild(projectElement);
    foreach(Actor* a, schema.getProcesses()) {
        QDomElement el = saveActor(a, projectElement);
        foreach(Port* p, a->getPorts()) {
            savePort(p, el);
        }
    }
    foreach(Link* l, schema.getFlows()) {
        saveLink(l, projectElement);
    }
    QDomElement el = xml.createElement(DOMAIN_EL);
    el.setAttribute(NAME_ATTR, schema.getDomain());
    projectElement.appendChild(el);
}

void SchemaSerializer::readConfiguration(Configuration* cfg, const QDomElement& owner) {
    QDomNodeList paramNodes = owner.elementsByTagName(PARAMS_EL);
    for(int i=0; i<paramNodes.size(); i++) {
        const QVariantMap& qm = QVariantUtils::string2Map(paramNodes.item(i).toElement().text(), true);
        QMapIterator<QString, QVariant> it(qm);
        while (it.hasNext()) {
            it.next();
            QVariant val = it.value();
            QString keyStr = it.key();
            if(cfg->hasParameter(keyStr)) {
                cfg->getParameter(keyStr)->fromVariant(val);
            }
        }
    }
}

void SchemaSerializer::readParamAliases( QMap<QString, QString> & aliases, const QDomElement& owner ) {
    QDomNodeList alisesNodes = owner.elementsByTagName( ALIASES_EL );
    int sz = alisesNodes.size();
    for( int i = 0; i < sz; ++i ) {
        QDomNamedNodeMap map = alisesNodes.at( i ).toElement().attributes();
        int mapSz = map.length();
        for( int j = 0; j < mapSz; ++j ) {
            QDomNode node = map.item( j );
            QString nodeName = node.nodeName();
            QString nodeValue = node.nodeValue();
            aliases.insert( nodeName, nodeValue );
        }
    }
}

static const QString META_EL = "info";

//static void saveMeta(const Workflow::Metadata* meta, QDomElement& proj){
//    QDomElement el = proj.ownerDocument().createElement(META_EL);
//    proj.appendChild(el);
//    el.setAttribute(NAME_ATTR, meta->name);
//    el.appendChild(proj.ownerDocument().createCDATASection(meta->comment));
//}

QString SchemaSerializer::readMeta(Workflow::Metadata* meta, const QDomElement& proj) {
    QDomElement el = proj.elementsByTagName(META_EL).item(0).toElement();
    meta->name = el.attribute(NAME_ATTR);
    meta->comment = el.text();
    return el.isNull() ? tr("no metadata") : QString();
}

static Port * findPort(const QList<Actor*> & procs, const ActorId & actorId, const QString & portId) {
    foreach(Actor * a, procs) {
        if(a->getId() == actorId) {
            foreach(Port * p, a->getPorts()) {
                if(p->getId() == portId) {
                    return p;
                }
            }
            return NULL;
        }
    }
    return NULL;
}

void SchemaSerializer::updatePortBindings(const QList<Actor*> & procs) {
    foreach(Actor * actor, procs) {
        foreach(Port * p, actor->getEnabledInputPorts()) {
            IntegralBusPort * port = qobject_cast<IntegralBusPort*>(p);
            StrStrMap busMap = port->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
            foreach(const QString & key, busMap.uniqueKeys()) {
                QString val = busMap.value(key);
                QStringList vals = val.split(":", QString::SkipEmptyParts);
                if(vals.size() == 2) {
                    ActorId actorId = str2aid(vals.at(0));
                    QString slot = vals.at(1);

                    Port * inP = findPort(procs, actorId, slot); if(!inP) {continue;}
                    if (inP == NULL) {
                        return;
                    }

                    DataTypePtr inPType = inP->Port::getType();
                    QMap<Descriptor, DataTypePtr> inPTypeMap = inPType->getDatatypesMap();
                    if(inP != NULL && inPType->isMap() && inPTypeMap.keys().size() == 1) {
                        Descriptor d = inPTypeMap.keys().at(0);
                        QString newVal = actorId + ":" + d.getId();
                        coreLog.details(QString("remapping old xml workflow for key %1: old value: %2, new value: %3").
                            arg(key).arg(val).arg(newVal));
                        port->setBusMapValue(key, newVal);
                    }
                }
            }
        }
    }
}

QString SchemaSerializer::xml2schema(const QDomElement& projectElement, Schema* schema, QMap<ActorId, ActorId>& idmap, bool stopIfError) {
    QMap<ActorId, Actor*> procMap;

    QDomElement domainEl = projectElement.elementsByTagName(DOMAIN_EL).item(0).toElement();
    if (!domainEl.isNull()) {
        schema->setDomain(domainEl.attribute(NAME_ATTR));
    }

    ActorPrototypeRegistry* registry = WorkflowEnv::getProtoRegistry();

    QDomNodeList procNodes = projectElement.elementsByTagName(PROCESS_EL);
    for(int i=0; i<procNodes.size(); i++) {
        QDomElement procElement = procNodes.item(i).toElement();
        if (projectElement.isNull()) {
            continue;
        }

        const ActorId id = str2aid(procElement.attribute(ID_ATTR));
        if (stopIfError && procMap.contains(id)) {
            return tr("Invalid content: duplicate process %1").arg(id);
        }

        const QString name = getElemType(procElement.attribute(TYPE_ATTR));
        ActorPrototype* proto = registry->getProto(name);
        if (!proto) {
            if (stopIfError) {
                return tr("Invalid content: unknown process type %1").arg(name);
            } else {
                continue;
            }
        }

        AttributeScript *script;
        const QString scriptText = procElement.attribute(SCRIPT_TEXT);
        if(scriptText.isEmpty()) {
            script = NULL;
        }
        else {
            script = new AttributeScript();
            script->setScriptText(scriptText);
        }
        Actor* proc = proto->createInstance(id, script);
        readConfiguration(proc, procElement);
        readParamAliases( proc->getParamAliases(), procElement );
        proc->setLabel(procElement.attribute(NAME_ATTR));
        procMap[id] = proc;
        schema->addProcess(proc);

        //read port params
        QDomNodeList nl = procElement.elementsByTagName(PORT_EL);
        for(int j=0; j<nl.size(); j++) {
            QDomElement el = nl.item(j).toElement();
            if (el.isNull()) continue;
            QString eid = el.attribute(ID_ATTR);
            Port* p = proc->getPort(eid);
            if (!p) {
                if (stopIfError) {
                    return tr("Invalid content: unknown port %1 requested for %2").arg(eid).arg(name);
                } else {
                    continue;
                }
            }
            readConfiguration(p, el);
        }
    }

    QMapIterator<ActorId, Actor*> it(procMap);
    while(it.hasNext()) {
        it.next();
        idmap[it.key()] = it.value()->getId();
    }
    foreach(Actor* a, procMap) {
        a->remap(idmap);
    }

    QDomNodeList flowNodes = projectElement.elementsByTagName(DATAFLOW_EL);
    for(int i=0; i<flowNodes.size(); i++) {
        QDomElement flowElement = flowNodes.item(i).toElement();
        if (flowElement.isNull()) {
            continue;
        }
        const ActorId inId = str2aid(flowElement.attribute(DST_PROC_ATTR));
        const ActorId outId = str2aid(flowElement.attribute(SRC_PROC_ATTR));

        if (!procMap.contains(inId)) {
            if (stopIfError) {
                return tr("Invalid content: no such process %1 to bind").arg(inId);
            } else {
                continue;
            }
        }
        if (!procMap.contains(outId)) {
            if (stopIfError) {
                return tr("Invalid content: no such process %1 to bind").arg(outId);
            } else {
                continue;
            }
        }
        QString inP = flowElement.attribute(DST_PORT_ATTR);
        QString outP = flowElement.attribute(SRC_PORT_ATTR);

        Port* input = procMap[inId]->getPort(inP);
        Port* output = procMap[outId]->getPort(outP);
        if ((!input || !output || !input->canBind(output))) {
            if (stopIfError) {
                return tr("Invalid content: cannot bind [%1 : %2] to [%3 : %4]").
                    arg(inId).arg(inP).arg(outId).arg(outP);
            }
        } else {
            Link* l = new Link(input, output);
            schema->addFlow(l);
        }
    }
    updatePortBindings(procMap.values());

    foreach(Actor* proc, procMap) {
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

}//namespace Workflow
}//namespace U2
