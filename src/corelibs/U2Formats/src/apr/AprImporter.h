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

#ifndef _U2_APR_IMPORTER_H_
#define _U2_APR_IMPORTER_H_

#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentProviderTask.h>

namespace U2 {

class LoadDocumentTask;


///////////////////////////////////
//// AprImporterTask
///////////////////////////////////

class U2FORMATS_EXPORT AprImporterTask : public DocumentProviderTask {
    Q_OBJECT
public:
    AprImporterTask(const GUrl& url, const QVariantMap &_settings);

    virtual void prepare();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
    QVariantMap              settings;
    GUrl                     srcUrl;
};

///////////////////////////////////
//// AprImporter
///////////////////////////////////

class U2FORMATS_EXPORT AprImporter : public DocumentImporter {
    Q_OBJECT
public:
    AprImporter();

    virtual FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& url);
    virtual DocumentProviderTask* createImportTask(const FormatDetectionResult& res, bool showGui, const QVariantMap &hints);
    virtual QString getRadioButtonText() const;

    static const QString ID;
    static const QString SRC_URL;
};

}   // namespace U2

#endif // _U2_APR_IMPORTER_H_
