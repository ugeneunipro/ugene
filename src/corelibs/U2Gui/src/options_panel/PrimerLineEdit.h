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

#ifndef _U2_PRIMER_LINE_EDIT_H_
#define _U2_PRIMER_LINE_EDIT_H_

#include <QLineEdit>

#include <U2Core/global.h>

namespace U2 {

/**
 * @PrimerLineEdit
 * Improved line edit for primers. Includes "5'" and "3'" labels and validatior for nucletide or amino characters.
 * Promote @QLineEdit to @PrimerLineEdit to make it work.
 */
class U2GUI_EXPORT PrimerLineEdit : public QLineEdit {
    Q_OBJECT
public:
    PrimerLineEdit(QWidget* parent);

    /**
     * Set text if it's valid. If not set empty string.
     */
    void setInvalidatedText(const QString& text);

protected:
    void paintEvent(QPaintEvent* event);

private:
    QRect getPlaceHolderRect() const;
};

}  // namespace U2

#endif  // _U2_PRIMER_LINE_EDIT_H_
