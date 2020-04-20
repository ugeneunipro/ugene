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

#ifndef _U2_EDIT_FRAGMENT_DIALOG_H_
#define _U2_EDIT_FRAGMENT_DIALOG_H_

#include "DNAFragment.h"

#include <ui_EditFragmentDialog.h>

namespace U2 {

class DNATranslation;

class EditFragmentDialog : public QDialog, public Ui_EditFragmentDialog {
    Q_OBJECT
public:
   EditFragmentDialog(DNAFragment& fragment, QWidget* p);
   virtual void accept();
private:
   DNAFragment& dnaFragment;
   DNATranslation* transl;
   QString seq,trseq;
   void updatePreview();
   void resetLeftOverhang();
   void resetRightOverhang();
   bool isValidOverhang(const QString& text);
private slots:
   void sl_updatePreview();
   void sl_onLeftResetClicked();
   void sl_onRightResetClicked();
   void sl_customOverhangSet(const QString& text);


};


} //namespace

#endif
