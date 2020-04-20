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

#ifndef _U2_BASE_NGS_WORKER_H_
#define _U2_BASE_NGS_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Core/GUrl.h>

namespace U2 {
namespace LocalWorkflow {

class U2LANG_EXPORT BaseNGSSetting{
public:
    BaseNGSSetting(): outDir(""), outName(""),inputUrl(""){}

    QString outDir;
    QString outName;
    QString inputUrl;
    QVariantMap customParameters;
    QList<ExternalToolListener*> listeners;
};

class U2LANG_EXPORT BaseNGSWorker: public BaseWorker {
    Q_OBJECT
public:
    BaseNGSWorker(Actor *a);
    void init();
    Task * tick();
    void cleanup();

    static const QString INPUT_PORT;
    static const QString OUTPUT_PORT;
    static const QString OUT_MODE_ID;
    static const QString CUSTOM_DIR_ID;
    static const QString OUT_NAME_ID;
    static const QString DEFAULT_NAME;

protected:
    virtual QVariantMap getCustomParameters() const {return QVariantMap();}
    virtual QString getDefaultFileName() const = 0;
    virtual Task* getTask(const BaseNGSSetting& settings) const = 0;

    QString takeUrl();
    QString getTargetName(const QString& fileUrl, const QString& outDir);
    void sendResult(const QString &url);

protected:
    IntegralBus *inputUrlPort;
    IntegralBus *outputUrlPort;
    QStringList outUrls;

public slots:
    void sl_taskFinished( Task *task );


}; //BaseNGSWorker


class U2LANG_EXPORT BaseNGSParser : public ExternalToolLogParser {
public:
    BaseNGSParser();

    void parseOutput(const QString& partOfLog);
    void parseErrOutput(const QString& partOfLog);

private:
    QString lastErrLine;
};

class U2LANG_EXPORT BaseNGSTask : public Task {
    Q_OBJECT
public:
    BaseNGSTask(const BaseNGSSetting &settings);

    void prepare();
    void run();

    QString getResult(){return resultUrl;}

protected:
    virtual void prepareStep() {};
    virtual void runStep() {};
    virtual void finishStep() {};
    /**
     * Don't delete customParser, it will be deleted automatically.
     */
    virtual ExternalToolRunTask* getExternalToolTask (const QString& toolId, ExternalToolLogParser *customParser = NULL);
    virtual QStringList getParameters(U2OpStatus& os) = 0;

protected:
    BaseNGSSetting settings;
    QString resultUrl;
};


} //LocalWorkflow
} //U2

#endif //_U2_BASE_NGS_WORKER_H_
