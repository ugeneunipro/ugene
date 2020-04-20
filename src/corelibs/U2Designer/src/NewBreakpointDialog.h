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

#ifndef _NEW_BREAKPOINT_DIALOG_H_
#define _NEW_BREAKPOINT_DIALOG_H_

#include <U2Core/global.h>
#include <QDialog>

class Ui_NewBreakpointDialog;

namespace U2 {

class U2DESIGNER_EXPORT NewBreakpointDialog : public QDialog {
    Q_OBJECT
public:
    NewBreakpointDialog(const QStringList &elementsNames, QWidget *parent = NULL,
        Qt::WindowFlags f = 0);
    ~NewBreakpointDialog();

signals:
    void si_newBreakpointCreated(const QString &element);

private slots:
    void sl_announceNewBreakpoint();

private:
    Ui_NewBreakpointDialog *ui;
};

} // namespace U2

#endif // _NEW_BREAKPOINT_DIALOG_H_
