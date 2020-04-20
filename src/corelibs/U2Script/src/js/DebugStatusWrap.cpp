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

#include <U2Core/AppContext.h>
#include <U2Lang/ScriptContext.h>

#include "ActorWrap.h"
#include "NodeApiUtils.h"
#include "DebugStatusWrap.h"

const char *WRONG_ARGUMENT_TYPE_ERROR = "Wrong argument type";

namespace U2 {

namespace Js {

Persistent<Function> DebugStatusWrap::CONSTRUCTOR;
const char *DebugStatusWrap::CLASS_NAME = "DebugStatus";

DebugStatusWrap::DebugStatusWrap( WorkflowDebugStatus *initDebugStatus )
    : debugStatus( initDebugStatus )
{

}

DebugStatusWrap::~DebugStatusWrap( ) {

}

void DebugStatusWrap::init( ) {
    Local<FunctionTemplate> tpl = FunctionTemplate::New( newObject );
    tpl->SetClassName( String::NewSymbol( CLASS_NAME ) );
    tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "addBreakpointToActor" ),
        FunctionTemplate::New( addBreakpointToActor )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "removeBreakpointFromActor" ),
        FunctionTemplate::New( removeBreakpointFromActor )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "setBreakpointEnabled" ),
        FunctionTemplate::New( setBreakpointEnabled )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "getActorsWithBreakpoints" ),
        FunctionTemplate::New( getActorsWithBreakpoints )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "setPaused" ),
        FunctionTemplate::New( setPaused )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "isPaused" ),
        FunctionTemplate::New( isPaused )->GetFunction( ) );

    CONSTRUCTOR = Persistent<Function>::New( tpl->GetFunction( ) );
}

Handle<Value> DebugStatusWrap::newInstance( int argc, const Handle<Value> *argv ) {
    HandleScope scope;
    const int requiredArgumentCount = 1;
    if ( !NodeApiUtils::isArgumentCountCorrect( argc, requiredArgumentCount ) ) {
        return scope.Close( Undefined( ) );
    }
    Handle<Value> objectInitData[] = { argv[0] };
    Local<Object> instance = CONSTRUCTOR->NewInstance( requiredArgumentCount, objectInitData );
    return scope.Close( instance );
}

Handle<Value> DebugStatusWrap::newObject( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 1 ) ) {
        return scope.Close( Undefined( ) );
    }
    WorkflowDebugStatus *debugStatus
        = reinterpret_cast<WorkflowDebugStatus *>( args[0]->IntegerValue( ) );
    DebugStatusWrap *obj = new DebugStatusWrap( debugStatus );
    obj->Wrap( args.This( ) );
    return args.This( );
}

Handle<Value> DebugStatusWrap::addBreakpointToActor( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 1 ) ) {
        return scope.Close( Undefined( ) );
    }
    if ( !args[0]->IsString( ) ) {
        ThrowException( Exception::TypeError( String::New( WRONG_ARGUMENT_TYPE_ERROR ) ) );
        return scope.Close( Undefined( ) );
    }
    DebugStatusWrap* obj = ObjectWrap::Unwrap<DebugStatusWrap>( args.This( ) );

    const String::Utf8Value actorId( args[0]->ToString( ) );
    obj->debugStatus->addBreakpointToActor( *actorId );
    return scope.Close( Undefined( ) );
}

Handle<Value> DebugStatusWrap::removeBreakpointFromActor( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 1 ) ) {
        return scope.Close( Undefined( ) );
    }
    if ( !args[0]->IsString( ) ) {
        ThrowException( Exception::TypeError( String::New( WRONG_ARGUMENT_TYPE_ERROR ) ) );
        return scope.Close( Undefined( ) );
    }
    DebugStatusWrap* obj = ObjectWrap::Unwrap<DebugStatusWrap>( args.This( ) );

    const String::Utf8Value actorId( args[0]->ToString( ) );
    obj->debugStatus->removeBreakpointFromActor( *actorId );
    return scope.Close( Undefined( ) );
}

Handle<Value> DebugStatusWrap::setBreakpointEnabled( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 2 ) ) {
        return scope.Close( Undefined( ) );
    }
    if ( !args[0]->IsString( ) || !args[1]->IsBoolean( ) ) {
        ThrowException( Exception::TypeError( String::New( WRONG_ARGUMENT_TYPE_ERROR ) ) );
        return scope.Close( Undefined( ) );
    }
    DebugStatusWrap* obj = ObjectWrap::Unwrap<DebugStatusWrap>( args.This( ) );

    const String::Utf8Value actorId( args[0]->ToString( ) );
    bool enabled = args[1]->ToBoolean( )->Value( );
    obj->debugStatus->setBreakpointEnabled( *actorId, enabled );
    return scope.Close( Undefined( ) );
}

Handle<Value> DebugStatusWrap::getActorsWithBreakpoints( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    DebugStatusWrap* obj = ObjectWrap::Unwrap<DebugStatusWrap>( args.This( ) );
    QList<ActorId> actorIds = obj->debugStatus->getActorsWithBreakpoints( );

    ScriptContext *scriptContext = AppContext::getScriptContext( );
    Q_ASSERT( NULL != scriptContext );

    Local<Object> actorList = Object::New( );
    foreach ( ActorId id, actorIds ) {
        Actor *actor = scriptContext->getActorById( id );
        Handle<Value> actorInitData[] = { Int32::New( reinterpret_cast<int>( actor ) ) };
        Handle<Value> wrappedActor = ActorWrap::newInstance( 1, actorInitData );
        actorList->Set( String::NewSymbol( actor->getId( ).toLocal8Bit( ).constData( ) ),
            wrappedActor );
    }
    return scope.Close( actorList );
}

Handle<Value> DebugStatusWrap::setPaused( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 1 ) ) {
        return scope.Close( Undefined( ) );
    }
    if ( !args[0]->IsBoolean( ) ) {
        ThrowException( Exception::TypeError( String::New( WRONG_ARGUMENT_TYPE_ERROR ) ) );
        return scope.Close( Undefined( ) );
    }
    DebugStatusWrap* obj = ObjectWrap::Unwrap<DebugStatusWrap>( args.This( ) );
    bool pause = args[0]->ToBoolean( )->Value( );
    if (pause) {
        obj->debugStatus->sl_pauseTriggerActivated();
    } else {
        obj->debugStatus->sl_resumeTriggerActivated();
    }
    return scope.Close( Undefined( ) );
}

Handle<Value> DebugStatusWrap::isPaused( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    DebugStatusWrap* obj = ObjectWrap::Unwrap<DebugStatusWrap>( args.This( ) );
    return scope.Close( Boolean::New( obj->debugStatus->isPaused( ) ) );
}

} // namespace Js

} // namespace U2
