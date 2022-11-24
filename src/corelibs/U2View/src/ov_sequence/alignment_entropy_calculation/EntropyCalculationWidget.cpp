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

#include "EntropyCalculationWidget.h"

#include <U2Gui/ShowHideSubgroupWidget.h>

#include <U2Core/L10n.h>

#include <U2View/AnnotatedDNAView.h>

namespace U2 {

EntropyCalculationWidget::EntropyCalculationWidget(AnnotatedDNAView* _annotatedDnaView)
    : annotatedDnaView(_annotatedDnaView) {
    setupUi(this);
    initLayout();
}

void EntropyCalculationWidget::initLayout() {
    additionalSettingsLayout->addWidget(new ShowHideSubgroupWidget(
        QObject::tr("Additional settings"), QObject::tr("Additional settings"), additionalSettingsWidget, true));
}

}  // namespace U2
