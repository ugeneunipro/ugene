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

#include "DNAGraphPackPlugin.h"
#include "BaseContentGraph.h"
#include "DeviationGraph.h"
#include "KarlinSignatureDifferenceGraph.h"
#include "EntropyAlgorithm.h"
#include "CumulativeSkew.h"
#include "GCFramePlot.h"

#include <U2Gui/MainWindow.h>
#include <U2Core/AppContext.h>

#include <U2View/AnnotatedDNAView.h>
#include <U2View/ADVConstants.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/ADVSequenceObjectContext.h>

#include <U2Gui/GUIUtils.h>

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        DNAGraphPackPlugin * plug = new DNAGraphPackPlugin();
        return plug;
    }
    return NULL;
}


DNAGraphPackPlugin::DNAGraphPackPlugin()
    : Plugin(tr("DNA/RNA Graph Package"),
    tr("Contains a set of graphs for DNA/RNA sequences."))
{
    ctx = new DNAGraphPackViewContext(this);
    ctx->init();
}


DNAGraphPackViewContext::DNAGraphPackViewContext(QObject* p) : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID)
{
    graphFactories.append(new BaseContentGraphFactory(BaseContentGraphFactory::GC, this));
    graphFactories.append(new BaseContentGraphFactory(BaseContentGraphFactory::AG, this));
    graphFactories.append(new GCFramePlotFactory(this));
    graphFactories.append(new DeviationGraphFactory(DeviationGraphFactory::GC, this));
    graphFactories.append(new DeviationGraphFactory(DeviationGraphFactory::AT, this));
    graphFactories.append(new KarlinGraphFactory(this));
    graphFactories.append(new EntropyGraphFactory(this));

//    graphFactories.append(new CumulativeSkewGraphFactory(CumulativeSkewGraphFactory::GC, this));
//    graphFactories.append(new CumulativeSkewGraphFactory(CumulativeSkewGraphFactory::AT, this));
}


void DNAGraphPackViewContext::initViewContext(GObjectView* view)
{
    AnnotatedDNAView* annotView = qobject_cast<AnnotatedDNAView*>(view);
    connect(annotView,
        SIGNAL(si_sequenceWidgetAdded(ADVSequenceWidget*)),
        SLOT(sl_sequenceWidgetAdded(ADVSequenceWidget*)));

    foreach(ADVSequenceWidget* sequenceWidget, annotView->getSequenceWidgets()) {
        sl_sequenceWidgetAdded(sequenceWidget);
    }
}


void DNAGraphPackViewContext::sl_sequenceWidgetAdded(ADVSequenceWidget* _sequenceWidget)
{
    ADVSingleSequenceWidget* sequenceWidget = qobject_cast<ADVSingleSequenceWidget*>(_sequenceWidget);
    if (sequenceWidget == NULL || sequenceWidget->getSequenceObject() == NULL) {
        return;
    }

    foreach (GSequenceGraphFactory* factory, graphFactories) {
        GraphAction *action = new GraphAction(factory);
        if (!factory->isEnabled(sequenceWidget->getSequenceObject())) {
            action->setDisabled(true);
        }
        connect(sequenceWidget, SIGNAL(si_updateGraphView(const QStringList &, const QVariantMap&)),
            action, SLOT(sl_updateGraphView(const QStringList &, const QVariantMap&)));
        GraphMenuAction::addGraphAction(sequenceWidget->getActiveSequenceContext(), action);
    }
}


}//namespace
