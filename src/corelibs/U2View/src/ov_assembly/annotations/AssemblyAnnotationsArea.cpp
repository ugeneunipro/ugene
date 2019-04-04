/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2SafePoints.h>

#include <U2View/SequenceObjectContext.h>

#include "ov_assembly/AssemblyBrowser.h"
#include "ov_assembly/AssemblyModel.h"

#include "AssemblyAnnotationsArea.h"
#include "AssemblyAnnotationsAreaWidget.h"

namespace U2 {

AssemblyAnnotationsArea::AssemblyAnnotationsArea(AssemblyBrowserUi* _ui)
                      : browserUi(_ui),
                        seqCtx(nullptr) {
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
    vertLayout = new QVBoxLayout(this);
    setLayout(vertLayout);
    vertLayout->setMargin(0);
    vertLayout->setSpacing(0);
    connectSignals();
}

AssemblyAnnotationsArea::~AssemblyAnnotationsArea() {
    delete widget;
}

void AssemblyAnnotationsArea::sl_contextChanged(SequenceObjectContext* ctx) {
    delete widget;
    seqCtx = ctx;
    connect(seqCtx, SIGNAL(si_annotationObjectAdded(AnnotationTableObject *)), SLOT(sl_createWidget()));
}

void AssemblyAnnotationsArea::sl_createWidget() {
    CHECK(nullptr == widget, );

    AssemblyBrowser* browser = browserUi->getWindow();
    SAFE_POINT(nullptr != browser, "Assembly Browser is missed", );

    widget = new AssemblyAnnotationsAreaWidget(browser, browserUi, seqCtx);
    vertLayout->addWidget(widget);
}

void AssemblyAnnotationsArea::connectSignals() {
    SAFE_POINT(nullptr != browserUi, "Assembly browser widget is missed", );

    AssemblyModel* model = browserUi->getModel().data();
    SAFE_POINT(nullptr != model, "Annotation model is missed", );

    connect(model, SIGNAL(si_contextChanged(SequenceObjectContext*)), SLOT(sl_contextChanged(SequenceObjectContext*)));
}

} // U2
