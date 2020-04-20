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

#ifndef _U2_CREATE_SCRIPT_WORKER_DIALOG_H_
#define _U2_CREATE_SCRIPT_WORKER_DIALOG_H_

#include <QDomDocument>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Settings.h>

#include <U2Designer/DelegateEditors.h>
#include <U2Lang/ActorModel.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowEnv.h>

#include "ui_CreateScriptBlockDialog.h"

namespace U2 {
using namespace Workflow;

class CreateScriptElementDialog : public QDialog,  public Ui_CreateScriptBlockDialog {
    Q_OBJECT
public:
    //CreateScriptElementDialog(QWidget* p = NULL);
    CreateScriptElementDialog(QWidget* p = NULL, ActorPrototype* proto = NULL);
    QList<DataTypePtr > getInput() const;
    QList<DataTypePtr > getOutput() const;
    QList<Attribute*> getAttributes() const;
    const QString getName() const;
    const QString getDescription() const;
    const QString getActorFilePath() const;

private slots:
    void sl_addInputClicked();
    void sl_addOutputClicked();
    void sl_okClicked();
    void sl_cancelClicked();
    void sl_addAttribute();
    void sl_deleteInputClicked();
    void sl_deleteOutputClicked();
    void sl_deleteAttributeClicked();
    void sl_getDirectory();

private:
    QDomDocument saveXml();
    bool saveParams();
    void fillFields(ActorPrototype *proto);
    void changeDirectoryForActors();

    QList<DataTypePtr > input;
    QList<DataTypePtr > output;
    QList<Attribute*> attrs;
    QString name;
    QString description;
    bool editing;
    QString actorFilePath;
};

} //namespace

#endif
