/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#pragma once
#include <U2Core/ExternalToolRegistry.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

// Initializes mfold tool.
class MfoldSupport final : public ExternalTool {
    Q_OBJECT
    GObjectViewWindowContext* viewCtx = nullptr;

public:
    MfoldSupport();
    GObjectViewWindowContext* getViewContext() const;
    static const QString ET_MFOLD_ID;
};

// Responsible for starting dialog and task.
class MfoldContext final : public GObjectViewWindowContext {
    Q_OBJECT
private slots:
    void sl_showDialog();

public:
    explicit MfoldContext(QObject* p);

protected:
    void initViewContext(GObjectViewController*) override;
};
}  // namespace U2
