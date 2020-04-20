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

#include "MSAEditorSequenceArea.h"
#include "MsaEditorStatusBar.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleAlignmentObject.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>

namespace U2 {

MsaEditorStatusBar::MsaEditorStatusBar(MultipleAlignmentObject* mobj, MaEditorSequenceArea* seqArea)
    : MaEditorStatusBar(mobj, seqArea) {
    setObjectName("msa_editor_status_bar");
    setStatusBarStyle();
    lineLabel->setPatterns(tr("Seq %1 / %2"), tr("Sequence %1 of %2"));
    updateLabels();
    setupLayout();
}

void MsaEditorStatusBar::setupLayout() {
    layout->addWidget(lineLabel);
    layout->addWidget(colomnLabel);
    layout->addWidget(positionLabel);
    layout->addWidget(selectionLabel);

    layout->addWidget(lockLabel);
}

void MsaEditorStatusBar::updateLabels() {
    updateLineLabel();
    updatePositionLabel();
    updateColumnLabel();
    updateSelectionLabel();
}

MaSearchValidator::MaSearchValidator(const DNAAlphabet* alphabet, QObject *parent)
: QRegExpValidator(parent)
{
    if (!alphabet->isRaw()){
        QByteArray alphabetChars = alphabet->getAlphabetChars(true);
        //remove special characters
        alphabetChars.remove(alphabetChars.indexOf('*'), 1);
        alphabetChars.remove(alphabetChars.indexOf('-'), 1);
        setRegExp(QRegExp(QString("[%1]+").arg(alphabetChars.constData())));
    }
}

QValidator::State MaSearchValidator::validate(QString &input, int &pos) const {
    input = input.simplified();
    input = input.toUpper();
    input.remove(" ");
    input.remove("-"); // Gaps are not used in search model
    return QRegExpValidator::validate(input, pos);
}

} // namespace
