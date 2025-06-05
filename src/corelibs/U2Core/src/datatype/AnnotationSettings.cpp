/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "AnnotationSettings.h"

#include <QSet>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/FeatureColors.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

#define SETTINGS_ROOT QString("annotation_settings/")
#define MAX_CACHE_SIZE 1000

AnnotationSettingsRegistry::AnnotationSettingsRegistry(const QList<AnnotationSettings*>& predefined) {
    changeSettings(predefined, false);
    read();
}

AnnotationSettingsRegistry::~AnnotationSettingsRegistry() {
    save();
    qDeleteAll(persistentMap);
    qDeleteAll(transientMap);
}

void AnnotationSettingsRegistry::changeSettings(const QList<AnnotationSettings*>& settings, bool saveAsPersistent) {
    if (settings.isEmpty()) {
        return;
    }
    QStringList changedNames;
    foreach (AnnotationSettings* s, settings) {
        assert(s->lightColor.isValid());
        assert(s->darkColor.isValid());
        assert(!s->name.isEmpty());
        persistentMap.remove(s->name);
        transientMap.remove(s->name);
        if (saveAsPersistent) {
            persistentMap[s->name] = s;
        } else {
            transientMap[s->name] = s;
        }
        changedNames.append(s->name);
    }
    emit si_annotationSettingsChanged(changedNames);
}

QStringList AnnotationSettingsRegistry::getAllSettings() const {
    QStringList allKeys = persistentMap.keys() + transientMap.keys();
    QSet<QString> uniqueKeys(allKeys.begin(), allKeys.end());
    return QStringList(uniqueKeys.begin(), uniqueKeys.end());
}

AnnotationSettings* AnnotationSettingsRegistry::getAnnotationSettings(const SharedAnnotationData& a) {
    AnnotationSettings* s = getAnnotationSettings(a->name);
    // don't show non-positional features that span the whole sequence
    if (a->findFirstQualifierValue("non-positional") != QString()) {
        s->visible = false;
    }
    return s;
}

AnnotationSettings* AnnotationSettingsRegistry::getAnnotationSettings(const QString& name) {
    // Search in persistent settings:
    AnnotationSettings* s = persistentMap.value(name);
    if (s != nullptr) {
        return s;
    }

    // search in transient cache:
    s = transientMap.value(name);
    if (s != nullptr) {
        return s;
    }
    s = new AnnotationSettings();
    s->name = name;
    s->lightColor = FeatureColors::genLightColor(name);
    s->darkColor = FeatureColors::transformLightToDark(s->lightColor);
    s->visible = true;
    if (transientMap.size() == MAX_CACHE_SIZE) {
        // todo: mutex!?
        transientMap.erase(transientMap.begin());
    }
    transientMap[name] = s;
    return s;
}

void AnnotationSettingsRegistry::read() {
    Settings* s = AppContext::getSettings();
    QStringList annotations = s->getChildGroups(SETTINGS_ROOT);
    QList<AnnotationSettings*> list;
    foreach (const QString& name, annotations) {
        AnnotationSettings* as = transientMap.value(name);
        if (as == nullptr) {
            as = new AnnotationSettings();
            as->name = name;
        }

        auto getColor = [s, as](const QString& tag, const QColor& defaultColor) -> QColor {
            QVariant color = s->getValue(SETTINGS_ROOT + as->name + "/" + tag, defaultColor.name());
            QColor result;
            result = QColor(color.toString());
            if (!result.isValid()) {
                // previously color was stored as QColor, not by name
                result = color.value<QColor>();
                if (!result.isValid()) {  // if still invalid - get the default value
                    result = defaultColor;
                }
            }

            return result;
        };

        as->lightColor = getColor("color", FeatureColors::genLightColor(as->name));
        as->darkColor = getColor("color_dark", FeatureColors::transformLightToDark(as->lightColor));
        as->visible = s->getValue(SETTINGS_ROOT + as->name + "/visible", true).toBool();
        as->amino = s->getValue(SETTINGS_ROOT + as->name + "/amino", true).toBool();
        as->showNameQuals = s->getValue(SETTINGS_ROOT + as->name + "/show_quals", false).toBool();
        QString qs = s->getValue(SETTINGS_ROOT + as->name + "/quals", "").toString();
        if (!qs.isEmpty()) {
            as->nameQuals = qs.split(',', Qt::SkipEmptyParts);
        }
        list.append(as);
    }
    changeSettings(list, false);
}

void AnnotationSettingsRegistry::save() {
    Settings* s = AppContext::getSettings();
    QStringList keys = s->getAllKeys(SETTINGS_ROOT);
    foreach (const AnnotationSettings* as, persistentMap.values()) {
        s->setValue(SETTINGS_ROOT + as->name + "/color", as->lightColor.name());
        s->setValue(SETTINGS_ROOT + as->name + "/color_dark", as->darkColor.name());
        s->setValue(SETTINGS_ROOT + as->name + "/visible", as->visible);
        s->setValue(SETTINGS_ROOT + as->name + "/amino", as->amino);
        s->setValue(SETTINGS_ROOT + as->name + "/show_quals", as->showNameQuals);
        s->setValue(SETTINGS_ROOT + as->name + "/quals", as->nameQuals.join(","));
    }
}

//////////////////////////////////////////////////////////////////////////
AnnotationSettings::AnnotationSettings() {
    amino = false;
    lightColor = Qt::black;
    darkColor = Qt::white;
    visible = true;
    showNameQuals = false;
}

AnnotationSettings::AnnotationSettings(const QString& _name, bool _amino, const QColor& _lightColor, bool _visible)
    : name(_name),
      lightColor(_lightColor),
      amino(_amino),
      visible(_visible),
      showNameQuals(false) {
    darkColor = FeatureColors::transformLightToDark(lightColor);
}

bool AnnotationSettings::equals(const AnnotationSettings* as) const {
    return name == as->name &&
           amino == as->amino &&
           lightColor == as->lightColor &&
           darkColor == as->darkColor &&
           visible == as->visible &&
           showNameQuals == as->showNameQuals &&
           nameQuals == as->nameQuals;
}

void AnnotationSettings::setLightColor(const QColor& _lightColor) {
    lightColor = _lightColor;
}

void AnnotationSettings::setDarkColor(const QColor& _darkColor) {
    darkColor = _darkColor;
}

void AnnotationSettings::setActiveColor(const QColor& color) {
    auto mw = AppContext::getMainWindow();
    CHECK(mw != nullptr, );

    if (mw->isDarkTheme()) {
        darkColor = color;
    } else {
        lightColor = color;
    }
}

const QColor& AnnotationSettings::getActiveColor() const {
    auto mw = AppContext::getMainWindow();
    CHECK(mw != nullptr, lightColor);

    return mw->isDarkTheme() ? darkColor : lightColor;
}

}  // namespace U2
