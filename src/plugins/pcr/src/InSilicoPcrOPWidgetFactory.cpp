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

#include "InSilicoPcrOPWidgetFactory.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotatedDNAView.h>

#include "InSilicoPcrOptionPanelWidget.h"

namespace U2 {

const QString InSilicoPcrOPWidgetFactory::GROUP_DOC_PAGE = "49448494";

InSilicoPcrOPWidgetFactory::InSilicoPcrOPWidgetFactory()
    : OPWidgetFactory() {
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget *InSilicoPcrOPWidgetFactory::createWidget(GObjectView *objView, const QVariantMap &options) {
    Q_UNUSED(options);

    AnnotatedDNAView *annotatedDnaView = qobject_cast<AnnotatedDNAView *>(objView);
    SAFE_POINT(annotatedDnaView != nullptr, L10N::nullPointerError("AnnotatedDNAView"), nullptr);

    InSilicoPcrOptionPanelWidget *opWidget = new InSilicoPcrOptionPanelWidget(annotatedDnaView);
    opWidget->setObjectName("InSilicoPcrOptionPanelWidget");
    return opWidget;
}

OPGroupParameters InSilicoPcrOPWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters("OP_IN_SILICO_PCR", QPixmap(":/primer3/images/primer3.png"), tr("In Silico PCR"), GROUP_DOC_PAGE);
}

bool InSilicoPcrOPWidgetFactory::passFiltration(OPFactoryFilterVisitorInterface *filter) {
    SAFE_POINT(filter != NULL, L10N::nullPointerError("Options Panel Filter"), false);

    return filter->typePass(getObjectViewType()) && filter->atLeastOneAlphabetPass(DNAAlphabet_NUCL);
}

}    // namespace U2
