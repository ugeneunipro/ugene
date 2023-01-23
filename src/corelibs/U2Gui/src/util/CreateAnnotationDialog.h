/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QDialog>

#include <U2Core/global.h>

class Ui_CreateAnnotationDialog;

namespace U2 {

class CreateAnnotationWidgetController;
class CreateAnnotationModel;
class HelpButton;

class U2GUI_EXPORT CreateAnnotationDialog : public QDialog {
    Q_OBJECT
public:
    // saves the results to the provided model
    CreateAnnotationDialog(QWidget* p, CreateAnnotationModel& m, const QString& helpButtonCode = "");
    ~CreateAnnotationDialog();

    void updateAppearance(const QString& newTitle, const QString& newHelpPage, const QString& newOkButtonName);

private slots:
    void accept() override;

private:
    CreateAnnotationModel& model;
    CreateAnnotationWidgetController* annWidgetController = nullptr;
    Ui_CreateAnnotationDialog* ui = nullptr;
    HelpButton* helpButton = nullptr;
};

}  // namespace U2
