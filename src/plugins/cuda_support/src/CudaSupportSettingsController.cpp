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

#include <QLabel>
#include <QLayout>
#include <QCheckBox>

#include <U2Algorithm/CudaGpuRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>

#include "CudaSupportSettingsController.h"

namespace U2 {

CudaSupportSettingsPageController::CudaSupportSettingsPageController( const QString & _displayMsg, QObject * p /* = 0 */ ) :
AppSettingsGUIPageController( tr("CUDA"), CudaSupportSettingsPageId, p ), displayMsg(_displayMsg) {}

const QString CudaSupportSettingsPageController::helpPageId = QString("444");

AppSettingsGUIPageState * CudaSupportSettingsPageController::getSavedState() {
    QList<CudaGpuModel *> registeredGpus = AppContext::getCudaGpuRegistry()->getRegisteredGpus();
    CudaSupportSettingsPageState * s = new CudaSupportSettingsPageState( registeredGpus.size() );
    for( int i = 0, end = s->enabledGpus.size(); i < end; ++i ) {
        s->enabledGpus[i] = registeredGpus.at(i)->isEnabled();
    }

    return s;
}

void CudaSupportSettingsPageController::saveState( AppSettingsGUIPageState * _s ) {
    QList<CudaGpuModel *> registeredGpus = AppContext::getCudaGpuRegistry()->getRegisteredGpus();
    CudaSupportSettingsPageState * s = qobject_cast<CudaSupportSettingsPageState *>(_s);

    //saving state of enabled/disabled GPUs into registry
    for( int i = 0, end = s->enabledGpus.size(); i < end; ++i ) {
        registeredGpus[i]->setEnabled( s->enabledGpus[i] );
    }

    //increasing/decreasing maxuse of according resource
    int totalEnabled = s->enabledGpus.count(true);
    AppResourceSemaphore* gpuResource = dynamic_cast<AppResourceSemaphore*>(AppResourcePool::instance()->getResource( RESOURCE_CUDA_GPU ));
    if( gpuResource ) {
        gpuResource->setMaxUse(totalEnabled);
    } //else - resource was not registered, nothing to do.
}

AppSettingsGUIPageWidget * CudaSupportSettingsPageController::createWidget( AppSettingsGUIPageState* state ) {
    CudaSupportSettingsPageWidget * w = new CudaSupportSettingsPageWidget(displayMsg, this);
    w->setState(state);
    return w;
}

CudaSupportSettingsPageState::CudaSupportSettingsPageState( int num_gpus ) {
    assert( num_gpus >= 0 );
    enabledGpus.resize( num_gpus );
}

const static char * gpusDiscoveredText =
    "The following CUDA-enabled GPUs are detected.<br>\
    Check the GPUs to use for accelerating algorithms computations.";

const static char * noGpusDiscoveredText = "No CUDA-enabled GPU detected.";

CudaSupportSettingsPageWidget::CudaSupportSettingsPageWidget( const QString & _msg, CudaSupportSettingsPageController * /*ctrl*/ ) :
onlyMsg(_msg){

    if( !onlyMsg.isEmpty() ) {
        //just display the centered warning message
        QHBoxLayout* hLayout = new QHBoxLayout(this);
        QLabel * msgLabel = new QLabel( onlyMsg, this );
        msgLabel->setAlignment( Qt::AlignLeft );

        hLayout->setAlignment( Qt::AlignTop | Qt::AlignLeft);
        hLayout->addWidget(msgLabel);
        hLayout->addStretch();
        setLayout(hLayout);
    } else {
        //everything is OK - adding info about all available GPUs

        QVBoxLayout * vLayout = new QVBoxLayout(this);

        QList<CudaGpuModel *> gpus = AppContext::getCudaGpuRegistry()->getRegisteredGpus();
        const QString & actualText = gpus.empty() ? tr(noGpusDiscoveredText) : tr(gpusDiscoveredText);
        QLabel * gpusDiscoveredLabel = new QLabel( actualText, this );
        vLayout->addWidget( gpusDiscoveredLabel );

        foreach( CudaGpuModel * m, gpus ) {
            vLayout->setAlignment( Qt::AlignLeft | Qt::AlignTop );
            QHBoxLayout * hLayout = new QHBoxLayout(this);

            QString gpuText = m->getName() + " " + QString::number(m->getGlobalMemorySizeBytes() / (1024*1024)) + " Mb";
            QCheckBox * check = new QCheckBox( gpuText, this );

            check->setChecked(true);

            gpuEnableChecks.push_back(check);
            hLayout->addWidget(check);
            vLayout->addLayout( hLayout );
        }
        setLayout(vLayout);
    }
}

void CudaSupportSettingsPageWidget::setState( AppSettingsGUIPageState * _state ) {
    CudaSupportSettingsPageState * state = qobject_cast<CudaSupportSettingsPageState*>(_state);
    assert( state->enabledGpus.size() == gpuEnableChecks.size() );

    for( int i = 0, end = state->enabledGpus.size(); i < end; ++i ) {
        gpuEnableChecks.at(i)->setChecked( state->enabledGpus.at(i) );
    }
}

AppSettingsGUIPageState* CudaSupportSettingsPageWidget::getState( QString & /*err*/ ) const {
    CudaSupportSettingsPageState * state = new CudaSupportSettingsPageState( gpuEnableChecks.size() );

    assert( state->enabledGpus.size() == gpuEnableChecks.size() );
    for( int i = 0, end = state->enabledGpus.size(); i < end; ++i ) {
        state->enabledGpus[i] = gpuEnableChecks.at(i)->isChecked();
    }
    return state;
}

} //namespace

