/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSortFilterProxyModel>

#include <U2Core/AppContext.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/HelpButton.h>

#include "ExternalToolSupportSettingsController.h"
#include "SnpEffDatabaseDelegate.h"
#include "SnpEffDatabaseListModel.h"
#include "SnpEffSupport.h"
#include "java/JavaSupport.h"

namespace U2 {
namespace LocalWorkflow {

/************************************************************************/
/* SnpEffDatabaseDialog */
/************************************************************************/
SnpEffDatabaseDialog::SnpEffDatabaseDialog(QWidget* parent)
    : QDialog(parent) {
    setupUi(this);
    new HelpButton(this, buttonBox, "21433685");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Select"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(lineEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString)));
    proxyModel->setSourceModel(SnpEffSupport::databaseModel);

    tableView->setModel(proxyModel);
    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView->verticalHeader()->hide();

    connect(tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    connect(tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(sl_selectionChanged()));

    setMinimumSize(600, 400);
}

QString SnpEffDatabaseDialog::getDatabase() const {
    QItemSelectionModel* model = tableView->selectionModel();
    SAFE_POINT(model != NULL, "Selection model is NULL", QString());
    QModelIndexList selection = model->selectedRows();
    SAFE_POINT(selection.size() == 1, "Invalid selection state", QString());
    QModelIndex index = proxyModel->mapToSource(selection.first());
    return SnpEffSupport::databaseModel->getGenome(index.row());
}

void SnpEffDatabaseDialog::sl_selectionChanged() {
    buttonBox->button(QDialogButtonBox::Ok)->setDisabled(tableView->selectionModel()->selectedRows().size() == 0);
}

/************************************************************************/
/* SnpEffDatabasePropertyWidget */
/************************************************************************/
SnpEffDatabasePropertyWidget::SnpEffDatabasePropertyWidget(QWidget *parent, DelegateTags *tags)
    : PropertyWidget(parent, tags) {
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(tr("Select genome"));
    lineEdit->setReadOnly(true);
    lineEdit->setObjectName("lineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("toolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_showDialog()));
    layout()->addWidget(toolButton);

    setObjectName("SnpEffDatabasePropertyWidget");
}

QVariant SnpEffDatabasePropertyWidget::value() {
    return lineEdit->text();
}

void SnpEffDatabasePropertyWidget::setValue(const QVariant &value) {
    lineEdit->setText(value.toString());
}

void SnpEffDatabasePropertyWidget::sl_showDialog() {
    // snpEff database list is available only if there is a valid tool!
    ExternalTool *java = AppContext::getExternalToolRegistry()->getByName(ET_JAVA);
    ExternalTool *snpEff = AppContext::getExternalToolRegistry()->getByName(ET_SNPEFF);
    CHECK(java != NULL, );
    CHECK(snpEff != NULL, );
    if (!(java->isValid() && snpEff->isValid())) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(this);
        msgBox->setWindowTitle(tr("%1 and %2").arg(snpEff->getName()).arg(java->getName()));
        msgBox->setText(tr("The list of genomes is not available.\r\nMake sure %1 and %2 tools are set in the UGENE Application Settings and can be validated.").arg(snpEff->getName()).arg(java->getName()));
        msgBox->setInformativeText(tr("Do you want to do it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), );

        switch (ret) {
           case QMessageBox::Yes:
               AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
               break;
           case QMessageBox::No:
               return;
           default:
               assert(false);
               break;
         }
        return;
    }

    QObjectScopedPointer<SnpEffDatabaseDialog> dlg(new SnpEffDatabaseDialog(this));
    if (dlg->exec() == QDialog::Accepted) {
        CHECK(!dlg.isNull(), );
        lineEdit->setText(dlg->getDatabase());
        emit si_valueChanged(lineEdit->text());
    }
    lineEdit->setFocus();
}

/************************************************************************/
/* SnpEffDatabaseDelegate */
/************************************************************************/
SnpEffDatabaseDelegate::SnpEffDatabaseDelegate(QObject *parent)
    : PropertyDelegate(parent) {
}

QWidget* SnpEffDatabaseDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
                                              const QModelIndex &/*index*/) const {
    SnpEffDatabasePropertyWidget* editor = new SnpEffDatabasePropertyWidget(parent);
    connect(editor, SIGNAL(si_valueChanged(QVariant)), SLOT(sl_commit()));
    return editor;
}

PropertyWidget * SnpEffDatabaseDelegate::createWizardWidget(U2OpStatus & /*os*/, QWidget *parent) const {
    return new SnpEffDatabasePropertyWidget(parent);
}

void SnpEffDatabaseDelegate::setEditorData(QWidget *editor,
                                           const QModelIndex &index) const {
    QVariant val = index.model()->data(index, ConfigurationEditor::ItemValueRole);
    SnpEffDatabasePropertyWidget *propertyWidget = dynamic_cast<SnpEffDatabasePropertyWidget*>(editor);
    propertyWidget->setValue(val);
}

void SnpEffDatabaseDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                          const QModelIndex &index) const {
    SnpEffDatabasePropertyWidget *propertyWidget = dynamic_cast<SnpEffDatabasePropertyWidget*>(editor);
    QString val = propertyWidget->value().toString();
    model->setData(index, val, ConfigurationEditor::ItemValueRole);
}

PropertyDelegate* SnpEffDatabaseDelegate::clone() {
    return new SnpEffDatabaseDelegate(parent());
}

void SnpEffDatabaseDelegate::sl_commit() {
    SnpEffDatabasePropertyWidget* editor = static_cast<SnpEffDatabasePropertyWidget*>(sender());
    CHECK(editor != NULL, );
    emit commitData(editor);
}

} // namespace LocalWorkflow
} // namespace U2
