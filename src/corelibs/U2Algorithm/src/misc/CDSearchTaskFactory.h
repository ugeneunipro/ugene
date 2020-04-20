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

#ifndef _U2_CDS_TASK_FACTORY_H_
#define _U2_CDS_TASK_FACTORY_H_

#include <U2Core/AnnotationData.h>


namespace U2 {

class U2ALGORITHM_EXPORT CDDNames {
public:
    static QString CDD_DB() { return "CDD"; }
    static QString PFAM_DB() { return "Pfam"; }
    static QString SMART_DB() { return "Smart"; }
    static QString COG_DB() { return "Cog"; }
    static QString KOG_DB() { return "Kog"; }
    static QString PRK_DB() { return "Prk"; }
    static QString TIGR_DB() { return "Tigr"; }
};

class DNAAlphabet;

class U2ALGORITHM_EXPORT CDSearchSettings {
public:
    CDSearchSettings() : ev(0.01f), alp(NULL) {}
    float ev;
    const DNAAlphabet* alp;
    QByteArray query;
    QString localDbFolder;
    QString dbName;
};

class Task;

class U2ALGORITHM_EXPORT CDSearchResultListener {
public:
    virtual ~CDSearchResultListener() {}
    virtual QList<SharedAnnotationData> getCDSResults() const = 0;
    virtual Task* getTask() const = 0;
};

class U2ALGORITHM_EXPORT CDSearchFactory {
public:
    virtual ~CDSearchFactory() {}

    virtual CDSearchResultListener* createCDSearch(const CDSearchSettings& settings) const = 0;
};

} //namespace

#endif
