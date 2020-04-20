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

#include "AttributeRelation.h"
#include <U2Core/FormatUtils.h>
#include <U2Core/GUrl.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/AppContext.h>

#include <U2Lang/ConfigurationEditor.h>

namespace U2 {

void AttributeRelation::updateDelegateTags(const QVariant & /*influencingValue*/, DelegateTags * /*dependentTags*/) const {

}

VisibilityRelation::VisibilityRelation(const QString &relatedAttrId, const QVariantList &_visibilityValues,
                                       bool invertVisibilityRules)
    : AttributeRelation(relatedAttrId),
      visibilityValues(_visibilityValues),
      invertAffectResult(invertVisibilityRules)
{

}

VisibilityRelation::VisibilityRelation(const QString &relatedAttrId, const QVariant &visibilityValue,
                                       bool invertVisibilityRules)
    : AttributeRelation(relatedAttrId),
      invertAffectResult(invertVisibilityRules)
{
    visibilityValues << visibilityValue;
}

QVariant VisibilityRelation::getAffectResult(const QVariant &influencingValue, const QVariant &,
    DelegateTags *, DelegateTags *) const {
    foreach (const QVariant &v, visibilityValues) {
        if ((v == influencingValue) != invertAffectResult) {
            return true;
        }
    }
    return false;
}

VisibilityRelation *VisibilityRelation::clone() const {
    return new VisibilityRelation(*this);
}

QVariant FileExtensionRelation::getAffectResult(const QVariant &influencingValue, const QVariant &dependentValue,
    DelegateTags * /*infTags*/, DelegateTags *depTags) const {

    QString newFormatId = influencingValue.toString();
    DocumentFormat *newFormat = AppContext::getDocumentFormatRegistry()->getFormatById(newFormatId);
    updateDelegateTags(influencingValue, depTags);

    QString urlStr = dependentValue.toString();
    if (urlStr.isEmpty()) {
        return "";
    }

    QString extension;
    if (NULL == newFormat) {
        extension = newFormatId;
    } else {
        extension = newFormat->getSupportedDocumentFileExtensions().first();
    }

    QString lastSuffix = GUrl(urlStr).lastFileSuffix();
    bool withGz = false;
    if ("gz" == lastSuffix) {
        int dotPos = urlStr.length() - lastSuffix.length() - 1;
        if ((dotPos >= 0) && (QChar('.') == urlStr[dotPos])) {
            withGz = true;
            urlStr = urlStr.left(dotPos);
            lastSuffix = GUrl(urlStr).lastFileSuffix();
        }
    }

    DocumentFormat *currentFormat = AppContext::getDocumentFormatRegistry()->selectFormatByFileExtension(lastSuffix);
    QString currentFormatId("");
    if(currentFormat){
       currentFormatId = currentFormat->getFormatId();
    }

    bool foundExt = false;
    if (0 == QString::compare(lastSuffix, "csv", Qt::CaseInsensitive)) {
        foundExt = true;
    }else if (NULL == currentFormat) {
        foundExt = (lastSuffix == currentFormatId);
    } else {
        QStringList extensions(currentFormat->getSupportedDocumentFileExtensions());
        if (NULL == newFormat) {
            extensions << newFormatId;
        } else {
            extensions << newFormat->getSupportedDocumentFileExtensions();
        }
        foreach (QString supExt, extensions) {
            if (lastSuffix == supExt) {
                foundExt = true;
                break;
            }
        }
    }

    if (foundExt) {
        int dotPos = urlStr.length() - lastSuffix.length() - 1;
        if ((dotPos >= 0) && (QChar('.') == urlStr[dotPos])) { //yes, lastSuffix is a correct extension with .
            urlStr = urlStr.left(dotPos);
        }
    }

    urlStr += "." + extension;
    if (withGz) {
        urlStr += ".gz";
    }
    return urlStr;
}

void FileExtensionRelation::updateDelegateTags(const QVariant &influencingValue, DelegateTags *dependentTags) const {
    QString newFormatId = influencingValue.toString();
    DocumentFormat *newFormat = AppContext::getDocumentFormatRegistry()->getFormatById(newFormatId);
    if (NULL != dependentTags) {
        dependentTags->set("format", newFormatId);
        QString filter = newFormatId + " files (*." + newFormatId + ")";
        if (NULL != newFormat) {
            filter = FormatUtils::prepareDocumentsFileFilter(newFormatId, true);
        }
        dependentTags->set("filter", filter);
    }
}

FileExtensionRelation *FileExtensionRelation::clone() const {
    return new FileExtensionRelation(*this);
}

QVariant ValuesRelation::getAffectResult(const QVariant &influencingValue, const QVariant &dependentValue,
                                         DelegateTags * /*infTags*/, DelegateTags *depTags) const {
    updateDelegateTags(influencingValue, depTags);
    QVariantMap items = dependencies.value(influencingValue.toString()).toMap();
    if (!items.isEmpty()) {
        return items.value(items.keys().first());
    }
    return dependentValue;
}

void ValuesRelation::updateDelegateTags(const QVariant &influencingValue, DelegateTags *dependentTags) const {
    QVariantMap items = dependencies.value(influencingValue.toString()).toMap();
    if (!items.isEmpty()) {
        dependentTags->set("AvailableValues", items);
    }
}

ValuesRelation *ValuesRelation::clone() const {
    return new ValuesRelation(*this);
}

} // U2
