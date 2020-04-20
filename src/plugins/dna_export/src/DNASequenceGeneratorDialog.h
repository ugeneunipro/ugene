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

#ifndef _U2_DNA_SEQUENCE_GENERATOR_DIALOG_H_
#define _U2_DNA_SEQUENCE_GENERATOR_DIALOG_H_

#include "ui_DNASequenceGeneratorDialog.h"

namespace U2 {

class SaveDocumentController;

class DNASequenceGeneratorDialog : public QDialog, public Ui_DNASequenceGeneratorDialog {
    Q_OBJECT
public:
    DNASequenceGeneratorDialog(QWidget* p=NULL);

private slots:
    void sl_browseReference();
    void sl_generate();
    void sl_seedStateChanged(int state);
    void sl_enableRefMode();
    void sl_enableBaseMode();
    void sl_enableGCSkewMode();

private:
    void initSaveController();

    SaveDocumentController* saveController;
    static QMap<char, qreal> content;
    QPushButton* generateButton;
    QPushButton* cancelButton;
    QMap<char, qreal>& percentMap;
    float gcSkew;
};

} //namespace

#endif
