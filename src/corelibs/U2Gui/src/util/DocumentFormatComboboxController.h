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

#ifndef _U2_DOCUMENT_FORMAT_COMBOBOX_CONTROLLER_H_
#define _U2_DOCUMENT_FORMAT_COMBOBOX_CONTROLLER_H_

#include <U2Core/DocumentModel.h>

#include <QComboBox>

namespace U2 {
    
class U2GUI_EXPORT DocumentFormatComboboxController : public QObject {
    Q_OBJECT
public:
    DocumentFormatComboboxController(QObject* p, QComboBox* combo, const DocumentFormatConstraints& c,
        DocumentFormatId active = DocumentFormatId::null);

    void updateConstraints(const DocumentFormatConstraints& c);

    DocumentFormatId getActiveFormatId() const;
    void setActiveFormatId(DocumentFormatId);
    bool hasSelectedFormat() const {return !getActiveFormatId().isNull();}
    QList<DocumentFormatId> getFormatsInCombo();
    QComboBox* comboBox()  { return combo; }

    static void fill(QComboBox* combo, QList<DocumentFormatId>& ids, DocumentFormatId active);
    static DocumentFormatId getActiveFormatId(QComboBox* combo);
    static QList<DocumentFormatId> getFormatsInCombo(QComboBox* combo);

private slots:
    void sl_onDocumentFormatRegistered(DocumentFormat*);
    void sl_onDocumentFormatUnregistered(DocumentFormat*);

private:
    void updateCombo(DocumentFormatId active);

    QComboBox* combo;
    DocumentFormatConstraints c;
};

}

#endif
