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

#include "PToolsPlugin.h"
#include "PToolsAligner.h"
#include "PToolsTests.h"

#include <U2Core/AppContext.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmFactory.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmRegistry.h>
#include <U2Test/XMLTestFormat.h>
#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Core/GAutoDeleteList.h>

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    PToolsPlugin *plug = new PToolsPlugin();
    return plug;
}

static const QString PTOOLS_ID("PTools");

/* class PToolsPlugin : public Plugin */

PToolsPlugin::PToolsPlugin() : Plugin(tr("PTools"), tr("Structural alignment algorithm (Sippl MJ, Stegbuchner H) from PTools library"))
{
    StructuralAlignmentAlgorithmFactory *ptools = new PToolsAlignerFactory();
    AppContext::getStructuralAlignmentAlgorithmRegistry()->registerAlgorithmFactory(ptools, PTOOLS_ID);

    // PToolsAligner tests
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = StructualAlignerTests::createTestFactories();

    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }
}

PToolsPlugin::~PToolsPlugin()
{
}

/* class StructualAlignerTests */

QList<XMLTestFactory*> StructualAlignerTests ::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(Gtest_PToolsAlignerTask::createFactory());
    return res;
}

}   // namespace U2

