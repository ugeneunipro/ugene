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

#include <QComboBox>
#include <QPushButton>

#include <U2Gui/GUIUtils.h>

#include "HelpButton.h"

namespace U2{

const QString HelpButton::INVALID_VALUE = "invalid";

HelpButton::HelpButton(QObject *parent, QDialogButtonBox *b, const QString& _pageId) 
: QObject(parent), pageId(_pageId), dialogBox(b)
{
    helpButton = new QPushButton(tr("Help"));
    connect(helpButton, SIGNAL(clicked()), SLOT(sl_buttonClicked()));
    dialogBox->addButton(helpButton, QDialogButtonBox::HelpRole);
}

HelpButton::HelpButton(QObject *parent, QAbstractButton *hb, const QString& _pageId) 
: QObject(parent), pageId(_pageId), helpButton(NULL), dialogBox(NULL)
{
    connect(hb, SIGNAL(clicked()), SLOT(sl_buttonClicked()));
}

void HelpButton::sl_buttonClicked(){
    GUIUtils::runWebBrowser("http://ugene.net/wiki/pages/viewpage.action?pageId="+pageId+"&from=ugene");
}

void HelpButton::updatePageId( const QString &newPageId ){
    pageId = newPageId;
}

ComboboxDependentHelpButton::ComboboxDependentHelpButton( QObject *parent, QDialogButtonBox *b, QComboBox *_cb, const QMap<QString, QString> &_pageMap )
: HelpButton(parent, b, "")
, pageMap(_pageMap)
, cb(_cb){}

void ComboboxDependentHelpButton::sl_buttonClicked(){
    QString pageId = pageMap[cb->currentText()];
    GUIUtils::runWebBrowser("http://ugene.net/wiki/pages/viewpage.action?pageId="+pageId+"&from=ugene");
}

}
