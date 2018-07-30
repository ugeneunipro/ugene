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

#ifndef _U2_MAX_INFO_STEP_H_
#define _U2_MAX_INFO_STEP_H_

#include "ui_MaxInfoSettingsWidget.h"
#include "trimmomatic/TrimmomaticStep.h"

namespace U2 {
namespace LocalWorkflow {

class MaxInfoStep : public TrimmomaticStep {
    Q_OBJECT
public:
    MaxInfoStep();

    TrimmomaticStepSettingsWidget* createWidget() const;

private:
    QString serializeState(const QVariantMap &widgetState) const;
    QVariantMap parseState(const QString &command) const;
};

class MaxInfoSettingsWidget : public TrimmomaticStepSettingsWidget, private Ui_MaxInfoSettingsWidget {
    Q_OBJECT
public:
    MaxInfoSettingsWidget();
    ~MaxInfoSettingsWidget();

    bool validate() const;

    QVariantMap getState() const;
    void setState(const QVariantMap &state);

    static const QString TARGET_LENGTH;
    static const QString STRICTNESS;
};

class MaxInfoStepFactory : public TrimmomaticStepFactory {
public:
    static const QString ID;

    MaxInfoStepFactory();

    MaxInfoStep *createStep() const;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_MAX_INFO_STEP_H_
