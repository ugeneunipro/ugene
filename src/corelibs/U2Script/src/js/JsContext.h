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

#ifndef _U2_JS_CONTEXT_H_
#define _U2_JS_CONTEXT_H_

#include <node.h>

#ifdef _DEBUG
#define U2SCRIPT_MODULE_NAME U2Scriptd
#else
#define U2SCRIPT_MODULE_NAME U2Script
#endif

using namespace v8;

namespace U2 {

namespace Js {

void                 initModule( Handle<Object> target );
Handle<Value>        launchSchemeWithScheduler( const Arguments &args );
Handle<Value>        debugStatus( const Arguments &args );
Handle<Value>        topologicalSortedGraph( const Arguments &args );
Handle<Value>        tick( const Arguments &args );

} // namespace Js

} // namespace U2

NODE_MODULE( U2SCRIPT_MODULE_NAME, U2::Js::initModule )

#endif // _U2_JS_CONTEXT_H_
