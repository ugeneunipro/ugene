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

#include <U2Lang/Actor.h>

#include "Utils.h"

namespace U2 {
namespace WorkflowSerialize {

FlowGraph::FlowGraph(const QList<QPair<Port*, Port*> >& d) : dataflowLinks(d), findRecursion(0) {
    removeDuplicates();
    for(int i = 0; i < dataflowLinks.size(); ++i) {
        const QPair<Port*, Port*> & pair = dataflowLinks.at(i);
        if(!graph.contains(pair.first)) {
            graph[pair.first] = QList<Port*>();
        }
        graph.find(pair.first)->append(pair.second);
    }
}

void FlowGraph::removeDuplicates() {
    QList<QPair<Port*, Port*> > links;
    for(int i = 0; i < dataflowLinks.size(); ++i) {
        const QPair<Port*, Port*> & p = dataflowLinks.at(i);
        bool found = false;
        for(int j = 0; j < links.size(); ++j) {
            const QPair<Port*, Port*> & pair = links.at(j);
            if(pair.first == p.first && pair.second == p.second) {
                found = true;
                break;
            }
        }
        if(!found) {
            links << p;
        }
    }
    dataflowLinks = links;
}

bool FlowGraph::findPath(Actor * from, Port * to) const {
    static const int RECURSION_MAX = 100;
    if(findRecursion == RECURSION_MAX) {
        throw ReadFailed(QObject::tr("Cannot create flow graph"));
    }
    foreach(Port * p, from->getOutputPorts()) {
        if(graph[p].contains(to) ) {
            return true;
        }
        foreach(Port * connection, graph[p]) {
            if(findPath(connection->owner(), to)) {
                return true;
            }
        }
    }
    return false;
}

void FlowGraph::minimize() {
    for(int i = 0; i < dataflowLinks.size(); ++i) {
        Port * src = dataflowLinks.at(i).first;
        Port * dst = dataflowLinks.at(i).second;
        foreach(Port * p, graph[src]) {
            findRecursion = 0;
            if(findPath(p->owner(), dst) ){
                graph.find(src)->removeAll(dst);
                break;
            }
        }
    }
}

} // WorkflowSerialize
} // U2
