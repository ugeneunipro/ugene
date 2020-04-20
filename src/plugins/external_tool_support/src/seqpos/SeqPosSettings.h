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

#ifndef _U2_SEQPOS_SETTINGS_
#define _U2_SEQPOS_SETTINGS_


#include <QList>
#include <QString>
#include <QStringList>

namespace U2 {


class SeqPosSettings {
public:

    static const QString MOTIF_DB_CISTROME;
    static const QString MOTIF_DB_PDM;
    static const QString MOTIF_DB_Y1H;
    static const QString MOTIF_DB_TRANSFAC;
    static const QString MOTIF_DB_HDPI;
    static const QString MOTIF_DB_JASPAR;

    SeqPosSettings();

    QString     outDir;
    QString     genomeAssembly;
    bool        findDeNovo;
    QString     motifDB;
    QString     outName;
    qint64      regWidth;
    float       pVal;

public:
     QStringList getArguments(const QString& annFilePath);

private:
    void initDefault();

};

} // U2

#endif // _U2_SEQPOS_SETTINGS_
