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

#ifndef _U2_CONVERTFILETASK_H_
#define _U2_CONVERTFILETASK_H_

#include <U2Core/GUrl.h>
#include <U2Core/Task.h>

namespace U2 {

class LoadDocumentTask;
class SaveDocumentTask;

//
class U2FORMATS_EXPORT ConvertFileTask : public Task {
    Q_OBJECT
public:
    ConvertFileTask(const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &dir);

    GUrl getSourceURL() const;
    QString getResult() const;

    void run();
protected:
    GUrl sourceURL;
    QString detectedFormat;
    QString targetFormat;
    QString workingDir;
    QString targetUrl;
};// ConvertFileTask


//use this task for default conversions
class U2FORMATS_EXPORT DefaultConvertFileTask : public ConvertFileTask  {
    Q_OBJECT
public:
    DefaultConvertFileTask(const GUrl &sourceUrl, const QString &detectedFormat, const QString &targetFormat, const QString &dir);
    DefaultConvertFileTask(const GUrl &sourceUrl, const QString &detectedFormat, const QString &targetUrl, const QString &targetFormat, const QString &dir);

private:
    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);

    LoadDocumentTask *loadTask;
    SaveDocumentTask *saveTask;

}; //DefaultConvertFileTask

//SAM->BAM creates a sorted and indexed BAM
class U2FORMATS_EXPORT BamSamConversionTask : public ConvertFileTask {
public:
    BamSamConversionTask(const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &dir);

protected:
    void prepare();
    void run();

private:
    bool samToBam;
}; //BamSamConversionTask

//////////////////////////////////////////////////////////////////////////
//Factories and registries
class U2FORMATS_EXPORT ConvertFileFactory: public QObject{
public:
    //return true if it is a custom conversion for given formats
    virtual bool isCustomFormatTask (const QString& detectedFormat, const QString& targetFormat);
    virtual ConvertFileTask* getTask(const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &dir) {return new DefaultConvertFileTask(sourceURL, detectedFormat, targetFormat, dir);}
};

class U2FORMATS_EXPORT BAMConvertFactory : public ConvertFileFactory{
public:
    virtual bool isCustomFormatTask (const QString& detectedFormat, const QString& targetFormat);
    virtual ConvertFileTask* getTask(const GUrl &sourceURL, const QString &detectedFormat, const QString &targetFormat, const QString &dir) {return new BamSamConversionTask(sourceURL, detectedFormat, targetFormat, dir);}
};

class U2FORMATS_EXPORT ConvertFactoryRegistry : public QObject{
public:
    ConvertFactoryRegistry(QObject *o = 0);
    ~ConvertFactoryRegistry();
    bool registerConvertFactory(ConvertFileFactory* f);
    void unregisterConvertFactory(ConvertFileFactory* f);
    ConvertFileFactory *getFactoryByFormats(const QString& detectedFormat, const QString& targetFormat);
private:
    QList<ConvertFileFactory*> factories;
};


} // U2

#endif // _U2_CONVERTFILETASK_H_
