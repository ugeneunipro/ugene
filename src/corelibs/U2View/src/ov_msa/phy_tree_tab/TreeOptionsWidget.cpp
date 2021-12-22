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
#include "ov_msa/phy_tree/MsaEditorTreeTabArea.h"
#include "ov_phyltree/TreeViewer.h"
#include "phyltree/TreeSettingsDialog.h"

namespace U2 {

const static QString SHOW_FONT_OPTIONS_LINK("show_font_options_link");
const static QString SHOW_PEN_OPTIONS_LINK("show_pen_options_link");

static inline QVBoxLayout* initLayout(QWidget* w) {
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    w->setLayout(layout);
    return layout;
}

TreeOptionsWidget::TreeOptionsWidget(MSAEditor* msaEditor, const TreeOpWidgetViewSettings& viewSettings)
    : editor(msaEditor), treeViewer(nullptr), viewSettings(viewSettings), showFontSettings(false), showPenSettings(false),
      savableTab(this, GObjectViewUtils::findViewByName(msaEditor->getName())), isUpdating(false) {
    SAFE_POINT(nullptr != editor, QString("Invalid parameter were passed into constructor TreeOptionsWidget"), );

    contentWidget = new QWidget();
    setupUi(contentWidget);

    initColorButtonsStyle();
    createGroups();
    savableTab.disableSavingForWidgets(getSaveDisabledWidgets());
    U2WidgetStateStorage::restoreWidgetState(savableTab);
    sl_selectionChanged();
}

TreeOptionsWidget::TreeOptionsWidget(TreeViewer* tree, const TreeOpWidgetViewSettings& viewSettings)
    : editor(nullptr), treeViewer(tree->getTreeViewerUI()), viewSettings(viewSettings), showFontSettings(false), showPenSettings(false),
      savableTab(this, GObjectViewUtils::findViewByName(tree->getName())), isUpdating(false) {
    SAFE_POINT(nullptr != treeViewer, QString("Invalid parameter were passed into constructor TreeOptionsWidget"), );

    contentWidget = new QWidget();
    setupUi(contentWidget);

    initColorButtonsStyle();
    createGroups();
    savableTab.disableSavingForWidgets(getSaveDisabledWidgets());
    U2WidgetStateStorage::restoreWidgetState(savableTab);
    sl_selectionChanged();
}

TreeOptionsWidget::~TreeOptionsWidget() {
    emit saveViewSettings(getViewSettings());
    delete contentWidget;
}

void TreeOptionsWidget::initColorButtonsStyle() {
    QStyle* buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
    buttonStyle->setParent(this);
    labelsColorButton->setStyle(buttonStyle);
    branchesColorButton->setStyle(buttonStyle);
}

const TreeOpWidgetViewSettings& TreeOptionsWidget::getViewSettings() {
    viewSettings.showFontSettings = showFontSettings;
    viewSettings.showPenSettings = showPenSettings;
    return viewSettings;
}

void TreeOptionsWidget::createGroups() {
    QVBoxLayout* mainLayout = initLayout(this);
    mainLayout->setSpacing(0);

    auto generalOpGroup = new ShowHideSubgroupWidget("TREE_GENERAL_OP", tr("General"), treeLayoutWidget, true);
    mainLayout->addWidget(generalOpGroup);

    auto labelsOpGroup = new ShowHideSubgroupWidget("TREE_LABELS_OP", tr("Labels"), labelsGroup, true);
    mainLayout->addWidget(labelsOpGroup);

    auto scalebarOpGroup = new ShowHideSubgroupWidget("SCALEBAR_OP", tr("Scale Bar"), scalebarGroup, true);
    mainLayout->addWidget(scalebarOpGroup);

    auto branchesOpGroup = new ShowHideSubgroupWidget("TREE_BRANCHES_OP", tr("Branches"), branchesGroup, true);
    mainLayout->addWidget(branchesOpGroup);

    initializeOptionsMap();
    updateAllWidgets();
    connectSlots();
}

void TreeOptionsWidget::updateAllWidgets() {
    showFontSettings = viewSettings.showFontSettings;
    showPenSettings = viewSettings.showPenSettings;
    createGeneralSettingsWidgets();

    QString fontLabel = showFontSettings ? tr("Hide font settings") : tr("Show font settings");
    updateShowFontOpLabel(fontLabel);
    QString penLabel = showPenSettings ? tr("Hide pen settings") : tr("Show pen settings");
    updateShowPenOpLabel(penLabel);
    fontSettingsWidget->setVisible(viewSettings.showFontSettings);
    penGroup->setVisible(viewSettings.showPenSettings);

    QMap<TreeViewOption, QVariant> settings = getTreeViewer()->getSettings();
    const QList<TreeViewOption> keyList = settings.keys();
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
    updateRelations(option, value);

    QString objectName = optionsMap.key(option);
    if (objectName.isEmpty()) {
        return;
    }
    isUpdating = true;
    savableTab.setChildValue(objectName, value);
    isUpdating = false;
}

void TreeOptionsWidget::sl_selectionChanged() {
    const QSignalBlocker fontSizeBlocker(fontSizeSpinBox);
    const QSignalBlocker fontComboBlocker(fontComboBox);

    fontComboBox->setCurrentFont(qvariant_cast<QFont>(getTreeViewer()->getOptionValue(LABEL_FONT_TYPE)));
    fontSizeSpinBox->setValue(getTreeViewer()->getOptionValue(LABEL_FONT_SIZE).toInt());
    boldAttrButton->setChecked(getTreeViewer()->getOptionValue(LABEL_FONT_BOLD).toBool());
    italicAttrButton->setChecked(getTreeViewer()->getOptionValue(LABEL_FONT_ITALIC).toBool());
    underlineAttrButton->setChecked(getTreeViewer()->getOptionValue(LABEL_FONT_UNDERLINE).toBool());
    updateButtonColor(labelsColorButton, qvariant_cast<QColor>(getTreeViewer()->getOptionValue(LABEL_COLOR)));
}

QStringList TreeOptionsWidget::getSaveDisabledWidgets() const {
    return QStringList()
           << fontComboBox->objectName()
           << fontSizeSpinBox->objectName()
           << boldAttrButton->objectName()
           << italicAttrButton->objectName()
           << underlineAttrButton->objectName()
           << labelsColorButton->objectName();
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

    optionsMap[heightSlider->objectName()] = HEIGHT_COEF;
    optionsMap[widthSlider->objectName()] = WIDTH_COEF;

    optionsMap[treeViewCombo->objectName()] = BRANCHES_TRANSFORMATION_TYPE;
    optionsMap[layoutCombo->objectName()] = TREE_LAYOUT;
}

void TreeOptionsWidget::connectSlots() {
    // Show more options labels
    connect(lblPenSettings, SIGNAL(linkActivated(const QString&)), SLOT(sl_onLblLinkActivated(const QString&)));
    connect(lblFontSettings, SIGNAL(linkActivated(const QString&)), SLOT(sl_onLblLinkActivated(const QString&)));

    // General settings widgets
    connect(treeViewCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_valueChanged()));
    connect(layoutCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_valueChanged()));

    connect(getTreeViewer(), SIGNAL(si_optionChanged(TreeViewOption, const QVariant&)), SLOT(sl_onOptionChanged(TreeViewOption, const QVariant&)));

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
    connect(widthSlider, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));
    connect(heightSlider, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));

    connect(branchesColorButton, SIGNAL(clicked()), SLOT(sl_branchesColorButton()));
    connect(lineWeightSpinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged()));

    connect(getTreeViewer(), SIGNAL(si_updateBranch()), SLOT(sl_selectionChanged()));
}

void TreeOptionsWidget::sl_valueChanged() {
    QWidget* inputWidget = qobject_cast<QWidget*>(sender());
    SAFE_POINT(inputWidget != nullptr, "Null sender in slot", );

    QVariant newValue = savableTab.getChildValue(inputWidget->objectName());
    TreeViewOption option = optionsMap[inputWidget->objectName()];
    if (option == SHOW_LABELS) {
        alignLabelsCheck->setEnabled(newValue.toBool());
    }
    updateRelations(option, newValue);

    CHECK(!isUpdating, );
    getTreeViewer()->changeOption(option, newValue);
}

void TreeOptionsWidget::createGeneralSettingsWidgets() {
    layoutCombo->addItems(QStringList()
                          << tr("Rectangular")
                          << tr("Circular")
                          << tr("Unrooted"));

    treeViewCombo->addItem(TreeSettingsDialog::getDefaultTreeModeText());
    treeViewCombo->addItem(TreeSettingsDialog::getPhylogramTreeModeText());
    treeViewCombo->addItem(TreeSettingsDialog::getCladogramTreeModeText());
}

void TreeOptionsWidget::updateFormatSettings() {
    // Update labels format settings widgets
    QColor curColor = qvariant_cast<QColor>(getTreeViewer()->getOptionValue(LABEL_COLOR));
    updateButtonColor(labelsColorButton, curColor);

    QFont curFont = qvariant_cast<QFont>(getTreeViewer()->getOptionValue(LABEL_FONT_TYPE));
    fontComboBox->setCurrentFont(curFont);

    fontSizeSpinBox->setValue(getTreeViewer()->getOptionValue(LABEL_FONT_SIZE).toInt());
    boldAttrButton->setCheckable(true);
    italicAttrButton->setCheckable(true);
    underlineAttrButton->setCheckable(true);

    boldAttrButton->setChecked(getTreeViewer()->getOptionValue(LABEL_FONT_BOLD).toBool());
    italicAttrButton->setChecked(getTreeViewer()->getOptionValue(LABEL_FONT_ITALIC).toBool());
    underlineAttrButton->setChecked(getTreeViewer()->getOptionValue(LABEL_FONT_UNDERLINE).toBool());
}

TreeViewerUI* TreeOptionsWidget::getTreeViewer() const {
    SAFE_POINT(editor != nullptr || treeViewer != nullptr, QString("Invalid parameter in constructor TreeOptionsWidget"), nullptr);
    MsaEditorWgt *msaEditorUi = qobject_cast<MsaEditorWgt *>(editor->getUI()->getUI(0));
    MSAEditorTreeViewer *currentTree = msaEditorUi->getCurrentTree();
    return treeViewer != nullptr
               ? treeViewer
               : currentTree != nullptr
                     ? currentTree->getTreeViewerUI()
                     : nullptr;
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
    QColor curColor = qvariant_cast<QColor>(getTreeViewer()->getOptionValue(LABEL_COLOR));
    QColor newColor = QColorDialog::getColor(curColor, AppContext::getMainWindow()->getQMainWindow());
    if (newColor.isValid()) {
        updateButtonColor(labelsColorButton, newColor);
        getTreeViewer()->changeOption(LABEL_COLOR, newColor);
    }
}

void TreeOptionsWidget::sl_branchesColorButton() {
    QColor curColor = qvariant_cast<QColor>(getTreeViewer()->getOptionValue(BRANCH_COLOR));
    QColor newColor = QColorDialog::getColor(curColor, AppContext::getMainWindow()->getQMainWindow());
    if (newColor.isValid()) {
        updateButtonColor(branchesColorButton, newColor);
        getTreeViewer()->changeOption(BRANCH_COLOR, newColor);
    }
}

void TreeOptionsWidget::sl_onLblLinkActivated(const QString& link) {
    if (link == SHOW_FONT_OPTIONS_LINK) {
        showFontSettings = !showFontSettings;
        QString labelText = showFontSettings ? tr("Hide font settings") : tr("Show font settings");
        updateShowFontOpLabel(labelText);
        fontSettingsWidget->setVisible(showFontSettings);
    } else if (link == SHOW_PEN_OPTIONS_LINK) {
        showPenSettings = !showPenSettings;
        QString labelText = showPenSettings ? tr("Hide pen settings") : tr("Show pen settings");
        updateShowPenOpLabel(labelText);
        penGroup->setVisible(showPenSettings);
    }
}

void TreeOptionsWidget::updateButtonColor(QPushButton* button, const QColor& newColor) {
    QPalette palette = button->palette();
    palette.setColor(button->backgroundRole(), newColor);
    button->setPalette(palette);
}

void TreeOptionsWidget::updateShowFontOpLabel(QString newText) {
    newText = QString("<a href=\"%1\" style=\"color: %2\">")
                  .arg(SHOW_FONT_OPTIONS_LINK)
                  .arg(Theme::linkColorLabelStr()) +
              newText + QString("</a>");

    lblFontSettings->setText(newText);
    lblFontSettings->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
}

void TreeOptionsWidget::updateShowPenOpLabel(QString newText) {
    newText = QString("<a href=\"%1\" style=\"color: %2\">").arg(SHOW_PEN_OPTIONS_LINK).arg(Theme::linkColorLabelStr()) + newText + QString("</a>");

    lblPenSettings->setText(newText);
    lblPenSettings->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
}

void TreeOptionsWidget::updateRelations(TreeViewOption option, QVariant newValue) {
    if (option == BRANCHES_TRANSFORMATION_TYPE) {
        TREE_TYPE type = static_cast<TREE_TYPE>(newValue.toUInt());
        scalebarGroup->setEnabled(type == PHYLOGRAM);
    } else if (option == TREE_LAYOUT) {
        TreeLayout layout = static_cast<TreeLayout>(newValue.toUInt());
        heightSlider->setEnabled(layout == RECTANGULAR_LAYOUT);
        lblHeightSlider->setEnabled(layout == RECTANGULAR_LAYOUT);
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
    openTreeButton->setObjectName("OpenTreeButton");

    auto horizontalSpacer = new QSpacerItem(50, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(horizontalSpacer);

    buildTreeButton = new QPushButton(QIcon(":core/images/phylip.png"), tr("Build tree"), this);
    buildTreeButton->setFixedWidth(102);
    buttonLayout->addWidget(buildTreeButton);
    buildTreeButton->setObjectName("BuildTreeButton");
    buildTreeButton->setEnabled(msaEditor->buildTreeAction->isEnabled());

    connect(msaEditor->buildTreeAction, &QAction::changed, [this, msaEditor]() { buildTreeButton->setEnabled(msaEditor->buildTreeAction->isEnabled()); });
    connect(buildTreeButton, &QPushButton::clicked, [msaEditor]() { msaEditor->buildTreeAction->trigger(); });

    mainLayout->addLayout(buttonLayout);

    connect(openTreeButton, SIGNAL(clicked()), SLOT(sl_onOpenTreeTriggered()));
}

void AddTreeWidget::sl_onOpenTreeTriggered() {
    editor->getTreeManager()->openTreeFromFile();
}

TreeOptionsSavableWidget::TreeOptionsSavableWidget(QWidget* wrappedWidget, MWMDIWindow* contextWindow /*= nullptr*/)
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
    if (widgetsNotToSave.contains(child->objectName())) {
        return false;
    } else {
        return U2SavableWidget::childCanBeSaved(child);
    }
}

}  // namespace U2
