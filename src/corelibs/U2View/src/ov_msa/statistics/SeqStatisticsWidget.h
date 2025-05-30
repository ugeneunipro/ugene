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

#include "../MsaOpSavableTab.h"
#include "ui_MsaDistancesColumnSettingsWidget.h"

namespace U2 {

class MsaEditor;
class MsaEditorWgt;
class SimilarityStatisticsSettings;

class U2VIEW_EXPORT SeqStatisticsWidget : public QWidget {
    Q_OBJECT
public:
    SeqStatisticsWidget(MsaEditor* msa);

private slots:
    void sl_onAlgoChanged();
    void sl_onGapsChanged(int state);
    void sl_onUnitsChanged(bool);
    void sl_onUpdateClicked();
    void sl_onShowStatisticsChanged(int);
    void sl_onAutoUpdateChanged(int);
    void sl_onRefSeqChanged(qint64);

private:
    void copySettings();
    void connectSlots();

    void hideSimilaritySettings();
    void showSimilaritySettings();
    void restoreSettings();

    void updateWidgetsSettings();

private:
    Ui_MsaDistanceColumnSettingsForm ui;

    MsaEditor* msa;

    QWidget* distancesStatisticsGroup;

    SimilarityStatisticsSettings* settings;
    bool statisticsIsShown;

    MsaOpSavableTab savableTab;
};

}  // namespace U2
