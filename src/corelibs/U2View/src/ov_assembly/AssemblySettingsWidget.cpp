/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>

#include "AssemblyBrowser.h"
#include "AssemblyConsensusArea.h"
#include "AssemblyReadsArea.h"
#include "AssemblySettingsWidget.h"

#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

namespace U2 {

static const int ITEMS_SPACING = 10;
static const int TITLE_SPACING = 5;

static inline QVBoxLayout * initLayout(QWidget * w) {
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    w->setLayout(layout);
    return layout;
}

AssemblySettingsWidget::AssemblySettingsWidget(AssemblyBrowserUi * ui_)
    : QWidget(ui_), ui(ui_), savableTab(this, GObjectViewUtils::findViewByName(ui_->getWindow()->getName()))
{
    QVBoxLayout* mainLayout = initLayout(this);
    mainLayout->setSpacing(0);

    QWidget * readsGroup = new ShowHideSubgroupWidget("READS", tr("Reads Area"), createReadsSettings(), true);
    mainLayout->addWidget(readsGroup);

    QWidget * consensusGroup = new ShowHideSubgroupWidget("CONSENSUS", tr("Consensus Area"), createConsensusSettings(), true);
    mainLayout->addWidget(consensusGroup);

    QWidget * rulerGroup = new ShowHideSubgroupWidget("RULER", tr("Ruler"), createRulerSettings(), true);
    mainLayout->addWidget(rulerGroup);

    U2WidgetStateStorage::restoreWidgetState(savableTab);
}

static inline void createTwoWayBinding(QCheckBox * checkBox, QAction * action) {
    QObject::connect(action,   SIGNAL(toggled(bool)), checkBox, SLOT(setChecked(bool)));
    QObject::connect(checkBox, SIGNAL(toggled(bool)), action,   SLOT(setChecked(bool)));
    checkBox->setChecked(action->isChecked());
}

// ------- Reads ----------

QWidget * AssemblySettingsWidget::createReadsSettings() {
    QWidget * group = new QWidget(this);
    QVBoxLayout * layout = initLayout(group);
    AssemblyReadsArea * readsArea = ui->getReadsArea();
    hint = new QLabel("", group);
    hint->setObjectName("HINT_HIGHLIGHTNING");
    hint->setWordWrap(true);
    hint->setStyleSheet(
        "color: green;"
        "font: bold;");

    layout->addSpacing(TITLE_SPACING);

    layout->addWidget(new QLabel(tr("Reads highlighting:"), group));

    readsHighlightCombo = new QComboBox(group);
    readsHighlightCombo->setObjectName("READS_HIGHLIGHTNING_COMBO");
    foreach(QAction * a, readsArea->getCellRendererActions()) {
        readsHighlightCombo->addItem(a->text());
        connect(a, SIGNAL(triggered()), SLOT(sl_cellRendererChanged()));
        if(a->isChecked()) {
            readsHighlightCombo->setCurrentIndex(readsHighlightCombo->count() - 1);
            AssemblyCellRendererFactory* factory = ui->getWindow()->getCellRendererRegistry()->getFactoryById(AssemblyCellRendererFactory::DIFF_NUCLEOTIDES);
            if (a->text() == factory->getName()){
                hint->setText(tr("You should add reference  first for correct displaying of this highlighting"));
                hint->show();
            }else{
                hint->setText("");
                hint->hide();
            }
        }
    }
    connect(readsHighlightCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_changeCellRenderer(int)));
    layout->addWidget(readsHighlightCombo);
    layout->addWidget(hint);

    QLabel * aboutScrolling = new QLabel(tr("Scrolling can be optimized by drawing only reads' positions without content while scrolling:"));
    aboutScrolling->setWordWrap(true);
    aboutScrolling->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(aboutScrolling);

    QCheckBox * optimizeScroll = new QCheckBox(tr("Optimize scrolling"), group);
    QAction * optimizeAction = readsArea->getOptimizeRenderAction();
    createTwoWayBinding(optimizeScroll, optimizeAction);
    layout->addWidget(optimizeScroll);

    layout->addSpacing(ITEMS_SPACING);

    QCheckBox * showHint = new QCheckBox(tr("Show pop-up hint"), group);
    QAction * hintAct = ui->getWindow()->getReadHintEnabledAction();
    createTwoWayBinding(showHint, hintAct);
    layout->addWidget(showHint);

    return group;
}

void AssemblySettingsWidget::sl_cellRendererChanged() {
    QAction * action = qobject_cast<QAction*>(sender());
    int index = ui->getReadsArea()->getCellRendererActions().indexOf(action);
    SAFE_POINT(index >= 0, "cell renderer action not found",);
    readsHighlightCombo->setCurrentIndex(index);
}

void AssemblySettingsWidget::sl_changeCellRenderer(int index) {
    QList<QAction*> actions = ui->getReadsArea()->getCellRendererActions();
    CHECK(index >= 0,);
    SAFE_POINT(index <= actions.count(), "too big cell renderer action index",);
    QAction *selected = actions.at(index);
    selected->trigger();
    AssemblyCellRendererFactory* factory = ui->getWindow()->getCellRendererRegistry()->getFactoryById(AssemblyCellRendererFactory::DIFF_NUCLEOTIDES);
    if (selected->text() == factory->getName()){
        hint->setText(tr("You should add a reference first for correct displaying of selected highlighting"));
        hint->show();
    }else{
        hint->setText("");
        hint->hide();
    }
}

// ------- Consensus ----------

QWidget * AssemblySettingsWidget::createConsensusSettings() {
    QWidget * group = new QWidget(this);
    QVBoxLayout * layout = initLayout(group);
    AssemblyConsensusArea * consensusArea = ui->getConsensusArea();

    layout->addSpacing(TITLE_SPACING);

    layout->addWidget(new QLabel(tr("Consensus algorithm:")));

    algorithmCombo = new QComboBox(group);
    algorithmCombo->setObjectName("consensusAlgorithmCombo");
    foreach(QAction * a, consensusArea->getAlgorithmActions()) {
        algorithmCombo->addItem(a->text());
        connect(a, SIGNAL(triggered()), SLOT(sl_consensusAlgorithmChanged()));
        if(a->isChecked()) {
            algorithmCombo->setCurrentIndex(algorithmCombo->count() - 1);
        }
    }
    connect(algorithmCombo, SIGNAL(currentIndexChanged(int)), SLOT(sl_changeConsensusAlgorithm(int)));
    layout->addWidget(algorithmCombo);

    layout->addSpacing(ITEMS_SPACING);

    QCheckBox * showDiff = new QCheckBox(tr("Difference from reference"), group);
    QAction * diffAct = consensusArea->getDiffAction();
    createTwoWayBinding(showDiff, diffAct);
    layout->addWidget(showDiff);

    return group;
}

void AssemblySettingsWidget::sl_consensusAlgorithmChanged() {
    QAction * action = qobject_cast<QAction*>(sender());
    int index = ui->getConsensusArea()->getAlgorithmActions().indexOf(action);
    SAFE_POINT(index >= 0, "consensus algorithm action not found",);
    algorithmCombo->setCurrentIndex(index);
}

void AssemblySettingsWidget::sl_changeConsensusAlgorithm(int index) {
    QList<QAction*> actions = ui->getConsensusArea()->getAlgorithmActions();
    CHECK(index >= 0,);
    SAFE_POINT(index <= actions.count(), "too big consensus algorithm action index",);
    actions.at(index)->trigger();
}

// ------- Ruler ----------

QWidget * AssemblySettingsWidget::createRulerSettings() {
    QWidget * group = new QWidget(this);
    QVBoxLayout * layout = initLayout(group);
    AssemblyBrowser * browser = ui->getWindow();

    layout->addSpacing(TITLE_SPACING);

    QCheckBox * showCoords = new QCheckBox(tr("Show coordinates"), group);
    QAction * coordAct = browser->getCoordsOnRulerAction();
    createTwoWayBinding(showCoords, coordAct);
    layout->addWidget(showCoords);

    layout->addSpacing(ITEMS_SPACING);

    QCheckBox * showCoverage = new QCheckBox(tr("Show coverage under cursor"), group);
    QAction * coverageAct = browser->getCoverageOnRulerAction();
    createTwoWayBinding(showCoverage, coverageAct);
    layout->addWidget(showCoverage);

    return group;
}


//
// AssemblySettingsWidgetFactory
////////////////////////////////////
const QString AssemblySettingsWidgetFactory::GROUP_ID = "OP_ASS_SETTINGS";
const QString AssemblySettingsWidgetFactory::GROUP_ICON_STR = ":core/images/settings2.png";
const QString AssemblySettingsWidgetFactory::GROUP_DOC_PAGE = "24748912";


AssemblySettingsWidgetFactory::AssemblySettingsWidgetFactory()
{
    objectViewOfWidget = ObjViewType_AssemblyBrowser;
}


QWidget* AssemblySettingsWidgetFactory::createWidget(GObjectView* objView)
{
    SAFE_POINT(NULL != objView,
        QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
        NULL);

    AssemblyBrowser* assemblyBrowser = qobject_cast<AssemblyBrowser*>(objView);
    SAFE_POINT(NULL != assemblyBrowser,
        QString("Internal error: unable to cast object view to Assembly Browser for group '%1'.").arg(GROUP_ID),
        NULL);

    AssemblySettingsWidget* widget = new AssemblySettingsWidget(assemblyBrowser->getMainWidget());
    return widget;
}


OPGroupParameters AssemblySettingsWidgetFactory::getOPGroupParameters()
{
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Assembly Browser Settings"), GROUP_DOC_PAGE);
}


} // namespace
