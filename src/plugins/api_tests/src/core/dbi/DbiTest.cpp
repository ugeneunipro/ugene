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

#include "DbiTest.h"

#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/TestRunnerSettings.h>

#include <QDir>


namespace U2 {

TestDbiProvider::TestDbiProvider() :
    useConnectionPool(false)
{
    initialized = false;
    dbi = NULL;
}
TestDbiProvider::~TestDbiProvider(){
    close();
}
bool TestDbiProvider::init(const QString& dbiFileName, bool _useConnectionPool){
    if(initialized){
        close();
        initialized = false;
    }

    TestRunnerSettings* trs = AppContext::getAppSettings()->getTestRunnerSettings();
    QString originalFile = trs->getVar("COMMON_DATA_DIR") + "/" + dbiFileName;

    QString tmpFile = QDir::temp().absoluteFilePath(QFileInfo(originalFile).fileName());

    if(QFile::exists(tmpFile)) {
        QFile::remove(tmpFile);
    }

    bool _create = false;
    if (QFile::exists(originalFile)) {
        SAFE_POINT(QFile::copy(originalFile, tmpFile), "db file not copied", false);
    }else{
        _create = true;
    }

    dbUrl = tmpFile;
    useConnectionPool = _useConnectionPool;

    U2DbiFactory *factory = AppContext::getDbiRegistry()->getDbiFactoryById(SQLITE_DBI_ID);
    SAFE_POINT(factory!=NULL, "No dbi factory", false);
    U2OpStatusImpl opStatus;

    if(useConnectionPool){
        U2DbiRef ref;
        ref.dbiFactoryId = factory->getId();
        ref.dbiId = dbUrl;
        dbi = AppContext::getDbiRegistry()->getGlobalDbiPool()->openDbi(ref, _create, opStatus);
        CHECK_OP(opStatus, false);
    }else{
        dbi = factory->createDbi();
        SAFE_POINT(NULL != dbi, "dbi not created", false);
        QHash<QString, QString> properties;
        if(_create){
           properties[U2DbiOptions::U2_DBI_OPTION_CREATE] = U2DbiOptions::U2_DBI_VALUE_ON;
        }
        properties["url"] = dbUrl;
        QVariantMap persistentData;
        dbi->init(properties, persistentData, opStatus);
        SAFE_POINT_OP(opStatus, false);
    }
    U2ObjectDbi* objDbi = dbi->getObjectDbi();
    SAFE_POINT(NULL != objDbi,  "object dbi not loaded", false);

    initialized = true;
    return true;
}
void TestDbiProvider::close(){
    U2OpStatusImpl opStatus;
    if(dbi){
        if(useConnectionPool){
            U2DbiRegistry* dbiReg = AppContext::getDbiRegistry();
            if (NULL != dbiReg) {
                dbiReg->getGlobalDbiPool()->releaseDbi(dbi, opStatus);
            }
        }else{
            dbi->shutdown(opStatus);
            SAFE_POINT_OP(opStatus, );
            delete dbi;
        }
    }
    dbi = NULL;
    initialized = false;
}
U2Dbi* TestDbiProvider::getDbi(){
    SAFE_POINT(initialized, "Dbi Provider is not initialized", NULL);
    return dbi;
}

} //namespace
