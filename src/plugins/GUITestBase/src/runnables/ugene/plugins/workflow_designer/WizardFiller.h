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

#include <QPushButton>
#include <QToolButton>

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class WizardFiller : public Filler {
public:
    WizardFiller(HI::GUITestOpStatus& _os, QString name, QList<QStringList> _inputFiles = QList<QStringList>(), QMap<QString, QVariant> _map = (QMap<QString, QVariant>()))
        : Filler(_os, name), inputFiles(_inputFiles), map(_map) {
    }
    WizardFiller(HI::GUITestOpStatus& _os, QString name, QStringList _inputFiles, QMap<QString, QVariant> _map = (QMap<QString, QVariant>()))
        : Filler(_os, name), inputFiles(QList<QStringList>() << _inputFiles), map(_map) {
    }
    WizardFiller(HI::GUITestOpStatus& _os, QString name, CustomScenario* c)
        : Filler(_os, name, c) {
    }

    void commonScenario() override;

private:
    QList<QStringList> inputFiles;
    QMap<QString, QVariant> map;
};

}  // namespace U2
