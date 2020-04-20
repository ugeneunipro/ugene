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

#ifndef _U2_PROJECT_UTILS__H_
#define _U2_PROJECT_UTILS__H_

#include <U2Core/ProjectModel.h>
#include <U2Core/GAutoDeleteList.h>

class QDomDocument;

typedef QMap<QString,QString> StrMap;

namespace U2 {

    //TODO eliminate dependency on object views and move to private??

class U2GUI_EXPORT ProjectParser : public QObject {
    Q_OBJECT
public:
    virtual Project* createProjectFromXMLModel(const QString& url, const QDomDocument& xmlDoc, U2OpStatus& si) = 0;
    virtual QString getVersion(){assert(false); return QString();};
};

class U2GUI_EXPORT ProjectParserRegistry {
public:
    ProjectParser* getProjectParserByVersion(const QString& id);
    static ProjectParserRegistry* instance();

private:
    ProjectParserRegistry(ProjectParserRegistry &ppr){Q_UNUSED(ppr); assert(0);};
    ProjectParserRegistry(){init();};
    void init();
    GAutoDeleteList<ProjectParser> parsers;
};


class ProjectParser10 : public ProjectParser {
    Q_OBJECT
public:
    ProjectParser10(){};
    Project* createProjectFromXMLModel(const QString& url, const QDomDocument& xmlDoc, U2OpStatus& si);
    QString getVersion(){return QString("1.0");};
};

class U2GUI_EXPORT ProjectFileUtils : public QObject {
    Q_OBJECT

public:

    static void saveProjectFile(U2OpStatus& ts, Project* p, const QString& projectUrl,   const StrMap& docUrlRemap = StrMap());

    static void loadXMLProjectModel(const QString& url, U2OpStatus & si, QDomDocument& doc, QString& version);
};



} //namespace

#endif
