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

#pragma once

#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>

namespace U2 {

class U2PRIVATE_EXPORT DocumentFormatRegistryImpl : public DocumentFormatRegistry {
    Q_OBJECT
public:
    DocumentFormatRegistryImpl(QObject* p = nullptr)
        : DocumentFormatRegistry(p) {
        init();
    }
    ~DocumentFormatRegistryImpl() override;

    bool registerFormat(DocumentFormat* dfs) override;

    bool unregisterFormat(DocumentFormat* dfs) override;

    QList<DocumentFormatId> getRegisteredFormats() const override;

    DocumentFormat* getFormatById(DocumentFormatId id) const override;

    DocumentFormat* selectFormatByFileExtension(const QString& fileExt) const override;

    QList<DocumentFormatId> selectFormats(const DocumentFormatConstraints& c) const override;

    DocumentImportersRegistry* getImportSupport() override {
        return &importSupport;
    }

private:
    void init();

    QList<QPointer<DocumentFormat>> formats;
    DocumentImportersRegistry importSupport;
};

}  // namespace U2
