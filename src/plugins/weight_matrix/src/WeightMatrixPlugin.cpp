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

#include <QDir>

#include <U2Algorithm/PWMConversionAlgorithm.h>

#include <U2Core/AppContext.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ToolsMenu.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Lang/QueryDesignerRegistry.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "PMatrixFormat.h"
#include "PWMBuildDialogController.h"
#include "PWMSearchDialogController.h"
#include "WMQuery.h"
#include "WeightMatrixIO.h"
#include "WeightMatrixPlugin.h"
#include "WeightMatrixWorkers.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    WeightMatrixPlugin* plug = new WeightMatrixPlugin();
    return plug;
}

WeightMatrixPlugin::WeightMatrixPlugin() : Plugin(tr("Weight matrix"), tr("Search for TFBS with weight matrices"))
{
    if (AppContext::getMainWindow()) {
        ctxADV = new WeightMatrixADVContext(this);
        ctxADV->init();

        QAction* buildAction = new QAction(tr("Build weight matrix..."), this);
        buildAction->setObjectName(ToolsMenu::TFBS_WEIGHT);
        connect(buildAction, SIGNAL(triggered()), SLOT(sl_build()));
        ToolsMenu::addAction(ToolsMenu::TFBS_MENU, buildAction);

        GObjectViewFactory *ff = new PFMatrixViewFactory(this);
        AppContext::getObjectViewFactoryRegistry()->registerGObjectViewFactory(ff);
        ff = new PWMatrixViewFactory(this);
        AppContext::getObjectViewFactoryRegistry()->registerGObjectViewFactory(ff);
    }

    AppContext::getDocumentFormatRegistry()->registerFormat(new PFMatrixFormat(this));
    AppContext::getDocumentFormatRegistry()->registerFormat(new PWMatrixFormat(this));

    LocalWorkflow::PWMatrixWorkerFactory::init();
    LocalWorkflow::PFMatrixWorkerFactory::init();

    QString defaultDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/position_weight_matrix";

    if (LastUsedDirHelper::getLastUsedDir(WeightMatrixIO::WEIGHT_MATRIX_ID).isEmpty()) {
        LastUsedDirHelper::setLastUsedDir(defaultDir, WeightMatrixIO::WEIGHT_MATRIX_ID);
    }

    if (LastUsedDirHelper::getLastUsedDir(WeightMatrixIO::FREQUENCY_MATRIX_ID).isEmpty()) {
        LastUsedDirHelper::setLastUsedDir(defaultDir, WeightMatrixIO::FREQUENCY_MATRIX_ID);
    }

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDWMActorPrototype);
}

WeightMatrixPlugin::~WeightMatrixPlugin() {
}

void WeightMatrixPlugin::sl_build() {
    QWidget *p = (QWidget*)(AppContext::getMainWindow()->getQMainWindow());
    QObjectScopedPointer<PWMBuildDialogController> d = new PWMBuildDialogController(p);
    d->exec();
}

WeightMatrixADVContext::WeightMatrixADVContext(QObject* p) : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID)
{
}

void WeightMatrixADVContext::initViewContext(GObjectView* view) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(view);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":weight_matrix/images/weight_matrix.png"), tr("Find TFBS with matrices..."), 80);
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, SIGNAL(triggered()), SLOT(sl_search()));
}

void WeightMatrixADVContext::sl_search() {
    GObjectViewAction* action = qobject_cast<GObjectViewAction*>(sender());
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(action->getObjectView());

    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    assert(seqCtx->getAlphabet()->isNucleic());
    QObjectScopedPointer<PWMSearchDialogController> d = new PWMSearchDialogController(seqCtx, av->getWidget());
    d->exec();
}

}//namespace
