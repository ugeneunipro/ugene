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

#ifndef _U2_DEBUG_STATUS_WRAP_H_
#define _U2_DEBUG_STATUS_WRAP_H_

#include <node.h>

#include <U2Lang/WorkflowDebugStatus.h>

using namespace v8;

namespace U2 {

namespace Js {

class DebugStatusWrap : public node::ObjectWrap {
public:
    static void                         init( );
    static Handle<Value>                newInstance( int argc, const Handle<Value> *argv );

private:
                                        DebugStatusWrap( WorkflowDebugStatus *initDebugStatus );
                                        ~DebugStatusWrap( );

    static Handle<Value>                newObject( const Arguments &args );
    static Handle<Value>                addBreakpointToActor( const Arguments &args );
    static Handle<Value>                removeBreakpointFromActor( const Arguments &args );
    static Handle<Value>                setBreakpointEnabled( const Arguments &args );
    static Handle<Value>                getActorsWithBreakpoints( const Arguments &args );
    static Handle<Value>                setPaused( const Arguments &args );
    static Handle<Value>                isPaused( const Arguments &args );

    static Persistent<Function>         CONSTRUCTOR;
    static const char *                 CLASS_NAME;

    WorkflowDebugStatus *               debugStatus;
};

} // namespace Js

} // namespace U2

#endif // _U2_DEBUG_STATUS_WRAP_H_
