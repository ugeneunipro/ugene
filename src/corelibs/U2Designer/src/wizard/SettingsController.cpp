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

#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <U2Gui/U2FileDialog.h>

#include "SettingsController.h"

namespace U2 {

const QString SettingsController::TYPE_URL = "url";

SettingsController::SettingsController(WizardController *wc, SettingsWidget *_sw)
    : WidgetController(wc), sw(_sw), lineEdit(NULL)
{

}

SettingsController::~SettingsController() {

}

QWidget * SettingsController::createGUI(U2OpStatus &os) {
    QWidget *result = NULL;

    if (sw->type() == TYPE_URL) {
        result = createUrlSettingsWidget(os);
    } else {
        os.setError("Unrecognized settings widget");
    }

    return result;
}

void SettingsController::sl_valueChanged() {
    wc->setVariableValue(sw->var(), getSettingValue());
}

void SettingsController::sl_fileSelect() {
    if (sw->type() == TYPE_URL) {
        QString newValue = U2FileDialog::getExistingDirectory();
        if (!newValue.isEmpty()) {
            lineEdit->setText(newValue);
            sl_valueChanged();
        }
    }
}

QWidget* SettingsController::createUrlSettingsWidget(U2OpStatus &/*os*/) {
    QWidget *result = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(result);
    mainLayout->setMargin(0);

    QLabel *label = new QLabel(sw->label());
    mainLayout->addWidget(label);

    lineEdit = new QLineEdit;
    lineEdit->setObjectName("urlLineEdit");

    QString settingName = sw->var();
    settingName.remove(0, SettingsWidget::SETTING_PREFIX.length());
    lineEdit->setText(AppContext::getSettings()->getValue(settingName).toString());
    sl_valueChanged();

    connect(lineEdit, SIGNAL(editingFinished()), SLOT(sl_valueChanged()));
    mainLayout->addWidget(lineEdit);

    QToolButton* toolButton = new QToolButton;
    toolButton->setText("...");
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_fileSelect()));
    mainLayout->addWidget(toolButton);
    return result;
}

QString SettingsController::getSettingValue() {
    if (sw->type() == TYPE_URL) {
        return lineEdit->text();
    }
    return QString();
}

} // U2
