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

#include "FindEnzymesDialog.h"

#include <QCoreApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/ComboBoxWithCheckBoxes.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/AnnotatedDNAView.h>
#include <U2View/AutoAnnotationUtils.h>

#include "EnzymesIO.h"
#include "FindEnzymesTask.h"

// TODO: group by TYPE, ORGANISM
// TODO: check whole group (tristate mode)

namespace U2 {
QList<SEnzymeData> EnzymesSelectorWidget::loadedEnzymes;
QSet<QString> EnzymesSelectorWidget::lastSelection;
QStringList EnzymesSelectorWidget::loadedSuppliers;

EnzymesSelectorWidget::EnzymesSelectorWidget(const QPointer<ADVSequenceObjectContext>& _advSequenceContext, QWidget* parent)
    : QWidget(parent), advSequenceContext(_advSequenceContext) {
    setupUi(this);
    ignoreItemChecks = false;

    filterComboBox->addItem(tr("name"), FILTER_BY_NAME);
    filterComboBox->addItem(tr("sequence"), FILTER_BY_SEQUENCE);

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    tree->setSortingEnabled(true);
    tree->sortByColumn(0, Qt::AscendingOrder);
    tree->setUniformRowHeights(true);
    tree->setColumnWidth(0, 160);  // id
    tree->setColumnWidth(1, 75);  // accession
    tree->setColumnWidth(2, 50);  // type
    tree->setColumnWidth(4, 270);  // organism

    totalEnzymes = 0;
    minLength = 1;

    connect(enzymesFileButton, SIGNAL(clicked()), SLOT(sl_openEnzymesFile()));
    connect(saveEnzymesButton, SIGNAL(clicked()), SLOT(sl_saveEnzymesFile()));
    connect(selectAllButton, SIGNAL(clicked()), SLOT(sl_selectAll()));
    connect(selectNoneButton, SIGNAL(clicked()), SLOT(sl_selectNone()));
    connect(selectByLengthButton, SIGNAL(clicked()), SLOT(sl_selectByLength()));
    connect(invertSelectionButton, SIGNAL(clicked()), SLOT(sl_inverseSelection()));
    connect(loadSelectionButton, SIGNAL(clicked()), SLOT(sl_loadSelectionFromFile()));
    connect(saveSelectionButton, SIGNAL(clicked()), SLOT(sl_saveSelectionToFile()));
    connect(enzymeInfo, SIGNAL(clicked()), SLOT(sl_openDBPage()));
    connect(enzymesFilterEdit, &QLineEdit::textChanged, this, &EnzymesSelectorWidget::sl_filterConditionsChanged);
    connect(filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EnzymesSelectorWidget::sl_filterConditionsChanged);

    if (loadedEnzymes.isEmpty()) {
        QString lastUsedFile = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();
        loadFile(lastUsedFile);
    }
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

QList<SEnzymeData> EnzymesSelectorWidget::getSelectedEnzymes() {
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
    connect(tree,
            &QTreeWidget::itemSelectionChanged,
            this,
            [this]() {
                auto item = tree->currentItem();
                CHECK(item != nullptr, );

                auto ei = dynamic_cast<EnzymeTreeItem*>(item);
                auto gi = dynamic_cast<EnzymeGroupTreeItem*>(item);
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
            });

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
                if (item->enzyme->id.contains(filterText, Qt::CaseInsensitive)) {
                    item->setHidden(false);
                } else {
                    item->setHidden(true);
                    ++numHiddenItems;
                }
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

void EnzymesSelectorWidget::updateStatus() {
    QString checkedNamesListString;
    int nChecked = gatherCheckedNamesListString(checkedNamesListString);
    if (nChecked > 1000) {
        checkedEnzymesEdit->setPlainText(tr("%1 sites selected. Click \"Save selection\" to export checked enzymes to a file.").arg(nChecked));
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

void EnzymesSelectorWidget::sl_selectByLength() {
    bool ok;
    int len = QInputDialog::getInt(this, tr("Minimum length"), tr("Enter minimum length of recognition sites"), minLength, 1, 20, 1, &ok);
    if (ok) {
        minLength = len;
        ignoreItemChecks = true;
        for (int i = 0, n = tree->topLevelItemCount(); i < n; i++) {
            auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
            for (int j = 0, m = gi->childCount(); j < m; j++) {
                auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
                if (item->enzyme->seq.length() < len) {
                    item->setCheckState(0, Qt::Unchecked);
                } else {
                    item->setCheckState(0, Qt::Checked);
                }
            }
            gi->updateVisual();
        }
        ignoreItemChecks = false;
    }
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

void EnzymesSelectorWidget::sl_openDBPage() {
    QTreeWidgetItem* ci = tree->currentItem();
    EnzymeTreeItem* item = ci == nullptr || ci->parent() == 0 ? nullptr : static_cast<EnzymeTreeItem*>(tree->currentItem());
    if (item == nullptr) {
        QMessageBox::critical(this, tr("Error!"), tr("No enzyme selected!"));
        return;
    }
    QString id = item->enzyme->id;
    if (id.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Selected enzyme has no ID!"));
        return;
    }
    GUIUtils::runWebBrowser("http://rebase.neb.com/cgi-bin/reb_get.pl?enzname=" + id);
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

void EnzymesSelectorWidget::saveSettings() {
    QStringList sl(lastSelection.toList());
    if (!sl.isEmpty()) {
        AppContext::getSettings()->setValue(EnzymeSettings::LAST_SELECTION, sl.join(ENZYME_LIST_SEPARATOR));
    }
}

void EnzymesSelectorWidget::initSelection() {
    QString selStr = AppContext::getSettings()->getValue(EnzymeSettings::LAST_SELECTION).toString();
    if (selStr.isEmpty()) {
        selStr = EnzymeSettings::COMMON_ENZYMES;
    }
    lastSelection = selStr.split(ENZYME_LIST_SEPARATOR).toSet();
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
            QStringList enzymes = line.split(QRegExp("[,;\\s]+"), QString::SkipEmptyParts);
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

static const QStringList RESTRICTION_SEQUENCE_LENGTH_VALUES = {"1", "2", "3", "4", "5", "6", "7", "8", "9+"};

static const int ANY_VALUE = -1;

FindEnzymesDialog::FindEnzymesDialog(const QPointer<ADVSequenceObjectContext>& _advSequenceContext)
    : QDialog(_advSequenceContext->getAnnotatedDNAView()->getWidget()), advSequenceContext(_advSequenceContext) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930747");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    maxHitSB->setMaximum(INT_MAX);
    minHitSB->setMaximum(INT_MAX);

    maxHitSB->setMinimum(ANY_VALUE);
    minHitSB->setMinimum(ANY_VALUE);

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

    U2Location searchLocation = FindEnzymesAutoAnnotationUpdater::getLastSearchLocationForObject(advSequenceContext->getSequenceObject());
    fixPreviousLocation(searchLocation);
    U2Region customRegion = searchLocation.data()->regions.isEmpty() ? U2Region() : searchLocation.data()->regions.first();

    QList<RegionPreset> searchPresets = {RegionPreset(RegionPreset::RegionPreset::getLocationModeDisplayName(), searchLocation)};
    const QVector<U2Region> selectedRegions = advSequenceContext->getSequenceSelection()->getSelectedRegions();
    if (!selectedRegions.isEmpty()) {
        searchPresets << RegionPreset(RegionPreset::RegionPreset::getSelectedRegionDisplayName(), U2Location({selectedRegions.first()}));
    }

    const quint64 sequenceLength = advSequenceContext->getSequenceLength();
    regionSelector = new RegionSelector(this, sequenceLength, false, advSequenceContext->getSequenceSelection(), advSequenceContext->getSequenceObject()->isCircular(), searchPresets);
    if (customRegion != U2Region()) {
        regionSelector->setCustomRegion(customRegion);
    }

    if (!selectedRegions.isEmpty()) {
        regionSelector->setCurrentPreset(RegionPreset::RegionPreset::getSelectedRegionDisplayName());
    }

    U2Location excludeLocation = FindEnzymesAutoAnnotationUpdater::getLastExcludeLocationForObject(advSequenceContext->getSequenceObject());
    fixPreviousLocation(excludeLocation);
    U2Region excludeRegion = excludeLocation.data()->regions.isEmpty() ? U2Region() : excludeLocation.data()->regions.first();

    QList<RegionPreset> excludePresets = {RegionPreset(RegionPreset::RegionPreset::getLocationModeDisplayName(), excludeLocation)};
    excludeRegionSelector = new RegionSelector(this, sequenceLength, false, advSequenceContext->getSequenceSelection(), advSequenceContext->getSequenceObject()->isCircular(), excludePresets);
    excludeRegionSelector->removePreset(RegionPreset::RegionPreset::getWholeSequenceModeDisplayName());
    excludeRegionSelector->setCurrentPreset(RegionPreset::RegionPreset::getCustomRegionModeDisplayName());
    excludeRegionSelector->setObjectName("exclude_range_selector");

    excludeCheckbox = new QCheckBox(this);
    excludeCheckbox->setObjectName("excludeCheckBox");
    excludeCheckbox->setText(tr("Uncut area:"));
    excludeCheckbox->setToolTip(tr("A region that will not be cut by any of the found enzymes. If an enzyme is present in this region, it will be excluded from the flank results."));

    bool excludeLocationIsEmpty = excludeLocation.data()->regions.size() == 1 && excludeLocation.data()->regions.first() == U2Region();
    bool excludeOn = FindEnzymesAutoAnnotationUpdater::getExcludeModeEnabledForObject(advSequenceContext->getSequenceObject()) && !excludeLocationIsEmpty;
    excludeCheckbox->setChecked(excludeOn);
    excludeRegionSelector->setEnabled(excludeOn);
    if (excludeOn && excludeRegion != U2Region()) {
        excludeRegionSelector->setCustomRegion(excludeRegion);
    }

    searchRegionLayout->addWidget(regionSelector, 0, 1);
    searchRegionLayout->addWidget(excludeCheckbox, 1, 0);
    searchRegionLayout->addWidget(excludeRegionSelector, 1, 1);

    initSettings();

    auto vl = new QVBoxLayout();
    enzSel = new EnzymesSelectorWidget(advSequenceContext, this);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->addWidget(enzSel);
    enzymesSelectorWidget->setLayout(vl);
    enzymesSelectorWidget->setMinimumSize(enzSel->size());

    cbSuppliers->allSelectedText = tr("All suppliers");
    cbSuppliers->noneSelectedText = tr("No suppliers");
    cbSuppliers->nSelectedText = tr("%1 suppliers");

    connect(cbSuppliers, &ComboBoxWithCheckBoxes::si_checkedChanged, this, &FindEnzymesDialog::sl_updateVisibleEnzymes);
    connect(cbMinLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_minLengthChanged);
    connect(cbMaxLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_maxLengthChanged);
    connect(enzSel, &EnzymesSelectorWidget::si_newEnzymeFileLoaded, this, &FindEnzymesDialog::sl_updateEnzymesVisibilityWidgets);
    connect(cbOverhangType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_updateVisibleEnzymes);
    connect(cbShowPalindromic, &QCheckBox::toggled, this, &FindEnzymesDialog::sl_updateVisibleEnzymes);
    connect(cbShowUninterrupted, &QCheckBox::toggled, this, &FindEnzymesDialog::sl_updateVisibleEnzymes);
    connect(cbShowNondegenerate, &QCheckBox::toggled, this, &FindEnzymesDialog::sl_updateVisibleEnzymes);
    sl_updateEnzymesVisibilityWidgets();

    connect(pbSelectAll, &QPushButton::clicked, this, &FindEnzymesDialog::sl_selectAll);
    connect(pbSelectNone, &QPushButton::clicked, this, &FindEnzymesDialog::sl_selectNone);
    connect(pbInvertSelection, &QPushButton::clicked, this, &FindEnzymesDialog::sl_invertSelection);
    connect(enzSel, SIGNAL(si_selectionModified(int, int)), SLOT(sl_onSelectionModified(int, int)));
    sl_onSelectionModified(enzSel->getTotalNumber(), enzSel->getNumSelected());

    connect(excludeCheckbox, &QCheckBox::stateChanged, this, [this]() {
        excludeRegionSelector->setEnabled(excludeCheckbox->isChecked());
    });
}

void FindEnzymesDialog::fixPreviousLocation(U2Location& prevLocation) {
    bool fitsWell = true;
    for (const U2Region& region : qAsConst(prevLocation.data()->regions)) {
        if (!(region.length > 0 && U2Region(0, advSequenceContext->getSequenceLength()).contains(region))) {
            fitsWell = false;
            break;
        }
    }
    if (!fitsWell) {
        prevLocation.data()->regions.clear();
        prevLocation.data()->regions << U2Region();
    }
}

void FindEnzymesDialog::sl_onSelectionModified(int visible, int selected) {
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

void FindEnzymesDialog::sl_updateVisibleEnzymes() {
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
    enzSel->setEnzymesList(visibleEnzymes);
}

void FindEnzymesDialog::accept() {
    if (advSequenceContext.isNull()) {
        QMessageBox::critical(this, tr("Error!"), tr("Sequence has been alredy closed."));
        return;
    }

    QList<SEnzymeData> selectedEnzymes = enzSel->getSelectedEnzymes();
    bool ok = false;
    U2Location searchLocation = regionSelector->getLocation(&ok);
    U2Location excludeLocation;
    if (excludeCheckbox->isChecked()) {
        bool prevOk = ok;
        excludeLocation = excludeRegionSelector->getLocation(&ok);
        ok = prevOk && ok;
    }
    if (!ok) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("Invalid 'Search in' or 'Uncut' region/location!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(tr("Given region or genbank location is invalid, please correct it."));
        msgBox->exec();
        CHECK(!msgBox.isNull(), );
        return;
    }

    QVector<U2Region> searchRegionsOutsideExcluded = searchLocation.data()->regions;
    for (const U2Region& excludedRegion : qAsConst(excludeLocation.data()->regions)) {
        for (const U2Region& searchRegion : qAsConst(searchLocation.data()->regions)) {
            if (excludedRegion.contains(searchRegion)) {
                searchRegionsOutsideExcluded.removeAll(searchRegion);
            }
        }
    }
    if (searchRegionsOutsideExcluded.isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("'Uncut' region/location fully contains 'Search in' inside it!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(tr("Nowhere to search!"));
        msgBox->exec();
        CHECK(!msgBox.isNull(), );
        return;
    }

    if (selectedEnzymes.isEmpty()) {
        int ret = QMessageBox::question(this,
                                        windowTitle(),
                                        tr("<html><body align=\"center\">No enzymes are selected! Do you want to turn off <br>enzymes annotations highlighting?</body></html>"),
                                        QMessageBox::Yes,
                                        QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            QAction* toggleAction = AutoAnnotationUtils::findAutoAnnotationsToggleAction(advSequenceContext.data(), ANNOTATION_GROUP_ENZYME);
            if (toggleAction) {
                toggleAction->setChecked(false);
            }
            saveSettings();
            QDialog::accept();
        }
        return;
    }

    int maxHitVal = maxHitSB->value(), minHitVal = minHitSB->value();
    if (maxHitVal == ANY_VALUE) {
        maxHitVal = INT_MAX;
    }
    if (minHitVal == ANY_VALUE) {
        minHitVal = 1;
    }

    if (minHitVal > maxHitVal) {
        QMessageBox::critical(this, tr("Error!"), tr("Minimum hit value must be lesser or equal then maximum!"));
        return;
    }

    if (FindEnzymesAutoAnnotationUpdater::isTooManyAnnotationsInTheResult(advSequenceContext->getSequenceLength(), selectedEnzymes.size())) {
        QString message = tr("Too many results to render. Please reduce the search region or number of selected enzymes.");
        QMessageBox::critical(this, tr("Error!"), message, QMessageBox::Ok);
        return;
    }

    saveSettings();

    AutoAnnotationUtils::triggerAutoAnnotationsUpdate(advSequenceContext.data(), ANNOTATION_GROUP_ENZYME);

    QDialog::accept();
}

void FindEnzymesDialog::sl_updateEnzymesVisibilityWidgets() {
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

void FindEnzymesDialog::sl_selectAll() {
    cbSuppliers->setCheckedItems(EnzymesSelectorWidget::getLoadedSuppliers());
}

void FindEnzymesDialog::sl_selectNone() {
    cbSuppliers->setCheckedItems({});
}

void FindEnzymesDialog::sl_invertSelection() {
    const auto& suppliers = EnzymesSelectorWidget::getLoadedSuppliers();
    const auto& selectedSuppliers = cbSuppliers->getCheckedItems();
    QStringList newSelectedSuppliers;
    for (const auto& supplier : qAsConst(suppliers)) {
        CHECK_CONTINUE(!selectedSuppliers.contains(supplier));

        newSelectedSuppliers << supplier;
    }
    cbSuppliers->setCheckedItems(newSelectedSuppliers);
}

void FindEnzymesDialog::sl_minLengthChanged(int index) {
    if (cbMaxLength->currentIndex() <= index) {
        disconnect(cbMaxLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_maxLengthChanged);
        cbMaxLength->setCurrentIndex(index);
        connect(cbMaxLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_maxLengthChanged);
    }
    sl_updateVisibleEnzymes();
}

void FindEnzymesDialog::sl_maxLengthChanged(int index) {
    if (cbMinLength->currentIndex() >= index) {
        disconnect(cbMinLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_minLengthChanged);
        cbMinLength->setCurrentIndex(index);
        connect(cbMinLength, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FindEnzymesDialog::sl_minLengthChanged);
    }
    sl_updateVisibleEnzymes();
}

void FindEnzymesDialog::initSettings() {
    EnzymesSelectorWidget::initSelection();
    auto settings = AppContext::getSettings();
    bool useHitCountControl = settings->getValue(EnzymeSettings::ENABLE_HIT_COUNT, false).toBool();
    int minHitValue = settings->getValue(EnzymeSettings::MIN_HIT_VALUE, 1).toInt();
    int maxHitValue = settings->getValue(EnzymeSettings::MAX_HIT_VALUE, 2).toInt();
    auto min = settings->getValue(EnzymeSettings::MIN_ENZYME_LENGTH, RESTRICTION_SEQUENCE_LENGTH_VALUES.first()).toString();
    cbMinLength->setCurrentText(min);
    auto max = settings->getValue(EnzymeSettings::MAX_ENZYME_LENGTH, RESTRICTION_SEQUENCE_LENGTH_VALUES.last()).toString();
    cbMaxLength->setCurrentText(max);
    auto overhangTypeIndex = settings->getValue(EnzymeSettings::OVERHANG_TYPE, 0).toInt();
    cbOverhangType->setCurrentIndex(overhangTypeIndex);
    cbShowPalindromic->setChecked(settings->getValue(EnzymeSettings::SHOW_PALINDROMIC, 0).toBool());
    cbShowUninterrupted->setChecked(settings->getValue(EnzymeSettings::SHOW_UNINTERRUPTED, 0).toBool());
    cbShowNondegenerate->setChecked(settings->getValue(EnzymeSettings::SHOW_NONDEGENERATE, 0).toBool());

    filterGroupBox->setChecked(useHitCountControl);
    if (useHitCountControl) {
        minHitSB->setValue(minHitValue);
        maxHitSB->setValue(maxHitValue);
    } else {
        minHitSB->setValue(1);
        maxHitSB->setValue(2);
    }
}

void FindEnzymesDialog::saveSettings() {
    auto settings = AppContext::getSettings();
    settings->setValue(EnzymeSettings::ENABLE_HIT_COUNT, filterGroupBox->isChecked());
    if (filterGroupBox->isChecked()) {
        settings->setValue(EnzymeSettings::MIN_HIT_VALUE, minHitSB->value());
        settings->setValue(EnzymeSettings::MAX_HIT_VALUE, maxHitSB->value());
    } else {
        settings->setValue(EnzymeSettings::MIN_HIT_VALUE, 1);
        settings->setValue(EnzymeSettings::MAX_HIT_VALUE, INT_MAX);
    }

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
    CHECK(!advSequenceContext.isNull(), );

    U2SequenceObject* sequenceObject = advSequenceContext->getSequenceObject();
    // Empty search region is processed as 'Whole sequence' by auto-annotation task.
    bool ok = true;
    U2Location loc = regionSelector->isWholeSequenceSelected() ? U2Location() : regionSelector->getLocation(&ok);
    CHECK(ok, );
    FindEnzymesAutoAnnotationUpdater::setLastSearchLocationForObject(sequenceObject, loc);

    if (excludeCheckbox->isChecked()) {
        loc = excludeRegionSelector->getLocation(&ok);
        CHECK(ok, );
        FindEnzymesAutoAnnotationUpdater::setLastExcludeLocationForObject(sequenceObject, loc);
        FindEnzymesAutoAnnotationUpdater::setExcludeModeEnabledForObject(sequenceObject, true);
    } else {
        FindEnzymesAutoAnnotationUpdater::setExcludeModeEnabledForObject(sequenceObject, false);
    }

    enzSel->saveSettings();
}

//////////////////////////////////////////////////////////////////////////
// Tree item

EnzymeTreeItem::EnzymeTreeItem(const SEnzymeData& ed)
    : enzyme(ed) {
    setText(Column::Id, enzyme->id);
    setCheckState(Column::Id, Qt::Unchecked);
    setText(Column::Accession, enzyme->accession);
    setText(Column::Type, enzyme->type);
    setData(Column::Type, Qt::ToolTipRole, getTypeInfo());
    setText(Column::Sequence, enzyme->seq);
    setData(Column::Sequence, Qt::ToolTipRole, enzyme->generateEnzymeTooltip());
    setText(Column::Organism, enzyme->organizm);
    setData(Column::Organism, Qt::ToolTipRole, enzyme->organizm);
    setText(5, enzyme->suppliers.join("; "));
    setData(5, Qt::ToolTipRole, enzyme->suppliers.join("\n"));
}

bool EnzymeTreeItem::operator<(const QTreeWidgetItem& other) const {
    int col = treeWidget()->sortColumn();
    const EnzymeTreeItem& ei = (const EnzymeTreeItem&)other;
    if (col == 0) {
        bool eq = enzyme->id == ei.enzyme->id;
        if (!eq) {
            return enzyme->id < ei.enzyme->id;
        }
        return this < &ei;
    }
    return text(col) < ei.text(col);
}

QString EnzymeTreeItem::getEnzymeInfo() const {
    QString result;
    result += QString("<a href=\"http://rebase.neb.com/rebase/enz/%1.html\">%1</a>")
                  .arg(text(Column::Id));
    if (enzymesNumber != INCORRECT_ENZYMES_NUMBER) {
        if (enzymesNumber > MAXIMUM_ENZYMES_NUMBER) {
            result += tr(" (>%1 sites)").arg(MAXIMUM_ENZYMES_NUMBER);
        } else {
            result += " (" + tr("%n sites", "", enzymesNumber) + ")";
        }
    }
    auto typeString = data(Column::Type, Qt::ToolTipRole).toString();
    if (!typeString.isEmpty()) {
        auto lower = typeString.front().toLower();
        typeString = typeString.replace(0, 1, lower);
        result += ": " + typeString;
    }
    result += data(Column::Sequence, Qt::ToolTipRole).toString();
    return result;
}

QString EnzymeTreeItem::getTypeInfo() const {
    auto type = text(Column::Type);
    QString result;
    if (type == "M") {
        result = tr("An orphan methylase,<br>not associated with a restriction enzyme or specificity subunit");
    } else if (type.size() == 2) {
        if (type == "IE") {
            result = tr("An intron-encoded (homing) endonuclease");
        } else if (type.startsWith("R")) {
            result = tr("Type %1 restriction enzyme").arg(type.back());
        } else if (type.startsWith("M")) {
            result = tr("Type %1 methylase").arg(type.back());
        }
    } else if (type.size() == 3) {
        if (type.startsWith("R") && type.endsWith("*")) {
            result = tr("Type %1 restriction enzyme,<br>but only recognizes the sequence when it is methylated").arg(type.at(1));
        } else if (type.startsWith("RM")) {
            result = tr("Type %1 enzyme, which acts as both -<br>a restriction enzyme and a methylase").arg(type.back());
        }
    }

    return result;
}

EnzymeGroupTreeItem::EnzymeGroupTreeItem(const QString& _s)
    : s(_s) {
    updateVisual();
}

void EnzymeGroupTreeItem::updateVisual() {
    int numChilds = childCount();
    checkedEnzymes.clear();
    for (int i = 0; i < numChilds; i++) {
        EnzymeTreeItem* item = static_cast<EnzymeTreeItem*>(child(i));
        if (item->checkState(0) == Qt::Checked) {
            checkedEnzymes.insert(item);
        }
    }
    QString text0 = s + " (" + QString::number(checkedEnzymes.size()) + ", " + QString::number(numChilds) + ")";
    setText(0, text0);

    if (numChilds > 0) {
        QString text4 = (static_cast<EnzymeTreeItem*>(child(0)))->enzyme->id;
        if (childCount() > 1) {
            text4 += " .. " + (static_cast<EnzymeTreeItem*>(child(numChilds - 1)))->enzyme->id;
        }
        setText(4, text4);
    }
}

bool EnzymeGroupTreeItem::operator<(const QTreeWidgetItem& other) const {
    if (other.parent() != nullptr) {
        return true;
    }
    int col = treeWidget()->sortColumn();
    return text(col) < other.text(col);
}
}  // namespace U2