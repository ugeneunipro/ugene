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

#ifndef _U2_GUI_DOCUMENT_UTILS_H_
#define _U2_GUI_DOCUMENT_UTILS_H_

#include "GTGlobals.h"
#include <U2Core/U2IdTypes.h>

namespace U2 {
using namespace HI;

class GObjectView;
class Document;

class GTUtilsDocument {
public:
    static const QString DocumentUnloaded;

    // checks if the document with a given name exists and loaded in a view with a given factory Id
    static void checkDocument(HI::GUITestOpStatus &os, const QString &documentName, const GObjectViewFactoryId &id = QString());

    static void removeDocument(HI::GUITestOpStatus &os, const QString &documentName, GTGlobals::UseMethod method = GTGlobals::UseKey);

    static Document* getDocument(HI::GUITestOpStatus &os, const QString& documentName);

    static bool isDocumentLoaded(HI::GUITestOpStatus &os, const QString& documentName);

    static void saveDocument(HI::GUITestOpStatus &os, const QString& documentName);
    static void unloadDocument(HI::GUITestOpStatus &os, const QString& documentName, bool waitForMessageBox = true);
    static void loadDocument(HI::GUITestOpStatus &os, const QString& documentName);

    static void lockDocument(HI::GUITestOpStatus &os, const QString& documentName);
    static void unlockDocument(HI::GUITestOpStatus &os, const QString& documentName);

    static void checkIfDocumentIsLocked(HI::GUITestOpStatus &os, const QString &documentName, bool isLocked);

protected:
    static GObjectView* getDocumentGObjectView(HI::GUITestOpStatus &os, Document* d);

private:
    static QList<GObjectView*> getAllGObjectViews();
};

} // namespace

#endif
