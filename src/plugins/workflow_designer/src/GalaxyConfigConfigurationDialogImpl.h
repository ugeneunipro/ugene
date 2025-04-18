/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Lang/Schema.h>

#include <ui_GalaxyConfigConfigurationDialog.h>

namespace U2 {
namespace Workflow {

class GalaxyConfigConfigurationDialogImpl : public QDialog, public Ui_GalaxyConfigConfigurationDialog {
    Q_OBJECT
public:
    GalaxyConfigConfigurationDialogImpl(const QString& schemePath, QWidget* p = nullptr);
    bool createGalaxyConfigTask();

private:
    QString schemePath;

private slots:
    void sl_ugeneToolButtonClicked();
    void sl_galaxyToolButtonClicked();
    void sl_destinationToolButtonClicked();

};  // GalaxyConfigConfigurationDialogImpl

}  // namespace Workflow
}  // namespace U2
