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

#ifndef _U2_MSA_EDITOR_SEQUENCE_AREA_H_
#define _U2_MSA_EDITOR_SEQUENCE_AREA_H_

#include <QMenu>
#include <QToolBar>
#include <QWidget>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include "DeleteGapsDialog.h"
#include "ExportHighlightedDialogController.h"
#include "MaCollapseModel.h"
#include "MsaEditorUserModStepController.h"
#include "SaveSelectedSequenceFromMSADialogController.h"

#include "view_rendering/MaEditorSequenceArea.h"

namespace U2 {

class GObjectView;
class MsaColorScheme;
class MsaColorSchemeFactory;
class MsaColorSchemeRegistry;
class MSAEditor;
class MsaEditorWgt;
class MsaHighlightingScheme;
class MsaHighlightingSchemeFactory;
class MsaHighlightingSchemeRegistry;
class MaModificationInfo;
class MultipleSequenceAlignmentObject;
class Settings;
class SequenceAreaRenderer;

class ModificationType {
public:
    static const int NoType = 0;
    static const int Reverse = 1;
    static const int Complement = 3;
    static const int ReverseComplement = 7;

    ModificationType(int _type = NoType) : type(_type) {}

    ModificationType operator + (const ModificationType & another) {
        switch (type + another.type) {
        case NoType + NoType: return ModificationType(NoType);
        case NoType + Reverse: return ModificationType(Reverse);
        case NoType + Complement: return ModificationType(Complement);
        case NoType + ReverseComplement: return ModificationType(ReverseComplement);
        case Reverse + Reverse: return ModificationType(NoType);
        case Reverse + Complement: return ModificationType(ReverseComplement);
        case Reverse + ReverseComplement: return ModificationType(Complement);
        case Complement + Complement: return ModificationType(NoType);
        case Complement + ReverseComplement: return ModificationType(Reverse);
        case ReverseComplement + ReverseComplement: return ModificationType(NoType);
        }
        return ModificationType(NoType);
    }

    int getType() {
        return type;
    }

    bool operator == (const ModificationType & another) {
        return type == another.type;
    }

    bool operator == (int _type) {
        return type == _type;
    }

    bool operator != (const ModificationType & another) {
        return type != another.type;
    }

    bool operator != (int _type) {
        return type != _type;
    }

    ModificationType & operator = (int _type) {
        type = _type;
        return *this;
    }

private:
    int type;
};

class U2VIEW_EXPORT MSAEditorSequenceArea : public MaEditorSequenceArea {
    Q_OBJECT
    Q_DISABLE_COPY(MSAEditorSequenceArea)
    friend class SequenceAreaRenderer;
    friend class SequenceWithChromatogramAreaRenderer;

public:
    MSAEditorSequenceArea(MaEditorWgt* ui, GScrollBar* hb, GScrollBar* vb);

    MSAEditor *getEditor() const;

    bool hasAminoAlphabet();

protected:
    void focusOutEvent(QFocusEvent* fe);
    void focusInEvent(QFocusEvent* fe);

private slots:
    void sl_buildStaticMenu(GObjectView* v, QMenu* m);
    void sl_buildStaticToolbar(GObjectView* v, QToolBar* t);
    void sl_buildContextMenu(GObjectView* v, QMenu* m);
    void sl_lockedStateChanged();
    void sl_addSeqFromFile();
    void sl_addSeqFromProject();

    void sl_copyCurrentSelection();
    void sl_copyFormattedSelection();
    void sl_paste();
    void sl_pasteFinished(Task* pasteTask);
    void sl_addSequencesToAlignmentFinished(Task* task);
    void sl_delCol();
    void sl_goto();
    void sl_removeAllGaps();
    void sl_sortByName();
    void sl_setCollapsingMode(bool enabled);
    void sl_updateCollapsingMode();
    void sl_reverseComplementCurrentSelection();
    void sl_reverseCurrentSelection();
    void sl_complementCurrentSelection();

    void sl_onPosChangeRequest(int position);

    void sl_createSubaligniment();

    void sl_saveSequence();

    void sl_modelChanged();

    void sl_resetCollapsibleModel();
    void sl_setCollapsingRegions(const QList<QStringList>&);
    void sl_fontChanged(QFont font);

    void sl_alphabetChanged(const MaModificationInfo &mi, const DNAAlphabet *prevAlphabet);

    void sl_updateActions();

private:
    void initRenderer();

    void buildMenu(QMenu* m);

    void reverseComplementModification(ModificationType& type);

    void updateCollapseModel(const MaModificationInfo& modInfo) override;

    QAction*        delColAction;
    QAction*        removeAllGapsAction;

    QAction*        createSubaligniment;
    QAction*        saveSequence;
    QAction*        addSeqFromFileAction;
    QAction*        addSeqFromProjectAction;
    QAction*        sortByNameAction;
    QAction*        collapseModeSwitchAction;
    QAction*        collapseModeUpdateAction;
    QAction*        reverseComplementAction;
    QAction*        reverseAction;
    QAction*        complementAction;
};

// SANGER_TODO: move to EditorTasks?
class U2VIEW_EXPORT ExportHighligtningTask : public Task {
    Q_OBJECT
public:
    ExportHighligtningTask(ExportHighligtingDialogController *dialog, MaEditor *editor);

    void run();
    QString generateReport() const;
    Task::ReportResult report();

    QString exportHighlighting(int startPos, int endPos, int startingIndex, bool keepGaps, bool dots, bool transpose);

private:
    int startPos;
    int endPos;
    int startingIndex;
    bool keepGaps;
    bool dots;
    bool transpose;
    GUrl url;
    MSAEditor *msaEditor;
};

}//namespace
#endif
