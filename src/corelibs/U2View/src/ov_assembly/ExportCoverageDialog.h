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

#ifndef _U2_EXPORT_COVERAGE_DIALOG_H_
#define _U2_EXPORT_COVERAGE_DIALOG_H_

#include <QDialog>

#include "ExportCoverageTask.h"
#include "ui_ExportCoverageDialog.h"

namespace U2 {

class SaveDocumentController;

class ExportCoverageDialog : public QDialog, public Ui_ExportCoverageDialog {
    Q_OBJECT
public:
    ExportCoverageDialog(const QString &assemblyName, QWidget *parent = NULL);

    ExportCoverageSettings::Format getFormat() const;
    ExportCoverageSettings getSettings() const;

public slots:
    void accept();

private slots:
    void sl_formatChanged(const QString &format);

private:
    void initLayout();
    void init(QString assemblyName);
    bool checkPermissions() const;

    SaveDocumentController * saveController;

    static const QString DIR_HELPER_NAME;
};

}   // namespace U2

#endif // _U2_EXPORT_COVERAGE_DIALOG_H_
