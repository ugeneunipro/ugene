/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QFrame>
#include <QLabel>

#include <U2Core/U2Assembly.h>

namespace U2 {

// hint that is shown under cursor for current read
class AssemblyReadsAreaHint : public QFrame {
    Q_OBJECT
public:
    static const QPoint OFFSET_FROM_CURSOR;
    static const int LETTER_MAX_COUNT;
    static QString getReadDataAsString(const U2AssemblyRead& r);

public:
    AssemblyReadsAreaHint(QWidget* parent);
    void setData(U2AssemblyRead r, QList<U2AssemblyRead> mates);

protected:
    bool eventFilter(QObject*, QEvent*) override;
    void leaveEvent(QEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    QLabel* label;

};  // AssemblyReadsAreaHint

}  // namespace U2
