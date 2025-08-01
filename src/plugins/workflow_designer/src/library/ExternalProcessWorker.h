/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Core/Task.h>

#include <U2Designer/PrompterBase.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowEnv.h>

namespace U2 {
namespace LocalWorkflow {

class ExternalProcessWorker : public BaseWorker {
    Q_OBJECT
public:
    ExternalProcessWorker(Actor* a);

    bool isReady() const override;
    Task* tick() override;
    void init() override;
    void cleanup() override;

private slots:
    void sl_onTaskFinishied();

private:
    enum InputsCheckResult {
        ALL_INPUTS_FINISH,
        SOME_INPUTS_FINISH,
        ALL_INPUTS_HAVE_MESSAGE,
        NOT_ALL_INPUTS_HAVE_MESSAGE,
        INTERNAL_ERROR
    };

    void applySpecialInternalEnvvars(QString& execString, ExternalProcessConfig* cfg);
    void applyAttributes(QString& execString);
    static bool applyParamsToExecString(QString& execString, QString parName, QString parValue);
    void applyEscapedSymbols(QString& execString);
    QStringList applyInputMessage(QString& execString, const DataConfig& dataCfg, const QVariantMap& data, U2OpStatus& os);
    QString prepareOutput(QString& execString, const DataConfig& dataCfg, U2OpStatus& os);

    InputsCheckResult checkInputBusState() const;
    bool finishWorkIfInputEnded(QString& error);
    void finish();

    IntegralBus* output;
    QList<IntegralBus*> inputs;
    QString commandLine;
    ExternalProcessConfig* cfg;

    QMap<QString, bool> urlsForDashboard;  // url -> open by system
    QStringList inputUrls;
};

class ExternalProcessWorkerFactory : public DomainFactory {
public:
    ExternalProcessWorkerFactory(QString name)
        : DomainFactory(name) {
    }
    static bool init(ExternalProcessConfig* cfg);
    Worker* createWorker(Actor* a) override {
        return new ExternalProcessWorker(a);
    }
};

class ExternalProcessWorkerPrompter : public PrompterBase<ExternalProcessWorkerPrompter> {
    Q_OBJECT
public:
    ExternalProcessWorkerPrompter(Actor* p = nullptr)
        : PrompterBase<ExternalProcessWorkerPrompter>(p) {
    }
    QString composeRichDoc() override;
};

class LaunchExternalToolTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(LaunchExternalToolTask)
public:
    LaunchExternalToolTask(const QString& execString, const QString& workingDir, const QMap<QString, DataConfig>& outputUrls);
    ~LaunchExternalToolTask();

    void run() override;

    QMap<QString, DataConfig> takeOutputUrls();
    void addListeners(const QList<ExternalToolListener*>& listenersToAdd);

private:
    QMap<QString, DataConfig> outputUrls;
    QString execString;
    QString workingDir;
    QList<ExternalToolListener*> listeners;
};

}  // namespace LocalWorkflow
}  // namespace U2
