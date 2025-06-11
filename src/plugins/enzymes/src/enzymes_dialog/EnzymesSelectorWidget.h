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

#pragma once

#include <QWidget>
#include <QPointer>

#include <U2Algorithm/EnzymeModel.h>

#include <U2View/ADVSequenceObjectContext.h>

#include <ui_EnzymesSelectorWidget.h>

namespace U2 {

class EnzymeGroupTreeItem;
class EnzymeTreeItem;

/**
 * @brief This class describes a widget, which contains enzyme tree, selected enzymes, enzyme info
 * and enzyme filter group box.
 */
class EnzymesSelectorWidget : public QWidget, public Ui_EnzymesSelectorWidget {
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param parent parent widget.
     */
    EnzymesSelectorWidget(QWidget* parent);
    ~EnzymesSelectorWidget() override;

    /**
    * @brief Load file with enzymes settings from .ini file.
    */
    static void setupSettings();
    /**
     * @brief Load selected enzymes from .ini file.
     */
    static void initSelection();
    /**
     * @return Returns list of loaded enzymes.
     */
    static const QList<SEnzymeData>& getLoadedEnzymes();
    /**
     * @return Returns list of selected suppliers.
     */
    static const QStringList& getLoadedSuppliers();
    /**
    * @brief Set sequence context. If it is nullptr,
    * that means that there is no sequence opened (e.g. widget is created in Query Designer).
    * @param advSequenceContext Context of opened sequence.
    */
    void setSequenceContext(const QPointer<ADVSequenceObjectContext>& advSequenceContext);
    /**
     * @return Returns list of selected enzymes.
     */
    QList<SEnzymeData> getSelectedEnzymes() const;
    /*
     * Get enzyme tree item by this enzyme.
     * \param enzyme An enzyme we should find tree item for.
     * \return Returns pointer to item (if exists).
    **/
    EnzymeTreeItem* getEnzymeTreeItemByEnzymeData(const SEnzymeData& enzyme) const;
    /**
     * @return Returns number of selected enzymes.
     */
    int getNumSelected();
    /**
     * @return Returns number of loaded enzymes.
     */
    int getTotalNumber() const;

    /**
     * @brief Saves dialog setting to the .ini file.
     */
    void saveSettings();

    /**
     * @brief Set the following list of enzymes to be selected.
     * \param enzymes Lit of enzymes.
     */
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
    void sl_itemSelectionChanged();
    void sl_colorThemeSwitched();

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

    QPointer<ADVSequenceObjectContext> advSequenceContext = nullptr;

    int totalEnzymes = 0;
    bool ignoreItemChecks = false;
    int minLength = 1;
};

}  // namespace U2
