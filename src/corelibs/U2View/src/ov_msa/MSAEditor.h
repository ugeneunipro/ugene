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

#ifndef _U2_MSA_EDITOR_H_
#define _U2_MSA_EDITOR_H_

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2Msa.h>

#include "PhyTrees/MSAEditorTreeManager.h"
#include "MaEditor.h"
#include "MsaEditorWgt.h"

namespace U2 {

class PairwiseAlignmentTask;
class U2SequenceObject;

class PairwiseAlignmentWidgetsSettings {
public:
    PairwiseAlignmentWidgetsSettings()
        : firstSequenceId(U2MsaRow::INVALID_ROW_ID),
        secondSequenceId(U2MsaRow::INVALID_ROW_ID), inNewWindow(true),
        pairwiseAlignmentTask(NULL), showSequenceWidget(true), showAlgorithmWidget(false),
        showOutputWidget(false), sequenceSelectionModeOn(false)
    {

    }

    qint64 firstSequenceId;
    qint64 secondSequenceId;
    QString algorithmName;
    bool inNewWindow;
    QString resultFileName;
    PairwiseAlignmentTask* pairwiseAlignmentTask;
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
    MSAEditor(const QString& viewName, MultipleSequenceAlignmentObject* obj);
    ~MSAEditor();

    QString getSettingsRoot() const { return MSAE_SETTINGS_ROOT; }

    MultipleSequenceAlignmentObject* getMaObject() const { return qobject_cast<MultipleSequenceAlignmentObject*>(maObject); }

    virtual void buildStaticToolbar(QToolBar* tb);

    virtual void buildStaticMenu(QMenu* m);

    MsaEditorWgt* getUI() const;

    //Return alignment row that is displayed on target line in MSAEditor
    const MultipleSequenceAlignmentRow getRowByLineNumber(int lineNumber) const;

    PairwiseAlignmentWidgetsSettings* getPairwiseAlignmentWidgetsSettings() const { return pairwiseAlignmentWidgetsSettings; }

    MSAEditorTreeManager* getTreeManager() {return &treeManager;}

    void buildTree();

    QString getReferenceRowName() const;

    char getReferenceCharAt(int pos) const;

protected slots:
    void sl_onContextMenuRequested(const QPoint & pos);

    void sl_buildTree();
    void sl_align();
    void sl_addToAlignment();
    void sl_searchInSequences();
    void sl_realignSomeSequences();
    void sl_setSeqAsReference();
    void sl_unsetReferenceSeq();

    void sl_onSeqOrderChanged(const QStringList& order);
    void sl_showTreeOP();
    void sl_hideTreeOP();
    void sl_rowsRemoved(const QList<qint64> &rowIds);
    void sl_updateRealignAction();
    void sl_showCustomSettings();

protected:
    QWidget* createWidget();
    bool eventFilter(QObject* o, QEvent* e);
    virtual bool onObjectRemoved(GObject* obj);
    virtual void onObjectRenamed(GObject* obj, const QString& oldName);
    virtual bool onCloseEvent();

private:
    void addExportMenu(QMenu* m);
    void addAppearanceMenu(QMenu* m);
    void addColorsMenu(QMenu* m);
    void addHighlightingMenu(QMenu* m);
    void addNavigationMenu(QMenu* m);
    void addTreeMenu(QMenu* m);
    void addAdvancedMenu(QMenu* m);
    void addStatisticsMenu(QMenu* m);

    virtual void updateActions();

    void initDragAndDropSupport();
    void alignSequencesFromObjectsToAlignment(const QList<GObject*>& objects);
    void alignSequencesFromFilesToAlignment();

    QAction* buildTreeAction;
    QAction* alignAction;
    QAction* alignSequencesToAlignmentAction;
    QAction* realignSomeSequenceAction;
    QAction* setAsReferenceSequenceAction;
    QAction* unsetReferenceSequenceAction;
    QAction* gotoAction;
    QAction* searchInSequencesAction;
    QAction* openCustomSettingsAction;

    PairwiseAlignmentWidgetsSettings* pairwiseAlignmentWidgetsSettings;
    MSAEditorTreeManager           treeManager;
};

}   // namespace U2

#endif // _U2_MSA_EDITOR_H_
