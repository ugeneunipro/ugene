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

#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#include <U2Core/GUrl.h>
#include <U2Core/PluginModel.h>

#include <QDialog>

#include <ui_LicenseDialog.h>

namespace U2 {

class LicenseDialog : public QDialog, private Ui_LicenseDialog
{
    Q_OBJECT

public:
    explicit LicenseDialog(Plugin * plugin, QWidget *parent = 0);
    ~LicenseDialog();
public slots:
    void sl_accept();

private:
    Ui_LicenseDialog   *ui;
    Plugin              *plugin;

};
}//namespace
#endif // LICENSEDIALOG_H
