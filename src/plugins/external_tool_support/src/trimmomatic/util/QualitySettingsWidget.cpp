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

#include <QIntValidator>
#include <U2Core/U2SafePoints.h>

#include "QualitySettingsWidget.h"

namespace U2 {
namespace LocalWorkflow {

const QString QualitySettingsWidget::QUALITY = "quality";

QualitySettingsWidget::QualitySettingsWidget(const QString &toolTip) {
    setupUi(this);

    lblQualityThreshold->setToolTip(toolTip);
    sbQualityThreshold->setToolTip(toolTip);

    connect(sbQualityThreshold, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged()));
}

QualitySettingsWidget::~QualitySettingsWidget() {
    emit si_widgetIsAboutToBeDestroyed(getState());
}

bool QualitySettingsWidget::validate() const {
    return true;
}

QVariantMap QualitySettingsWidget::getState() const {
    QVariantMap state;
    state[QUALITY] = sbQualityThreshold->value();
    return state;
}

void QualitySettingsWidget::setState(const QVariantMap &state) {
    const bool contains = state.contains(QUALITY);
    bool valid = false;
    const int quality = state.value(QUALITY).toInt(&valid);
    if (contains && valid) {
        sbQualityThreshold->setValue(quality);
    }
}

QString QualitySettingsWidget::serializeState(const QVariantMap &widgetState) {
    if (widgetState.contains(QUALITY)) {
        return QString::number(widgetState.value(QUALITY).toInt());
    } else {
        return QString();
    }
}

QVariantMap QualitySettingsWidget::parseState(const QString &command, const QString &stepName) {
    QVariantMap state;
    QRegExp regExp(stepName + ":" + "(\\d*)");

    const bool matched = regExp.exactMatch(command);
    CHECK(matched, state);

    const QString quality = regExp.cap(1);
    if (!quality.isEmpty()) {
        state[QUALITY] = quality.toInt();
    }

    return state;
}

}   // namespace LocalWorkflow
}   // namespace U2
