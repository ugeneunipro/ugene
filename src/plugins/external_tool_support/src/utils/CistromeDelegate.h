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

#ifndef _U2_CISTROME_DELEGATE_H_
#define _U2_CISTROME_DELEGATE_H_

#include <U2Designer/DelegateEditors.h>

namespace U2 {
namespace LocalWorkflow {

class CistromeComboBoxWithUrlsDelegate : public ComboBoxWithUrlsDelegate {
public:
    CistromeComboBoxWithUrlsDelegate(const QVariantMap& items, bool _isPath = false, QObject *parent = 0) : ComboBoxWithUrlsDelegate(items, _isPath, parent) {}

    virtual void update();

protected:
    virtual void updateUgeneSettings() = 0;
    virtual void updateValues(const QString& dataPathName, const QString& attributeName, const QString& defaultValue = "");
    virtual void updateDataPath(const QString& dataPathName, const QString& dirName, bool folders = false);

    virtual QString getDataPathName() = 0;
    virtual QString getAttributeName() = 0;
    virtual QString getDefaultValue() { return ""; }
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_CISTROME_DELEGATE_H_
