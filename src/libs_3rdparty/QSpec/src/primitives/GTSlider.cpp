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

#include <core/CustomScenario.h>
#include <utils/GTThread.h>

#include "GTSlider.h"

namespace HI {
#define GT_CLASS_NAME "GTSlider"

#define GT_METHOD_NAME "setValue"
void GTSlider::setValue(GUITestOpStatus& os, QSlider* slider, int value) {
    GT_CHECK(slider != nullptr, "Slider is null!");

    class MainThreadScenario : public CustomScenario {
    public:
        MainThreadScenario(QSlider* _slider, int _value)
            : slider(_slider), value(_value) {
        }
        void run(GUITestOpStatus& os) override {
            int min = slider->minimum();
            int max = slider->maximum();

            GT_CHECK(slider->isEnabled(), "slider is disabled");
            GT_CHECK(value >= min, QString("can not set value %1, minimum is %2").arg(value).arg(min));
            GT_CHECK(value <= max, QString("can not set value %1, maximum is %2").arg(value).arg(max));

            slider->setValue(value);
        }
        QSlider* slider;
        int value;
    };

    GTThread::runInMainThread(os, new MainThreadScenario(slider, value));
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace HI
