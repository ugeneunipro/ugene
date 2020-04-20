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

#ifndef _U2_DI_PROPERTIES_SITECON_H_
#define _U2_DI_PROPERTIES_SITECON_H_

#include <QMap>
#include <QList>
#include <QString>

#include <U2Core/DIProperties.h>

#include <assert.h>

namespace U2 {

class DiPropertySitecon;
class DinucleotitePropertyRegistry {
public:
    DinucleotitePropertyRegistry();
    ~DinucleotitePropertyRegistry();

    QList<DiPropertySitecon*> getProperties() const {return props;}

private:
    void registerProperty(const QString& str);
    QList<DiPropertySitecon*> props;
};

class DiPropertySitecon : public DiProperty {

public:
    //todo: default val;

    QMap<QString,QString>   keys;
    float                   original[16];
    float                   normalized[16];
    float                   average;
    float                   sdeviation;

    float getOriginal(char c1, char c2) const {return original[index(c1, c2)];}
    float getNormalized(char c1, char c2) const {return normalized[index(c1, c2)];}
};

}//namespace

#endif
