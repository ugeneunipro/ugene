/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GenecutOPWidgetFactory.h"
#include "GenecutOPWidget.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotatedDNAView.h>

namespace U2 {

const QString GenecutOPWidgetFactory::GROUP_ID = "OP_GENECUT";
const QString GenecutOPWidgetFactory::GROUP_ICON_STR = ":genecut/icon/genecut.png";
const QString GenecutOPWidgetFactory::GROUP_DOC_PAGE = "88080434";

GenecutOPWidgetFactory::GenecutOPWidgetFactory()
    : OPWidgetFactory() {
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget* GenecutOPWidgetFactory::createWidget(GObjectView* objView, const QVariantMap& ) {
    auto annotatedDnaView = qobject_cast<AnnotatedDNAView*>(objView);
    SAFE_POINT(annotatedDnaView != nullptr, L10N::nullPointerError("AnnotatedDNAView"), nullptr);

    auto opWidget = new GenecutOPWidget(annotatedDnaView);
    opWidget->setObjectName("GenecurOpInnerWidget");
    return opWidget;
}

OPGroupParameters GenecutOPWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), tr("Genecut desktop"), GROUP_DOC_PAGE);
}

bool GenecutOPWidgetFactory::passFiltration(OPFactoryFilterVisitorInterface* filter) {
    CHECK(OPWidgetFactory::passFiltration(filter), false);

    return AppContext::getAppSettings()->getUserAppsSettings()->isExperimentalFeaturesModeEnabled();
}

}
