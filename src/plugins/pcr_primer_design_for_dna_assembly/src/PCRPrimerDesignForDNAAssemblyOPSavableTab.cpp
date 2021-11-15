/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "PCRPrimerDesignForDNAAssemblyOPSavableTab.h"
#include "PCRPrimerDesignForDNAAssemblyOPWidget.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/U2WidgetStateStorage.h>

Q_DECLARE_METATYPE(U2::ResultTableData)
Q_DECLARE_METATYPE(U2::UserPimerLineEditResult)

namespace U2 {

PCRPrimerDesignForDNAAssemblyOPSavableTab::PCRPrimerDesignForDNAAssemblyOPSavableTab(QWidget* wrappedWidget, MWMDIWindow* contextWindow)
    : U2SavableWidget(wrappedWidget, contextWindow), originalWrappedWidget(qobject_cast<PCRPrimerDesignForDNAAssemblyOPWidget*>(wrappedWidget)) {
    SAFE_POINT(originalWrappedWidget != nullptr, L10N::nullPointerError("PCRPrimerDesignForDNAAssemblyOPWidget"), );
}

PCRPrimerDesignForDNAAssemblyOPSavableTab::~PCRPrimerDesignForDNAAssemblyOPSavableTab() {
    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;
}

QVariant PCRPrimerDesignForDNAAssemblyOPSavableTab::getChildValue(const QString& childId) const {
    auto productTable = qobject_cast<ResultTable*>(getChildWidgetById(childId));
    auto userPrimerLineEdit = qobject_cast<UserPimerLineEdit*>(getChildWidgetById(childId));
    auto toolButton = qobject_cast<QToolButton*>(getChildWidgetById(childId));
    if (productTable != nullptr) {
        return QVariant::fromValue<ResultTableData>(productTable->getPCRPrimerProductTableData());
    } else if (userPrimerLineEdit != nullptr) {
        return QVariant::fromValue<UserPimerLineEditResult>(userPrimerLineEdit->getData());
    } else if (toolButton != nullptr) {
        return {};
    } else {
        return U2SavableWidget::getChildValue(childId);
    }
}

void PCRPrimerDesignForDNAAssemblyOPSavableTab::setChildValue(const QString &childId, const QVariant &value) {
    auto productTable = qobject_cast<ResultTable *>(getChildWidgetById(childId));
    auto userPrimerLineEdit = qobject_cast<UserPimerLineEdit*>(getChildWidgetById(childId));
    auto toolButton = qobject_cast<QToolButton*>(getChildWidgetById(childId));
    if (productTable != nullptr) {
        const ResultTableData data = value.value<ResultTableData>();
        productTable->setCurrentProducts(data.currentProducts, data.associatedView);
        productTable->setAnnotationGroup(data.associatedGroup);
        if (data.currentProducts.count(U2Region()) < ResultTable::MAXIMUM_ROW_COUNT) {
            productTable->show();
        }
    } else if (userPrimerLineEdit != nullptr) {
        UserPimerLineEditResult data = value.value<UserPimerLineEditResult>();
        userPrimerLineEdit->setData(data);
    } else if (toolButton != nullptr) {
        return;
    } else {
        return U2SavableWidget::setChildValue(childId, value);
    }
}

}    // namespace U2
