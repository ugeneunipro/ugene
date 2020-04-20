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

#include "MaxInfoStep.h"

namespace U2 {
namespace LocalWorkflow {

const QString MaxInfoStepFactory::ID = "MAXINFO";

MaxInfoStep::MaxInfoStep()
    : TrimmomaticStep(MaxInfoStepFactory::ID)
{
    name = "MAXINFO";
    description = tr("<html><head></head><body>"
                     "<h4>MAXINFO</h4>"
                     "<p>This step performs an adaptive quality trim, balancing the benefits of "
                     "retaining longer reads against the costs of retaining bases with errors. "
                     "See Trimmomatic manual for details.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Target length</b>: the read length which is likely to allow the "
                     "location of the read within the target sequence. Extremely short reads, "
                     "which can be placed into many different locations, provide little value. "
                     "Typically, the length would be in the order of 40 bases, however, the value "
                     "also depends on the size and complexity of the target sequence.</li>"
                     "<li><b>Strictness</b>: the balance between preserving as much read length "
                     "as possible vs. removal of incorrect bases. A low value of this parameter "
                     "(<0.2) favours longer reads, while a high value (>0.8) favours read correctness.</li>"
                     "</ul>"
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *MaxInfoStep::createWidget() const {
    return new MaxInfoSettingsWidget();
}

QString MaxInfoStep::serializeState(const QVariantMap &widgetState) const {
    QString serializedState;
    if (widgetState.contains(MaxInfoSettingsWidget::TARGET_LENGTH)) {
        serializedState += QString::number(widgetState.value(MaxInfoSettingsWidget::TARGET_LENGTH).toInt());
    }
    serializedState += ":";
    if (widgetState.contains(MaxInfoSettingsWidget::STRICTNESS)) {
        serializedState += QString::number(widgetState.value(MaxInfoSettingsWidget::STRICTNESS).toDouble());
    }
    return serializedState;
}

QVariantMap MaxInfoStep::parseState(const QString &command) const {
    QVariantMap state;
    QRegExp regExp(id + ":" + "(\\d*)" + ":" + "((0|1)(\\.|,)\\d*)");

    const bool matched = regExp.exactMatch(command);
    CHECK(matched, state);

    const QString targetLength = regExp.cap(1);
    if (!targetLength.isEmpty()) {
        state[MaxInfoSettingsWidget::TARGET_LENGTH] = targetLength.toInt();
    }

    const QString strictness = regExp.cap(2);
    if (!strictness.isEmpty()) {
        state[MaxInfoSettingsWidget::STRICTNESS] = strictness.toDouble();
    }

    return state;
}

const QString MaxInfoSettingsWidget::TARGET_LENGTH = "targetLength";
const QString MaxInfoSettingsWidget::STRICTNESS = "strictness";

MaxInfoSettingsWidget::MaxInfoSettingsWidget() {
    setupUi(this);

    connect(sbTargetLength, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged()));
    connect(dsbStrictness, SIGNAL(valueChanged(double)), SIGNAL(si_valueChanged()));
}

MaxInfoSettingsWidget::~MaxInfoSettingsWidget() {
    emit si_widgetIsAboutToBeDestroyed(getState());
}

bool MaxInfoSettingsWidget::validate() const {
    return true;
}

QVariantMap MaxInfoSettingsWidget::getState() const {
    QVariantMap state;
    state[TARGET_LENGTH] = sbTargetLength->value();
    state[STRICTNESS] = dsbStrictness->value();
    return state;
}

void MaxInfoSettingsWidget::setState(const QVariantMap &state) {
    bool contains = state.contains(TARGET_LENGTH);
    bool valid = false;
    const int targetLength = state[TARGET_LENGTH].toInt(&valid);
    if (contains && valid) {
        sbTargetLength->setValue(targetLength);
    }

    contains = state.contains(STRICTNESS);
    const double strictness = state[STRICTNESS].toDouble(&valid);
    if (contains && valid) {
        dsbStrictness->setValue(strictness);
    }
}

MaxInfoStepFactory::MaxInfoStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

MaxInfoStep *MaxInfoStepFactory::createStep() const {
    return new MaxInfoStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
