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

#include <U2View/ADVSequenceWidget.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2Core/DNASequenceSelection.h>

#include "SetSequenceOriginDialog.h"

namespace U2 {

SetSequenceOriginDialog::SetSequenceOriginDialog(ADVSequenceWidget* parent ) : QDialog(parent), seqContext(parent->getActiveSequenceContext())
{
    setupUi(this);
    seqOriginBox->setMinimum(1);
    seqOriginBox->setMaximum(seqContext->getSequenceLength());
    seqOriginBox->selectAll(); // allow user to start typing or copy-paste without cleaning default '1' first.
    const QVector<U2Region>& selectedRegions = seqContext->getSequenceSelection()->getSelectedRegions();

    if (selectedRegions.size() > 0) {
        seqOriginBox->setValue(selectedRegions.first().startPos + 1);
    }

}

int SetSequenceOriginDialog::getSequenceShift()
{
    return seqOriginBox->value();
}



} // namespace

