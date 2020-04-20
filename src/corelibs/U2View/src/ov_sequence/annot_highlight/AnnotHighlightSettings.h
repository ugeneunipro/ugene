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

#ifndef _U2_ANNOT_HIGHLIGHT_SETTINGS_H_
#define _U2_ANNOT_HIGHLIGHT_SETTINGS_H_

#include <ui_AnnotHighlightSettings.h>

namespace U2 {

class AnnotationSettings;

class AnnotHighlightSettingsWidget : public QWidget, private Ui_annotHighlightSettings
{
    Q_OBJECT
public:
    AnnotHighlightSettingsWidget(QWidget* parent = 0);

    void setSettings(AnnotationSettings* annotSettings, bool disableShowTranslations);

signals:
    void si_annotSettingsChanged(AnnotationSettings* annotSettings);

private slots:
    void sl_onShowHideChanged(int checkedState);
    void sl_onShowOnTranslationChanged(int checkedState);
    void sl_onShowQualifierChanged(int checkedState);
    void sl_onEditQualifiersChanged(const QString&);

private:
    void setIncorrectState();
    void setCorrectState();

    AnnotationSettings* currentSettings;
};

} // namespace

#endif
