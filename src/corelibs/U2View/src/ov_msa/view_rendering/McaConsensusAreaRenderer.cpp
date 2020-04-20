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

#include "McaConsensusAreaRenderer.h"

#include "ov_msa/McaEditor.h"
#include "ov_msa/McaEditorConsensusArea.h"
#include "ov_msa/McaReferenceCharController.h"
#include "ov_msa/helpers/BaseWidthController.h"


namespace U2 {

McaConsensusAreaRenderer::McaConsensusAreaRenderer(MaEditorConsensusArea *area)
    : MaConsensusAreaRenderer(area) {
    McaEditorWgt* wgt = qobject_cast<McaEditorWgt*>(area->getEditorWgt());
    SAFE_POINT(wgt != NULL, "McaEditorWgt is NULL", );
    refCharController = wgt->getRefCharController();
}

void McaConsensusAreaRenderer::drawRuler(QPainter &painter, const ConsensusRenderSettings &renderSettings) {
    McaEditorConsensusArea* mcaConsArea = qobject_cast<McaEditorConsensusArea*>(area);
    SAFE_POINT(mcaConsArea != NULL, "Failed to cast consensus area to MCA consensus area", );
    McaEditorWgt * wgt = qobject_cast<McaEditorWgt *>(mcaConsArea->getEditorWgt());
    SAFE_POINT(wgt != NULL, "Failed to cast!", );
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

} // namespace
