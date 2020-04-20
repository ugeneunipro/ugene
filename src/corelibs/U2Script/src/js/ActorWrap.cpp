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

#include <U2Lang/LocalDomain.h>

#include "NodeApiUtils.h"
#include "ActorWrap.h"

namespace U2 {

namespace Js {

Persistent<Function> ActorWrap::CONSTRUCTOR;
const char *ActorWrap::CLASS_NAME = "Actor";

ActorWrap::ActorWrap( const Workflow::Actor *initActor ) : actor( initActor ) {

}

ActorWrap::~ActorWrap( ) {

}

void ActorWrap::init( ) {
    Local<FunctionTemplate> tpl = FunctionTemplate::New( newObject );
    tpl->SetClassName( String::NewSymbol( CLASS_NAME ) );
    tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "id" ),
        FunctionTemplate::New( id )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "label" ),
        FunctionTemplate::New( label )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "isDone" ),
        FunctionTemplate::New( isDone )->GetFunction( ) );
    tpl->PrototypeTemplate( )->Set( String::NewSymbol( "isReady" ),
        FunctionTemplate::New( isReady )->GetFunction( ) );

    CONSTRUCTOR = Persistent<Function>::New( tpl->GetFunction( ) );
}

Handle<Value> ActorWrap::newInstance( int argc, const Handle<Value> *argv ) {
    HandleScope scope;
    const int requiredArgumentCount = 1;
    if ( !NodeApiUtils::isArgumentCountCorrect( argc, requiredArgumentCount ) ) {
        return scope.Close( Undefined( ) );
    }
    Handle<Value> objectInitData[] = { argv[0] };
    Local<Object> instance = CONSTRUCTOR->NewInstance( requiredArgumentCount, objectInitData );
    return scope.Close( instance );
}

Handle<Value> ActorWrap::newObject( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 1 ) ) {
        return scope.Close( Undefined( ) );
    }
    const Workflow::Actor *actor = reinterpret_cast<Workflow::Actor *>( args[0]->IntegerValue( ) );
    Q_ASSERT( NULL != actor );
    ActorWrap *obj = new ActorWrap( actor );
    obj->Wrap( args.This( ) );
    return args.This( );
}

Handle<Value> ActorWrap::id( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    ActorWrap* obj = ObjectWrap::Unwrap<ActorWrap>( args.This( ) );
    return scope.Close( String::New( obj->actor->getId( ).toLocal8Bit( ).constData( ) ) );
}

Handle<Value> ActorWrap::label( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    ActorWrap* obj = ObjectWrap::Unwrap<ActorWrap>( args.This( ) );
    return scope.Close( String::New( obj->actor->getLabel( ).toLocal8Bit( ).constData() ) );
}

Handle<Value> ActorWrap::isDone( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    ActorWrap* obj = ObjectWrap::Unwrap<ActorWrap>( args.This( ) );
    LocalWorkflow::BaseWorker *worker = obj->actor->castPeer<LocalWorkflow::BaseWorker>( );
    Q_ASSERT( NULL != worker );
    return scope.Close( Boolean::New( worker->isDone() ) );
}

Handle<Value> ActorWrap::isReady( const Arguments &args ) {
    HandleScope scope;
    if ( !NodeApiUtils::isArgumentCountCorrect( args.Length( ), 0 ) ) {
        return scope.Close( Undefined( ) );
    }
    ActorWrap* obj = ObjectWrap::Unwrap<ActorWrap>( args.This( ) );
    LocalWorkflow::BaseWorker *worker = obj->actor->castPeer<LocalWorkflow::BaseWorker>( );
    Q_ASSERT( NULL != worker );
    return scope.Close( Boolean::New( worker->isReady() ) );
}

} // namespace Js

} // namespace U2
