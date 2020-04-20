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

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>
#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include <U2Core/QObjectScopedPointer.h>

#include "DNAFlexDialog.h"
#include "DNAFlexGraph.h"
#include "DNAFlexPlugin.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC()
{
    DNAFlexPlugin* plugin = new DNAFlexPlugin();
    return plugin;
}


DNAFlexPlugin::DNAFlexPlugin()
: Plugin(
         tr("DNA Flexibility"),
         tr("Searches a DNA sequence for regions of high DNA helix flexibility.")),
         viewCtx(0)
{
    if (AppContext::getMainWindow())
    {
        viewCtx = new DNAFlexViewContext(this);
        viewCtx->init();
    }
}


DNAFlexViewContext::DNAFlexViewContext(QObject* parent)
    : GObjectViewWindowContext(parent, ANNOTATED_DNA_VIEW_FACTORY_ID)
{
    graphFactory = new DNAFlexGraphFactory(this);
}


void DNAFlexViewContext::sl_showDNAFlexDialog()
{
    QAction* action = (QAction*) sender();
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(action);
    AnnotatedDNAView* annotView = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    assert(annotView);

    ADVSequenceObjectContext* seqCtx = annotView->getSequenceInFocus();
    SAFE_POINT(seqCtx != NULL, "no sequence to perform flex search",);

    const DNAAlphabet *alphabet = seqCtx->getAlphabet();
    SAFE_POINT(alphabet->isNucleic(), "alphabet is not nucleic, dialog should not have been invoked",);

    if (alphabet->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT())
    {
        QObjectScopedPointer<DNAFlexDialog> dialog = new DNAFlexDialog(seqCtx);
        dialog->exec();
        CHECK(!dialog.isNull(), );
    }
    else
    {
        QMessageBox::critical(0, L10N::errorTitle(),
            tr("Unsupported sequence alphabet, only standard DNA alphabet is supported"));
        return;
    }
}


void DNAFlexViewContext::initViewContext(GObjectView* view)
{
    AnnotatedDNAView* annotView = qobject_cast<AnnotatedDNAView*>(view);

    // Adding the graphs item
    connect(annotView,
        SIGNAL(si_sequenceWidgetAdded(ADVSequenceWidget*)),
        SLOT(sl_sequenceWidgetAdded(ADVSequenceWidget*)));

    foreach (ADVSequenceWidget* sequenceWidget, annotView->getSequenceWidgets())
    {
        sl_sequenceWidgetAdded(sequenceWidget);
    }

    // Adding the action to the Analyze menu, but not to the toolbar
    ADVGlobalAction* action = new ADVGlobalAction(annotView,
        QIcon(":dna_flexibility/images/flexibility.png"),
        tr("Find high DNA flexibility regions..."),
        2000,
        ADVGlobalActionFlags(ADVGlobalActionFlag_AddToAnalyseMenu));
    action->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(action, SIGNAL(triggered()), SLOT(sl_showDNAFlexDialog()));
}


void DNAFlexViewContext::sl_sequenceWidgetAdded(ADVSequenceWidget* _sequenceWidget)
{
    ADVSingleSequenceWidget* sequenceWidget = qobject_cast<ADVSingleSequenceWidget*>(_sequenceWidget);
    if (sequenceWidget == NULL || sequenceWidget->getSequenceObject() == NULL)
    {
        return;
    }

    // If the alphabet is not the  standard DNA alphabet
    if (sequenceWidget->getSequenceContext()->getAlphabet()->getId() != BaseDNAAlphabetIds::NUCL_DNA_DEFAULT())
    {
        return;
    }

    // Otherwise add the "DNA Flexibility" action to the graphs menu
    GraphAction* graphAction = new GraphAction(graphFactory);
    connect(sequenceWidget, SIGNAL(si_updateGraphView(const QStringList &, const QVariantMap&)), graphAction, SLOT(sl_updateGraphView(const QStringList &, const QVariantMap&)));
    GraphMenuAction::addGraphAction(sequenceWidget->getActiveSequenceContext(), graphAction);
}


} // namespace
