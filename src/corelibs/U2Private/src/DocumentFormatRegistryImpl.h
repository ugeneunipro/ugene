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

#ifndef _DOCUMENT_FORMAT_REGISTRY_IMPL_H_
#define _DOCUMENT_FORMAT_REGISTRY_IMPL_H_

#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentImport.h>

namespace U2 {

class U2PRIVATE_EXPORT DocumentFormatRegistryImpl  : public DocumentFormatRegistry {
    Q_OBJECT
public:
    DocumentFormatRegistryImpl(QObject* p = NULL) : DocumentFormatRegistry(p) {init();}
    ~DocumentFormatRegistryImpl() override;

    virtual bool registerFormat(DocumentFormat* dfs) override;

    virtual bool unregisterFormat(DocumentFormat* dfs) override;

    virtual QList<DocumentFormatId> getRegisteredFormats() const override;

    virtual DocumentFormat* getFormatById(DocumentFormatId id) const override;

    virtual DocumentFormat* selectFormatByFileExtension(const QString& fileExt) const override;

    virtual QList<DocumentFormatId> selectFormats(const DocumentFormatConstraints& c) const override;

    virtual DocumentImportersRegistry* getImportSupport() override {return &importSupport;}

private:
    void init();

    QList<QPointer<DocumentFormat> > formats;
    DocumentImportersRegistry   importSupport;
};

}//namespace
#endif
