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

#include "RPackageDialorFiller.h"
#include "api/GTWidget.h"

#include <QtGui/QPushButton>
#include <QApplication>
namespace U2{

#define GT_CLASS_NAME "GTUtilsDialog::StartupDialogFiller"
#define GT_METHOD_NAME "run"
void RPackageDialorFiller::run(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QList<QPushButton*> list= dialog->findChildren<QPushButton*>();

    foreach(QPushButton* but, list){
        if (but->text().contains("Cancel"))
            GTWidget::click(os,but);
    }

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}

