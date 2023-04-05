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

#include <limits>

#include <QMessageBox>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/ADVUtils.h>

#include "CircularView.h"
#include "CircularViewPlugin.h"
#include "CircularViewSettingsWidgetFactory.h"
#include "CircularViewSplitter.h"
#include "RestrictionMapWidget.h"
#include "SetSequenceOriginDialog.h"
#include "ShiftSequenceStartTask.h"

const QString EXPORT_ACTION_NAME = "Save circular view as image";
const QString NEW_SEQ_ORIGIN_ACTION_NAME = "Set new sequence origin";

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        CircularViewPlugin* plug = new CircularViewPlugin();
        return plug;
    }
    return nullptr;
}

CircularViewSettings::CircularViewSettings()
    : showTitle(true),
      showLength(true),
      titleFontSize(11),
      titleBold(false),
      showRulerLine(true),
      showRulerCoordinates(true),
      rulerFontSize(11),
      labelMode(Mixed),
      labelFontSize(11) {
    titleFont = QApplication::font().family();
}

CircularViewPlugin::CircularViewPlugin()
    : Plugin(tr("CircularView"), tr("Enables drawing of DNA sequences using circular representation")) {
    viewCtx = new CircularViewContext(this);
    viewCtx->init();

    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();
    SAFE_POINT(opWidgetFactoryRegistry != nullptr, tr("OPWidgetFactoryRegistry is NULL"), );
    opWidgetFactoryRegistry->registerFactory(new CircularViewSettingsWidgetFactory(qobject_cast<CircularViewContext*>(viewCtx)));
}

#define CIRCULAR_ACTION_NAME "CircularViewAction"
#define CIRCULAR_VIEW_NAME "CIRCULAR_VIEW"

CircularViewContext::CircularViewContext(QObject* p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
    ;
}

CircularViewSettings* CircularViewContext::getSettings(AnnotatedDNAView* view) {
    return viewSettings.value(view);
}

void CircularViewContext::initViewContext(GObjectViewController* v) {
    auto av = qobject_cast<AnnotatedDNAView*>(v);
    SAFE_POINT(!viewSettings.contains(av), "Unexpected sequence view", );

    viewSettings.insert(av, new CircularViewSettings());

    GObjectViewAction* exportAction = new GObjectViewAction(this, v, tr("Save circular view as image..."));
    exportAction->setIcon(QIcon(":/core/images/cam2.png"));
    exportAction->setObjectName(EXPORT_ACTION_NAME);
    addViewAction(exportAction);

    GObjectViewAction* setSequenceOriginAction = new GObjectViewAction(this, v, tr("Set new sequence origin..."));
    setSequenceOriginAction->setObjectName(NEW_SEQ_ORIGIN_ACTION_NAME);
    connect(setSequenceOriginAction, SIGNAL(triggered()), SLOT(sl_setSequenceOrigin()));
    addViewAction(setSequenceOriginAction);

    foreach (ADVSequenceWidget* w, av->getSequenceWidgets()) {
        sl_sequenceWidgetAdded(w);
    }
    connect(av, SIGNAL(si_sequenceWidgetAdded(ADVSequenceWidget*)), SLOT(sl_sequenceWidgetAdded(ADVSequenceWidget*)));
    connect(av, SIGNAL(si_sequenceWidgetRemoved(ADVSequenceWidget*)), SLOT(sl_sequenceWidgetRemoved(ADVSequenceWidget*)));
    connect(av, SIGNAL(si_onClose(AnnotatedDNAView*)), SLOT(sl_onDNAViewClosed(AnnotatedDNAView*)));

    ADVGlobalAction* globalToggleViewAction = new ADVGlobalAction(av, QIcon(":circular_view/images/circular.png"), tr("Toggle circular views"), std::numeric_limits<int>::max(), ADVGlobalActionFlags(ADVGlobalActionFlag_AddToToolbar) | ADVGlobalActionFlag_SingleSequenceOnly);  // big enough to be the last one?

    globalToggleViewAction->addAlphabetFilter(DNAAlphabet_NUCL);
    globalToggleViewAction->setObjectName("globalToggleViewAction");
    connect(globalToggleViewAction, SIGNAL(triggered()), SLOT(sl_toggleViews()));
}

#define MIN_LENGTH_TO_AUTO_SHOW (1000 * 1000)

void CircularViewContext::sl_sequenceWidgetAdded(ADVSequenceWidget* w) {
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (sw == nullptr || sw->getSequenceObject() == nullptr || !sw->getSequenceObject()->getAlphabet()->isNucleic()) {
        return;
    }

    CircularViewAction* action = new CircularViewAction();
    action->setIcon(QIcon(":circular_view/images/circular.png"));
    action->setCheckable(true);
    action->setChecked(false);
    action->addToMenu = true;
    action->addToBar = true;
    connect(action, SIGNAL(triggered()), SLOT(sl_showCircular()));

    sw->addADVSequenceWidgetActionToViewsToolbar(action);

    qint64 len = sw->getSequenceContext()->getSequenceLength();
    if (len < MIN_LENGTH_TO_AUTO_SHOW) {
        bool circular = sw->getSequenceContext()->getSequenceObject()->isCircular();
        if (circular) {
            action->trigger();
        }
    }

    connect(sw->getSequenceObject(), SIGNAL(si_sequenceCircularStateChanged()), action, SLOT(sl_circularStateChanged()));
}

void CircularViewContext::sl_sequenceWidgetRemoved(ADVSequenceWidget* w) {
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (!sw->getSequenceObject()->getAlphabet()->isNucleic()) {
        return;
    }

    CircularViewSplitter* splitter = getView(sw->getAnnotatedDNAView(), false);
    if (splitter != nullptr) {
        auto a = qobject_cast<CircularViewAction*>(sw->getADVSequenceWidgetAction(CIRCULAR_ACTION_NAME));
        SAFE_POINT(a != nullptr, "Circular view action is not found", );
        CHECK(a->view != nullptr, );
        splitter->removeView(a->view, a->rmapWidget);
        delete a->view;
        delete a->rmapWidget;
        if (splitter->isEmpty()) {
            removeCircularView(sw->getAnnotatedDNAView());
        }
    }
}

CircularViewSplitter* CircularViewContext::getView(GObjectViewController* view, bool create) {
    CircularViewSplitter* circularView = nullptr;
    QList<QObject*> resources = viewResources.value(view);
    foreach (QObject* r, resources) {
        circularView = qobject_cast<CircularViewSplitter*>(r);
        if (nullptr != circularView) {
            return circularView;
        }
    }
    if (create) {
        auto av = qobject_cast<AnnotatedDNAView*>(view);
        circularView = new CircularViewSplitter(av);
        QList<GObjectViewAction*> actions = getViewActions(view);
        SAFE_POINT(2 == actions.size(), "Unexpected CV action count", circularView);
        connect(actions[0], SIGNAL(triggered()), circularView, SLOT(sl_export()));

        emit si_cvSplitterWasCreatedOrRemoved(circularView, viewSettings.value(av));
        resources.append(circularView);
        viewResources.insert(view, resources);
    }
    return circularView;
}

//////////////////////////////////////////////////////////////////////////
void CircularViewContext::buildStaticOrContextMenu(GObjectViewController* v, QMenu* m) {
    bool empty = true;
    QList<QObject*> resources = viewResources.value(v);
    foreach (QObject* r, resources) {
        auto circularView = qobject_cast<CircularViewSplitter*>(r);
        if (nullptr != circularView) {
            if (!circularView->isEmpty()) {
                empty = false;
            }
            break;
        }
    }
    CHECK(!empty, );

    QMenu* exportMenu = GUIUtils::findSubMenu(m, ADV_MENU_EXPORT);
    SAFE_POINT(exportMenu != nullptr, "Invalid exporting menu", );
    GObjectViewAction* exportAction = findViewAction(v, EXPORT_ACTION_NAME);
    SAFE_POINT(nullptr != exportAction, "Invalid exporting action", );
    exportMenu->addAction(exportAction);

    QMenu* editMenu = GUIUtils::findSubMenu(m, ADV_MENU_EDIT);
    SAFE_POINT(editMenu != nullptr, "Invalid editing menu", );
    GObjectViewAction* newSeqOriginAction = findViewAction(v, NEW_SEQ_ORIGIN_ACTION_NAME);
    SAFE_POINT(nullptr != newSeqOriginAction, "Invalid new sequence origin action", );
    editMenu->addAction(newSeqOriginAction);
}
//////////////////////////////////////////////////////////////////////////

void CircularViewContext::removeCircularView(GObjectViewController* view) {
    QList<QObject*> resources = viewResources.value(view);
    foreach (QObject* r, resources) {
        auto circularView = qobject_cast<CircularViewSplitter*>(r);
        if (nullptr != circularView) {
            SAFE_POINT(circularView->isEmpty(), "Circular view is not empty", );
            auto av = qobject_cast<AnnotatedDNAView*>(view);
            av->unregisterSplitWidget(circularView);
            resources.removeOne(circularView);
            viewResources.insert(view, resources);
            delete circularView;
            emit si_cvSplitterWasCreatedOrRemoved(nullptr, viewSettings.value(av));
        }
    }
}

void CircularViewContext::toggleViews(AnnotatedDNAView* av) {
    SAFE_POINT(av != nullptr, "AnnotatedDNAView is NULL", );
    QList<ADVSequenceWidget*> sWidgets = av->getSequenceWidgets();

    bool openAll = true;
    foreach (ADVSequenceWidget* sw, sWidgets) {
        SAFE_POINT(sw != nullptr, "ADVSequenceWidget is NULL", );
        auto a = qobject_cast<CircularViewAction*>(sw->getADVSequenceWidgetAction(CIRCULAR_ACTION_NAME));
        CHECK_CONTINUE(a != nullptr);
        // if there is at least one opened CV, close them all
        if (a->isChecked()) {
            openAll = false;
            a->trigger();
        }
    }

    if (openAll) {
        foreach (ADVSequenceWidget* sw, sWidgets) {
            SAFE_POINT(sw != nullptr, "ADVSequenceWidget is NULL", );
            auto a = qobject_cast<CircularViewAction*>(sw->getADVSequenceWidgetAction(CIRCULAR_ACTION_NAME));
            CHECK_CONTINUE(a != nullptr);
            a->trigger();
        }
    }
}

void CircularViewContext::sl_showCircular() {
    auto a = qobject_cast<CircularViewAction*>(sender());
    SAFE_POINT(nullptr != a, "Invalid CV action", );
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(a->seqWidget);
    if (a->isChecked()) {
        a->setText(tr("Remove circular view"));
        assert(a->view == nullptr);
        CircularViewSplitter* splitter = getView(sw->getAnnotatedDNAView(), true);
        a->view = new CircularView(sw, sw->getSequenceContext(), viewSettings.value(sw->getAnnotatedDNAView()));
        a->view->setObjectName("CV_" + sw->objectName());
        a->rmapWidget = new RestrctionMapWidget(sw->getSequenceContext(), splitter);
        splitter->addView(a->view, a->rmapWidget);
        sw->getAnnotatedDNAView()->insertWidgetIntoSplitter(splitter);
        splitter->adaptSize();
    } else {
        a->setText(tr("Show circular view"));
        assert(a->view != nullptr);
        CircularViewSplitter* splitter = getView(sw->getAnnotatedDNAView(), false);
        if (splitter != nullptr) {
            splitter->removeView(a->view, a->rmapWidget);
            delete a->view;
            delete a->rmapWidget;
            if (splitter->isEmpty()) {
                removeCircularView(sw->getAnnotatedDNAView());
            }
        }
        a->view = nullptr;
    }
}

void CircularViewContext::sl_toggleViews() {
    auto globalToggleViewAction = qobject_cast<ADVGlobalAction*>(sender());
    assert(globalToggleViewAction != nullptr);
    auto av = qobject_cast<AnnotatedDNAView*>(globalToggleViewAction->getObjectView());
    if (av == nullptr) {
        return;
    }
    toggleViews(av);
}

void CircularViewContext::sl_setSequenceOrigin() {
    auto setSequenceOriginAction = qobject_cast<GObjectViewAction*>(sender());
    SAFE_POINT(nullptr != setSequenceOriginAction, "Invalid action detected", );

    auto av = qobject_cast<AnnotatedDNAView*>(setSequenceOriginAction->getObjectView());
    CHECK(nullptr != av, );

    ADVSequenceObjectContext* seqCtx = av->getActiveSequenceContext();
    U2SequenceObject* seqObj = seqCtx->getSequenceObject();
    CHECK(nullptr != seqObj, );

    QObjectScopedPointer<SetSequenceOriginDialog> dlg = new SetSequenceOriginDialog(av->getActiveSequenceWidget());
    const int res = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (res == QDialog::Accepted) {
        int newSeqStart = dlg->getSequenceShift();
        if (newSeqStart != 1) {
            seqCtx->getSequenceSelection()->clear();
            Task* t = new ShiftSequenceStartTask(seqObj, newSeqStart - 1);
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
            connect(t, SIGNAL(si_stateChanged()), av, SLOT(sl_sequenceModifyTaskStateChanged()));
        }
    }
}

void CircularViewContext::sl_onDNAViewClosed(AnnotatedDNAView* v) {
    CircularViewSettings* settings = viewSettings.value(v);
    viewSettings.remove(v);
    delete settings;
}

void CircularViewContext::sl_toggleBySettings(CircularViewSettings* s) {
    AnnotatedDNAView* av = viewSettings.key(s, nullptr);
    SAFE_POINT(av != nullptr, "No AnnotatedDNAView corresponds to provided CircularViewSettings", );
    toggleViews(av);
}

CircularViewAction::CircularViewAction()
    : ADVSequenceWidgetAction(CIRCULAR_ACTION_NAME, tr("Show circular view")), view(nullptr), rmapWidget(nullptr) {
}

void CircularViewAction::sl_circularStateChanged() {
    auto seqObj = qobject_cast<U2SequenceObject*>(sender());
    SAFE_POINT(seqObj != nullptr, "Sequence Object is NULL", );

    // if sequence is marked as circular and CV is hidden, show CV, and hide it if unmarked
    if (seqObj->isCircular() && !isChecked()) {
        trigger();
    } else if (!seqObj->isCircular() && isChecked()) {
        trigger();
    }
}

}  // namespace U2
