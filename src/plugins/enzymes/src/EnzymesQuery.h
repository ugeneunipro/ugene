/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/Task.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/QDScheme.h>
#include <U2Lang/QueryDesignerRegistry.h>

#include "enzymes_dialog/EnzymesSelectorWidget.h"

namespace U2 {

class FindEnzymesTask;
class EnzymesSelectorDialogHandler;
class QDEnzymesActor : public QDActor {
    Q_OBJECT
public:
    QDEnzymesActor(QDActorPrototype const* proto);
    int getMinResultLen() const override {
        return 1;
    }
    int getMaxResultLen() const override {
        return 20;
    }
    QString getText() const override;
    Task* getAlgorithmTask(const QVector<U2Region>& location) override;
    QColor defaultColor() const override {
        return QColor(0xB4, 0x9F, 0xD4);
    }
    bool hasStrand() const override {
        return false;
    }
private slots:
    void sl_onAlgorithmTaskFinished();

private:
    QStringList ids;
    QList<FindEnzymesTask*> enzymesTasks;
    EnzymesSelectorDialogHandler* selectorFactory;
};

class QDEnzymesActorPrototype : public QDActorPrototype {
public:
    QDEnzymesActorPrototype();
    QIcon getIcon() const override {
        return QIcon(":enzymes/images/enzymes.png");
    }
    QDActor* createInstance() const override {
        return new QDEnzymesActor(this);
    }
};

class EnzymesSelectorDialogHandler : public SelectorDialogHandler {
public:
    EnzymesSelectorDialogHandler() = default;

    QDialog* createSelectorDialog(const QString&) override;
    QString getSelectedString(QDialog* dlg) override;
};

}  // namespace U2
