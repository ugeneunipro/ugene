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

#include "TreeOptionsWidget.h"

#include <QColorDialog>
#include <QLineEdit>
#include <QMainWindow>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QTextEdit>

#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Theme.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MSAEditor.h>

#include "ov_msa/phy_tree/MSAEditorMultiTreeViewer.h"
#include "ov_msa/phy_tree/MSAEditorTreeViewer.h"
#include "ov_phyltree/TreeViewer.h"
#include "phyltree/TreeSettingsDialog.h"

namespace U2 {

static inline QVBoxLayout* initLayout(QWidget* w) {
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    w->setLayout(layout);
    return layout;
}

TreeOptionsWidget::TreeOptionsWidget(MSAEditor* msaEditor)
    : editor(msaEditor),
      savableTab(this, GObjectViewUtils::findViewByName(msaEditor->getName())) {
    SAFE_POINT(editor != nullptr, QString("Invalid parameter were passed into constructor TreeOptionsWidget"), );
    setObjectName("TreeOptionsWidget");

    contentWidget = new QWidget();
    setupUi(contentWidget);

    initColorButtonsStyle();
    createGroups();
    savableTab.disableSavingForWidgets(getSaveDisabledWidgets());
    U2WidgetStateStorage::restoreWidgetState(savableTab);
    sl_selectionChanged();
}

TreeOptionsWidget::TreeOptionsWidget(TreeViewer* tree)
    : treeViewer(tree->getTreeViewerUI()),
      savableTab(this, GObjectViewUtils::findViewByName(tree->getName())) {
    SAFE_POINT(treeViewer != nullptr, "Invalid parameter were passed into constructor TreeOptionsWidget", );
    setObjectName("TreeOptionsWidget");

    contentWidget = new QWidget();
    setupUi(contentWidget);

    initColorButtonsStyle();
    createGroups();
    savableTab.disableSavingForWidgets(getSaveDisabledWidgets());
    U2WidgetStateStorage::restoreWidgetState(savableTab);
    sl_selectionChanged();
}

TreeOptionsWidget::~TreeOptionsWidget() {
    delete contentWidget;
}

void TreeOptionsWidget::initColorButtonsStyle() {
    QStyle* buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
    buttonStyle->setParent(this);
    labelsColorButton->setStyle(buttonStyle);
    branchesColorButton->setStyle(buttonStyle);
}

void TreeOptionsWidget::createGroups() {
    QVBoxLayout* mainLayout = initLayout(this);
    mainLayout->setSpacing(0);

    generalOpGroup = new ShowHideSubgroupWidget("TREE_GENERAL_OP", tr("General"), treeLayoutWidget, true);
    mainLayout->addWidget(generalOpGroup);

    labelsOpGroup = new ShowHideSubgroupWidget("TREE_LABELS_OP", tr("Labels"), labelsGroup, true);
    mainLayout->addWidget(labelsOpGroup);

    branchesOpGroup = new ShowHideSubgroupWidget("TREE_BRANCHES_OP", tr("Branches"), branchesGroup, true);
    mainLayout->addWidget(branchesOpGroup);

    scalebarOpGroup = new ShowHideSubgroupWidget("SCALEBAR_OP", tr("Scale Bar"), scalebarGroup, true);
    scalebarOpGroup->setVisible(false);
    mainLayout->addWidget(scalebarOpGroup);

    initializeOptionsMap();
    createGeneralSettingsWidgets();
    updateAllWidgets();
    connectSlots();
}

void TreeOptionsWidget::updateAllWidgets() {
    QMap<TreeViewOption, QVariant> settings = getTreeViewer()->getSettings();
    QList<TreeViewOption> keyList = settings.keys();
    for (const TreeViewOption& option : qAsConst(keyList)) {
        sl_onOptionChanged(option, settings[option]);
    }
    if (!settings[SHOW_NODE_LABELS].toBool()) {
        showNodeLabelsCheck->setEnabled(false);
    }
}

void TreeOptionsWidget::sl_onOptionChanged(TreeViewOption option, const QVariant& value) {
    if (option == SHOW_LABELS) {
        alignLabelsCheck->setEnabled(value.toBool());
    }
    if (option == LABEL_COLOR || option == LABEL_FONT_TYPE || option == LABEL_FONT_SIZE ||
        option == LABEL_FONT_BOLD || option == LABEL_FONT_ITALIC || option == LABEL_FONT_UNDERLINE) {
        updateFormatSettings();
        return;
    }
    if (option == BRANCH_COLOR) {
        updateButtonColor(branchesColorButton, qvariant_cast<QColor>(value));
        return;
    }
    updateRelatedOptionsState(option, value);

    QString objectName = optionsMap.key(option);
    CHECK(!objectName.isEmpty(), );
    isUpdating = true;
    savableTab.setChildValue(objectName, value);
    isUpdating = false;
}

void TreeOptionsWidget::sl_selectionChanged() {
    const QSignalBlocker fontSizeBlocker(fontSizeSpinBox);
    const QSignalBlocker fontComboBlocker(fontComboBox);

    auto treeViewerUi = getTreeViewer();
    fontComboBox->setCurrentFont(qvariant_cast<QFont>(treeViewerUi->getOptionValue(LABEL_FONT_TYPE)));
    fontSizeSpinBox->setValue(treeViewerUi->getOptionValue(LABEL_FONT_SIZE).toInt());
    boldAttrButton->setChecked(treeViewerUi->getOptionValue(LABEL_FONT_BOLD).toBool());
    italicAttrButton->setChecked(treeViewerUi->getOptionValue(LABEL_FONT_ITALIC).toBool());
    underlineAttrButton->setChecked(treeViewerUi->getOptionValue(LABEL_FONT_UNDERLINE).toBool());
    updateButtonColor(labelsColorButton, qvariant_cast<QColor>(treeViewerUi->getOptionValue(LABEL_COLOR)));
}

QStringList TreeOptionsWidget::getSaveDisabledWidgets() const {
    return {
        fontComboBox->objectName(),
        fontSizeSpinBox->objectName(),
        boldAttrButton->objectName(),
        italicAttrButton->objectName(),
        underlineAttrButton->objectName(),
        labelsColorButton->objectName()};
}

void TreeOptionsWidget::initializeOptionsMap() {
    // Scalebar settings widgets
    optionsMap[scaleSpinBox->objectName()] = SCALEBAR_RANGE;
    optionsMap[scaleFontSizeSpinBox->objectName()] = SCALEBAR_FONT_SIZE;
    optionsMap[lineWidthSpinBox->objectName()] = SCALEBAR_LINE_WIDTH;

    optionsMap[showNamesCheck->objectName()] = SHOW_LABELS;
    optionsMap[showDistancesCheck->objectName()] = SHOW_DISTANCES;
    optionsMap[alignLabelsCheck->objectName()] = ALIGN_LABELS;
    optionsMap[showNodeLabelsCheck->objectName()] = SHOW_NODE_LABELS;

    optionsMap[lineWeightSpinBox->objectName()] = BRANCH_THICKNESS;
    optionsMap[breadthScaleAdjustmentSlider->objectName()] = BREADTH_SCALE_ADJUSTMENT_PERCENT;
    optionsMap[curvatureSlider->objectName()] = BRANCH_CURVATURE;

    optionsMap[treeViewCombo->objectName()] = BRANCHES_TRANSFORMATION_TYPE;
    optionsMap[layoutCombo->objectName()] = TREE_LAYOUT;
}

void TreeOptionsWidget::connectSlots() {
    // General settings widgets
    connect(treeViewCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_valueChanged()));
    connect(layoutCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_valueChanged()));

    auto treeViewerUi = getTreeViewer();
    connect(treeViewerUi, SIGNAL(si_optionChanged(TreeViewOption, const QVariant&)), SLOT(sl_onOptionChanged(TreeViewOption, const QVariant&)));

    // Labels settings widgets
    connect(showNamesCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));
    connect(showDistancesCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));
    connect(alignLabelsCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));
    connect(showNodeLabelsCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));

    // Labels format widgets
    connect(labelsColorButton, SIGNAL(clicked()), SLOT(sl_labelsColorButton()));
    connect(boldAttrButton, SIGNAL(clicked(bool)), SLOT(sl_fontBoldChanged()));
    connect(italicAttrButton, SIGNAL(clicked(bool)), SLOT(sl_fontItalicChanged()));
    connect(underlineAttrButton, SIGNAL(clicked(bool)), SLOT(sl_fontUnderlineChanged()));
    connect(fontSizeSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_fontSizeChanged()));
    connect(fontComboBox, SIGNAL(currentFontChanged(const QFont&)), SLOT(sl_fontTypeChanged()));

    // Scalebar settings widgets
    connect(scaleSpinBox, SIGNAL(valueChanged(double)), SLOT(sl_valueChanged()));
    connect(scaleFontSizeSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));
    connect(lineWidthSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));

    // Branches settings widgets
    connect(breadthScaleAdjustmentSlider, &QSlider::valueChanged, this, &TreeOptionsWidget::sl_valueChanged);
    connect(curvatureSlider, &QSlider::valueChanged, this, &TreeOptionsWidget::sl_valueChanged);

    connect(branchesColorButton, SIGNAL(clicked()), SLOT(sl_branchesColorButton()));
    connect(lineWeightSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));

    connect(treeViewerUi, SIGNAL(si_updateBranch()), SLOT(sl_selectionChanged()));
    if (editor != nullptr) {
        auto multiTreeViewer = editor->getUI()->getMultiTreeViewer();
        SAFE_POINT(multiTreeViewer != nullptr, "Tree options widget is instantiated with no active tree view", );
        connect(multiTreeViewer, &MSAEditorMultiTreeViewer::si_activeTreeViewChanged, this, [this] { updateAllWidgets(); });
    }
}

void TreeOptionsWidget::sl_valueChanged() {
    auto inputWidget = qobject_cast<QWidget*>(sender());
    SAFE_POINT(inputWidget != nullptr, "sl_valueChanged: Sender is not a widget", );

    QVariant newValue = savableTab.getChildValue(inputWidget->objectName());
    TreeViewOption option = optionsMap[inputWidget->objectName()];
    if (option == SHOW_LABELS) {
        alignLabelsCheck->setEnabled(newValue.toBool());
    } else if (option == BRANCHES_TRANSFORMATION_TYPE) {
        scalebarOpGroup->setVisible(newValue.toInt() == TreeType::PHYLOGRAM);
    }
    updateRelatedOptionsState(option, newValue);

    CHECK(!isUpdating, );
    getTreeViewer()->changeOption(option, newValue);
}

void TreeOptionsWidget::createGeneralSettingsWidgets() {
    layoutCombo->addItems({tr("Rectangular"),
                           tr("Circular"),
                           tr("Unrooted")});
    treeViewCombo->addItems({TreeSettingsDialog::getDefaultTreeModeText(),
                             TreeSettingsDialog::getPhylogramTreeModeText(),
                             TreeSettingsDialog::getCladogramTreeModeText()});
}

void TreeOptionsWidget::updateFormatSettings() {
    // Update labels format settings widgets
    auto treeViewerUi = getTreeViewer();
    auto curColor = qvariant_cast<QColor>(treeViewerUi->getOptionValue(LABEL_COLOR));
    updateButtonColor(labelsColorButton, curColor);

    auto curFont = qvariant_cast<QFont>(treeViewerUi->getOptionValue(LABEL_FONT_TYPE));
    fontComboBox->setCurrentFont(curFont);

    fontSizeSpinBox->setValue(treeViewerUi->getOptionValue(LABEL_FONT_SIZE).toInt());
    boldAttrButton->setCheckable(true);
    italicAttrButton->setCheckable(true);
    underlineAttrButton->setCheckable(true);

    boldAttrButton->setChecked(treeViewerUi->getOptionValue(LABEL_FONT_BOLD).toBool());
    italicAttrButton->setChecked(treeViewerUi->getOptionValue(LABEL_FONT_ITALIC).toBool());
    underlineAttrButton->setChecked(treeViewerUi->getOptionValue(LABEL_FONT_UNDERLINE).toBool());
}

TreeViewerUI* TreeOptionsWidget::getTreeViewer() const {
    SAFE_POINT(editor != nullptr || treeViewer != nullptr, QString("Invalid parameter in constructor TreeOptionsWidget"), nullptr);
    if (treeViewer != nullptr) {
        return treeViewer;
    }
    MSAEditorTreeViewer* msaTreeViewer = editor->getUI()->getCurrentTree();
    SAFE_POINT(msaTreeViewer != nullptr, "MSAEditorTreeViewer not found", nullptr);
    return msaTreeViewer->getTreeViewerUI();
}

void TreeOptionsWidget::sl_fontTypeChanged() {
    getTreeViewer()->changeOption(LABEL_FONT_TYPE, fontComboBox->currentFont());
}

void TreeOptionsWidget::sl_fontSizeChanged() {
    getTreeViewer()->changeOption(LABEL_FONT_SIZE, fontSizeSpinBox->value());
}

void TreeOptionsWidget::sl_fontBoldChanged() {
    getTreeViewer()->changeOption(LABEL_FONT_BOLD, boldAttrButton->isChecked());
}

void TreeOptionsWidget::sl_fontItalicChanged() {
    getTreeViewer()->changeOption(LABEL_FONT_ITALIC, italicAttrButton->isChecked());
}

void TreeOptionsWidget::sl_fontUnderlineChanged() {
    getTreeViewer()->changeOption(LABEL_FONT_UNDERLINE, underlineAttrButton->isChecked());
}

void TreeOptionsWidget::sl_labelsColorButton() {
    auto treeViewerUi = getTreeViewer();
    auto curColor = qvariant_cast<QColor>(treeViewerUi->getOptionValue(LABEL_COLOR));
    auto newColor = QColorDialog::getColor(curColor, AppContext::getMainWindow()->getQMainWindow());
    if (newColor.isValid()) {
        updateButtonColor(labelsColorButton, newColor);
        treeViewerUi->changeOption(LABEL_COLOR, newColor);
    }
}

void TreeOptionsWidget::sl_branchesColorButton() {
    auto treeViewerUi = getTreeViewer();
    auto curColor = qvariant_cast<QColor>(treeViewerUi->getOptionValue(BRANCH_COLOR));
    auto newColor = QColorDialog::getColor(curColor, AppContext::getMainWindow()->getQMainWindow());
    if (newColor.isValid()) {
        updateButtonColor(branchesColorButton, newColor);
        treeViewerUi->changeOption(BRANCH_COLOR, newColor);
    }
}

void TreeOptionsWidget::updateButtonColor(QPushButton* button, const QColor& newColor) {
    QPalette palette = button->palette();
    palette.setColor(button->backgroundRole(), newColor);
    button->setPalette(palette);
}

void TreeOptionsWidget::updateRelatedOptionsState(const TreeViewOption& option, const QVariant& newValue) {
    if (option == BRANCHES_TRANSFORMATION_TYPE) {
        auto type = static_cast<TreeType>(newValue.toUInt());
        scalebarGroup->setEnabled(type == PHYLOGRAM);
    } else if (option == TREE_LAYOUT) {
        auto layout = static_cast<TreeLayout>(newValue.toUInt());
        breadthScaleAdjustmentSlider->setEnabled(layout == RECTANGULAR_LAYOUT);
        curvatureSlider->setEnabled(layout == RECTANGULAR_LAYOUT);
    }
}

AddTreeWidget::AddTreeWidget(MSAEditor* msaEditor)
    : editor(msaEditor), openTreeButton(nullptr), buildTreeButton(nullptr), addTreeHint(nullptr) {
    setObjectName("AddTreeWidget");
    QVBoxLayout* mainLayout = initLayout(this);
    mainLayout->setSpacing(0);

    addTreeHint = new QLabel(tr("There are no displayed trees so settings are hidden."), this);
    addTreeHint->setWordWrap(true);

    mainLayout->addWidget(addTreeHint);

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 5, 0, 0);

    openTreeButton = new QPushButton(QIcon(":ugene/images/advanced_open.png"), tr("Open tree"), this);
    openTreeButton->setFixedWidth(102);
    openTreeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    buttonLayout->addWidget(openTreeButton);
    openTreeButton->setObjectName("openTreeButton");

    auto horizontalSpacer = new QSpacerItem(50, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(horizontalSpacer);

    buildTreeButton = new QPushButton(QIcon(":core/images/phylip.png"), tr("Build tree"), this);
    buildTreeButton->setFixedWidth(102);
    buttonLayout->addWidget(buildTreeButton);
    buildTreeButton->setObjectName("buildTreeButton");
    buildTreeButton->setEnabled(msaEditor->buildTreeAction->isEnabled());

    connect(msaEditor->buildTreeAction, &QAction::changed, this, [this, msaEditor]() {
        buildTreeButton->setEnabled(msaEditor->buildTreeAction->isEnabled());
    });
    connect(buildTreeButton, &QPushButton::clicked, this, [msaEditor]() { msaEditor->buildTreeAction->trigger(); });

    mainLayout->addLayout(buttonLayout);

    connect(openTreeButton, &QPushButton::clicked, this, [&] { editor->getTreeManager()->openTreeFromFile(); });
}

TreeOptionsSavableWidget::TreeOptionsSavableWidget(QWidget* wrappedWidget, MWMDIWindow* contextWindow)
    : U2SavableWidget(wrappedWidget, contextWindow) {
}

TreeOptionsSavableWidget::~TreeOptionsSavableWidget() {
    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;
}

void TreeOptionsSavableWidget::disableSavingForWidgets(const QStringList& s) {
    widgetsNotToSave.append(s);
}

bool TreeOptionsSavableWidget::childCanBeSaved(QWidget* child) const {
    return !widgetsNotToSave.contains(child->objectName()) && U2SavableWidget::childCanBeSaved(child);
}

}  // namespace U2
