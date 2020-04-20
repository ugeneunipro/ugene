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

#include <U2Core/U2SafePoints.h>

#include "WDWizardPage.h"
#include "WizardPageController.h"

namespace U2 {

WDWizardPage::WDWizardPage(WizardPageController *controller, QWidget *parent)
: QWizardPage(parent), controller(controller)
{
    controller->setQtPage(this);
}

WDWizardPage::~WDWizardPage() {

}

void WDWizardPage::initializePage () {
    controller->applyLayout();
}

static QAbstractButton * getRunButton(QWizard *w) {
    QAbstractButton *runButton = w->button(QWizard::CustomButton1);
    CHECK(NULL != runButton, NULL);
    CHECK(!runButton->text().isEmpty(), NULL);
    return runButton;
}

void WDWizardPage::showEvent(QShowEvent *event) {
    if (wizard()->currentPage() == this) {
        setupDialogSize();
    }
    QAbstractButton *runButton = getRunButton(wizard());
    if (NULL != runButton) {
        runButton->setVisible(isFinalPage());
    }
    QWizardPage::showEvent(event);
}

void WDWizardPage::setupDialogSize() {
    adjustSize();
    wizard()->setFixedSize(wizard()->sizeHint());
}

int WDWizardPage::nextId() const {
    return controller->nextId();
}

} // U2
