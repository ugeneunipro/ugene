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

#include "ViewMatrixDialogController.h"

#include <U2Core/AppContext.h>
#include <U2Core/DIProperties.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Gui/HelpButton.h>

#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace U2 {

MatrixAndLogoController::MatrixAndLogoController( PFMatrix matrix, QWidget *p):QWidget(p), logoArea(NULL){
    setupUi(this);
    if (matrix.getType() == PFM_MONONUCLEOTIDE) {
        tableWidget->setRowCount(4);
        tableWidget->setColumnCount(matrix.getLength());
        for (int i = 0, n = matrix.getLength(); i < n; i++) {
            tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(QString("%1").arg(i + 1)));
        }
        for (int i = 0; i < 4; i++) {
            tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString(DiProperty::fromIndex(i))));
            for (int j = 0, n = matrix.getLength(); j < n; j++) {
                tableWidget->setItem(i, j, new QTableWidgetItem(QString("%1").arg(matrix.getValue(i, j))));
                tableWidget->item(i, j)->setTextAlignment(Qt::AlignRight);
                tableWidget->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
    } else {
        tableWidget->setRowCount(16);
        tableWidget->setColumnCount(matrix.getLength());
        for (int i = 0, n = matrix.getLength(); i < n; i++) {
            tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(QString("%1").arg(i + 1)));
        }
        for (int i = 0; i < 16; i++) {
            tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString(DiProperty::fromIndexHi(i))+QString(DiProperty::fromIndexLo(i))));
            for (int j = 0, n = matrix.getLength(); j < n; j++) {
                tableWidget->setItem(i, j, new QTableWidgetItem(QString("%1").arg(matrix.getValue(i, j))));
                tableWidget->item(i, j)->setTextAlignment(Qt::AlignRight);
                tableWidget->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
    }
    tableWidget->resizeRowsToContents();
    tableWidget->resizeColumnsToContents();
    tableWidget->setMinimumWidth(tableWidget->width());
    tableWidget->setFixedHeight(tableWidget->verticalHeader()->length() + tableWidget->horizontalHeader()->height() + 20);

    int len = matrix.getLength();
    int size = 0;
    int n = matrix.getType() == PFM_MONONUCLEOTIDE ? 4 : 16;

    int logoheight = 150;
    int logowidth = 8 * len;

    for (int i = 0; i < n; i++) {
        size += matrix.getValue(i, 0);
    }

    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    MultipleSequenceAlignment ma(QString("Temporary alignment"), al);
    for (int i = 0; i < size; i++) {
        QByteArray arr;
        for (int j = 0; j < len; j++) {
            int row = 0;
            int sum = i;
            while (row < n && sum >= matrix.getValue(row, j)) {
                sum -= matrix.getValue(row, j);
                row++;
            }
            if (row == n) row--;
            if (matrix.getType() == PFM_MONONUCLEOTIDE) {
                arr.append(DiProperty::fromIndex(row));
            } else {
                arr.append(DiProperty::fromIndexHi(row));
                if (j == len - 1) {
                    arr.append(DiProperty::fromIndexLo(row));
                }
            }
        }
        ma->addRow(QString("Row %1").arg(i), arr);
    }
    AlignmentLogoSettings logoSettings(ma);
    logoWidget->resize(logowidth, logoheight);
    scrollArea->resize(logowidth, logoheight + 10);
    if (logoArea != NULL) {
        logoArea->replaceSettings(logoSettings);
    } else {
        logoArea = new AlignmentLogoRenderArea(logoSettings, logoWidget);
    }
    logoArea->repaint();
}

MatrixAndLogoController::MatrixAndLogoController( PWMatrix matrix, QWidget *p):QWidget(p){
    setupUi(this);
    if (matrix.getType() == PWM_MONONUCLEOTIDE) {
        tableWidget->setRowCount(4);
        tableWidget->setColumnCount(matrix.getLength());
        for (int i = 0, n = matrix.getLength(); i < n; i++) {
            tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(QString("%1").arg(i + 1)));
        }
        for (int i = 0; i < 4; i++) {
            tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString(DiProperty::fromIndex(i))));
            for (int j = 0, n = matrix.getLength(); j < n; j++) {
                tableWidget->setItem(i, j, new QTableWidgetItem(QString("%1").arg(matrix.getValue(i, j))));
                tableWidget->item(i, j)->setTextAlignment(Qt::AlignRight);
                tableWidget->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
    } else {
        tableWidget->setRowCount(16);
        tableWidget->setColumnCount(matrix.getLength());
        for (int i = 0, n = matrix.getLength(); i < n; i++) {
            tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(QString("%1").arg(i + 1)));
        }
        for (int i = 0; i < 16; i++) {
            tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(QString(DiProperty::fromIndexHi(i))+QString(DiProperty::fromIndexLo(i))));
            for (int j = 0, n = matrix.getLength(); j < n; j++) {
                tableWidget->setItem(i, j, new QTableWidgetItem(QString("%1").arg(matrix.getValue(i, j))));
                tableWidget->item(i, j)->setTextAlignment(Qt::AlignRight);
                tableWidget->item(i, j)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
    }
    tableWidget->resizeRowsToContents();
    tableWidget->resizeColumnsToContents();
    tableWidget->setMinimumWidth(tableWidget->width());
    tableWidget->setMinimumHeight(tableWidget->verticalHeader()->length() + tableWidget->horizontalHeader()->height() + 20);
    scrollArea->setHidden(true);

    verticalLayout->setStretch(0, 1);
    verticalLayout->setStretch(1, 0);
    verticalLayout->setStretch(2, 0);

    resize(tableWidget->width(), tableWidget->minimumHeight());
}

ViewMatrixDialogController::ViewMatrixDialogController(PFMatrix matrix, QWidget *w): QDialog(w){
    setupUi(this);
    buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    ml = new MatrixAndLogoController(matrix, this);
    MLLayout->addWidget(ml);

    QPushButton* closeButton = buttonBox->button(QDialogButtonBox::Close);
    setMinimumHeight(ml->height() + closeButton->height() + layout()->margin()*2 + layout()->spacing());
    setMinimumWidth(ml->width());
    connect(closeButton, SIGNAL(clicked()), SLOT(sl_onCloseButton()));
}

ViewMatrixDialogController::ViewMatrixDialogController(PWMatrix matrix, QWidget *w): QDialog(w){
    setupUi(this);
    new HelpButton(this, buttonBox, "24749004");
    buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    ml = new MatrixAndLogoController(matrix, this);
    MLLayout->addWidget(ml);

    QPushButton* closeButton = buttonBox->button(QDialogButtonBox::Close);
    setMinimumHeight(ml->height() + closeButton->height() + layout()->margin()*2 + layout()->spacing());
    setMinimumWidth(ml->width());
    connect(closeButton, SIGNAL(clicked()), SLOT(sl_onCloseButton()));
}

void ViewMatrixDialogController::sl_onCloseButton() {
    QDialog::reject();
}


MatrixViewController::MatrixViewController( PFMatrix matrix ): MWMDIWindow(tr("Matrix viewer")){
    d = new MatrixAndLogoController(matrix, this);
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(d);
    setMinimumSize(d->minimumSize());
    setLayout(layout);
}

MatrixViewController::MatrixViewController( PWMatrix matrix ): MWMDIWindow(tr("Matrix viewer")){
    d = new MatrixAndLogoController(matrix, this);
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(d);
    setMinimumSize(d->minimumSize());
    setLayout(layout);
}

} //namespace
