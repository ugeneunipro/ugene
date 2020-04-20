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

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "Primer3Dialog.h"
#include "Primer3Plugin.h"
#include "Primer3Query.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    Primer3Plugin * plug = new Primer3Plugin();
    return plug;
}

Primer3Plugin::Primer3Plugin() : Plugin(tr("Primer3"), tr("Integrated tool for PCR primers design.")), viewCtx(NULL)
{
    if (AppContext::getMainWindow()) {
        viewCtx = new Primer3ADVContext(this);
        viewCtx->init();
    }

    QDActorPrototypeRegistry* qdpr = AppContext::getQDActorProtoRegistry();
    qdpr->registerProto(new QDPrimerActorPrototype());

    //////////////////////////////////////////////////////////////////////////
    //tests
    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    assert(xmlTestFormat!=NULL);

    GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = Primer3Tests::createTestFactories();


    foreach(XMLTestFactory* f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        Q_UNUSED(res);
        assert(res);
    }
}

Primer3Plugin::~Primer3Plugin() {
}

Primer3ADVContext::Primer3ADVContext(QObject* p) : GObjectViewWindowContext(p, ANNOTATED_DNA_VIEW_FACTORY_ID)
{

}

void Primer3ADVContext::initViewContext(GObjectView* v) {
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(v);
    ADVGlobalAction* a = new ADVGlobalAction(av, QIcon(":/primer3/images/primer3.png"), tr("Primer3..."), 95);
    a->setObjectName("primer3_action");
    a->addAlphabetFilter(DNAAlphabet_NUCL);
    connect(a, SIGNAL(triggered()), SLOT(sl_showDialog()));
}

void Primer3ADVContext::sl_showDialog() {
    QAction* a = (QAction*)sender();
    GObjectViewAction* viewAction = qobject_cast<GObjectViewAction*>(a);
    AnnotatedDNAView* av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    assert(av);

    ADVSequenceObjectContext* seqCtx = av->getSequenceInFocus();
    assert(seqCtx->getAlphabet()->isNucleic());
    {
        Primer3TaskSettings defaultSettings;
        {
            QList< U2Region > sizeRange;
            sizeRange.append(U2Region(150, 101));  // 150-250
            sizeRange.append(U2Region(100, 201));  // 100-300
            sizeRange.append(U2Region(301, 100));  // 301-400
            sizeRange.append(U2Region(401, 100));  // 401-500
            sizeRange.append(U2Region(501, 100));  // 501-600
            sizeRange.append(U2Region(601, 100));  // 601-700
            sizeRange.append(U2Region(701, 150));  // 701-850
            sizeRange.append(U2Region(851, 150));  // 851-1000
            defaultSettings.setProductSizeRange(sizeRange);
        }
        defaultSettings.setDoubleProperty("PRIMER_MAX_END_STABILITY",9.0);
        defaultSettings.setAlignProperty("PRIMER_MAX_TEMPLATE_MISPRIMING",1200);
        defaultSettings.setAlignProperty("PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING",2400);
        defaultSettings.setIntProperty("PRIMER_LIBERAL_BASE",1);
        defaultSettings.setDoubleProperty("PRIMER_WT_POS_PENALTY",0.0);
        defaultSettings.setIntProperty("PRIMER_FIRST_BASE_INDEX",1);

        QObjectScopedPointer<Primer3Dialog> dialog = new Primer3Dialog(defaultSettings, seqCtx);
        dialog->exec();
        CHECK(!dialog.isNull(), );

        if(QDialog::Accepted == dialog->result())
        {
            Primer3TaskSettings settings = dialog->getSettings();
            U2OpStatusImpl os;
            QByteArray seqData = seqCtx->getSequenceObject()->getWholeSequenceData(os);
            CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );
            settings.setSequence(seqData,
                                 seqCtx->getSequenceObject()->isCircular());
            QString err = dialog->checkModel();
            if (!err.isEmpty()) {
                QMessageBox::warning(QApplication::activeWindow(), dialog->windowTitle(), err);
                return;
            }
            bool objectPrepared = dialog->prepareAnnotationObject();
            if (!objectPrepared){
                QMessageBox::warning(QApplication::activeWindow(), tr("Error"), tr("Cannot create an annotation object. Please check settings"));
                return;
            }
            const CreateAnnotationModel &model = dialog->getCreateAnnotationModel();
            AppContext::getTaskScheduler()->registerTopLevelTask(new Primer3ToAnnotationsTask(settings, seqCtx->getSequenceObject(),
                model.getAnnotationObject(), model.groupName, model.data->name, model.description));
        }
    }
}

QList<XMLTestFactory*> Primer3Tests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_Primer3::createFactory());
    return res;
}

}//namespace
