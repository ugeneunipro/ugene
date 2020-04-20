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

#ifndef _U2_PRIMER_LIBRARY_WIDGET_H_
#define _U2_PRIMER_LIBRARY_WIDGET_H_

#include <QAbstractItemModel>

#include <U2Core/U2OpStatus.h>

#include <U2Gui/MainWindow.h>

#include "Primer.h"

#include "ui_PrimerLibraryWidget.h"

namespace U2 {

class PrimerLibraryWidget : public QWidget, private Ui_PrimerLibraryWidget {
    Q_OBJECT
public:
    PrimerLibraryWidget(QWidget *parent);

signals:
    void si_close();

private slots:
    void sl_newPrimer();
    void sl_editPrimer();
    void sl_removePrimers();
    void sl_importPrimers();
    void sl_exportPrimers();
    void sl_selectionChanged();

private:
    QPushButton *editPrimerButton;
    QPushButton *removePrimersButton;
    QPushButton *exportPrimersButton;
};

} // U2

#endif // _U2_PRIMER_LIBRARY_WIDGET_H_
