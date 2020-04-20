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

#ifndef _U2_SEQPOS_SUPPORT_WORKER_
#define _U2_SEQPOS_SUPPORT_WORKER_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include <U2Designer/DelegateEditors.h>

#include "SeqPosTask.h"
#include "SeqPosSettings.h"
#include "utils/CistromeDelegate.h"
#include <QLabel>

namespace U2 {
namespace LocalWorkflow {

class SeqPosWorker : public BaseWorker {
    Q_OBJECT
public:
    SeqPosWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();

private:
    IntegralBus *inChannel;

private:
    SeqPosSettings createSeqPosSettings(U2OpStatus &os);

private slots:
    void sl_taskFinished();
}; // SeqPosWorker

class SeqPosComboBoxWithChecksDelegate : public ComboBoxWithChecksDelegate {
    Q_OBJECT
public:
    SeqPosComboBoxWithChecksDelegate(const QVariantMap& items, QObject *parent = 0) : ComboBoxWithChecksDelegate(items, parent) {}
    virtual ~SeqPosComboBoxWithChecksDelegate() {}

    virtual PropertyWidget* createWizardWidget(U2OpStatus &os, QWidget *parent) const;
    virtual PropertyDelegate *clone() { return new SeqPosComboBoxWithChecksDelegate(items, parent()); }
};

class SeqPosComboBoxWithChecksWidget : public ComboBoxWithChecksWidget {
    Q_OBJECT
public:
    SeqPosComboBoxWithChecksWidget( const QVariantMap &items, QWidget *parent = NULL);

    void setValue(const QVariant &value);
    void setHint(const QString &hint) { hintLabel->setText(hint); }

protected slots:
    void sl_valueChanged(int index);

private:
    void checkHint();

private:
    QLabel* hintLabel;
};

class SeqPosComboBoxWithUrlsDelegate : public CistromeComboBoxWithUrlsDelegate {
public:
    SeqPosComboBoxWithUrlsDelegate(const QVariantMap& items, bool _isPath = false, QObject *parent = 0) : CistromeComboBoxWithUrlsDelegate(items, _isPath, parent) {}

private:
    virtual void updateUgeneSettings();

    virtual QString getDataPathName();
    virtual QString getAttributeName();
    virtual QString getDefaultValue() { return "hg19"; }
}; // SeqPosComboBoxWithUrlsDelegate

class SeqPosWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    SeqPosWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);
}; // SeqPosWorkerFactory

class SeqPosPrompter : public PrompterBase<SeqPosPrompter> {
    Q_OBJECT
public:
    SeqPosPrompter(Actor *p = NULL) : PrompterBase<SeqPosPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // SeqPosPrompter

} // LocalWorkflow
} // U2

#endif // _U2_SEQPOS_SUPPORT_WORKER_
