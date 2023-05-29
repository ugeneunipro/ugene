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

    splitter->setStretchFactor(0, 5);
    splitter->setStretchFactor(1, 1);

    tree->setSortingEnabled(true);
    tree->sortByColumn(0, Qt::AscendingOrder);
    tree->setUniformRowHeights(true);
    tree->setColumnWidth(0, 110);  // id
    tree->setColumnWidth(1, 75);  // accession
    tree->setColumnWidth(2, 50);  // type

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
    } else {
        setEnzymesList(loadedEnzymes);
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

    setEnzymesList(loadedEnzymes);
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
            setEnzymesList(loadedEnzymes);
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
EnzymeTreeItem::EnzymeTreeItem(const SEnzymeData& ed)
    : enzyme(ed) {
    setText(0, enzyme->id);
    setCheckState(0, Qt::Unchecked);
    setText(1, enzyme->accession);
    setText(2, enzyme->type);
    setText(3, enzyme->seq);
    setData(3, Qt::ToolTipRole, enzyme->seq);
    setText(4, enzyme->organizm);  // todo: show cut sites
    setData(4, Qt::ToolTipRole, enzyme->organizm);
    auto suppliers = enzyme->suppliers.join("; ");
    setText(5, suppliers);
    setData(5, Qt::ToolTipRole, suppliers);
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
