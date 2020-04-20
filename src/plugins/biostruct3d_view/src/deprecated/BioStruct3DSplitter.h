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

#include <U2Core/Task.h>
#include <U2View/ADVSplitWidget.h>

#include <QAction>
#include <QComboBox>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>

namespace U2 {

class BioStruct3DObject;
class BioStruct3DGLWidget;
class AnnotatedDNAView;
class GObject;
class GObjectView;
class SplitterHeaderWidget;
class Document;
class GLFrameManager;
class OrderedToolbar;
class WidgetWithLocalToolbar;

/*!
* @class BioStruct3DSplitter BioStruct3DSplitter.h
* @brief Class for multiple BioStruct3DWidget layout.
*
* This widget provides insertion of BioStruct3DGLWidgets into active UGENE view. There
* could be one or more glWidgets controlled by the splitter. One can add new glWidgets using
* the splitter.
*/
class BioStruct3DSplitter : public ADVSplitWidget {

    Q_OBJECT

public:
    /*!
    * Constructor.
    * @param closeAction QAction provided for correct widget closing
    * @param view DnaView context for connecting structure 3D representation and sequence view
    */
    BioStruct3DSplitter(QAction* closeAction, AnnotatedDNAView* view);
    /*!
    * Destructor.
    */
    ~BioStruct3DSplitter();
    /*!
    * @return True, if GObject has type BioStruct3D and could be added to splitter.
    */
    virtual bool acceptsGObject(GObject* obj);
    /*!
    * Creates BioStruct3DWidget for object visualization and adds it to the splitter.
    */
    BioStruct3DGLWidget* addBioStruct3DGLWidget(BioStruct3DObject* obj);
    /*!
    * @return First found BioStruct3DObject with corresponding name, NULL if nothing is found.
    */
    BioStruct3DObject* findBioStruct3DObjByName(const QString& objName);
    virtual void saveState(QVariantMap& m);
    virtual void updateState(const QVariantMap& m);
    /*!
    * @return List of splitter children widgets.
    */
    QList<BioStruct3DGLWidget*> getChildWidgets() const;
    /*!
    * @return Current active view.
    */
    const QList<QAction* > getSettingsMenuActions() const;
    /*!
    * @return ADVDNAView splitter, parent widget for BioStruct3DSplitter.
    */
    int getNumVisibleWidgets();
    /*!
    * @return If number of visible children is null, sets splitter view collapsed, else restores it.
    */
    void adaptSize(int numVisibleWidgets);
    /*!
    * @return GLFrameManager for splitter child widgets.
    */
    GLFrameManager* getGLFrameManager();
    /*!
    *Removes widgets, updates context
    */
    void removeBioStruct3DGLWidget(BioStruct3DGLWidget* widget);
    /*!
    * This is used to close 3D split widget from toolbar
    */
    QAction* getCloseSplitterAction() { return closeAction; }
    /*!
    * QWidget virtual function, returns preferred widget size.
    */
    virtual QSize sizeHint () const {return QSize(0, 400);}
    /*!
    * Adds object and its new view.
    */
    void addObject(BioStruct3DObject* obj);
    /*!
    * AddModelTask helper function.
    */
    void addModelFromObject(BioStruct3DObject* obj);
    /*!
    * Removes object and its views.
    */
    bool removeObject(BioStruct3DObject* obj);
    /*!
    * Adds action button to the toolbar on the left
    */
    void addActionToLocalToolBar(QAction* action);

signals:
    void si_bioStruct3DGLWidgetAdded(BioStruct3DGLWidget* widget);
    void si_bioStruct3DGLWidgetRemoved(BioStruct3DGLWidget* widget);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject* o, QEvent* e);

private:
    QSplitter* getParentSplitter();
    QMultiMap<BioStruct3DObject*, BioStruct3DGLWidget*> biostrucViewMap;
    QScopedPointer<GLFrameManager> glFrameManager;
    QSplitter*  splitter;
    QSplitter*  parentSplitter;
    QAction*    closeAction;
    QLayout*    layout;
    QList<QAction*>     toggleActions;
    int                 splitterHeight;
    bool                isViewCollapsed;
    WidgetWithLocalToolbar* widgetWithToolbar;

    SplitterHeaderWidget* header;
};


struct DBLink {
    DBLink(const QString& _name, QString _url) : name(_name), url(_url) { }
    QString name;
    QString url;
};

class DBLinksFile {
    QList<DBLink> links;
public:
    bool load();
    QList<DBLink> getLinks() { return links; }
};


// Implemented as Task for unloaded documents support
class AddModelToSplitterTask : public Task {
    Q_OBJECT
public:
    AddModelToSplitterTask(GObject* o, BioStruct3DSplitter* s);
    virtual void prepare();
    virtual void run();
    virtual ReportResult report();
private:
    Document* doc;
    GObject* obj;
    BioStruct3DObject* bObj;
    BioStruct3DSplitter* splitter;
};


class SplitterHeaderWidget : public QWidget {
    Q_OBJECT

public:
    SplitterHeaderWidget(BioStruct3DSplitter* splitter);

private:
    BioStruct3DSplitter* splitter;
    QAction* widgetStateMenuAction;
    QAction* addModelAction;
    QAction* webMenuAction;
    QAction* displayMenuAction;
    QAction* restoreDefaultsAction;
    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* syncLockAction;
    QComboBox* activeWidgetBox;
    QMap<QAction*,QString> webActionMap;
    QList<QAction*> toggleActions;

    OrderedToolbar* toolbar;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:

    void sl_bioStruct3DGLWidgetAdded(BioStruct3DGLWidget* glWidget);
    void sl_bioStruct3DGLWidgetRemoved(BioStruct3DGLWidget* glWidget);
    void sl_toggleBioStruct3DWidget(bool visible);
    void sl_toggleSyncLock(bool on);
    void sl_addModel();
    void sl_showStateMenu();
    void sl_showDisplayMenu();
//    void sl_showSettingsMenu();    // TODO: add settings functionality
    void sl_showWebMenu();
    void sl_zoomIn();
    void sl_zoomOut();
    void sl_restoreDefaults();
    void sl_openBioStructUrl();


private:
    BioStruct3DGLWidget* getActiveWidget();
    void addToolbarAction(QAction *action);
    void updateToolbar();
    void updateActiveWidgetBox();
    void enableToolbar();
    void registerWebUrls();
    void setActiveView(BioStruct3DGLWidget* glWidget);

};

} //namespace
