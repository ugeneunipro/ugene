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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "WidgetController.h"
#include "WDWizardPage.h"
#include "WizardController.h"

#include "WizardPageController.h"

namespace U2 {

WizardPageController::WizardPageController(WizardController *wc, WizardPage *page)
: wPage(NULL), wc(wc), page(page)
{

}

WizardPageController::~WizardPageController() {

}

void WizardPageController::setQtPage(WDWizardPage *value) {
    wPage = value;
}

WDWizardPage * WizardPageController::getQtPage() const {
    return wPage;
}

WizardPage * WizardPageController::getPage() const {
    return page;
}

void WizardPageController::applyLayout() {
    wc->clearControllers();
    qDeleteAll(controllers);
    controllers.clear();
    QLayout *old = wPage->layout();
    removeLayout(old);

    U2OpStatusImpl os;
    page->validate(wc->getCurrentActors(), os);
    if (os.hasError()) {
        coreLog.error(os.getError());
        setError(wPage);
        return;
    }

    PageContentCreator pcc(wc);
    page->getContent()->accept(&pcc);
    if (wc->isBroken()) {
        setError(wPage);
        return;
    }
    pcc.setPageTitle(page->getTitle());
    wPage->setLayout(pcc.getResult());
    controllers << pcc.getControllers();

    wPage->setFinalPage(page->isFinal());
}

void WizardPageController::setError(WDWizardPage *wPage) {
    wc->setBroken();
    QLayout *l = new QHBoxLayout(wPage);
    QString text = QObject::tr("The page is broken. Please, close the wizard and report us the error: ugene@unipro.ru");
    wPage->setFinalPage(true);
    l->addWidget(new QLabel(text));
    wPage->setLayout(l);
}

namespace {
    QList<QLayout*> removeOneLayoutContent(QLayout *l) {
        QList<QLayout*> result;

        while (l->count() > 0) {
            QLayoutItem *item = l->takeAt(0);
            if (NULL != item->widget()) {
                item->widget()->setParent(NULL);
                delete item;
            } else if (NULL != item->layout()) {
                result << item->layout();
            } else {
                delete item;
            }
        }
        return result;
    }
}

void WizardPageController::removeLayout(QLayout *l) {
    CHECK(NULL != l, );
    QList<QLayout*> layouts;
    QList<QLayout*> layoutStack;
    layouts << l;
    layoutStack << l;

    while (!layouts.isEmpty()) {
        QLayout *current = layouts.takeFirst();
        QList<QLayout*> innerLayouts = removeOneLayoutContent(current);
        layouts << innerLayouts;
        layoutStack << innerLayouts;
    }

    while (!layoutStack.isEmpty()) {
        QLayout *l = layoutStack.takeLast();
        delete l;
    }
}

int WizardPageController::nextId() const {
    if (wc->isBroken()) {
        return -1;
    }
    QString id = page->getNextId(wc->getVariables());
    if (id.isEmpty()) {
        return -1;
    }
    return wc->getQtPageId(id);
}

} // U2
