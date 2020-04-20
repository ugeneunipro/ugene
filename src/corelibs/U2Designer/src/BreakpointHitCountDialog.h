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

#ifndef _BREAKPOINT_HIT_COUNT_DIALOG_H_
#define _BREAKPOINT_HIT_COUNT_DIALOG_H_

#include <U2Core/global.h>

#include <QDialog>

extern const int LOWER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER;

class QStringList;

class Ui_BreakpointHitCountDialog;

namespace U2 {

class U2DESIGNER_EXPORT BreakpointHitCountDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(BreakpointHitCountDialog)
public:
    BreakpointHitCountDialog(const QStringList &hitCountConditions, const QString &conditionOnLaunch
        = "", quint32 hitCountParameterOnLaunch = LOWER_BOUNDARY_FOR_HIT_COUNTER_PARAMETER,
        quint32 hitCountOnLaunch = 0, const QStringList &hitCountersListWithoutParameter
        = QStringList(), QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~BreakpointHitCountDialog();

signals:
    void si_resetHitCount();
    void si_hitCounterAssigned(const QString &hitCounterCondition, quint32 parameter);

private slots:
    void sl_dialogAccepted();
    void sl_resetHitCount();
    void sl_hitConditionChanged(const QString &text);

private:
    const QString initialCondition;
    const quint32 initialParameter;
    const QStringList hitCountersConditionsWithoutParameter;
    QString chosenCondition;
    quint32 hitCounterParameter;
    bool isHitCounterReset;
    Ui_BreakpointHitCountDialog *ui;
};

} // namespace U2

#endif // _BREAKPOINT_HIT_COUNT_DIALOG_H_
