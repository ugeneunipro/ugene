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

#ifndef _U2_EXTRACT_ASSEMBLY_REGION_DIALOG_H_
#define _U2_EXTRACT_ASSEMBLY_REGION_DIALOG_H_

#include <QDialog>

#include "ExtractAssemblyRegionTask.h"

#include "ui_ExtractAssemblyRegionDialog.h"

#include <U2Core/U2Region.h>

#include <U2Formats/SQLiteDbi.h>

#include "ExtractAssemblyRegionTask.h"
#include "ui_ExtractAssemblyRegionDialog.h"

namespace U2 {

class SaveDocumentController;
class RegionSelector;

class ExtractAssemblyRegionDialog : public QDialog, public Ui_ExtractAssemblyRegionDialog {
    Q_OBJECT
public:
    ExtractAssemblyRegionDialog(QWidget * p, ExtractAssemblyRegionTaskSettings* settings);

    virtual void accept();

private slots:
    void sl_regionChanged(const U2Region& newRegion);

private:
    void initSaveController();

    SaveDocumentController * saveController;
    RegionSelector * regionSelector;
    ExtractAssemblyRegionTaskSettings *settings;
};

} // namespace

#endif
