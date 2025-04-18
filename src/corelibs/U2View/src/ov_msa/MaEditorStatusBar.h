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

#include <QLabel>
#include <QVariant>

class QHBoxLayout;

namespace U2 {

class MaEditor;
class MaEditorSelection;

class MaEditorStatusBar : public QFrame {
    Q_OBJECT
protected:
    class TwoArgPatternLabel : public QLabel {
    public:
        TwoArgPatternLabel(const QString& textPattern, const QString& tooltipPattern, const QString& objectName, QWidget* parent = nullptr);
        void setPatterns(const QString& textPattern, const QString& tooltipPattern);

        void update(const QString& firstArg, int minWidth);
        void update(const QString& firstArg, const QString& secondArg);

        void updateMinWidth(QString maxLenArg);

    private:
        QString textPattern;
        QString tooltipPattern;
        QFontMetrics fm;
    };

public:
    MaEditorStatusBar(MaEditor* editor);

signals:
    // See comments in constructor
    void si_updateStatusBar2();

public slots:
    void sl_updateStatusBar();

private slots:
    // See comments in constructor
    void sl_updateStatusBar2();
    void sl_lockStateChanged();

protected:
    virtual void updateLabels() = 0;

    /** Return a pair of <column, alignment-len> text labels to display for the current top-left position of the selection. */
    QPair<QString, QString> getGappedPositionInfo() const;

    void updateLock();
    virtual void updateLineLabel();
    virtual void updatePositionLabel();
    void updateColumnLabel();
    void updateSelectionLabel();
    void setStatusBarStyle();

protected:
    MaEditor* editor;
    QPixmap lockedIcon;
    QPixmap unlockedIcon;

    QHBoxLayout* layout;

    /** Sequence line number. As visible on the screen. */
    TwoArgPatternLabel* lineLabel;

    TwoArgPatternLabel* columnLabel;
    TwoArgPatternLabel* positionLabel;
    TwoArgPatternLabel* selectionLabel;
    QLabel* lockLabel;

    static const QString NONE_MARK;
    static const QString GAP_MARK;

private:
    QString selectionPattern;
};

}  // namespace U2
