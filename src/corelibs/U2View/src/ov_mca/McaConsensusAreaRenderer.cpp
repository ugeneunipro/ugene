/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "McaConsensusAreaRenderer.h"

#include "ov_mca/McaEditor.h"
#include "ov_mca/McaEditorConsensusArea.h"
#include "ov_mca/McaReferenceCharController.h"
#include "ov_msa/BaseWidthController.h"

namespace U2 {

McaConsensusAreaRenderer::McaConsensusAreaRenderer(MaEditorConsensusArea* area)
    : MaConsensusAreaRenderer(area) {
    auto wgt = qobject_cast<McaEditorWgt*>(area->getEditorWgt());
    SAFE_POINT(wgt != nullptr, "McaEditorWgt is NULL", );
    refCharController = wgt->getRefCharController();
}

void McaConsensusAreaRenderer::drawRuler(QPainter& painter, const ConsensusRenderSettings& renderSettings) {
    auto mcaConsArea = qobject_cast<McaEditorConsensusArea*>(area);
    SAFE_POINT(mcaConsArea != nullptr, "Failed to cast consensus area to MCA consensus area", );
    auto wgt = qobject_cast<McaEditorWgt*>(mcaConsArea->getEditorWgt());
    SAFE_POINT(wgt != nullptr, "Failed to cast!", );
    OffsetRegions charRegions = refCharController->getCharRegions(U2Region(renderSettings.firstNotchedBasePosition,
                                                                           renderSettings.lastNotchedBasePosition - renderSettings.firstNotchedBasePosition + 1));
    ConsensusRenderSettings cutRenderSettings = renderSettings;
    for (int i = 0; i < charRegions.getSize(); i++) {
        U2Region r = charRegions.getRegion(i);
        cutRenderSettings.firstNotchedBasePosition = r.startPos - charRegions.getOffset(i);
        cutRenderSettings.lastNotchedBasePosition = r.endPos() - 1 - charRegions.getOffset(i);

        cutRenderSettings.firstNotchedBaseXRange = ui->getBaseWidthController()->getBaseScreenRange(r.startPos);
        cutRenderSettings.lastNotchedBaseXRange = ui->getBaseWidthController()->getBaseScreenRange(r.endPos() - 1);
        MaConsensusAreaRenderer::drawRuler(painter, cutRenderSettings);
    }
}

}  // namespace U2
