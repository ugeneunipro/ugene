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

#include "CircularViewSettingsWidgetFactory.h"
#include "CircularViewSettingsWidget.h"
#include "CircularViewPlugin.h"

#include <U2View/AnnotatedDNAView.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString CircularViewSettingsWidgetFactory::GROUP_ID = "OP_CV_SETTINGS";
const QString CircularViewSettingsWidgetFactory::GROUP_ICON_STR  = ":circular_view/images/circular.png";
const QString CircularViewSettingsWidgetFactory::GROUP_DOC_PAGE = "24748797";

CircularViewSettingsWidgetFactory::CircularViewSettingsWidgetFactory(CircularViewContext* context)
    : ctx(context) {
    SAFE_POINT(context != NULL, tr("Circular view context is NULL"), );
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget* CircularViewSettingsWidgetFactory::createWidget(GObjectView *objView) {
    SAFE_POINT(objView != NULL, tr("Object view is NULL"), NULL);

    CircularViewSplitter *cvSplitter = ctx->getView(objView, false);
    AnnotatedDNAView* annotatedDnaView = qobject_cast<AnnotatedDNAView*>(objView);
    SAFE_POINT(annotatedDnaView != NULL, "Can not cast GObjectView to AnnotatedDNAView", NULL);
    CircularViewSettingsWidget* widget = new CircularViewSettingsWidget(ctx->getSettings(annotatedDnaView), cvSplitter);
    connect(ctx, SIGNAL(si_cvSplitterWasCreatedOrRemoved(CircularViewSplitter*, CircularViewSettings*)),
            widget, SLOT(sl_cvSplitterWasCreatedOrRemoved(CircularViewSplitter*, CircularViewSettings*)));
    connect(widget, SIGNAL(si_openCvButtonClicked(CircularViewSettings*)),
            ctx, SLOT(sl_toggleBySettings(CircularViewSettings*)));
    widget->setObjectName("CircularViewSettingsWidget");
    return widget;
}

OPGroupParameters CircularViewSettingsWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Circular View Settings"), GROUP_DOC_PAGE);
}

bool CircularViewSettingsWidgetFactory::passFiltration(OPFactoryFilterVisitorInterface *filter) {
    bool res = false;
    SAFE_POINT( filter != NULL, "OPFactoryFilterVisitorInterface::filter is NULL", res);

    res = filter->typePass(getObjectViewType()) && filter->atLeastOneAlphabetPass(DNAAlphabet_NUCL);
    return res;
}

} // namespace
