/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include <U2Core/U2Region.h>
#include <U2Core/global.h>

class Ui_RangeSelectionDialog;

namespace U2 {

class U2GUI_EXPORT RangeSelector : public QWidget {
    Q_OBJECT
public:
    RangeSelector(QDialog* dialog, qint64 rangeStart, qint64 rangeEnd, qint64 len, bool autoClose);

    qint64 getStart() const;

    qint64 getEnd() const;

signals:

    void si_rangeChanged(qint64 startPos, qint64 endPos);

private slots:

    void sl_onGoButtonClicked(bool);

    void sl_onMinButtonClicked(bool);

    void sl_onMaxButtonClicked(bool);

    void sl_onReturnPressed();

private:
    void init();

    void exec();

    qint64 rangeStart = 0;
    qint64 rangeEnd = 0;
    qint64 len = 0;

    QLineEdit* startEdit;
    QLineEdit* endEdit;
    QToolButton* minButton;
    QToolButton* maxButton;
    QLabel* rangeLabel;

    QDialog* dialog;

    bool autoClose;
};

class U2GUI_EXPORT MultipleRangeSelector : public QDialog {
    Q_OBJECT
public:
    MultipleRangeSelector(QWidget* parent, const QVector<U2Region>& _regions, qint64 _seqLen, bool isCircular);

    ~MultipleRangeSelector();

    void accept() override;

    QVector<U2Region> getSelectedRegions();

private:
    qint64 seqLen = 0;
    QVector<U2Region> selectedRanges;
    bool isCircular;
    QPalette normalPalette;

    Ui_RangeSelectionDialog* ui;

protected slots:

    void sl_multipleButtonToggled(bool);

    void sl_buttonClicked(QAbstractButton* b);

    void sl_minButton();

    void sl_maxButton();

    void sl_returnPressed();

    void sl_textEdited(const QString&);
};

}  // namespace U2
