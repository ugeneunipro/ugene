/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "NodeApiUtils.h"

#include <U2Core/AppContext.h>

const char* TOO_MANY_ARGUMENTS_ERROR = "The function requires less arguments than passed";
const char* TOO_FEW_ARGUMENTS_ERROR = "The function requires more arguments than passed";
const char* CONTEXT_IS_NOT_INITIALIZED = "Ugene script context isn't initialized";

namespace U2 {

namespace Js {

bool NodeApiUtils::isArgumentCountCorrect(int actualCount, int requiredCount) {
    const char* errorText = (actualCount > requiredCount) ? TOO_MANY_ARGUMENTS_ERROR : (actualCount < requiredCount) ? TOO_FEW_ARGUMENTS_ERROR
                                                                                                                     : nullptr;
    if (nullptr != errorText) {
        ThrowException(Exception::TypeError(String::New(errorText)));
    }
    return (nullptr == errorText);
}

ScriptContext* NodeApiUtils::getScriptContext() {
    ScriptContext* scriptContext = AppContext::getScriptContext();
    if (nullptr == scriptContext) {
        ThrowException(Exception::TypeError(String::New(CONTEXT_IS_NOT_INITIALIZED)));
    }
    return scriptContext;
}

}  // namespace Js

}  // namespace U2
