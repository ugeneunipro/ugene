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

#ifndef _U2_ANNOTATION_SETTINGS_H_
#define _U2_ANNOTATION_SETTINGS_H_

#include <QHash>
#include <QColor>

#include <U2Core/AnnotationData.h>

namespace U2 {

class Settings;

class U2CORE_EXPORT AnnotationSettings {
public:
    AnnotationSettings();
    AnnotationSettings(const QString& name, bool amino, const QColor& color, bool visible);

    bool operator==(const AnnotationSettings *as) const {return equals(as);}
    bool equals(const AnnotationSettings *as) const;

    QString     name;
    QColor      color;
    bool        amino;
    bool        visible;
    bool        showNameQuals; // Specifies whether to show value of qualifier or not
    QStringList nameQuals;     // The list of qualifiers separated by comma.
                               // If "showNameQuals" is true, the first found value of a qualifier from the list
                               // is shown on the annotation.
};

class U2CORE_EXPORT AnnotationSettingsRegistry : public QObject {
Q_OBJECT
public:
    AnnotationSettingsRegistry(const QList<AnnotationSettings *> &predefined);
    ~AnnotationSettingsRegistry();

    QStringList getAllSettings() const;
    AnnotationSettings * getAnnotationSettings(const QString &name);
    AnnotationSettings * getAnnotationSettings(const SharedAnnotationData &a);

    // persistent==true -> save settings to file, ==false -> this session only
    void changeSettings(const QList<AnnotationSettings *> &settings, bool saveAsPersistent);

signals:
    void si_annotationSettingsChanged(const QStringList &changedSettings);

private:
    void read();
    void save();

    QHash<QString, AnnotationSettings *> persistentMap;
    QHash<QString, AnnotationSettings *> transientMap;
};


}//namespace

#endif

