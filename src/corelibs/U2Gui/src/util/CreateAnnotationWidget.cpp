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

#include <QMenu>

#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2FeatureType.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include "CreateAnnotationWidget.h"

namespace U2 {

CreateAnnotationWidget::CreateAnnotationWidget(QWidget *parent) :
    QWidget(parent)
{
}

QPair<QWidget *, QWidget *> CreateAnnotationWidget::getTabOrderEntryAndExitPoints() const {
    return QPair<QWidget *, QWidget *>(NULL, NULL);
}

void CreateAnnotationWidget::sl_selectExistingTableRequest() {
    emit si_selectExistingTableRequest();
}

void CreateAnnotationWidget::sl_selectGroupNameMenuRequest() {
    emit si_selectGroupNameMenuRequest();
}

void CreateAnnotationWidget::sl_groupNameEdited() {
    emit si_groupNameEdited();
}

void CreateAnnotationWidget::sl_annotationNameEdited() {
    emit si_annotationNameEdited();
}

void CreateAnnotationWidget::sl_usePatternNamesStateChanged() {
    emit si_usePatternNamesStateChanged();
}

QStringList CreateAnnotationWidget::getFeatureTypes(bool useAminoAnnotationTypes) {
    QStringList featureTypes;
    const QList<U2FeatureType> types = U2FeatureTypes::getTypes(useAminoAnnotationTypes ? U2FeatureTypes::Alphabet_Amino : U2FeatureTypes::Alphabet_Nucleic);
    foreach (U2FeatureType type, types) {
        featureTypes << U2FeatureTypes::getVisualName(type);
    }
    return featureTypes;
}

bool CreateAnnotationWidget::caseInsensitiveLessThan(const QString &first, const QString &second) {
    return QString::compare(first, second, Qt::CaseInsensitive) < 0;
}

void CreateAnnotationWidget::sl_complementLocation() {
    QString locationString = getLocationString();
    const bool wasComplement = isComplementLocation(locationString);
    if (wasComplement) {
        locationString = locationString.mid(11, locationString.length() - 12);
    } else {
        locationString = "complement(" + locationString + ")";
    }
    setLocation(parseGenbankLocationString(locationString));
}

QString CreateAnnotationWidget::getGenbankLocationString(const U2Location &location) {
    QString locationString = U1AnnotationUtils::buildLocationString(location->regions);
    if (location->strand.isCompementary()) {
        locationString = "complement(" + locationString + ")";
    }
    return locationString;
}

U2Location CreateAnnotationWidget::parseGenbankLocationString(const QString &locationString) {
    U2Location location;
    Genbank::LocationParser::parseLocation(locationString.toLatin1().constData(), locationString.length(), location);
    return location;
}

bool CreateAnnotationWidget::isComplementLocation(const QString &locationString) {
    return locationString.startsWith("complement(") && locationString.endsWith(")");
}

}   // namespace U2
