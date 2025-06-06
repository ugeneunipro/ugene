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

#include <U2Lang/BaseOneOneWorker.h>

namespace U2 {
namespace LocalWorkflow {

class U2LANG_EXPORT BaseDatasetWorker : public BaseOneOneWorker {
    Q_OBJECT
public:
    BaseDatasetWorker(Actor* a, const QString& inPortId, const QString& outPortId);

    void init() override;
    void cleanup() override;

protected:
    // BaseOneOneWorker
    Task* processNextInputMessage() override;
    Task* onInputEnded() override;
    QList<Message> fetchResult(Task* task, U2OpStatus& os) override;

    virtual Task* createTask(const QList<Message>& messages) const = 0;
    virtual QVariantMap getResult(Task* task, U2OpStatus& os) const = 0;
    virtual MessageMetadata generateMetadata(const QString& datasetName) const;

private:
    QString getDatasetName(const Message& message) const;
    bool datasetChanged(const Message& message) const;
    void takeMessage();
    Task* onDatasetChanged();

private:
    bool datasetInited;
    QString datasetName;
    QList<Message> datasetMessages;
};

}  // namespace LocalWorkflow
}  // namespace U2
