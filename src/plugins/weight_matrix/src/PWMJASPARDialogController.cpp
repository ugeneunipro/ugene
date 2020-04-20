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

#include "PWMJASPARDialogController.h"

#include <U2Gui/GUIUtils.h>

#include <QDir>
#include <U2Gui/HelpButton.h>
#include <QPushButton>
#include <QMessageBox>

namespace U2 {

PWMJASPARDialogController::PWMJASPARDialogController(QWidget *w)
: QDialog(w) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24749003");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Select"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QString jasparDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/position_weight_matrix/JASPAR";
    QDir dir(jasparDir);
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0, n = list.size(); i < n; i++) {
        QString filename = jasparDir;
        filename.append("/").append(list[i]).append("/matrix_list.txt");
        if (QFile::exists(filename)) {
            JasparGroupTreeItem* gti = new JasparGroupTreeItem(list[i]);
            gti->setFlags(gti->flags() & ~Qt::ItemIsSelectable);
            jasparTree->addTopLevelItem(gti);
            QFile base(filename);
            base.open(QIODevice::ReadOnly);
            while (!base.atEnd()) {
                QString curr = base.readLine();
                JasparInfo info=(curr);
                JasparTreeItem* ti = new JasparTreeItem(info);
                gti->addChild(ti);
            }
            base.close();
        }
    }
    fileName = "";

    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    connect(okButton, SIGNAL(clicked()), SLOT(sl_onOK()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_onCancel()));
    connect(jasparTree, SIGNAL(itemSelectionChanged()), SLOT(sl_onSelectionChanged()));
    connect(jasparTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(sl_onDoubleClicked(QTreeWidgetItem*, int)));
    connect(propertiesTable, SIGNAL(itemClicked(QTableWidgetItem*)), SLOT(sl_onTableItemClicked(QTableWidgetItem*)));

}

void PWMJASPARDialogController::sl_onOK() {
    QDialog::accept();
}

void PWMJASPARDialogController::sl_onCancel() {
    QDialog::reject();
}

void PWMJASPARDialogController::sl_onSelectionChanged() {
    QTreeWidgetItem* item = jasparTree->currentItem();
    if (item == 0) {
        fileName = "";
        return;
    }
    if (!item->isSelected()) {
        fileName = "";
        return;
    }
    JasparTreeItem* it = static_cast<JasparTreeItem*>(item);
    QMap<QString, QString> props = it->matrix.getProperties();
    fileName = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/position_weight_matrix/JASPAR/";
    fileName.append(it->matrix.getProperty("tax_group")).append("/");
    fileName.append(it->matrix.getProperty("id")).append(".pfm");
    propertiesTable->clear();
    propertiesTable->setRowCount(props.size());
    propertiesTable->setColumnCount(2);
    propertiesTable->verticalHeader()->setVisible(false);
    propertiesTable->horizontalHeader()->setVisible(false);

    QMapIterator<QString, QString> iter(props);
    int pos = 0;
    while (iter.hasNext()) {
        iter.next();
        propertiesTable->setItem(pos, 0, new QTableWidgetItem(iter.key()));
        propertiesTable->setItem(pos, 1, new QTableWidgetItem(iter.value()));
        pos++;
    }
}

void PWMJASPARDialogController::sl_onTableItemClicked(QTableWidgetItem* item) {
    if (item->column() != 1) return;
    int row = item->row();
    QString text = propertiesTable->item(row, 0)->text();
    QString link = "";
    if (text == "acc") {
        link = "http://www.uniprot.org/uniprot/" + item->text();
    }
    if (text == "medline") {
        link = "http://www.ncbi.nlm.nih.gov/pubmed/" + item->text();
    }
    if (text == "species") {
        link = "http://www.ncbi.nlm.nih.gov/Taxonomy/Browser/wwwtax.cgi?id=" + item->text();
    }
    if (!link.isEmpty()) {
        GUIUtils::runWebBrowser(link);
    }
}

void PWMJASPARDialogController::sl_onDoubleClicked(QTreeWidgetItem* item, int col) {
    Q_UNUSED(col);
    if (item == 0) return;
    if (!item->isSelected()) return;
    QDialog::accept();
}

//////////////////////////////////////////////////////////////////////////
// Tree item
JasparTreeItem::JasparTreeItem(const JasparInfo& ed)
: matrix(ed)
{
    this->setText(0, matrix.getProperty(QString("name")));
    this->setText(1, matrix.getProperty(QString("id")));
    this->setText(2, matrix.getProperty(QString("class")));
    this->setText(3, matrix.getProperty(QString("family")));
}

bool JasparTreeItem::operator<(const QTreeWidgetItem & other) const {
    int col = treeWidget()->sortColumn();
    const JasparTreeItem& ei = static_cast<const JasparTreeItem&>(other);
    return text(col) < ei.text(col);
}

JasparGroupTreeItem::JasparGroupTreeItem(const QString& _s) : s(_s){
    this->setText(0, s);
}

bool JasparGroupTreeItem::operator<(const QTreeWidgetItem & other) const {
    if (other.parent() != NULL) {
        return true;
    }
    int col = treeWidget()->sortColumn();
    return text(col) < other.text(col);
}

}
