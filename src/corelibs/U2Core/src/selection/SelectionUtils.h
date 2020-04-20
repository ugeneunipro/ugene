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

#ifndef _U2_SELECTION_UTILS_H_
#define _U2_SELECTION_UTILS_H_

#include <U2Core/U2Region.h>

#include <QSet>

namespace U2 {

class GObject;
class Document;
class MultiGSelection;
class GSelection;

class U2CORE_EXPORT SelectionUtils {
public:

    static QList<GObject*>  findObjects(GObjectType t, const MultiGSelection* s, UnloadedObjectFilter f);

    static QList<GObject*>  findObjectsKeepOrder(GObjectType t, const MultiGSelection* s, UnloadedObjectFilter f);

    static QList<GObject*>  findObjects(GObjectType t, const GSelection* s, UnloadedObjectFilter f);

    static QList<GObject*>  findObjectsKeepOrder(GObjectType t, const GSelection* s, UnloadedObjectFilter f);

    static QSet<Document*>  findDocumentsWithObjects(GObjectType t, const MultiGSelection* s,
                                UnloadedObjectFilter f, bool deriveDocsFromObjectSelection);

    static QSet<Document*>  findDocumentsWithObjects(GObjectType t, const GSelection* s,
                                UnloadedObjectFilter f, bool deriveDocsFromObjectSelection);

    static bool isDocumentInSelection(const Document* doc, const MultiGSelection& ms, bool deriveDocsFromObjectSelection);

    static QList<Document*> getSelectedDocs(const MultiGSelection& ms);

    static QList<GObject*> getSelectedObjects(const MultiGSelection& ms);

    //todo: find a better place: like DNAUtils..
    static U2Region normalizeRegionBy3(U2Region reg, int seqLen, bool direct);
};




}//namespace

#endif

