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

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPointer>

#include <U2Algorithm/EnzymeModel.h>

#include <U2View/ADVSequenceObjectContext.h>

#include <U2Gui/MainWindow.h>

#include <ui_EnzymesSelectorWidget.h>
#include <ui_FindEnzymesDialog.h>

namespace U2 {

class ADVSequenceObjectContext;
class CreateAnnotationWidgetController;
class EnzymeGroupTreeItem;
class EnzymeTreeItem;
class RegionSelector;

class EnzymesSelectorWidget : public QWidget, public Ui_EnzymesSelectorWidget {
    Q_OBJECT
public:
    EnzymesSelectorWidget(const QPointer<ADVSequenceObjectContext>& advSequenceContext = nullptr, QWidget* parent = nullptr);
    ~EnzymesSelectorWidget() override;

    static void setupSettings();
    static void saveSettings();
    static void initSelection();
    static const QList<SEnzymeData>& getLoadedEnzymes();
    static const QStringList& getLoadedSuppliers();
    QList<SEnzymeData> getSelectedEnzymes();
    /*
     * Get enzyme tree item by this enzyme.
     * \param enzyme An enzyme we should find tree item for.
     * \return Returns pointer to item (if exists).
    **/
    EnzymeTreeItem* getEnzymeTreeItemByEnzymeData(const SEnzymeData& enzyme) const;
    int getNumSelected();
    int getTotalNumber() const {
        return totalEnzymes;
    }

    void setEnzymesList(const QList<SEnzymeData>& enzymes);

signals:
    void si_selectionModified(int, int);
    void si_newEnzymeFileLoaded();

private slots:
    void sl_openEnzymesFile();
    void sl_saveEnzymesFile();
    void sl_selectAll();
    void sl_selectNone();
    void sl_selectByLength();
    void sl_inverseSelection();
    void sl_saveSelectionToFile();
    void sl_loadSelectionFromFile();
    void sl_openDBPage();
    void sl_itemChanged(QTreeWidgetItem* item, int col);
    void sl_filterTextChanged(const QString& filterText);
    void sl_findSingleEnzymeTaskStateChanged();

private:
    static void calculateSuppliers();

    void loadFile(const QString& url);
    void saveFile(const QString& url);
    void updateStatus();
    int gatherCheckedNamesListString(QString& checkedNamesList) const;

    EnzymeGroupTreeItem* findGroupItem(const QString& s, bool create);

    static QList<SEnzymeData> loadedEnzymes;
    // saves selection between calls to getSelectedEnzymes()
    static QSet<QString> lastSelection;
    static QStringList loadedSuppliers;

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    int totalEnzymes;
    bool ignoreItemChecks;
    int minLength;
};

class FindEnzymesDialog : public QDialog, public Ui_FindEnzymesDialog {
    Q_OBJECT
public:
    FindEnzymesDialog(const QPointer<ADVSequenceObjectContext>& advSequenceContext);
    void accept() override;

private slots:
    void sl_onSelectionModified(int visible, int selected);
    void sl_updateVisibleEnzymes();
    void sl_updateEnzymesVisibilityWidgets();
    void sl_selectAll();
    void sl_selectNone();
    void sl_invertSelection();
    void sl_minLengthChanged(int index);
    void sl_maxLengthChanged(int index);

private:
    void initSettings();
    void saveSettings();

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    EnzymesSelectorWidget* enzSel;
    RegionSelector* regionSelector;
    RegionSelector* excludeRegionSelector;
    QCheckBox* excludeCheckbox;
};

class EnzymeGroupTreeItem : public QTreeWidgetItem {
public:
    EnzymeGroupTreeItem(const QString& s);
    void updateVisual();
    QString s;
    QSet<EnzymeTreeItem*> checkedEnzymes;
    bool operator<(const QTreeWidgetItem& other) const override;
};

class EnzymeTreeItem : public QTreeWidgetItem {
    Q_DECLARE_TR_FUNCTIONS(EnzymeTreeItem)
public:
    EnzymeTreeItem(const SEnzymeData& ed);
    const SEnzymeData enzyme;
    static constexpr int INCORRECT_ENZYMES_NUMBER = -1;
    static constexpr int MAXIMUM_ENZYMES_NUMBER = 10'000;

    // Number of enzymes in the current sequence
    int enzymesNumber = INCORRECT_ENZYMES_NUMBER;
    // True if FindEnzymesTask, which calculates number of enzymes,
    // has already been run
    bool hasNumberCalculationTask = false;
    bool operator<(const QTreeWidgetItem& other) const override;
    // Get text information about this enzyme
    QString getEnzymeInfo() const;

private:
    enum Column {
        Id = 0,
        Accession,
        Type,
        Sequence,
        Organism,
    };

    QString getTypeInfo() const;
};

}  // namespace U2
