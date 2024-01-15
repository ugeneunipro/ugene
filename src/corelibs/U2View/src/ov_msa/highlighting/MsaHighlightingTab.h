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

#include <U2Gui/U2SavableWidget.h>

#include "MsaHighlightingSavableTab.h"
#include "MsaSchemeComboBoxController.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QRadioButton;
class QSlider;
class QToolButton;

namespace U2 {

class MsaEditor;
class MaEditorSequenceArea;
class MsaColorSchemeFactory;
class MsaHighlightingSchemeFactory;
class MsaColorSchemeRegistry;
class MsaHighlightingSchemeRegistry;

class U2VIEW_EXPORT MsaHighlightingTab : public QWidget {
    Q_OBJECT
public:
    MsaHighlightingTab(MsaEditor* msa);

signals:
    void si_colorSchemeChanged();

private slots:
    void sl_sync();

    void sl_updateHint();
    void sl_updateColorSchemeWidgets();
    void sl_exportHighlightningClicked();
    void sl_colorParametersChanged();
    void sl_highlightingParametersChanged();
    void sl_refreshSchemes();

private:
    void initSeqArea();
    QWidget* createColorGroup();
    QWidget* createHighlightingGroup();

    MsaEditor* msa;
    MaEditorSequenceArea* seqArea;
    MsaSchemeComboBoxController<MsaColorSchemeFactory, MsaColorSchemeRegistry>* colorSchemeController;
    MsaSchemeComboBoxController<MsaHighlightingSchemeFactory, MsaHighlightingSchemeRegistry>* highlightingSchemeController;
    QLabel* hint;
    QCheckBox* useDots;
    QToolButton* exportHighlightning;

    QLabel* colorThresholdLabel;
    QSlider* colorThresholdSlider;
    QDoubleSpinBox* colorSpinBox;

    QLabel* thresholdLabel;
    QSlider* highlightingThresholdSlider;
    QLabel* lessMoreLabel;
    QRadioButton* thresholdLessRb;
    QRadioButton* thresholdMoreRb;

    MsaHighlightingSavableTab savableTab;
};

}  // namespace U2
