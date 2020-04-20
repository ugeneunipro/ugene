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

#ifndef _U2_CEAS_SETTINGS_
#define _U2_CEAS_SETTINGS_

#include <QList>
#include <QString>
#include <QStringList>

namespace U2 {

class CEASSettings {
public:
    CEASSettings();
    CEASSettings(const QString &bedFile, const QString &wigFile, const QString &gdbFile);

    void setBedFile(const QString &value);
    void setWigFile(const QString &value);
    void setGdbFile(const QString &value);

    void setImagePath(const QString &value);
    //unused
    void setImageFormat(const QString &value);
    void setAnnsFilePath(const QString &value);

    void setExtraBedFile(const QString &value);
    void setExperimentName(const QString &value);

    void setSizes(int value);
    void setSizes(int val1, int val2, int val3);

    void setBisizes(int value);
    void setBisizes(int val1, int val2);

    void setBg(bool value);
    void setSpan(int value);
    void setProfilingRes(int value);
    void setRelativeDist(int value);

    void addGeneGroup(const QString &file);
    void removeGeneGroups();

    void addGeneGroupName(const QString &name);
    void removeGeneGroupsNames();

    void setName2(bool value);
    void setDump(bool value);

    QStringList getArgumentList() const;
    const QString & getImageFilePath() const;
    const QString & getImageFileFormat() const;
    const QString & getAnnsFilePath() const;

private:
    QString bedFile;            // --bed=
    QString wigFile;            // --wig=
    QString gdbFile;            // --gt=
    QString extraBedFile;       // --ebed=""
    QString experimentName;     // --name=""
    QList<int> sizes;           // --sizes=1000,2000,3000
    QList<int> bisizes;         // --bisizes=2500,5000
    bool bg;                    // --bg
    int span;                   // --span=3000
    int profilingRes;           // --pf-res=50
    int relativeDist;           // --rel-dist=3000
    QStringList geneGroups;     // --gn-groups=""
    QStringList geneNames;      // --gn-group-names=""
    bool name2;                 // --gname2
    bool dump;                  // --dump

    QString imageFile;
    QString imageFormat;
    QString annsFile;

private:
    QString getCorrectArgValue(const QString &value) const;
    void initializeDefaults();
};

} // U2

#endif // _U2_CEAS_SETTINGS_
