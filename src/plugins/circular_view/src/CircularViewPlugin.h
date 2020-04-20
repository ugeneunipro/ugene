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

#ifndef _U2_CIRCULAR_VIEW_PLUGIN_H_
#define _U2_CIRCULAR_VIEW_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>

#include <QMap>
#include <QAction>

#include <U2View/ADVSequenceWidget.h>

namespace U2 {

class CircularView;
struct CircularViewSettings;
class CircularViewSplitter;
class RestrctionMapWidget;

class CircularViewPlugin : public Plugin {
    Q_OBJECT
public:
    CircularViewPlugin();

private:
    GObjectViewWindowContext *viewCtx;
};

class CircularViewAction : public ADVSequenceWidgetAction {
    Q_OBJECT
public:
    CircularViewAction();

    CircularView *view;
    RestrctionMapWidget *rmapWidget;

public slots:
    void sl_circularStateChanged();
};

struct CircularViewSettings {
    enum LabelMode {
        Inside,
        Outside,
        Mixed,
        None
    };

    CircularViewSettings();

    bool        showTitle;
    bool        showLength;
    int         titleFontSize;
    QString     titleFont;
    bool        titleBold;

    bool        showRulerLine;
    bool        showRulerCoordinates;
    int         rulerFontSize;

    LabelMode   labelMode;
    int         labelFontSize;
};

class CircularViewContext: public GObjectViewWindowContext {
    friend class CircularViewSettingsWidgetFactory;
    Q_OBJECT
public:
    CircularViewContext(QObject *p);

    CircularViewSettings * getSettings(AnnotatedDNAView *view);

signals:
    void si_cvSplitterWasCreatedOrRemoved(CircularViewSplitter *, CircularViewSettings *);

protected slots:
    void sl_showCircular();
    void sl_sequenceWidgetAdded(ADVSequenceWidget *);
    void sl_sequenceWidgetRemoved(ADVSequenceWidget *w);
    void sl_toggleViews();
    void sl_toggleBySettings(CircularViewSettings *s);
    void sl_setSequenceOrigin();
    void sl_onDNAViewClosed(AnnotatedDNAView *v);

protected:
    virtual void initViewContext(GObjectView *view);
    void buildMenu(GObjectView *v, QMenu *m);
    CircularViewSplitter * getView(GObjectView *view, bool create);
    void removeCircularView(GObjectView *view);
    void toggleViews(AnnotatedDNAView *view);

private:
    QMap<AnnotatedDNAView *, CircularViewSettings *> viewSettings;
};

} //namespace

#endif
