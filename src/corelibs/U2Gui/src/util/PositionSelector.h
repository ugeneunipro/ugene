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

#include <QDialog>
#include <QLineEdit>
#include <QValidator>

#include <U2Core/global.h>

namespace U2 {

class U2GUI_EXPORT PositionSelector : public QWidget {
    Q_OBJECT
public:
    PositionSelector(QWidget* p, qint64 rangeStart, qint64 rangeEnd, bool fixedSize = true);
    PositionSelector(QDialog* d, qint64 rangeStart, qint64 rangeEnd, bool autoclose);

    ~PositionSelector();

    void updateRange(qint64 rangeStart, qint64 rangeEnd);
    QLineEdit* getPosEdit() const {
        return posEdit;
    }

    /* Returns position if dialog accepted, otherwise -1 */
    qint64 getPosition() const;

signals:
    void si_positionChanged(int pos);

private slots:
    void sl_onButtonClicked(bool);
    void sl_onReturnPressed();

private:
    void init(bool fixedSize);
    void exec();

    qint64 rangeStart;
    qint64 rangeEnd;
    QLineEdit* posEdit;
    bool autoclose;
    QDialog* dialog;
    qint64 position = -1;
};

}  // namespace U2
