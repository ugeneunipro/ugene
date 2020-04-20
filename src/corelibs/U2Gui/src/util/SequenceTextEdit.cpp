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

#include <QApplication>
#include <QMessageBox>
#include <QMimeData>

#include <U2Core/U2SafePoints.h>

#include <U2Core/QObjectScopedPointer.h>

#include "SequenceTextEdit.h"

namespace U2 {

SequenceTextEdit::SequenceTextEdit(QWidget *p)
    : QPlainTextEdit(p)
{}

void SequenceTextEdit::insertFromMimeData(const QMimeData *source) {
    try {
        SAFE_POINT(source != NULL, tr("Invalid mimedata"), );
        QString data = source->text();
        if (data.size() > NO_QUESTION_LIMIT
                && QMessageBox::question(this,
                                         tr("Pasting large data"),
                                         tr("The clipboard contains a large amount of data.\nIt will take time to paste it.\nDo you want to continue?"),
                                         QMessageBox::Yes, QMessageBox::No)
                == QMessageBox::No) {
            return;
        }

        int splitter = PARAGRAPH_SIZE;
        while ( splitter < data.size() ) {
            data.insert(splitter, '\n');
            splitter += PARAGRAPH_SIZE + 1;
        }

        QMimeData* mimeData = new QMimeData();
        mimeData->setText(data);
        QPlainTextEdit::insertFromMimeData(mimeData);
        delete mimeData;
    }
    catch (...) {
        QObjectScopedPointer<QMessageBox> warning = new QMessageBox(QMessageBox::Warning, tr("Error on pasting large data"), tr("An error occurred on pasting large amount of data.\nText edit was cleared."), QMessageBox::Ok, QApplication::activeWindow());
        warning->setObjectName("ExceptionWarning");
        warning->exec();
        CHECK(!warning.isNull(), );
        clear();
    }
}

} // namespace
