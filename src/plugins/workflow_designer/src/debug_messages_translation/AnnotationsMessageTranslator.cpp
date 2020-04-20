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

#include <QVariant>

#include <U2Core/AnnotationTableObject.h>
#include <U2Lang/WorkflowContext.h>

#include "AnnotationsMessageTranslator.h"

const char *ANNOTATIONS_COUNT_LABEL = "Count of annotations: ";
const char *ANNOTATION_NAME_LABEL = " Name: ";
const char *REGION_LIST_LABEL = " Regions: ";
const QString NEW_LINE_SYMBOL = "\n";

namespace U2 {

using namespace Workflow;

AnnotationsMessageTranslator::AnnotationsMessageTranslator(const QVariant &atomicMessage, WorkflowContext *initContext)
    : BaseMessageTranslator(atomicMessage, initContext)
{
    annTable = StorageUtils::getAnnotationTable(context->getDataStorage(), source);
}

QString AnnotationsMessageTranslator::getTranslation() const {
    QString result = QObject::tr(ANNOTATIONS_COUNT_LABEL) + QString::number(annTable.size()) + INFO_TAGS_SEPARATOR + NEW_LINE_SYMBOL;
    quint32 annotationsCounter = 1;
    foreach (const SharedAnnotationData &data, annTable) {
        result += " " + QString::number(annotationsCounter) + ". " + QObject::tr(ANNOTATION_NAME_LABEL) + "'" + data->name + "'" + INFO_FEATURES_SEPARATOR;
        QVector<U2Region> annotatedRegions = data->getRegions();
        if (!annotatedRegions.isEmpty()) {
            result += QObject::tr(REGION_LIST_LABEL);
            foreach (const U2Region &region, annotatedRegions) {
                result += region.toString() + INFO_FEATURES_SEPARATOR;
            }
            result = result.left(result.size() - INFO_FEATURES_SEPARATOR.size());
        }
        result += INFO_TAGS_SEPARATOR + NEW_LINE_SYMBOL;
        ++annotationsCounter;
    }
    result = result.left(result.size() - INFO_TAGS_SEPARATOR.size() - NEW_LINE_SYMBOL.size());
    return result;
}

} // namespace U2
