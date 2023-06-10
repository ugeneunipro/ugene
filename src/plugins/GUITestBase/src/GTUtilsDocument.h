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

#include <U2Core/U2IdTypes.h>

#include "GTGlobals.h"

namespace U2 {
using namespace HI;

class GObjectViewController;
class Document;

class GTUtilsDocument {
public:
    static const QString DocumentUnloaded;

    // checks if the document with a given name exists and loaded in a view with a given factory Id
    static void checkDocument(const QString& documentName, const GObjectViewFactoryId& id = QString());

    static void removeDocument(const QString& documentName, GTGlobals::UseMethod method = GTGlobals::UseKey);

    /**
     * Finds document with the given name in the project.
     * Waits until document is found or fails unless options.failIfNotFound is false.
     */
    static Document* getDocument(const QString& documentName, const GTGlobals::FindOptions& options = {});

    static bool isDocumentLoaded(const QString& documentName);

    static void saveDocument(const QString& documentName);
    static void unloadDocument(const QString& documentName, bool waitForMessageBox = true);
    static void loadDocument(const QString& documentName);

    static void lockDocument(const QString& documentName);
    static void unlockDocument(const QString& documentName);

    static void checkIfDocumentIsLocked(const QString& documentName, bool isLocked);

protected:
    static GObjectViewController* getDocumentGObjectView(Document* d);

private:
    static QList<GObjectViewController*> getAllGObjectViews();
};

}  // namespace U2
