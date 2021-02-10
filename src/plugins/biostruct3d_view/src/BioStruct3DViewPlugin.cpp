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

#include "BioStruct3DViewPlugin.h"

#include <QMenu>
#include <QMessageBox>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>

#include "BioStruct3DGLWidget.h"
#include "BioStruct3DSplitter.h"

namespace U2 {

/*!
* \mainpage BioStruct3D Viewer Plugin Documentation
*
* \section viewer Introduction
*
* BioStruct3D Viewer is a macromolecular viewing / editing tool.
* It is activated whenever UGENE loads document that contains BioStruct3DObject.
*
*
* \subsection main Main Classes
*
* plugin classes:
* - BioStruct3DGLWidget : Widget for rendering 3d representations of macromolecular structure.
* - BioStruct3DSplitter : Multiple glWidgets layout and manipulation
* - GLFrame : Class for manipulating the 3d viewpoint
*
* plugin interfaces:
* - BioStruct3DGLRenderer : General interface for structure 3d graphical styles
* - BioStruct3DColorScheme : Interface for coloring atoms, bonds, etc.
*
*
*/

extern "C" Q_DECL_EXPORT Plugin *U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        BioStruct3DViewPlugin *plug = new BioStruct3DViewPlugin();
        return plug;
    }
    return NULL;
}

extern "C" Q_DECL_EXPORT bool U2_PLUGIN_VERIFY_FUNC() {
    BioStruct3DGLWidget::checkShaderPrograms();
    BioStruct3DGLWidget::tryGL();
    return true;
}

extern "C" Q_DECL_EXPORT QString *U2_PLUGIN_FAIL_MASSAGE_FUNC() {
    return new QString(BioStruct3DViewPlugin::tr("Unfortunately, your system does not have OpenGL Support.\n"
                                                 "The 3D Structure Viewer is not available.\n"
                                                 "You may try to upgrade your system by updating the video card driver."));
}

BioStruct3DViewPlugin::BioStruct3DViewPlugin()
    : Plugin(tr("3D Structure Viewer"), tr("Visualizes 3D structures of biological molecules.")) {
    // Init plugin view context
    if (BioStruct3DGLWidget::checkShaderPrograms()) {
        viewContext = new BioStruct3DViewContext(this);
        viewContext->init();
    }
}

BioStruct3DViewPlugin::~BioStruct3DViewPlugin() {
}
BioStruct3DViewContext::BioStruct3DViewContext(QObject *p)
    : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID) {
}

void BioStruct3DViewContext::initViewContext(GObjectView *v) {
    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(v);
    U2SequenceObject *dna = av->getSequenceInFocus()->getSequenceObject();

    QList<GObject *> allBiostructs = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::BIOSTRUCTURE_3D);
    QList<GObject *> targetBiostructs = GObjectUtils::findObjectsRelatedToObjectByRole(dna,
                                                                                       GObjectTypes::BIOSTRUCTURE_3D,
                                                                                       ObjectRole_Sequence,
                                                                                       allBiostructs,
                                                                                       UOF_LoadedOnly);
    CHECK(!targetBiostructs.isEmpty(), );

    QList<ADVSequenceWidget *> seqWidgets = av->getSequenceWidgets();
    foreach (ADVSequenceWidget *w, seqWidgets) {
        ADVSingleSequenceWidget *aw = qobject_cast<ADVSingleSequenceWidget *>(w);
        if (aw != NULL) {
            aw->setDetViewCollapsed(true);
            aw->setOverviewCollapsed(true);
        }
    }
    foreach (GObject *obj, targetBiostructs) {
        v->addObject(obj);
    }
}

bool BioStruct3DViewContext::canHandle(GObjectView *v, GObject *o) {
    Q_UNUSED(v);
    bool res = qobject_cast<BioStruct3DObject *>(o) != NULL;
    return res;
}

void BioStruct3DViewContext::onObjectAdded(GObjectView *view, GObject *obj) {
    //todo: add sequence & all objects associated with sequence to the view?

    BioStruct3DObject *obj3d = qobject_cast<BioStruct3DObject *>(obj);
    if (obj3d == NULL || view == NULL) {
        return;
    }

    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(view);
    BioStruct3DSplitter *splitter = NULL;
    if (splitterMap.contains(view)) {
        splitter = splitterMap.value(view);
    } else {
        splitter = new BioStruct3DSplitter(getClose3DViewAction(view), av);
    }

    av->insertWidgetIntoSplitter(splitter);
    splitter->addObject(obj3d);
    splitterMap.insert(view, splitter);
}

void BioStruct3DViewContext::onObjectRemoved(GObjectView *v, GObject *obj) {
    BioStruct3DObject *obj3d = qobject_cast<BioStruct3DObject *>(obj);
    if (obj3d == NULL) {
        return;
    }
    BioStruct3DSplitter *splitter = splitterMap.value(v);
    bool close = splitter->removeObject(obj3d);
    if (close) {
        splitter->close();
        //unregister3DView(v,splitter);
    }
}

void BioStruct3DViewContext::unregister3DView(GObjectView *view, BioStruct3DSplitter *splitter) {
    assert(splitter->getChildWidgets().isEmpty());
    splitter->close();
    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView *>(view);
    av->unregisterSplitWidget(splitter);
    splitterMap.remove(view);
    splitter->deleteLater();
}

QAction *BioStruct3DViewContext::getClose3DViewAction(GObjectView *view) {
    QList<QObject *> resources = viewResources.value(view);
    foreach (QObject *r, resources) {
        GObjectViewAction *a = qobject_cast<GObjectViewAction *>(r);
        if (a != NULL) {
            return a;
        }
    }
    QAction *a = new GObjectViewAction(this, view, tr("Close 3D Structure Viewer"));
    connect(a, SIGNAL(triggered()), SLOT(sl_close3DView()));
    resources.append(a);
    return a;
}

void BioStruct3DViewContext::sl_close3DView() {
    GObjectViewAction *action = qobject_cast<GObjectViewAction *>(sender());
    GObjectView *ov = action->getObjectView();
    QList<GObject *> objects = ov->getObjects();
    foreach (GObject *obj, objects) {
        if (obj->getGObjectType() == GObjectTypes::BIOSTRUCTURE_3D) {
            ov->removeObject(obj);
        }
    }
}

void BioStruct3DViewContext::sl_windowClosing(MWMDIWindow *w) {
    GObjectViewWindow *gvw = qobject_cast<GObjectViewWindow *>(w);
    if (gvw) {
        GObjectView *view = gvw->getObjectView();
        // safe to remove: splitter will be deleted with ADV
        splitterMap.remove(view);
    }

    GObjectViewWindowContext::sl_windowClosing(w);
}

}    // namespace U2
