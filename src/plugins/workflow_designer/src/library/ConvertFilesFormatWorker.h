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

#include <U2Core/GUrl.h>

#include <U2Designer/PrompterBase.h>

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class ConvertFilesFormatPrompter;
typedef PrompterBase<ConvertFilesFormatPrompter> ConvertFilesFormatBase;

class ConvertFilesFormatPrompter : public ConvertFilesFormatBase {
    Q_OBJECT
public:
    ConvertFilesFormatPrompter(Actor* p = 0)
        : ConvertFilesFormatBase(p) {
    }

protected:
    QString composeRichDoc() override;
};  // ConvertFilesFormatPrompter

class ConvertFilesFormatWorker : public BaseWorker {
    Q_OBJECT
public:
    ConvertFilesFormatWorker(Actor* a);
    void init() override;
    Task* tick() override;
    void cleanup() override;

private:
    IntegralBus* inputUrlPort;
    IntegralBus* outputUrlPort;
    QString targetFormat;
    QStringList selectedFormatExtensions;
    QStringList excludedFormats;

public slots:
    void sl_taskFinished(Task* task);

private:
    bool ensureFileExists(const QString& url);
    QString takeUrl();
    QString detectFormat(const QString& url);
    QString createWorkingDir(const QString& fileUrl);
    void sendResult(const QString& url);
    Task* getConvertTask(const QString& detectedFormat, const QString& url);
};  // ConvertFilesFormatWorker

class ConvertFilesFormatWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;

public:
    static void init();
    ConvertFilesFormatWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) override {
        return new ConvertFilesFormatWorker(a);
    }
};  // ConvertFilesFormatWorkerFactory

}  // namespace LocalWorkflow
}  // namespace U2
