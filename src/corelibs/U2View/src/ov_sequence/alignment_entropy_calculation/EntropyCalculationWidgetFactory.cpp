/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "EntropyCalculationWidgetFactory.h"

#include <QPixmap>

#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>

#include <U2View/AnnotatedDNAView.h>

#include "EntropyCalculationWidget.h"

namespace U2 {

const QString EntropyCalculationWidgetFactory::GROUP_ID = "OP_ENTROPY_CALCULATION";
//TODO: add icon & doc page
const QString EntropyCalculationWidgetFactory::GROUP_ICON_STR = ""; 
const QString EntropyCalculationWidgetFactory::GROUP_DOC_PAGE = "";

EntropyCalculationWidgetFactory::EntropyCalculationWidgetFactory() {
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget* EntropyCalculationWidgetFactory::createWidget(GObjectView* objView, const QVariantMap& /*options*/) {
    SAFE_POINT(objView != nullptr, L10N::nullPointerError("GObjectView"), nullptr);

    AnnotatedDNAView* annotatedDnaView = qobject_cast<AnnotatedDNAView*>(objView);
    SAFE_POINT(annotatedDnaView != nullptr, L10N::nullPointerError("AnnotatedDNAView"), nullptr);

    EntropyCalculationWidget* widget = new EntropyCalculationWidget(annotatedDnaView);
    return widget;
}

OPGroupParameters EntropyCalculationWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Alignment Entropy Calculation"), GROUP_DOC_PAGE);
}

const QString& EntropyCalculationWidgetFactory::getGroupId() {
    return GROUP_ID;
}

}  // namespace U2
