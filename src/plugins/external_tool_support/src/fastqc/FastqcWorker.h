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

class FastQCPrompter;
typedef PrompterBase<FastQCPrompter> FastQCBase;

class FastQCPrompter : public FastQCBase {
    Q_OBJECT
public:
    FastQCPrompter(Actor* p = 0)
        : FastQCBase(p) {
    }

protected:
    QString composeRichDoc();
};  // FastQCPrompter

class FastQCWorker : public BaseWorker {
    Q_OBJECT
public:
    FastQCWorker(Actor* a);

    void init() override;
    Task* tick() override;
    void cleanup() override;

    static const QString BASE_FASTQC_SUBDIR;

    static const QString INPUT_PORT;
    static const QString OUT_MODE_ID;
    static const QString CUSTOM_DIR_ID;
    static const QString OUT_FILE;

    static const QString ADAPTERS;
    static const QString CONTAMINANTS;

private:
    IntegralBus* inputUrlPort;

public slots:
    void sl_taskFinished(Task* task);

private:
    QString getUrlAndSetupScriptValues();
};  // FastQCWorker

class FastQCFactory : public DomainFactory {
    static const QString ACTOR_ID;

public:
    static void init();
    FastQCFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) {
        return new FastQCWorker(a);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2
