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

#include <QAction>
#include <QMap>
#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/AnnotatedDNAView.h>
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/MSAEditor.h>
#include <U2View/MaEditorFactory.h>

#include "DNAStatMSAProfileDialog.h"
#include "DNAStatPlugin.h"
#include "DistanceMatrixMSAProfileDialog.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        DNAStatPlugin* plug = new DNAStatPlugin();
        return plug;
    }
    return NULL;
}

DNAStatPlugin::DNAStatPlugin() : Plugin(tr("DNA Statistics"), tr("Provides statistical reports for sequences and alignments"))
{
    statViewCtx = new DNAStatMSAEditorContext(this);
    statViewCtx->init();

    distanceViewCtx = new DistanceMatrixMSAEditorContext(this);
    distanceViewCtx->init();
}

//////////////////////////////////////////////////////////////////////////

DNAStatMSAEditorContext::DNAStatMSAEditorContext(QObject* p) :
GObjectViewWindowContext(p, MsaEditorFactory::ID) {}

void DNAStatMSAEditorContext::initViewContext(GObjectView* v) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    if (msaed != NULL && !msaed->getMaObject())
        return;

    GObjectViewAction* profileAction = new GObjectViewAction(this, v, tr("Generate grid profile..."));
    profileAction->setObjectName("Generate grid profile");
    connect(profileAction, SIGNAL(triggered()), SLOT(sl_showMSAProfileDialog()));

    addViewAction(profileAction);
}

void DNAStatMSAEditorContext::buildMenu(GObjectView* v, QMenu* m) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    if (msaed != NULL && !msaed->getMaObject())
        return;

    QList<GObjectViewAction *> actions = getViewActions(v);
    QMenu* statMenu = GUIUtils::findSubMenu(m, MSAE_MENU_STATISTICS);
    SAFE_POINT(statMenu != NULL, "statMenu", );
    foreach(GObjectViewAction* a, actions) {
        statMenu->addAction(a);
    }
}

void DNAStatMSAEditorContext::sl_showMSAProfileDialog() {
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(sender());
    MSAEditor* msaEd = qobject_cast<MSAEditor*>(viewAction->getObjectView());
    QObjectScopedPointer<DNAStatMSAProfileDialog> d = new DNAStatMSAProfileDialog(msaEd->getWidget(), msaEd);
    d->exec();
}

//////////////////////////////////////////////////////////////////////////

DistanceMatrixMSAEditorContext::DistanceMatrixMSAEditorContext(QObject* p) :
GObjectViewWindowContext(p, MsaEditorFactory::ID) {}

void DistanceMatrixMSAEditorContext::initViewContext(GObjectView* v) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    if (msaed != NULL && !msaed->getMaObject())
        return;

    GObjectViewAction* profileAction = new GObjectViewAction(this, v, tr("Generate distance matrix..."));
    profileAction->setObjectName("Generate distance matrix");
    connect(profileAction, SIGNAL(triggered()), SLOT(sl_showDistanceMatrixDialog()));
    addViewAction(profileAction);
}

void DistanceMatrixMSAEditorContext::buildMenu(GObjectView* v, QMenu* m) {
    MSAEditor* msaed = qobject_cast<MSAEditor*>(v);
    if (msaed != NULL && !msaed->getMaObject())
        return;

    QList<GObjectViewAction *> actions = getViewActions(v);
    QMenu* statMenu = GUIUtils::findSubMenu(m, MSAE_MENU_STATISTICS);
    SAFE_POINT(statMenu != NULL, "statMenu", );
    foreach(GObjectViewAction* a, actions) {
        statMenu->addAction(a);
    }
}

void DistanceMatrixMSAEditorContext::sl_showDistanceMatrixDialog() {
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(sender());
    MSAEditor* msaEd = qobject_cast<MSAEditor*>(viewAction->getObjectView());
    QObjectScopedPointer<DistanceMatrixMSAProfileDialog> d = new DistanceMatrixMSAProfileDialog(msaEd->getWidget(), msaEd);
    d->exec();
}

}//namespace

