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

#ifndef _U2_MSA_EDITOR_OFFSETS_VIEW_H_
#define _U2_MSA_EDITOR_OFFSETS_VIEW_H_

#include <QWidget>

#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class MaEditor;
class MaEditorWgt;
class MSAEditorOffsetsViewWidget;
class MaEditorSequenceArea;
class MaModificationInfo;
class MultipleSequenceAlignmentObject;

class U2VIEW_EXPORT MSAEditorOffsetsViewController : public QObject {
    Q_OBJECT
public:
    MSAEditorOffsetsViewController(MaEditorWgt *maEditorUi, MaEditor* editor, MaEditorSequenceArea* seqArea);

    MSAEditorOffsetsViewWidget* getLeftWidget() const;
    MSAEditorOffsetsViewWidget* getRightWidget() const;

    QAction* getToggleColumnsViewAction() const;
    bool eventFilter(QObject* o, QEvent* e);

private slots:
    void sl_updateOffsets();
    void sl_showOffsets(bool);

private:
    void updateOffsets();

    MaEditorSequenceArea*       seqArea;
    MaEditor*                   editor;
    MSAEditorOffsetsViewWidget* lw;
    MSAEditorOffsetsViewWidget* rw;
    QAction*                    viewAction;
};

class MSAEditorOffsetsViewWidget : public QWidget {
    Q_OBJECT
    friend class MSAEditorOffsetsViewController;
public:
    MSAEditorOffsetsViewWidget(MaEditorWgt *maEditorUi, MaEditor *editor, MaEditorSequenceArea *seqArea, bool showStartPos);

private slots:
    void sl_completeRedraw();

protected:
    void paintEvent(QPaintEvent *e);
    void updateView();
    void drawAll(QPainter &p);
    QFont getOffsetsFont();
    void drawRefSequence(QPainter &p, const QRect &r);

private:
    int getBaseCounts(int seqNum, int aliPos, bool inclAliPos) const;

    MaEditorSequenceArea *      seqArea;
    MaEditor *                  editor;
    bool                        showStartPos;
    bool                        completeRedraw;
    QPixmap                     cachedView;
};

}//namespace;

#endif
