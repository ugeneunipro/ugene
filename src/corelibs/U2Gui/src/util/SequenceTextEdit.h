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

#include <QPlainTextEdit>

#include <U2Core/global.h>

namespace U2 {

class U2GUI_EXPORT SequenceTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    /**
     * From qt-project.org:
     * "QTextEdit is optimized for working with paragraphs and it is not designed for handling very large paragraph."
     * So, inserting a big sequence as one huge paragraph will cause perfomance problems.
     * TextEdit splits inserted data to paragraphs with the size PARAGRAPH_SIZE.
     */
    SequenceTextEdit(QWidget* p = nullptr);

protected:
    void insertFromMimeData(const QMimeData* source) override;

    static const int PARAGRAPH_SIZE = 5120;
    static const int NO_QUESTION_LIMIT = 5000000;
};

}  // namespace U2
