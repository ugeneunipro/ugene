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

#include <U2Core/Annotation.h>

#include <QMessageBox>
#include <QKeyEvent>
#include <U2Gui/HelpButton.h>
#include <QPushButton>

#include <ui_EditQualifierDialog.h>
#include "EditQualifierDialog.h"

namespace U2 {

EditQualifierDialog::EditQualifierDialog(QWidget* p, const U2Qualifier& q, bool ro, bool existingQualifier)
: QDialog(p)
{
    ui = new Ui_EditQualifierDialog;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24748787");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    
    if(true == ro){
        this->setWindowTitle(tr("View Qualifier"));
    }

    if( !existingQualifier ) { // adding new qualifier
        setWindowTitle("Add New Qualifier");
    }
    
    ui->nameEdit->setReadOnly(ro);
    ui->valueEdit->setReadOnly(ro);

    ui->nameEdit->setText(q.name);
    ui->valueEdit->setText(q.value);

    ui->valueEdit->installEventFilter(this);
}

bool EditQualifierDialog::eventFilter(QObject *obj, QEvent *e) {
    Q_UNUSED(obj);
    QEvent::Type t = e->type();
    if (t == QEvent::KeyPress) {
        QKeyEvent* ke = (QKeyEvent*)e;
        int key = ke->key();
        if (key == Qt::Key_Tab) {
            ui->nameEdit->setFocus();
            return true;
        } 
        if (key == Qt::Key_Enter || key == Qt::Key_Return) {
            accept();
            return true;
        }
    }
    return false;
}

static QString simplify(const QString& s) {
    QString res = s;
    res = res.replace("\t", "    ");
    res = res.replace("\r", "");
    res = res.replace("\n", " ");
    res = res.trimmed();
    return res;
}

void EditQualifierDialog::accept() {
    if (ui->nameEdit->isReadOnly()) {
        QDialog::accept();
        return;
    }
    QString name = simplify(ui->nameEdit->text());
    QString val = simplify(ui->valueEdit->toPlainText());
    if (!Annotation::isValidQualifierName(name)) {
        QMessageBox::critical(this, tr("Error!"), tr("Illegal qualifier name"));
        return;
    }
    if (!Annotation::isValidQualifierValue(val)) {
        QMessageBox::critical(this, tr("Error!"), tr("Illegal qualifier value"));
        return;
    }
    q = U2Qualifier(name, val);
    QDialog::accept();
}

EditQualifierDialog::~EditQualifierDialog()
{
    delete ui;
}
}//namespace
