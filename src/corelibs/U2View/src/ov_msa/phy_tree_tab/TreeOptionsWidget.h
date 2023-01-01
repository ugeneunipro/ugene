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

#ifndef _U2_TREE_OPTIONS_WIDGET_H_
#define _U2_TREE_OPTIONS_WIDGET_H_

#include <U2Gui/U2SavableWidget.h>

#include "ov_phyltree/TreeSettings.h"
#include "ui_TreeOptionsWidget.h"

namespace U2 {

class MSAEditor;
class SimilarityStatisticsSettings;
class ShowHideSubgroupWidget;
class TreeViewer;
class TreeViewerUI;
class MultipleAlignment;
class MaModificationInfo;

class TreeOptionsSavableWidget : public U2SavableWidget {
public:
    TreeOptionsSavableWidget(QWidget* wrappedWidget, MWMDIWindow* contextWindow = nullptr);
    ~TreeOptionsSavableWidget() override;

    void disableSavingForWidgets(const QStringList& s);

protected:
    bool childCanBeSaved(QWidget* child) const override;

private:
    QStringList widgetsNotToSave;
};

class U2VIEW_EXPORT TreeOptionsWidget : public QWidget, private Ui_TreeOptionWidget {
    Q_OBJECT
public:
    TreeOptionsWidget(TreeViewer* tree);
    TreeOptionsWidget(MSAEditor* msaEditor);
    ~TreeOptionsWidget() override;

private slots:
    void sl_labelsColorButton();
    void sl_branchesColorButton();
    void sl_fontTypeChanged();
    void sl_fontSizeChanged();
    void sl_fontBoldChanged();
    void sl_fontItalicChanged();
    void sl_fontUnderlineChanged();

    void sl_valueChanged();

    void sl_onOptionChanged(const TreeViewOption& option, const QVariant& value);

private:
    QStringList getSaveDisabledWidgets() const;
    void initializeOptionsMap();
    void initColorButtonsStyle();
    void createGroups();
    void createGeneralSettingsWidgets();
    void updateFormatSettings();

    /** Synchronizes current options panel with the active tree widget settings. */
    void updateAllWidgets();

    void connectSlots();

    void updateButtonColor(QPushButton* button, const QColor& newColor);

    TreeViewerUI* getTreeViewer() const;

    MSAEditor* editor = nullptr;
    TreeViewerUI* treeViewer = nullptr;
    QWidget* contentWidget = nullptr;

    TreeOptionsSavableWidget savableTab;

    QMap<QString, TreeViewOption> optionsMap;

    ShowHideSubgroupWidget* generalOpGroup = nullptr;
    ShowHideSubgroupWidget* labelsOpGroup = nullptr;
    ShowHideSubgroupWidget* branchesOpGroup = nullptr;
    ShowHideSubgroupWidget* nodesOpGroup = nullptr;
    ShowHideSubgroupWidget* scalebarOpGroup = nullptr;
};

class U2VIEW_EXPORT AddTreeWidget : public QWidget {
    Q_OBJECT
public:
    AddTreeWidget(MSAEditor* msaEditor);

private:
    MSAEditor* editor;
    QPushButton* openTreeButton;
    QPushButton* buildTreeButton;
    QLabel* addTreeHint;
};

}  // namespace U2

#endif
