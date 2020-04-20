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

#ifndef _U2_MSA_EDITOR_STATUS_BAR_H_
#define _U2_MSA_EDITOR_STATUS_BAR_H_

#include "MaEditorStatusBar.h"

#include <QRegExpValidator>

class QLineEdit;
class QPushButton;

namespace U2 {

class DNAAlphabet;
class MaSearchValidator;

class MsaEditorStatusBar : public MaEditorStatusBar {
    Q_OBJECT
public:
    MsaEditorStatusBar(MultipleAlignmentObject* mobj, MaEditorSequenceArea* seqArea);

private:
    void setupLayout();
    void updateLabels();
};

class MaSearchValidator : public QRegExpValidator {
public:
    MaSearchValidator(const DNAAlphabet* alphabet, QObject* parent);
    State validate(QString &input, int &pos) const;
};

} // namespace

#endif // _U2_MSA_EDITOR_STATUS_BAR_H_
