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

#ifndef _U2_MA_CONSENSUS_AREA_RENDERER_H_
#define _U2_MA_CONSENSUS_AREA_RENDERER_H_

#include <QBitArray>

#include <U2Core/U2Region.h>

#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorConsensusAreaSettings.h"

class QPainter;

namespace U2 {

class MsaColorScheme;
class MaEditorConsensusArea;
class MSAEditorConsensusCache;

class ConsensusRenderSettings {
public:
    ConsensusRenderSettings();

    U2Region                            xRangeToDrawIn;
    QMap<MaEditorConsElement, U2Region> yRangeToDrawIn;

    int                     columnWidth;
    QFont                   font;
    QFont                   rulerFont;
    bool                    drawSelection;
    MsaColorScheme*         colorScheme;
    MaEditor::ResizeMode    resizeMode;
    bool                    highlightMismatches;

    int rulerWidth;
    int firstNotchedBasePosition;
    int lastNotchedBasePosition;
    U2Region firstNotchedBaseXRange;
    U2Region lastNotchedBaseXRange;
};

class ConsensusRenderData {
public:
    bool isValid() const;

    U2Region        region;
    U2Region        selectedRegion;
    QByteArray      data;
    QBitArray       mismatches;
    QList<int>      percentage;
};

class ConsensusCharRenderData {
public:
    ConsensusCharRenderData();

    QRect getCharRect() const;

    U2Region    xRange;
    U2Region    yRange;
    int         column;
    char        consensusChar;
    bool        isMismatch;
    bool        isSelected;
};

class MaConsensusAreaRenderer : public QObject {
    Q_OBJECT
public:
    MaConsensusAreaRenderer(MaEditorConsensusArea *area);

    void drawContent(QPainter &painter);
    void drawContent(QPainter &painter,
                            const ConsensusRenderData &consensusRenderData,
                            const MaEditorConsensusAreaSettings &consensusSettings,
                            const ConsensusRenderSettings &renderSettings);

    ConsensusRenderData getConsensusRenderData(const QList<int> &seqIdx, const U2Region &region) const;
    ConsensusRenderSettings getRenderSettigns(const U2Region &region, const MaEditorConsensusAreaSettings &consensusSettings) const;

    int getHeight() const;
    int getHeight(const MaEditorConsElements &visibleElements) const;
    U2Region getYRange(const MaEditorConsElements &visibleElements, MaEditorConsElement element) const;
    U2Region getYRange(MaEditorConsElement element) const;

protected:
    static void drawConsensus(QPainter &painter, const ConsensusRenderData &consensusRenderData, const ConsensusRenderSettings &settings);
    static void drawConsensusChar(QPainter &painter, const ConsensusCharRenderData& charData, const ConsensusRenderSettings &settings);
    virtual void drawRuler(QPainter &painter, const ConsensusRenderSettings &settings);
    static void drawHistogram(QPainter &painter, const ConsensusRenderData &consensusRenderData, const ConsensusRenderSettings &settings);

    ConsensusRenderData getScreenDataToRender() const;
    ConsensusRenderSettings getScreenRenderSettings(const MaEditorConsensusAreaSettings &consensusSettings) const;

    int getYRangeLength(MaEditorConsElement element) const;

    MaEditor *editor;
    MaEditorWgt *ui;
    MaEditorConsensusArea *area;

    static const QColor DEFAULT_MISMATCH_COLOR;
};

}   // namespace U2

#endif // _U2_MA_CONSENSUS_AREA_RENDERER_H_
