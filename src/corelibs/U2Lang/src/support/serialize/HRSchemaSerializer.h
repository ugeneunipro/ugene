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

#ifndef _U2_WORKFLOW_HR_SCHEMA_SERIALIZER_H_
#define _U2_WORKFLOW_HR_SCHEMA_SERIALIZER_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <U2Core/global.h>
#include <U2Lang/Aliasing.h>
#include <U2Lang/Dataset.h>
#include <U2Lang/Schema.h>
#include <U2Lang/Port.h>
#include <U2Lang/ActorModel.h>
#include <U2Lang/GrouperOutSlot.h>
#include <U2Lang/URLContainer.h>

#include "Utils.h"

namespace U2 {

using namespace Workflow;
using namespace WorkflowSerialize;

class AttributeConfig;
class DataConfig;
class ExternalProcessConfig;
class Marker;

class U2LANG_EXPORT HRSchemaSerializer : public QObject {
    Q_OBJECT
public:
    typedef QMap<ActorId, QString> NamesMap;

    static void parseHeader(Tokenizer & tokenizer, Metadata * meta);
    static void parseIncludes(Tokenizer &tokenizer, QList<QString> includedUrls);
    static void parseBodyHeader(Tokenizer & tokenizer, Metadata * meta, bool needName = true);
    static Actor* parseElementsDefinition(Tokenizer & tokenizer, const QString & actorName, QMap<QString, Actor*> & actorMap,
                                            QMap<ActorId, ActorId>* idMap = NULL);
    static ValidatorDesc parseValidator(const QString &desc, U2OpStatus &os);
    static void parseGrouperOutSlots(Actor *proc, const QStringList &outSlots, const QString &attrId);
    static QPair<Port*, Port*> parseDataflow(Tokenizer & tokenizer, const QString & srcTok, const QMap<QString, Actor*> & actorMap);
    static QString parseAt(const QString & dottedStr, int ind);
    static QString parseAfter(const QString & dottedStr, int ind);
    static QMap<ActorId, QVariantMap> parseIteration(Tokenizer & tokenizer,
                                        const QMap<QString, Actor*> & actorMap, bool pasteMode = false);
    static void parseActorBindings(Tokenizer &tokenizer, WorkflowSchemaReaderData &data);
    static void parseParameterAliases(Tokenizer & tokenizer, const QMap<QString, Actor*> & actorMap);
    static void parsePortAliases(Tokenizer & tokenizer, const QMap<QString, Actor*> & actorMap, QList<PortAlias> &portAliases);

    static void finalizeGrouperSlots(const QMap<QString, Actor*> &actorMap);

    // -------------- backward compatibility --------------
        static void parseOldAliases(Tokenizer & tokenizer, const QMap<QString, Actor*> & actorMap);
        static void parseAliasesHelp(Tokenizer & tokenizer, const QList<Actor*> & procs);
        static Actor* deprecatedActorsReplacer(const QString &id, const QString &protoId, ParsedPairs &pairs);
    // ----------------------------------------------------

    // if slot has no val-> add it to binding
    static void addEmptyValsToBindings(const QList<Actor*> & procs);
    // idMap not null in copy mode
    static QString string2Schema(const QString & data, Schema * schema, Metadata * meta = NULL, QMap<ActorId, ActorId>* idMap = NULL, QList<QString> includedUrls = QList<QString>());
    // the method checks port relations:
    // if the attribute has port relation and the port has a link - the value of attribute must be set to the value that allows port enabling
    static void postProcessing(Schema* schema);

    static void addPart( QString & to, const QString & w);
    static QString header2String(const Metadata * meta);
    static QString makeBlock(const QString & title, const QString & name, const QString & blockItself, int tabsNum = 1, bool nl = false, bool sc = false);
    static QString makeEqualsPair(const QString & key, const QString & value, int tabsNum = 2, bool quoteEmpty = false);
    static QString makeArrowPair( const QString & left, const QString & right, int tabsNum = 1 );
    static QString scriptBlock(const QString & scriptText, int tabsNum = 3);
    static QString includesDefinition(const QList<Actor*> & procs);
    static QString elementsDefinition(const QList<Actor*> & procs, const NamesMap & nmap, bool copyMode = false);
    static QString markersDefinition(Attribute *attribute);
    static QString grouperOutSlotsDefinition(Attribute *attribute);
    static QString actorBindings(const ActorBindingsGraph & graph, const NamesMap &nmap, bool copyMode = false);
    static QString dataflowDefinition(const QList<Actor*> & procs, const NamesMap & nmap);
    static QString schemaParameterAliases(const QList<Actor*> & procs, const NamesMap& nmap);
    static QString schemaPortAliases(const NamesMap &nmap, const QList<PortAlias> &portAliases);
    static NamesMap generateElementNames(const QList<Actor*>& procs);
    static QString schema2String(const Schema & schema, const Metadata * meta, bool copyMode = false);
    static QString items2String(const QList<Actor*> &actors, const Metadata *meta);

    static ExternalProcessConfig* string2Actor(const QString & bytes);
    static QString actor2String(ExternalProcessConfig *cfg);
    static ExternalProcessConfig *parseActorBody(Tokenizer & tokenizer);
    static void parsePorts(Tokenizer & tokenizer, QList<DataConfig>& ports);
    static void parseAttributes(Tokenizer & tokenizer, QList<AttributeConfig>& attrs);

    static QMap<ActorId, ActorId> deepCopy(const Schema& from, Schema* to, U2OpStatus &os);

    static bool isHeaderLine(const QString &line);
    static QString valueString(const QString & s, bool quoteEmpty = false);

    static void saveSchema(Schema *schema, Metadata *meta, const QString &url, U2OpStatus &os);

    static Marker * parseMarker(ParsedPairs &pairs, const QString &MARKER_TYPE, const QString &MARKER_NAME);

private:
    static GrouperSlotAction parseAction(Tokenizer &tokenizer);
    static QList<Dataset> parseUrlAttribute(const QString attrId, QList<StrStrPair> &blockPairs);
    static void deprecatedUrlAttribute(Actor *proc, const QString &urls);
    static URLContainer * parseDirectoryUrl(Tokenizer &tokenizer);
    static URLContainer * parseDbSelectUrl(Tokenizer &tokenizer);
    static URLContainer * createDbObjectUrl(const QString &dbUrl, const qint64 objId, const QString &dataType, const QString &objCachedName);
    static DbFolderUrlContainer * createDbFolderUrl(const QString &dbUrl, const QString &dataType, const QString &path, bool recursive);
    static void checkHeaderLine(const QString &line, Tokenizer &tokenizer);
    static void parseMarkers(Actor *proc, const QStringList &markers, const QString &attrId);
    static Marker * parseMarker(const QString &def);
};

} // U2

#endif // _U2_WORKFLOW_HR_SCHEMA_SERIALIZER_H_
