/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "UnwantedStructuresInBackboneDialog.h"

#include <U2Gui/HelpButton.h>

namespace U2 {

UnwantedStructuresInBackboneDialog::UnwantedStructuresInBackboneDialog(const QByteArray &sequence,
                                                                       const QString &unwantedStructures,
                                                                       int sequencesCandidatesNumber,
                                                                       QWidget *parent)
    : QDialog(parent) {
    setupUi(this);
    sequenceTextEdit->setText(QString(sequence));
    unwantedTextEdit->setText(unwantedStructures);
    QString sequenceDeclension;
    switch (sequencesCandidatesNumber) {
        case 0:
            sequenceDeclension = tr("are no sequences");
            break;
        case 1:
            sequenceDeclension = tr("is 1 sequence");
            break;
        default:
            sequenceDeclension = tr("are %1 sequences").arg(sequencesCandidatesNumber);
    }
    questionLabel->setText(tr("There %1 in the file left. Use this sequence as the backbone?").arg(sequenceDeclension));

    buttonBox->button(QDialogButtonBox::Yes)->setText(tr("Yes"));
    buttonBox->button(QDialogButtonBox::No)->setText(tr("No"));
    new HelpButton(this, buttonBox, "71958585");
}

}  // namespace U2
