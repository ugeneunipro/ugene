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

#include "GTSlider.h"

namespace HI{
#define GT_CLASS_NAME "GTSlider"

#define GT_METHOD_NAME "setValue"
void GTSlider::setValue(GUITestOpStatus &os, QSlider *slider, int value){
    Q_UNUSED(os);
    GT_CHECK(slider != NULL, "slider not found");
    int min = slider->minimum();
    int max = slider->maximum();
    GT_CHECK(slider->isEnabled(), "slider is disabled");
    GT_CHECK(value >= min, QString("can not set value %1, mininum is %2").arg(value).arg(min));
    GT_CHECK(value <= max, QString("can not set value %1, maximum is %2").arg(value).arg(max));

    slider->setValue(value);

}
#undef GT_CLASS_NAME

#undef GT_METHOD_NAME
}
