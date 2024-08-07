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

#include <U2Core/AppContext.h>
#include <U2Core/IconParameters.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include <QObject>

namespace U2 {

template<class T>
class IconSwitcher {
public:
    IconSwitcher(T* owner);
    IconSwitcher(T* owner, const IconParameters& iconParameters);

    void setIconParameters(const IconParameters& iconParameters);

protected:
    void colorModeSwitched();

private:
    T* owner = nullptr;
    IconParameters iconParameters;
};


template<class T>
IconSwitcher<T>::IconSwitcher(T* _owner) : owner(_owner) {}

template<class T>
IconSwitcher<T>::IconSwitcher(T* _owner, const IconParameters& _iconParameters)
    : owner(_owner) {
    setIconParameters(_iconParameters);
}

template<class T>
void IconSwitcher<T>::setIconParameters(const IconParameters& _iconParameters) {
    CHECK(!_iconParameters.isEmpty(), );

    iconParameters = _iconParameters;
    owner->setIcon(GUIUtils::getIconResource(iconParameters));
}

template<class T>
void IconSwitcher<T>::colorModeSwitched() {
    CHECK(!iconParameters.isEmpty(), );

    owner->setIcon(GUIUtils::getIconResource(iconParameters));
}

}  // namespace U2