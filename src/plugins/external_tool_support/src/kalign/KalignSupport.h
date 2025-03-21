/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2View/MsaEditor.h>

namespace U2 {

class Kalign3Support : public ExternalTool {
    Q_OBJECT
public:
    Kalign3Support();

    GObjectViewWindowContext* getViewContext() const;

    static const QString ET_KALIGN_ID;
    static const QString KALIGN_TMP_DIR;

public slots:
    void sl_runWithExternalFile();

private:
    GObjectViewWindowContext* viewCtx = nullptr;
};

class Kalign3SupportContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    Kalign3SupportContext(QObject* p);

protected slots:
    void sl_align();

protected:
    void initViewContext(GObjectViewController* view) override;
};

}  // namespace U2
