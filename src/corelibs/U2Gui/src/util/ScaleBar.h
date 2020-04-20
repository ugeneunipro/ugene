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

#ifndef _U2_SCALE_BAR_H_
#define _U2_SCALE_BAR_H_

#include <QWidget>

#include <U2Core/global.h>

class QAbstractButton;
class QAction;
class QSlider;
class QToolButton;

namespace U2 {

class U2GUI_EXPORT ScaleBar : public QWidget {
    Q_OBJECT
public:
    ScaleBar(Qt::Orientation ori = Qt::Vertical, QWidget* parent = 0);

    int value() const;
    void setValue(int value);

    void setRange(int minumum, int maximum);
    void setTickInterval(int interval);

    QAction *getPlusAction() const;
    QAction *getMinusAction() const;

    QAbstractButton *getPlusButton() const;
    QAbstractButton *getMinusButton() const;

signals:
    void valueChanged(int value);

private slots:
    void sl_minusButtonClicked();
    void sl_plusButtonClicked();
    void sl_updateState();

protected:
    QSlider *scaleBar;
    QAction *minusAction;
    QAction *plusAction;
    QToolButton *plusButton;
    QToolButton *minusButton;
};

}   // namespace U2

#endif // _U2_SCALE_BAR_H_
