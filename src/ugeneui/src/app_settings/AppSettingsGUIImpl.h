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

#ifndef _U2_APPSETTINGS_GUI_IMPL_H_
#define _U2_APPSETTINGS_GUI_IMPL_H_

#include <U2Gui/AppSettingsGUI.h>

namespace U2 {

class AppSettingsGUIImpl : public AppSettingsGUI {
    Q_OBJECT
public:
    AppSettingsGUIImpl(QObject* p = NULL);

    ~AppSettingsGUIImpl();
    
    virtual bool registerPage(AppSettingsGUIPageController* page, const QString& beforePage = QString());

    virtual bool unregisterPage(AppSettingsGUIPageController* page);

    virtual void showSettingsDialog(const QString& pageId = QString()) const;

    AppSettingsGUIPageController* findPageById(const QString& pageId) const;
    
    virtual QList<AppSettingsGUIPageController*> getRegisteredPages() const  {return pages;}

private slots:
    void sl_showSettingsDialog() {showSettingsDialog();}

private:
    void registerBuiltinPages();

    QList<AppSettingsGUIPageController*>      pages;

};

}//namespace

#endif
