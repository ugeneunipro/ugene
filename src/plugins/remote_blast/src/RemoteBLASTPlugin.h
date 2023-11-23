/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>
#include <U2Core/PluginModel.h>

#include <U2Gui/ObjectViewModel.h>

#include "DBRequestFactory.h"
#include "RemoteBLASTPluginTests.h"
#include "RemoteBLASTTask.h"
#include "RemoteBLASTWorker.h"
#include "SendSelectionDialog.h"

namespace U2 {

class Annotation;
class AnnotationGroupSelection;
class XMLTestFactory;
class RemoteBLASTViewContext;

class RemoteBLASTPlugin : public Plugin {
    Q_OBJECT
public:
    RemoteBLASTPlugin();

private:
    RemoteBLASTViewContext* ctx;
};

class RemoteBLASTViewContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    RemoteBLASTViewContext(QObject* p);

protected:
    void initViewContext(GObjectViewController* view) override;

private slots:
    void sl_showDialog();

private:
    // Check annotation selection and if some primer pairs group(s) if (are) selected
    // (or its (their) parent, which named "top_primers"), than we should blast these primer pairs
    static const QList<QPair<Annotation*, Annotation*>> getSelectedPrimerPairs(AnnotationGroupSelection* ags);
};

class RemoteBLASTPluginTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
