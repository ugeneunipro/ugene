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

#include "AlignmentAlgorithmGUIExtension.h"

namespace U2 {

AlignmentAlgorithmMainWidget::AlignmentAlgorithmMainWidget(QWidget* parent, QVariantMap* s) :
    QWidget(parent), externSettings(s), externSettingsExists(s != NULL) {
}

AlignmentAlgorithmMainWidget::~AlignmentAlgorithmMainWidget() {
    getAlignmentAlgorithmCustomSettings(true);
}

void AlignmentAlgorithmMainWidget::sl_externSettingsInvalide() {
    externSettingsExists = false;
}

QMap<QString, QVariant> AlignmentAlgorithmMainWidget::getAlignmentAlgorithmCustomSettings(bool append = false) {
    if (append == true && externSettingsExists == true) {
        foreach (QString key, innerSettings.keys()) {
            externSettings->insert(key, innerSettings.value(key));
        }
    }
    return innerSettings;
}

void AlignmentAlgorithmMainWidget::fillInnerSettings() {
}

bool AlignmentAlgorithmGUIExtensionFactory::hasMainWidget(const QWidget* parent) {
    return mainWidgets.contains(parent);
}

void AlignmentAlgorithmGUIExtensionFactory::sl_widgetDestroyed(QObject * obj) {
    foreach (AlignmentAlgorithmMainWidget* mainWidget, mainWidgets.values()) {
        if (obj == mainWidget) {
            mainWidgets.remove(mainWidgets.key(mainWidget));
        }
    }
}

}   //namespace
