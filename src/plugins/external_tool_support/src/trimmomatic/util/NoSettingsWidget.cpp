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

#include <QLabel>
#include <QVBoxLayout>

#include "NoSettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

NoSettingsWidget::NoSettingsWidget() {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
    mainLayout->addWidget(new QLabel(tr("There are no settings for this step.")));
}

bool NoSettingsWidget::validate() const {
    return true;
}

QVariantMap NoSettingsWidget::getState() const {
    return QVariantMap();
}

void NoSettingsWidget::setState(const QVariantMap &) {
    // Do nothing
}

QString NoSettingsWidget::serializeState(const QVariantMap &) {
    return QString();
}

QVariantMap NoSettingsWidget::parseState(const QString &) {
    return QVariantMap();
}

}   // namespace LocalWorkflow
}   // namespace U2
