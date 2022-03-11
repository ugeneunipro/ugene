/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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
#include "MaEditorSequenceArea.h"
#include "MsaEditorUserModStepController.h"
#include "SaveSelectedSequenceFromMSADialogController.h"

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

    ModificationType(int _type = NoType)
        : type(_type) {
    }

    ModificationType operator+(const ModificationType& another) {
        switch (type + another.type) {
            case NoType + NoType:
                return ModificationType(NoType);
            case NoType + Reverse:
                return ModificationType(Reverse);
            case NoType + Complement:
                return ModificationType(Complement);
            case NoType + ReverseComplement:
                return ModificationType(ReverseComplement);
            case Reverse + Reverse:
                return ModificationType(NoType);
            case Reverse + Complement:
                return ModificationType(ReverseComplement);
            case Reverse + ReverseComplement:
                return ModificationType(Complement);
            case Complement + Complement:
                return ModificationType(NoType);
            case Complement + ReverseComplement:
                return ModificationType(Reverse);
            case ReverseComplement + ReverseComplement:
                return ModificationType(NoType);
        }
        return ModificationType(NoType);
    }

    int getType() {
        return type;
    }

    bool operator==(const ModificationType& another) {
        return type == another.type;
    }

    bool operator==(int _type) {
        return type == _type;
    }

    bool operator!=(const ModificationType& another) {
        return type != another.type;
    }

    bool operator!=(int _type) {
        return type != _type;
    }

    ModificationType& operator=(int _type) {
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

    MSAEditor* getEditor() const;

    bool hasAminoAlphabet();

public slots:

    /** Switches between Original and Sequence row orders. */
    void sl_toggleSequenceRowOrder(bool isOrderBySequence) override;

    void sl_copySelectionFormatted();

public:
    /**
     * Enables 'Free' ordering mode. Re-orders and re-groups rows according to the name lists.
     * The 'marker' object is provided by the 'master' controller that requested the Free mode to be ON (See 'freeModeMasterMarkersSet').
     * TODO: rework to use sequence IDs. Multiple same-name sequences can be present in the list.
     * TODO: move this method to MSAEditor class.
     */
    void enableFreeRowOrderMode(QObject* marker, const QList<QStringList>&);

    /**
     * Removes 'marker' object from a 'Free' mode locks (See 'freeModeMasterMarkersSet').
     * When all 'markers' for the Free mode are removed the view is automatically switched to the Original mode.
     */
    void disableFreeRowOrderMode(QObject* marker);

protected:
    void focusOutEvent(QFocusEvent *fe) override;
    void focusInEvent(QFocusEvent *fe) override;
    void wheelEvent(QWheelEvent *we) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private slots:
    void sl_buildMenu(GObjectView* v, QMenu* m, const QString& menuType);
    void sl_buildStaticToolbar(GObjectView* v, QToolBar* t);
    void sl_lockedStateChanged();
    void sl_addSeqFromFile();
    void sl_addSeqFromProject();

    void sl_copySelection();
    void sl_paste();
    void sl_pasteBefore();
    void sl_cutSelection();

    /** Takes data from the pasteTask and runs AddSequencesFromDocumentsToAlignmentTask. */
    void sl_pasteTaskFinished(Task* pasteTask);

    void sl_addSequencesToAlignmentFinished(Task* task);
    void sl_delCol();
    void sl_goto();
    void sl_removeAllGaps();

    /**
     * Groups sequences by content, so each group contains only sequences with the equal base (gaps-excluded) content.
     * Works a a part of virtual ordering mode only.
     */
    void sl_groupSequencesByContent();

    void sl_reverseComplementCurrentSelection();
    void sl_reverseCurrentSelection();
    void sl_complementCurrentSelection();

    void sl_onPosChangeRequest(int position);

    void sl_createSubalignment();

    void sl_saveSequence();

    void sl_modelChanged() override;

    void sl_fontChanged(QFont font);

    void sl_alphabetChanged(const MaModificationInfo& mi, const DNAAlphabet* prevAlphabet);

    void sl_updateActions() override;

private:
    void initRenderer() override;
    void runPasteTask(bool isPasteBefore);

    /** Updates enabled/checked states of row-ordering actions based on the current row-order-mode in MSA. */
    void updateRowOrderActionsState();

    void reverseComplementModification(ModificationType& type);

    void updateCollapseModel(const MaModificationInfo& modInfo) override;

    QAction* delColAction;
    QAction* removeAllGapsAction;

    QAction* createSubaligniment;
    QAction* saveSequence;
    QAction* addSeqFromFileAction;
    QAction* addSeqFromProjectAction;

    /**
     * Switches between Sequence and Original row ordering modes.
     * When checked the Sequence mode is ON.
     * TODO: this is a global action for MA editor. Move it to the M(S)AEditor.h
     */
    QAction* toggleSequenceRowOrderAction;

    /**
     * The action is enabled only in Sequence row ordering mode and triggers recompute of sequence order/groups by content.
     * TODO: this is a global action for MA editor. Move it to M(S)AEditor.h
     * TODO: this action has no use today, because in Sequence mode MA editor automatically adjusts order/groups on every MA update.
     */
    QAction* refreshSequenceRowOrder;

    QAction* reverseComplementAction;
    QAction* reverseAction;
    QAction* complementAction;
};

// SANGER_TODO: move to EditorTasks?
class U2VIEW_EXPORT ExportHighlightingTask : public Task {
    Q_OBJECT
public:
    ExportHighlightingTask(ExportHighligtingDialogController* dialog, MaEditor* editor);

    void run() override;
    QString generateReport() const override;
    Task::ReportResult report() override;

private:
    QString generateExportHighlightingReport() const;

private:
    int startPos;
    int endPos;
    int startingIndex;
    bool keepGaps;
    bool dots;
    bool transpose;
    GUrl url;
    MSAEditor* msaEditor;
};

}  // namespace U2
#endif
