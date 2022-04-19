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

#include "McaEditorOverviewArea.h"

#include "overview/MaSangerOverview.h"
#include "ov_msa/MaEditorWgt.h"

namespace U2 {

const QString McaEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME = "mca_overview_area";

McaEditorOverviewArea::McaEditorOverviewArea(MaEditorWgt *ui)
    : MaEditorOverviewArea((QWidget *)ui, OVERVIEW_AREA_OBJECT_NAME)
{
    // The McaEditorOverview can be resized vertically.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    sangerOverview = new MaSangerOverview(ui->getEditor(), ui);
    sangerOverview->setObjectName(OVERVIEW_AREA_OBJECT_NAME + QString("_sanger"));

    addOverview(sangerOverview);
}

}  // namespace U2
