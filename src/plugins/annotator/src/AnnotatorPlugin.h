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

#ifndef _U2_ANNOTATION_COLLOCATOR_PLUGIN_H_
#define _U2_ANNOTATION_COLLOCATOR_PLUGIN_H_

#include <U2Core/PluginModel.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class XMLTestFactory;

class AnnotatorPlugin : public Plugin {
    Q_OBJECT
public:
    AnnotatorPlugin();

private:
    GObjectViewWindowContext* viewCtx;
};

class AnnotatorViewContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    AnnotatorViewContext(QObject* p, bool customFeaturesAvailable);
private:
    bool customFeaturesAvailable;

protected slots:
    void sl_showCollocationDialog();
    void sl_showCustomAutoAnnotationDialog();

protected:
    virtual void initViewContext(GObjectView* view);
};

class AnnotatorTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

} //namespace
#endif
