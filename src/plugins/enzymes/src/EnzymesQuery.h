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

#ifndef _U2_ENZYMES_QUERY_H_
#define _U2_ENZYMES_QUERY_H_

#include <U2Algorithm/EnzymeModel.h>
#include "FindEnzymesDialog.h"
#include <ui_EnzymesSelectorDialog.h>

#include <U2Lang/QDScheme.h>
#include <U2Lang/QueryDesignerRegistry.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Core/Task.h>


namespace U2 {

class FindEnzymesTask;
class EnzymesSelectorDialogHandler;
class QDEnzymesActor : public QDActor {
    Q_OBJECT
public:
    QDEnzymesActor(QDActorPrototype const* proto);
    int getMinResultLen() const { return 1; }
    int getMaxResultLen() const { return 20; }
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const { return QColor(0xB4, 0x9F, 0xD4); }
    virtual bool hasStrand() const { return false; }
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
    QIcon getIcon() const { return QIcon(":enzymes/images/enzymes.png"); }
    QDActor* createInstance() const { return new QDEnzymesActor(this); }
};

class EnzymesSelectorDialog : public QDialog, public Ui_EnzymesSelectorDialog {
    Q_OBJECT
public:
    EnzymesSelectorDialog(EnzymesSelectorDialogHandler* parent);
    QString getSelectedString() const;
private:
    EnzymesSelectorDialogHandler* factory;
    EnzymesSelectorWidget* enzSel;
};

class EnzymesSelectorDialogHandler : public SelectorDialogHandler {
public:
    EnzymesSelectorDialogHandler() {}
    virtual QDialog* createSelectorDialog(const QString&) { return new EnzymesSelectorDialog(this); }
    virtual QString getSelectedString(QDialog* dlg);
};

}//namespace

#endif
