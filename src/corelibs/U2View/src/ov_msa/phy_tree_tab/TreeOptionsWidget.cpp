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

#include "TreeOptionsWidget.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QTextEdit>

#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Theme.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MSAEditor.h>

#include "../../ov_phyltree/TreeViewerUtils.h"
#include "ov_msa/phy_tree/MSAEditorMultiTreeViewer.h"
#include "ov_msa/phy_tree/MSAEditorTreeViewer.h"
#include "ov_phyltree/TreeViewer.h"
#include "phyltree/TreeSettingsDialog.h"

namespace U2 {

TreeOptionsWidget::TreeOptionsWidget(MSAEditor* msaEditor)
    : editor(msaEditor),
      savableTab(this, GObjectViewUtils::findViewByName(msaEditor->getName())) {
    SAFE_POINT(editor != nullptr, QString("Invalid parameter were passed into constructor TreeOptionsWidget"), );
    init();
}

TreeOptionsWidget::TreeOptionsWidget(TreeViewer* tree)
    : treeViewer(tree->getTreeViewerUI()),
      savableTab(this, GObjectViewUtils::findViewByName(tree->getName())) {
    SAFE_POINT(treeViewer != nullptr, "Invalid parameter were passed into constructor TreeOptionsWidget", );
    init();
}

void TreeOptionsWidget::init() {
    setObjectName("TreeOptionsWidget");

    setupUi(this);
    new ResetSliderOnDoubleClickBehavior(curvatureSlider, curvatureLabel);
    new ResetSliderOnDoubleClickBehavior(breadthScaleAdjustmentSlider, breadthScaleAdjustmentLabel);

    initColorButtonsStyle();
    createGroups();

    initializeOptionsMap();
    createGeneralSettingsWidgets();
    updateAllWidgets();
    connectSlots();

    savableTab.disableSavingForWidgets(getSaveDisabledWidgets());
    U2WidgetStateStorage::restoreWidgetState(savableTab);
}

void TreeOptionsWidget::initColorButtonsStyle() {
    QStyle* buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
    buttonStyle->setParent(this);
    labelsColorButton->setStyle(buttonStyle);
    branchesColorButton->setStyle(buttonStyle);
}

void TreeOptionsWidget::createGroups() {
    auto mainLayout = verticalLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Clean the layout. We will re-populate it with groups below.
    for (int i = mainLayout->count(); --i >= 0;) {
        mainLayout->removeItem(mainLayout->itemAt(i));
    }

    generalOpGroup = new ShowHideSubgroupWidget("TREE_GENERAL_OP", tr("General"), treeLayoutWidget, true);
    mainLayout->addWidget(generalOpGroup);

    labelsOpGroup = new ShowHideSubgroupWidget("TREE_LABELS_OP", tr("Labels"), labelsGroup, true);
    mainLayout->addWidget(labelsOpGroup);

    branchesOpGroup = new ShowHideSubgroupWidget("TREE_BRANCHES_OP", tr("Branches"), branchesGroup, true);
    mainLayout->addWidget(branchesOpGroup);

    nodesOpGroup = new ShowHideSubgroupWidget("TREE_NODES_OP", tr("Nodes"), nodesGroup, true);
    mainLayout->addWidget(nodesOpGroup);

    scalebarOpGroup = new ShowHideSubgroupWidget("TREE_SCALEBAR_OP", tr("Scale Bar"), scalebarGroup, true);
    scalebarOpGroup->setVisible(false);
    mainLayout->addWidget(scalebarOpGroup);
}

void TreeOptionsWidget::updateAllWidgets() {
    QMap<TreeViewOption, QVariant> settings = getTreeViewer()->getSelectionSettings();
    QList<TreeViewOption> keyList = settings.keys();
    for (const TreeViewOption& option : qAsConst(keyList)) {
        sl_onOptionChanged(option, settings[option]);
    }
    showNodeLabelsCheck->setVisible(getTreeViewer()->phyObject->getTree()->hasNamedInnerNodes());
}

void TreeOptionsWidget::sl_onOptionChanged(const TreeViewOption& option, const QVariant& value) {
    switch (option) {
        case SHOW_LEAF_NODE_LABELS:
            showNamesCheck->setChecked(value.toBool());
            alignLabelsCheck->setEnabled(showNamesCheck->isChecked());
            break;
        case BRANCH_DEPTH_SCALE_MODE:
            treeViewCombo->setCurrentIndex(value.toInt());
            scalebarGroup->setEnabled(static_cast<TreeType>(value.toInt()) == PHYLOGRAM);
            break;
        case TREE_LAYOUT_TYPE: {
            auto layout = static_cast<TreeLayoutType>(value.toInt());
            layoutCombo->setCurrentIndex(layout);
            breadthScaleAdjustmentSlider->setEnabled(layout == RECTANGULAR_LAYOUT);
            curvatureSlider->setEnabled(layout == RECTANGULAR_LAYOUT);
            break;
        }
        case BREADTH_SCALE_ADJUSTMENT_PERCENT:
            breadthScaleAdjustmentSlider->setValue(value.toInt());
            break;
        case BRANCH_CURVATURE:
            curvatureSlider->setValue(value.toInt());
            break;
        case LABEL_COLOR:
        case LABEL_FONT_FAMILY:
        case LABEL_FONT_SIZE:
        case LABEL_FONT_BOLD:
        case LABEL_FONT_ITALIC:
        case LABEL_FONT_UNDERLINE:
            updateFormatSettings();
            break;
        case BRANCH_COLOR:
            updateButtonColor(branchesColorButton, qvariant_cast<QColor>(value));
            break;
        case BRANCH_THICKNESS:
            lineWeightSpinBox->setValue(value.toInt());
            break;
        case SHOW_NODE_SHAPE:
            showNodeShapeCheck->setChecked(value.toBool());
            break;
        case SHOW_TIP_SHAPE:
            showTipShapeCheck->setChecked(value.toBool());
            break;
        case NODE_COLOR:
        case NODE_RADIUS:
            // Not used.
            break;
        case SHOW_BRANCH_DISTANCE_LABELS:
            showDistancesCheck->setChecked(value.toBool());
            break;
        case SHOW_INNER_NODE_LABELS:
            showNodeLabelsCheck->setChecked(value.toBool());
            break;
        case ALIGN_LEAF_NODE_LABELS:
            alignLabelsCheck->setChecked(value.toBool());
            break;
        case SCALEBAR_RANGE:
            scaleSpinBox->setValue(value.toBool());
            break;
        case SCALEBAR_FONT_SIZE:
            scaleFontSizeSpinBox->setValue(value.toInt());
            break;
        case SCALEBAR_LINE_WIDTH:
            lineWidthSpinBox->setValue(value.toInt());
            break;
        default:
            break;
    }
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

    optionsMap[showNamesCheck->objectName()] = SHOW_LEAF_NODE_LABELS;
    optionsMap[showDistancesCheck->objectName()] = SHOW_BRANCH_DISTANCE_LABELS;
    optionsMap[alignLabelsCheck->objectName()] = ALIGN_LEAF_NODE_LABELS;
    optionsMap[showNodeLabelsCheck->objectName()] = SHOW_INNER_NODE_LABELS;
    optionsMap[showNodeShapeCheck->objectName()] = SHOW_NODE_SHAPE;
    optionsMap[showTipShapeCheck->objectName()] = SHOW_TIP_SHAPE;

    optionsMap[lineWeightSpinBox->objectName()] = BRANCH_THICKNESS;
    optionsMap[breadthScaleAdjustmentSlider->objectName()] = BREADTH_SCALE_ADJUSTMENT_PERCENT;
    optionsMap[curvatureSlider->objectName()] = BRANCH_CURVATURE;

    optionsMap[treeViewCombo->objectName()] = BRANCH_DEPTH_SCALE_MODE;
    optionsMap[layoutCombo->objectName()] = TREE_LAYOUT_TYPE;
}

void TreeOptionsWidget::connectSlots() {
    // General settings widgets.
    connect(treeViewCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_valueChanged()));
    connect(layoutCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_valueChanged()));

    auto treeViewerUi = getTreeViewer();
    connect(treeViewerUi, &TreeViewerUI::si_optionChanged, this, &TreeOptionsWidget::sl_onOptionChanged);

    // Labels settings widgets.
    connect(showNamesCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));
    connect(showDistancesCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));
    connect(alignLabelsCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));
    connect(showNodeLabelsCheck, SIGNAL(stateChanged(int)), SLOT(sl_valueChanged()));

    // Labels format widgets.
    connect(labelsColorButton, SIGNAL(clicked()), SLOT(sl_labelsColorButton()));
    connect(boldAttrButton, SIGNAL(clicked(bool)), SLOT(sl_fontBoldChanged()));
    connect(italicAttrButton, SIGNAL(clicked(bool)), SLOT(sl_fontItalicChanged()));
    connect(underlineAttrButton, SIGNAL(clicked(bool)), SLOT(sl_fontUnderlineChanged()));
    connect(fontSizeSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_fontSizeChanged()));
    connect(fontComboBox, SIGNAL(currentFontChanged(const QFont&)), SLOT(sl_fontTypeChanged()));

    // Nodes.
    connect(showNodeShapeCheck, &QCheckBox::stateChanged, this, &TreeOptionsWidget::sl_valueChanged);
    connect(showTipShapeCheck, &QCheckBox::stateChanged, this, &TreeOptionsWidget::sl_valueChanged);

    // Scalebar settings widgets
    connect(scaleSpinBox, SIGNAL(valueChanged(double)), SLOT(sl_valueChanged()));
    connect(scaleFontSizeSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));
    connect(lineWidthSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));

    // Branches settings widgets
    connect(breadthScaleAdjustmentSlider, &QSlider::valueChanged, this, &TreeOptionsWidget::sl_valueChanged);
    connect(curvatureSlider, &QSlider::valueChanged, this, &TreeOptionsWidget::sl_valueChanged);

    connect(branchesColorButton, SIGNAL(clicked()), SLOT(sl_branchesColorButton()));
    connect(lineWeightSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));

    if (editor != nullptr) {
        auto multiTreeViewer = qobject_cast<MsaEditorWgt*>(editor->getMaEditorWgt())->getMultiTreeViewer();
        SAFE_POINT(multiTreeViewer != nullptr, "Tree options widget is instantiated with no active tree view", );
        connect(multiTreeViewer, &MSAEditorMultiTreeViewer::si_activeTreeViewChanged, this, [this] { updateAllWidgets(); });
    }
}

void TreeOptionsWidget::sl_valueChanged() {
    auto inputWidget = qobject_cast<QWidget*>(sender());
    SAFE_POINT(inputWidget != nullptr, "sl_valueChanged: Sender is not a widget", );

    QVariant newValue = savableTab.getChildValue(inputWidget->objectName());
    TreeViewOption option = optionsMap[inputWidget->objectName()];
    getTreeViewer()->updateOption(option, newValue);
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
    QMap<TreeViewOption, QVariant> settings = treeViewerUi->getSelectionSettings();

    updateButtonColor(labelsColorButton, qvariant_cast<QColor>(settings[LABEL_COLOR]));

    QFont font = TreeViewerUtils::getFontFromSettings(settings);
    if (fontComboBox->currentFont().family() != font.family()) {
        fontComboBox->setCurrentFont(font.family());
    }
    fontSizeSpinBox->setValue(settings[LABEL_FONT_SIZE].toInt());
    boldAttrButton->setChecked(settings[LABEL_FONT_BOLD].toBool());
    italicAttrButton->setChecked(settings[LABEL_FONT_ITALIC].toBool());
    underlineAttrButton->setChecked(settings[LABEL_FONT_UNDERLINE].toBool());
}

TreeViewerUI* TreeOptionsWidget::getTreeViewer() const {
    SAFE_POINT(editor != nullptr || treeViewer != nullptr, QString("Invalid parameter in constructor TreeOptionsWidget"), nullptr);

    if (treeViewer != nullptr) {
        return treeViewer;
    }
    auto mui = qobject_cast<MsaEditorMultilineWgt*>(editor->getUI());
    MSAEditorTreeViewer* currentTree = mui->getCurrentTree();
    return currentTree != nullptr
               ? currentTree->getTreeViewerUI()
               : nullptr;
}

void TreeOptionsWidget::sl_fontTypeChanged() {
    getTreeViewer()->updateOption(LABEL_FONT_FAMILY, fontComboBox->currentFont().family());
}

void TreeOptionsWidget::sl_fontSizeChanged() {
    getTreeViewer()->updateOption(LABEL_FONT_SIZE, fontSizeSpinBox->value());
}

void TreeOptionsWidget::sl_fontBoldChanged() {
    getTreeViewer()->updateOption(LABEL_FONT_BOLD, boldAttrButton->isChecked());
}

void TreeOptionsWidget::sl_fontItalicChanged() {
    getTreeViewer()->updateOption(LABEL_FONT_ITALIC, italicAttrButton->isChecked());
}

void TreeOptionsWidget::sl_fontUnderlineChanged() {
    getTreeViewer()->updateOption(LABEL_FONT_UNDERLINE, underlineAttrButton->isChecked());
}

void TreeOptionsWidget::sl_labelsColorButton() {
    auto treeViewerUi = getTreeViewer();
    auto curColor = qvariant_cast<QColor>(treeViewerUi->getOption(LABEL_COLOR));
    auto newColor = QColorDialog::getColor(curColor, AppContext::getMainWindow()->getQMainWindow());
    if (newColor.isValid()) {
        updateButtonColor(labelsColorButton, newColor);
        treeViewerUi->updateOption(LABEL_COLOR, newColor);
    }
}

void TreeOptionsWidget::sl_branchesColorButton() {
    auto treeViewerUi = getTreeViewer();
    auto curColor = qvariant_cast<QColor>(treeViewerUi->getOption(BRANCH_COLOR));
    auto newColor = QColorDialog::getColor(curColor, AppContext::getMainWindow()->getQMainWindow());
    if (newColor.isValid()) {
        updateButtonColor(branchesColorButton, newColor);
        treeViewerUi->updateOption(BRANCH_COLOR, newColor);
    }
}

void TreeOptionsWidget::updateButtonColor(QPushButton* button, const QColor& newColor) {
    QPalette palette = button->palette();
    CHECK(palette.color(button->backgroundRole()) != newColor, );
    palette.setColor(button->backgroundRole(), newColor);
    button->setPalette(palette);
}

AddTreeWidget::AddTreeWidget(MSAEditor* msaEditor)
    : editor(msaEditor), openTreeButton(nullptr), buildTreeButton(nullptr), addTreeHint(nullptr) {
    setObjectName("AddTreeWidget");
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 10, 8, 0);

    addTreeHint = new QLabel(tr("No active trees found."), this);
    mainLayout->addWidget(addTreeHint);

    mainLayout->addSpacing(10);

    auto buttonLayout = new QHBoxLayout();
    openTreeButton = new QPushButton(QIcon(":ugene/images/advanced_open.png"), tr("Open tree"), this);
    openTreeButton->setMinimumWidth(102);
    buttonLayout->addWidget(openTreeButton);
    openTreeButton->setObjectName("openTreeButton");

    auto horizontalSpacer = new QSpacerItem(50, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(horizontalSpacer);

    buildTreeButton = new QPushButton(QIcon(":core/images/phylip.png"), tr("Build tree"), this);
    buildTreeButton->setMinimumWidth(102);
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
