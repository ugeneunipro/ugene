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

#include "FindPatternWidgetSavableTab.h"
#include "FindPatternWidget.h"

#include <U2Gui/U2WidgetStateStorage.h>
#include <U2Core/DNASequenceObject.h>

namespace U2 {

FindPatternWidgetSavableTab::FindPatternWidgetSavableTab(QWidget *wrappedWidget, MWMDIWindow *contextWindow)
    : U2SavableWidget(wrappedWidget, contextWindow)
{
    SAFE_POINT(NULL != qobject_cast<FindPatternWidget *>(wrappedWidget), "Invalid widget provided", );
}

FindPatternWidgetSavableTab::~FindPatternWidgetSavableTab() {
    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;
}

void FindPatternWidgetSavableTab::setChildValue(const QString &childId, const QVariant &value) {
    SAFE_POINT(childExists(childId), "Child widget expected", );
    QVariant result = value;
    if (regionWidgetIds.contains(childId)){
        bool ok = false;
        int intVal = value.toInt(&ok);
        FindPatternWidget *parentWidget = qobject_cast<FindPatternWidget*>(wrappedWidget);
        SAFE_POINT(parentWidget != NULL, "Wrong casting", )
        int sequenceLength = parentWidget->getTargetSequenceLength();
        SAFE_POINT(ok, "Invalid conversion to int", );
        CHECK(regionWidgetIds.size() == 2, );
        if(intVal > sequenceLength){
            if (childId == regionWidgetIds.at(1)) {
                result = QVariant(sequenceLength);
            } else {
                result = QVariant(1);
            }
        }
    }
    U2SavableWidget::setChildValue(childId, result);
}

void FindPatternWidgetSavableTab::setRegionWidgetIds(const QStringList &s) {
    /*
    First item should be start position, second - end
    */
    regionWidgetIds.append(s);
}

}
