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

#ifndef _U2_UMUSCLE_PLUGIN_H_
#define _U2_UMUSCLE_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Core/AppContext.h>
#include <U2Gui/ObjectViewModel.h>

#include <QMenu>

namespace U2 {

class MSAEditor;
class MuscleMSAEditorContext;
class XMLTestFactory;

class MusclePlugin : public Plugin {
    Q_OBJECT
public:
    MusclePlugin();
    ~MusclePlugin();

public slots:
    void sl_runWithExtFileSpecify();

private:
    MuscleMSAEditorContext* ctx;
    
};


class MuscleMSAEditorContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    MuscleMSAEditorContext(QObject* p);

protected slots:
    void sl_align();
    void sl_alignSequencesToProfile();
    void sl_alignProfileToProfile();

protected:
    virtual void initViewContext(GObjectView* view);
    virtual void buildMenu(GObjectView* v, QMenu* m);
};

class MuscleAction : public GObjectViewAction {
    Q_OBJECT
public:
    MuscleAction(QObject* p, GObjectView* v, const QString& text, int order) 
		: GObjectViewAction(p, v, text, order) {}

    MSAEditor*  getMSAEditor() const;

private slots:
    void sl_updateState();
};

} //namespace

#endif
