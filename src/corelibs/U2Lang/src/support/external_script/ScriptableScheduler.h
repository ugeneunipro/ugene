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

#ifndef _U2_SCRIPTABLE_SCHEDULER_H_
#define _U2_SCRIPTABLE_SCHEDULER_H_

#include <U2Lang/LastReadyScheduler.h>

namespace U2 {

class U2LANG_EXPORT ScriptableScheduler : public LocalWorkflow::LastReadyScheduler {
public:
                                                ScriptableScheduler( Workflow::Schema *scheme );
    virtual                                     ~ScriptableScheduler( );

    // the method is intended for the inheritors which will be
    // capable of processing a few tasks during a single tick
    virtual void                                addActorToNextTick( const ActorId &actor );
    QMap<int, QList<Workflow::Actor *> >        getTopologicalSortedGraph( ) const;
    void                                        setScheme( Workflow::Schema *newScheme );
    Workflow::Actor *                           getActorById( const ActorId &id ) const;
    WorkflowDebugStatus *                       getDebugStatus( ) const;

protected:
    QList<ActorId>                              nextTicks;
};

} // namespace U2

#endif // _U2_SCRIPTABLE_SCHEDULER_H_
