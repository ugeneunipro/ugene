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

#include "ColorSchemaSettingsController.h"

#include <QColor>
#include <QDir>

#include <U2Algorithm/ColorSchemeUtils.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>

#include "ColorSchemaDialogController.h"

namespace U2 {

static void setSchemaColors(const ColorSchemeData &customSchema) {
    QString dirPath = ColorSchemeUtils::getColorsDir();

    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);

    QScopedPointer<IOAdapter> io(factory->createIOAdapter());

    const QMap<char, QColor> &alphColors = customSchema.alpColors;
    bool defaultType = customSchema.defaultAlpType;

    QString keyword(customSchema.type == DNAAlphabet_AMINO ? ColorSchemeUtils::COLOR_SCHEME_AMINO_KEYWORD : (defaultType ? ColorSchemeUtils::COLOR_SCHEME_NUCL_DEFAULT_KEYWORD : ColorSchemeUtils::COLOR_SCHEME_NUCL_EXTENDED_KEYWORD));

    QString url = dir.filePath(customSchema.name + ColorSchemeUtils::COLOR_SCHEME_NAME_FILTERS);
    bool ok = io->open(url, IOAdapterMode_Write);
    if (!ok) {
        uiLog.error(ColorSchemaSettingsPageWidget::tr("Failed to save schema file: '%1'").arg(url));
        return;
    }

    // write header
    QByteArray header;
    header.append(QString("%1\n").arg(keyword));
    io->writeBlock(header);
    // write body
    QMapIterator<char, QColor> it(alphColors);
    while (it.hasNext()) {
        it.next();
        QByteArray line;
        line.append(QString("%1=%2\n").arg(it.key()).arg(it.value().name()));
        io->writeBlock(line);
    }
}

const QString ColorSchemaSettingsPageController::helpPageId = QString("49447083");

ColorSchemaSettingsPageController::ColorSchemaSettingsPageController(MsaColorSchemeRegistry *mcsr, QObject *p)
    : AppSettingsGUIPageController(tr("Alignment Color Scheme"), ColorSchemaSettingsPageId, p) {
    connect(this, SIGNAL(si_customSettingsChanged()), mcsr, SLOT(sl_onCustomSettingsChanged()));
}

AppSettingsGUIPageState *ColorSchemaSettingsPageController::getSavedState() {
    ColorSchemaSettingsPageState *state = new ColorSchemaSettingsPageState();
    state->colorsDir = ColorSchemeUtils::getColorsDir();
    state->customSchemas = ColorSchemeUtils::getSchemas();

    return state;
}

void ColorSchemaSettingsPageController::saveState(AppSettingsGUIPageState *s) {
    ColorSchemaSettingsPageState *state = qobject_cast<ColorSchemaSettingsPageState *>(s);

    ColorSchemeUtils::setColorsDir(state->colorsDir);
    QDir dir(ColorSchemeUtils::getColorsDir());
    foreach (const ColorSchemeData &schema, state->removedCustomSchemas) {
        dir.remove(schema.name + ColorSchemeUtils::COLOR_SCHEME_NAME_FILTERS);
    }
    foreach (const ColorSchemeData &schema, state->customSchemas) {
        setSchemaColors(schema);
    }
    emit si_customSettingsChanged();
}

AppSettingsGUIPageWidget *ColorSchemaSettingsPageController::createWidget(AppSettingsGUIPageState *state) {
    ColorSchemaSettingsPageWidget *r = new ColorSchemaSettingsPageWidget(this);
    r->setState(state);
    return r;
}

}    // namespace U2
