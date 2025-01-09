/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPointer>

#include <U2Algorithm/EnzymeModel.h>

#include <U2View/ADVSequenceObjectContext.h>

#include <U2Gui/MainWindow.h>

#include <ui_EnzymesSelectorWidget.h>
#include <ui_ResultsCountFilter.h>

namespace U2 {

class ADVSequenceObjectContext;
class CreateAnnotationWidgetController;
class EnzymeGroupTreeItem;
class EnzymeTreeItem;
class RegionSelector;

class EnzymesSelectorWidget : public QWidget, public Ui_EnzymesSelectorWidget {
    Q_OBJECT
public:
    EnzymesSelectorWidget(QWidget* parent, const QPointer<ADVSequenceObjectContext>& advSequenceContext = nullptr);
    ~EnzymesSelectorWidget() override;

    static void setupSettings();
    static void initSelection();
    static const QList<SEnzymeData>& getLoadedEnzymes();
    static const QStringList& getLoadedSuppliers();
    QList<SEnzymeData> getSelectedEnzymes() const;
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

    void saveSettings();

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
    void sl_filterConditionsChanged();
    void sl_findSingleEnzymeTaskStateChanged();

private slots:
    void sl_onSelectionModified(int visible, int selected);
    void sl_updateVisibleEnzymes();
    void sl_updateEnzymesVisibilityWidgets();
    void sl_selectAllSuppliers();
    void sl_selectNoneSuppliers();
    void sl_invertSelection();
    void sl_minLengthChanged(int index);
    void sl_maxLengthChanged(int index);

private:
    static void calculateSuppliers();

    void initSettings();

    void loadFile(const QString& url);
    void saveFile(const QString& url);
    void updateStatus();
    int gatherCheckedNamesListString(QString& checkedNamesList) const;

    static constexpr int FILTER_BY_NAME = 0;
    static constexpr int FILTER_BY_SEQUENCE = 1;

    EnzymeGroupTreeItem* findGroupItem(const QString& s, bool create);

    static QList<SEnzymeData> loadedEnzymes;
    // saves selection between calls to getSelectedEnzymes()
    static QSet<QString> lastSelection;
    static QStringList loadedSuppliers;

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    int totalEnzymes = 0;
    bool ignoreItemChecks = false;
    int minLength = 1;
};

class ResultsCountFilter : public QWidget, public Ui_ResultsCountFilter {
    Q_OBJECT
public:
    ResultsCountFilter(QWidget* parent);

    void saveSettings();

private:
    void initSettings();

};

class RegionSelectorWithExclude : public QWidget {
public:
    RegionSelectorWithExclude(QWidget* parent, const QPointer<ADVSequenceObjectContext>& advSequenceContext);

    U2Location getRegionSelectorLocation(bool* ok) const;
    U2Location getExcludeRegionSelectorLocation(bool* ok) const;
    bool isExcludeCheckboxChecked() const;

    void saveSettings();

private:
    void fixPreviousLocation(U2Location& prevLocation) const;

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    RegionSelector* regionSelector = nullptr;
    RegionSelector* excludeRegionSelector = nullptr;
    QCheckBox* excludeCheckbox = nullptr;

};

class FindEnzymesDialog : public QDialog {
    Q_OBJECT
public:
    FindEnzymesDialog(QWidget* parent, const QPointer<ADVSequenceObjectContext>& advSequenceContext);
    void accept() override;

private:
    void saveSettings();

    QPointer<ADVSequenceObjectContext> advSequenceContext;

    EnzymesSelectorWidget* enzSel = nullptr;
    ResultsCountFilter* countFilter = nullptr;
    RegionSelectorWithExclude* regionSelector = nullptr;
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
