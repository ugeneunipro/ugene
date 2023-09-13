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

#include "Primer3PresetsWidget.h"

#include <U2Gui/ShowHideSubgroupWidget.h>

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/Log.h>

namespace U2 {

Primer3PresetsWidget::Primer3PresetsWidget(QWidget* parent) : QWidget(parent) {
    setupUi(this);

    /*ShowHideSubgroupWidget* saveShowHideWidget = new ShowHideSubgroupWidget("primer-3-additional-preset-settings", tr("Additional preset settings"), swPresets, false);
    layout()->addWidget(saveShowHideWidget);*/



}

void Primer3PresetsWidget::sl_setCurrentStackedWidgetIndex(int index) {
    swPresets->setCurrentIndex(index);




    for (int i = 0; i < swPresets->count(); i++) {
        CHECK_CONTINUE(i != index);

        swPresets->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    auto currentWidget = swPresets->widget(index);
    SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), );

    currentWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    currentWidget->adjustSize();
    swPresets->adjustSize();
    adjustSize();
    //parentWidget()->adjustSize();
    //dialog->adjustSize();
    coreLog.info(QString("swPresets->widget(0)->height(): %1").arg(swPresets->widget(0)->height()));
    coreLog.info(QString("swPresets->widget(1)->height(): %1").arg(swPresets->widget(1)->height()));
    coreLog.info(QString("swPresets->height(): %1").arg(swPresets->height()));
    coreLog.info(QString("height(): %1").arg(height()));
    coreLog.info(QString("dialog->height(): %1").arg(dialog->height()));


    int he = swPresets->widget(index)->height();
    int mh = swPresets->widget(index)->minimumHeight();
    int ch = height();
    int cmh = minimumHeight();
    int i = 0;

    //connect(swPresets, &QStackedWidget::currentChanged, this, [this](int index) {
    //    // setSizePolicy() and adjustSize() are required for widget resizing on @settingsWidget widget changed
    //    for (int i = 0; i < swPresets->count(); i++) {
    //        CHECK_CONTINUE(i != index);

    //        swPresets->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //    }
    //    auto currentWidget = swPresets->widget(index);
    //    SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), );

    //    currentWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //    currentWidget->adjustSize();
    //    swPresets->adjustSize();
    //    adjustSize();
    //    //parentWidget()->adjustSize();
    //    dialog->adjustSize();
    //    int he = swPresets->widget(index)->height();
    //    int mh = swPresets->widget(index)->minimumHeight();
    //    int ch = height();
    //    int cmh = minimumHeight();
    //    int i = 0;
    //});

}



}