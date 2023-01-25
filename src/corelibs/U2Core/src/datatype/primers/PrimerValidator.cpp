/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PrimerValidator.h"

#include <U2Core/AppContext.h>
#include <U2Core/U2AlphabetUtils.h>

namespace U2 {

PrimerValidator::PrimerValidator(QObject* parent, bool allowExtended)
    : QRegExpValidator(parent) {
    const DNAAlphabet* alphabet = AppContext::getDNAAlphabetRegistry()->findById(
        allowExtended ? BaseDNAAlphabetIds::NUCL_DNA_EXTENDED() : BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    QByteArray alphabetChars = alphabet->getAlphabetChars(true);
    // Gaps are not allowed
    alphabetChars.remove(alphabetChars.indexOf('-'), 1);
    setRegExp(QRegExp(QString("[%1]+").arg(alphabetChars.constData())));
}

QValidator::State PrimerValidator::validate(QString& input, int& pos) const {
    input = input.simplified();
    input = input.toUpper();
    input.remove(" ");
    return QRegExpValidator::validate(input, pos);
}

}  // namespace U2
