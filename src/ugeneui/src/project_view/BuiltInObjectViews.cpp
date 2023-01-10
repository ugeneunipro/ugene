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

#include <U2Core/AppContext.h>

#include <U2Gui/ObjectViewModel.h>

#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/AssemblyBrowserFactory.h>
#include <U2View/MaEditorFactory.h>
#include <U2View/SimpleTextObjectView.h>
#include <U2View/TreeViewerFactory.h>

#include "ProjectViewImpl.h"

namespace U2 {

void ProjectViewImpl::registerBuiltInObjectViews() {
    GObjectViewFactoryRegistry* reg = AppContext::getObjectViewFactoryRegistry();

    reg->registerGObjectViewFactory(new SimpleTextObjectViewFactory());
    reg->registerGObjectViewFactory(new AnnotatedDNAViewFactory());

    // MSA Editor.
    auto msaEditorFactory = new MsaEditorFactory();
    reg->registerGObjectViewFactory(msaEditorFactory);
    msaEditorFactory->registerMsaEditorViewFeatures();

    reg->registerGObjectViewFactory(new McaEditorFactory());
    reg->registerGObjectViewFactory(new TreeViewerFactory());
    reg->registerGObjectViewFactory(new AssemblyBrowserFactory());
}

void ProjectViewImpl::unregisterBuiltInObjectViews() {
    GObjectViewFactoryRegistry* reg = AppContext::getObjectViewFactoryRegistry();

    GObjectViewFactory* f = reg->getFactoryById(SimpleTextObjectViewFactory::ID);
    assert(f != nullptr);
    reg->unregisterGObjectViewFactory(f);
    delete f;

    f = reg->getFactoryById(AnnotatedDNAViewFactory::ID);
    assert(f != nullptr);
    reg->unregisterGObjectViewFactory(f);
    delete f;

    f = reg->getFactoryById(MsaEditorFactory::ID);
    assert(f != nullptr);
    reg->unregisterGObjectViewFactory(f);
    delete f;

    f = reg->getFactoryById(TreeViewerFactory::ID);
    assert(f != nullptr);
    reg->unregisterGObjectViewFactory(f);
    delete f;
}

}  // namespace U2
