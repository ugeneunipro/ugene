/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_EDITOR_H_
#define _U2_MSA_EDITOR_H_

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2Msa.h>

#include "MaEditor.h"
#include "MsaEditorWgt.h"
#include "PhyTrees/MSAEditorTreeManager.h"

namespace U2 {

class PairwiseAlignmentTask;
class U2SequenceObject;

class PairwiseAlignmentWidgetsSettings {
public:
    PairwiseAlignmentWidgetsSettings()
        : firstSequenceId(U2MsaRow::INVALID_ROW_ID),
          secondSequenceId(U2MsaRow::INVALID_ROW_ID), inNewWindow(true),
          showSequenceWidget(true), showAlgorithmWidget(false),
          showOutputWidget(false), sequenceSelectionModeOn(false) {
    }

    qint64 firstSequenceId;
    qint64 secondSequenceId;
    QString algorithmName;
    bool inNewWindow;
    QString resultFileName;
    QPointer<PairwiseAlignmentTask> pairwiseAlignmentTask;
    bool showSequenceWidget;
    bool showAlgorithmWidget;
    bool showOutputWidget;
    bool sequenceSelectionModeOn;

    QVariantMap customSettings;
};

class U2VIEW_EXPORT MSAEditor : public MaEditor {
    Q_OBJECT
    Q_DISABLE_COPY(MSAEditor)

    friend class MSAEditorTreeViewerUI;
    friend class SequenceAreaRenderer;
    friend class SequenceWithChromatogramAreaRenderer;

public:
    MSAEditor(const QString &viewName, MultipleSequenceAlignmentObject *obj);
    ~MSAEditor();

    QString getSettingsRoot() const override {
        return MSAE_SETTINGS_ROOT;
    }

    MultipleSequenceAlignmentObject *getMaObject() const override {
        return qobject_cast<MultipleSequenceAlignmentObject *>(maObject);
    }

    virtual void buildStaticToolbar(QToolBar *tb) override;

    void buildMenu(QMenu *m, const QString &type) override;

    MsaEditorWgt *getUI() const override;

    //Return alignment row that is displayed on target line in MSAEditor
    MultipleSequenceAlignmentRow getRowByViewRowIndex(int viewRowIndex) const;

    PairwiseAlignmentWidgetsSettings *getPairwiseAlignmentWidgetsSettings() const {
        return pairwiseAlignmentWidgetsSettings;
    }

    MSAEditorTreeManager *getTreeManager() {
        return &treeManager;
    }

    void buildTree();

    QString getReferenceRowName() const override;

    char getReferenceCharAt(int pos) const override;

    void sortSequences(const MultipleAlignment::SortType &sortType, const MultipleAlignment::Order &sortOrder);

protected slots:
    void sl_onContextMenuRequested(const QPoint &pos);

    void sl_buildTree();
    void sl_align();
    void sl_addToAlignment();
    void sl_searchInSequences();
    void sl_searchInSequenceNames();
    void sl_realignSomeSequences();
    void sl_setSeqAsReference();
    void sl_unsetReferenceSeq();

    void sl_showTreeOP();
    void sl_hideTreeOP();
    void sl_rowsRemoved(const QList<qint64> &rowIds);
    void sl_updateRealignAction();
    void sl_showCustomSettings();
    void sl_sortSequencesByName();
    void sl_sortSequencesByLength();
    void sl_sortSequencesByLeadingGap();
    void sl_convertBetweenDnaAndRnaAlphabets();

protected:
    QWidget *createWidget() override;
    bool eventFilter(QObject *o, QEvent *e) override;
    bool onObjectRemoved(GObject *obj) override;
    void onObjectRenamed(GObject *obj, const QString &oldName) override;
    bool onCloseEvent() override;

    void addCopyPasteMenu(QMenu *m) override;
    void addEditMenu(QMenu *m) override;
    void addSortMenu(QMenu *m);
    void addExportMenu(QMenu *m) override;
    void addAppearanceMenu(QMenu *m);
    void addColorsMenu(QMenu *m);
    void addHighlightingMenu(QMenu *m);
    void addNavigationMenu(QMenu *m);
    void addTreeMenu(QMenu *m);
    void addAdvancedMenu(QMenu *m);
    void addStatisticsMenu(QMenu *m);

    void updateActions() override;

    void initDragAndDropSupport();
    void alignSequencesFromObjectsToAlignment(const QList<GObject *> &objects);
    void alignSequencesFromFilesToAlignment();

public:
    QAction *buildTreeAction = nullptr;
    QAction *alignAction = nullptr;
    QAction *alignSequencesToAlignmentAction = nullptr;
    QAction *realignSomeSequenceAction = nullptr;
    QAction *setAsReferenceSequenceAction = nullptr;
    QAction *unsetReferenceSequenceAction = nullptr;
    QAction *gotoAction = nullptr;
    QAction *searchInSequencesAction = nullptr;
    QAction *searchInSequenceNamesAction = nullptr;
    QAction *openCustomSettingsAction = nullptr;
    QAction *sortByNameAscendingAction = nullptr;
    QAction *sortByNameDescendingAction = nullptr;
    QAction *sortByLengthAscendingAction = nullptr;
    QAction *sortByLengthDescendingAction = nullptr;
    QAction *sortByLeadingGapAscendingAction = nullptr;
    QAction *sortByLeadingGapDescendingAction = nullptr;

    QAction *convertDnaToRnaAction = nullptr;
    QAction *convertRnaToDnaAction = nullptr;

private:
    PairwiseAlignmentWidgetsSettings *pairwiseAlignmentWidgetsSettings = nullptr;
    MSAEditorTreeManager treeManager;
};

/** Set of custom menu actions in MSA editor. */
class U2VIEW_EXPORT MsaEditorMenuType : public GObjectViewMenuType {
public:
    /** "Align" button menu identifier. */
    const static QString ALIGN;
};

}    // namespace U2

#endif    // _U2_MSA_EDITOR_H_
