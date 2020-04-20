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

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/Aliasing.h>
#include <U2Core/AppContext.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowEnv.h>

#include "library/ExternalProcessWorker.h"
#include "library/SchemaWorker.h"
#include "library/ScriptWorker.h"

#include "util/CustomWorkerUtils.h"

#include "CmdlineBasedWorkerValidator.h"
#include "IncludedProtoFactoryImpl.h"


namespace U2 {
using namespace WorkflowSerialize;
namespace Workflow {

const static QString INPUT_PORT_TYPE("input-for-");
const static QString OUTPUT_PORT_TYPE("output-for-");

static const QString IN_PORT_ID("in");
static const QString OUT_PORT_ID("out");

ActorPrototype *IncludedProtoFactoryImpl::_getScriptProto(QList<DataTypePtr > input, QList<DataTypePtr > output, QList<Attribute*> attrs,
                                                          const QString &name,const QString &description, const QString &actorFilePath, bool isAliasName) {
    QList<PortDescriptor*> portDescs;
    QList<Attribute*> attribs = attrs;

    QMap<Descriptor, DataTypePtr> map;
    foreach(const DataTypePtr & tptr, input) {
        if(!tptr || tptr == DataTypePtr()) {
            coreLog.error(LocalWorkflow::ScriptWorker::tr("For input port was set empty data type"));
            return NULL;
        }
        map[WorkflowUtils::getSlotDescOfDatatype(tptr)] = tptr;
    }

    DataTypePtr inSet( new MapDataType(Descriptor(INPUT_PORT_TYPE + name), map) );
    DataTypeRegistry * dr = WorkflowEnv::getDataTypeRegistry();
    assert(dr);
    dr->registerEntry( inSet );

    map.clear();
    foreach(const DataTypePtr & tptr, output) {
        if(!tptr || tptr == DataTypePtr()) {
            coreLog.error(LocalWorkflow::ScriptWorker::tr("For output port was set empty data type"));
            return NULL;
        }
        map[WorkflowUtils::getSlotDescOfDatatype(tptr)] = tptr;
    }

    DataTypePtr outSet( new MapDataType(Descriptor(OUTPUT_PORT_TYPE + name), map) );
    dr->registerEntry( outSet );

    Descriptor inDesc( IN_PORT_ID, LocalWorkflow::ScriptWorker::tr("Input data"), LocalWorkflow::ScriptWorker::tr("Input data") );
    Descriptor outDesc( OUT_PORT_ID, LocalWorkflow::ScriptWorker::tr("Output data"), LocalWorkflow::ScriptWorker::tr("Output data") );

    if(!input.isEmpty()) {
        portDescs << new PortDescriptor( inDesc, inSet, /*input*/ true );
    }
    if(!output.isEmpty()) {
        portDescs << new PortDescriptor( outDesc, outSet, /*input*/false, /*multi*/true );
    }


    QString namePrefix;
    if (!isAliasName) {
        namePrefix = LocalWorkflow::ScriptWorkerFactory::ACTOR_ID;
    }
    Descriptor desc(namePrefix + name, name, description);
    ActorPrototype *proto = new IntegralBusActorPrototype( desc, portDescs, attribs );
    proto->setEditor( new DelegateEditor(QMap<QString, PropertyDelegate*>()) );
    proto->setIconPath(":workflow_designer/images/script.png");

    proto->setPrompter( new LocalWorkflow::ScriptPromter() );
    proto->setScriptFlag();
    proto->setNonStandard(actorFilePath);

    return proto;
}

ActorPrototype *IncludedProtoFactoryImpl::_getExternalToolProto(ExternalProcessConfig *cfg) {
    DataTypeRegistry *dtr = WorkflowEnv::getDataTypeRegistry();
    QList<PortDescriptor*> portDescs;
    foreach(const DataConfig& dcfg, cfg->inputs) {
        QMap<Descriptor, DataTypePtr> map;
        if(dcfg.type == SEQ_WITH_ANNS) {
            map[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
            map[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
        } else {
            map[WorkflowUtils::getSlotDescOfDatatype(dtr->getById(dcfg.type))] = dtr->getById(dcfg.type);
        }

        DataTypePtr input( new MapDataType(Descriptor(INPUT_PORT_TYPE + dcfg.attributeId), map) );
        DataTypeRegistry * dr = WorkflowEnv::getDataTypeRegistry();
        assert(dr);
        dr->registerEntry( input );
        portDescs << new PortDescriptor(Descriptor(dcfg.attributeId, dcfg.attrName, dcfg.description), input, true);
    }

    QMap<Descriptor, DataTypePtr> map;
    foreach(const DataConfig& dcfg, cfg->outputs) {
        if(dcfg.type == SEQ_WITH_ANNS) {
            map[BaseSlots::ANNOTATION_TABLE_SLOT()] = BaseTypes::ANNOTATION_TABLE_TYPE();
            map[BaseSlots::DNA_SEQUENCE_SLOT()] = BaseTypes::DNA_SEQUENCE_TYPE();
        } else {
            const Descriptor slotDesc = generateUniqueSlotDescriptor( map.keys( ), dcfg );
            map[slotDesc] = dtr->getById( dcfg.type );
        }
    }
    if(!map.isEmpty()) {
        DataTypePtr outSet( new MapDataType(Descriptor(OUTPUT_PORT_TYPE + cfg->id), map) );
        DataTypeRegistry * dr = WorkflowEnv::getDataTypeRegistry();
        assert(dr);
        dr->registerEntry( outSet );
        Descriptor outDesc( OUT_PORT_ID, LocalWorkflow::ExternalProcessWorker::tr("Output data"), LocalWorkflow::ExternalProcessWorker::tr("Output data") );
        portDescs << new PortDescriptor( outDesc, outSet, false, true );
    }

    Descriptor desc( cfg->id, cfg->name, cfg->description.isEmpty() ? cfg->name : cfg->description );

    QList<Attribute*> attribs;
    QMap<QString, PropertyDelegate*> delegates;
    foreach(const AttributeConfig& acfg, cfg->attrs) {
        DataTypePtr type;
        QString descr = acfg.description.isEmpty() ? acfg.type : acfg.description;
        if (acfg.type == AttributeConfig::INPUT_FILE_URL_TYPE) {
            type = BaseTypes::STRING_TYPE();
            delegates[acfg.attributeId] = new URLDelegate("", "", false, false, false, nullptr, "", false, true);
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        } else if (acfg.type == AttributeConfig::OUTPUT_FILE_URL_TYPE) {
            type = BaseTypes::STRING_TYPE();
            delegates[acfg.attributeId] = new URLDelegate("", "", false, false, true, nullptr, "", false, false);
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        } else if (acfg.type == AttributeConfig::INPUT_FOLDER_URL_TYPE) {
            type = BaseTypes::STRING_TYPE();
            delegates[acfg.attributeId] = new URLDelegate("", "", false, true, false, nullptr, "", false, true);
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        } else if (acfg.type == AttributeConfig::OUTPUT_FOLDER_URL_TYPE) {
            type = BaseTypes::STRING_TYPE();
            delegates[acfg.attributeId] = new URLDelegate("", "", false, true, true, nullptr, "", false, false);
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        } else if (acfg.type == AttributeConfig::STRING_TYPE) {
            type = BaseTypes::STRING_TYPE();
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        } else if (acfg.type == AttributeConfig::BOOLEAN_TYPE) {
            type = BaseTypes::BOOL_TYPE();
            delegates[acfg.attributeId] = new ComboBoxWithBoolsDelegate();
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, (acfg.defaultValue == "true" ? QVariant(true) : QVariant(false)));
        } else if (acfg.type == AttributeConfig::INTEGER_TYPE) {
            type = BaseTypes::NUM_TYPE();
            QVariantMap integerValues;
            integerValues["minimum"] = QVariant(std::numeric_limits<int>::min());
            integerValues["maximum"] = QVariant(std::numeric_limits<int>::max());
            delegates[acfg.attributeId] = new SpinBoxDelegate(integerValues);
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        } else if (acfg.type == AttributeConfig::DOUBLE_TYPE) {
            type = BaseTypes::NUM_TYPE();
            QVariantMap doubleValues;
            doubleValues["singleStep"] = 0.1;
            doubleValues["minimum"] = QVariant(std::numeric_limits<double>::lowest());
            doubleValues["maximum"] = QVariant(std::numeric_limits<double>::max());
            doubleValues["decimals"] = 6;
            delegates[acfg.attributeId] = new DoubleSpinBoxDelegate(doubleValues);
            attribs << new Attribute(Descriptor(acfg.attributeId, acfg.attrName, descr), type, Attribute::None, acfg.defaultValue);
        }
    }

    ActorPrototype * proto = new IntegralBusActorPrototype( desc, portDescs, attribs );

    proto->setEditor(new DelegateEditor(delegates));
    proto->setIconPath(":workflow_designer/images/external_cmd_tool.png");

    proto->setPrompter( new LocalWorkflow::ExternalProcessWorkerPrompter() );
    proto->setNonStandard(cfg->filePath);
    proto->setValidator(new CmdlineBasedWorkerValidator());

    QStringList commandIdList = CustomWorkerUtils::getToolIdsFromCommand(cfg->cmdLine);
    foreach(const QString& id, commandIdList) {
        proto->addExternalTool(id);
    }

    return proto;
}

ActorPrototype *IncludedProtoFactoryImpl::_getSchemaActorProto(Schema *schema, const QString &name, const QString &actorFilePath) {
    QList<PortDescriptor*> portDescs;
    QList<Attribute*> attrs;

    QMap< QString, PropertyDelegate* > delegateMap;
    QList<Actor*> procs = schema->getProcesses();
    foreach (Actor *proc, procs) {
        if (proc->hasParamAliases()) {
            DelegateEditor *ed = (DelegateEditor*)(proc->getProto()->getEditor());
            QMap<QString, QString> paramAliases = proc->getParamAliases();
            foreach (QString attrId, paramAliases.keys()) {
                Attribute *origAttr = proc->getParameter(attrId);
                Descriptor attrDesc(paramAliases.value(attrId), paramAliases.value(attrId), origAttr->getDocumentation());

                attrs << new Attribute(attrDesc, origAttr->getAttributeType(), origAttr->getFlags(), origAttr->getAttributePureValue());
                PropertyDelegate *d = ed->getDelegate(attrId);
                if (NULL != d) {
                    delegateMap[attrDesc.getId()] = d->clone();
                }
            }
        }
    }

    foreach (const PortAlias &portAlias, schema->getPortAliases()) {
        Descriptor portDescr(portAlias.getAlias(), portAlias.getAlias(), portAlias.getDescription());
        QMap<Descriptor, DataTypePtr> typeMap;

        foreach (const SlotAlias &slotAlias, portAlias.getSlotAliases()) {
            const Port *sourcePort = slotAlias.getSourcePort();
            QMap<Descriptor, DataTypePtr> sourceTypeMap = sourcePort->getOutputType()->getDatatypesMap();
            Descriptor slotDescr(slotAlias.getAlias(), slotAlias.getAlias(), "");

            typeMap[slotDescr] = sourceTypeMap[slotAlias.getSourceSlotId()];
        }
        DataTypePtr type(new MapDataType(dynamic_cast<Descriptor&>(*(portAlias.getSourcePort()->getType())), typeMap));
        PortDescriptor *port = new PortDescriptor(portDescr, type, portAlias.isInput());
        portDescs << port;
    }

    Descriptor desc(name, name, name);
    ActorPrototype *proto = new IntegralBusActorPrototype(desc, portDescs, attrs);
    proto->setEditor(new DelegateEditor(delegateMap));
    proto->setIconPath(":workflow_designer/images/wd.png");

    proto->setPrompter( new LocalWorkflow::SchemaWorkerPrompter());
    proto->setSchema(actorFilePath);

    return proto;
}

bool IncludedProtoFactoryImpl::_registerExternalToolWorker(ExternalProcessConfig *cfg) {
    const bool configRegistered = WorkflowEnv::getExternalCfgRegistry()->registerExternalTool(cfg);
    CHECK(configRegistered, false);

    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalWorkflow::LocalDomainFactory::ID);
    QScopedPointer<LocalWorkflow::ExternalProcessWorkerFactory> factory(new LocalWorkflow::ExternalProcessWorkerFactory(cfg->id));
    const bool factoryRegistered = localDomain->registerEntry(factory.data());
    CHECK_EXT(factoryRegistered, WorkflowEnv::getExternalCfgRegistry()->unregisterConfig(cfg->id), false);
    factory.take();

    return true;
}

void IncludedProtoFactoryImpl::_registerScriptWorker(const QString &actorName) {
    DomainFactory* localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalWorkflow::LocalDomainFactory::ID);
    localDomain->registerEntry(new LocalWorkflow::ScriptWorkerFactory(actorName));
}

ExternalProcessConfig* IncludedProtoFactoryImpl::_getExternalToolWorker(const QString& id) {
    return WorkflowEnv::getExternalCfgRegistry()->getConfigById(id);
}

ExternalProcessConfig *IncludedProtoFactoryImpl::_unregisterExternalToolWorker(const QString &id) {
    DomainFactory *localDomain = WorkflowEnv::getDomainRegistry()->getById(LocalWorkflow::LocalDomainFactory::ID);
    delete localDomain->unregisterEntry(id);

    ExternalProcessConfig *config = WorkflowEnv::getExternalCfgRegistry()->getConfigById(id);
    WorkflowEnv::getExternalCfgRegistry()->unregisterConfig(id);
    return config;
}

Descriptor IncludedProtoFactoryImpl::generateUniqueSlotDescriptor(
    const QList<Descriptor> &existingSlots, const DataConfig &dcfg )
{
    const DataTypeRegistry *dtr = WorkflowEnv::getDataTypeRegistry( );
    Descriptor slotDesc = WorkflowUtils::getSlotDescOfDatatype(
        dtr->getById( dcfg.type ) );
    // add suffix to slot id if there is a slot with the same id
    const int slotDuplicateCounterStart = 1;
    int lastSuffixLength = -1;
    for ( int i = slotDuplicateCounterStart; existingSlots.contains( slotDesc ); ++i ) {
        if ( slotDuplicateCounterStart != i ) {
            const int slotIdBaseLength = slotDesc.getId( ).length( ) - lastSuffixLength;
            slotDesc.setId( slotDesc.getId( ).left( slotIdBaseLength ) );
        }
        const QString suffix = Constants::DASH + QString::number( i );
        lastSuffixLength = suffix.length( );
        slotDesc.setId( slotDesc.getId( ) + suffix );
    }
    return slotDesc;
}

} // Workflow
} // U2
