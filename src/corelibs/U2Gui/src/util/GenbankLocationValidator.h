/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QValidator>

class QLineEdit;
class QPushButton;

namespace U2 {

class GenbankLocationValidator : public QValidator {
public:
    GenbankLocationValidator(QPushButton* okButton, qint64 length, bool isCircular, QLineEdit* le);
    ~GenbankLocationValidator();

    State validate(QString& str, int& ii) const override;

private:
    State failValidate() const;

    QPushButton* okButton;
    bool isCircular;
    qint64 seqLen = 0;
    QLineEdit* le;
};

}  // namespace U2
