/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "HRSchemaSerializer.h"

#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/GUrl.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/CollectionUtils.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/ExternalToolCfg.h>
#include <U2Lang/GrouperSlotAttribute.h>
#include <U2Lang/HRWizardSerializer.h>
#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/Marker.h>
#include <U2Lang/MarkerAttribute.h>
#include <U2Lang/SchemaSerializer.h>
#include <U2Lang/ScriptWorkerSerializer.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "Constants.h"
#include "HRVisualSerializer.h"
#include "OldUWL.h"

namespace U2 {
using namespace WorkflowSerialize;

namespace {
template<class T>
void setIfNotNull(const T& what, T* to) {
    if (to != nullptr) {
        *to = what;
    }
}

Attribute* getAttribute(Actor* proc, const QString& attrId) {
    assert(proc != nullptr);
    if (proc->hasParameter(attrId)) {
        return proc->getParameter(attrId);
    } else if (proc->hasParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId()) && attrId == BaseAttributes::URL_LOCATION_ATTRIBUTE().getId()) {
        auto attr = new Attribute(BaseAttributes::URL_LOCATION_ATTRIBUTE(), BaseTypes::BOOL_TYPE(), false, true);
        proc->addParameter(BaseAttributes::URL_LOCATION_ATTRIBUTE().getId(), attr);
        return attr;
    }
    return nullptr;
}

QVariant getAttrValue(Actor* proc, const QString& attrId, const QString& valueStr) {
    Attribute* attr = getAttribute(proc, attrId);
    if (attr == nullptr) {
        throw ReadFailed(HRSchemaSerializer::tr("Parameter '%1' undefined for element '%2'").arg(attrId).arg(proc->getLabel()));
    }
    DataTypeValueFactory* valueFactory = WorkflowEnv::getDataTypeValueFactoryRegistry()->getById(attr->getAttributeType()->getId());
    if (valueFactory == nullptr) {
        throw ReadFailed(HRSchemaSerializer::tr("Cannot parse value from '%1': no value factory").arg(valueStr));
    }
    bool ok = false;
    QVariant value = valueFactory->getValueFromString(valueStr, &ok);
    if (!ok) {
        throw ReadFailed(HRSchemaSerializer::tr("Cannot parse value from '%1'").arg(valueStr));
    }
    return value;
}

QString makeIndent(int tabsNum) {
    tabsNum = tabsNum <= 0 ? 0 : tabsNum;
    QString res;
    for (int i = 0; i < tabsNum; ++i) {
        res += Constants::TAB;
    }
    return res;
}

QString quotedString(const QString& str) {
    return Constants::QUOTE + str + Constants::QUOTE;
}

enum IncludeElementType {
    SCHEMA,
    EXTERNAL_TOOL,
    SCRIPT
};
}  // namespace

const QString HRSchemaSerializer::SCHEMA_PATHS_SETTINGS_TAG = "workflow_settings/schema_paths";

QString HRSchemaSerializer::valueString(const QString& s, bool quoteEmpty) {
    QString str = s;
    str.replace("\"", "'");
    if (str.contains(QRegExp("\\s")) || str.contains(Constants::SEMICOLON) ||
        str.contains(Constants::EQUALS_SIGN) || str.contains(Constants::DATAFLOW_SIGN) ||
        str.contains(Constants::BLOCK_START) || str.contains(Constants::BLOCK_END) ||
        str.contains(Constants::SINGLE_QUOTE) || str.contains(OldConstants::MARKER_START) ||
        (str.isEmpty() && quoteEmpty)) {
        return quotedString(str);
    } else {
        return str;
    }
}

void HRSchemaSerializer::saveSchema(Schema* schema, Metadata* meta, const QString& url, U2OpStatus& os) {
    QFile file(url);
    if (!file.open(QIODevice::WriteOnly)) {
        os.setError(L10N::errorOpeningFileWrite(url));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << schema2String(*schema, meta);

    file.close();
}

bool HRSchemaSerializer::isHeaderLine(const QString& line) {
    return (line.startsWith(Constants::HEADER_LINE) ||
            line.startsWith(Constants::DEPRECATED_HEADER_LINE));
}

void HRSchemaSerializer::checkHeaderLine(const QString& line, Tokenizer& tokenizer) {
    if (!isHeaderLine(line)) {
        if (tokenizer.notEmpty() && line + " " + tokenizer.take() == Constants::OLD_XML_HEADER) {
            throw ReadFailed(tr("XML workflow format is obsolete and not supported"));
        }
        throw ReadFailed(tr("Bad header: expected '%1', got '%2'").arg(Constants::HEADER_LINE).arg(line));
    }
}

void HRSchemaSerializer::parseHeader(Tokenizer& tokenizer, Metadata* meta) {
    QString head = tokenizer.take();
    checkHeaderLine(head, tokenizer);
    QString desc;
    while (tokenizer.look().startsWith(Constants::SERVICE_SYM)) {
        desc += tokenizer.take().mid(Constants::SERVICE_SYM.size()) + Constants::NEW_LINE;
    }
    setIfNotNull<QString>(desc, meta == nullptr ? nullptr : &meta->comment);
}

static bool getAbsoluteIncludePath(QString& path) {
    if (QFileInfo(path).isAbsolute()) {
        return QFile::exists(path);
    }

    QString absPath;
    absPath = WorkflowSettings::getExternalToolDirectory() + path;
    if (QFile::exists(absPath)) {
        path = absPath;
        return true;
    }

    absPath = WorkflowSettings::getUserDirectory() + path;
    if (QFile::exists(absPath)) {
        path = absPath;
        return true;
    }

    absPath = WorkflowSettings::getIncludedElementsDirectory() + path;
    if (QFile::exists(absPath)) {
        path = absPath;
        return true;
    }

    return false;
}

void HRSchemaSerializer::parseIncludes(Tokenizer& tokenizer, QList<QString> includedUrls) {
    tokenizer.assertToken(Constants::INCLUDE);
    QString path = tokenizer.take();
    QString actorId;
    bool includeAs = false;
    QString tok = tokenizer.look();
    if (Constants::INCLUDE_AS == tok) {
        tokenizer.assertToken(Constants::INCLUDE_AS);
        includeAs = true;
        actorId = tokenizer.take();
    }

    if (!getAbsoluteIncludePath(path)) {
        throw ReadFailed(tr("The included file '%1' doesn't exists").arg(path));
    }

    // read the file content
    QString ext = GUrl(path).lastFileSuffix();
    QString rawData;
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            throw ReadFailed(tr("Can't open '%1'").arg(path));
        }
        QTextStream in(&file);
        in.setCodec("UTF-8");
        rawData = in.readAll();
        rawData = rawData.trimmed();
    }

    IncludeElementType includeType;
    ActorPrototype* proto = nullptr;
    ExternalProcessConfig* cfg = nullptr;
    Schema* schema = nullptr;
    QString error;

    // construct the needed proto from the file content
    if (isHeaderLine(rawData)) {
        if ("etc" == ext) {
            includeType = EXTERNAL_TOOL;
            cfg = string2Actor(rawData);
            if (cfg == nullptr) {
                throw ReadFailed(tr("File '%1' contains mistakes").arg(path));
            }
            if (includeAs) {
                cfg->id = actorId;
            } else {
                actorId = cfg->id;
            }
            cfg->filePath = path;
            proto = IncludedProtoFactory::getExternalToolProto(cfg);
        } else {
            includeType = SCHEMA;
            if (includedUrls.contains(path)) {
                throw ReadFailed(tr("There is recursive including of the file: '%1'").arg(path));
            }
            QList<QString> newUrlList = includedUrls;
            newUrlList.append(path);

            schema = new Schema();
            QMap<ActorId, ActorId> procMap;
            error = string2Schema(rawData, schema, nullptr, &procMap, newUrlList);
            if (schema != nullptr && error.isEmpty()) {
                if (includeAs) {
                    schema->setTypeName(actorId);
                } else {
                    actorId = schema->getTypeName();
                }
                proto = IncludedProtoFactory::getSchemaActorProto(schema, actorId, path);
            }
        }
    } else if (rawData.startsWith(Constants::OLD_XML_HEADER)) {
        includeType = SCRIPT;
        proto = ScriptWorkerSerializer::string2actor(rawData, actorId, error, path);
        if (!includeAs && proto != nullptr) {
            actorId = proto->getDisplayName();
        }
    } else {
        throw ReadFailed(tr("Unknown file format: '%1'").arg(path));
    }
    if (proto == nullptr || !error.isEmpty()) {
        throw ReadFailed(QString("Included element \"%1\" contains a error: %2").arg(path).arg(error));
    }

    // register the new proto
    if (IncludedProtoFactory::isRegistered(actorId)) {
        bool isEqualProtos = IncludedProtoFactory::isRegisteredTheSameProto(actorId, proto);
        if (!isEqualProtos) {
            throw ReadFailed(QString("Another worker with ID '%1' is already registered: %1").arg(actorId));
        }
    } else {
        WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_INCLUDES(), proto);
        if (EXTERNAL_TOOL == includeType) {
            IncludedProtoFactory::registerExternalToolWorker(cfg);
        } else if (SCRIPT == includeType) {
            IncludedProtoFactory::registerScriptWorker(actorId);
        } else if (SCHEMA == includeType) {
            WorkflowEnv::getSchemaActorsRegistry()->registerSchema(actorId, schema);
        }
    }
}

void HRSchemaSerializer::parseBodyHeader(Tokenizer& tokenizer, Metadata* meta, bool needName) {
    QString bodyStart = tokenizer.take();
    if (bodyStart != Constants::BODY_START) {
        throw ReadFailed(tr("Bad header: expected '%1', got '%2'").arg(Constants::BODY_START).arg(bodyStart));
    }

    if (tokenizer.look() == Constants::BLOCK_START) {
        if (needName) {
            coreLog.details(tr("Workflow name not specified"));
        }
    } else {
        setIfNotNull<QString>(tokenizer.take(), meta == nullptr ? nullptr : &meta->name);
    }
}

void HRSchemaSerializer::deprecatedUrlAttribute(Actor* proc, const QString& urls) {
    QStringList urlList = urls.split(Constants::SEMICOLON);
    Dataset dSet;
    foreach (const QString& url, urlList) {
        dSet.addUrl(new FileUrlContainer(url));
    }
    Attribute* a = proc->getParameter(BaseAttributes::URL_IN_ATTRIBUTE().getId());
    if (a != nullptr) {
        QList<Dataset> sets;
        sets << dSet;
        a->setAttributeValue(QVariant::fromValue<QList<Dataset>>(sets));
    }
}

QList<Dataset> HRSchemaSerializer::parseUrlAttribute(const QString attrId, QList<StrStrPair>& blockPairs) {
    QList<Dataset> sets;
    QStringList setBlocks;
    foreach (const StrStrPair& pair, blockPairs) {
        if (attrId == pair.first) {
            setBlocks << pair.second;
            blockPairs.removeOne(pair);
        }
    }
    for (const QString& block : qAsConst(setBlocks)) {
        Tokenizer tokenizer;
        tokenizer.tokenize(block);

        QString name;
        QList<URLContainer*> urls;
        try {
            while (tokenizer.notEmpty()) {
                QString tok = tokenizer.take();
                if (Constants::DATASET_NAME == tok) {
                    tokenizer.assertToken(Constants::EQUALS_SIGN);
                    name = tokenizer.take();
                } else if (Constants::FILE_URL == tok) {
                    tokenizer.assertToken(Constants::EQUALS_SIGN);
                    urls << new FileUrlContainer(tokenizer.take());
                } else if (Constants::DIRECTORY_URL == tok) {
                    urls << parseDirectoryUrl(tokenizer);
                } else if (Constants::DB_SELECT == tok) {
                    urls << parseDbSelectUrl(tokenizer);
                }
            }

            if (name.isEmpty()) {
                throw ReadFailed(tr("Url definition does not contain dataset name"));
            }
        } catch (const ReadFailed& ex) {
            for (URLContainer* url : qAsConst(urls)) {
                delete url;
            }
            throw ReadFailed(ex.what);
        }
        Dataset dSet(name);
        for (URLContainer* url : qAsConst(urls)) {
            dSet.addUrl(url);
        }
        sets << dSet;
    }

    return sets;
}

URLContainer* HRSchemaSerializer::createDbObjectUrl(const QString& dbUrl, const qint64 objId, const QString& dataType, const QString& objCachedName) {
    if (-1 == objId) {
        throw ReadFailed(tr("Database select definition: '%1' expected but not found").arg(Constants::DB_OBJECT_ID));
    } else if (dataType.isEmpty()) {
        throw ReadFailed(tr("Database select definition: '%1' expected but not found").arg(Constants::DB_OBJECT_TYPE));
    } else if (objCachedName.isEmpty()) {
        throw ReadFailed(tr("Database select definition: '%1' expected but not found").arg(Constants::DB_OBJ_CACHED_NAME));
    } else {
        const QString objUrl = SharedDbUrlUtils::createDbObjectUrl(dbUrl, objId, dataType, objCachedName);
        if (objUrl.isEmpty()) {
            throw ReadFailed(tr("Database select definition: invalid DB object URL"));
        }
        return new DbObjUrlContainer(objUrl);
    }
}

DbFolderUrlContainer* HRSchemaSerializer::createDbFolderUrl(const QString& dbUrl, const QString& dataType, const QString& path, bool recursive) {
    if (dataType.isEmpty()) {
        throw ReadFailed(tr("Database select definition: '%1' expected but not found").arg(Constants::DB_OBJECT_TYPE));
    } else if (path.isEmpty()) {
        throw ReadFailed(tr("Database select definition: '%1' expected but not found").arg(Constants::PATH));
    }

    const QString folderUrl = SharedDbUrlUtils::createDbFolderUrl(dbUrl, path, BaseTypes::toDataType(dataType));
    if (folderUrl.isEmpty()) {
        throw ReadFailed(tr("Database select definition: invalid DB folder URL"));
    }
    return new DbFolderUrlContainer(folderUrl, QString(), QString(), recursive);
}

URLContainer* HRSchemaSerializer::parseDbSelectUrl(Tokenizer& tokenizer) {
    const QString sign = tokenizer.take();
    if (Constants::BLOCK_START == sign) {
        ParsedPairs pairs(tokenizer);
        tokenizer.assertToken(Constants::BLOCK_END);
        const QString dbUrl = pairs.equalPairs.value(Constants::DB_URL, "");

        const QString dataType = pairs.equalPairs.value(Constants::DB_OBJECT_TYPE, "");
        const QString objIdStr = pairs.equalPairs.value(Constants::DB_OBJECT_ID, "");

        DataTypeValueFactory* numberValueFactory = WorkflowEnv::getDataTypeValueFactoryRegistry()->getById(BaseTypes::NUM_TYPE()->getId());
        qint64 objId = -1;
        bool ok = false;
        const QVariant objIdVar = numberValueFactory->getValueFromString(objIdStr, &ok);
        if (ok) {
            objId = objIdVar.value<qint64>();
        }

        const QString objCachedName = pairs.equalPairs.value(Constants::DB_OBJ_CACHED_NAME, "");

        const QString path = pairs.equalPairs.value(Constants::PATH, "");
        const QString objNameFilter = pairs.equalPairs.value(Constants::DB_OBJ_NAME_FILTER, "");
        const QString accFilter = pairs.equalPairs.value(Constants::DB_SEQ_ACC_FILTER, "");
        const QString recursiveStr = pairs.equalPairs.value(Constants::RECURSIVE, "");

        DataTypeValueFactory* boolValueFactory = WorkflowEnv::getDataTypeValueFactoryRegistry()->getById(BaseTypes::BOOL_TYPE()->getId());
        bool recursive = false;
        ok = false;
        const QVariant recursiveVar = boolValueFactory->getValueFromString(recursiveStr, &ok);
        if (ok) {
            recursive = recursiveVar.toBool();
        }

        if (dbUrl.isEmpty()) {
            throw ReadFailed(tr("Database select definition: '%1' expected but not found").arg(Constants::DB_URL));
        } else if (objId != -1 && !path.isEmpty()) {
            throw ReadFailed(tr("Database select definition: expected either object or folder definition but both found"));
        } else if (objId != -1) {
            return createDbObjectUrl(dbUrl, objId, dataType, objCachedName);
        } else if (!path.isEmpty()) {
            DbFolderUrlContainer* result = createDbFolderUrl(dbUrl, dataType, path, recursive);
            if (!AppContext::isGUIMode()) {
                result->setObjNameFilter(objNameFilter);
                result->setSequenceAccFilter(accFilter);
            }
            return result;
        } else {
            throw ReadFailed(tr("Database select definition: '%1' or '%2' expected but neither found").arg(Constants::DB_OBJECT_ID).arg(Constants::PATH));
        }
    } else {
        throw ReadFailed(tr("Database select definition: '%1' is expected, '%2' is found").arg(Constants::BLOCK_START).arg(sign));
    }
}

URLContainer* HRSchemaSerializer::parseDirectoryUrl(Tokenizer& tokenizer) {
    QString sign = tokenizer.take();
    if (Constants::EQUALS_SIGN == sign) {
        return new DirUrlContainer(tokenizer.take());
    } else if (Constants::BLOCK_START == sign) {
        ParsedPairs pairs(tokenizer);
        tokenizer.assertToken(Constants::BLOCK_END);
        QString path = pairs.equalPairs.value(Constants::PATH, "");
        QString incFilter = pairs.equalPairs.value(Constants::INC_FILTER, "");
        QString excFilter = pairs.equalPairs.value(Constants::EXC_FILTER, "");
        QString recursiveStr = pairs.equalPairs.value(Constants::RECURSIVE, "false");
        DataTypeValueFactory* valueFactory = WorkflowEnv::getDataTypeValueFactoryRegistry()->getById(BaseTypes::BOOL_TYPE()->getId());
        bool recursive = false;
        bool ok = false;
        QVariant v = valueFactory->getValueFromString(recursiveStr, &ok);
        if (ok) {
            recursive = v.toBool();
        }

        return new DirUrlContainer(path, incFilter, excFilter, recursive);
    } else {
        throw ReadFailed(tr("folder url definition: '%1' or '%2' are expected, '%3' is found")
                             .arg(Constants::BLOCK_START)
                             .arg(Constants::EQUALS_SIGN)
                             .arg(sign));
    }
}

Actor* HRSchemaSerializer::parseElementsDefinition(Tokenizer& tokenizer, const QString& actorName, QMap<QString, Actor*>& actorMap, QMap<ActorId, ActorId>* idMap) {
    if (actorName.contains(QRegExp("\\s"))) {
        throw ReadFailed(tr("Element name cannot contain whitespaces: '%1'").arg(actorName));
    }
    if (actorName.contains(Constants::DOT)) {
        throw ReadFailed(tr("Element name cannot contain dots: '%1'").arg(actorName));
    }
    if (actorMap.contains(actorName)) {
        throw ReadFailed(tr("Element '%1' already defined").arg(actorName));
    }

    ParsedPairs pairs(tokenizer);
    QString procType = pairs.equalPairs.take(Constants::TYPE_ATTR);
    if (procType.isEmpty()) {
        throw ReadFailed(tr("Type attribute not set for %1 element").arg(actorName));
    }
    ActorPrototype* proto = WorkflowEnv::getProtoRegistry()->getProto(SchemaSerializer::getElemType(procType));

    QString procScriptText = pairs.blockPairs.take(Constants::SCRIPT_ATTR);
    Actor* proc = nullptr;
    proc = deprecatedActorsReplacer(actorName, procType, pairs);  // AttributeScript always empty for replaced actors
    if (proto == nullptr && proc == nullptr) {
        throw ReadFailed(tr("Unknown type of %1 element: %2").arg(actorName).arg(procType));
    }
    if (proc == nullptr) {
        proc = proto->createInstance(actorName, procScriptText.isEmpty() ? nullptr : new AttributeScript(procScriptText));
    }
    actorMap[actorName] = proc;

    QString procName = pairs.equalPairs.take(Constants::NAME_ATTR);
    if (procName.isEmpty()) {
        throw ReadFailed(tr("Name attribute not set for %1 element").arg(actorName));
    }
    proc->setLabel(procName);

    ActorId oldId = str2aid(pairs.equalPairs.take(Constants::ELEM_ID_ATTR));
    if (idMap != nullptr && !oldId.isEmpty()) {
        idMap->insert(oldId, proc->getId());
    }

    OldUWL::parseOldAttributes(proc, pairs);

    foreach (const QString& key, pairs.blockPairs.uniqueKeys()) {
        Attribute* a = proc->getParameter(key);
        if (a == nullptr) {
            continue;
        }
        if (GROUPER_SLOT_GROUP == a->getGroup()) {
            parseGrouperOutSlots(proc, pairs.blockPairs.values(key), key);
        } else if (MARKER_GROUP == a->getGroup()) {
            parseMarkers(proc, pairs.blockPairs.values(key), key);
        } else if (a->getAttributeType()->getId() == BaseTypes::URL_DATASETS_TYPE()->getId()) {
            QList<Dataset> sets = parseUrlAttribute(a->getId(), pairs.blockPairsList);
            a->setAttributeValue(QVariant::fromValue<QList<Dataset>>(sets));
        } else {
            proc->getParameter(key)->getAttributeScript().setScriptText(pairs.blockPairs.value(key));
        }
    }

    bool workflowContainsInvalidFormatIds = false;
    foreach (const QString& key, pairs.equalPairs.keys()) {
        Attribute* attr = proc->getParameter(key);
        QString value = pairs.equalPairs.value(key);

        if (key == BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId()) {
            if (BaseDocumentFormats::isInvalidId(value)) {
                workflowContainsInvalidFormatIds = true;
                value = BaseDocumentFormats::toValidId(value);
            }
        }

        if (attr != nullptr) {
            attr->setAttributeValue(getAttrValue(proc, key, value));
        } else {
            coreLog.details(tr("Unexpected actor attribute: %1").arg(key));
        }
    }
    if (workflowContainsInvalidFormatIds) {
        GCOUNTER(cvar, "Invalid format IDs: an element was saved with 1.26.0");
    }

    foreach (const QString& valDef, pairs.blockPairs.values(Constants::VALIDATOR)) {
        U2OpStatus2Log os;
        ValidatorDesc desc = parseValidator(valDef, os);
        if (!os.hasError()) {
            proc->addCustomValidator(desc);
        }
    }
    proc->updateItemsAvailability();

    return proc;
}

ValidatorDesc HRSchemaSerializer::parseValidator(const QString& desc, U2OpStatus& os) {
    ValidatorDesc result;
    ParsedPairs pairs(desc, 0);
    if (!pairs.equalPairs.contains(Constants::V_TYPE)) {
        os.setError(tr("No validator type"));
        return result;
    }
    result.type = pairs.equalPairs.take(Constants::V_TYPE);
    int blocks = 0;
    if (Constants::V_SCRIPT == result.type) {
        blocks = 1;
        if (!pairs.blockPairs.contains(Constants::V_SCRIPT)) {
            os.setError(tr("Script validator has not a script"));
            return result;
        }
    }

    if (blocks > pairs.blockPairs.size()) {
        os.setError(tr("Too many blocks in validator definition"));
        return result;
    }

    unite(result.options, pairs.equalPairs);
    unite(result.options, pairs.blockPairs);
    return result;
}

GrouperSlotAction HRSchemaSerializer::parseAction(Tokenizer& tokenizer) {
    ParsedPairs pairs(tokenizer);
    tokenizer.assertToken(Constants::BLOCK_END);

    QString type = pairs.equalPairs.value(Constants::TYPE_ATTR, "");
    if (type.isEmpty()) {
        throw ReadFailed(tr("Grouper out slot action: empty type"));
    } else if (!ActionTypes::isValidType(type)) {
        throw ReadFailed(tr("Grouper out slot action: invalid type: %1").arg(type));
    }
    pairs.equalPairs.take(Constants::TYPE_ATTR);

    GrouperSlotAction result(type);

    foreach (const QString& paramId, pairs.equalPairs.keys()) {
        QString param = pairs.equalPairs.take(paramId);
        if (!ActionParameters::isValidParameter(type, paramId)) {
            throw ReadFailed(tr("Grouper out slot action: invalid parameter: %1").arg(paramId));
        }

        ActionParameters::ParameterType pType = ActionParameters::getType(paramId);
        QVariant var;
        bool ok = false;
        bool b = false;
        int num = 0;
        switch (pType) {
            case ActionParameters::INTEGER:
                num = param.toInt(&ok);
                if (!ok) {
                    throw ReadFailed(tr("Grouper out slot action: bad int '%1' at parameter %2")
                                         .arg(param)
                                         .arg(paramId));
                }
                var = num;
                break;
            case ActionParameters::BOOLEAN:
                if ("true" == param) {
                    b = true;
                } else if ("false" == param) {
                    b = false;
                } else {
                    throw ReadFailed(tr("Grouper out slot action: bad bool '%1' at parameter %2")
                                         .arg(param)
                                         .arg(paramId));
                }
                var = b;
                break;
            case ActionParameters::STRING:
                var = param;
                break;
        }

        result.setParameterValue(paramId, var);
    }

    return result;
}

void HRSchemaSerializer::parseGrouperOutSlots(Actor* proc, const QStringList& outSlotDefs, const QString& attrId) {
    auto attr = dynamic_cast<GrouperOutSlotAttribute*>(proc->getParameter(attrId));
    Tokenizer tokenizer;

    QStringList names;

    foreach (const QString& slotDef, outSlotDefs) {
        tokenizer.tokenizeSchema(slotDef);
        QString name;
        QString inSlot;
        QScopedPointer<GrouperSlotAction> action(nullptr);

        while (tokenizer.notEmpty()) {
            QString tok = tokenizer.take();
            QString next = tokenizer.take();
            if (Constants::EQUALS_SIGN == next) {
                if (Constants::NAME_ATTR == tok) {
                    name = tokenizer.take();
                    if (names.contains(name)) {
                        throw ReadFailed(tr("Grouper out slot: duplicated slot name: %1").arg(name));
                    }
                    names << name;
                } else if (Constants::IN_SLOT == tok) {
                    inSlot = tokenizer.take();
                } else {
                    throw ReadFailed(tr("Grouper out slot: unknown attribute: %1").arg(tok));
                }
            } else if (Constants::BLOCK_START == next) {
                if (Constants::ACTION != tok) {
                    throw ReadFailed(tr("Grouper out slot: unknown block definition: '%1'. %2 expected").arg(tok).arg(Constants::ACTION));
                }
                action.reset(new GrouperSlotAction(parseAction(tokenizer)));
            } else {
                throw ReadFailed(tr("Grouper out slot: unknown token: '%1'. %2 or %3 expected").arg(next).arg(Constants::BLOCK_START).arg(Constants::EQUALS_SIGN));
            }
        }

        if (name.isEmpty()) {
            throw ReadFailed(tr("Grouper out slot: empty slot name"));
        }
        if (inSlot.isEmpty()) {
            throw ReadFailed(tr("Grouper out slot: empty in-slot"));
        }

        GrouperOutSlot slot(name, inSlot);
        if (!action.isNull()) {
            slot.setAction(*action.data());
        }
        attr->addOutSlot(slot);

        Port* outPort = proc->getOutputPorts().at(0);
        assert(outPort->getOutputType()->isMap());
        QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();
        Descriptor newTmpSlot = Descriptor(name, name, name);
        outTypeMap[newTmpSlot] = ActionTypes::getDataTypeByAction(!action.isNull() ? action->getType() : "");
        DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
        outPort->setNewType(newType);
    }
}

void HRSchemaSerializer::finalizeGrouperSlots(const QMap<QString, Actor*>& actorMap) {
    foreach (Actor* p, actorMap.values()) {
        if (p->getId() != "grouper") {  // TODO: fix it
            continue;
        }

        // check incoming slots
    }
}

QString HRSchemaSerializer::parseAt(const QString& dottedStr, int ind) {
    QStringList list = dottedStr.split(Constants::DOT);
    return list.size() > ind ? list.at(ind) : "";
}

QString HRSchemaSerializer::parseAfter(const QString& dottedStr, int ind) {
    QStringList list = dottedStr.split(Constants::DOT);
    QString res;
    for (int i = ind + 1; i < list.size(); ++i) {
        res += list.at(i) + Constants::DOT;
    }
    return res.mid(0, res.size() - Constants::DOT.size());
}

QMap<ActorId, QVariantMap> HRSchemaSerializer::parseIteration(Tokenizer& tokenizer,
                                                              const QMap<QString, Actor*>& actorMap,
                                                              bool pasteMode) {
    QPair<QString, QString> idPair = ParsedPairs::parseOneEqual(tokenizer);
    if (idPair.first != Constants::ITERATION_ID) {
        throw ReadFailed(tr("%1 definition expected at .iterations block").arg(Constants::ITERATION_ID));
    }

    QMap<ActorId, QVariantMap> cfg;
    bool ok = false;
    idPair.second.toInt(&ok);
    if (!ok) {
        throw ReadFailed(tr("Cannot parse integer from '%1': iteration id").arg(idPair.second));
    }

    while (tokenizer.look() != Constants::BLOCK_END) {
        QString actorName = tokenizer.take();
        if (!actorMap.contains(actorName)) {
            if (!pasteMode) {
                throw ReadFailed(tr("Element id '%1' undefined in .iteration block").arg(actorName));
            } else {
                tokenizer.assertToken(Constants::BLOCK_START);
                ParsedPairs::skipBlock(tokenizer);
                continue;
            }
        }

        tokenizer.assertToken(Constants::BLOCK_START);
        ParsedPairs pairs(tokenizer, true /*bigBlocks*/);
        tokenizer.assertToken(Constants::BLOCK_END);

        QString actorId = actorMap[actorName]->getId();
        foreach (Attribute* attr, actorMap[actorName]->getParameters()) {
            QString attrId = attr->getId();
            if (pairs.equalPairs.contains(attrId)) {
                cfg[actorId][attrId] =
                    getAttrValue(actorMap[actorName], attrId, pairs.equalPairs[attrId]);
            }
            if (attr->getAttributeType()->getId() != BaseTypes::URL_DATASETS_TYPE()->getId()) {
                continue;
            }
            QList<Dataset> sets = parseUrlAttribute(attrId, pairs.blockPairsList);
            if (!sets.isEmpty()) {
                cfg[actorId][attrId] = QVariant::fromValue(sets);
            }
        }
    }
    return cfg;
}

static void tryToConnect(Schema* schema, Port* input, Port* output) {
    if (!input || !output || !input->canBind(output) || WorkflowUtils::isPathExist(input, output)) {
        throw ReadFailed(HRSchemaSerializer::tr("Cannot bind %1:%2 to %3:%4").arg(input->owner()->getId()).arg(input->getId()).arg(output->owner()->getId()).arg(output->getId()));
    }
    schema->addFlow(new Link(input, output));
}

void HRSchemaSerializer::parseActorBindings(Tokenizer& tokenizer, WorkflowSchemaReaderData& data) {
    const ActorBindingsGraph& graph = data.schema->getActorBindingsGraph();

    if (!graph.isEmpty()) {
        throw ReadFailed(HRSchemaSerializer::tr("Links list is not empty. Maybe .meta is defined earlier than actor-bindings"));
    }

    while (tokenizer.look() != Constants::BLOCK_END) {
        QString from = tokenizer.take();
        QString srcActorName = HRSchemaSerializer::parseAt(from, 0);
        Actor* srcActor = data.actorMap.value(srcActorName);
        if (srcActor == nullptr) {
            throw ReadFailed(HRSchemaSerializer::tr("Undefined element id: '%1'").arg(srcActorName));
        }
        QString srcPortId = HRSchemaSerializer::parseAt(from, 1);
        Port* srcPort = srcActor->getPort(srcPortId);
        if (srcPort == nullptr) {
            throw ReadFailed(HRSchemaSerializer::tr("Cannot find '%1' port at '%2'").arg(srcPortId).arg(srcActorName));
        }

        tokenizer.assertToken(Constants::DATAFLOW_SIGN);
        QString to = tokenizer.take();
        QString dstActorName = HRSchemaSerializer::parseAt(to, 0);
        Actor* dstActor = data.actorMap.value(dstActorName);
        if (dstActor == nullptr) {
            throw ReadFailed(HRSchemaSerializer::tr("Undefined element id: '%1'").arg(dstActorName));
        }
        QString dstPortId = HRSchemaSerializer::parseAt(to, 1);
        Port* dstPort = dstActor->getPort(dstPortId);
        if (dstPort == nullptr) {
            throw ReadFailed(HRSchemaSerializer::tr("Cannot find '%1' port at '%2'").arg(dstPortId).arg(dstActorName));
        }

        tryToConnect(data.schema, srcPort, dstPort);
    }

    data.defineGraph();

    QString message;
    if (!graph.validateGraph(message)) {
        throw ReadFailed(tr("Validating actor bindings graph failed: '%1'").arg(message));
    }
}

void parseAndCheckParameterAlias(const QString& paramString, const QMap<QString, Actor*>& actorMap, QString& actorName, QString& paramId) {
    actorName = HRSchemaSerializer::parseAt(paramString, 0);
    Actor* actor = actorMap.value(actorName);
    if (actor == nullptr) {
        throw ReadFailed(
            HRSchemaSerializer::tr("%1 element is undefined: at \"%2\" in aliases block").arg(actorName).arg(paramString));
    }

    paramId = HRSchemaSerializer::parseAt(paramString, 1);
    Attribute* attr = actor->getParameter(paramId);
    if (attr == nullptr) {
        throw ReadFailed(
            HRSchemaSerializer::tr("%1 parameter is undefined: at \"%2\" in aliases block").arg(paramId).arg(paramString));
    }
}

void HRSchemaSerializer::parseParameterAliases(Tokenizer& tokenizer, const QMap<QString, Actor*>& actorMap) {
    QList<QString> newParamNames;  // keeps all unique parameters aliases
    QList<QString> paramStrings;  // keeps all unique aliased parameters

    while (tokenizer.look() != Constants::BLOCK_END) {
        QString paramString = tokenizer.take();
        tokenizer.assertToken(Constants::BLOCK_START);
        if (paramStrings.contains(paramString)) {
            throw ReadFailed(tr("Duplicate parameter alias \"%1\"").arg(paramString));
        }
        paramStrings.append(paramString);

        QString actorName;
        QString paramId;
        parseAndCheckParameterAlias(paramString, actorMap, actorName, paramId);

        ParsedPairs pairs(tokenizer);
        if (!pairs.blockPairs.isEmpty()) {
            throw ReadFailed(tr("Empty parameter alias block: \"%1\"").arg(paramString));
        }

        QString alias = pairs.equalPairs.take(Constants::ALIAS);
        if (alias.isEmpty()) {
            alias = paramString;
            alias.replace(Constants::DOT, "_at_");
        }
        if (newParamNames.contains(alias)) {
            throw ReadFailed(tr("Duplicate parameter alias name \"%1\" at \"%2\"").arg(alias).arg(paramString));
        }
        newParamNames.append(alias);

        QString descr = pairs.equalPairs.take(Constants::DESCRIPTION);

        Actor* actor = actorMap[actorName];
        actor->getParamAliases()[paramId] = alias;
        actor->getAliasHelp()[alias] = descr;

        tokenizer.assertToken(Constants::BLOCK_END);
    }
}

// -------------- backward compatibility --------------
void HRSchemaSerializer::parseOldAliases(Tokenizer& tokenizer, const QMap<QString, Actor*>& actorMap) {
    ParsedPairs pairs(tokenizer);
    if (!pairs.blockPairs.isEmpty()) {
        throw ReadFailed(tr("No other blocks allowed in alias block"));
    }
    foreach (const QString& key, pairs.equalPairs.keys()) {
        QString actorName = parseAt(key, 0);
        Actor* actor = actorMap.value(actorName);
        if (actor == nullptr) {
            throw ReadFailed(tr("%1 element undefined in aliases block").arg(actorName));
        }
        QString attributeId = parseAt(key, 1);
        if (!actor->hasParameter(attributeId)) {
            throw ReadFailed(tr("%1 has no parameter %2: in aliases block").arg(actorName).arg(attributeId));
        }
        actor->getParamAliases()[attributeId] = pairs.equalPairs.value(key);
    }
}

void HRSchemaSerializer::parseAliasesHelp(Tokenizer& tokenizer, const QList<Actor*>& procs) {
    ParsedPairs pairs(tokenizer);
    if (!pairs.blockPairs.isEmpty()) {
        throw ReadFailed(tr("No other blocks allowed in help block"));
    }

    foreach (const QString& key, pairs.equalPairs.keys()) {
        QString paramName;
        Actor* actor = WorkflowUtils::findActorByParamAlias(procs, key, paramName, false);
        if (actor == nullptr) {
            throw ReadFailed(tr("Undefined parameter alias used in help block: '%1'").arg(key));
        }
        QString help = pairs.equalPairs.value(key);
        assert(!help.isEmpty());
        actor->getAliasHelp()[key] = help;
    }
}
// ----------------------------------------------------

void parseAndCheckPortAlias(const QString& portString, const QMap<QString, Actor*>& actorMap, QString& actorName, QString& portId) {
    actorName = HRSchemaSerializer::parseAt(portString, 0);
    Actor* actor = actorMap.value(actorName);
    if (actor == nullptr) {
        throw ReadFailed(
            HRSchemaSerializer::tr("%1 element is undefined: at \"%2\" in aliases block").arg(actorName).arg(portString));
    }

    portId = HRSchemaSerializer::parseAt(portString, 1);
    Port* port = actor->getPort(portId);
    if (port == nullptr) {
        throw ReadFailed(
            HRSchemaSerializer::tr("%1 port is undefined: at \"%2\" in aliases block").arg(portId).arg(portString));
    }
}

void parseSlotAlias(const QString& slotString, const QMap<QString, Actor*>& actorMap, QString& actorName, QString& portId, QString& slotId) {
    parseAndCheckPortAlias(slotString, actorMap, actorName, portId);

    slotId = HRSchemaSerializer::parseAt(slotString, 2);
    DataTypePtr dt = actorMap.value(actorName)->getPort(portId)->Port::getType();
    QList<Descriptor> descs = dt->getAllDescriptors();
    if (!descs.contains(slotId)) {
        throw ReadFailed(
            HRSchemaSerializer::tr("%1 slot is undefined: at \"%2\" in aliases block'").arg(slotId).arg(slotString));
    }
}

QPair<Port*, Port*> HRSchemaSerializer::parseDataflow(Tokenizer& tokenizer, const QString& srcTok, const QMap<QString, Actor*>& actorMap) {
    QString srcActorName = parseAt(srcTok, 0);
    QString srcSlotId = parseAfter(srcTok, 0);
    if (!actorMap.contains(srcActorName)) {
        throw ReadFailed(tr("Undefined element id '%1' at '%2'").arg(srcActorName).arg(srcTok));
    }
    bool slotFound = false;
    Port* srcPort = nullptr;
    foreach (Port* port, actorMap.value(srcActorName)->getEnabledOutputPorts()) {
        DataTypePtr dt = port->Port::getType();
        QList<Descriptor> descs = dt->getAllDescriptors();
        descs << *dt;
        slotFound = slotFound || descs.contains(srcSlotId);
        if (slotFound) {
            srcPort = port;
            break;
        }
    }
    if (!slotFound) {
        throw ReadFailed(tr("Undefined slot id '%1' at '%2'").arg(srcSlotId).arg(srcTok));
    }

    tokenizer.assertToken(Constants::DATAFLOW_SIGN);  // "->"
    QString destTok = tokenizer.take();
    QString destActorName = parseAt(destTok, 0);
    QString destPortId = parseAt(destTok, 1);
    QString destSlotId = parseAfter(destTok, 1);
    if (!actorMap.contains(destActorName)) {
        throw ReadFailed(tr("Undefined element id '%1' at '%2'").arg(destActorName).arg(destTok));
    }

    Port* destPort = actorMap.value(destActorName)->getPort(destPortId);
    if (destPort == nullptr) {
        throw ReadFailed(tr("Undefined port id '%1' at '%2'").arg(destPortId).arg(destTok));
    }
    if (!destPort->isInput()) {
        throw ReadFailed(tr("Destination port should be input: %1").arg(destPortId));
    }

    DataTypePtr dt = destPort->Port::getType();
    QList<Descriptor> descs = dt->getAllDescriptors();
    descs << *dt;
    if (!descs.contains(destSlotId)) {
        throw ReadFailed(tr("Undefined slot id '%1' at '%2'").arg(destSlotId).arg(destTok));
    }

    auto bus = qobject_cast<IntegralBusPort*>(destPort);
    IntegralBusSlot slot(srcSlotId, "", actorMap.value(srcActorName)->getId());
    bus->setBusMapValue(destSlotId, slot.toString());

    QString token = tokenizer.look();
    if (Constants::BLOCK_START == token) {
        tokenizer.assertToken(Constants::BLOCK_START);
        ParsedPairs pairs(tokenizer);
        tokenizer.assertToken(Constants::BLOCK_END);

        if (pairs.equalPairs.contains(Constants::PATH_THROUGH)) {
            QStringList path;
            QString value = pairs.equalPairs.take(Constants::PATH_THROUGH);
            foreach (QString p, value.split(",")) {
                p = p.trimmed();
                if (!actorMap.contains(p)) {
                    throw ReadFailed(tr("Undefined actor id '%1' at '%2'").arg(p).arg(value));
                }
                path.append(p);
            }

            bus->addPathBySlotsPair(destSlotId, slot.toString(), path);
        }
    }
    return QPair<Port*, Port*>(srcPort, destPort);
}

static void parseMeta(WorkflowSchemaReaderData& data) {
    QString tok = data.tokenizer.look();
    if (Constants::BLOCK_START != tok) {
        data.schema->setTypeName(tok);
        data.tokenizer.take();
    }

    data.tokenizer.assertToken(Constants::BLOCK_START);
    while (data.tokenizer.look() != Constants::BLOCK_END) {
        tok = data.tokenizer.take();
        if (Constants::PARAM_ALIASES_START == tok) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parseParameterAliases(data.tokenizer, data.actorMap);
            data.tokenizer.assertToken(Constants::BLOCK_END);
        } else if (Constants::VISUAL_START == tok) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            if (data.meta == nullptr) {
                ParsedPairs::skipBlock(data.tokenizer);
            } else {
                HRVisualParser vp(data);
                U2OpStatus2Log os;
                vp.parse(os);
                if (os.hasError()) {
                    data.meta->resetVisual();
                }
                data.tokenizer.assertToken(Constants::BLOCK_END);
            }
        } else if (Constants::OLD_ALIASES_START == tok) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parseOldAliases(data.tokenizer, data.actorMap);
            data.tokenizer.assertToken(Constants::BLOCK_END);
        } else if (Constants::ALIASES_HELP_START == tok) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parseAliasesHelp(data.tokenizer, data.actorMap.values());
            data.tokenizer.assertToken(Constants::BLOCK_END);
        } else if (HRWizardParser::WIZARD == tok) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            HRWizardParser ws(data.tokenizer, data.actorMap);
            U2OpStatusImpl os;
            Wizard* w = ws.parseWizard(os);
            CHECK_OP_EXT(os, throw ReadFailed(os.getError()), );
            data.wizards << w;
            data.tokenizer.assertToken(Constants::BLOCK_END);
        } else if (Constants::ESTIMATIONS == tok) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            QString code = data.tokenizer.take();
            if (data.meta != nullptr) {
                data.meta->estimationsCode = code;
            }
            data.tokenizer.assertToken(Constants::BLOCK_END);
        } else if (Constants::LEGACY_TOKENS_TO_SKIP.contains(tok)) {
            int innerBlocks = 0;
            do {
                QString takenToken = data.tokenizer.take();
                if (takenToken == Constants::BLOCK_START) {
                    innerBlocks++;
                } else if (takenToken == Constants::BLOCK_END) {
                    innerBlocks--;
                }
            } while (innerBlocks != 0);
        } else {
            throw ReadFailed(Constants::UNDEFINED_META_BLOCK.arg(tok));
        }
    }
}

static void parseBody(WorkflowSchemaReaderData& data) {
    Tokenizer& tokenizer = data.tokenizer;
    while (tokenizer.notEmpty() && tokenizer.look() != Constants::BLOCK_END) {
        QString tok = tokenizer.take();
        QString next = tokenizer.look();
        if (tok == Constants::META_START) {
            parseMeta(data);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (tok == Constants::DOT_ITERATION_START) {
            QString itName = tokenizer.look() == Constants::BLOCK_START ? "" : tokenizer.take();
            tokenizer.assertToken(Constants::BLOCK_START);
            QMap<ActorId, QVariantMap> cfg = HRSchemaSerializer::parseIteration(tokenizer, data.actorMap);
            data.schema->applyConfiguration(cfg);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (tok == Constants::ACTOR_BINDINGS) {
            tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parseActorBindings(tokenizer, data);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (tok == OldConstants::MARKER_START) {
            OldUWL::parseMarkerDefinition(tokenizer, data.actorMap);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (next == Constants::DATAFLOW_SIGN) {
            data.dataflowLinks << HRSchemaSerializer::parseDataflow(tokenizer, tok, data.actorMap);
        } else if (next == Constants::BLOCK_START) {
            tokenizer.take();
            Actor* proc = HRSchemaSerializer::parseElementsDefinition(tokenizer, tok, data.actorMap, data.idMap);
            data.schema->addProcess(proc);
            proc->updateDelegateTags();
            tokenizer.assertToken(Constants::BLOCK_END);
        } else {
            throw ReadFailed(Constants::UNDEFINED_CONSTRUCT.arg(tok).arg(next));
        }
    }

    foreach (Actor* proc, data.actorMap.values()) {
        ActorPrototype* proto = proc->getProto();
        if (proto->getEditor() != nullptr) {
            auto actorEd = dynamic_cast<ActorConfigurationEditor*>(proto->getEditor());
            if (actorEd != nullptr) {
                auto editor = dynamic_cast<ActorConfigurationEditor*>(proto->getEditor()->clone());
                editor->setConfiguration(proc);
                proc->setEditor(editor);
            }
        }
    }
}

static void setFlows(WorkflowSchemaReaderData& data) {
    if (data.isGraphDefined()) {
        return;
    }
    if (!data.links.isEmpty()) {
        QList<QPair<Port*, Port*>>::iterator i = data.links.begin();
        for (; i != data.links.end(); i++) {
            tryToConnect(data.schema, i->first, i->second);
        }
    } else {
        FlowGraph graph(data.dataflowLinks);
        graph.minimize();
        QList<Port*> ports = graph.graph.keys();
        for (Port* input : qAsConst(ports)) {
            foreach (Port* output, graph.graph.value(input)) {
                tryToConnect(data.schema, input, output);
            }
        }
    }
}

void HRSchemaSerializer::addEmptyValsToBindings(const QList<Actor*>& procs) {
    for (Actor* actor : qAsConst(procs)) {
        foreach (Port* p, actor->getInputPorts()) {
            auto port = qobject_cast<IntegralBusPort*>(p);
            StrStrMap busMap = port->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
            DataTypePtr t = port->Port::getType();
            assert(t->isMap());
            QMap<Descriptor, DataTypePtr> typeMap = t->getDatatypesMap();
            QList<Descriptor> descriptors = typeMap.keys();
            for (const Descriptor& d : qAsConst(descriptors)) {
                if (!busMap.contains(d.getId())) {
                    port->setBusMapValue(d.getId(), "");
                }
            }
        }
    }
}

QString HRSchemaSerializer::string2Schema(const QString& bytes, Schema* schema, Metadata* meta, QMap<ActorId, ActorId>* idMap, QList<QString> includedUrls) {
    try {
        WorkflowSchemaReaderData data(bytes, schema, meta, idMap);
        parseHeader(data.tokenizer, data.meta);
        data.tokenizer.removeCommentTokens();

        QString tok = data.tokenizer.look();
        while (Constants::INCLUDE == tok) {
            parseIncludes(data.tokenizer, includedUrls);
            tok = data.tokenizer.look();
        }
        parseBodyHeader(data.tokenizer, data.meta);

        if (schema != nullptr) {
            data.tokenizer.assertToken(Constants::BLOCK_START);
            parseBody(data);
            data.tokenizer.assertToken(Constants::BLOCK_END);
            setFlows(data);
            addEmptyValsToBindings(data.actorMap.values());
            data.schema->setWizards(data.wizards);
        }
    } catch (const ReadFailed& ex) {
        return ex.what;
    } catch (...) {
        return Constants::UNKNOWN_ERROR;
    }
    postProcessing(schema);
    return Constants::NO_ERROR;
}

void HRSchemaSerializer::postProcessing(Schema* schema) {
    CHECK(schema != nullptr, );

    foreach (Actor* a, schema->getProcesses()) {
        CHECK(a != nullptr, );
        ActorPrototype* proto = a->getProto();
        CHECK(proto != nullptr, );
        QList<Attribute*> attributes = proto->getAttributes();
        for (Attribute* attr : qAsConst(attributes)) {
            CHECK(attr != nullptr, );
            QList<PortRelationDescriptor*> portRelations = attr->getPortRelations();
            for (PortRelationDescriptor* pd : qAsConst(portRelations)) {
                Port* p = a->getPort(pd->getPortId());
                CHECK(p != nullptr, );
                CHECK(a->hasParameter(attr->getId()), );
                QVariant value = a->getParameter(attr->getId())->getAttributePureValue();
                if (!p->getLinks().isEmpty() && !pd->isPortEnabled(value)) {
                    a->setParameter(attr->getId(), pd->getValuesWithEnabledPort().first());
                }
            }
        }
    }
}

void HRSchemaSerializer::parsePorts(Tokenizer& tokenizer, QList<DataConfig>& ports) {
    while (tokenizer.look() != Constants::BLOCK_END) {
        DataConfig cfg;
        cfg.attributeId = tokenizer.take();
        tokenizer.assertToken(Constants::BLOCK_START);
        ParsedPairs pairs(tokenizer);
        cfg.attrName = pairs.equalPairs.take(Constants::NAME_ATTR);
        cfg.type = pairs.equalPairs.take(Constants::TYPE_PORT);
        cfg.format = pairs.equalPairs.take(Constants::FORMAT_PORT);
        cfg.description = pairs.equalPairs.take(Constants::DESCRIPTION);
        tokenizer.assertToken(Constants::BLOCK_END);

        if (cfg.attrName.isEmpty()) {
            cfg.attrName = cfg.attributeId;
        }

        ports << cfg;
    }
}

void HRSchemaSerializer::parseAttributes(Tokenizer& tokenizer, QList<AttributeConfig>& attrs) {
    while (tokenizer.look() != Constants::BLOCK_END) {
        AttributeConfig cfg;
        cfg.attributeId = tokenizer.take();
        tokenizer.assertToken(Constants::BLOCK_START);
        ParsedPairs pairs(tokenizer);
        cfg.attrName = pairs.equalPairs.take(Constants::NAME_ATTR);
        cfg.type = pairs.equalPairs.take(Constants::TYPE_PORT);
        cfg.defaultValue = pairs.equalPairs.take(Constants::DEFAULT_VALUE);
        cfg.description = pairs.equalPairs.take(Constants::DESCRIPTION);
        if (0 == QString::compare(pairs.equalPairs.take(Constants::ADD_TO_DASHBOARD), Constants::TRUE)) {
            cfg.flags |= AttributeConfig::AddToDashboard;
        }
        if (0 == QString::compare(pairs.equalPairs.take(Constants::OPEN_WITH_UGENE), Constants::TRUE)) {
            cfg.flags |= AttributeConfig::OpenWithUgene;
        }
        tokenizer.assertToken(Constants::BLOCK_END);
        if (cfg.attrName.isEmpty()) {
            cfg.attrName = cfg.attributeId;
        }
        cfg.fixTypes();
        attrs << cfg;
    }
}

ExternalProcessConfig* HRSchemaSerializer::parseActorBody(Tokenizer& tokenizer) {
    auto cfg = new ExternalProcessConfig();
    cfg->id = tokenizer.take();
    while (tokenizer.notEmpty() && tokenizer.look() != Constants::BLOCK_END) {
        QString tok = tokenizer.take();
        QString next = tokenizer.look();
        if (tok == Constants::INPUT_START) {
            tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parsePorts(tokenizer, cfg->inputs);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (tok == Constants::OUTPUT_START) {
            tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parsePorts(tokenizer, cfg->outputs);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (tok == Constants::ATTRIBUTES_START) {
            tokenizer.assertToken(Constants::BLOCK_START);
            HRSchemaSerializer::parseAttributes(tokenizer, cfg->attrs);
            tokenizer.assertToken(Constants::BLOCK_END);
        } else if (tok == Constants::BLOCK_START) {
            // tokenizer.take();
            /*Actor * proc = HRSchemaSerializer::parseElementsDefinition(tokenizer, tok, data.actorMap, data.idMap);
            data.schema->addProcess(proc);
            tokenizer.assertToken(HRSchemaSerializer::BLOCK_END);*/
        } else if (tok == Constants::NAME_ATTR) {
            tokenizer.assertToken(Constants::COLON);
            cfg->name = tokenizer.take();
        } else if (tok == Constants::USE_INTEGRATED_TOOL) {
            tokenizer.assertToken(Constants::COLON);
            cfg->useIntegratedTool = (0 != QString::compare(tokenizer.take(), Constants::FALSE, Qt::CaseInsensitive));
        } else if (tok == Constants::CUSTOM_TOOL_PATH) {
            tokenizer.assertToken(Constants::COLON);
            cfg->customToolPath = tokenizer.take();
        } else if (tok == Constants::INTEGRATED_TOOL_ID) {
            tokenizer.assertToken(Constants::COLON);
            cfg->integratedToolId = tokenizer.take();
        } else if (tok == Constants::CMDLINE) {
            tokenizer.assertToken(Constants::COLON);
            cfg->cmdLine = tokenizer.take();
        } else if (tok == Constants::DESCRIPTION) {
            tokenizer.assertToken(Constants::COLON);
            cfg->description = tokenizer.take();
        } else if (tok == Constants::PROMPTER) {
            tokenizer.assertToken(Constants::COLON);
            cfg->templateDescription = tokenizer.take();
        } else {
            throw ReadFailed(Constants::UNDEFINED_CONSTRUCT.arg(tok).arg(next));
        }
    }

    if (cfg->name.isEmpty()) {
        // Name is absent in old config files, ID was used as worker name.
        cfg->name = cfg->id;
    }

    return cfg;
}

ExternalProcessConfig* HRSchemaSerializer::string2Actor(const QString& bytes) {
    ExternalProcessConfig* cfg = nullptr;
    try {
        WorkflowSchemaReaderData data(bytes, nullptr, nullptr, nullptr);
        parseHeader(data.tokenizer, data.meta);
        cfg = parseActorBody(data.tokenizer);
    } catch (...) {
        return nullptr;
    }
    return cfg;
}

void HRSchemaSerializer::addPart(QString& to, const QString& w) {
    QString what = w;
    if (!what.endsWith(Constants::NEW_LINE)) {
        what.append(Constants::NEW_LINE);
    }
    to += what + Constants::NEW_LINE;
}

QString HRSchemaSerializer::header2String(const Metadata* meta) {
    QString res = Constants::HEADER_LINE + "\n";
    if (meta != nullptr) {
        QStringList descLines = meta->comment.split(Constants::NEW_LINE, Qt::KeepEmptyParts);
        for (int lineIdx = 0; lineIdx < descLines.size(); lineIdx++) {
            const QString& line = descLines.at(lineIdx);
            bool lastLine = (lineIdx == descLines.size() - 1);
            if (lastLine && line.isEmpty()) {
                continue;
            }
            res += Constants::SERVICE_SYM + line + Constants::NEW_LINE;
        }
    }
    return res;
}

QString HRSchemaSerializer::makeBlock(const QString& title, const QString& name, const QString& blockItself, int tabsNum, bool nl, bool sc) {
    QString indent = makeIndent(tabsNum);
    QString blockStart = Constants::BLOCK_START + Constants::NEW_LINE;
    if (nl) {
        blockStart += Constants::NEW_LINE;
    }
    QString blockEnd = Constants::BLOCK_END;
    if (sc) {
        blockEnd += Constants::SEMICOLON;
    }
    blockEnd += Constants::NEW_LINE;
    return indent + title + " " + valueString(name) + blockStart + blockItself + indent + blockEnd;
}

QString HRSchemaSerializer::makeEqualsPair(const QString& key, const QString& value, int tabsNum, bool quoteEmpty) {
    return makeIndent(tabsNum) + key + Constants::EQUALS_SIGN + valueString(value, quoteEmpty) + Constants::SEMICOLON + Constants::NEW_LINE;
}

QString HRSchemaSerializer::makeArrowPair(const QString& left, const QString& right, int tabsNum) {
    return makeIndent(tabsNum) + left + Constants::DATAFLOW_SIGN + right;
}

QString HRSchemaSerializer::scriptBlock(const QString& scriptText, int tabsNum) {
    QString indent = makeIndent(tabsNum);
    QString res;
    QStringList scriptLines = scriptText.split(Constants::NEW_LINE, Qt::SkipEmptyParts);
    foreach (const QString& line, scriptLines) {
        res += indent + line + Constants::NEW_LINE;
    }
    return res;
}

QString HRSchemaSerializer::grouperOutSlotsDefinition(Attribute* attribute) {
    auto a = dynamic_cast<GrouperOutSlotAttribute*>(attribute);
    QString result;

    foreach (const GrouperOutSlot& slot, a->getOutSlots()) {
        QString mRes;
        mRes += makeEqualsPair(Constants::NAME_ATTR, slot.getOutSlotId(), 3);
        mRes += makeEqualsPair(Constants::IN_SLOT, slot.getInSlotStr(), 3);

        GrouperSlotAction* const action = slot.getAction();
        if (action != nullptr) {
            QString actionBlock;
            actionBlock += makeEqualsPair(Constants::TYPE_ATTR, action->getType(), 4);
            QList<QString> parameters = action->getParameters().keys();
            for (const QString& paramId : qAsConst(parameters)) {
                QVariant value = action->getParameterValue(paramId);
                actionBlock += makeEqualsPair(paramId, value.toString(), 4);
            }
            mRes += makeBlock(Constants::ACTION, Constants::NO_NAME, actionBlock, 3);
        }

        result += makeBlock(Constants::OUT_SLOT_ATTR, Constants::NO_NAME, mRes, 2);
    }

    return result;
}

class HRUrlSerializer : public URLContainerVisitor {
public:
    HRUrlSerializer(int _tabCount)
        : tabCount(_tabCount) {
    }

    void visit(FileUrlContainer* url) override {
        result = HRSchemaSerializer::makeEqualsPair(Constants::FILE_URL, url->getUrl(), tabCount);
    }

    void visit(DirUrlContainer* url) override {
        if (url->getIncludeFilter().isEmpty() && url->getExcludeFilter().isEmpty() && !url->isRecursive()) {
            result = HRSchemaSerializer::makeEqualsPair(Constants::DIRECTORY_URL, url->getUrl(), tabCount);
            return;
        }

        QString res;
        res += HRSchemaSerializer::makeEqualsPair(Constants::PATH, url->getUrl(), tabCount + 1);

        processDirUrlContainerOptionalParams(url, res);

        result = HRSchemaSerializer::makeBlock(Constants::DIRECTORY_URL, Constants::NO_NAME, res, tabCount);
    }

    void visit(DbObjUrlContainer* url) override {
        const QString dbObjUrl = url->getUrl();

        QString res;
        res += HRSchemaSerializer::makeEqualsPair(Constants::DB_URL, SharedDbUrlUtils::getDbUrlFromEntityUrl(dbObjUrl), tabCount + 1);
        res += HRSchemaSerializer::makeEqualsPair(Constants::DB_OBJECT_ID, QString::number(SharedDbUrlUtils::getObjectNumberIdByUrl(dbObjUrl)), tabCount + 1);
        res += HRSchemaSerializer::makeEqualsPair(Constants::DB_OBJECT_TYPE, SharedDbUrlUtils::getDbSerializedObjectTypeByUrl(dbObjUrl), tabCount + 1);
        res += HRSchemaSerializer::makeEqualsPair(Constants::DB_OBJ_CACHED_NAME, SharedDbUrlUtils::getDbObjectNameByUrl(dbObjUrl), tabCount + 1);

        result = HRSchemaSerializer::makeBlock(Constants::DB_SELECT, Constants::NO_NAME, res, tabCount);
    }

    void visit(DbFolderUrlContainer* url) override {
        const QString dbFolderUrl = url->getUrl();

        QString res;
        res += HRSchemaSerializer::makeEqualsPair(Constants::DB_URL, SharedDbUrlUtils::getDbUrlFromEntityUrl(dbFolderUrl), tabCount + 1);
        res += HRSchemaSerializer::makeEqualsPair(Constants::PATH, SharedDbUrlUtils::getDbFolderPathByUrl(dbFolderUrl), tabCount + 1);
        res += HRSchemaSerializer::makeEqualsPair(Constants::DB_OBJECT_TYPE, SharedDbUrlUtils::getDbFolderSerializedDataTypeByUrl(dbFolderUrl), tabCount + 1);

        processDbFolderUrlContainerOptionalParams(url, res);

        result = HRSchemaSerializer::makeBlock(Constants::DB_SELECT, Constants::NO_NAME, res, tabCount);
    }

    const QString& getResult() {
        return result;
    }

private:
    void processDirUrlContainerOptionalParams(DirUrlContainer* url, QString& res) {
        const QString incFilter = url->getIncludeFilter();
        if (!incFilter.isEmpty()) {
            res += HRSchemaSerializer::makeEqualsPair(Constants::INC_FILTER, incFilter, tabCount + 1);
        }

        const QString excFilter = url->getExcludeFilter();
        if (!excFilter.isEmpty()) {
            res += HRSchemaSerializer::makeEqualsPair(Constants::EXC_FILTER, excFilter, tabCount + 1);
        }

        bool recursive = url->isRecursive();
        if (recursive) {
            const QString recStr = recursive ? BoolTypeValueFactory::TRUE_STR : BoolTypeValueFactory::FALSE_STR;
            res += HRSchemaSerializer::makeEqualsPair(Constants::RECURSIVE, recStr, tabCount + 1);
        }
    }

    void processDbFolderUrlContainerOptionalParams(DbFolderUrlContainer* url, QString& res) {
        bool recursive = url->isRecursive();
        if (recursive) {
            const QString recStr = recursive ? BoolTypeValueFactory::TRUE_STR : BoolTypeValueFactory::FALSE_STR;
            res += HRSchemaSerializer::makeEqualsPair(Constants::RECURSIVE, recStr, tabCount + 1);
        }

        const QString accFilter = url->getSequenceAccFilter();
        if (!accFilter.isEmpty()) {
            res += HRSchemaSerializer::makeEqualsPair(Constants::DB_SEQ_ACC_FILTER, accFilter, tabCount + 1);
        }

        const QString objNameFilter = url->getObjNameFilter();
        if (!objNameFilter.isEmpty()) {
            res += HRSchemaSerializer::makeEqualsPair(Constants::DB_OBJ_NAME_FILTER, objNameFilter, tabCount + 1);
        }
    }

    int tabCount;
    QString result;
};

static QString inUrlDefinitionBlocks(const QString& attrId, const QList<Dataset>& sets, int depth) {
    QString res;
    for (const Dataset& dSet : qAsConst(sets)) {
        QString setDef;
        setDef += HRSchemaSerializer::makeEqualsPair(Constants::DATASET_NAME, dSet.getName(), depth + 1);
        foreach (URLContainer* url, dSet.getUrls()) {
            HRUrlSerializer us(depth + 1);
            url->accept(&us);
            setDef += us.getResult();
        }
        res += HRSchemaSerializer::makeBlock(attrId,
                                             Constants::NO_NAME,
                                             setDef,
                                             depth);
    }
    return res;
}

static QString validatorDefinition(const ValidatorDesc& desc, int depth) {
    QString result;
    QMap<QString, QString> options = desc.options;
    result += HRSchemaSerializer::makeEqualsPair(Constants::V_TYPE, desc.type, depth);
    if (Constants::V_SCRIPT == desc.type) {
        QString script = options.take(Constants::V_SCRIPT);
        result += HRSchemaSerializer::makeBlock(Constants::V_SCRIPT, Constants::NO_NAME, makeIndent(depth + 1) + script + Constants::NEW_LINE, depth, false, false);
    }
    foreach (const QString& key, options.keys()) {
        result += HRSchemaSerializer::makeEqualsPair(key, options[key], depth);
    }
    return result;
}

static QString elementsDefinitionBlock(Actor* actor, bool copyMode) {
    assert(actor != nullptr);
    QString res;
    // save global attributes
    res += HRSchemaSerializer::makeEqualsPair(Constants::TYPE_ATTR, actor->getProto()->getId());
    res += HRSchemaSerializer::makeEqualsPair(Constants::NAME_ATTR, actor->getLabel());
    if (copyMode) {
        res += HRSchemaSerializer::makeEqualsPair(Constants::ELEM_ID_ATTR, actor->getId());
    }
    AttributeScript* actorScript = actor->getScript();
    if (actorScript != nullptr && !actorScript->getScriptText().trimmed().isEmpty()) {
        res += HRSchemaSerializer::makeBlock(Constants::SCRIPT_ATTR, Constants::NO_NAME, actorScript->getScriptText() + Constants::NEW_LINE, 2, false, true);
    }

    // save local attributes
    foreach (Attribute* attribute, actor->getParameters().values()) {
        assert(attribute != nullptr);
        if (attribute->getGroup() == GROUPER_SLOT_GROUP) {
            res += HRSchemaSerializer::grouperOutSlotsDefinition(attribute);
        } else if (MARKER_GROUP == attribute->getGroup()) {
            res += HRSchemaSerializer::markersDefinition(attribute);
        } else {
            if (attribute->getAttributeType() == BaseTypes::URL_DATASETS_TYPE()) {
                QVariant v = attribute->getAttributePureValue();
                if (v.canConvert<QList<Dataset>>()) {
                    QList<Dataset> sets = v.value<QList<Dataset>>();
                    res += inUrlDefinitionBlocks(attribute->getId(), sets, 2);
                    continue;
                }
            }
            QString attributeId = attribute->getId();
            assert(!attributeId.contains(QRegExp("\\s")));

            const AttributeScript& attrScript = attribute->getAttributeScript();
            if (!attrScript.isEmpty()) {
                res += HRSchemaSerializer::makeBlock(attributeId, Constants::NO_NAME, attrScript.getScriptText() + Constants::NEW_LINE, 2, false, true);
                continue;
            }

            if (attribute->isDefaultValue()) {
                continue;
            }
            QVariant value = attribute->getAttributePureValue();

#ifdef _DEBUG
            const bool valueIsNull = value.isNull();
            const bool valueCanConvertToString = value.canConvert<QString>();
            const bool valueCanConvertToStringList = value.canConvert<QStringList>();
            const bool valueCanConvertToMap = value.canConvert<QMap<QString, QVariant>>();
            assert(valueIsNull ||
                   valueCanConvertToString ||
                   valueCanConvertToStringList ||
                   valueCanConvertToMap);
#endif

            QString valueString;
            if (attribute->getAttributeType() == BaseTypes::STRING_LIST_TYPE()) {
                valueString = StrPackUtils::packStringList(value.toStringList(), StrPackUtils::SingleQuotes);
            } else if (attribute->getAttributeType() == BaseTypes::MAP_TYPE()) {
                valueString = StrPackUtils::packMap(value.toMap(), StrPackUtils::SingleQuotes);
            } else {
                valueString = value.toString();
            }
            res += HRSchemaSerializer::makeEqualsPair(attributeId, valueString, 2, true);
        }
    }

    foreach (const ValidatorDesc& desc, actor->getCustomValidators()) {
        res += HRSchemaSerializer::makeBlock(Constants::VALIDATOR, Constants::NO_NAME, validatorDefinition(desc, 3), 2, false, false);
    }

    return res;
}

static QString tryGetRelativePath(const QString& path) {
    QString dir;

    if (path.startsWith(WorkflowSettings::getExternalToolDirectory())) {
        dir = WorkflowSettings::getExternalToolDirectory();
    } else if (path.startsWith(WorkflowSettings::getUserDirectory())) {
        dir = WorkflowSettings::getUserDirectory();
    } else if (path.startsWith(WorkflowSettings::getIncludedElementsDirectory())) {
        dir = WorkflowSettings::getIncludedElementsDirectory();
    }

    if (dir.isEmpty()) {
        return path;
    } else {
        return path.mid(dir.length());
    }
}

QString HRSchemaSerializer::includesDefinition(const QList<Actor*>& procs) {
    QString res;
    foreach (Actor* proc, procs) {
        ActorPrototype* proto = proc->getProto();
        if (!proto->isStandardFlagSet()) {
            res += Constants::INCLUDE + " \"" + tryGetRelativePath(proto->getFilePath()) + "\" ";
            res += Constants::INCLUDE_AS + " \"" + proto->getId() + "\"" + Constants::NEW_LINE;
        }
    }

    return res;
}

QString HRSchemaSerializer::elementsDefinition(const QList<Actor*>& procs, const NamesMap& nmap, bool copyMode) {
    QString res;
    foreach (Actor* actor, procs) {
        QString idStr = nmap[actor->getId()];
        SAFE_POINT(!idStr.contains(QRegExp("\\s")), "Error: element name in the workflow file contains spaces", QString());
        res += makeBlock(idStr, Constants::NO_NAME, elementsDefinitionBlock(actor, copyMode));
    }
    return res + Constants::NEW_LINE;
}

static QString markerDefinitionBlock(Marker* marker, int tabsNum) {
    assert(marker != nullptr);
    QString res;
    res += HRSchemaSerializer::makeEqualsPair(Constants::TYPE_ATTR, marker->getType(), tabsNum);
    res += HRSchemaSerializer::makeEqualsPair(Constants::NAME_ATTR, marker->getName(), tabsNum);

    if (QUALIFIER == marker->getGroup()) {
        const QString& qualName = dynamic_cast<QualifierMarker*>(marker)->getQualifierName();
        if (!qualName.isEmpty()) {
            res += HRSchemaSerializer::makeEqualsPair(Constants::QUAL_NAME, qualName, tabsNum);
        }
    } else if (ANNOTATION == marker->getGroup()) {
        const QString& annName = dynamic_cast<AnnotationMarker*>(marker)->getAnnotationName();
        if (!annName.isEmpty()) {
            res += HRSchemaSerializer::makeEqualsPair(Constants::ANN_NAME, annName, tabsNum);
        }
    }

    foreach (QString key, marker->getValues().keys()) {
        QString val = marker->getValues().value(key);
        res += HRSchemaSerializer::makeEqualsPair("\"" + key + "\"", val, tabsNum);
    }
    return res;
}

static QString actorBindingsBlock(const ActorBindingsGraph& graph, const HRSchemaSerializer::NamesMap& nmap, bool) {
    QString res;

    foreach (Port* srcPort, graph.getBindings().keys()) {
        QString srcActorId = nmap[srcPort->owner()->getId()];
        QString srcPortId = srcPort->getId();
        QList<Port*> ports = graph.getBindings().value(srcPort);
        for (Port* dstPort : qAsConst(ports)) {
            QString dstActorId = nmap[dstPort->owner()->getId()];
            QString dstPortId = dstPort->getId();

            res += HRSchemaSerializer::makeArrowPair(srcActorId + Constants::DOT + srcPortId,
                                                     dstActorId + Constants::DOT + dstPortId,
                                                     2) +
                   Constants::NEW_LINE;
        }
    }
    return res;
}

QString HRSchemaSerializer::actorBindings(const ActorBindingsGraph& graph, const NamesMap& nmap, bool copyMode) {
    QString res;
    res += makeBlock(Constants::ACTOR_BINDINGS, Constants::NO_NAME, actorBindingsBlock(graph, nmap, copyMode));
    return res + Constants::NEW_LINE;
}

static bool containsProcWithId(const QList<Actor*>& procs, const ActorId& id) {
    foreach (Actor* a, procs) {
        if (a->getId() == id) {
            return true;
        }
    }
    return false;
}

QString HRSchemaSerializer::dataflowDefinition(const QList<Actor*>& procs, const NamesMap& nmap) {
    QString res;
    for (Actor* actor : qAsConst(procs)) {
        foreach (Port* inputPort, actor->getEnabledInputPorts()) {
            StrStrMap busMap = inputPort->getParameter(IntegralBusPort::BUS_MAP_ATTR_ID)->getAttributeValueWithoutScript<StrStrMap>();
            auto busPort = qobject_cast<IntegralBusPort*>(inputPort);

            const QList<QString>& keys = busMap.keys();
            for (const QString& key : qAsConst(keys)) {
                QStringList srcList = busMap.value(key).split(";", Qt::SkipEmptyParts);
                QStringList uniqList;
                for (QString src : qAsConst(srcList)) {
                    if (!uniqList.contains(src)) {
                        uniqList << src;
                    }
                }

                for (QString src : qAsConst(uniqList)) {
                    if (src.isEmpty()) {
                        continue;
                    }
                    QList<QStringList> paths = busPort->getPathsBySlotsPair(key, src);
                    src = src.replace(Constants::COLON, Constants::DOT);
                    ActorId srcActorId = parseAt(src, 0);

                    if (containsProcWithId(procs, srcActorId)) {
                        QString arrowPair = makeArrowPair(src.replace(srcActorId, nmap[srcActorId]),
                                                          nmap[actor->getId()] + Constants::DOT + inputPort->getId() + Constants::DOT + key,
                                                          0);

                        if (paths.isEmpty()) {
                            res += makeIndent(1) + arrowPair + Constants::NEW_LINE;
                        } else {
                            for (const QStringList& path : qAsConst(paths)) {
                                QString pathString = path.join(", ");
                                QString pair = makeEqualsPair(Constants::PATH_THROUGH, pathString, 2);
                                res += makeBlock(arrowPair, Constants::NO_NAME, pair);
                            }
                        }
                    }
                }
            }
        }
    }
    return res + Constants::NEW_LINE;
}

static QString visualData(const Schema& schema, const HRSchemaSerializer::NamesMap& nmap) {
    QString res;
    foreach (Link* link, schema.getFlows()) {
        Port* src = link->source();
        Port* dst = link->destination();
        res += HRSchemaSerializer::makeArrowPair(nmap[src->owner()->getId()] + Constants::DOT + src->getId(),
                                                 nmap[dst->owner()->getId()] + Constants::DOT + dst->getId(),
                                                 0) +
               Constants::NEW_LINE;
    }
    return res;
}

static QString itemsMetaData(const QList<Actor*>& actors, const Metadata* meta, const HRSchemaSerializer::NamesMap& nmap) {
    QString res;
    bool hasParameterAliases = false;
    foreach (Actor* a, actors) {
        if (a->hasParamAliases()) {
            hasParameterAliases = true;
            break;
        }
    }
    if (hasParameterAliases) {
        res += HRSchemaSerializer::makeBlock(Constants::PARAM_ALIASES_START, Constants::NO_NAME, HRSchemaSerializer::schemaParameterAliases(actors, nmap), 2);
    }

    if (meta != nullptr) {
        HRVisualSerializer vs(*meta, nmap);
        res += vs.serialize(2);
    }
    return res;
}

static QString metaData(const Schema& schema, const Metadata* meta, const HRSchemaSerializer::NamesMap& nmap) {
    QString res;

    res += itemsMetaData(schema.getProcesses(), meta, nmap);

    if (meta != nullptr && !meta->estimationsCode.isEmpty()) {
        res += HRSchemaSerializer::makeBlock(Constants::ESTIMATIONS, Constants::NO_NAME, meta->estimationsCode + Constants::NEW_LINE, 2);
    }

    if (meta == nullptr) {
        res += HRSchemaSerializer::makeBlock(Constants::VISUAL_START, Constants::NO_NAME, visualData(schema, nmap), 2);
    }

    foreach (Wizard* w, schema.getWizards()) {
        HRWizardSerializer ws;
        res += ws.serialize(w, 2);
    }
    return res;
}

QString HRSchemaSerializer::schemaParameterAliases(const QList<Actor*>& procs, const NamesMap& nmap) {
    QString res;
    for (Actor* actor : qAsConst(procs)) {
        const QMap<QString, QString>& aliases = actor->getParamAliases();
        foreach (const QString& attrId, aliases.keys()) {
            QString pairs;
            QString alias = aliases.value(attrId);
            QString descr = actor->getAliasHelp()[alias];
            pairs += makeEqualsPair(Constants::ALIAS, alias, 4);
            if (!descr.isEmpty()) {
                pairs += makeEqualsPair(Constants::DESCRIPTION, descr, 4);
            }
            QString paramString = nmap[actor->getId()] + Constants::DOT + attrId;
            res += makeBlock(paramString, Constants::NO_NAME, pairs, 3);
        }
    }
    return res;
}

HRSchemaSerializer::NamesMap HRSchemaSerializer::generateElementNames(const QList<Actor*>& procs) {
    QMap<ActorId, QString> nmap;
    foreach (Actor* proc, procs) {
        QString id = aid2str(proc->getId());
        QString name = id.replace(QRegExp("\\s"), "-");
        nmap[proc->getId()] = name;  // generateElementName(proc, nmap.values());
    }
    return nmap;
}

static QString bodyItself(const Schema& schema, const Metadata* meta, bool copyMode) {
    HRSchemaSerializer::NamesMap nmap = HRSchemaSerializer::generateElementNames(schema.getProcesses());
    QString res;
    res += HRSchemaSerializer::elementsDefinition(schema.getProcesses(), nmap, copyMode);
    res += HRSchemaSerializer::actorBindings(schema.getActorBindingsGraph(), nmap, copyMode);
    res += HRSchemaSerializer::dataflowDefinition(schema.getProcesses(), nmap);
    res += HRSchemaSerializer::makeBlock(Constants::META_START, schema.getTypeName(), metaData(schema, meta, nmap));
    return res;
}

QString HRSchemaSerializer::schema2String(const Schema& schema, const Metadata* meta, bool copyMode) {
    QString res;
    addPart(res, header2String(meta));
    addPart(res, includesDefinition(schema.getProcesses()));
    addPart(res, makeBlock(Constants::BODY_START, meta ? meta->name : "", bodyItself(schema, meta, copyMode), 0, true));
    return res;
}

QString HRSchemaSerializer::items2String(const QList<Actor*>& actors, const Metadata* meta) {
    assert(!actors.isEmpty());
    QString res;
    HRSchemaSerializer::addPart(res, HRSchemaSerializer::header2String(meta));

    QString iData;
    HRSchemaSerializer::NamesMap nmap = HRSchemaSerializer::generateElementNames(actors);
    iData += HRSchemaSerializer::elementsDefinition(actors, nmap);
    iData += HRSchemaSerializer::dataflowDefinition(actors, nmap);
    iData += HRSchemaSerializer::makeBlock(Constants::META_START, Constants::NO_NAME, itemsMetaData(actors, meta, nmap));

    HRSchemaSerializer::addPart(res, HRSchemaSerializer::makeBlock(Constants::BODY_START, Constants::NO_NAME, iData, 0, true));
    return res;
}

QMap<ActorId, ActorId> HRSchemaSerializer::deepCopy(const Schema& from, Schema* to, U2OpStatus& os) {
    assert(to != nullptr);
    QString data = schema2String(from, nullptr, true);
    QMap<ActorId, ActorId> idMap;
    QString err = string2Schema(data, to, nullptr, &idMap);
    if (!err.isEmpty()) {
        os.setError(err);
        coreLog.details(err);
        to->reset();
        return QMap<ActorId, ActorId>();
    }
    to->setDeepCopyFlag(true);
    return idMap;
}

static QString inputsDefenition(const QList<DataConfig>& inputs) {
    QString res = Constants::TAB + Constants::INPUT_START + " {\n";
    foreach (const DataConfig& cfg, inputs) {
        res += Constants::TAB + Constants::TAB + cfg.attributeId + " {\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "name:\"" + cfg.attrName + "\";\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "type:" + cfg.type + ";\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "format:" + cfg.format + ";\n";
        if (!cfg.description.isEmpty()) {
            res += Constants::TAB + Constants::TAB + Constants::TAB + "description:\"" + cfg.description + "\";\n";
        }
        res += Constants::TAB + Constants::TAB + "}\n";
    }
    res += Constants::TAB + "}\n";
    return res;
}

static QString outputsDefenition(const QList<DataConfig>& inputs) {
    QString res = Constants::TAB + Constants::OUTPUT_START + " {\n";
    foreach (const DataConfig& cfg, inputs) {
        res += Constants::TAB + Constants::TAB + cfg.attributeId + " {\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "name:\"" + cfg.attrName + "\";\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "type:" + cfg.type + ";\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "format:" + cfg.format + ";\n";
        if (!cfg.description.isEmpty()) {
            res += Constants::TAB + Constants::TAB + Constants::TAB + "description:\"" + cfg.description + "\";\n";
        }
        res += Constants::TAB + Constants::TAB + "}\n";
    }
    res += Constants::TAB + "}\n";
    return res;
}

namespace {

QString bool2String(bool value) {
    return value ? Constants::TRUE : Constants::FALSE;
}

}  // namespace

static QString attributesDefinition(const QList<AttributeConfig>& attrs) {
    QString res = Constants::TAB + Constants::ATTRIBUTES_START + " {\n";
    foreach (const AttributeConfig& cfg, attrs) {
        res += Constants::TAB + Constants::TAB + cfg.attributeId + " {\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "name:\"" + cfg.attrName + "\";\n";
        res += Constants::TAB + Constants::TAB + Constants::TAB + "type:" + cfg.type + ";\n";
        if (!cfg.defaultValue.isEmpty()) {
            res += Constants::TAB + Constants::TAB + Constants::TAB + Constants::DEFAULT_VALUE + ":\"" + cfg.defaultValue + "\";\n";
        }
        if (!cfg.description.isEmpty()) {
            res += Constants::TAB + Constants::TAB + Constants::TAB + Constants::DESCRIPTION + Constants::COLON + Constants::QUOTE + cfg.description + Constants::QUOTE + Constants::SEMICOLON + Constants::NEW_LINE;
        }
        if (cfg.isOutputUrl()) {
            res += Constants::TAB + Constants::TAB + Constants::TAB + Constants::ADD_TO_DASHBOARD + Constants::COLON + bool2String(cfg.flags.testFlag(AttributeConfig::AddToDashboard)) + Constants::SEMICOLON + Constants::NEW_LINE;
            if (cfg.isFile()) {
                res += Constants::TAB + Constants::TAB + Constants::TAB + Constants::OPEN_WITH_UGENE + Constants::COLON + bool2String(cfg.flags.testFlag(AttributeConfig::OpenWithUgene)) + Constants::SEMICOLON + Constants::NEW_LINE;
            }
        }
        res += Constants::TAB + Constants::TAB + "}\n";
    }
    res += Constants::TAB + "}\n";
    return res;
}

QString HRSchemaSerializer::actor2String(ExternalProcessConfig* cfg) {
    QString res = Constants::HEADER_LINE + "\n";
    res += "\"" + cfg->id + "\" {\n";
    res += inputsDefenition(cfg->inputs);
    res += outputsDefenition(cfg->outputs);
    res += attributesDefinition(cfg->attrs);
    res += Constants::TAB + Constants::NAME_ATTR + ":\"" + cfg->name + "\";\n";
    res += Constants::TAB + Constants::USE_INTEGRATED_TOOL + ":" + (cfg->useIntegratedTool ? Constants::TRUE : Constants::FALSE) + ";\n";
    if (!cfg->customToolPath.isEmpty()) {
        res += Constants::TAB + Constants::CUSTOM_TOOL_PATH + ":\"" + cfg->customToolPath + "\";\n";  // TODO: it should be escaped (UGENE-6437)
    }
    if (!cfg->integratedToolId.isEmpty()) {
        res += Constants::TAB + Constants::INTEGRATED_TOOL_ID + ":\"" + cfg->integratedToolId + "\";\n";  // TODO: it also should be escaped (UGENE-6437)
    }
    res += Constants::TAB + Constants::CMDLINE + ":\"" + cfg->cmdLine + "\";\n";
    if (!cfg->description.isEmpty()) {
        res += Constants::TAB + Constants::DESCRIPTION + ":\"" + cfg->description + "\";\n";
    }
    if (!cfg->templateDescription.isEmpty()) {
        res += Constants::TAB + Constants::PROMPTER + ":\"" + cfg->templateDescription + "\";\n";
    }
    res += "}";
    return res;
}

Actor* HRSchemaSerializer::deprecatedActorsReplacer(const QString& id, const QString& protoId, ParsedPairs& pairs) {
    Actor* a = nullptr;
    ActorPrototype* apt = nullptr;
    if (protoId == CoreLibConstants::WRITE_CLUSTAL_PROTO_ID) {
        apt = WorkflowEnv::getProtoRegistry()->getProto(SchemaSerializer::getElemType(CoreLibConstants::WRITE_MSA_PROTO_ID));
        a = apt->createInstance(id);
        a->setParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), BaseDocumentFormats::CLUSTAL_ALN);
        pairs.blockPairs.remove("accumulate");
        pairs.equalPairs.remove("accumulate");
    }
    if (protoId == CoreLibConstants::WRITE_STOCKHOLM_PROTO_ID) {
        apt = WorkflowEnv::getProtoRegistry()->getProto(SchemaSerializer::getElemType(CoreLibConstants::WRITE_MSA_PROTO_ID));
        a = apt->createInstance(id);
        a->setParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), BaseDocumentFormats::STOCKHOLM);
        pairs.blockPairs.remove("accumulate");
        pairs.equalPairs.remove("accumulate");
    }
    if (protoId == CoreLibConstants::WRITE_FASTQ_PROTO_ID) {
        apt = WorkflowEnv::getProtoRegistry()->getProto(SchemaSerializer::getElemType(CoreLibConstants::WRITE_SEQ_PROTO_ID));
        a = apt->createInstance(id);
        a->setParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), BaseDocumentFormats::FASTQ);
    }
    if (protoId == CoreLibConstants::WRITE_GENBANK_PROTO_ID) {
        apt = WorkflowEnv::getProtoRegistry()->getProto(SchemaSerializer::getElemType(CoreLibConstants::WRITE_SEQ_PROTO_ID));
        a = apt->createInstance(id);
        a->setParameter(BaseAttributes::DOCUMENT_FORMAT_ATTRIBUTE().getId(), BaseDocumentFormats::PLAIN_GENBANK);
    }
    return a;
}

void HRSchemaSerializer::parseMarkers(Actor* proc, const QStringList& markerDefs, const QString& attrId) {
    auto attr = dynamic_cast<MarkerAttribute*>(proc->getParameter(attrId));
    if (attr == nullptr) {
        throw ReadFailed(tr("%1 actor has not marker attribute").arg(proc->getId()));
    }

    SAFE_POINT(1 == proc->getEnabledOutputPorts().size(), "Wrong out ports count", );
    Port* outPort = proc->getEnabledOutputPorts().first();
    QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();

    foreach (const QString& def, markerDefs) {
        Marker* marker = parseMarker(def);
        SAFE_POINT_EXT(marker != nullptr, throw ReadFailed("NULL marker"), );

        Descriptor newSlot = MarkerSlots::getSlotByMarkerType(marker->getType(), marker->getName());
        outTypeMap[newSlot] = BaseTypes::STRING_TYPE();
        attr->getMarkers() << marker;
    }

    DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
    outPort->setNewType(newType);
}

Marker* HRSchemaSerializer::parseMarker(ParsedPairs& pairs, const QString& MARKER_TYPE, const QString& MARKER_NAME) {
    const QString markerType = pairs.equalPairs.take(MARKER_TYPE);
    const QString markerName = pairs.equalPairs.take(MARKER_NAME);
    if (markerName.isEmpty()) {
        throw ReadFailed(tr("Name attribute is not set for the marker"));
    }
    if (markerType.isEmpty()) {
        throw ReadFailed(tr("Type attribute is not set for %1 marker").arg(markerName));
    }

    Marker* marker = nullptr;
    if (markerType == MarkerTypes::QUAL_INT_VALUE_MARKER_ID || markerType == MarkerTypes::QUAL_TEXT_VALUE_MARKER_ID || markerType == MarkerTypes::QUAL_FLOAT_VALUE_MARKER_ID) {
        const QString qualName = pairs.equalPairs.take(Constants::QUAL_NAME);
        if (qualName.isEmpty()) {
            throw ReadFailed(tr("Qualifier name attribute is not set for %1 marker").arg(markerName));
        }
        marker = new QualifierMarker(markerType, markerName, qualName);
    } else if (MarkerTypes::ANNOTATION_LENGTH_MARKER_ID == markerType || MarkerTypes::ANNOTATION_COUNT_MARKER_ID == markerType) {
        QString annName = pairs.equalPairs.take(Constants::ANN_NAME);
        marker = new AnnotationMarker(markerType, markerName, annName);
    } else if (MarkerTypes::TEXT_MARKER_ID == markerType) {
        marker = new TextMarker(markerType, markerName);
    } else {
        marker = new SequenceMarker(markerType, markerName);
    }
    foreach (const QString& key, pairs.equalPairs.keys()) {
        marker->addValue(key, pairs.equalPairs.value(key));
    }

    return marker;
}

Marker* HRSchemaSerializer::parseMarker(const QString& def) {
    ParsedPairs pairs(def);
    return parseMarker(pairs, Constants::TYPE_ATTR, Constants::NAME_ATTR);
}

QString HRSchemaSerializer::markersDefinition(Attribute* attribute) {
    auto mAttr = dynamic_cast<MarkerAttribute*>(attribute);
    SAFE_POINT(mAttr != nullptr, "NULL marker attribute", "");
    QString res;

    foreach (Marker* marker, mAttr->getMarkers()) {
        res += makeBlock(attribute->getId(), Constants::NO_NAME, markerDefinitionBlock(marker, 3), 2);
    }
    return res + Constants::NEW_LINE;
}

void HRSchemaSerializer::updateWorkflowSchemaPathSettings(const Metadata& meta) {
    Settings* settings = AppContext::getSettings();
    assert(settings != nullptr);
    QVariantMap pathsMap = settings->getValue(SCHEMA_PATHS_SETTINGS_TAG).toMap();
    pathsMap.insert(meta.name, meta.url);
    settings->setValue(SCHEMA_PATHS_SETTINGS_TAG, pathsMap);
}

}  // namespace U2
