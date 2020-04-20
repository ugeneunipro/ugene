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

#include "AnnotationSettings.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/FeatureColors.h>
#include <U2Core/AnnotationTableObject.h>

#include <QSet>

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
    foreach(AnnotationSettings* s, settings) {
        assert(s->color.isValid());
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
    return (persistentMap.keys() + transientMap.keys()).toSet().toList();
}

AnnotationSettings * AnnotationSettingsRegistry::getAnnotationSettings(const SharedAnnotationData &a) {
    AnnotationSettings *s = getAnnotationSettings(a->name);
    //don't show non-positional features that span the whole sequence
    if (a->findFirstQualifierValue("non-positional") != QString::null) {
        s->visible = false;
    }
    return s;
}

AnnotationSettings* AnnotationSettingsRegistry::getAnnotationSettings(const QString &name) {
    //Search in persistent settings:
    AnnotationSettings *s = persistentMap.value(name);
    if (s != NULL) {
        return s;
    }

    //search in transient cache:
    s = transientMap.value(name);
    if (s!=NULL) {
        return s;
    }
    s = new AnnotationSettings();
    s->name = name;
    s->color = FeatureColors::genLightColor(name);
    s->visible = true;
    if (transientMap.size() == MAX_CACHE_SIZE) {
        //todo: mutex!?
        transientMap.erase(transientMap.begin());
    }
    transientMap[name]=s;
    return s;
}

void AnnotationSettingsRegistry::read() {

    Settings* s = AppContext::getSettings();
    QStringList annotations = s->getChildGroups(SETTINGS_ROOT);
    QList<AnnotationSettings*> list;
    foreach(const QString& name, annotations) {
        AnnotationSettings* as = transientMap.value(name);
        if (as == NULL) {
            as = new AnnotationSettings();
            as->name = name;
        }
        QVariant color = s->getValue(SETTINGS_ROOT + as->name + "/color", FeatureColors::genLightColor(as->name).name());
        as->color = QColor(color.toString());
        if (!as->color.isValid()) {
            // previously color was stored as QColor, not by name
            as->color = color.value<QColor>();
            if (!as->color.isValid()) { // if still invalid - get the default value
                as->color = FeatureColors::genLightColor(as->name);
            }
        }
        as->visible = s->getValue(SETTINGS_ROOT + as->name + "/visible", true).toBool();
        as->amino = s->getValue(SETTINGS_ROOT + as->name + "/amino", true).toBool();
        as->showNameQuals = s->getValue(SETTINGS_ROOT + as->name + "/show_quals", false).toBool();
        QString qs = s->getValue(SETTINGS_ROOT + as->name + "/quals", "").toString();
        if (!qs.isEmpty()) {
            as->nameQuals = qs.split(',', QString::SkipEmptyParts);
        }
        list.append(as);
    }
    changeSettings(list, false);
}

void AnnotationSettingsRegistry::save() {
    Settings* s = AppContext::getSettings();
    QStringList keys = s->getAllKeys(SETTINGS_ROOT);
    foreach(const AnnotationSettings* as, persistentMap.values()) {
        s->setValue(SETTINGS_ROOT + as->name + "/color", as->color.name());
        s->setValue(SETTINGS_ROOT + as->name + "/visible", as->visible);
        s->setValue(SETTINGS_ROOT + as->name + "/amino", as->amino);
        s->setValue(SETTINGS_ROOT + as->name + "/show_quals", as->showNameQuals);
        s->setValue(SETTINGS_ROOT + as->name + "/quals", as->nameQuals.join(","));
    }
}

//////////////////////////////////////////////////////////////////////////
AnnotationSettings::AnnotationSettings() {
    amino = false;
    color = Qt::black;
    visible = true;
    showNameQuals = false;
}

AnnotationSettings::AnnotationSettings(const QString& _name, bool _amino, const QColor& _color, bool _visible)
    : name(_name),
      color(_color),
      amino(_amino),
      visible(_visible),
      showNameQuals(false)
{
}

bool AnnotationSettings::equals(const AnnotationSettings* as) const {
    return name == as->name
        && amino == as->amino
        && color == as->color
        && visible == as->visible
        && showNameQuals == as->showNameQuals
        && nameQuals == as->nameQuals;
}


}//namespace
