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

#ifndef _SAMTOOLS_MPILEUP_WORKER_H_
#define _SAMTOOLS_MPILEUP_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "AssemblySamtoolsMpileup.h"

namespace U2 {

namespace LocalWorkflow {

class CallVariantsPrompter;
typedef PrompterBase<CallVariantsPrompter> CallVariantsPrompterBase;

class CallVariantsPrompter : public CallVariantsPrompterBase {
    Q_OBJECT
public:
    CallVariantsPrompter(Actor* p = 0) : CallVariantsPrompterBase(p) {}
protected:
    QString composeRichDoc();
};

class CallVariantsWorker : public BaseWorker {
    Q_OBJECT
public:
    CallVariantsWorker(Actor* a);

    virtual bool isReady() const;
    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

    private slots:
        void sl_taskFinished();

private:
    IntegralBus *refSeqPort, *assemblyPort, *output;

    CallVariantsTaskSettings settings;
    QList<Message> cache;
    QList<QString> assemblyUrls;
    QString currentDatasetName;
    bool useDatasets;

    enum ReferenceSource {
        FromPort,
        FromFile
    };
    ReferenceSource referenceSource;

private:
    void initDatasetMode();
    void takeAssembly(U2OpStatus &os);
    void takeReference(U2OpStatus &os);
    CallVariantsTaskSettings getSettings();
    void processError(const U2OpStatus& os);
    void checkState(U2OpStatus& os);
    bool hasAssembly() const;
    bool hasReferenceInPort() const;
};

class CallVariantsWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    CallVariantsWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new CallVariantsWorker(a);}
};

class ScientificDoubleWidget : public PropertyWidget {
    Q_OBJECT
public:
    ScientificDoubleWidget(QWidget *parent);

    virtual QVariant value();

public slots:
    virtual void setValue(const QVariant &value);

private:
    QLineEdit *lineEdit;
};

class ScientificDoubleDelegate : public PropertyDelegate {
public:
    ScientificDoubleDelegate(QObject *parent = NULL);

    // PropertyDelegate
    virtual QVariant getDisplayValue(const QVariant &v) const;
    virtual PropertyDelegate * clone();
    virtual PropertyWidget * createWizardWidget(U2OpStatus &os, QWidget *parent) const;

    // QItemDelegate
    virtual QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

}// Workflow namespace
} // namespace

#endif //_SAMTOOLS_MPILEUP_WORKER_H_
