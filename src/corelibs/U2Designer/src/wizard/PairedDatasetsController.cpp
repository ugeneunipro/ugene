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

#include <U2Core/U2SafePoints.h>

#include "PairedDatasetsController.h"
#include "WizardController.h"

namespace U2 {

PairedDatasetsController::PairedDatasetsController(WizardController *wc, PairedReadsWidget *_widget)
: WidgetController(wc), widget(_widget), dsc(NULL)
{

}

PairedDatasetsController::~PairedDatasetsController() {
    delete dsc;
}

QWidget * PairedDatasetsController::createGUI(U2OpStatus &os) {
    if (2 != widget->getInfos().size()) {
        os.setError("Only 2 datasets are supported");
        return NULL;
    }

    AttributeInfo info1 = widget->getInfos().at(0);
    AttributeInfo info2 = widget->getInfos().at(1);
    QVariant value1 = wc->getAttributeValue(info1);
    QVariant value2 = wc->getAttributeValue(info2);
    QList<Dataset> sets1 = value1.value< QList<Dataset> >();
    QList<Dataset> sets2 = value2.value< QList<Dataset> >();

    if (NULL != dsc) {
        delete dsc;
    }
    dsc = new PairedReadsController(sets1, sets2, info1.hints[AttributeInfo::LABEL].toString(), info2.hints[AttributeInfo::LABEL].toString());
    connect(dsc, SIGNAL(si_attributeChanged()), SLOT(sl_datasetsChanged()));
    return dsc->getWigdet();
}

void PairedDatasetsController::sl_datasetsChanged() {
    CHECK(2 == widget->getInfos().size(), );
    wc->setAttributeValue(widget->getInfos().at(0), qVariantFromValue< QList<Dataset> >(dsc->getDatasets(0)));
    wc->setAttributeValue(widget->getInfos().at(1), qVariantFromValue< QList<Dataset> >(dsc->getDatasets(1)));
}

} // U2
