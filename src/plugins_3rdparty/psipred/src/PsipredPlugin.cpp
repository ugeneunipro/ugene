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
#include <QMenu>
#include <QMessageBox>
#include <QMap>
#include <QFile>

#include <U2Core/GAutoDeleteList.h>
#include <U2Gui/GUIUtils.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2Core/AppContext.h>
#include <U2Algorithm/SecStructPredictAlgRegistry.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Test/GTestFrameworkComponents.h>

#include <U2Algorithm/SecStructPredictTask.h>

#include "PsipredPlugin.h"
#include "PsipredAlgTask.h"
#include "sspred_utils.h"

#define PSIPRED_PLUGIN_NAME "PsiPred"

namespace U2 {


extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    PsipredPlugin* plug = new PsipredPlugin();
    return plug;
}


PsipredPlugin::PsipredPlugin()
    : Plugin(tr(PSIPRED_PLUGIN_NAME), tr("PsiPred protein secondary structure prediction"), false)
{
    // Register PsiPred algorithm
    SecStructPredictAlgRegistry* registry = AppContext::getSecStructPredictAlgRegistry();
    SecStructPredictTaskFactory* taskFactory = new PsipredAlgTask::Factory;
    registry->registerAlgorithm(taskFactory, PSIPRED_PLUGIN_NAME);
    
    //Register PsiPred annotation settings
    AnnotationSettingsRegistry* asr =AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = new AnnotationSettings(PSIPRED_ANNOTATION_NAME, true, QColor(102,255, 0), true);
    as->showNameQuals = true;
    as->nameQuals.append(BioStruct3D::SecStructTypeQualifierName);
    asr->changeSettings(QList<AnnotationSettings*>() << as, false);
}

PsipredPlugin::~PsipredPlugin() {

}

}//namespace

