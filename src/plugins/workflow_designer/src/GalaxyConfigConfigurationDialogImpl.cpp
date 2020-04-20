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

#include <U2Core/AppContext.h>

#include <U2Gui/U2FileDialog.h>

#include "GalaxyConfigConfigurationDialogImpl.h"
#include "cmdline/GalaxyConfigTask.h"

namespace U2 {
namespace Workflow {

GalaxyConfigConfigurationDialogImpl::GalaxyConfigConfigurationDialogImpl( const QString &_schemePath , QWidget * p ) : QDialog(p) {
    setupUi(this);

    ugenePathLineEdit->setText( QApplication::applicationDirPath() );
    galaxyPathLineEdit->setText( "Set Galaxy folder" );
    destinationPathLineEdit->setText( "Set destination folder" );

    connect( cancelPushButton, SIGNAL(clicked()), SLOT( reject() ) );
    connect( createPushButton, SIGNAL(clicked()), SLOT( accept() ) );

    connect( ugenePathToolButton,       SIGNAL(clicked()), SLOT( sl_ugeneToolButtonClicked() ) );
    connect( galaxyPathToolButton,      SIGNAL(clicked()), SLOT( sl_galaxyToolButtonClicked() ) );
    connect( destinationPathToolButton, SIGNAL(clicked()), SLOT( sl_destinationToolButtonClicked() ) );

    schemePath = _schemePath;

}

void GalaxyConfigConfigurationDialogImpl::sl_ugeneToolButtonClicked( ) {
    QString newDirectoryPath = U2FileDialog::getExistingDirectory(this, tr( "Set UGENE folder" ), ugenePathLineEdit->text() );
    if( !newDirectoryPath.isEmpty() ) {
        ugenePathLineEdit->setText( newDirectoryPath );
    }
}

void GalaxyConfigConfigurationDialogImpl::sl_galaxyToolButtonClicked( ) {
    QString newDirectoryPath = U2FileDialog::getExistingDirectory(this, tr( "Set Galaxy folder" ), galaxyPathLineEdit->text() );
    if( !newDirectoryPath.isEmpty() ) {
        galaxyPathLineEdit->setText( newDirectoryPath );
    }
}

void GalaxyConfigConfigurationDialogImpl::sl_destinationToolButtonClicked( ) {
    QString newDirectoryPath = U2FileDialog::getExistingDirectory(this, tr( "Set destination folder" ), destinationPathLineEdit->text() );
    if( !newDirectoryPath.isEmpty() ) {
        destinationPathLineEdit->setText( newDirectoryPath );
    }
}

bool GalaxyConfigConfigurationDialogImpl::createGalaxyConfigTask() {
    Task *task = new GalaxyConfigTask( schemePath, ugenePathLineEdit->text(), galaxyPathLineEdit->text(), destinationPathLineEdit->text() );
    if( task != NULL ) {
        AppContext::getTaskScheduler()->registerTopLevelTask( task );
        return true;
    }
    return false;
}

} //Workflow
} //U2
