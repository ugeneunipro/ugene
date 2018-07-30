/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "LengthSettingsWidget.h"
#include "LineEditHighlighter.h"

namespace U2 {
namespace LocalWorkflow {

const QString LengthSettingsWidget::LENGTH = "length";

LengthSettingsWidget::LengthSettingsWidget(const QString &toolTip) {
    setupUi(this);

    leLength->setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));
    new LineEditHighlighter(leLength);

    lblLength->setToolTip(toolTip);
    leLength->setToolTip(toolTip);

    connect(leLength, SIGNAL(textChanged(QString)), SIGNAL(si_valueChanged()));
}

LengthSettingsWidget::~LengthSettingsWidget() {
    emit si_widgetIsAboutToBeDestroyed(getState());
}

bool LengthSettingsWidget::validate() const {
    return !leLength->text().isEmpty();
}

QVariantMap LengthSettingsWidget::getState() const {
    QVariantMap state;

    const QString lengthString = leLength->text();
    const bool isEmpty = lengthString.isEmpty();
    bool valid = false;
    const int length = lengthString.toInt(&valid);

    if (!isEmpty && valid) {
        state[LENGTH] = length;
    }

    return state;
}

void LengthSettingsWidget::setState(const QVariantMap &state) {
    const bool contains = state.contains(LENGTH);
    bool valid = false;
    const int length = state.value(LENGTH).toInt(&valid);
    if (contains && valid) {
        leLength->setText(QString::number(length));
    }
}

QString LengthSettingsWidget::serializeState(const QVariantMap &widgetState) {
    if (widgetState.contains(LENGTH)) {
        return QString::number(widgetState.value(LENGTH).toInt());
    } else {
        return QString();
    }
}

QVariantMap LengthSettingsWidget::parseState(const QString &command, const QString &stepName) {
    QVariantMap state;
    QRegExp regExp(stepName + ":" + "(\\d*)");

    const bool matched = regExp.exactMatch(command);
    CHECK(matched, state);

    const QString length = regExp.cap(1);
    if (!length.isEmpty()) {
        state[LENGTH] = length.toInt();
    }

    return state;
}

}   // namespace LocalWorkflow
}   // namespace U2
