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

#ifndef _U2_MCA_EDITOR_CONSENSUS_AREA_H_
#define _U2_MCA_EDITOR_CONSENSUS_AREA_H_

#include <QWidget>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>

#include <U2View/MaEditorConsensusArea.h>

namespace U2 {

class MaConsensusMismatchController;
class McaEditorWgt;

class U2VIEW_EXPORT McaEditorConsensusArea : public MaEditorConsensusArea {
    Q_OBJECT
    Q_DISABLE_COPY(McaEditorConsensusArea)

public:
    McaEditorConsensusArea(McaEditorWgt *ui);

    QString getDefaultAlgorithmId() const override {
        return BuiltInConsensusAlgorithms::SIMPLE_EXTENDED_ALGO;
    }

    MaConsensusMismatchController *getMismatchController() {
        return mismatchController;
    }
    void buildStaticToolbar(QToolBar *tb);

protected:
    QString getConsensusPercentTip(int pos, int minReportPercent, int maxReportChars) const override;
    void initRenderer() override;
    bool highlightConsensusChar(int pos) override;
    QString getLastUsedAlgoSettingsKey() const override;

private:
    MaConsensusMismatchController *mismatchController;
};

}  // namespace U2
#endif  // _U2_MCA_EDITOR_CONSENSUS_AREA_H_
