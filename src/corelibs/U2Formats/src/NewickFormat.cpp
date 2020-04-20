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

#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "NewickFormat.h"

namespace U2 {

/* TRANSLATOR U2::IOAdapter */
/* TRANSLATOR U2::NewickFormat */

NewickFormat::NewickFormat(QObject* p) : TextDocumentFormat(p, BaseDocumentFormats::NEWICK, DocumentFormatFlags_W1) {
    fileExtensions << "nwk" << "newick" << "nh" << "ph";
    formatName = tr("Newick Standard");
    formatDescription = tr("Newick is a simple format used to write out trees in a text file");
    supportedObjectTypes += GObjectTypes::PHYLOGENETIC_TREE;
}


#define BUFF_SIZE 1024

static QList<GObject*> parseTrees(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap &fs, U2OpStatus& si);

Document* NewickFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){
    QList<GObject*> objects = parseTrees(io, dbiRef, fs, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);
    Document* d = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
    return d;
}

void NewickFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    Q_UNUSED(os);
    assert(d->getDocumentFormat() == this);

    foreach(GObject* obj, d->getObjects()) {
        PhyTreeObject* phyObj = qobject_cast<PhyTreeObject*>(obj);
        if (phyObj != NULL) {
            QByteArray data = NewickPhyTreeSerializer::serialize(phyObj->getTree());
            io->writeBlock(data.constData(), data.size());
        }
    }
}

FormatCheckResult NewickFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    bool containsBinary = TextUtils::contains(TextUtils::BINARY, data, size);
    if (containsBinary) {
        return FormatDetection_NotMatched;
    }
    int brackets = 0;
    typedef enum { letter, letter_than_whites, any } Cases;
    Cases last = any;
    bool quotedLabelStarted = false;
    for (int i = 0; i < size; ++i) {
        if('\'' == data[i]) {
            if(!quotedLabelStarted && i > 0 && (data[i-1] == '(' || data[i-1] == ',')) {
                quotedLabelStarted = true;
            } else if(quotedLabelStarted) {
                quotedLabelStarted = false;
            }
            continue;
        }
        if(quotedLabelStarted) {
            continue;
        }
        switch (data[i]) {
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
                if (data[i] < 0) { // for ex. if file contains utf-8 symbols
                    return FormatDetection_NotMatched;
                }
                if (TextUtils::ALPHA_NUMS[data[i]] || data[i] == '-' || data[i] == '_') {
                    if (last == letter_than_whites) {
                        return FormatDetection_NotMatched;
                    }
                    last = letter;
                    continue;
                }
                if (TextUtils::WHITES[data[i]]) {
                    if (last == letter || last == letter_than_whites) {
                        last = letter_than_whites;
                        continue;
                    }
                }
        }
        last = any;
    }
    if (!rawData.contains(';') || (!rawData.contains('(') && rawData.contains(','))) {
        return FormatDetection_LowSimilarity;
    }
    if (QRegExp("[a-zA-Z\r\n]*").exactMatch(rawData)) {
        return FormatDetection_LowSimilarity;
    }
    int braces = (rawData.contains('(') ? 1 : 0) + (rawData.contains(')') ? 1 : 0) ;
    if (braces == 0 && rawData.length() > 50)  {
        return FormatDetection_LowSimilarity;
    }
    if (braces == 1) {
        return FormatDetection_NotMatched;
    }
    return FormatDetection_HighSimilarity;
}

static QList<GObject*> parseTrees(IOAdapter *io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& si) {
    QList<GObject*> objects;
    DbiOperationsBlock opBlock(dbiRef, si);
    CHECK_OP(si, objects);
    Q_UNUSED(opBlock);
    QList<PhyTree> trees = NewickPhyTreeSerializer::parseTrees(io, si);
    CHECK_OP(si, objects);

    for (int i=0; i<trees.size(); i++) {
        PhyTree tree = trees[i];
        QString objName = (0 == i) ? QString("Tree") : QString("Tree%1").arg(i + 1);
        QVariantMap hints;
        hints.insert(DocumentFormat::DBI_FOLDER_HINT, fs.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
        PhyTreeObject *obj = PhyTreeObject::createInstance(tree, objName, dbiRef, si, hints);
        CHECK_OP(si, objects);
        objects.append(obj);
    }

    return objects;
}

}//namespace
