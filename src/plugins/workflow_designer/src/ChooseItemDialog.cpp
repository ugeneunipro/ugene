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

#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Lang/WorkflowUtils.h>
#include <QPushButton>

#include "ChooseItemDialog.h"


namespace U2 {

ChooseItemDialog::ChooseItemDialog(QWidget* p) : QDialog(p)
{
    setupUi(this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(accept()));

}

QString ChooseItemDialog::select(const QMap<QString, QIcon>& items) {
    listWidget->clear();
    QMapIterator<QString, QIcon> it(items);
    while (it.hasNext())
    {
        it.next();
        listWidget->addItem(new QListWidgetItem(it.value(), it.key()));
    }
    listWidget->setItemSelected(listWidget->item(0), true);
    if (exec() == QDialog::Accepted) {
        return listWidget->currentItem()->text();
    } else {
        return QString();
    }
}

Workflow::ActorPrototype* ChooseItemDialog::select(const QList<Workflow::ActorPrototype*>& items) {
    listWidget->clear();
    foreach(Workflow::ActorPrototype* a, items) {
        QListWidgetItem* it = new QListWidgetItem(a->getIcon(), a->getDisplayName());
        it->setToolTip(a->getDocumentation());
        listWidget->addItem(it);
    }
    listWidget->setItemSelected(listWidget->item(0), true);
    if (exec() == QDialog::Accepted) {
        return items.at(listWidget->currentRow());
    } else {
        return NULL;
    }
}

}//namespace
