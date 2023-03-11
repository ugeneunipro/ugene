/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QWidget>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>

#include "../ov_msa/MaEditorConsensusArea.h"

namespace U2 {

class MsaEditorWgt;

class U2VIEW_EXPORT MSAEditorConsensusArea : public MaEditorConsensusArea {
    Q_OBJECT
    Q_DISABLE_COPY(MSAEditorConsensusArea)
public:
    MSAEditorConsensusArea(MsaEditorWgt* ui);

    QString getDefaultAlgorithmId() const override {
        return BuiltInConsensusAlgorithms::DEFAULT_ALGO;
    }

protected:
    QString getConsensusPercentTip(int pos, int minReportPercent, int maxReportChars) const override;
    void initRenderer() override;
    QString getLastUsedAlgoSettingsKey() const override;

private:
    void buildMenu(QMenu* menu);

private slots:
    void sl_buildMenu(GObjectViewController* view, QMenu* menu, const QString& type);
};

}  // namespace U2
