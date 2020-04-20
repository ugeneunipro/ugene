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
#include <QFile>
#include <QMap>
#include <QMenu>
#include <QMessageBox>

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
#include <U2Algorithm/SecStructPredictTask.h>

#include "GorIVPlugin.h"
#include "GorIVAlgTask.h"
#include "gor.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    SecStructPredictPlugin* plug = new SecStructPredictPlugin();
    return plug;
}

SecStructPredictPlugin::SecStructPredictPlugin() : Plugin(tr("GORIV"), tr("GORIV protein secondary structure prediction")) {
    // Register GORIV algorithm
    SecStructPredictAlgRegistry* registry = AppContext::getSecStructPredictAlgRegistry();
    SecStructPredictTaskFactory* taskFactory = new GorIVAlgTask::Factory;
    registry->registerAlgorithm(taskFactory, GorIVAlgTask::taskName);

    //Register GORIV annotation settings
    AnnotationSettingsRegistry* asr =AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* as = new AnnotationSettings(GORIV_ANNOTATION_NAME, true, QColor(102,255, 0), true);
    as->showNameQuals = true;
    as->nameQuals.append(BioStruct3D::SecStructTypeQualifierName);
    asr->changeSettings(QList<AnnotationSettings*>() << as, false);
}

SecStructPredictPlugin::~SecStructPredictPlugin() {
}


}//namespace

