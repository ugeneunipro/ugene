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

#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <U2Core/Theme.h>

#include <U2Gui/HelpButton.h>

#include "StartupDialog.h"

namespace U2 {

StartupDialog::StartupDialog(QWidget *parent)
: QDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28966927");

    label->setStyleSheet(Theme::infoHintStyleSheet());

    outDirWidget = new OutputDirectoryWidget(this, false /*don't commitOnHide*/);
    QVBoxLayout *l = new QVBoxLayout(box);
    l->setMargin(3);
    l->addWidget(outDirWidget);

    const QPushButton *button = buttonBox->button(QDialogButtonBox::Ok);
    connect(button, SIGNAL(clicked()), this, SLOT(sl_accepted()));

#ifdef Q_OS_WIN
    QFont f( "Arial", 8, QFont::Bold);
    label->setFont(f);
#endif

}

void StartupDialog::sl_accepted(){
    QDir dir(outDirWidget->getChoosenDir());
    if(!dir.exists()){
        dir.mkpath(dir.absolutePath());
    }
    QFile file(dir.filePath("ex1.txt"));
    if (!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this, this->windowTitle(), tr("You have no write access to the folder. Please choose another one."));
        return;
    }
    file.close();
    file.remove();
    outDirWidget->commit();
    QDialog::accept();
}

} // U2
