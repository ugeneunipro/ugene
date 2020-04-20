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

#include <QtCore/QObject>
#include <QtCore/QString>

#include <U2Core/AppContext.h>
#include <U2Lang/ScriptContext.h>
#include <U2Lang/WorkflowRunFromScriptTask.h>
#include <U2Script/U2Script.h>

#include "ActorWrap.h"
#include "DebugStatusWrap.h"
#include "JsContext.h"
#include "JsScheduler.h"
#include "NodeApiUtils.h"

const char *WRONG_ARGUMENT_TYPE_MESSAGE = "Wrong argument type";
const char *ACTOR_NOT_FOUND_MESSAGE = "Actor with passed id wasn't found on the scheme";
const char *CONTEXT_NOT_INITIALIZED = "Ugene script context wasn't initialized";

const int COUNT_OF_NUMBERS_IN_COUNT_OF_SCHEME_TIERS = 3;

namespace U2 {

namespace Js {

void initModule( Handle<Object> target ) {
    ActorWrap::init( );
    DebugStatusWrap::init( );

    target->Set( String::NewSymbol( "debugStatus" ),
        FunctionTemplate::New( debugStatus )->GetFunction( ) );
    target->Set( String::NewSymbol( "launchSchemeWithScheduler" ),
        FunctionTemplate::New( launchSchemeWithScheduler )->GetFunction( ) );
    target->Set( String::NewSymbol( "topologicalSortedGraph" ),
        FunctionTemplate::New( topologicalSortedGraph )->GetFunction( ) );
    target->Set( String::NewSymbol( "tick" ), FunctionTemplate::New( tick )->GetFunction( ) );
}

Handle<Value> launchSchemeWithScheduler( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length(), 3 ) ) {
        return scope.Close( Undefined( ) );
    }
    if ( !args[0]->IsString( ) || !args[1]->IsFunction( ) || !args[2]->IsString( ) ) {
        ThrowException( Exception::TypeError( String::New( WRONG_ARGUMENT_TYPE_MESSAGE ) ) );
        return scope.Close( Undefined( ) );
    }
    Local<Function> schedulerCallback = Local<Function>::Cast( args[1] );
    JsScheduler *scheduler = new JsScheduler( NULL, schedulerCallback );

    const String::Utf8Value pathToScheme( args[0]->ToString( ) );
    const String::Utf8Value workingDir( args[2]->ToString( ) );
    Handle<Value> result;
    initContext( *workingDir );
    SchemeHandle *scheme = NULL;
    U2ErrorType error = createScheme( *pathToScheme, scheme );
    CHECK( U2_OK == error, scope.Close( Undefined( ) ) );
    result = scope.Close( Number::New( launchScheme( scheme ) ) );
    releaseContext( );
    return result;
}

Handle<Value> topologicalSortedGraph( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length(), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    ScriptContext *scriptContext = NodeApiUtils::getScriptContext( );
    if ( NULL == scriptContext ) {
        return scope.Close( Undefined( ) );
    }
    QMap<int, QList<Actor *> > topologicalSortedGraph = scriptContext->getTopologicalSortedGraph();
    const int schemeTiersCount = topologicalSortedGraph.size();
    Local<Object> scheme = Object::New( );
    for ( int i = 0; schemeTiersCount > i; ++i ) {
        Local<Object> tier = Object::New( );
        int actorCounter = 0;
        foreach ( Actor *actor, topologicalSortedGraph[i] ) {
            Handle<Value> actorInitData[] = { Integer::New( reinterpret_cast<int>( actor ) ) };
            Handle<Value> wrappedActor = ActorWrap::newInstance( 1, actorInitData );
            tier->Set( String::NewSymbol( actor->getId( ).toLocal8Bit( ).constData( ) ),
                wrappedActor );
            ++actorCounter;
        }
        scheme->Set( i, tier );
    }
    return scope.Close( scheme );
}

Handle<Value> tick( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length(), 1 ) ) {
        return scope.Close( Undefined( ) );
    }
    if ( !args[0]->IsString() ) {
        ThrowException( Exception::TypeError( String::New( WRONG_ARGUMENT_TYPE_MESSAGE ) ) );
        return scope.Close( Undefined( ) );
    }
    const String::Utf8Value actorId( args[0]->ToString( ) );
    ScriptContext *scriptContext = NodeApiUtils::getScriptContext( );
    if ( NULL == scriptContext ) {
        return scope.Close( Undefined( ) );
    }
    if ( NULL == scriptContext->getActorById( *actorId ) ) {
        ThrowException( Exception::TypeError( String::New( ACTOR_NOT_FOUND_MESSAGE ) ) );
        return scope.Close( Undefined( ) );
    }
    scriptContext->addActorTick( *actorId );
    return scope.Close( Undefined( ) );
}

Handle<Value> debugStatus( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length(), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    ScriptContext *scriptContext = NodeApiUtils::getScriptContext( );
    if ( NULL == scriptContext ) {
        return scope.Close( Undefined( ) );
    }
    Handle<Value> wrappedDebugInfo;
    WorkflowDebugStatus *debugInfo = scriptContext->getDebugStatus( );
    if ( NULL != debugInfo ) {
        Handle<Value> debugInfoInitData[] = { Integer::New( reinterpret_cast<int>( debugInfo ) ) };
        wrappedDebugInfo = DebugStatusWrap::newInstance( 1, debugInfoInitData );
    } else {
        wrappedDebugInfo = Undefined( );
    }
    return scope.Close( wrappedDebugInfo );
}

} // namespace Js

} // namespace U2
