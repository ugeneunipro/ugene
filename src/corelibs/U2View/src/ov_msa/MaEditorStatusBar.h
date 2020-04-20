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

#ifndef _U2_MA_EDITOR_STATUS_BAR_H_
#define _U2_MA_EDITOR_STATUS_BAR_H_

#include <QLabel>
#include <QVariant>

class QHBoxLayout;

namespace U2 {

class MultipleAlignmentObject;
class MaEditorSequenceArea;
class MaEditorSelection;

class MaEditorStatusBar : public QFrame {
    Q_OBJECT
protected:
    class TwoArgPatternLabel : public QLabel {
    public:
        TwoArgPatternLabel(QString textPattern, QString tooltipPattern,
                           QString objectName, QWidget* parent = NULL);
        TwoArgPatternLabel(QString objectName, QWidget* parent = NULL);
        void setPatterns(QString textPattern, QString tooltipPattern);

        void update(QString firstArg, int minWidth);
        void update(QString firstArg, QString secondArg);

        void updateMinWidth(QString maxLenArg);

    private:
        QString         textPattern;
        QString         tooltipPattern;
        QFontMetrics    fm;
    };

public:
    MaEditorStatusBar(MultipleAlignmentObject* mobj, MaEditorSequenceArea* seqArea);

private slots:
    void sl_update();
    void sl_lockStateChanged();

protected:
    virtual void setupLayout() = 0;
    virtual void updateLabels() = 0;
    QPair<QString, QString> getGappedPositionInfo(const QPoint& pos) const;

    void updateLock();
    virtual void updateLineLabel();
    virtual void updatePositionLabel();
    void updateColumnLabel();
    void updateSelectionLabel();
    void setStatusBarStyle();

protected:
    MultipleAlignmentObject*    aliObj;
    MaEditorSequenceArea*       seqArea;
    QPixmap                     lockedIcon;
    QPixmap                     unlockedIcon;

    QHBoxLayout*                layout;
    TwoArgPatternLabel*         lineLabel;
    TwoArgPatternLabel*         colomnLabel;
    TwoArgPatternLabel*         positionLabel;
    TwoArgPatternLabel*         selectionLabel;
    QLabel*                     lockLabel;

    static const QString NONE_MARK;
    static const QString GAP_MARK;

private:
    QString selectionPattern;
};



}//namespace;

#endif // _U2_MA_EDITOR_STATUS_BAR_H_
