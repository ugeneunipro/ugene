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

#include <QListWidgetItem>

#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/IntegralBusType.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowIOTasks.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/IntegralBus.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/URLAttribute.h>
#include <U2Lang/WorkflowSettings.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CredentialsAsker.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Folder.h>
#include <U2Core/GObject.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/Settings.h>
#include <U2Core/StringAdapter.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "WorkflowUtils.h"

namespace U2 {
/*****************************
 * WorkflowUtils
 *****************************/
const QStringList WorkflowUtils::WD_FILE_EXTENSIONS = initExtensions();
const QString WorkflowUtils::WD_XML_FORMAT_EXTENSION("uws");
const QString WorkflowUtils::HREF_PARAM_ID("param");

QStringList WorkflowUtils::initExtensions() {
    QStringList exts;
    exts << "uwl";
    return exts;
}

QString WorkflowUtils::getRichDoc(const Descriptor& d) {
    QString result = QString();
    if(d.getDisplayName().isEmpty()) {
        if(!d.getDocumentation().isEmpty()) {
            result = QString("%1").arg(d.getDocumentation());
        }
    } else {
        if(d.getDocumentation().isEmpty()) {
            result = QString("<b>%1</b>").arg(d.getDisplayName());
        } else {
            result = QString("<b>%1</b>: %2").arg(d.getDisplayName()).arg(d.getDocumentation());
        }
    }
    result.replace("\n", "<br>");
    return result;
}

QString WorkflowUtils::getDropUrl(QList<DocumentFormat*>& fs, const QMimeData* md) {
    QString url;
    const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
    const DocumentMimeData* domd = qobject_cast<const DocumentMimeData*>(md);
    if (gomd) {
        GObject* obj = gomd->objPtr.data();
        if (obj) {
            fs << obj->getDocument()->getDocumentFormat();
            url = obj->getDocument()->getURLString();
        }
    } else if (domd) {
        Document* doc = domd->objPtr.data();
        if (doc) {
            fs << doc->getDocumentFormat();
            url = doc->getURLString();
        }
    } else if (md->hasUrls()) {
        QList<QUrl> urls = md->urls();
        if (urls.size() == 1) {
            url = urls.first().toLocalFile();
            QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url);
            foreach(const FormatDetectionResult& di, formats) {
                fs << di.format;
            }
        }
    }
    return url;
}

void WorkflowUtils::setQObjectProperties(QObject &o , const QVariantMap & params) {
    QMapIterator<QString, QVariant> i(params);
    while (i.hasNext()) {
        i.next();
        //log.debug("set param " + i.key() + "="+i.value().toString());
        o.setProperty(i.key().toLatin1(), i.value());
    }
}

QStringList WorkflowUtils::expandToUrls(const QString& s) {
    QStringList urls = s.split(";");
    QStringList result;
    QRegExp wcard("[*?\\[\\]]");
    foreach(QString url, urls)
    {
        int idx = url.indexOf(wcard);
        if (idx >= 0) {
            int dirIdx = url.lastIndexOf('/', idx);
            QDir dir;
            if (dirIdx >= 0) {
                dir = QDir(url.left(dirIdx));
                url = url.right(url.length() - dirIdx - 1);
            }

            foreach(QFileInfo fi, dir.entryInfoList((QStringList() << url), QDir::Files|QDir::NoSymLinks)) {
                result << fi.absoluteFilePath();
            }
        } else {
            //if (QFile::exists(url))
            {
                result << url;
            }
        }
    }
    return result;
}

namespace {

bool validateParameters(const Schema &schema, NotificationsList &infoList) {
    bool good = true;
    foreach (Actor* a, schema.getProcesses()) {
        const int notificationCountBefore = infoList.size();
        good &= a->validate(infoList);
        for (int i = notificationCountBefore; i < infoList.size(); ++i) {
            infoList[i].actorId = a->getId();
        }
    }
    return good;
}

bool validateExternalTools(Actor *a, NotificationsList &infoList) {
    bool good = true;
    StrStrMap tools = a->getProto()->getExternalTools();
    foreach (const QString &toolId, tools.keys()) {
        Attribute *attr = a->getParameter(tools[toolId]);
        ExternalTool *tool = AppContext::getExternalToolRegistry()->getById(toolId);
        if (nullptr == tool) {
            good = false;
            infoList << WorkflowNotification(WorkflowUtils::externalToolIsAbsentError(toolId),
                                             a->getId(),
                                             WorkflowNotification::U2_ERROR);
            continue;
        }

        bool fromAttr = (NULL != attr) && !attr->isDefaultValue();
        bool valid = fromAttr ? !attr->isEmpty() : !tool->getPath().isEmpty();
        if (!valid) {
            good = false;
            infoList << WorkflowNotification(WorkflowUtils::externalToolError(tool->getName()),
                                a->getId(),
                                WorkflowNotification::U2_ERROR);
        } else if (!fromAttr && !tool->isValid()) {
            if (tool->isCustom()) {
                infoList << WorkflowNotification(WorkflowUtils::customExternalToolInvalidError(tool->getName(), a->getLabel()),
                    a->getProto()->getId(),
                    WorkflowNotification::U2_ERROR);
                good = false;
            } else {
                infoList << WorkflowNotification(WorkflowUtils::externalToolInvalidError(tool->getName()),
                    a->getProto()->getId(),
                    WorkflowNotification::U2_WARNING);
            }
        }
    }
    return good;
}

bool validatePorts(Actor *a, NotificationsList &infoList) {
    bool good = true;
    foreach(Port *p, a->getEnabledPorts()) {
        NotificationsList notificationList;
        good &= p->validate(notificationList);
        if (!notificationList.isEmpty()) {
            foreach(WorkflowNotification notification, notificationList) {
                WorkflowNotification item;
                item.message = notification.message;
                item.port = p->getId();
                item.actorId = a->getId();
                item.type = notification.type;
                infoList << item;
            }
        }
    }
    return good;
}

bool graphDepthFirstSearch(Actor *vertex, QList<Actor *> &visitedVertices) {
    visitedVertices.append(vertex);
    const QList<Port *> outputPorts = vertex->getOutputPorts();
    QList<Actor *> receivingVertices;
    foreach (Port *outputPort, outputPorts) {
        foreach (Port *receivingPort, outputPort->getLinks().keys()) {
            receivingVertices.append(receivingPort->owner());
        }
    }
    foreach (Actor *receivingVertex, receivingVertices) {
        if (visitedVertices.contains(receivingVertex)) {
            return false;
        } else {
            return graphDepthFirstSearch(receivingVertex, visitedVertices);
        }
    }
    return true;
}

// the returning values signals about cycles existence in the scheme
bool hasSchemeCycles(const Schema &scheme) {
    foreach (Actor *vertex, scheme.getProcesses()) {
        QList<Actor *> visitedVertices;
        if (!graphDepthFirstSearch(vertex, visitedVertices)) {
            return false;
        }
    }
    return true;
}

bool validateScript(Actor *a, NotificationsList &infoList) {
    SAFE_POINT(NULL != a, "NULL actor", false);
    SAFE_POINT(NULL != a->getScript(), "NULL script", false);
    const QString scriptText = a->getScript()->getScriptText();
    if (scriptText.simplified().isEmpty()) {
        infoList << WorkflowNotification(QObject::tr("Empty script text"), a->getId());
        return false;
    }
    QScopedPointer<WorkflowScriptEngine> engine(new WorkflowScriptEngine(NULL));
    QScriptSyntaxCheckResult syntaxResult = engine->checkSyntax(scriptText);

    if (syntaxResult.state() != QScriptSyntaxCheckResult::Valid) {
        WorkflowNotification notification;
        notification.message = QObject::tr("Script syntax check failed! Line: %1, error: %2")
            .arg(syntaxResult.errorLineNumber())
            .arg(syntaxResult.errorMessage());
        notification.actorId = a->getId();
        notification.type = WorkflowNotification::U2_ERROR;
        infoList << notification;
        return false;
    }
    return true;
}

}

bool WorkflowUtils::validate(const Schema &schema, NotificationsList &notificationList) {
    bool good = validateOutputDir(WorkflowSettings::getWorkflowOutputDirectory(), notificationList);
    foreach (Actor *a, schema.getProcesses()) {
        good &= validatePorts(a, notificationList);
        if (a->getProto()->isScriptFlagSet()) {
            good &= validateScript(a, notificationList);
        }
        good &= validateExternalTools(a, notificationList);
    }
    if (!hasSchemeCycles(schema)) {
        good = false;
        notificationList << WorkflowNotification(QObject::tr("The workflow contains a cycle"));
    }

    good &= validateParameters(schema, notificationList);

    return good;
}

// used in GUI schema validating
bool WorkflowUtils::validate(const Schema &schema, QList<QListWidgetItem*> &infoList) {
    NotificationsList notifications;
    bool good = validate(schema, notifications);

    foreach (const WorkflowNotification &notification, notifications) {
        QListWidgetItem *item = nullptr;
        Actor *a = nullptr;
        if (notification.actorId.isEmpty()) {
            item = new QListWidgetItem(notification.message);
        } else {
            a = schema.actorById(notification.actorId);
            item = new QListWidgetItem(QString("%1: %2").arg(a->getLabel()).arg(notification.message));
        }
        if (notification.type == WorkflowNotification::U2_ERROR) {
            item->setIcon(QIcon(":U2Lang/images/error.png"));
        } else if (notification.type == WorkflowNotification::U2_WARNING) {
            item->setIcon(QIcon(":U2Lang/images/warning.png"));
        } else if (a != nullptr){
            item->setIcon(a->getProto()->getIcon());
        }

        item->setData(ACTOR_ID_REF, notification.actorId);
        item->setData(PORT_REF, notification.port);
        item->setData(TEXT_REF, notification.message);
        item->setData(TYPE_REF, notification.type);

        infoList << item;
    }

    return good;
}

// used in cmdline schema validating
bool WorkflowUtils::validate(const Workflow::Schema &schema, QStringList &errs) {
    NotificationsList notifications;
    bool good = validate(schema, notifications);

    foreach (const WorkflowNotification &notification, notifications) {
        QString res = QString();
        if (notification.actorId.isEmpty()) {
            res = notification.message;
        } else {
            Actor *a = schema.actorById(notification.actorId);
            QString message = notification.message;
            res = QString("%1: %2").arg(a->getLabel()).arg(message);

            QString option;
            foreach (const Attribute *attr, a->getAttributes()) {
                if (message.contains(attr->getDisplayName())) {
                    option = a->getParamAliases().value(attr->getId());
                }
            }
            if (!option.isEmpty()) {
                res += tr(" (use --%1 option)").arg(option);
            }
        }
        errs << res;
    }

    return good;
}

QList<Descriptor> WorkflowUtils::findMatchingTypes(DataTypePtr set, DataTypePtr elementDataType) {
    QList<Descriptor> result;
    foreach(const Descriptor& d, set->getAllDescriptors()) {
        if (set->getDatatypeByDescriptor(d) == elementDataType) {
            result.append(d);
        }
    }
    return result;
}

QStringList WorkflowUtils::candidatesAsStringList(const QList<Descriptor> &descList) {
    QStringList res;
    foreach (const Descriptor &desc, descList) {
        res << desc.getId();
    }
    return res;
}

QStringList WorkflowUtils::findMatchingTypesAsStringList(DataTypePtr set, DataTypePtr elementDatatype) {
    QList<Descriptor> descList = findMatchingTypes(set, elementDatatype);
    return candidatesAsStringList(descList);
}

Descriptor newEmptyValuesDesc() {
    return Descriptor("", QObject::tr("<empty>"), QObject::tr("Default value"));
}

QList<Descriptor> WorkflowUtils::findMatchingCandidates(DataTypePtr from, DataTypePtr elementDatatype) {
    QList<Descriptor> candidates = findMatchingTypes(from, elementDatatype);
    if (elementDatatype->isList()) {
        candidates += findMatchingTypes(from, elementDatatype->getDatatypeByDescriptor());
    } else {
        candidates.append(newEmptyValuesDesc());
    }
    return candidates;
}

QList<Descriptor> WorkflowUtils::findMatchingCandidates(DataTypePtr from, DataTypePtr to, const Descriptor & key) {
    return findMatchingCandidates(from, to->getDatatypeByDescriptor(key));
}

Descriptor WorkflowUtils::getCurrentMatchingDescriptor(const QList<Descriptor> & candidates, DataTypePtr to,
                                                       const Descriptor & key, const StrStrMap & bindings) {
    DataTypePtr elementDatatype = to->getDatatypeByDescriptor(key);
    if (elementDatatype->isList()) {
        QString currentVal = bindings.value(key.getId());
        if (!currentVal.isEmpty()) {
            return Descriptor(currentVal, tr("<List of values>"), tr("List of values"));
        } else {
            return newEmptyValuesDesc();
        }
    } else {
        int idx = bindings.contains(key.getId()) ? candidates.indexOf(bindings.value(key.getId())) : 0;
        return idx >= 0 ? candidates.at(idx) : newEmptyValuesDesc();
    }
}

DataTypePtr WorkflowUtils::getToDatatypeForBusport(IntegralBusPort * p) {
    assert(p != NULL);
    DataTypePtr to;
    DataTypePtr t = to = p->getType();
    if (!t->isMap()) {
        QMap<Descriptor, DataTypePtr> map;
        map.insert(*p, t);
        to = new MapDataType(Descriptor(), map);
        //IntegralBusType* bt = new IntegralBusType(Descriptor(), QMap<Descriptor, DataTypePtr>());
        //bt->addOutput(t, p);
    }
    return to;
}

DataTypePtr WorkflowUtils::getFromDatatypeForBusport(IntegralBusPort * p, DataTypePtr to) {
    assert(p != NULL);

    DataTypePtr from;
    if (p->isOutput() || p->getWidth() == 0) {
        //nothing to edit, go info mode
        from = to;
    } else {
        //port is input and has links, go editing mode
        IntegralBusType* bt = new IntegralBusType(Descriptor(), QMap<Descriptor, DataTypePtr>());
        bt->addInputs(p, false);
        from = bt;
    }
    return from;
}

QString WorkflowUtils::findPathToSchemaFile(const QString & name) {
    // full path given
    if(QFile::exists(name)) {
        return name;
    }
    // search schema in data dir
    QString filenameWithDataPrefix = QString(PATH_PREFIX_DATA) + ":" + "cmdline/" + name;
    if(QFile::exists(filenameWithDataPrefix)) {
        return filenameWithDataPrefix;
    }
    foreach(const QString & ext, WorkflowUtils::WD_FILE_EXTENSIONS) {
        QString filenameWithDataPrefixAndExt = QString(PATH_PREFIX_DATA) + ":" + "cmdline/" + name + "." + ext;
        if(QFile::exists(filenameWithDataPrefixAndExt)) {
            return filenameWithDataPrefixAndExt;
        }
    }

    // if no such file found -> search name in settings. user saved schemas
    Settings * settings = AppContext::getSettings();
    assert(settings != NULL);

    // FIXME: same as WorkflowSceneIOTasks::SCHEMA_PATHS_SETTINGS_TAG
    QVariantMap pathsMap = settings->getValue("workflow_settings/schema_paths").toMap();
    QString path = pathsMap.value(name).toString();
    if(QFile::exists(path)) {
        return path;
    }
    return QString();
}

void WorkflowUtils::getLinkedActorsId(Actor *a, QList<QString> &linkedActors) {
    if(!linkedActors.contains(a->getId())) {
        linkedActors.append(a->getId());
        foreach(Port *p, a->getPorts()) {
            foreach(Port *pp, p->getLinks().keys()) {
                getLinkedActorsId(pp->owner(), linkedActors);
            }
        }
    } else {
        return;
    }
}

bool WorkflowUtils::isPathExist(const Port *src, const Port *dest) {
    SAFE_POINT((src->isInput() ^ dest->isInput()), "The ports have the same direction",
        true);
    if (!src->isOutput() && !dest->isInput()) {
        const Port *tmp = dest;
        dest = src;
        src = tmp;
    }
    const Actor *destElement = dest->owner();

    foreach (const Port *port, src->owner()->getPorts()) {
        if (src == port) {
            continue;
        }
        foreach (const Port *p, port->getLinks().keys()) {
            if (destElement == p->owner()) {
                return true;
            }
            if (isPathExist(p, dest)) {
                return true;
            }
        }
    }
    return false;
}

Descriptor WorkflowUtils::getSlotDescOfDatatype(const DataTypePtr & dt) {
    QString dtId = dt->getId();
    if(dtId == BaseTypes::DNA_SEQUENCE_TYPE()->getId()) {
        return BaseSlots::DNA_SEQUENCE_SLOT();
    }
    if(dtId == BaseTypes::ANNOTATION_TABLE_TYPE()->getId()) {
        return BaseSlots::ANNOTATION_TABLE_SLOT();
    }
    if(dtId == BaseTypes::MULTIPLE_ALIGNMENT_TYPE()->getId()) {
        return BaseSlots::MULTIPLE_ALIGNMENT_SLOT();
    }
    if(dtId == BaseTypes::STRING_TYPE()->getId()) {
        return BaseSlots::TEXT_SLOT();
    }
    SAFE_POINT(false, "Unexpected slot type", Descriptor());
    return Descriptor();
}

static QStringList initLowerToUpperList() {
    QStringList res;
    res << "true";
    res << "false";
    return res;
}
static const QStringList lowerToUpperList = initLowerToUpperList();

QString WorkflowUtils::getStringForParameterDisplayRole(const QVariant & value) {
    if (value.canConvert< QList<Dataset> >()) {
        QString res;
        foreach (const Dataset &dSet, value.value< QList<Dataset> >()) {
            res += dSet.getName() + "; ";
        }
        return res;
    }
    QString str = value.toString();
    if(lowerToUpperList.contains(str)) {
        return str.at(0).toUpper() + str.mid(1);
    }
    return str;
}

Actor * WorkflowUtils::findActorByParamAlias(const QList<Actor*> & procs, const QString & alias, QString & attrName, bool writeLog) {
    QList<Actor*> actors;
    foreach(Actor * actor, procs) {
        assert(actor != NULL);
        if(actor->getParamAliases().values().contains(alias)) {
            actors << actor;
        }
    }

    if (actors.isEmpty()) {
        return NULL;
    } else if(actors.size() > 1) {
        if(writeLog) {
            coreLog.error(WorkflowUtils::tr("%1 actors in workflow have '%2' alias").arg(actors.size()).arg(alias));
        }
    }

    Actor * ret = actors.first();
    attrName = ret->getParamAliases().key(alias);
    return ret;
}

QString WorkflowUtils::getParamIdFromHref(const QString& href) {
    QStringList args = href.split('&');
    const QString& prefix = QString("%1:").arg(HREF_PARAM_ID);
    QString id;
    foreach(QString arg, args) {
        if (arg.startsWith(prefix)) {
            id = arg.mid(prefix.length());
            break;
        }
    }
    return id;
}

QString WorkflowUtils::generateIdFromName(const QString &name) {
    QString id = name;
    id.replace(QRegularExpression("\\s"), "-").replace(WorkflowEntityValidator::INACCEPTABLE_SYMBOLS_IN_ID, "_");
    return id;
}

static void data2text(WorkflowContext *context, DocumentFormatId formatId, GObject *obj, QString &text) {
    QList<GObject*> objList;
    objList << obj;

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::STRING);
    DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(formatId);
    QScopedPointer<Document> d(new Document(df, iof, GUrl(), context->getDataStorage()->getDbiRef(), objList));
    d->setDocumentOwnsDbiResources(false);
    StringAdapter *io = dynamic_cast<StringAdapter*>(iof->createIOAdapter());
    io->open(GUrl(), IOAdapterMode_Write);
    U2OpStatusImpl os;

    df->storeDocument(d.data(), io, os);

    text += io->getBuffer();
    io->close();
}

#define STRING_TYPE QVariant::String
#define SEQUENCE_TYPE QVariant::ByteArray
#define MSA_TYPE QVariant::UserType
#define ANNOTATIONS_TYPE QVariant::List

void WorkflowUtils::print(const QString &slotString, const QVariant &data, DataTypePtr type, WorkflowContext *context) {
    QString text = slotString + ":\n";
    Workflow::DbiDataStorage *storage = context->getDataStorage();
    if ("string" == type->getId() || BaseTypes::STRING_LIST_TYPE() == type) {
        text += data.toString();
    } else if (BaseTypes::DNA_SEQUENCE_TYPE() == type) {
        QScopedPointer<U2SequenceObject> obj(StorageUtils::getSequenceObject(storage, data.value<SharedDbiDataHandler>()));
        CHECK(NULL != obj.data(),);
        data2text(context, BaseDocumentFormats::FASTA, obj.data(), text);
    } else if (BaseTypes::MULTIPLE_ALIGNMENT_TYPE() == type) {
        QScopedPointer<MultipleSequenceAlignmentObject> obj(StorageUtils::getMsaObject(storage, data.value<SharedDbiDataHandler>()));
        CHECK(NULL != obj.data(),);
        data2text(context, BaseDocumentFormats::CLUSTAL_ALN, obj.data(), text);
    } else if (BaseTypes::ANNOTATION_TABLE_TYPE() == type || BaseTypes::ANNOTATION_TABLE_LIST_TYPE() == type) {
        QList<SharedAnnotationData> annotationList = StorageUtils::getAnnotationTable(storage, data);
        AnnotationTableObject obj("Annotations", storage->getDbiRef());
        obj.addAnnotations(annotationList);
        data2text(context, BaseDocumentFormats::PLAIN_GENBANK, &obj, text);
    } else {
        text += "Can not print data of this type: " + type->getDisplayName();
    }
    printf("\n%s\n", text.toLatin1().data());
}

bool WorkflowUtils::validateSchemaForIncluding(const Schema &s, QString &error) {
    // TEMPORARY disallow filter and grouper elements in includes
    static QString errorStr = tr("The %1 element is a %2. Sorry, but current version of "
        "UGENE doesn't support of filters and groupers in the includes.");
    foreach (Actor *actor, s.getProcesses()) {
        ActorPrototype *proto = actor->getProto();
        if (proto->getInfluenceOnPathFlag() || CoreLibConstants::GROUPER_ID == proto->getId()) {
            error = errorStr;
            error = error.arg(actor->getLabel());
            if (proto->getInfluenceOnPathFlag()) {
                error = error.arg(tr("filter"));
            } else {
                error = error.arg(tr("grouper"));
            }
            return false;
        }
    }

    const QList<PortAlias> &portAliases = s.getPortAliases();
    if (portAliases.isEmpty()) {
        error = tr("The workflow has not any aliased ports");
        return false;
    }

    foreach (Actor *actor, s.getProcesses()) {
        // check that free input ports are aliased
        foreach (Port *port, actor->getPorts()) {
            if (!port->isInput()) {
                continue;
            }
            if (!port->getLinks().isEmpty()) {
                continue;
            }
            bool aliased = false;
            foreach (const PortAlias &alias, portAliases) {
                if (alias.getSourcePort() == port) {
                    if (alias.getSlotAliases().isEmpty()) {
                        error = tr("The aliased port %1.%2 has no aliased slots").arg(actor->getLabel()).arg(port->getDisplayName());
                        return false;
                    } else {
                        aliased = true;
                        break;
                    }
                }
            }
            if (!aliased) {
                error = tr("The free port %1.%2 is not aliased").arg(actor->getLabel()).arg(port->getId());
                return false;
            }
        }

        // check that every required attribute is aliased or has set value
        const QMap<QString, QString> &paramAliases = actor->getParamAliases();
        foreach (const QString &attrName, actor->getParameters().keys()) {
            Attribute *attr = actor->getParameters().value(attrName);
            if (attr->isRequiredAttribute() && !attr->canBeEmpty()) {
                if (!paramAliases.contains(attr->getId())) {
                    QVariant val = attr->getAttributeValueWithoutScript<QVariant>();
                    if (val.isNull()) {
                        error = tr("The required parameter %1.%2 is empty and not aliased").arg(actor->getLabel()).arg(attr->getDisplayName());
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

void WorkflowUtils::extractPathsFromBindings(StrStrMap &busMap, SlotPathMap &pathMap) {
    QString srcId;
    QStringList path;
    foreach (const QString &dest, busMap.keys()) {
        QStringList srcs = busMap.value(dest).split(";");
        foreach (const QString &src, srcs) {
            BusMap::parseSource(src, srcId, path);
            if (!path.isEmpty()) {
                QPair<QString, QString> slotPair(dest, srcId);
                busMap[dest] = srcId;
                pathMap.insertMulti(slotPair, path);
            }
        }
    }
}

void WorkflowUtils::applyPathsToBusMap(StrStrMap &busMap, const SlotPathMap &pathMap) {
    foreach (const QString &dest, busMap.keys()) {
        QStringList newSrcs;

        QStringList srcs = busMap.value(dest).split(";");
        QStringList uniqList;
        foreach(QString src, srcs) {
            if (!uniqList.contains(src)) {
                uniqList << src;
            }
        }

        foreach (const QString &src, uniqList) {
            QPair<QString, QString> slotPair(dest, src);
            if (pathMap.contains(slotPair)) {
                QList<QStringList> paths = pathMap.values(slotPair);
                if (!paths.isEmpty()) {
                    foreach (const QStringList &path, paths) {
                        QString newSrc = src + ">" + path.join(",");
                        newSrcs << newSrc;
                    }
                }
            } else {
                newSrcs << src;
            }
        }
        busMap[dest] = newSrcs.join(";");
    }
}

bool WorkflowUtils::startExternalProcess(QProcess *process, const QString &program, const QStringList &arguments) {
    return ExternalToolSupportUtils::startExternalProcess(process, program, arguments);
}

QStringList WorkflowUtils::getDatasetsUrls(const QList<Dataset> &sets) {
    QStringList result;
    foreach (const Dataset &dSet, sets) {
        foreach (URLContainer *url, dSet.getUrls()) {
            result << url->getUrl();
        }
    }
    return result;
}

QStringList WorkflowUtils::getAttributeUrls(Attribute *attribute){
    QStringList urlList;
    QVariant var = attribute->getAttributePureValue();
    if(var.canConvert<QList<Dataset> >()){
        urlList = WorkflowUtils::getDatasetsUrls(var.value<QList<Dataset> >());
    }
    else if (var.canConvert(QVariant::String)) {
        urlList = var.toString().split(";");
    }
    return urlList;
}

Actor * WorkflowUtils::actorById(const QList<Actor*> &actors, const ActorId &id) {
    foreach(Actor *a, actors) {
        if (a->getId() == id) {
            return a;
        }
    }
    return NULL;
}

QMap<Descriptor, DataTypePtr> WorkflowUtils::getBusType(Port *inPort) {
    QMap<Port*,Link*> links = inPort->getLinks();
    if (links.size() == 1) {
        Port *src = links.keys().first();
        assert(src->isOutput());
        IntegralBusPort *bus = dynamic_cast<IntegralBusPort*>(src);
        assert(NULL != bus);
        DataTypePtr type = bus->getType();
        return type->getDatatypesMap();
    }
    return QMap<Descriptor, DataTypePtr>();
}

bool WorkflowUtils::isBindingValid(const QString &srcSlotId, const QMap<Descriptor, DataTypePtr> &srcBus,
    const QString &dstSlotId, const QMap<Descriptor, DataTypePtr> &dstBus) {
    DataTypePtr srcType;
    // Check that incoming bus contains source slot
    bool found = false;
    foreach (const Descriptor &d,  srcBus.keys()) {
        if (d.getId() == srcSlotId) {
            srcType = srcBus.value(d);
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    // Check that source and destination slots have equal types
    foreach (const Descriptor &d, dstBus.keys()) {
        if (d.getId() == dstSlotId) {
            DataTypePtr destType = dstBus.value(d);
            QString stringTypeId("string");
            if (destType == srcType) {
                return true;
            } else if (destType == BaseTypes::ANNOTATION_TABLE_TYPE()) {
                return (srcType == BaseTypes::ANNOTATION_TABLE_LIST_TYPE());
            } else if (destType == BaseTypes::ANNOTATION_TABLE_LIST_TYPE()) {
                return (srcType == BaseTypes::ANNOTATION_TABLE_TYPE());
            } else if (destType->getId() == stringTypeId) {
                return (srcType == BaseTypes::STRING_LIST_TYPE());
            } else if (destType == BaseTypes::STRING_LIST_TYPE()) {
                return (srcType->getId() == stringTypeId);
            }
            break;
        }
    }

    return false;
}

QString WorkflowUtils::createUniqueString(const QString &str, const QString &sep, const QStringList &uniqueStrs) {
    QString result = str;
    int number = 0;
    bool found = false;
    foreach (const QString &uniq, uniqueStrs) {
        if (uniq == str) {
            found = true;
            number = qMax(number, 1);
        } else {
            int idx = uniq.lastIndexOf(sep);
            if (-1 != idx) {
                QString left = uniq.left(idx);
                if (str == left) {
                    QString right = uniq.mid(idx + 1);
                    bool ok = false;
                    int num = right.toInt(&ok);
                    if(ok) {
                        found = true;
                        number = qMax(number, num + 1);
                    }
                }
            }
        }
    }

    if (found) {
        result += sep + QString::number(number);
    }
    return result;
}

QString WorkflowUtils::updateExternalToolPath(const QString &id, const QString &path) {
    ExternalToolRegistry *registry = AppContext::getExternalToolRegistry();
    SAFE_POINT(NULL != registry, "NULL external tool registry", "");
    ExternalTool *tool = registry->getById(id);
    SAFE_POINT(NULL != tool, QString("Unknown tool: %1").arg(id), "");

    if (QString::compare(path, "default", Qt::CaseInsensitive) != 0) {
        tool->setPath(path);
    }
    return tool->getPath();
}

QString WorkflowUtils::getExternalToolPath(const QString &toolId) {
    ExternalToolRegistry *registry = AppContext::getExternalToolRegistry();
    SAFE_POINT(NULL != registry, "NULL external tool registry", "");

    ExternalTool *tool = registry->getById(toolId);
    SAFE_POINT(NULL != tool, QString("Unknown tool (id): %1").arg(toolId), "");

    return tool->getPath();
}

QString WorkflowUtils::externalToolIsAbsentError(const QString& toolName) {
    return tr("Specified variable \"%%1%\" does not exist, please check the command again.").arg(toolName);
}

QString WorkflowUtils::externalToolError(const QString &toolName) {
    return tr("External tool \"%1\" is not set. You can set it in Settings -> Preferences -> External Tools").arg(toolName);
}

QString WorkflowUtils::externalToolInvalidError(const QString &toolName) {
    return tr("External tool \"%1\" is invalid. UGENE may not support this version of the tool or a wrong path to the tools is selected").arg(toolName);
}

QString WorkflowUtils::customExternalToolInvalidError(const QString& toolName, const QString& elementName) {
    return tr("Custom tool \"%1\", specified for the \"%2\" element, didn't pass validation.").arg(toolName).arg(elementName);
}

void WorkflowUtils::schemaFromFile(const QString &url, Schema *schema, Metadata *meta, U2OpStatus &os) {
    QFile file(url);
    if(!file.open(QIODevice::ReadOnly)) {
        os.setError(L10N::errorOpeningFileRead(url));
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString rawData = in.readAll();
    file.close();

    QString error = HRSchemaSerializer::string2Schema(rawData, schema, meta);
    if (!error.isEmpty()) {
        os.setError(error);
    }
}

static bool isDatasetsAttr(Attribute *attr) {
    URLAttribute *dsa = dynamic_cast<URLAttribute*>(attr);
    return (NULL != dsa);
}

UrlAttributeType WorkflowUtils::isUrlAttribute(Attribute *attr, const Actor *actor) {
    SAFE_POINT(NULL != attr, "NULL attribute!", NotAnUrl);
    SAFE_POINT(NULL != actor, "NULL actor!", NotAnUrl);

    if (isDatasetsAttr(attr)) {
        return DatasetAttr;
    }

    ConfigurationEditor *editor = actor->getEditor();
    CHECK(NULL != editor, NotAnUrl);
    PropertyDelegate *delegate = editor->getDelegate(attr->getId());
    CHECK(NULL != delegate, NotAnUrl);

    if (PropertyDelegate::INPUT_FILE == delegate->type()) {
        return InputFile;
    }
    if (PropertyDelegate::INPUT_DIR == delegate->type()) {
        return InputDir;
    }
    if (PropertyDelegate::OUTPUT_FILE == delegate->type()) {
        return OutputFile;
    }
    if (PropertyDelegate::OUTPUT_DIR == delegate->type()) {
        return OutputDir;
    }

    return NotAnUrl;
}

/** Truncate the last ';' character */
static void normalizeUrls(QString &urls) {
    if (!urls.isEmpty() && (1 != urls.size()) && (urls[ urls.size() - 1 ] == ';')) {
        urls.truncate(urls.size() - 1);
    }
}

bool WorkflowUtils::validateInputFiles(QString urls, NotificationsList &notificationList) {
    normalizeUrls(urls);
    if (urls.isEmpty()) {
        return true;
    }

    // Verify each URL
    QStringList urlsList = urls.split(';');
    bool res = true;
    foreach (const QString &url, urlsList) {
        QFileInfo fi(url);
        if (!fi.exists()) {
            notificationList << WorkflowNotification(L10N::errorFileNotFound(url));
            res = false;
        }
        else if (!fi.isFile()) {
            notificationList << WorkflowNotification(L10N::errorIsNotAFile(url));
            res = false;
        }
        else {
            QFile testReadAccess(url);
            if (testReadAccess.open(QIODevice::ReadOnly)) {
                testReadAccess.close();
            }
            else {
                notificationList << WorkflowNotification(L10N::errorOpeningFileRead(url));
                res = false;
            }
        }
    }
    return res;
}

bool WorkflowUtils::validateInputDirs(QString urls, NotificationsList &notificationList) {
    normalizeUrls(urls);
    if (urls.isEmpty()) {
        return true;
    }

    QStringList urlsList = urls.split(';');
    bool res = true;
    foreach (const QString &url, urlsList) {
        QFileInfo fi(url);
        if (!fi.exists()) {
            notificationList << WorkflowNotification(L10N::errorDirNotFound(url));
            res = false;
        }
        else if (!fi.isDir()) {
            notificationList << WorkflowNotification(L10N::errorIsNotADir(url));
            res = false;
        }
    }
    return res;
}

namespace {

U2DbiRef url2Ref(const QString &url) {
    const QStringList urlParts = url.split(SharedDbUrlUtils::DB_PROVIDER_SEP);
    CHECK(urlParts.size() == 2, U2DbiRef());

    return U2DbiRef(urlParts[0], urlParts[1]);
}

bool checkDbCredentials(const QString &dbUrl) {
    QString userName;
    const QString shortDbiUrl = U2DbiUtils::full2shortDbiUrl(dbUrl, userName);
    CHECK(!userName.isEmpty(), false);

    if (!AppContext::getPasswordStorage()->contains(dbUrl)) {
        return AppContext::getCredentialsAsker()->askWithFixedLogin(dbUrl);
    } else {
        return true;
    }
}

bool checkObjectInDb(const QString &url) {
    const QStringList urlParts = url.split(",");
    SAFE_POINT(urlParts.size() == 2, "Invalid DB object URL", false);
    const QString dbUrl = urlParts[0];

    U2OpStatusImpl os;
    const U2DbiRef dbRef = url2Ref(dbUrl);
    CHECK(dbRef.isValid(), false);

    const U2DataId realId = SharedDbUrlUtils::getObjectIdByUrl(url);
    CHECK(!realId.isEmpty(), false);

    DbiConnection connection(dbRef, os);
    CHECK_OP(os, false);
    CHECK(NULL != connection.dbi, false);

    U2ObjectDbi *oDbi = connection.dbi->getObjectDbi();
    CHECK(NULL != oDbi, false);
    U2Object testObject;
    oDbi->getObject(testObject, realId, os);
    CHECK_OP(os, false);

    return testObject.hasValidId();
}

bool checkFolderInDb(const QString &dbUrl, const QString &folderPath) {
    U2OpStatusImpl os;
    const U2DbiRef dbRef = url2Ref(dbUrl);
    CHECK(dbRef.isValid(), false);

    CHECK(!folderPath.isEmpty() && folderPath.startsWith(U2ObjectDbi::ROOT_FOLDER), false);

    DbiConnection connection(dbRef, os);
    CHECK_OP(os, false);
    CHECK(NULL != connection.dbi, false);

    U2ObjectDbi *oDbi = connection.dbi->getObjectDbi();
    CHECK(NULL != oDbi, false);
    const qint64 folderVersion = oDbi->getFolderLocalVersion(folderPath, os);
    CHECK_OP(os, false);

    return -1 != folderVersion;
}

bool checkWritePermissionsForDb(const QString &fullDbUrl) {
    U2OpStatusImpl os;
    const U2DbiRef dbRef = SharedDbUrlUtils::getDbRefFromEntityUrl(fullDbUrl);
    CHECK(dbRef.isValid(), false);

    DbiConnection connection(dbRef, os);
    CHECK_OP(os, false);
    return !connection.dbi->getFeatures().contains(U2DbiFeature_GlobalReadOnly);
}

// If a database was unavailable for some reasons during previous validation procedures
// and now has become available, it is needed to remove previous error messages regarding this from a notification list.
bool checkDbConnectionAndFixProblems(const QString &dbUrl, NotificationsList &notificationList, const WorkflowNotification &notificationMsg) {
    if (!WorkflowUtils::checkSharedDbConnection(dbUrl)) {
        notificationList << notificationMsg;
        return false;
    } else {
        foreach(WorkflowNotification notification, notificationList) {
            if (notification.message == notificationMsg.message && notification.type == notificationMsg.type) {
                notificationList.removeAll(notification);
            }
        }
        return true;
    }
}

}

bool WorkflowUtils::checkSharedDbConnection(const QString &fullDbUrl) {
    U2OpStatusImpl os;
    const U2DbiRef dbRef = SharedDbUrlUtils::getDbRefFromEntityUrl(fullDbUrl);
    CHECK(dbRef.isValid(), false);
    CHECK(checkDbCredentials(dbRef.dbiId), false);

    DbiConnection connection(dbRef, os);
    CHECK_OP_EXT(os, AppContext::getPasswordStorage()->removeEntry(dbRef.dbiId), false);
    return connection.isOpen();
}

bool WorkflowUtils::validateInputDbObject(const QString &url, NotificationsList &notificationList) {
    const QString dbUrl = SharedDbUrlUtils::getDbUrlFromEntityUrl(url);
    const U2DataId objId = SharedDbUrlUtils::getObjectIdByUrl(url);
    const QString objName = SharedDbUrlUtils::getDbObjectNameByUrl(url);
    const QString shortDbName = SharedDbUrlUtils::getDbShortNameFromEntityUrl(url);
    if (dbUrl.isEmpty() || objId.isEmpty() || objName.isEmpty()) {
        notificationList << WorkflowNotification(L10N::errorWrongDbObjUrlFormat(url));
        return false;
    } else if (!checkDbConnectionAndFixProblems(dbUrl, notificationList, WorkflowNotification(L10N::errorDbInacsessible(shortDbName)))) {
        return false;
    } else if (!checkObjectInDb(url)) {
        notificationList << WorkflowNotification(L10N::errorDbObjectInaccessible(shortDbName, objName));
        return false;
    }
    return true;
}

bool WorkflowUtils::validateInputDbFolders(QString urls, NotificationsList &notificationList) {
    normalizeUrls(urls);
    if (urls.isEmpty()) {
        return true;
    }

    QStringList urlsList = urls.split(';');
    bool res = true;
    foreach (const QString &url, urlsList) {
        const QString dbUrl = SharedDbUrlUtils::getDbUrlFromEntityUrl(url);
        const QString folderPath = SharedDbUrlUtils::getDbFolderPathByUrl(url);
        const U2DataType dataType = SharedDbUrlUtils::getDbFolderDataTypeByUrl(url);
        const QString shortDbName = SharedDbUrlUtils::getDbShortNameFromEntityUrl(url);
        if (dbUrl.isEmpty() || folderPath.isEmpty() || U2Type::Unknown == dataType) {
            notificationList << WorkflowNotification(L10N::errorWrongDbFolderUrlFormat(url));
            res = false;
        } else if (!checkDbConnectionAndFixProblems(dbUrl, notificationList, WorkflowNotification(L10N::errorDbInacsessible(shortDbName)))) {
            res = false;
        } else if (!checkFolderInDb(dbUrl, folderPath)) {
            notificationList << WorkflowNotification(L10N::errorDbFolderInacsessible(shortDbName, folderPath));
            res = false;
        }
    }
    return res;
}

/**
 * Input @dirAbsPath must be an absolute path to a folder (or empty).
 * The method returns "true" if it is possible to create a file in it.
 */
static bool canWriteToPath(QString dirAbsPath) {
    if (dirAbsPath.isEmpty()) {
        return true;
    }
    QFileInfo fi(dirAbsPath);
    SAFE_POINT(fi.dir().isAbsolute(), "Not an absolute path!", false);

    // Find out the folder that exists
    QDir existenDir(dirAbsPath);
    while (!existenDir.exists()) {
        // Get upper folder
        QString dirPath = existenDir.path();
        QString dirName = existenDir.dirName();
        dirPath.remove(// remove dir name and slash (if any) from the path
            dirPath.length() - dirName.length() - 1,
            dirName.length() + 1);
        if (dirPath.isEmpty()) {
            return false;
        }
        existenDir.setPath(dirPath);
    }

    // Attempts to write a file to the folder.
    // This assumes possibility to create any sub-folder, file, etc.
    QFile file(existenDir.filePath("testWriteAccess.txt"));
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.close();
    file.remove();

    return true;
}

bool WorkflowUtils::validateOutputFile(const QString &url, NotificationsList &notificationList) {
    if (url.isEmpty()) {
        return true;
    }

    QFileInfo fi(url);
    if (fi.isRelative()) {
        fi.setFile(QDir(WorkflowSettings::getWorkflowOutputDirectory()), url);
    }

    if (canWriteToPath(fi.absolutePath())) {
        return true;
    }
    else {
        notificationList << WorkflowNotification(tr("Can't access output file path: '%1'").arg(fi.absoluteFilePath()));
        return false;
    }
}

bool WorkflowUtils::validateOutputDir(const QString &url, NotificationsList &notificationList) {
    if (url.isEmpty()) {
        return true;
    }

    QFileInfo fi(url);
    if (fi.isRelative()) {
        fi.setFile(QDir(WorkflowSettings::getWorkflowOutputDirectory()), url);
    }

    if (canWriteToPath(fi.absoluteFilePath())) {
        return true;
    } else {
        notificationList << WorkflowNotification(tr("Workflow output folder '%1' can't be accessed. Check that the folder exists and you have"
            " enough permissions to write to it, or choose another folder in the UGENE Application Settings.").arg(url), "", WorkflowNotification::U2_ERROR);
        return false;
    }
}

bool WorkflowUtils::isSharedDbUrlAttribute(const Attribute *attr, const Actor *actor) {
    SAFE_POINT(NULL != attr, "Invalid attribute supplied", false);
    SAFE_POINT(NULL != actor, "Invalid actor supplied", false);

    ConfigurationEditor *editor = actor->getEditor();
    CHECK(NULL != editor, false);
    PropertyDelegate *delegate = editor->getDelegate(attr->getId());
    CHECK(NULL != delegate, false);

    return PropertyDelegate::SHARED_DB_URL == delegate->type();
}

bool WorkflowUtils::validateSharedDbUrl(const QString &url, NotificationsList &notificationList) {
    if (url.isEmpty()) {
        notificationList << WorkflowNotification(tr("Empty shared database URL specified"));
        return false;
    }

    const U2DbiRef dbRef = SharedDbUrlUtils::getDbRefFromEntityUrl(url);
    const QString shortDbName = SharedDbUrlUtils::getDbShortNameFromEntityUrl(url);
    if (!dbRef.isValid()) {
        notificationList << WorkflowNotification(L10N::errorWrongDbFolderUrlFormat(url));
        return false;
    } else if (!checkDbConnectionAndFixProblems(url, notificationList, WorkflowNotification(L10N::errorDbInacsessible(shortDbName)))) {
        return false;
    } else if (!checkWritePermissionsForDb(url)) {
        notificationList << WorkflowNotification(L10N::errorDbWritePermissons(shortDbName));
        return false;
    }

    return true;
}

bool WorkflowUtils::validateDatasets(const QList<Dataset> &sets, NotificationsList &notificationList) {
    bool res = true;
    foreach (const Dataset &set, sets) {
        foreach (URLContainer* urlContainer, set.getUrls()) {
            SAFE_POINT(NULL != urlContainer, "NULL URLContainer!", false);
            bool urlIsValid = urlContainer->validateUrl(notificationList);
            res = res && urlIsValid;
        }
    }
    return res;
}

QScriptValue WorkflowUtils::datasetsToScript(const QList<Dataset> &sets, QScriptEngine &engine) {
    QScriptValue setsArray = engine.newArray(sets.size());

    for (int setIdx = 0; setIdx < sets.size(); setIdx++) {
        Dataset set = sets[setIdx];
        QScriptValue urls = engine.newArray(set.getUrls().size());
        for (int urlIdx = 0; urlIdx < set.getUrls().size(); urlIdx++) {
            QString url = set.getUrls()[urlIdx]->getUrl();
            urls.setProperty(urlIdx, engine.newVariant(url));
        }
        setsArray.setProperty(setIdx, urls);
    }

    return setsArray;
}

QString WorkflowUtils::getDatasetSplitter(const QString& filePaths){
    static const QString defaultSplitter = ";";
    static const QString additionalSplitter = ",";

    if(filePaths.contains(defaultSplitter)){
        return defaultSplitter;
    }
    return additionalSplitter;
}

QString WorkflowUtils::packSamples(const QList<TophatSample> &samples) {
    QStringList result;
    foreach (const TophatSample &sample, samples) {
        result << sample.name + ":" + sample.datasets.join(";");
    }
    return result.join(";;");
}

QList<TophatSample> WorkflowUtils::unpackSamples(const QString &samplesStr, U2OpStatus &os) {
    QList<TophatSample> result;

    QStringList pairs = samplesStr.split(";;", QString::SkipEmptyParts);
    foreach (const QString &pairStr, pairs) {
        QStringList pair = pairStr.split(":", QString::KeepEmptyParts);
        if (2 != pair.size()) {
            os.setError(tr("Wrong samples map string"));
            return result;
        }
        result << TophatSample(pair[0], pair[1].split(";", QString::SkipEmptyParts));
    }
    return result;
}

const QString WorkflowEntityValidator::NAME_INACCEPTABLE_SYMBOLS_TEMPLATE = "=\\\"";
const QString WorkflowEntityValidator::ID_ACCEPTABLE_SYMBOLS_TEMPLATE = "a-zA-Z0-9\\-_";

const QRegularExpression WorkflowEntityValidator::ACCEPTABLE_NAME("[^" + NAME_INACCEPTABLE_SYMBOLS_TEMPLATE + "]*");
const QRegularExpression WorkflowEntityValidator::INACCEPTABLE_SYMBOL_IN_NAME("[" + NAME_INACCEPTABLE_SYMBOLS_TEMPLATE + "]");
const QRegularExpression WorkflowEntityValidator::ACCEPTABLE_ID("[" + ID_ACCEPTABLE_SYMBOLS_TEMPLATE + "]*");
const QRegularExpression WorkflowEntityValidator::INACCEPTABLE_SYMBOLS_IN_ID("[^" + ID_ACCEPTABLE_SYMBOLS_TEMPLATE + "]");

/*****************************
 * PrompterBaseImpl
 *****************************/
QVariant PrompterBaseImpl::getParameter(const QString& id) {
    if (map.contains(id)) {
        return map.value(id);
    } else {
        return target->getParameter(id)->getAttributePureValue();
    }
}

QString PrompterBaseImpl::getURL(const QString& id, bool * empty, const QString &onEmpty, const QString &defaultValue) {
    QVariant urlVar = getParameter(id);
    QString url;
    if (urlVar.canConvert< QList<Dataset> >()) {
        QStringList urls = WorkflowUtils::getDatasetsUrls(urlVar.value< QList<Dataset> >());
        url = urls.join(";");
    } else {
        url = getParameter(id).toString();
    }
    if(empty != NULL) { *empty = false; }
    if(!target->getParameter(id)->getAttributeScript().isEmpty()) {
        url = "got from user script";
    } else if (url.isEmpty()) {
        if (!onEmpty.isEmpty()) {
            return onEmpty;
        }
        if (!defaultValue.isEmpty()) {
            url = defaultValue;
        } else {
            url = "<font color='red'>" + tr("unset") + "</font>";
        }
        if(empty != NULL) { *empty = true; }
    } else if (url.indexOf(";") != -1) {
        url = tr("the list of files");
    } else if (SharedDbUrlUtils::isDbObjectUrl(url)) {
        url = SharedDbUrlUtils::getDbObjectNameByUrl(url);
    } else if (SharedDbUrlUtils::isDbFolderUrl(url)) {
        url = Folder::getFolderName(SharedDbUrlUtils::getDbFolderPathByUrl(url));
    } else {
        QString name = QFileInfo(url).fileName();
        if (!name.isEmpty()) {
            url = name;
        }
    }
    return url;
}

QString PrompterBaseImpl::getRequiredParam(const QString& id) {
    QString url = getParameter(id).toString();
    if (url.isEmpty()) {
        url = "<font color='red'>"+tr("unset")+"</font>";
    }
    return url;
}

QString PrompterBaseImpl::getScreenedURL(IntegralBusPort* input, const QString& id, const QString& slot, const QString &onEmpty) {
    bool empty = false;
    QString attrUrl = QString("<u>%1</u>").arg(getURL(id, &empty, onEmpty));
    if(!empty) {
        return attrUrl;
    }

    Actor * origin = input->getProducer(slot);
    QString slotUrl;
    if(origin != NULL) {
        slotUrl = tr("file(s) alongside of input sources of <u>%1</u>").arg(origin->getLabel());
        return slotUrl;
    }

    assert(!attrUrl.isEmpty());
    return attrUrl;
}

QString PrompterBaseImpl::getProducers(const QString& port, const QString& slot)
{
    IntegralBusPort* input = qobject_cast<IntegralBusPort*>(target->getPort(port));
    CHECK(NULL != input, "");
    QList<Actor*> producers = input->getProducers(slot);

    QStringList labels;
    foreach(Actor* a, producers) {
        labels << a->getLabel();
    }
    return labels.join(", ");
}

QString PrompterBaseImpl::getProducersOrUnset(const QString &port, const QString &slot) {
    static const QString unsetStr = "<font color='red'>"+tr("unset")+"</font>";
    QString prods = getProducers(port, slot);
    return prods.isEmpty() ? unsetStr : prods;
}

QString PrompterBaseImpl::getHyperlink(const QString& id, const QString& val) {
    return QString("<a href=%1:%2>%3</a>").arg(WorkflowUtils::HREF_PARAM_ID).arg(id).arg(val);
}

QString PrompterBaseImpl::getHyperlink(const QString& id, int val) {
    return getHyperlink(id, QString::number(val));
}

QString PrompterBaseImpl::getHyperlink(const QString& id, qreal val) {
    return getHyperlink(id, QString::number(val));
}

void PrompterBaseImpl::sl_actorModified() {
    if (AppContext::isGUIMode()) {
        setHtml(QString("<center><b>%1</b></center><hr>%2").arg(target->getLabel()).arg(composeRichDoc()));
    }
}

}//namespace
