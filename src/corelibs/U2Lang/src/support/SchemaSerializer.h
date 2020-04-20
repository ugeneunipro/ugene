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

#ifndef _U2_WORKFLOW_SERIALIZER_H_
#define _U2_WORKFLOW_SERIALIZER_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/Schema.h>

class QDomDocument;
class QDomElement;

namespace U2 {
namespace Workflow {

/**
 * utility class for serialization/deserialization schema and all schema elements
 * serialization to xml become obsolete. See HRSchemaSerializer
 *
 */
class U2LANG_EXPORT SchemaSerializer : public QObject {
    Q_OBJECT
public:
    // old schemas support
    static const QMap<QString, QString> ELEM_TYPES_MAP;
    static QString getElemType(const QString & t);

    static const QString WORKFLOW_DOC;
    static void readConfiguration(Configuration*, const QDomElement& );
    static void readParamAliases( QMap<QString, QString> &, const QDomElement& );
    static QDomElement saveActor(const Actor*, QDomElement&);
    static QDomElement savePort(const Port*, QDomElement&);
    static QDomElement saveLink(const Link*, QDomElement&);
    static QString readMeta(Metadata* meta, const QDomElement& proj);
    static void updatePortBindings(const QList<Actor*> & procs);

    static void schema2xml(const Schema& scema, QDomDocument& xml);
    static QString xml2schema(const QDomElement& projectElement, Schema* schema, QMap<ActorId, ActorId>& remapping, bool stopOnErrors = true);

private:
    static QMap<QString, QString> initElemTypesMap();

}; // SchemaSerializer

} //namespace Workflow
} //namespace U2
#endif
