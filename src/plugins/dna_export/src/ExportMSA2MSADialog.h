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

#ifndef _U2_EXPORT_MSA_TO_MSA_DIALOG_H_
#define _U2_EXPORT_MSA_TO_MSA_DIALOG_H_

#include <U2Core/global.h>

#include "ui_ExportMSA2MSADialog.h"

namespace U2 {

class SaveDocumentController;

class ExportMSA2MSADialog : public QDialog, Ui_ExportMSA2MSADialog {
    Q_OBJECT
public:
    ExportMSA2MSADialog(const QString& defaultFileName, const DocumentFormatId& f, bool wholeAlignmentOnly, QWidget* p);

    /*!
     * \class UnknownAmino
     * \brief Enum class, which indicates what char unknown amino bases should be
     */
    enum class UnknownAmino {
        X,
        Gap
    };

    void updateModel();
    DocumentFormatId formatId;
    QString file;
    /*!
     * Include gaps of trim before translating
     */
    bool includeGaps = false;
    /*!
     * The character unknown amino acids should be translated to
     */
    UnknownAmino unknownAmino = UnknownAmino::X;
    /*!
     * The translation frame. Positive - direct, negative - complement.
     * Possible values - 1, 2, 3, -1, -2, -3.
     */
    int translationFrame = 1;
    bool addToProjectFlag;
    QString translationTable;
    bool exportWholeAlignment;

private slots:
    void sl_exportClicked();

private:
    void initSaveController(const QString& defaultFileName, const DocumentFormatId& defaultFormatId);

    SaveDocumentController* saveController;
    QList<QString> tableID;
};

}  // namespace U2

#endif
