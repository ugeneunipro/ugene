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

#include <U2Gui/ShowHideSubgroupWidget.h>

#include "McaGeneralTab.h"
#include "ov_msa/McaEditor.h"

namespace U2 {

McaGeneralTab::McaGeneralTab(McaEditor *mca)
    : mca(mca) {
    setupUi(this);

    ShowHideSubgroupWidget* alignmentInfo = new ShowHideSubgroupWidget("ALIGNMENT_INFO", tr("Alignment info"),
                                                                       alignmentInfoWgt, true);

    Ui_McaGeneralTab::verticalLayout->addWidget(alignmentInfo);

    lengthLabel->setText(QString::number(mca->getAlignmentLen()));
    seqNumLabel->setText(QString::number(mca->getNumSequences()));

    connect(mca->getMaObject(),
            SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)),
            SLOT(sl_alignmentChanged()));
}

void McaGeneralTab::sl_alignmentChanged() {
    lengthLabel->setText(QString::number(mca->getAlignmentLen()));
    seqNumLabel->setText(QString::number(mca->getNumSequences()));
}

} // namespace
