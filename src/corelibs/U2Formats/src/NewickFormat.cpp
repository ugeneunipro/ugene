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

#include "NewickFormat.h"

#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

NewickFormat::NewickFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::NEWICK, DocumentFormatFlags_W1, {"nwk", "newick", "nh", "ph"}) {
    formatName = tr("Newick Standard");
    formatDescription = tr("Newick is a simple format used to write out trees in a text file");
    supportedObjectTypes += GObjectTypes::PHYLOGENETIC_TREE;
}

static QList<GObject*> parseTrees(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& si);

Document* NewickFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    QList<GObject*> objects = parseTrees(reader, dbiRef, hints, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), nullptr);
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints);
}

void NewickFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    const QList<GObject*>& objects = document->getObjects();
    for (GObject* obj : qAsConst(objects)) {
        if (auto phyObj = qobject_cast<PhyTreeObject*>(obj)) {
            QString text = NewickPhyTreeSerializer::serialize(phyObj->getTree(), os);
            CHECK_OP(os, );
            writer.write(os, text);
            CHECK_OP(os, );
        }
    }
}

FormatCheckResult NewickFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    int brackets = 0;
    typedef enum { letter,
                   letter_than_whites,
                   any } Cases;
    Cases last = any;
    bool quotedLabelStarted = false;
    for (int i = 0; i < dataPrefix.size(); ++i) {
        char ch = dataPrefix[i].toLatin1();
        if (ch == '\'') {
            if (!quotedLabelStarted && i > 0 && (dataPrefix[i - 1] == '(' || dataPrefix[i - 1] == ',')) {
                quotedLabelStarted = true;
            } else if (quotedLabelStarted) {
                quotedLabelStarted = false;
            }
            continue;
        }
        if (quotedLabelStarted) {
            continue;
        }
        switch (ch) {
            case '(':
                ++brackets;
                break;
            case ')':
                if (brackets == 0) {
                    return FormatDetection_NotMatched;
                }
                --brackets;
                break;
            case ';':
                if (brackets != 0) {
                    return FormatDetection_NotMatched;
                }
                break;
            default:
                if (ch < 0) {  // for ex. if file contains utf-8 symbols
                    return FormatDetection_NotMatched;
                }
                if (TextUtils::ALPHA_NUMS[ch] || ch == '-' || ch == '_') {
                    if (last == letter_than_whites) {
                        return FormatDetection_NotMatched;
                    }
                    last = letter;
                    continue;
                }
                if (TextUtils::WHITES[ch]) {
                    if (last == letter || last == letter_than_whites) {
                        last = letter_than_whites;
                        continue;
                    }
                }
        }
        last = any;
    }
    if (!dataPrefix.contains(';') || (!dataPrefix.contains('(') && dataPrefix.contains(','))) {
        return FormatDetection_LowSimilarity;
    }
    if (QRegExp("[a-zA-Z\r\n]*").exactMatch(dataPrefix)) {
        return FormatDetection_LowSimilarity;
    }
    int braces = (dataPrefix.contains('(') ? 1 : 0) + (dataPrefix.contains(')') ? 1 : 0);
    if (braces == 0 && dataPrefix.length() > 50) {
        return FormatDetection_LowSimilarity;
    }
    if (braces == 1) {
        return FormatDetection_NotMatched;
    }
    return FormatDetection_HighSimilarity;
}

static QList<GObject*> parseTrees(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& si) {
    QList<GObject*> objects;
    DbiOperationsBlock opBlock(dbiRef, si);
    CHECK_OP(si, objects);
    QList<PhyTree> trees = NewickPhyTreeSerializer::parseTrees(reader, si);
    CHECK_OP(si, objects);

    for (int i = 0; i < trees.size(); i++) {
        const PhyTree& tree = trees[i];
        QString objName = (i == 0) ? QString("Tree") : QString("Tree%1").arg(i + 1);
        QVariantMap hints;
        hints.insert(DocumentFormat::DBI_FOLDER_HINT, fs.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
        PhyTreeObject* obj = PhyTreeObject::createInstance(tree, objName, dbiRef, si, hints);
        CHECK_OP(si, objects);
        objects.append(obj);
    }
    return objects;
}

}  // namespace U2
