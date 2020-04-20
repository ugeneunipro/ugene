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

#ifndef _U2_SCRIPT_CONTEXT_H_
#define _U2_SCRIPT_CONTEXT_H_

#include <U2Lang/LastReadyScheduler.h>
#include <U2Lang/Schema.h>

namespace U2 {

using namespace LocalWorkflow;

class ScriptableScheduler;
class WorkflowDebugStatus;

class U2LANG_EXPORT ScriptContext {
public:
                                        ScriptContext( );
                                        ~ScriptContext( );
    LastReadyScheduler *                getWorkflowScheduler( Workflow::Schema *scheme ) const;
    void                                setWorkflowScheduler( ScriptableScheduler *newScheduler );
    void                                addActorTick( const ActorId &id );
    QMap<int, QList<Actor *> >          getTopologicalSortedGraph( ) const;
    Actor *                             getActorById( const ActorId &id ) const;
    WorkflowDebugStatus *               getDebugStatus( ) const;

private:
    ScriptableScheduler *               scheduler;
};

} // namespace U2

#endif // _U2_SCRIPT_CONTEXT_H_
