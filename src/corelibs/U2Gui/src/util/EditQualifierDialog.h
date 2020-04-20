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

#ifndef _U2_EDIT_QUALIFIER_DIALOG_H_
#define _U2_EDIT_QUALIFIER_DIALOG_H_

#include <U2Core/global.h>
#include <U2Core/U2Qualifier.h>

#include <QDialog>

class Ui_EditQualifierDialog;

namespace U2 {

class U2GUI_EXPORT EditQualifierDialog: public QDialog {
    Q_OBJECT
public:
    EditQualifierDialog(QWidget* p, const U2Qualifier& q, bool ro, bool existingQualifier );
    ~EditQualifierDialog();
    
    virtual void accept();
    
    const U2Qualifier& getModifiedQualifier() const {return q;}

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    U2Qualifier q;
    Ui_EditQualifierDialog* ui;
    
};

}//namespace
#endif
