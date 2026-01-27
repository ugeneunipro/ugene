/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/GUrl.h>

#include <U2Designer/PrompterBase.h>

#include <U2Lang/BaseNGSWorker.h>
#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

//////////////////////////////////////////////////
// Primer3ModuleCutadapt
class Primer3ModuleCutadaptPrompter;
typedef PrompterBase<Primer3ModuleCutadaptPrompter> Primer3ModuleCutadaptBase;

class Primer3ModuleCutadaptPrompter : public Primer3ModuleCutadaptBase {
    Q_OBJECT
public:
    Primer3ModuleCutadaptPrompter(Actor* p = 0)
        : Primer3ModuleCutadaptBase(p) {
    }

protected:
    QString composeRichDoc();
};  // Primer3ModuleCutadaptPrompter

class Primer3ModuleCutadaptWorker : public BaseNGSWorker {
    Q_OBJECT
public:
    Primer3ModuleCutadaptWorker(Actor* a);

protected:
    QVariantMap getCustomParameters() const;
    QString getDefaultFileName() const;
    Task* getTask(const BaseNGSSetting& settings) const;

    QStringList inputUrls;

};  // Primer3ModuleCutadaptWorker

class Primer3ModuleCutadaptWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;

public:
    static void init();
    Primer3ModuleCutadaptWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) {
        return new Primer3ModuleCutadaptWorker(a);
    }
};  // Primer3ModuleCutadaptWorkerFactory

class Primer3ModuleCutadaptTask : public BaseNGSTask {
    Q_OBJECT
public:
    Primer3ModuleCutadaptTask(const BaseNGSSetting& settings);

protected:
    void prepareStep();
    QStringList getParameters(U2OpStatus& os);
};

class Primer3ModuleCutadaptParser : public ExternalToolLogParser {
public:
    void parseErrOutput(const QString& partOfLog);

    static QString parseTextForErrors(const QStringList& lastPartOfLog);

private:
    static QStringList initStringsToIgnore();

    QString lastErrLine;

    static const QStringList stringsToIgnore;
};

class Primer3ModuleCutadaptLogProcessor : public ExternalToolLogProcessor {
public:
    Primer3ModuleCutadaptLogProcessor(WorkflowMonitor* monitor, const QString& actor);

    void processLogMessage(const QString& message);

private:
    WorkflowMonitor* monitor;
    const QString actor;
};

}  // namespace LocalWorkflow
}  // namespace U2
