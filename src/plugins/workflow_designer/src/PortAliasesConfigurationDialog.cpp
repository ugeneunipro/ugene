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

#include <QMessageBox>

#include <U2Lang/ActorModel.h>
#include <U2Lang/WorkflowUtils.h>

#include "PortAliasesConfigurationDialog.h"
#include <U2Gui/HelpButton.h>
#include <QPushButton>

namespace U2 {
namespace Workflow {

PortAliasesConfigurationDialog::PortAliasesConfigurationDialog( const Schema & schema, QWidget * p )
: QDialog(p), currentRow(-1) {
    setupUi(this);
    new HelpButton(this, buttonBox, "28967074");

    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    QPushButton* cancelPushButton = buttonBox->button(QDialogButtonBox::Cancel);
    QPushButton* okPushButton = buttonBox->button(QDialogButtonBox::Ok);

    connect(cancelPushButton, SIGNAL(clicked()), SLOT(reject()));
    connect(okPushButton, SIGNAL(clicked()), SLOT(accept()));
    connect(portAliasEdit, SIGNAL(textChanged (const QString &)), SLOT(sl_portAliasChanged(const QString &)));
    connect(portDescriptionEdit, SIGNAL(textChanged (const QString &)), SLOT(sl_portDescriptionChanged(const QString &)));

    okPushButton->setDefault(true);
    portAliasesTableWidget->verticalHeader()->hide();

    portAliasesTableWidget->horizontalHeader()->setSectionsClickable(false);
    portAliasesTableWidget->horizontalHeader()->setStretchLastSection( true );

    foreach (Actor *actor, schema.getProcesses()) {
        assert(NULL != actor);
        foreach (Port *port, actor->getPorts()) {
            assert(NULL != port);
            // show only output and free input ports
            if (port->isInput()) {
                if (!port->getLinks().isEmpty()) {
                    continue;
                }
            }
            int pos = portListWidget->count();
            QString itemName = actor->getLabel() + "." + port->getDisplayName();
            QListWidgetItem *item = new QListWidgetItem(itemName);
            portListWidget->insertItem(pos, item);
            portListMap.insert(pos, port);
        }
    }
    if (portListMap.isEmpty()) {
        portInfoGroupBox->setVisible(false);
    }

    connect(portListWidget, SIGNAL(currentRowChanged(int)), SLOT(sl_portSelected(int)));
    connect(portAliasesTableWidget, SIGNAL(cellChanged(int, int)), SLOT(sl_onDataChange(int, int)));

    initializeModel(schema);

}

void PortAliasesConfigurationDialog::clearAliasTable() {
    portAliasesTableWidget->clearContents();
    portAliasesTableWidget->setRowCount(0);
}

void PortAliasesConfigurationDialog::sl_portSelected(int row) {
    if( row == -1 ) {
        return;
    }
    currentRow = row;
    clearAliasTable();

    assert(row >= 0 && row < portListWidget->count());
    Port *currentPort = portListMap.value(row);
    assert(NULL != currentPort);

    int rowInd = 0;
    QMap<Descriptor, QString> aliasMap = model.aliases.value(currentPort);
    QMap<Descriptor, QString>::const_iterator it = aliasMap.constBegin();
    while( it != aliasMap.constEnd() ) {
        portAliasesTableWidget->insertRow(rowInd);

        QTableWidgetItem *portNameItem = new QTableWidgetItem(it.key().getDisplayName()) ;
        portAliasesTableWidget->setItem(rowInd, 0, portNameItem);
        portNameItem->setData( Qt::UserRole, QVariant::fromValue( it.key() ) );
        portNameItem->setFlags(portNameItem->flags() ^ Qt::ItemIsSelectable ^ Qt::ItemIsEditable);

        QTableWidgetItem *aliasItem = new QTableWidgetItem(it.value());
        portAliasesTableWidget->setItem(rowInd, 1, aliasItem);

        rowInd++;
        ++it;
    }

    if (currentPort->isInput()) {
        portTypeLabel->setText(tr("Input"));
    } else {
        portTypeLabel->setText(tr("Output"));
    }

    portAliasEdit->setText(model.ports.value(currentPort).first);
    portDescriptionEdit->setText(model.ports.value(currentPort).second);

}

void PortAliasesConfigurationDialog::sl_onDataChange(int row, int col) {
    assert(row >= 0 && row < portAliasesTableWidget->rowCount());
    if(0 == col) {
        return;
    }

    Port *port = portListMap.value(portListWidget->currentRow());
    assert(NULL != port);

    Descriptor desc = portAliasesTableWidget->item(row, 0)->data(Qt::UserRole).value<Descriptor>();
    assert(model.aliases.value(port).contains(desc));

    model.aliases[port][desc] = portAliasesTableWidget->item(row, 1)->text();
}

void PortAliasesConfigurationDialog::sl_portAliasChanged(const QString &newStr) {
    Port *currentPort = portListMap.value(currentRow);
    PortInfo info = model.ports.value(currentPort);
    model.ports.insert(currentPort, PortInfo(newStr, info.second));
}

void PortAliasesConfigurationDialog::sl_portDescriptionChanged(const QString &newStr) {
    Port *currentPort = portListMap.value(currentRow);
    PortInfo info = model.ports.value(currentPort);
    model.ports.insert(currentPort, PortInfo(info.first, newStr));
}

void PortAliasesConfigurationDialog::initializeModel(const Schema &schema) {
    foreach( Actor * actor, schema.getProcesses() ) {
        assert(NULL != actor);
        foreach (Port *port, actor->getPorts()) {
            assert(NULL != port);
            if (port->isInput()) {
                if (!port->getLinks().isEmpty()) {
                    continue;
                }
            }
            model.ports.insert(port, PortInfo("", ""));
            QList<Descriptor> slotList;
            {
                DataTypePtr to = WorkflowUtils::getToDatatypeForBusport(qobject_cast<IntegralBusPort*>(port));
                if (port->isInput()) {
                    slotList = to->getDatatypesMap().keys();
                } else {
                    DataTypePtr from = WorkflowUtils::getFromDatatypeForBusport(qobject_cast<IntegralBusPort*>(port), to);
                    slotList = from->getDatatypesMap().keys();
                }
            }

            QList<SlotAlias> slotAliases;
            foreach (const PortAlias &portAlias, schema.getPortAliases()) {
                if (portAlias.getSourcePort() == port) {
                    slotAliases = portAlias.getSlotAliases();
                    model.ports.insert(port, PortInfo(portAlias.getAlias(), portAlias.getDescription()));
                    break;
                }
            }

            QMap<Descriptor, QString> aliasMap;
            foreach(Descriptor slotDescr, slotList) {
                QString actorId;
                QString slotId;
                {
                    if (port->isInput()) {
                        actorId = port->owner()->getId();
                        slotId = slotDescr.getId();
                    } else {
                        QStringList tokens = slotDescr.getId().split(':');
                        assert(2 == tokens.size());
                        actorId = tokens[0];
                        slotId = tokens[1];
                    }
                }

                QString alias;
                foreach (const SlotAlias &slotAlias, slotAliases) {
                    if (slotAlias.getSourceSlotId() == slotId) {
                        if (slotAlias.getSourcePort()->owner()->getId() == actorId) {
                            alias = slotAlias.getAlias();
                            break;
                        }
                    }
                }
                aliasMap.insert(slotDescr, alias);
            }
            model.aliases.insert(port, aliasMap);
        }
    }
}

void PortAliasesConfigurationDialog::accept() {
    foreach (Port *port, model.aliases.keys()) {
        QStringList slotAliases = model.aliases.value(port).values();

        bool slotsNotAliased = true;
        foreach (const QString &alias, slotAliases) {
            slotsNotAliased = slotsNotAliased && alias.isEmpty();
        }

        bool portNotAliases = model.ports.value(port).first.isEmpty();
        if (!slotsNotAliased && portNotAliases) {
            QString portStr = port->owner()->getLabel() + "." + port->getId();
            QMessageBox::information(this, tr("Workflow Designer"),
                tr("There is a port with some aliased slots but without alias name:\n%1").arg(portStr));
            return;
        }

        slotAliases.removeAll("");
        if (0 != slotAliases.removeDuplicates()) {
            QMessageBox::information(this, tr("Workflow Designer"),
                tr("Slot aliases of one port must be different!"));
            return;
        }

        foreach (Port *otherPort, model.ports.keys()) {
            if (otherPort == port) {
                continue;
            }
            if (model.ports.value(port).first.isEmpty()) {
                continue;
            }
            if (model.ports.value(port).first == model.ports.value(otherPort).first) {
                QString portStr1 = port->owner()->getLabel() + "." + port->getId();
                QString portStr2 = otherPort->owner()->getLabel() + "." + otherPort->getId();

                QMessageBox::information(this, tr("Workflow Designer"),
                    tr("Port aliases must be different! Rename one of ports:\n%1 or %2").arg(portStr1).arg(portStr2));
                return;
            }
        }
    }

    QDialog::accept();
}

PortAliasesCfgDlgModel PortAliasesConfigurationDialog::getModel() const {
    PortAliasesCfgDlgModel ret;
    foreach (Port *port, model.ports.keys()) {
        QString newPortName = model.ports.value(port).first;
        if (newPortName.isEmpty()) {
            continue;
        }
        ret.ports.insert(port, model.ports.value(port));

        QMap<Descriptor, QString> aliases = model.aliases.value(port);
        foreach(const Descriptor &slotDescr, aliases.keys()) {
            if (aliases.value(slotDescr).isEmpty()) {
                aliases.remove(slotDescr);
            }
        }
        ret.aliases.insert(port, aliases);
    }
    return ret;
}

} // Workflow
} // U2
