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

#include "FindPatternMsaWidgetFactory.h"

#include <QPixmap>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include "FindPatternMsaWidget.h"

namespace U2 {

const QString FindPatternMsaWidgetFactory::GROUP_ID = "OP_MSA_FIND_PATTERN_WIDGET";
const QString FindPatternMsaWidgetFactory::GROUP_ICON_STR = ":core/images/find_dialog.png";
const QString FindPatternMsaWidgetFactory::GROUP_DOC_PAGE = "65929651";

FindPatternMsaWidgetFactory::FindPatternMsaWidgetFactory() {
    objectViewOfWidget = ObjViewType_AlignmentEditor;
}

#define SEARCH_MODE_OPTION_KEY "FindPatternMsaWidgetFactory_searchMode"

QWidget* FindPatternMsaWidgetFactory::createWidget(GObjectView* objView, const QVariantMap& options) {
    SAFE_POINT(objView != nullptr,
               QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
               nullptr);

    MSAEditor* msaEditor = qobject_cast<MSAEditor*>(objView);
    SAFE_POINT(msaEditor != nullptr,
               QString("Internal error: unable to cast object view to MSAEditor for group '%1'.").arg(GROUP_ID),
               nullptr);

    int searchMode = options.value(SEARCH_MODE_OPTION_KEY).toInt();
    TriState searchInNamesTriState = searchMode == 2 ? TriState_Yes : (searchMode == 1 ? TriState_No : TriState_Unknown);
    return new FindPatternMsaWidget(msaEditor, searchInNamesTriState);
}

OPGroupParameters FindPatternMsaWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Search in Alignment"), GROUP_DOC_PAGE);
}

void FindPatternMsaWidgetFactory::applyOptionsToWidget(QWidget* widget, const QVariantMap& options) {
    FindPatternMsaWidget* findPatternMsaWidget = qobject_cast<FindPatternMsaWidget*>(widget);
    CHECK(findPatternMsaWidget != nullptr, )
    int mode = options.value(SEARCH_MODE_OPTION_KEY).toInt();
    if (mode == 1 || mode == 2) {
        findPatternMsaWidget->setSearchInNamesMode(mode == 2);
    }
}

const QString& FindPatternMsaWidgetFactory::getGroupId() {
    return GROUP_ID;
}

const QVariantMap FindPatternMsaWidgetFactory::getOptionsToActivateSearchInSequences() {
    QVariantMap options;
    options[SEARCH_MODE_OPTION_KEY] = 1;
    return options;
}

const QVariantMap FindPatternMsaWidgetFactory::getOptionsToActivateSearchInNames() {
    QVariantMap options;
    options[SEARCH_MODE_OPTION_KEY] = 2;
    return options;
}

}  // namespace U2
