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

#include "PCRPrimerDesignForDNAAssemblyOPWidgetFactory.h"
#include "PCRPrimerDesignForDNAAssemblyOPWidget.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotatedDNAView.h>

namespace U2 {

const QString PCRPrimerDesignForDNAAssemblyOPWidgetFactory::GROUP_ID = "OP_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY";
const QString PCRPrimerDesignForDNAAssemblyOPWidgetFactory::GROUP_ICON_STR = ":/pcr_primer_design_for_dna_assembly/icon/pcr_primer_design_icon.png";
const QString PCRPrimerDesignForDNAAssemblyOPWidgetFactory::GROUP_DOC_PAGE = "71958581";

PCRPrimerDesignForDNAAssemblyOPWidgetFactory::PCRPrimerDesignForDNAAssemblyOPWidgetFactory()
    : OPWidgetFactory() {
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget* PCRPrimerDesignForDNAAssemblyOPWidgetFactory::createWidget(GObjectView* objView, const QVariantMap& ) {
    auto annotatedDnaView = qobject_cast<AnnotatedDNAView*>(objView);
    SAFE_POINT(annotatedDnaView != nullptr, L10N::nullPointerError("AnnotatedDNAView"), nullptr);

    auto opWidget = new PCRPrimerDesignForDNAAssemblyOPWidget(annotatedDnaView);
    opWidget->setObjectName("PcrPrimerDesignOpInnerWidget");
    return opWidget;
}

OPGroupParameters PCRPrimerDesignForDNAAssemblyOPWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), tr("PCR Primer Design for DNA assembly"), GROUP_DOC_PAGE);
}

bool PCRPrimerDesignForDNAAssemblyOPWidgetFactory::passFiltration(OPFactoryFilterVisitorInterface* filter) {
    SAFE_POINT(filter != nullptr, L10N::nullPointerError("Options Panel Filter"), false);

    return filter->typePass(getObjectViewType()) && filter->atLeastOneDnaPass();
}

}
