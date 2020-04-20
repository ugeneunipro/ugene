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

#ifndef _U2_MACS_SETTINGS_
#define _U2_MACS_SETTINGS_

#include <U2Core/U2Region.h>

#include <QList>
#include <QString>
#include <QStringList>

namespace U2 {

class MACSSettings {
public:
    MACSSettings();

    QString     outDir;
    QString     fileNames;
    bool        wiggleOut;
    qint64      wiggleSpace;

    qint64      genomeSizeInMbp;
    double      pValue;
    double      qValue;    //(MACS 2)
    bool        useModel;
    U2Region    modelFold;
    qint64      shiftSize;
    QString     keepDublicates;
    qint64      bandWidth;
    bool        extFr;    //(???)
    //optional
    qint64      tagSize; //0 for default
    //advanced
    bool        useLambda;
    qint64      smallNearby;
    qint64      largeNearby;
    bool        autoBimodal;
    bool        scaleLarge;
    bool        shiftControl;    //(MACS 2)
    bool        halfExtend;    //(MACS 2)
    bool        broad;    //(MACS 2)
    double      broadCutoff;    //(MACS 2)

public:
    QStringList getArguments(const QString& treatFilePath, const QString& conFilePath);

private:
    void initDefault();

};

} // U2

#endif // _U2_MACS_SETTINGS_
