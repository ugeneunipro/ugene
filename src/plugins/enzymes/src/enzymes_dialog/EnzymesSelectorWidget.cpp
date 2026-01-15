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

#include "EnzymesSelectorWidget.h"

#include <QDir>
#include <QInputDialog>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/CollectionUtils.h>
#include <U2Core/DNASequence.h>
#include <U2Core/Settings.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/U2FileDialog.h>

#include "EnzymeGroupTreeItem.h"
#include "EnzymeTreeItem.h"
#include "EnzymesIO.h"
#include "FindEnzymesTask.h"

// TODO: group by TYPE, ORGANISM
// TODO: check whole group (tristate mode)

namespace U2 {

QList<SEnzymeData> EnzymesSelectorWidget::loadedEnzymes;
QSet<QString> EnzymesSelectorWidget::lastSelection;
QStringList EnzymesSelectorWidget::loadedSuppliers;

static const QStringList RESTRICTION_SEQUENCE_LENGTH_VALUES = {"1", "2", "3", "4", "5", "6", "7", "8", "9+"};

EnzymesSelectorWidget::EnzymesSelectorWidget(QWidget* parent)
    : QWidget(parent) {
    setupUi(this);

    filterComboBox->addItem(tr("name"), FILTER_BY_NAME);
    filterComboBox->addItem(tr("sequence"), FILTER_BY_SEQUENCE);

    selectionSplitter->setStretchFactor(0, 1);
    selectionSplitter->setStretchFactor(1, 1);

    enzymesSplitter->setStretchFactor(0, 3);
    enzymesSplitter->setStretchFactor(1, 2);

    checkedEnzymesEdit->setWordWrapMode(QTextOption::WrapAnywhere);

    tree->setSortingEnabled(true);
    tree->sortByColumn(0, Qt::AscendingOrder);
    tree->setUniformRowHeights(true);
    tree->setColumnWidth(0, 160);  // id
    tree->setColumnWidth(1, 75);  // accession
    tree->setColumnWidth(2, 50);  // type
    tree->setColumnWidth(4, 270);  // organism

    for (const auto& k : qAsConst(RESTRICTION_SEQUENCE_LENGTH_VALUES)) {
        if (k != RESTRICTION_SEQUENCE_LENGTH_VALUES.back()) {
            cbMinLength->addItem(k, k.toInt());
            cbMaxLength->addItem(k, k.toInt());
        } else {
            cbMinLength->addItem(k, 9);
            cbMaxLength->addItem(k, INT_MAX);
        }
    }
    cbMinLength->setCurrentIndex(0);
    cbMaxLength->setCurrentIndex(RESTRICTION_SEQUENCE_LENGTH_VALUES.size() - 1);

    const QList<QPair<QString, EnzymeData::OverhangTypes>> overhangTypeValues = {
        {tr("Any"), (EnzymeData::OverhangType::NoOverhang | EnzymeData::OverhangType::Blunt | EnzymeData::OverhangType::Sticky)},
        {tr("No overhang"), EnzymeData::OverhangType::NoOverhang},
        {tr("Blunt or Sticky"), (EnzymeData::OverhangType::Blunt | EnzymeData::OverhangType::Sticky)},
        {tr("Blunt or Nondegenerate Sticky"), (EnzymeData::OverhangType::Blunt | EnzymeData::OverhangType::NondegenerateSticky)},
        {tr("Blunt"), EnzymeData::OverhangType::Blunt},
        {tr("Sticky"), EnzymeData::OverhangType::Sticky},
        {tr("Nondegenerate Sticky"), EnzymeData::OverhangType::NondegenerateSticky},
        {"5'", EnzymeData::OverhangType::Cut5},
        {"3'", EnzymeData::OverhangType::Cut3}};
    for (const auto& ov : qAsConst(overhangTypeValues)) {
        cbOverhangType->addItem(ov.first, QVariant(ov.second));
    }
    cbOverhangType->setCurrentIndex(0);

    cbSuppliers->allSelectedText = tr("All suppliers");
    cbSuppliers->noneSelectedText = tr("No suppliers");
    cbSuppliers->nSelectedText = tr("%1 suppliers");

    initSettings();

    connect(enzymesFileButton, SIGNAL(clicked()), SLOT(sl_openEnzymesFile()));
    connect(saveEnzymesButton, SIGNAL(clicked()), SLOT(sl_saveEnzymesFile()));
    connect(selectAllButton, SIGNAL(clicked()), SLOT(sl_selectAll()));
    connect(selectNoneButton, SIGNAL(clicked()), SLOT(sl_selectNone()));
    connect(invertSelectionButton, SIGNAL(clicked()), SLOT(sl_inverseSelection()));
    connect(loadSelectionButton, SIGNAL(clicked()), SLOT(sl_loadSelectionFromFile()));
    connect(saveSelectionButton, SIGNAL(clicked()), SLOT(sl_saveSelectionToFile()));
    connect(enzymesFilterEdit, &QLineEdit::textChanged, this, &EnzymesSelectorWidget::sl_filterConditionsChanged);
    connect(filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_filterConditionsChanged);
    connect(AppContext::getMainWindow(), &MainWindow::si_colorThemeSwitched, this, &EnzymesSelectorWidget::sl_colorThemeSwitched);

    if (loadedEnzymes.isEmpty()) {
        QString lastUsedFile = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();
        loadFile(lastUsedFile);
    }

    connect(cbSuppliers, &ComboBoxWithCheckBoxes::si_checkedChanged, this, &EnzymesSelectorWidget::sl_updateVisibleEnzymes);
    connect(cbMinLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_minLengthChanged);
    connect(cbMaxLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_maxLengthChanged);
    connect(this, &EnzymesSelectorWidget::si_newEnzymeFileLoaded, this, &EnzymesSelectorWidget::sl_updateEnzymesVisibilityWidgets);
    connect(cbOverhangType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_updateVisibleEnzymes);
    connect(cbShowPalindromic, &QCheckBox::toggled, this, &EnzymesSelectorWidget::sl_updateVisibleEnzymes);
    connect(cbShowUninterrupted, &QCheckBox::toggled, this, &EnzymesSelectorWidget::sl_updateVisibleEnzymes);
    connect(cbShowNondegenerate, &QCheckBox::toggled, this, &EnzymesSelectorWidget::sl_updateVisibleEnzymes);
    sl_updateEnzymesVisibilityWidgets();

    connect(pbSelectAll, &QPushButton::clicked, this, &EnzymesSelectorWidget::sl_selectAllSuppliers);
    connect(pbSelectNone, &QPushButton::clicked, this, &EnzymesSelectorWidget::sl_selectNoneSuppliers);
    connect(pbInvertSelection, &QPushButton::clicked, this, &EnzymesSelectorWidget::sl_invertSelection);
    connect(this, SIGNAL(si_selectionModified(int, int)), SLOT(sl_onSelectionModified(int, int)));

    sl_onSelectionModified(getTotalNumber(), getNumSelected());
}

EnzymesSelectorWidget::~EnzymesSelectorWidget() {
    saveSettings();
}

void EnzymesSelectorWidget::setupSettings() {
    QString dir = LastUsedDirHelper::getLastUsedDir(EnzymeSettings::DATA_DIR_KEY);
    if (dir.isEmpty() || !QDir(dir).exists()) {
        dir = QDir::searchPaths(PATH_PREFIX_DATA).first() + "/enzymes/";
        LastUsedDirHelper::setLastUsedDir(dir, EnzymeSettings::DATA_DIR_KEY);
    }
    QString lastEnzFile = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();
    if (lastEnzFile.isEmpty() || !QFile::exists(lastEnzFile)) {
        lastEnzFile = dir + "/" + DEFAULT_ENZYMES_FILE;
        AppContext::getSettings()->setValue(EnzymeSettings::DATA_FILE_KEY, lastEnzFile);
    }
    initSelection();
}

QList<SEnzymeData> EnzymesSelectorWidget::getSelectedEnzymes() const {
    QList<SEnzymeData> selectedEnzymes;
    lastSelection.clear();
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        for (int j = 0, m = gi->childCount(); j < m; j++) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            if (item->checkState(0) == Qt::Checked) {
                selectedEnzymes.append(item->enzyme);
                lastSelection.insert(item->enzyme->id);
            }
        }
    }
    return selectedEnzymes;
}

EnzymeTreeItem* EnzymesSelectorWidget::getEnzymeTreeItemByEnzymeData(const SEnzymeData& enzyme) const {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        for (int j = 0, m = gi->childCount(); j < m; j++) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            CHECK_CONTINUE((item->enzyme->id == enzyme->id) && (item->enzyme->seq == enzyme->seq));

            return item;
        }
    }

    return nullptr;
}

const QList<SEnzymeData>& EnzymesSelectorWidget::getLoadedEnzymes() {
    static const QList<SEnzymeData> emptyList;
    if (loadedEnzymes.isEmpty()) {
        U2OpStatus2Log os;
        QString lastUsedFile = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();
        loadedEnzymes = EnzymesIO::readEnzymes(lastUsedFile, os);
        CHECK_OP(os, emptyList);

        calculateSuppliers();
    }
    return loadedEnzymes;
}

const QStringList& EnzymesSelectorWidget::getLoadedSuppliers() {
    return loadedSuppliers;
}

void EnzymesSelectorWidget::setSequenceContext(const QPointer<ADVSequenceObjectContext>& _advSequenceContext) {
    advSequenceContext = _advSequenceContext;
}

void EnzymesSelectorWidget::calculateSuppliers() {
    loadedSuppliers.clear();
    for (const auto& enzyme : qAsConst(loadedEnzymes)) {
        for (const auto& supplier : qAsConst(enzyme->suppliers)) {
            CHECK_CONTINUE(!loadedSuppliers.contains(supplier));

            loadedSuppliers << supplier;
        }
    }
    std::sort(loadedSuppliers.begin(),
              loadedSuppliers.end(),
              [](const QString& first, const QString& second) {
                  static const QString sign = EnzymesIO::tr(EnzymesIO::NOT_DEFINED_SIGN);
                  if (first == sign) {
                      return true;
                  } else if (second == sign) {
                      return false;
                  }
                  return first < second;
              });
}

void EnzymesSelectorWidget::initSettings() {
    EnzymesSelectorWidget::initSelection();
    auto settings = AppContext::getSettings();
    auto min = settings->getValue(EnzymeSettings::MIN_ENZYME_LENGTH, RESTRICTION_SEQUENCE_LENGTH_VALUES.first()).toString();
    cbMinLength->setCurrentText(min);
    auto max = settings->getValue(EnzymeSettings::MAX_ENZYME_LENGTH, RESTRICTION_SEQUENCE_LENGTH_VALUES.last()).toString();
    cbMaxLength->setCurrentText(max);
    auto overhangTypeIndex = settings->getValue(EnzymeSettings::OVERHANG_TYPE, 0).toInt();
    cbOverhangType->setCurrentIndex(overhangTypeIndex);
    cbShowPalindromic->setChecked(settings->getValue(EnzymeSettings::SHOW_PALINDROMIC, 0).toBool());
    cbShowUninterrupted->setChecked(settings->getValue(EnzymeSettings::SHOW_UNINTERRUPTED, 0).toBool());
    cbShowNondegenerate->setChecked(settings->getValue(EnzymeSettings::SHOW_NONDEGENERATE, 0).toBool());
}

void EnzymesSelectorWidget::loadFile(const QString& url) {
    U2OpStatus2Log os;
    {
        QList<SEnzymeData> enzymes;
        if (!QFileInfo::exists(url)) {
            os.setError(tr("File not exists: %1").arg(url));
        } else {
            GTIMER(c1, t1, "FindEnzymesDialog::loadFile [EnzymesIO::readEnzymes]");
            enzymes = EnzymesIO::readEnzymes(url, os);
        }
        if (os.hasError()) {
            if (isVisible()) {
                QMessageBox::critical(nullptr, tr("Error"), os.getError());
            } else {
                ioLog.error(os.getError());
            }
            return;
        }

        loadedEnzymes = enzymes;
        calculateSuppliers();
    }

    if (!loadedEnzymes.isEmpty()) {
        if (AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString() != url) {
            lastSelection.clear();
        }
        AppContext::getSettings()->setValue(EnzymeSettings::DATA_FILE_KEY, url);
    }
    emit si_newEnzymeFileLoaded();
}

void EnzymesSelectorWidget::saveFile(const QString& url) {
    TaskStateInfo ti;
    QString source = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();

    GTIMER(c1, t1, "FindEnzymesDialog::saveFile [EnzymesIO::writeEnzymes]");

    QSet<QString> enzymes;

    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        for (int j = 0, m = gi->childCount(); j < m; j++) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            if (item->checkState(0) == Qt::Checked) {
                enzymes.insert(item->enzyme->id);
            }
        }
    }

    EnzymesIO::writeEnzymes(url, source, enzymes, ti);

    if (ti.hasError()) {
        if (isVisible()) {
            QMessageBox::critical(nullptr, tr("Error"), ti.getError());
        } else {
            uiLog.error(ti.getError());
        }
        return;
    }
    if (QMessageBox::question(this, tr("New enzymes database has been saved."), tr("Do you want to work with new database?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        loadFile(url);
    }
}

void EnzymesSelectorWidget::setEnzymesList(const QList<SEnzymeData>& enzymes) {
    tree->setSortingEnabled(false);
    tree->disconnect(this);
    tree->clear();
    totalEnzymes = 0;

    GTIMER(c2, t2, "FindEnzymesDialog::loadFile [refill data tree]");

    enzymesFilterEdit->clear();

    for (const SEnzymeData& enz : qAsConst(enzymes)) {
        auto item = new EnzymeTreeItem(enz);
        if (lastSelection.contains(enz->id)) {
            item->setCheckState(0, Qt::Checked);
        }
        totalEnzymes++;
        EnzymeGroupTreeItem* gi = findGroupItem(enz->id.isEmpty() ? QString(" ") : enz->id.left(1), true);
        gi->addChild(item);
    }
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        gi->updateVisual();
    }
    if (tree->topLevelItemCount() > 0 && tree->topLevelItem(0)->childCount() < 10) {
        tree->topLevelItem(0)->setExpanded(true);
    }
    t2.stop();

    GTIMER(c3, t3, "FindEnzymesDialog::loadFile [sort tree]");
    tree->setSortingEnabled(true);
    t3.stop();

    connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(sl_itemChanged(QTreeWidgetItem*, int)));
    connect(tree, &QTreeWidget::itemSelectionChanged, this, &EnzymesSelectorWidget::sl_itemSelectionChanged);

    //     GTIMER(c4,t4,"FindEnzymesDialog::loadFile [resize tree]");
    //     tree->header()->resizeSections(QHeaderView::ResizeToContents);
    //     t4.stop();

    updateStatus();
}

int EnzymesSelectorWidget::gatherCheckedNamesListString(QString& checkedNamesListString) const {
    int checked = 0;
    QStringList checkedNamesList;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        checked += gi->checkedEnzymes.size();
        foreach (const EnzymeTreeItem* ci, gi->checkedEnzymes) {
            checkedNamesList.append(ci->enzyme->id);
        }
    }
    checkedNamesList.sort();
    checkedNamesListString = checkedNamesList.join(",");

    return checked;
}

EnzymeGroupTreeItem* EnzymesSelectorWidget::findGroupItem(const QString& s, bool create) {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        if (gi->s == s) {
            return gi;
        }
    }
    if (create) {
        auto gi = new EnzymeGroupTreeItem(s);
        tree->addTopLevelItem(gi);
        return gi;
    }
    return nullptr;
}

void EnzymesSelectorWidget::sl_filterConditionsChanged() {
    const QString filterText = enzymesFilterEdit->text();
    const int filterMode = filterComboBox->currentData().toInt();
    for (int i = 0, n = tree->topLevelItemCount(); i < n; ++i) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        int numHiddenItems = 0;
        int itemCount = gi->childCount();
        for (int j = 0; j < itemCount; ++j) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            if (filterMode == FILTER_BY_NAME) {
                const QStringList selectedEnzymeIdsList = filterText.split(",", Qt::SkipEmptyParts);
                bool hideItem = !selectedEnzymeIdsList.isEmpty();
                for (const QString& itemFromList : qAsConst(selectedEnzymeIdsList)) {
                    if (item->enzyme->id.contains(itemFromList, Qt::CaseInsensitive)) {
                        hideItem = false;
                        break;
                    }
                }
                if (hideItem) {
                    ++numHiddenItems;
                }
                item->setHidden(hideItem);
            } else {
                CHECK(filterMode == FILTER_BY_SEQUENCE, );
                const QString enzymeSequence(item->enzyme->seq);
                if (enzymeSequence.contains(filterText, Qt::CaseInsensitive)) {
                    item->setHidden(false);
                } else {
                    item->setHidden(true);
                    ++numHiddenItems;
                }
            }
        }
        gi->setHidden(numHiddenItems == itemCount);
    }
}

void EnzymesSelectorWidget::sl_findSingleEnzymeTaskStateChanged() {
    auto t = qobject_cast<FindSingleEnzymeTask*>(sender());
    SAFE_POINT_NN(t, );

    CHECK_OP(t->getStateInfo(), );
    CHECK(t->getState() == Task::State_Finished, );

    auto taskEnzyme = t->getEnzyme();
    auto taskItem = getEnzymeTreeItemByEnzymeData(taskEnzyme);
    CHECK(taskItem != nullptr, );

    int size = t->getResults().size();
    bool maxResultsFound = t->wasStoppedOnMaxResults();
    taskItem->enzymesNumber = maxResultsFound ? size + 1 : size;
    auto currentItem = dynamic_cast<EnzymeTreeItem*>(tree->currentItem());
    CHECK(currentItem != nullptr, );
    CHECK((currentItem->enzyme->id == taskItem->enzyme->id) && (currentItem->enzyme->seq == taskItem->enzyme->seq), );

    teSelectedEnzymeInfo->setHtml(currentItem->getEnzymeInfo());
}

void EnzymesSelectorWidget::sl_itemSelectionChanged() {
    auto item = tree->currentItem();
    CHECK(item != nullptr, );

    EnzymeTreeItem* ei = dynamic_cast<EnzymeTreeItem*>(item);
    EnzymeGroupTreeItem* gi = dynamic_cast<EnzymeGroupTreeItem*>(item);
    if (ei != nullptr) {
        teSelectedEnzymeInfo->setHtml(ei->getEnzymeInfo());
        static constexpr int MAX_CHECKED_SEQUENCE_LENGTH = 200'000;
        if (!ei->hasNumberCalculationTask &&
            !advSequenceContext.isNull() &&
            advSequenceContext->getSequenceLength() < MAX_CHECKED_SEQUENCE_LENGTH) {
            auto seqObj = advSequenceContext->getSequenceObject();
            const auto& er = seqObj->getEntityRef();
            U2Region reg(0, seqObj->getSequenceLength());
            auto t = new FindSingleEnzymeTask(er, reg, ei->enzyme, nullptr, seqObj->isCircular(), EnzymeTreeItem::MAXIMUM_ENZYMES_NUMBER, false);
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
            connect(t, &FindSingleEnzymeTask::si_stateChanged, this, &EnzymesSelectorWidget::sl_findSingleEnzymeTaskStateChanged);
            ei->hasNumberCalculationTask = true;
        }
    } else if (gi != nullptr) {
        teSelectedEnzymeInfo->clear();
    } else {
        FAIL("Unexpected item type", );
    }
}

void EnzymesSelectorWidget::sl_colorThemeSwitched() {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; ++i) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        int itemCount = gi->childCount();
        for (int j = 0; j < itemCount; ++j) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            item->colorThemeSwitched();
        }
    }

    sl_itemSelectionChanged();
}

void EnzymesSelectorWidget::sl_onSelectionModified(int visible, int selected) {
    int total = EnzymesSelectorWidget::getLoadedEnzymes().size();
    int hidden = total - visible;
    QString text = tr("Total number of enzymes: %1, visible: %2, hidden: %3, selected: %4. ")
                       .arg(total)
                       .arg(visible)
                       .arg(hidden)
                       .arg(selected);
    if (hidden != 0) {
        text += tr("Some enzymes are hidden due to \"Enzyme table filter\" settings.");
    }
    statusLabel->setText(text);
}

void EnzymesSelectorWidget::sl_updateVisibleEnzymes() {
    QStringList checkedSuppliers = cbSuppliers->getCheckedItems();
    int min = cbMinLength->itemData(cbMinLength->currentIndex()).toInt();
    int max = cbMaxLength->itemData(cbMaxLength->currentIndex()).toInt();
    U2Region region(min, max - min + 1);
    auto overhangType = static_cast<EnzymeData::OverhangTypes>(cbOverhangType->itemData(cbOverhangType->currentIndex()).toInt());
    const auto& enzymes = EnzymesSelectorWidget::getLoadedEnzymes();
    QList<SEnzymeData> visibleEnzymes;
    for (const auto& enzyme : qAsConst(enzymes)) {
        bool okSupplier = std::find_if(enzyme->suppliers.begin(),
                                       enzyme->suppliers.end(),
                                       [&checkedSuppliers](const QString& s) {
                                           return checkedSuppliers.contains(s);
                                       }) != enzyme->suppliers.end();

        int recognitionSequenceLength = 0;
        for (const auto& ch : qAsConst(enzyme->seq)) {
            CHECK_CONTINUE(ch != EnzymeData::UNDEFINED_BASE);

            recognitionSequenceLength++;
        }
        bool okRSLength = region.contains(recognitionSequenceLength);
        bool okOverhangType = enzyme->overhangTypes & overhangType;
        bool okPalindromic = !cbShowPalindromic->isChecked() || (cbShowPalindromic->isChecked() && enzyme->palindromic);
        bool okUninterrupted = !cbShowUninterrupted->isChecked() || (cbShowUninterrupted->isChecked() && enzyme->uninterrupted);
        bool okNondegenerate = !cbShowNondegenerate->isChecked() || (cbShowNondegenerate->isChecked() && enzyme->nondegenerate);

        if (okSupplier && okRSLength && okOverhangType && okPalindromic && okUninterrupted && okNondegenerate) {
            visibleEnzymes.append(enzyme);
        }
    }
    setEnzymesList(visibleEnzymes);
}

void EnzymesSelectorWidget::sl_updateEnzymesVisibilityWidgets() {
    static const QString notDefinedTr = EnzymesIO::tr(EnzymesIO::NOT_DEFINED_SIGN);

    const QStringList& loadedSuppliers = EnzymesSelectorWidget::getLoadedSuppliers();
    cbSuppliers->clear();
    cbSuppliers->addItems(loadedSuppliers);
    auto settings = AppContext::getSettings();
    QString suppliersSettingString = settings->getValue(EnzymeSettings::CHECKED_SUPPLIERS).toString();
    auto checkedSuppliers = suppliersSettingString.isEmpty()
                                ? loadedSuppliers
                                : suppliersSettingString.split(SUPPLIERS_LIST_SEPARATOR);
    if (suppliersSettingString.isEmpty()) {
        checkedSuppliers.removeOne(EnzymesIO::NOT_DEFINED_SIGN);  // Do not enable "Non defined" by default.
    } else if (checkedSuppliers.contains(EnzymesIO::NOT_DEFINED_SIGN)) {
        checkedSuppliers.replace(checkedSuppliers.indexOf(EnzymesIO::NOT_DEFINED_SIGN), notDefinedTr);
    }
    cbSuppliers->setCheckedItems(checkedSuppliers);
}

void EnzymesSelectorWidget::sl_selectAllSuppliers() {
    cbSuppliers->setCheckedItems(EnzymesSelectorWidget::getLoadedSuppliers());
}

void EnzymesSelectorWidget::sl_selectNoneSuppliers() {
    cbSuppliers->setCheckedItems({});
}

void EnzymesSelectorWidget::sl_invertSelection() {
    const auto& suppliers = EnzymesSelectorWidget::getLoadedSuppliers();
    const auto& selectedSuppliers = cbSuppliers->getCheckedItems();
    QStringList newSelectedSuppliers;
    for (const auto& supplier : qAsConst(suppliers)) {
        CHECK_CONTINUE(!selectedSuppliers.contains(supplier));

        newSelectedSuppliers << supplier;
    }
    cbSuppliers->setCheckedItems(newSelectedSuppliers);
}

void EnzymesSelectorWidget::sl_minLengthChanged(int index) {
    if (cbMaxLength->currentIndex() <= index) {
        disconnect(cbMaxLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_maxLengthChanged);
        cbMaxLength->setCurrentIndex(index);
        connect(cbMaxLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_maxLengthChanged);
    }
    sl_updateVisibleEnzymes();
}

void EnzymesSelectorWidget::sl_maxLengthChanged(int index) {
    if (cbMinLength->currentIndex() >= index) {
        disconnect(cbMinLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_minLengthChanged);
        cbMinLength->setCurrentIndex(index);
        connect(cbMinLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_minLengthChanged);
    }
    sl_updateVisibleEnzymes();
}

void EnzymesSelectorWidget::updateStatus() {
    QString checkedNamesListString;
    int nChecked = gatherCheckedNamesListString(checkedNamesListString);
    if (nChecked > 1000) {
        checkedEnzymesEdit->setPlainText(tr("%1 sites selected. Click \"Save\" to export checked enzymes to a file.").arg(nChecked));
    } else {
        checkedEnzymesEdit->setPlainText(checkedNamesListString);
    }

    emit si_selectionModified(totalEnzymes, nChecked);
}

void EnzymesSelectorWidget::sl_openEnzymesFile() {
    LastUsedDirHelper dir(EnzymeSettings::DATA_DIR_KEY);
    dir.url = U2FileDialog::getOpenFileName(this, tr("Select enzyme database file"), dir.dir, EnzymesIO::getFileDialogFilter());
    if (!dir.url.isEmpty()) {
        const QString& previousEnzymeFile = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();
        if (previousEnzymeFile != dir.url) {
            lastSelection.clear();
        }
        loadFile(dir.url);
        if (!loadedEnzymes.isEmpty()) {
            emit si_newEnzymeFileLoaded();
        }
    }
}

void EnzymesSelectorWidget::sl_selectAll() {
    ignoreItemChecks = true;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        for (int j = 0, m = gi->childCount(); j < m; j++) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            item->setCheckState(0, Qt::Checked);
        }
        gi->updateVisual();
    }
    ignoreItemChecks = false;
    updateStatus();
}

void EnzymesSelectorWidget::sl_selectNone() {
    ignoreItemChecks = true;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        for (int j = 0, m = gi->childCount(); j < m; j++) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            item->setCheckState(0, Qt::Unchecked);
        }
        gi->updateVisual();
    }
    ignoreItemChecks = false;
    updateStatus();
}

void EnzymesSelectorWidget::sl_inverseSelection() {
    ignoreItemChecks = true;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        for (int j = 0, m = gi->childCount(); j < m; j++) {
            EnzymeTreeItem* item = static_cast<EnzymeTreeItem*>(gi->child(j));
            Qt::CheckState oldState = item->checkState(0);
            item->setCheckState(0, oldState == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        }
        gi->updateVisual();
    }
    ignoreItemChecks = false;
    updateStatus();
}

void EnzymesSelectorWidget::sl_saveSelectionToFile() {
    QString selectionData;
    gatherCheckedNamesListString(selectionData);

    if (selectionData.size() == 0) {
        QMessageBox::warning(this, tr("Save selection"), tr("Can not save empty selection!"));
        return;
    }

    LastUsedDirHelper dir;
    dir.url = U2FileDialog::getSaveFileName(this, tr("Select enzymes selection"), dir.dir);
    if (!dir.url.isEmpty()) {
        QFile data(dir.url);
        if (!data.open(QFile::WriteOnly)) {
            QMessageBox::critical(this, tr("Save selection"), tr("Failed to open %1 for writing").arg(dir.url));
            return;
        }
        QTextStream out(&data);
        out << selectionData;
    }
}

void EnzymesSelectorWidget::sl_itemChanged(QTreeWidgetItem* item, int col) {
    if (item->parent() == nullptr || col != 0 || ignoreItemChecks) {
        return;
    }
    EnzymeTreeItem* ei = static_cast<EnzymeTreeItem*>(item);
    EnzymeGroupTreeItem* gi = static_cast<EnzymeGroupTreeItem*>(ei->parent());
    gi->updateVisual();
    updateStatus();
}

int EnzymesSelectorWidget::getNumSelected() {
    int nChecked = 0;
    for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
        EnzymeGroupTreeItem* gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        nChecked += gi->checkedEnzymes.size();
    }
    return nChecked;
}

int EnzymesSelectorWidget::getTotalNumber() const {
    return totalEnzymes;
}

void EnzymesSelectorWidget::saveSettings() {
    auto settings = AppContext::getSettings();
    QStringList checkedSuppliers = cbSuppliers->getCheckedItems();
    static const QString notDefinedTr = EnzymesIO::tr(EnzymesIO::NOT_DEFINED_SIGN);
    if (checkedSuppliers.contains(notDefinedTr)) {
        checkedSuppliers.replace(checkedSuppliers.indexOf(notDefinedTr), EnzymesIO::NOT_DEFINED_SIGN);
    }
    auto value = checkedSuppliers.join(SUPPLIERS_LIST_SEPARATOR);
    settings->setValue(EnzymeSettings::CHECKED_SUPPLIERS, value);
    settings->setValue(EnzymeSettings::MIN_ENZYME_LENGTH, cbMinLength->currentText());
    settings->setValue(EnzymeSettings::MAX_ENZYME_LENGTH, cbMaxLength->currentText());
    settings->setValue(EnzymeSettings::OVERHANG_TYPE, cbOverhangType->currentIndex());
    settings->setValue(EnzymeSettings::SHOW_PALINDROMIC, cbShowPalindromic->isChecked());
    settings->setValue(EnzymeSettings::SHOW_UNINTERRUPTED, cbShowUninterrupted->isChecked());
    settings->setValue(EnzymeSettings::SHOW_NONDEGENERATE, cbShowNondegenerate->isChecked());

    QStringList sl(toList(lastSelection));
    if (!sl.isEmpty()) {
        settings->setValue(EnzymeSettings::LAST_SELECTION, sl.join(ENZYME_LIST_SEPARATOR));
    }
}

void EnzymesSelectorWidget::initSelection() {
    QString selStr = AppContext::getSettings()->getValue(EnzymeSettings::LAST_SELECTION).toString();
    if (selStr.isEmpty()) {
        selStr = EnzymeSettings::COMMON_ENZYMES;
    }
    lastSelection = toSet(selStr.split(ENZYME_LIST_SEPARATOR));
}

void EnzymesSelectorWidget::sl_loadSelectionFromFile() {
    LastUsedDirHelper dir;
    dir.url = U2FileDialog::getOpenFileName(this, tr("Select enzymes selection"), dir.dir);
    if (!dir.url.isEmpty()) {
        QFile selectionFile(dir.url);
        if (!selectionFile.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Load selection"), tr("Failed to open selection file %1").arg(dir.url));
            return;
        }

        QSet<QString> enzymeNames;
        QTextStream in(&selectionFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList enzymes = line.split(QRegExp("[,;\\s]+"), Qt::SkipEmptyParts);
            foreach (const QString& enz, enzymes) {
                enzymeNames.insert(enz);
            }
        }

        if (enzymeNames.isEmpty()) {
            QMessageBox::critical(this, tr("Load selection"), tr("Enzymes selection is empty!"));
            return;
        }

        ignoreItemChecks = true;
        for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
            EnzymeGroupTreeItem* gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
            for (int j = 0, m = gi->childCount(); j < m; j++) {
                EnzymeTreeItem* item = static_cast<EnzymeTreeItem*>(gi->child(j));
                QString eName = item->enzyme->id;
                if (enzymeNames.contains(eName)) {
                    item->setCheckState(0, Qt::Checked);
                    enzymeNames.remove(eName);
                } else {
                    item->setCheckState(0, Qt::Unchecked);
                }
            }
            gi->updateVisual();
        }
        ignoreItemChecks = false;

        updateStatus();

        foreach (const QString& enzyme, enzymeNames) {
            ioLog.error(tr("Failed to load %1 from selection.").arg(enzyme));
        }
    }
}

void EnzymesSelectorWidget::sl_saveEnzymesFile() {
    LastUsedDirHelper dir(EnzymeSettings::DATA_DIR_KEY);
    dir.url = U2FileDialog::getSaveFileName(this, tr("Select enzyme database file"), dir.dir, EnzymesIO::getFileDialogFilter());
    if (!dir.url.isEmpty()) {
        saveFile(dir.url);
    }
}

}  // namespace U2
