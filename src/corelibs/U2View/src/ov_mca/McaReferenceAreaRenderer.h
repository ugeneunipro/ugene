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

#pragma once

#include <U2View/PanViewRenderer.h>

namespace U2 {

class MaEditor;

class U2VIEW_EXPORT McaReferenceAreaRenderer : public PanViewRenderer {
    Q_OBJECT
public:
    McaReferenceAreaRenderer(PanView* panView, SequenceObjectContext* ctx, MaEditor* maEditor);

    int getMinimumHeight() const override;

    int posToXCoord(const qint64 position, const QSize& canvasSize, const U2Region& visibleRange) const override;

    void setFont(const QFont& font);

private:
    void drawSequence(QPainter& p, const QSize& canvasSize, const U2Region& region) override;

    MaEditor* maEditor;
};

}  // namespace U2
