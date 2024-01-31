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

#include <U2Core/MsaObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/MaEditor.h>
#include <U2View/MaEditorWgt.h>

#include "McaEditorWgt.h"

namespace U2 {

class McaEditor;
class McaEditorSelectionController;
class SequenceObjectContext;

#define MCAE_MENU_ALIGNMENT "MCAE_MENU_ALIGNMENT"
#define MCAE_MENU_APPEARANCE "MCAE_MENU_APPEARANCE"
#define MCAE_MENU_NAVIGATION "MCAE_MENU_NAVIGATION"
#define MCAE_MENU_EDIT "MCAE_MENU_EDIT"

#define MCAE_SETTINGS_SHOW_CHROMATOGRAMS "show_chromatograms"
#define MCAE_SETTINGS_SHOW_OVERVIEW "show_overview"
#define MCAE_SETTINGS_PEAK_HEIGHT "peak_height"
#define MCAE_SETTINGS_CONSENSUS_TYPE "consensus_type"

class U2VIEW_EXPORT McaEditor : public MaEditor {
    Q_OBJECT
    friend class McaEditorSequenceArea;

public:
    McaEditor(const QString& viewName,
              MsaObject* obj);

    QString getSettingsRoot() const override {
        return MCAE_SETTINGS_ROOT;
    }

    McaEditorWgt* getUI() const;

    /** Returns current MCA editor selection controller instance. */
    MaEditorSelectionController* getSelectionController() const override;

    void buildStaticToolbar(QToolBar* tb) override;

    void buildMenu(QMenu* menu, const QString& type) override;

    int getRowContentIndent(int rowId) const override;

    bool isChromatogramRowExpanded(int rowIndex) const;

    QString getReferenceRowName() const override;

    char getReferenceCharAt(int pos) const override;

    SequenceObjectContext* getReferenceContext() const;

    MaEditorWgt* getLineWidget(int index) const override {
        SAFE_POINT(index == 0, "Calling getLineWidget(index) with index > 0 is prohibited for Mca", nullptr);
        return ui;
    }

protected slots:
    void sl_onContextMenuRequested(const QPoint& pos) override;
    void sl_showHideChromatograms(bool show);

private slots:
    void sl_showGeneralTab();
    void sl_showConsensusTab();

    void sl_saveOverviewState();
    void sl_saveChromatogramState();

protected:
    QWidget* createViewWidget(QWidget* parent) override;
    void initActions() override;

    QAction* showChromatogramsAction = nullptr;
    QAction* showGeneralTabAction = nullptr;
    QAction* showConsensusTabAction = nullptr;

    QMap<qint64, bool> chromVisibility;

    SequenceObjectContext* referenceCtx = nullptr;

    /** Selection state controller. */
    McaEditorSelectionController* selectionController = nullptr;

    McaEditorWgt* ui = nullptr;

    void addEditMenu(QMenu* menu) override;
    void addAlignmentMenu(QMenu* menu);
    void addAppearanceMenu(QMenu* menu);
    void addNavigationMenu(QMenu* menu);
};

}  // namespace U2
