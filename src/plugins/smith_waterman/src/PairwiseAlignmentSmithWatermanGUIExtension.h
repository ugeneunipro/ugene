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

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QVariantMap>
#include <QWidget>

#include <U2View/AlignmentAlgorithmGUIExtension.h>

#include "SWAlgorithmTask.h"
#include "ui_PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.h"

namespace U2 {

class PairwiseAlignmentSmithWatermanMainWidget : public AlignmentAlgorithmMainWidget,
                                                 public Ui_PairwiseAlignmentSmithWatermanOptionsPanelMainWidget {
    Q_OBJECT

public:
    PairwiseAlignmentSmithWatermanMainWidget(QWidget* parent, QVariantMap* s);
    virtual ~PairwiseAlignmentSmithWatermanMainWidget();
    virtual QVariantMap getAlignmentAlgorithmCustomSettings(bool append);
    virtual void updateWidget();

private slots:
    void sl_viewMatrixClicked();

private:
    void initParameters();
    void addScoredMatrixes();
    virtual void fillInnerSettings();

protected:
    static const qint64 SW_MIN_GAP_OPEN = 1;
    static const qint64 SW_MAX_GAP_OPEN = 65535;
    static const qint64 SW_DEFAULT_GAP_OPEN = 10;  // http://www.ebi.ac.uk - default value is 10
    static const qint64 SW_MIN_GAP_EXTD = 1;
    static const qint64 SW_MAX_GAP_EXTD = 65535;  // it isn`t the maximum, it may be larger
    static const qint64 SW_DEFAULT_GAP_EXTD = 1;  // http://www.ebi.ac.uk - default value is 0.5
};

class PairwiseAlignmentSmithWatermanGUIExtensionFactory : public AlignmentAlgorithmGUIExtensionFactory {
    Q_OBJECT

public:
    PairwiseAlignmentSmithWatermanGUIExtensionFactory(SW_AlgType _algType);
    virtual AlignmentAlgorithmMainWidget* createMainWidget(QWidget* parent, QVariantMap* s);

private:
    SW_AlgType algType;
};

}  // namespace U2
