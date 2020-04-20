/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GT_RADIOBUTTON_H_
#define _HI_GT_RADIOBUTTON_H_

#include "GTGlobals.h"

#include <QRadioButton>

namespace HI {
/*!
 * \brief The class for working with QRadioButton primitive
 */
class HI_EXPORT GTRadioButton {
public:
    // fails if the radioButton is NULL or can't click
    static void click(GUITestOpStatus& os, QRadioButton *radioButton);
    static void click(GUITestOpStatus& os, const QString &radioButtonName, QWidget *parent = NULL);

    static QRadioButton* getRadioButtonByText(GUITestOpStatus& os, QString text, QWidget* parent=NULL);
    static QList<QRadioButton*> getAllButtonsByText(GUITestOpStatus &os, QString text, QWidget* parent=NULL);
};

}
#endif // _HI_GT_RADIOBUTTON_H_
