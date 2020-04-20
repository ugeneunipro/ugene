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

#include <U2Core/AppContext.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>

#include "FormatUtils.h"

namespace U2 {

QString FormatUtils::splitThousands(qint64 num) {
    QString prefix = "";
    QString numStr = QString::number(num);
    QString result = "";
    int j = 0;
    for (int i = numStr.length(); --i >= 0; j++) {
        result = numStr.mid(i, 1) + (j > 0 && j % 3 == 0?" " : "") + result;
    }
    return result;
}

QString FormatUtils::formatNumber(qint64 num) {
    if (num >= 1000 * 1000 * 1000) {
        if (num % (1000 * 1000 * 1000) == 0) {
            return QString::number(num/(double)(1000 * 1000 * 1000)) + QString("G");
        }
        if (num % (100 * 1000 * 1000) == 0) {
            return QString::number(num / (double) (1000 * 1000 * 1000), 'f', 1) +
                QString("G");
        }
    }
    if (num >= 1000 * 1000) {
        if (num % (1000 * 1000) == 0) {
            return QString::number(num / (1000 * 1000)) + QString("m");
        }
        if (num % (100 * 1000) == 0) {
            return QString::number(num / (double) (1000 * 1000), 'f', 1) +
                QString("m");
        }
    }
    if (num >= 1000) {
        if (num % 1000 == 0) {
            return QString::number(num / 1000) + QString("k");
        }
        if (num % 100 == 0) {
            return QString::number(num / (double) 1000, 'f', 1) +
                QString("k");
        }
    }
    return QString::number(num);
}

// 6031769.1k -> 6 031 769.1k
static QString insertSpaceSeparators(QString str) {
    for(int i = str.length()-3; i > 0; i-=3) {
        if(str.at(i).isDigit() && i > 0 && str.at(i-1).isDigit()) {
            str.insert(i, " ");
        }
    }
    return str;
}

QString FormatUtils::formatNumberWithSeparators(int num) {
    return insertSpaceSeparators(formatNumber(num));
}

QString FormatUtils::insertSeparators(int num) {
    return insertSpaceSeparators(QString::number(num));
}

QString FormatUtils::getShortMonthName( int num )
{
    switch (num) {
        case 1:
            return QString("JAN");
        case 2:
            return QString("FEB");
        case 3:
            return QString("MAR");
        case 4:
            return QString("APR");
        case 5:
            return QString("MAY");
        case 6:
            return QString("JUN");
        case 7:
            return QString("JUL");
        case 8:
            return QString("AUG");
        case 9:
            return QString("SEP");
        case 10:
            return QString("OCT");
        case 11:
            return QString("NOV");
        case 12:
            return QString("DEC");
        default:
            return QString();
    }
}

static QString getAllFilesFilter() {
    return QObject::tr("All files") + " ( * )";
}

QString FormatUtils::prepareFileFilter(const QMap<QString, QStringList> &formatNamesWithExtensions, bool allowAnyFiles, const QStringList &extraExtensions) {
    QStringList filters;
    foreach (const QString &formatName, formatNamesWithExtensions.keys()) {
        filters << prepareFileFilter(formatName, formatNamesWithExtensions[formatName], false, extraExtensions);
    }

    if (allowAnyFiles) {
        filters << getAllFilesFilter();
    }

    return filters.join(";;");
}

QString FormatUtils::prepareFileFilter(const QString& name, const QStringList& exts, bool any, const QStringList& extra) {
    QString line = name + " (";
    foreach(QString ext, exts) {
        line+=" *."+ext;
    }
    foreach(QString ext, exts) {
        foreach(QString s, extra) {
            line+=" *."+ext+s;
        }
    }
    line+=" )";
    if (any) {
        line += ";;" + getAllFilesFilter();
    }
    return line;
}

static QStringList getExtra(DocumentFormat* df, const QStringList& originalExtra) {
    bool useExtra = !df->getFlags().testFlag(DocumentFormatFlag_NoPack);
    if (useExtra) {
        return originalExtra;
    }
    return QStringList();
}

QString FormatUtils::prepareDocumentsFileFilter(const DocumentFormatId &fid, bool any, const QStringList &extra) {
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(fid);
    QStringList effectiveExtra = getExtra(df, extra);
    QString result = prepareFileFilter(df->getFormatName(), df->getSupportedDocumentFileExtensions(), any, effectiveExtra);
    return result;
}

QString FormatUtils::prepareDocumentsFileFilter(bool any, const QStringList& extra) {
    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> ids = fr->getRegisteredFormats();
    QStringList result;
    foreach(DocumentFormatId id , ids) {
        DocumentFormat* df = fr->getFormatById(id);
        if (df->checkFlags(DocumentFormatFlag_CannotBeCreated)) {
            continue;
        }
        QStringList effectiveExtra = getExtra(df, extra);
        result << prepareFileFilter(df->getFormatName(), df->getSupportedDocumentFileExtensions(), false, effectiveExtra);
    }
    foreach(DocumentImporter* importer, fr->getImportSupport()->getImporters()) {
        QStringList importerExts = importer->getSupportedFileExtensions();
        result << prepareFileFilter(importer->getImporterName(), importerExts, false, QStringList());
    }

    result.sort();
    if (any) {
        result.prepend(getAllFilesFilter());
    }
    return result.join(";;");
}

QString FormatUtils::prepareDocumentsFileFilter(const DocumentFormatConstraints& c, bool any) {
    QStringList result;

    DocumentFormatConstraints internalConstraints(c);
    internalConstraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> ids = AppContext::getDocumentFormatRegistry()->getRegisteredFormats();
    foreach(const DocumentFormatId& id, ids) {
        DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(id);
        if (df->checkConstraints(internalConstraints)) {
            result.append(prepareDocumentsFileFilter(id, false));
        }
    }
    result.sort();
    if (any) {
        result.prepend(getAllFilesFilter());
    }
    return result.join(";;");
}

QString FormatUtils::prepareDocumentsFileFilterByObjType(const GObjectType& t, bool any) {
    DocumentFormatConstraints c;
    c.supportedObjectTypes += t;
    return prepareDocumentsFileFilter(c, any);
}

}
