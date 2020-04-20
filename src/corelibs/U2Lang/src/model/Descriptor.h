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

#ifndef _U2_DESC_H_
#define _U2_DESC_H_

#include <U2Core/global.h>
#include <QMap>
#include <QString>
#include <QMetaType>

#include <QIcon>

namespace U2 {

/**
 * Class Descriptor represents an uniquely identifiable entity,
 * which can be exposed to user in some way. So it also has a name and a description.
 */
class U2LANG_EXPORT Descriptor {
public:
    Descriptor(const QString& id, const QString& name, const QString& doc);
    Descriptor(const QString& _id);
    Descriptor(const char* _id);
    Descriptor();
    virtual ~Descriptor() {}

    // standard getters/setters
    QString getId() const;
    QString getDisplayName() const;
    QString getDocumentation() const;
    void setId(const QString& i);
    virtual void setDisplayName(const QString& n);
    virtual void setDocumentation(const QString& d);

    // descriptors with equal id's are equal
    inline bool operator==(const Descriptor& d) const { return id == d.id; }
    inline bool operator!=(const Descriptor& d) const { return id != d.id; }
    inline bool operator==(const QString& s) const { return id == s; }
    inline bool operator!=(const QString& s) const { return id != s; }

    // necessary for using with QMap e.g. QMap<Descriptor, Attribute>
    inline bool operator<(const Descriptor& d) const { return id < d.id; }

protected:
    // entity identifier
    QString id;
    // entity name
    QString name;
    // description of entity
    QString desc;

}; // Descriptor

/**
 * It is a Descriptor with icon
 */
class U2LANG_EXPORT VisualDescriptor : public Descriptor {
public:
    VisualDescriptor(const Descriptor& d, const QString & _iconPath = QString());

    //getIcon will create icon from its path (if path exists) on the first call.
    //This is compatibility issue for congene.
    QIcon getIcon();
    void setIcon(QIcon icon_);
    void setIconPath( const QString & iconPath_ );

private:
    // full path to the icon
    QString iconPath;
    // user defined or taken from iconPath icon
    QIcon icon;

}; // VisualDescriptor

} //namespace U2

Q_DECLARE_METATYPE(U2::Descriptor)
Q_DECLARE_METATYPE(QList<U2::Descriptor>)

#endif
