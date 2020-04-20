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

#ifndef __U2_ASSEMBLY_READS_AREA_HINT_H__
#define __U2_ASSEMBLY_READS_AREA_HINT_H__

#include <QLabel>
#include <QFrame>

#include <U2Core/U2Assembly.h>

namespace U2 {

// hint that is shown under cursor for current read
class AssemblyReadsAreaHint : public QFrame {
    Q_OBJECT
public:
    static const QPoint OFFSET_FROM_CURSOR;
    static const int LETTER_MAX_COUNT = 60;
    static QString getReadDataAsString(const U2AssemblyRead & r);

public:
    AssemblyReadsAreaHint(QWidget * p);
    void setData(U2AssemblyRead r, QList<U2AssemblyRead> mates);

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void leaveEvent(QEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);

    QLabel * label;

}; // AssemblyReadsAreaHint

} // U2

#endif // __U2_ASSEMBLY_READS_AREA_HINT_H__
