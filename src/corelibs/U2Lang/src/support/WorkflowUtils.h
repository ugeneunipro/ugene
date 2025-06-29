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

#pragma once

#include <QObject>
#include <QProcess>
#include <QVariant>

#include <U2Lang/Dataset.h>
#include <U2Lang/Descriptor.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/Schema.h>
#include <U2Lang/SupportClass.h>
#include <U2Lang/SupportStructures.h>
#include <U2Lang/WorkflowContext.h>

class QListWidgetItem;

namespace U2 {

class Descriptor;
class DocumentFormat;
class Folder;

using namespace Workflow;

enum UrlAttributeType { NotAnUrl,
                        DatasetAttr,
                        InputFile,
                        InputDir,
                        OutputFile,
                        OutputDir };

class U2LANG_EXPORT WorkflowUtils : public QObject {
    Q_OBJECT
public:
    static QString getRichDoc(const Descriptor& d);
    static void setQObjectProperties(QObject&, const QVariantMap&);
    static QString getDropUrl(QList<DocumentFormat*>& fs, const QMimeData* md);
    static QStringList expandToUrls(const QString&);

    static const QStringList WD_FILE_EXTENSIONS;
    static const QString WD_XML_FORMAT_EXTENSION;
    static const QString HREF_PARAM_ID;

    // used in GUI schema validating
    static bool validate(const Workflow::Schema& s, QList<QListWidgetItem*>& errs);
    // used in cmdline schema validating
    static bool validate(const Workflow::Schema& s, QStringList& errs);

    static QList<Descriptor> findMatchingTypes(DataTypePtr set, DataTypePtr elementDataType);
    static QStringList candidatesAsStringList(const QList<Descriptor>& candidates);
    static QList<Descriptor> findMatchingCandidates(DataTypePtr from, DataTypePtr to, const Descriptor& key);
    static QList<Descriptor> findMatchingCandidates(DataTypePtr from, DataTypePtr elementDatatype);
    static Descriptor getCurrentMatchingDescriptor(const QList<Descriptor>& candidates, DataTypePtr to, const Descriptor& key, const StrStrMap& bindings);
    static DataTypePtr getToDatatypeForBusport(IntegralBusPort* p);
    static DataTypePtr getFromDatatypeForBusport(IntegralBusPort* p, DataTypePtr to);

    // find schema with 'name' in common folders or from settings
    static QString findPathToSchemaFile(const QString& name);

    static void getLinkedActorsId(Actor* a, QList<QString>& linkedActors);  // get list of ID's of all linked actors

    static bool isPathExist(const Port* src, const Port* dest);

    static QString getStringForParameterDisplayRole(const QVariant& value);

    static Actor* findActorByParamAlias(const QList<Actor*>& procs, const QString& alias, QString& attrName, bool writeLog = true);

    static Descriptor getSlotDescOfDatatype(const DataTypePtr& dt);

    static QString getParamIdFromHref(const QString& href);

    static QString generateIdFromName(const QString& name);

    static void print(const QString& slotString, const QVariant& data, DataTypePtr type, WorkflowContext* context);

    static void extractPathsFromBindings(StrStrMap& busMap, SlotPathMap& pathMap);

    static void applyPathsToBusMap(StrStrMap& busMap, const SlotPathMap& pathMap);

    static bool startExternalProcess(QProcess* process, const QString& program, const QStringList& arguments);

    static QStringList getDatasetsUrls(const QList<Dataset>& sets);

    static QStringList getAttributeUrls(Attribute* attr);

    static Actor* actorById(const QList<Actor*>& actors, const ActorId& id);

    static QMap<Descriptor, DataTypePtr> getBusType(Port* inPort);

    static bool isBindingValid(const QString& srcSlotId, const QMap<Descriptor, DataTypePtr>& srcBus, const QString& dstSlotId, const QMap<Descriptor, DataTypePtr>& dstBus);

    /** Returns the string which is not contained by @uniqueStrs list
     * Result is created from @str rolling @sep + number suffix
     */
    static QString createUniqueString(const QString& str, const QString& sep, const QStringList& uniqueStrs);

    /** if path == "default" then nothing is changed. Returns the new path */
    static QString updateExternalToolPath(const QString& id, const QString& path);

    static QString getExternalToolPath(const QString& toolId);
    static QString externalToolIsAbsentError(const QString& toolName);
    static QString externalToolError(const QString& toolName);
    static QString externalToolInvalidError(const QString& toolName);
    static QString customExternalToolInvalidError(const QString& toolName, const QString& elementName);

    static void schemaFromFile(const QString& url, Schema* schema, Metadata* meta, U2OpStatus& os);

    /** Use it to check if the attribute contains URL(s) and what are they (input/output, etc.) */
    static UrlAttributeType isUrlAttribute(Attribute* attr, const Actor* actor);

    static bool checkSharedDbConnection(const QString& fullDbUrl);

    /**
     * Validation of input files/folders.
     * Empty input string is considered valid.
     * Otherwise, the input string is split into separate URL(s) by ';'.
     * For each input file: the URL must exist, be a file and have permissions to read from it.
     * For each input folder: the URL must exist and be a folder.
     * For each object from a database: DB URL must be available, object must exist
     * For each folder from a database: DB URL must be available, folder must exist
     */
    static bool validateInputFiles(QString urls, NotificationsList& notificationList);
    static bool validateInputFile(const QString& url, NotificationsList& notificationList);
    static bool validateInputDir(QString url, NotificationsList& notificationList);

    static bool validateInputDbObject(const QString& url, NotificationsList& notificationList);
    static bool validateInputDbFolders(QString urls, NotificationsList& notificationList);

    /**
     * Validation of output file/folder.
     * Empty URL is considered valid.
     * For output URL it is verified that it is accessible for
     * writing (the path can be absolute or relative to the Workflow Output Folder).
     */
    static bool validateOutputFile(const QString& url, NotificationsList& notificationList);
    static bool validateOutputDir(const QString& url, NotificationsList& notificationList);

    static bool isSharedDbUrlAttribute(const Attribute* attr, const Actor* actor);
    static bool validateSharedDbUrl(const QString& url, NotificationsList& notificationList);

    /**
     * Validates input files in datasets are present and readable (i.e.
     * filtered files in input folders are verified).
     */
    static bool validateDatasets(const QList<Dataset>& sets, NotificationsList& notificationList);

    static QScriptValue datasetsToScript(const QList<Dataset>& sets, QScriptEngine& engine);

    static QString packSamples(const QList<TophatSample>& samples);
    static QList<TophatSample> unpackSamples(const QString& samplesStr, U2OpStatus& os);

    static QList<QString> unpackListOfDatasets(const QString& textWithMultipleDatasets);
    static QString packListOfDatasets(const QList<QString>& datasetStrings);

    static QList<QString> unpackListOfUrls(const QString& datasetString);
    static QString packListOfUrls(const QList<QString>& urls);

private:
    static QStringList initExtensions();
    static bool validate(const Workflow::Schema& s, NotificationsList& notificationList);
};  // WorkflowUtils

class U2LANG_EXPORT WorkflowEntityValidator {
public:
    static const QString NAME_INACCEPTABLE_SYMBOLS_TEMPLATE;
    static const QString ID_ACCEPTABLE_SYMBOLS_TEMPLATE;

    static const QRegularExpression ACCEPTABLE_NAME;
    static const QRegularExpression ACCEPTABLE_ID;
    static const QRegularExpression INACCEPTABLE_SYMBOLS_IN_ID;
};


}  // namespace U2
