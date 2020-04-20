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

#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>

#include "InSilicoPcrOptionPanelWidget.h"
#include "InSilicoPcrProductsTable.h"
#include "PrimerGroupBox.h"
#include "PrimerLineEdit.h"

#include "PcrOptionsPanelSavableTab.h"

static const QString PCR_PRODUCTS_TABLE_NAME = "productsTable";
static const QString WIDGET_ID_SEPARATOR = "__";

typedef QPair<U2::ADVSequenceObjectContext *, QList<U2::InSilicoPcrProduct> > AdvContextPcrProductPair;

Q_DECLARE_METATYPE(AdvContextPcrProductPair)

namespace U2 {

PcrOptionsPanelSavableTab::PcrOptionsPanelSavableTab(QWidget *wrappedWidget, MWMDIWindow *contextWindow)
    : U2SavableWidget(wrappedWidget, contextWindow), originalWrappedWidget(qobject_cast<InSilicoPcrOptionPanelWidget *>(wrappedWidget))
{
    SAFE_POINT(NULL != originalWrappedWidget, "Invalid input widget", );
}

PcrOptionsPanelSavableTab::~PcrOptionsPanelSavableTab() {
    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;
}

QVariant PcrOptionsPanelSavableTab::getChildValue(const QString &childId) const {
    InSilicoPcrProductsTable *productTable = qobject_cast<InSilicoPcrProductsTable *>(getChildWidgetById(childId));

    if (NULL != productTable) {
        const QList<InSilicoPcrProduct> &products = qobject_cast<InSilicoPcrProductsTable *>(productTable)->getAllProducts();
        AdvContextPcrProductPair data(productTable->getCurrentSequenceContext(), products);
        return QVariant::fromValue<AdvContextPcrProductPair>(data);
    } else {
        return U2SavableWidget::getChildValue(childId);
    }
}

void PcrOptionsPanelSavableTab::setChildValue(const QString &childId, const QVariant &value) {
    InSilicoPcrProductsTable *productTable = qobject_cast<InSilicoPcrProductsTable *>(getChildWidgetById(childId));

    if (NULL != productTable) {
        originalWrappedWidget->setResultTableShown(true);

        const AdvContextPcrProductPair data = value.value<AdvContextPcrProductPair>();
        productTable->showProducts(data.second, data.first);
    } else {
        return U2SavableWidget::setChildValue(childId, value);
    }
}

bool PcrOptionsPanelSavableTab::childValueIsAcceptable(const QString &childId, const QVariant &value) const {
    if (PCR_PRODUCTS_TABLE_NAME == childId) {
        const AdvContextPcrProductPair data = value.value<AdvContextPcrProductPair>();

        AnnotatedDNAView *dnaView = originalWrappedWidget->getDnaView();
        SAFE_POINT(NULL != dnaView, "Invalid sequence view detected", false);

        return dnaView->getSequenceContexts().contains(data.first);
    } else {
        return U2SavableWidget::childValueIsAcceptable(childId, value);
    }
}

QString PcrOptionsPanelSavableTab::getChildId(QWidget *child) const {
    if (NULL != qobject_cast<PrimerGroupBox *>(child->parent())) {
        QWidget *parentGroupBox = qobject_cast<QWidget *>(child->parent());
        return U2SavableWidget::getChildId(parentGroupBox) + WIDGET_ID_SEPARATOR + child->objectName();
    } else {
        return U2SavableWidget::getChildId(child);
    }
}

QWidget * PcrOptionsPanelSavableTab::getPrimerEditWidgetById(const QString &childId) const {
    const QStringList ids = childId.split(WIDGET_ID_SEPARATOR);
    SAFE_POINT(2 == ids.size(), "Invalid widget ID", NULL);
    QWidget *primerGroup = wrappedWidget->findChild<QWidget *>(ids.first());
    SAFE_POINT(NULL != primerGroup, "Invalid parent widget", NULL);
    return primerGroup->findChild<QWidget *>(ids.last());
}

QWidget * PcrOptionsPanelSavableTab::getChildWidgetById(const QString &childId) const {
    if (childId.contains(WIDGET_ID_SEPARATOR)) {
        return getPrimerEditWidgetById(childId);
    } else {
        return U2SavableWidget::getChildWidgetById(childId);
    }
}

bool PcrOptionsPanelSavableTab::childExists(const QString &childId) const {
    if (childId.contains(WIDGET_ID_SEPARATOR)) {
        return NULL != getPrimerEditWidgetById(childId);
    } else {
        return U2SavableWidget::childExists(childId);
    }
}

} // namespace U2
