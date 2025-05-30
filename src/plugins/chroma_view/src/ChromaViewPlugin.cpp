/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ChromaViewPlugin.h"

#include <U2Core/ChromatogramObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>

#include "ChromatogramView.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        auto plug = new ChromaViewPlugin();
        return plug;
    }
    return nullptr;
}

ChromaViewPlugin::ChromaViewPlugin()
    : Plugin(tr("Chromatogram View"), tr("Chromatograms visualization")) {
    viewCtx = new ChromaViewContext(this);
    viewCtx->init();
}

ChromaViewPlugin::~ChromaViewPlugin() {
}

ChromaViewContext::ChromaViewContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
}

void ChromaViewContext::initViewContext(GObjectViewController* v) {
    auto av = qobject_cast<AnnotatedDNAView*>(v);
    foreach (ADVSequenceWidget* w, av->getSequenceWidgets()) {
        sl_sequenceWidgetAdded(w);
    }
    connect(av, SIGNAL(si_sequenceWidgetAdded(ADVSequenceWidget*)), SLOT(sl_sequenceWidgetAdded(ADVSequenceWidget*)));
}

static ChromatogramObject* findChromaObj(ADVSingleSequenceWidget* sw) {
    U2SequenceObject* seqObj = sw->getSequenceObject();

    QList<GObject*> allChromas = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::CHROMATOGRAM);
    QList<GObject*> targetChromas = GObjectUtils::findObjectsRelatedToObjectByRole(seqObj,
                                                                                   GObjectTypes::CHROMATOGRAM,
                                                                                   ObjectRole_Sequence,
                                                                                   allChromas,
                                                                                   UOF_LoadedOnly);
    CHECK(!targetChromas.isEmpty(), nullptr);

    return qobject_cast<ChromatogramObject*>(targetChromas.first());
}

void ChromaViewContext::sl_sequenceWidgetAdded(ADVSequenceWidget* w) {
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (sw == nullptr || sw->getSequenceObject() == nullptr || findChromaObj(sw) == nullptr) {
        return;
    }

    auto action = new ChromaViewAction();
    action->setIcon(QIcon(":chroma_view/images/cv.png"));
    action->setCheckable(true);
    action->setChecked(false);
    action->addToMenu = true;
    action->addToBar = true;
    connect(action, SIGNAL(triggered()), SLOT(sl_showChromatogram()));

    sw->addADVSequenceWidgetActionToViewsToolbar(action);

    action->trigger();
}

void ChromaViewContext::sl_showChromatogram() {
    auto a = qobject_cast<ChromaViewAction*>(sender());
    CHECK(a != nullptr, );
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(a->seqWidget);
    ChromatogramObject* chromaObj = findChromaObj(sw);
    CHECK(sw->getSequenceContext(), );
    AnnotatedDNAView* adv = sw->getSequenceContext()->getAnnotatedDNAView();
    CHECK(adv, );
    if (a->isChecked()) {
        CHECK(a->view == nullptr, );
        CHECK(chromaObj != nullptr, );
        adv->addObject(chromaObj);
        a->view = new ChromatogramView(sw, sw->getSequenceContext(), sw->getPanGSLView(), chromaObj->getChromatogram());
        sw->addSequenceView(a->view);
    } else {
        CHECK(a->view != nullptr, );
        GObject* editSeq = a->view->getEditedSequence();
        if (editSeq != nullptr) {
            //! SANGER_RED_TODO: check that it does not brake anything
            adv->removeObject(editSeq);
            //            a->view->getSequenceContext()->getAnnotatedDNAView()->removeObject(editSeq);
        }
        adv->removeObject(chromaObj);
        delete a->view;
        a->view = nullptr;
    }
}

bool ChromaViewContext::canHandle(GObjectViewController* v, GObject* o) {
    Q_UNUSED(v);
    return qobject_cast<ChromatogramObject*>(o) != nullptr;
}

ChromaViewAction::ChromaViewAction()
    : ADVSequenceWidgetAction("CHROMA_ACTION", tr("Show chromatogram")), view(nullptr) {
}

}  // namespace U2
