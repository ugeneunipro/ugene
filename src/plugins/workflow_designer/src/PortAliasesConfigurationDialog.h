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

#ifndef _PORT_ALIASES_DIALOG_CONTROLLER_H_
#define _PORT_ALIASES_DIALOG_CONTROLLER_H_

#include <U2Lang/Aliasing.h>
#include <U2Lang/Schema.h>
#include <ui_PortAliasesConfigurationDialog.h>

namespace U2 {
namespace Workflow {

typedef QPair<QString, QString> PortInfo; // alias, description
struct PortAliasesCfgDlgModel {
    QMap<Port*, QMap<Descriptor, QString> > aliases;
    QMap<Port*, PortInfo> ports;
};

class PortAliasesConfigurationDialog : public QDialog, public Ui_PortAliasesConfigurationDialog {
    Q_OBJECT
public:
    PortAliasesConfigurationDialog(const Schema &sc, QWidget *p = NULL);

    /*
     * it removes empty aliases from model
     */
    PortAliasesCfgDlgModel getModel() const;

private:
    void initializeModel(const Schema &schema);
    void clearAliasTable();

private slots:
    void sl_portSelected(int row);
    void sl_onDataChange(int row, int col);
    void sl_portAliasChanged(const QString &newStr);
    void sl_portDescriptionChanged(const QString &newStr);

private:
    PortAliasesCfgDlgModel      model;
    QMap<int, Port*>            portListMap; // pairs (row, port)
    int                         currentRow;

    void accept();

}; // PortAliasesConfigurationDialog

} // Workflow
} // U2

#endif // _PORT_ALIASES_DIALOG_CONTROLLER_H_
