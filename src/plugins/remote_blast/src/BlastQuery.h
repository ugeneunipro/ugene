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

#include <U2Lang/QDScheme.h>
#include <U2Lang/QueryDesignerRegistry.h>

#include "RemoteBLASTConsts.h"
#include "RemoteBLASTTask.h"

namespace U2 {

class QDCDDActor : public QDActor {
    Q_OBJECT
public:
    QDCDDActor(QDActorPrototype const* a);
    int getMinResultLen() const override;
    int getMaxResultLen() const override;
    QString getText() const override;
    Task* getAlgorithmTask(const QVector<U2Region>& location) override;
    QColor defaultColor() const override {
        return QColor(0x6A, 0x94, 0xd4);
    }
    bool hasStrand() const override {
        return false;
    }
private slots:
    void sl_onAlgorithmTaskFinished();

private:
    RemoteBLASTTaskSettings settings;
    QMap<RemoteBLASTTask*, int> offsetMap;
};

class QDCDDActorPrototype : public QDActorPrototype {
public:
    QDCDDActorPrototype();
    QDActor* createInstance() const override {
        return new QDCDDActor(this);
    }
    QIcon getIcon() const override {
        return QIcon(":remote_blast/images/remote_db_request.png");
    }
};

}  // namespace U2
