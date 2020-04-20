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

#ifndef _U2_WCFG_SCHEMA_ALIASES_DIALOG_CONTROLLER_H_
#define _U2_WCFG_SCHEMA_ALIASES_DIALOG_CONTROLLER_H_

#include <U2Lang/Schema.h>
#include <ui_SchemaAliasesConfigurationDialog.h>

namespace U2 {
namespace Workflow {

//typedef QMap<ActorId, QMap<Descriptor, QString> > SchemaAliasesCfgDlgModel;
struct SchemaAliasesCfgDlgModel {
    QMap<ActorId, QMap<Descriptor, QString> > aliases;
    QMap<ActorId, QMap<Descriptor, QString> > help;
};

class SchemaAliasesConfigurationDialogImpl : public QDialog, public Ui_SchemaAliasesConfigurationDialog {
    Q_OBJECT
public:
    SchemaAliasesConfigurationDialogImpl( const Schema & sc, QWidget * p = NULL );
    // remove empty aliases from model
    SchemaAliasesCfgDlgModel getModel() const;
    // aliases for schema should be different
    bool validateModel() const;

private:
    void initializeModel( const Schema & schema );
    void clearAliasTable();

private slots:
    void sl_procSelected( int row );
    void sl_onDataChange( int row, int col );

private:
    SchemaAliasesCfgDlgModel    model;
    QMap<int, ActorId>          procListMap; // pairs (row, actorId)

}; // SchemaAliasesConfigurationDialogImpl

} // Workflow
} // U2

#endif // _U2_WCFG_SCHEMA_ALIASES_DIALOG_CONTROLLER_H_
