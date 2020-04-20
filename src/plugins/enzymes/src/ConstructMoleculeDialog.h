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

#ifndef _U2_CONSTRUCT_MOLECULE_DIALOG_H_
#define _U2_CONSTRUCT_MOLECULE_DIALOG_H_

#include <QList>
#include <ui_ConstructMoleculeDialog.h>

#include "CloningUtilTasks.h"

namespace U2 {

class SaveDocumentController;

class ConstructMoleculeDialog : public QDialog, public Ui_ConstructMoleculeDialog {
    Q_OBJECT
public:
    ConstructMoleculeDialog(const QList<DNAFragment>& fragments, QWidget* parent);
    virtual void accept();

private slots:
    void sl_onTakeButtonClicked();
    void sl_onTakeAllButtonClicked();
    void sl_onAddFromProjectButtonClicked();
    void sl_onClearButtonClicked();
    void sl_onUpButtonClicked();
    void sl_onDownButtonClicked();
    void sl_onRemoveButtonClicked();
    void sl_makeCircularBoxClicked();
    void sl_forceBluntBoxClicked();
    void sl_onEditFragmentButtonClicked();
    void sl_onItemClicked( QTreeWidgetItem * item, int column );

protected:
    bool eventFilter(QObject* obj , QEvent* event);

private:
    void update();
    void initSaveController();

    QList<DNAFragment> fragments;
    QList<int> selected;
    SaveDocumentController *saveController;
};


} //namespace

#endif // _U2_CONSTRUCT_MOLECULE_DIALOG_H_
