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

#ifndef _EDIT_BREAKPOINT_LABELS_DIALOG_H_
#define _EDIT_BREAKPOINT_LABELS_DIALOG_H_

#include <QDialog>
#include <QMap>

#include <U2Core/global.h>

class QKeyEvent;
class Ui_EditBreakpointLabelsDialog;

namespace U2 {

class U2DESIGNER_EXPORT EditBreakpointLabelsDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(EditBreakpointLabelsDialog)
public:
    EditBreakpointLabelsDialog(const QStringList &existingLabels = QStringList(),
        const QStringList &initCallingBreakpointLabels = QStringList(), QWidget *parent = 0,
        Qt::WindowFlags f = 0);
    ~EditBreakpointLabelsDialog();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

signals:
    void si_labelsCreated(QStringList newLabels);
    void si_labelAddedToCallingBreakpoint(QStringList newLabels);

private slots:
    void sl_labelApplianceStateChanged(int state);
    void sl_newLabelEditChanged(const QString &text);
    void sl_newLabelAdded();
    void sl_dialogAccepted();

private:
    void initExistingLabelsList(const QStringList &existingLabels);
    void addNewLabelToList(const QString &newLabel, bool appliedToCallingBreakpoint);

    QMap<QWidget *, QString> applienceControlsForLabels;

    QStringList callingBreakpointLabels;
    QStringList newLabelsAdded;
    Ui_EditBreakpointLabelsDialog *ui;
};

} // namespace U2

#endif // _EDIT_BREAKPOINT_LABELS_DIALOG_H_
