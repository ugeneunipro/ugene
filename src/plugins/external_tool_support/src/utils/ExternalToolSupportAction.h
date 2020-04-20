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

#ifndef _U2_EXTERNAL_TOOL_SUPPORT_ACTION_H
#define _U2_EXTERNAL_TOOL_SUPPORT_ACTION_H

#include <U2Core/ExternalToolRegistry.h>
#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class ExternalToolSupportAction : public GObjectViewAction {
    Q_OBJECT
public:
    ExternalToolSupportAction(QObject* p, GObjectView* v, const QString& _text, int order, const QStringList& _toolIds);
    ExternalToolSupportAction(const QString& text, QObject* p, const QStringList& _toolIds);

    const QStringList getToolIds() const {return toolIds;}

private slots:
    void sl_pathChanged();
    void sl_toolStateChanged(bool) { sl_pathChanged(); }

private:
    bool checkTools(bool connectSignals = false);
    void setState(bool isAnyToolConfigured);

    QStringList toolIds;
};

}//namespace
#endif // _U2_EXTERNAL_TOOL_SUPPORT_ACTION_H
