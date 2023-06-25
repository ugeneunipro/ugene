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

#include "FindEnzymesDialog.h"

#include <QCoreApplication>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/ComboBoxWithCheckBoxes.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/RegionSelectorWithExcludedRegion.h>

#include <U2View/ADVSequenceObjectContext.h>
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

EnzymesSelectorWidget::EnzymesSelectorWidget() {
    setupUi(this);
    ignoreItemChecks = false;

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
    connect(enzymesFilterEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_filterTextChanged(const QString&)));

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

QList<SEnzymeData> EnzymesSelectorWidget::getLoadedEnzymes() {
    if (loadedEnzymes.isEmpty()) {
        U2OpStatus2Log os;
        QString lastUsedFile = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();
        loadedEnzymes = EnzymesIO::readEnzymes(lastUsedFile, os);
        CHECK_OP(os, {});

        calculateSuppliers();
    }
    return loadedEnzymes;
}

QStringList EnzymesSelectorWidget::getLoadedSuppliers() {
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
    std::sort(loadedSuppliers.begin(), loadedSuppliers.end(), [](const QString& first, const QString& second) {
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
        if (!QFileInfo(url).exists()) {
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
    emit si_newEnzimeFileLoaded();
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
        EnzymeTreeItem* item = new EnzymeTreeItem(enz);
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
    connect(tree, &QTreeWidget::itemSelectionChanged, this, [this]() {
        auto item = tree->currentItem();
        EnzymeTreeItem* ei = dynamic_cast<EnzymeTreeItem*>(item);
        EnzymeGroupTreeItem* gi = dynamic_cast<EnzymeGroupTreeItem*>(item);
        if (ei != nullptr) {
            teSelectedEnzymeInfo->setHtml(ei->getEnzymeInfo());
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
        EnzymeGroupTreeItem* gi = new EnzymeGroupTreeItem(s);
        tree->addTopLevelItem(gi);
        return gi;
    }
    return nullptr;
}

void EnzymesSelectorWidget::sl_filterTextChanged(const QString& filterText) {
    for (int i = 0, n = tree->topLevelItemCount(); i < n; ++i) {
        auto gi = static_cast<EnzymeGroupTreeItem*>(tree->topLevelItem(i));
        int numHiddenItems = 0;
        int itemCount = gi->childCount();
        for (int j = 0; j < itemCount; ++j) {
            auto item = static_cast<EnzymeTreeItem*>(gi->child(j));
            if (item->enzyme->id.contains(filterText, Qt::CaseInsensitive)) {
                item->setHidden(false);
            } else {
                item->setHidden(true);
                ++numHiddenItems;
            }
        }
        gi->setHidden(numHiddenItems == itemCount);
    }
}

void EnzymesSelectorWidget::updateStatus() {
    QString checkedNamesListString;
    int nChecked = gatherCheckedNamesListString(checkedNamesListString);
    if (nChecked > 1000) {
        checkedEnzymesEdit->setPlainText(tr("%1 sites selected. Click \"Save selection\" to export them to the separate file").arg(nChecked));
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
            emit si_newEnzimeFileLoaded();
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

FindEnzymesDialog::FindEnzymesDialog(ADVSequenceObjectContext* advSequenceContext)
    : QDialog(advSequenceContext->getAnnotatedDNAView()->getWidget()), advSequenceContext(advSequenceContext) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930747");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    maxHitSB->setMaximum(INT_MAX);
    minHitSB->setMaximum(INT_MAX);

    maxHitSB->setMinimum(ANY_VALUE);
    minHitSB->setMinimum(ANY_VALUE);

    regionSelector = new RegionSelectorWithExcludedRegion(this,
                                                          advSequenceContext->getSequenceLength(),
                                                          advSequenceContext->getSequenceSelection(),
                                                          advSequenceContext->getSequenceObject()->isCircular());
    searchRegionLayout->addWidget(regionSelector);

    initSettings();

    QVBoxLayout* vl = new QVBoxLayout();
    enzSel = new EnzymesSelectorWidget();
    vl->setMargin(0);
    vl->addWidget(enzSel);
    enzymesSelectorWidget->setLayout(vl);
    enzymesSelectorWidget->setMinimumSize(enzSel->size());

    connect(cbSuppliers, &ComboBoxWithCheckBoxes::si_checkedChanged, this, &FindEnzymesDialog::sl_handleSupplierSelectionChange);
    connect(enzSel, &EnzymesSelectorWidget::si_newEnzimeFileLoaded, this, &FindEnzymesDialog::sl_updateSuppliers);
    sl_updateSuppliers();

    connect(pbSelectAll, &QPushButton::clicked, this, &FindEnzymesDialog::sl_selectAll);
    connect(pbSelectNone, &QPushButton::clicked, this, &FindEnzymesDialog::sl_selectNone);
    connect(pbInvertSelection, &QPushButton::clicked, this, &FindEnzymesDialog::sl_invertSelection);

    connect(enzSel, SIGNAL(si_selectionModified(int, int)), SLOT(sl_onSelectionModified(int, int)));
    sl_onSelectionModified(enzSel->getTotalNumber(), enzSel->getNumSelected());
}

void FindEnzymesDialog::sl_onSelectionModified(int total, int nChecked) {
    statusLabel->setText(tr("Total number of enzymes: %1, selected %2").arg(total).arg(nChecked));
}

void FindEnzymesDialog::accept() {
    QList<SEnzymeData> selectedEnzymes = enzSel->getSelectedEnzymes();

    if (regionSelector->hasError()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Warning, L10N::errorTitle(), tr("Invalid 'Search' region!"), QMessageBox::Ok, this);
        msgBox->setInformativeText(regionSelector->getErrorMessage());
        msgBox->exec();
        CHECK(!msgBox.isNull(), );
        return;
    }

    if (selectedEnzymes.isEmpty()) {
        int ret = QMessageBox::question(this, windowTitle(), tr("<html><body align=\"center\">No enzymes are selected! Do you want to turn off <br>enzymes annotations highlighting?</body></html>"), QMessageBox::Yes, QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            QAction* toggleAction = AutoAnnotationUtils::findAutoAnnotationsToggleAction(advSequenceContext, ANNOTATION_GROUP_ENZYME);
            if (toggleAction) {
                toggleAction->setChecked(false);
            }
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

    AutoAnnotationUtils::triggerAutoAnnotationsUpdate(advSequenceContext, ANNOTATION_GROUP_ENZYME);

    QDialog::accept();
}

void FindEnzymesDialog::sl_handleSupplierSelectionChange(QStringList checkedSuppliers) {
    const auto enzymes = EnzymesSelectorWidget::getLoadedEnzymes();
    QList<SEnzymeData> visibleEnzymes;
    for (const auto& enzyme : qAsConst(enzymes)) {
        for (const auto& supplier : qAsConst(enzyme->suppliers)) {
            CHECK_CONTINUE(checkedSuppliers.contains(supplier));

            visibleEnzymes.append(enzyme);
            break;
        }
    }
    enzSel->setEnzymesList(visibleEnzymes);
    static const QString notDefinedTr = EnzymesIO::tr(EnzymesIO::NOT_DEFINED_SIGN);
    if (checkedSuppliers.contains(notDefinedTr)) {
        checkedSuppliers.replace(checkedSuppliers.indexOf(notDefinedTr), EnzymesIO::NOT_DEFINED_SIGN);
    }
    auto value = checkedSuppliers.join(SUPPLIERS_LIST_SEPARATOR);
    AppContext::getSettings()->setValue(EnzymeSettings::CHECKED_SUPPLIERS, value);
}

void FindEnzymesDialog::sl_updateSuppliers() {
    const auto& loadedSuppliers = EnzymesSelectorWidget::getLoadedSuppliers();
    cbSuppliers->clear();
    cbSuppliers->addItems(loadedSuppliers);
    QString selStr = AppContext::getSettings()->getValue(EnzymeSettings::CHECKED_SUPPLIERS).toString();
    static const QString notDefinedTr = EnzymesIO::tr(EnzymesIO::NOT_DEFINED_SIGN);
    auto suppliersList = selStr.isEmpty() ? loadedSuppliers : selStr.split(SUPPLIERS_LIST_SEPARATOR);
    if (suppliersList.contains(EnzymesIO::NOT_DEFINED_SIGN)) {
        suppliersList.replace(suppliersList.indexOf(EnzymesIO::NOT_DEFINED_SIGN), notDefinedTr);
    }
    if (selStr.isEmpty()) {
        suppliersList.removeOne(notDefinedTr);
    }
    cbSuppliers->setCheckedItems(suppliersList);
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

void FindEnzymesDialog::initSettings() {
    EnzymesSelectorWidget::initSelection();
    bool useHitCountControl = AppContext::getSettings()->getValue(EnzymeSettings::ENABLE_HIT_COUNT, false).toBool();
    int minHitValue = AppContext::getSettings()->getValue(EnzymeSettings::MIN_HIT_VALUE, 1).toInt();
    int maxHitValue = AppContext::getSettings()->getValue(EnzymeSettings::MAX_HIT_VALUE, 2).toInt();

    U2SequenceObject* sequenceObject = advSequenceContext->getSequenceObject();
    U2Region searchRegion = FindEnzymesAutoAnnotationUpdater::getLastSearchRegionForObject(sequenceObject);
    if (searchRegion.length > 0 && U2Region(0, advSequenceContext->getSequenceLength()).contains(searchRegion)) {
        regionSelector->setIncludeRegion(searchRegion);
    }

    U2Region excludeRegion = FindEnzymesAutoAnnotationUpdater::getLastExcludeRegionForObject(sequenceObject);
    if (excludeRegion.length > 0) {
        regionSelector->setExcludeRegion(excludeRegion);
        regionSelector->setExcludedCheckboxChecked(true);
    } else {
        regionSelector->setExcludedCheckboxChecked(false);
    }

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
    AppContext::getSettings()->setValue(EnzymeSettings::ENABLE_HIT_COUNT, filterGroupBox->isChecked());
    if (filterGroupBox->isChecked()) {
        AppContext::getSettings()->setValue(EnzymeSettings::MIN_HIT_VALUE, minHitSB->value());
        AppContext::getSettings()->setValue(EnzymeSettings::MAX_HIT_VALUE, maxHitSB->value());
    } else {
        AppContext::getSettings()->setValue(EnzymeSettings::MIN_HIT_VALUE, 1);
        AppContext::getSettings()->setValue(EnzymeSettings::MAX_HIT_VALUE, INT_MAX);
    }

    U2SequenceObject* sequenceObject = advSequenceContext->getSequenceObject();
    // Empty search region is processed as 'Whole sequence' by auto-annotation task.
    U2Region searchRegion = regionSelector->isWholeSequenceSelected() ? U2Region() : regionSelector->getIncludeRegion();
    FindEnzymesAutoAnnotationUpdater::setLastSearchRegionForObject(sequenceObject, searchRegion);
    FindEnzymesAutoAnnotationUpdater::setLastExcludeRegionForObject(sequenceObject, regionSelector->getExcludeRegion());
    enzSel->saveSettings();
}

//////////////////////////////////////////////////////////////////////////
// Tree item

static const QString TOOLTIP_TAG = "<p style='font-family:Courier,monospace'><br><strong>3'&nbsp;</strong>%1<strong>&nbsp;5'</strong><br><strong>5'&nbsp;</strong>%2<strong>&nbsp;3'</strong><br></p>";
static const QString TOOLTIP_N_MARKER = "(N)<sub>%1</sub>";
static const QString TOOLTIP_FORWARD_MARKER = "<sup>&#x25BC;</sup>";
static const QString TOOLTIP_REVERSE_MARKER = "<sub>&#x25B2;</sub>";
static const QString TOOLTIP_SPACE = "<sub>&nbsp;</sub>";

EnzymeTreeItem::EnzymeTreeItem(const SEnzymeData& ed)
    : enzyme(ed) {
    setText(Column::Id, enzyme->id);
    setCheckState(Column::Id, Qt::Unchecked);
    setText(Column::Accession, enzyme->accession);
    setText(Column::Type, enzyme->type);
    setData(Column::Type, Qt::ToolTipRole, getTypeInfo());
    setText(Column::Sequence, enzyme->seq);
    setData(Column::Sequence, Qt::ToolTipRole, generateEnzymeTooltip());
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
    auto typeString = data(Column::Type, Qt::ToolTipRole).toString();
    if (!typeString.isEmpty()) {
        auto lower = typeString.front().toLower();
        typeString = typeString.replace(0, 1, lower);
        result += ": " + typeString;
    }
    result += data(Column::Sequence, Qt::ToolTipRole).toString();
    return result;
}

QString EnzymeTreeItem::generateEnzymeTooltip() const {
    // Enum, which shows either enzyme has a cut inside of the sequence,
    // to the left of the sequence or to the right of the sequence
    // Ns in this scope means "if this enzyme has N character on to the left or
    // to the right of the enzyme, or no N charactest at all"
    enum class Ns {
        // Enzyme has N charactes to the left
        // Example:
        // N N N A C G T
        //       T G C A
        Left,
        // Enzyme has N charactes to the right
        // Example:
        // A C G T N N N
        // T G C A
        Right,
        // Enzyme doesn't have N characters
        // Example:
        // A C G T
        // T G C A
        No
    };

    auto alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    auto seqComplement = DNASequenceUtils::reverseComplement(enzyme->seq, alphabet);
    if (enzyme->cutDirect == ENZYME_CUT_UNKNOWN) {
        return TOOLTIP_TAG.arg(QString(enzyme->seq)).arg(QString(DNASequenceUtils::complement(enzyme->seq, alphabet)));
    }

    auto enzymeSize = enzyme->seq.size();
    // Calculates nubmer of Ns outside of enzyme
    // returns side to shift and shift number
    auto calculateNShift = [enzymeSize](int cut, bool forward) -> QPair<Ns, int> {
        QPair<Ns, int> calculateNShiftRes = { Ns::No, 0 };
        if (cut < 0) {
            calculateNShiftRes.first = forward ? Ns::Left : Ns::Right;
            calculateNShiftRes.second = qAbs(cut);
        } else if (enzymeSize < cut) {
            calculateNShiftRes.first = forward ? Ns::Right : Ns::Left;
            calculateNShiftRes.second = cut - enzymeSize;
        }
        return calculateNShiftRes;
    };
    // generates tooltop elements, which are located outside of enzyme
    // these elements will be joined in the end
    // returns list of elements
    auto generateTooltipElements = [](int out, int in, bool forward, Ns type, bool otherHasLeftOut, bool otherHasLeftIn, bool otherHasRightIn, bool otherHasRightOut) -> QStringList {
        // Look at the enxyme:
        // N N N N N A C G T
        //       N N T G C A
        // This enzyme is separated to the several parts (name "parts" is not official, just used here)
        //   Out     In    Main
        // |N N N| |N N| |A C G T|
        //         |N N| |T G C A|
        // @generateOutPartElements generates the "Out" part
        // Returns the list, wich contails the cut elements and Ns if this enzyme has the "Out" part,
        // Or spaces if does not have
        auto generateOutPartElements = [out, in, forward, type, otherHasLeftOut, otherHasRightOut]() -> QStringList {
            QStringList generateOutPartElementsResult;
            if (out != 0) {
                switch (type) {
                case Ns::Left:
                    generateOutPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                    generateOutPartElementsResult << TOOLTIP_N_MARKER.arg(out);
                    break;
                case Ns::Right:
                    generateOutPartElementsResult << TOOLTIP_N_MARKER.arg(out);
                    generateOutPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                    break;
                }
            } else if (out == 0 && otherHasLeftOut && type == Ns::Left) {
                generateOutPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
            } else if (out == 0 && otherHasRightOut && type == Ns::Right) {
                generateOutPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
            }
            return generateOutPartElementsResult;
        };
        // THe same as @generateOutPartElements but for the "In" part
        auto generateInPartElements = [out, in, forward, type, otherHasLeftOut, otherHasLeftIn, otherHasRightIn, otherHasRightOut]() -> QStringList {
            QStringList generateInPartElementsResult;
            if (in != 0) {
                if (out == 0) {
                    switch (type) {
                    case Ns::Left:
                        generateInPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                        generateInPartElementsResult << TOOLTIP_N_MARKER.arg(in);
                        break;
                    case Ns::Right:
                        generateInPartElementsResult << TOOLTIP_N_MARKER.arg(in);
                        generateInPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                        break;
                    }
                } else if (out != 0) {
                    generateInPartElementsResult << TOOLTIP_N_MARKER.arg(in);
                }
            } else if (in == 0 && otherHasLeftIn && type == Ns::Left) {
                if (otherHasLeftOut) {
                    generateInPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
                } else {
                    generateInPartElementsResult << QString("&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
                }
            } else if (in == 0 && otherHasRightIn && type == Ns::Right) {
                if (otherHasRightOut) {
                    generateInPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
                } else {
                    generateInPartElementsResult << QString("&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
                }
            }
            return generateInPartElementsResult;
        };
        QStringList result;
        switch (type) {
        case Ns::Left:
            result << generateOutPartElements();
            result << generateInPartElements();
            break;
        case Ns::Right:
            result << generateInPartElements();
            result << generateOutPartElements();
            break;
        }
        return result;
    };
    QPair<Ns, int> forwardNShift = calculateNShift(enzyme->cutDirect, true);
    QPair<Ns, int> reverseNShift = calculateNShift(enzyme->cutComplement, false);
    QStringList forwardTooltipElements;
    QStringList reverseTooltipElements;
    if (forwardNShift.first == reverseNShift.first && forwardNShift.first != Ns::No) {
        int forwardNOut = 0;
        int forwardNIn = 0;
        int reverseNOut = 0;
        int reverseNIn = 0;
        if (forwardNShift.second > reverseNShift.second) {
            forwardNOut = forwardNShift.second - reverseNShift.second;
            forwardNIn = reverseNShift.second;
            reverseNOut = 0;
            reverseNIn = reverseNShift.second;
        } else if (forwardNShift.second == reverseNShift.second) {
            forwardNOut = 0;
            forwardNIn = forwardNShift.second;
            reverseNOut = 0;
            reverseNIn = reverseNShift.second;
        } else if (forwardNShift.second < reverseNShift.second) {
            forwardNOut = 0;
            forwardNIn = forwardNShift.second;
            reverseNOut = reverseNShift.second - forwardNShift.second;
            reverseNIn = forwardNShift.second;
        }

         forwardTooltipElements = generateTooltipElements(forwardNOut, forwardNIn, true, forwardNShift.first,
                                                         (reverseNOut != 0 && reverseNShift.first == Ns::Left),
                                                         (reverseNIn != 0 && reverseNShift.first == Ns::Left),
                                                         (reverseNIn != 0 && reverseNShift.first == Ns::Right),
                                                         (reverseNOut != 0 && reverseNShift.first == Ns::Right));
         reverseTooltipElements = generateTooltipElements(reverseNOut, reverseNIn, false, reverseNShift.first,
                                                         (forwardNOut != 0 && forwardNShift.first == Ns::Left),
                                                         (forwardNIn != 0 && forwardNShift.first == Ns::Left),
                                                         (forwardNIn != 0 && forwardNShift.first == Ns::Right),
                                                         (forwardNOut != 0 && forwardNShift.first == Ns::Right));
    } else {
        if (forwardNShift.first != Ns::No) {
            forwardTooltipElements = generateTooltipElements(0, forwardNShift.second, true, forwardNShift.first,
                                                            false,
                                                            (reverseNShift.second != 0 && reverseNShift.first == Ns::Left),
                                                            (reverseNShift.second != 0 && reverseNShift.first == Ns::Right),
                                                            false);
        }
        if (reverseNShift.first != Ns::No) {
            reverseTooltipElements = generateTooltipElements(0, reverseNShift.second, false, reverseNShift.first,
                                                            false,
                                                            (forwardNShift.second != 0 && forwardNShift.first == Ns::Left),
                                                            (forwardNShift.second != 0 && forwardNShift.first == Ns::Right),
                                                            false);
        }
    }
    // generates the "Main" part (see @generateOutPartElements for details)
    auto generateMainPart = [enzymeSize](const QByteArray& seq, int cut, bool forward) -> QString {
        QString result;
        auto append2Result = [&result, forward](const QString& add) {
            if (forward) {
                result += add;
            } else {
                result.insert(0, add);
            }
        };
        auto removeSpaceFromResult = [&result, forward]() {
            if (forward) {
                result = result.left(result.size() - TOOLTIP_SPACE.size());
            } else {
                result = result.right(result.size() - TOOLTIP_SPACE.size());
            }
        };
        append2Result(TOOLTIP_SPACE);
        for (int i = 0; i < enzymeSize; i++) {
            if (i == cut) {
                removeSpaceFromResult();
                append2Result(forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
            }
            append2Result(QString(seq.at(i)));
            append2Result(TOOLTIP_SPACE);
        }
        if (seq.size() == cut) {
            removeSpaceFromResult();
            append2Result(forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
        }
        return result;
    };
    QString forwardMainPart = generateMainPart(enzyme->seq, enzyme->cutDirect, true);
    QString reverseMainPart = generateMainPart(seqComplement, enzyme->cutComplement, false);

    // Join parts to a single tooltip
    auto generateTooltip = [](Ns type, const QStringList& elements, const QString& mainPart) -> QString {
        QString result;
        // Join elements which are calculated in @generateTooltipElements
        auto joinElements = [&elements]() -> QString {
            QString joinElementsResult;
            for (const auto el : qAsConst(elements)) {
                if (el == TOOLTIP_FORWARD_MARKER || el == TOOLTIP_REVERSE_MARKER) {
                    if (!joinElementsResult.isEmpty()) {
                        joinElementsResult = joinElementsResult.left(joinElementsResult.size() - TOOLTIP_SPACE.size());
                    }
                    joinElementsResult += el;
                } else {
                    joinElementsResult += el;
                    if (el != TOOLTIP_SPACE) {
                        joinElementsResult += TOOLTIP_SPACE;
                    }
                }
            }
            if (joinElementsResult.endsWith(TOOLTIP_SPACE)) {
                joinElementsResult = joinElementsResult.left(joinElementsResult.size() - TOOLTIP_SPACE.size());
            }
            return joinElementsResult;
        };
        if (type == Ns::Left) {
            result += joinElements();
        }
        result += mainPart;
        if (type == Ns::Right) {
            result += joinElements();
        }
        return result;
    };
    QString forwardTooltip = generateTooltip(forwardNShift.first, forwardTooltipElements, forwardMainPart);
    QString reverseTooltip = generateTooltip(reverseNShift.first, reverseTooltipElements, reverseMainPart);
    if (forwardNShift.first == Ns::Left && reverseNShift.first != Ns::Left) {
        if (reverseNShift.first == Ns::Right) {
            forwardTooltip += QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        }
        reverseTooltip.insert(0, QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE));
    } else if (reverseNShift.first == Ns::Left && forwardNShift.first != Ns::Left) {
        forwardTooltip.insert(0, QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE));
        if (forwardNShift.first == Ns::Right) {
            reverseTooltip += QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        }
    }

    return TOOLTIP_TAG.arg(forwardTooltip).arg(reverseTooltip);
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
