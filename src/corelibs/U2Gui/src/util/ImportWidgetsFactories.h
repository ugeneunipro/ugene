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

#ifndef _U2_IMPORT_WIDGETS_H_
#define _U2_IMPORT_WIDGETS_H_

#include <U2Core/DocumentImport.h>

namespace U2 {

/** If you need a dialog on file import, specify it here,
 *  crete its factory and add the factory to the importer.
 *  Note that importer should exec the dialog itself.
 **/

class AceImportWidget;
class AprImportWidget;
class ImportWidget;

class U2GUI_EXPORT ImportWidgetsFactories {
public:
    static void registerFactories();
};

class AceImportWidgetFactory : public ImportWidgetFactory {
public:
    virtual ImportWidget* getWidget(const GUrl& url, const QVariantMap& settings) const;
};

class AprImportWidgetFactory : public ImportWidgetFactory {
public:
    virtual ImportWidget* getWidget(const GUrl& url, const QVariantMap& settings) const;
};

}   // namespace U2

#endif // _U2_IMPORT_WIDGETS_H_
