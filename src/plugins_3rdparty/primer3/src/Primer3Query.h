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

#include "U2Lang/QDScheme.h"
#include "U2Lang/QueryDesignerRegistry.h"

#include "task/Primer3TaskSettings.h"

namespace U2 {

class QDPrimerActor : public QDActor {
    Q_OBJECT
public:
    QDPrimerActor(QDActorPrototype const* proto);

    int getMinResultLen() const {
        return 1;
    }
    int getMaxResultLen() const {
        return 1000;
    }
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const {
        return QColor(0x88, 0x78, 0xEC);
    }

private slots:
    void sl_onAlgorithmTaskFinished(Task* t);
};

class QDPrimerActorPrototype : public QDActorPrototype {
public:
    QDPrimerActorPrototype();
    QIcon getIcon() const {
        return QIcon(":primer3/images/primer3.png");
    }
    QDActor* createInstance() const {
        return new QDPrimerActor(this);
    }
};

}  // namespace U2

