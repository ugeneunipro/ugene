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
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/GTestFrameworkComponents.h>

#include "ApiTestsPlugin.h"
#include "UnitTestSuite.h"

namespace U2 {

extern "C" Q_DECL_EXPORT U2::Plugin *U2_PLUGIN_INIT_FUNC()
{
    return new ApiTestsPlugin();
}

ApiTestsPlugin::ApiTestsPlugin() :
Plugin("UGENE 2.0 API tests", "Tests for UGENE 2.0 public API") {
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    XMLTestFactory* f = UnitTestSuite::createFactory();
    bool res = xmlTestFormat->registerTestFactory(f);
    SAFE_POINT(res, "API tests is not registered",);
    Q_UNUSED(res);
}

} // namespace U2
