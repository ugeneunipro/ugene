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

#ifndef _U2_WORKFLOW_SERIALIZE_UTILS_H_
#define _U2_WORKFLOW_SERIALIZE_UTILS_H_

#include <U2Lang/Schema.h>

#include "Tokenizer.h"

namespace U2 {
using namespace Workflow;
namespace WorkflowSerialize {

struct ReadFailed {
    ReadFailed(const QString & msg) : what(msg) {}
    QString what;
}; // ReadFailed

class WorkflowSchemaReaderData {
public:
    WorkflowSchemaReaderData(const QString & bytes, Schema * s, Metadata * m, QMap<ActorId, ActorId>* im)
        : schema(s), meta(m), idMap(im) {
        graphDefined = false;
        tokenizer.tokenizeSchema(bytes);
    }

    Tokenizer tokenizer;
    Schema * schema;
    Metadata * meta;
    QMap<QString, Actor*> actorMap;
    QList<QPair<Port*, Port*> > dataflowLinks;
    QList<QPair<Port*, Port*> > links;
    QMap<ActorId, ActorId> * idMap;
    QList<PortAlias> portAliases;
    QList<Wizard*> wizards;

    bool isGraphDefined() const {
        return graphDefined;
    }

    void defineGraph() {
        graphDefined = true;
    }

private:
    bool graphDefined;
}; // WorkflowSchemaReaderData

class FlowGraph {
public:
    FlowGraph( const QList<QPair<Port*, Port*> >& d );
    bool findPath(Actor * from, Port * to) const;
    void removeDuplicates();
    void minimize();

    QMap<Port*, QList<Port*> > graph;
    QList<QPair<Port*, Port*> > dataflowLinks;
    int findRecursion;
}; // FlowGraph

} // WorkflowSerialize
} // U2

#endif // _U2_WORKFLOW_SERIALIZE_UTILS_H_
