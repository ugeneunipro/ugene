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

#include <QComboBox>
#include <QObject>

#include <U2Core/DocumentModel.h>

namespace U2 {

class Project;

class U2GUI_EXPORT ProjectDocumentComboBoxController : public QObject {
    Q_OBJECT
public:
    ProjectDocumentComboBoxController(Project* p, QComboBox* cb, QObject* parent, const DocumentFilter* f);
    ~ProjectDocumentComboBoxController();

    Document* getDocument() const;

    void selectDocument(Document* d);

    void selectDocument(const QString& url);

private slots:
    void sl_onDocumentAdded(Document* d);
    void sl_onDocumentRemoved(Document* d);

private:
    bool checkConstraints(Document* d);
    void addDocument(Document* d);
    void removeDocument(Document* d);

    Project* p;
    QComboBox* cb;
    const DocumentFilter* filter;
};

}  // namespace U2
